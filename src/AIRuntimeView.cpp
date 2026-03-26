#include <unistd.h>
#include <stdio.h>
#include "AIRuntimeView.h"
#include "ui_AIRuntimeView.h"
#include <QNetworkRequest>
#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QDirIterator>
#include <QHeaderView>
#include "ConfigManager.h"
#include "LiveMonitoringWidget.h"
#include "InferenceResourceManager.h"

AIRuntimeView::AIRuntimeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIRuntimeView),
    m_netManager(new QNetworkAccessManager(this)),
    m_metadataClient(new MetadataClient(this)),
    m_liveWidget(nullptr)
{
    m_metadataClient->setBaseUrl(ConfigManager::instance().metadataUrl());
    ui->setupUi(this);

    // Connect metadata client signals
    connect(m_metadataClient, &MetadataClient::devicesReceived, this, [this](const QJsonArray &devices) {
        m_discoveredDevices = devices;
        ui->comboEdgeXDevice->clear();
        for (const auto &dev : devices) {
            ui->comboEdgeXDevice->addItem(dev.toObject()["name"].toString());
        }
        // Restore saved device from QSettings into profile field
        QSettings s;
        QString savedDevice = s.value("edgex/last_device").toString();
        if (!savedDevice.isEmpty()) {
            ui->comboEdgeXDevice->setCurrentText(savedDevice);
        }
    });

    connect(m_metadataClient, &MetadataClient::deviceProfilesReceived, this, [this](const QJsonArray &profiles) {
        m_discoveredProfiles = profiles;
        ui->comboEdgeXProfile->clear();
        for (const auto &prof : profiles) {
            ui->comboEdgeXProfile->addItem(prof.toObject()["name"].toString());
        }
    });

    // Initial setup
    setupLivePanel();
    
    m_sdkPath = qgetenv("AXELERA_SDK_PATH");
    if (m_sdkPath.isEmpty()) m_sdkPath = "/data/voyager-sdk";
    
    // Set temporary testing defaults for Aetina platform
    ui->editModelPath->setText(m_sdkPath + "/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml");
    
    // Add a testing source for the Brio 100 on Aetina
    m_sources.append({"Brio 100 (Aetina)", "USB Camera", "/dev/video20/yuyv"});
    
    // Create a testing profile
    InferenceProfile testProfile;
    testProfile.name = "Aetina Testing";
    testProfile.modelZooName = "voyager-person-detection";
    testProfile.modelYamlPath = m_sdkPath + "/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml";
    testProfile.cameraSourceName = "Brio 100 (Aetina)";
    testProfile.confidence = 70.0;
    testProfile.pipelineType = "gst";
    testProfile.aipuCores = "0,1,2,3";
    m_profiles.append(testProfile);
    
    scanModelZoo();
    populateCameras();
    loadSettings();
    fetchEdgeXMetaData();

    // Ensure the testing profile is visible in combo boxes
    for (const auto &p : m_profiles) {
        if (ui->comboProfiles->findText(p.name) == -1) {
            ui->comboProfiles->addItem(p.name);
            ui->comboActiveProfile->addItem(p.name);
        }
    }

    // Control Buttons
    connect(ui->btnLaunch, &QPushButton::clicked, this, &AIRuntimeView::startInference);
    connect(ui->btnStop, &QPushButton::clicked, this, &AIRuntimeView::stopInference);
    
    // Source Management
    connect(ui->btnAddSource, &QPushButton::clicked, this, &AIRuntimeView::addSource);
    connect(ui->btnEditSource, &QPushButton::clicked, this, &AIRuntimeView::editSource);
    connect(ui->btnSaveSource, &QPushButton::clicked, this, &AIRuntimeView::onSaveSource);
    connect(ui->btnDeleteSource, &QPushButton::clicked, this, &AIRuntimeView::deleteSource);
    connect(ui->btnBrowseFile, &QPushButton::clicked, this, &AIRuntimeView::browseVideoFile);
    connect(ui->btnBrowseOutput, &QPushButton::clicked, this, &AIRuntimeView::onBrowseOutput);
    
    // Model Settings
    connect(ui->btnBrowseModelYaml, &QPushButton::clicked, this, &AIRuntimeView::onBrowseModelYaml);
    connect(ui->btnBrowseClassMap, &QPushButton::clicked, this, &AIRuntimeView::onBrowseClassMap);
    connect(ui->btnBrowseEmbedding, &QPushButton::clicked, this, &AIRuntimeView::onBrowseEmbedding);
    connect(ui->comboModelZoo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AIRuntimeView::onModelZooChanged);
    
    // Profile Management
    connect(ui->btnNewProfile, &QPushButton::clicked, this, &AIRuntimeView::onNewProfile);
    connect(ui->btnSaveProfile, &QPushButton::clicked, this, &AIRuntimeView::onSaveProfile);
    connect(ui->btnDeleteProfile, &QPushButton::clicked, this, &AIRuntimeView::onDeleteProfile);
    connect(ui->comboProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AIRuntimeView::onProfileSelectionChanged);
    connect(ui->comboActiveProfile, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AIRuntimeView::onProfileSelectionChanged);
    
    // EdgeX Selection Persistence (Profile-based now)
    connect(ui->comboEdgeXDevice, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index >= 0) onEdgeXDeviceChanged(ui->comboEdgeXDevice->itemText(index));
    });
    connect(ui->comboEdgeXProfile, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index >= 0) onEdgeXProfileChanged(ui->comboEdgeXProfile->itemText(index));
    });

    // Misc
    connect(ui->comboSourceType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AIRuntimeView::onSourceTypeChanged);
    connect(ui->tableSources, &QTableWidget::itemSelectionChanged, this, &AIRuntimeView::onSourceSelectionChanged);

    m_aipuTimer = new QTimer(this);
    connect(m_aipuTimer, &QTimer::timeout, this, &AIRuntimeView::updateAipuStatus);
    m_aipuTimer->start(2000);

    ui->tableSources->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->btnStop->setEnabled(false);
}

AIRuntimeView::~AIRuntimeView()
{
    stopInference();
    
    saveSettings();
    delete ui;
}

void AIRuntimeView::setupLivePanel()
{
    m_liveWidget = new LiveMonitoringWidget(this);
    ui->videoContainer->addWidget(m_liveWidget);
}

void AIRuntimeView::startInference()
{
    QString profileName = ui->comboActiveProfile->currentText();
    if (m_workers.contains(profileName)) {
        m_activeProfile = profileName;
        ui->btnLaunch->setEnabled(false);
        ui->btnStop->setEnabled(true);
        return;
    }

    // Allocate Resources
    PortSet ports = InferenceResourceManager::instance().allocatePorts();
    if (!ports.isValid()) {
        QMessageBox::warning(this, "Error", "No available ports for new inference stream.");
        return;
    }

    QString cores = ui->editAipuCores->text();
    if (!InferenceResourceManager::instance().checkAndAllocateCores(cores)) {
        InferenceResourceManager::instance().releasePorts(ports);
        QMessageBox::warning(this, "Error", QString("AIPU Cores %1 are already in use or invalid.").arg(cores));
        return;
    }

    // Camera Source
    QString camName = ui->comboProfileCamera->currentText();
    QString sourceValue = "usb:20";
    for (const auto &src : m_sources) {
        if (src.name == camName) { sourceValue = src.value; break; }
    }

    // Build Config
    QJsonObject configJson;
    configJson["profileName"]         = profileName;
    configJson["modelPath"]           = ui->editModelPath->text();
    configJson["confidenceThreshold"] = ui->spinConfidence->value() / 100.0;
    configJson["aipuCores"]           = cores;
    configJson["cameraSource"]        = sourceValue;
    configJson["videoPort"]           = ports.video;
    configJson["metaPort"]            = ports.meta;
    configJson["cmdPort"]             = ports.command;

    // EdgeX Settings
    int profileIdx = ui->comboActiveProfile->currentIndex();
    if (profileIdx >= 0 && profileIdx < m_profiles.size()) {
        const auto &p = m_profiles[profileIdx];
        configJson["edgexDeviceName"]  = p.edgexDeviceName;
        configJson["edgexProfileName"] = p.edgexProfileName;
        configJson["edgexBn"]          = p.senmlBn;
        configJson["pipelineType"]     = p.pipelineType.isEmpty() ? ui->comboPipelineType->currentText() : p.pipelineType;
    }

    configJson["classMapPath"]  = ui->editClassMapPath->text();
    configJson["embeddingPath"] = ui->editEmbeddingPath->text();

    // Create Worker
    AIInferenceWorker* worker = new AIInferenceWorker(configJson, this);
    m_workers[profileName] = worker;
    m_activeProfile = profileName;

    connect(worker, &AIInferenceWorker::frameReady, this, &AIRuntimeView::onFrameReceived);
    connect(worker, &AIInferenceWorker::metadataReady, this, &AIRuntimeView::onMetadataReceived);
    connect(worker, &AIInferenceWorker::statusChanged, this, &AIRuntimeView::onStatusChanged);
    connect(worker, &AIInferenceWorker::errorOccurred, this, &AIRuntimeView::onErrorOccurred);
    connect(worker, &AIInferenceWorker::logMessage, this, &AIRuntimeView::onWorkerLog);

    worker->start();

    ui->btnLaunch->setEnabled(false);
    ui->btnStop->setEnabled(true);
    if (m_liveWidget) m_liveWidget->play("inference");
}

void AIRuntimeView::stopInference()
{
    QString profileName = ui->comboActiveProfile->currentText();
    if (!m_workers.contains(profileName)) return;

    AIInferenceWorker* worker = m_workers.take(profileName);
    
    // Release EXACT resources used by this worker
    InferenceResourceManager::instance().releasePorts(worker->ports());
    InferenceResourceManager::instance().releaseCores(worker->aipuCores());

    worker->stop();

    worker->deleteLater();
    
    if (m_activeProfile == profileName) {
        m_activeProfile = "";
        if (m_liveWidget) m_liveWidget->stop();
    }

    ui->btnLaunch->setEnabled(true);
    ui->btnStop->setEnabled(false);
}

void AIRuntimeView::onNavItemChanged(int index)
{
    ui->contentStack->setCurrentIndex(index);
    if (index == 1 || index == 2) {
        populateCameras();
        onProfileSelectionChanged(ui->comboActiveProfile->currentIndex());
    }
}

void AIRuntimeView::populateCameras()
{
    ui->comboProfileCamera->clear();
    QDir devDir("/dev");
    QStringList videoDevices = devDir.entryList(QStringList() << "video*", QDir::System);
    
    for (const QString &dev : videoDevices) {
        // Convert /dev/videoN -> usb:N for Axelera SDK compatibility
        QString num = dev.mid(5);  // strip "video" prefix
        QString usbSrc = "usb:" + num;
        bool found = false;
        for (const auto &src : m_sources) {
            if (src.value == usbSrc) { found = true; break; }
        }
        if (!found) {
            QString name = "Camera " + num + " (" + usbSrc + ")";
            m_sources.append({name, "USB Camera", usbSrc});
        }
    }
    updateSourcesUI();
}

void AIRuntimeView::updateSourcesUI()
{
    ui->tableSources->setRowCount(0);
    ui->comboProfileCamera->clear();
    for (int i = 0; i < m_sources.size(); ++i) {
        const auto &src = m_sources[i];
        ui->tableSources->insertRow(i);
        ui->tableSources->setItem(i, 0, new QTableWidgetItem(src.name));
        ui->tableSources->setItem(i, 1, new QTableWidgetItem(src.type));
        ui->tableSources->setItem(i, 2, new QTableWidgetItem(src.value));
        ui->comboProfileCamera->addItem(src.name);
    }
}

void AIRuntimeView::onSaveSource()
{
    int row = ui->tableSources->currentRow();
    if (row >= 0 && row < m_sources.size()) {
        auto &src = m_sources[row];
        src.name = ui->editSourceName->text();
        src.type = ui->comboSourceType->currentText();
        
        if (src.type == "USB Camera") {
            src.value = "usb:" + QString::number(ui->spinUSBIndex->value());
        } else if (src.type == "RTSP Stream") {
            src.value = ui->editRTSPUrl->text();
        } else if (src.type == "Video File") {
            src.value = ui->editFilePath->text();
        }
        
        updateSourcesUI();
        // Keep the same row selected after update
        ui->tableSources->setCurrentCell(row, 0);
        
        saveSettings(); // Persist changes
        ui->groupSourceEdit->setEnabled(false);
    } else {
        QMessageBox::warning(this, "Save Error", "Please select a source to save changes.");
    }
}

void AIRuntimeView::scanModelZoo()
{
    ui->comboModelZoo->clear();
    m_modelZooPaths.clear();
    QDir dir(m_sdkPath + "/ax_models/zoo");
    if (!dir.exists()) return;

    QDirIterator it(dir.path(), QStringList() << "*.yaml", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QString name = QFileInfo(path).baseName();
        ui->comboModelZoo->addItem(name);
        m_modelZooPaths[name] = path;
    }
}

void AIRuntimeView::onModelZooChanged(int index)
{
    QString name = ui->comboModelZoo->itemText(index);
    if (m_modelZooPaths.contains(name)) {
        QString path = m_modelZooPaths[name];
        ui->editModelPath->setText(path);
        loadModelMetadata(path);
    }
}

void AIRuntimeView::loadModelMetadata(const QString& p)
{
    QFile file(p);
    if (file.open(QIODevice::ReadOnly)) {
        QString content = QString::fromUtf8(file.readAll());
        int descIdx = content.indexOf("description:");
        if (descIdx != -1) {
            QString desc = content.mid(descIdx + 12, content.indexOf("\n", descIdx) - (descIdx + 12)).trimmed();
            ui->lblModelInfo->setText("<b>Description:</b> " + desc);
        }
    }
}

void AIRuntimeView::onBrowseModelYaml()
{
    QFileDialog diag(this, "Select Model YAML", m_sdkPath, "YAML (*.yaml)");
    diag.setOption(QFileDialog::DontUseNativeDialog);
    diag.setLabelText(QFileDialog::Accept, "Select");
    if (diag.exec()) {
        QString file = diag.selectedFiles().first();
        ui->editModelPath->setText(file);
        loadModelMetadata(file);
    }
}

void AIRuntimeView::onProfileSelectionChanged(int index)
{
    if (index < 0 || index >= m_profiles.size()) return;
    const auto &p = m_profiles[index];
    
    m_activeProfile = p.name;
    bool running = m_workers.contains(m_activeProfile);
    
    ui->btnLaunch->setEnabled(!running);
    ui->btnStop->setEnabled(running);
    
    if (running && m_liveWidget) {
        m_liveWidget->play("inference");
    }

    ui->editProfileName->setText(p.name);
    ui->editModelPath->setText(p.modelYamlPath);
    ui->comboProfileCamera->setCurrentText(p.cameraSourceName);
    ui->spinConfidence->setValue(p.confidence);
    ui->editAipuCores->setText(p.aipuCores);
    ui->editClassMapPath->setText(p.classMapPath);
    ui->editEmbeddingPath->setText(p.embeddingPath);
    ui->comboModelZoo->setCurrentText(p.modelZooName);
    ui->comboPipelineType->setCurrentText(p.pipelineType);
    ui->comboDisplayMode->setCurrentText(p.displayMode);
    ui->comboProfileCamera->setCurrentText(p.cameraSourceName);
    
    m_requestedDeviceName = p.edgexDeviceName;
    m_requestedProfileName = p.edgexProfileName;

    if (!m_requestedDeviceName.isEmpty()) {
        ui->comboEdgeXDevice->setCurrentText(m_requestedDeviceName);
    }
    if (!m_requestedProfileName.isEmpty()) {
        ui->comboEdgeXProfile->setCurrentText(m_requestedProfileName);
    }
    ui->editSenmlBn->setText(p.senmlBn);
}

void AIRuntimeView::onSaveProfile()
{
    int idx = ui->comboProfiles->currentIndex();
    if (idx < 0 || idx >= m_profiles.size()) return;
    
    auto &p = m_profiles[idx];
    QString oldName = p.name;
    p.name = ui->editProfileName->text().trimmed();
    
    if (p.name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Profile name cannot be empty.");
        p.name = oldName;
        ui->editProfileName->setText(oldName);
        return;
    }

    p.modelYamlPath = ui->editModelPath->text();
    p.cameraSourceName = ui->comboProfileCamera->currentText();
    p.confidence = ui->spinConfidence->value();
    p.aipuCores = ui->editAipuCores->text();
    p.classMapPath = ui->editClassMapPath->text();
    p.embeddingPath = ui->editEmbeddingPath->text();
    p.modelZooName = ui->comboModelZoo->currentText();
    p.pipelineType = ui->comboPipelineType->currentText();
    p.displayMode  = ui->comboDisplayMode->currentText();
    
    p.edgexDeviceName = ui->comboEdgeXDevice->currentText();
    p.edgexProfileName = ui->comboEdgeXProfile->currentText();
    p.senmlBn = ui->editSenmlBn->text().trimmed();

    if (p.cameraSourceName.isEmpty() || p.edgexDeviceName.isEmpty() || 
        p.edgexProfileName.isEmpty() || p.senmlBn.isEmpty()) {
        QMessageBox::warning(this, "Error", "Camera Source, EdgeX Target Device/Profile, and SenML Base Name (bn) are all mandatory.");
        return;
    }
    
    // Update combo boxes
    int currentProfilesIdx = ui->comboProfiles->currentIndex();
    int currentActiveIdx = ui->comboActiveProfile->currentIndex();
    
    ui->comboProfiles->setItemText(idx, p.name);
    ui->comboActiveProfile->setItemText(idx, p.name);
    
    // Force refresh displays if names changed
    if (idx == currentProfilesIdx) ui->editProfileName->setText(p.name);
    
    saveSettings();
    QMessageBox::information(this, "Saved", "Profile '" + p.name + "' updated successfully.");
}

void AIRuntimeView::loadSettings()
{
    QSettings s;
    
    // Load Sources
    int srcCount = s.beginReadArray("sources");
    if (srcCount > 0) m_sources.clear(); // Only clear if we have saved sources
    for (int i = 0; i < srcCount; ++i) {
        s.setArrayIndex(i);
        m_sources.append({
            s.value("name").toString(),
            s.value("type").toString(),
            s.value("value").toString()
        });
    }
    s.endArray();
    updateSourcesUI();

    // Load Profiles
    int count = s.beginReadArray("profiles");
    m_profiles.clear();
    ui->comboProfiles->clear();
    ui->comboActiveProfile->clear();
    for (int i=0; i<count; ++i) {
        s.setArrayIndex(i);
        InferenceProfile p;
        p.name = s.value("name").toString();
        p.modelYamlPath = s.value("model").toString();
        p.cameraSourceName = s.value("camera").toString();
        p.confidence = s.value("conf").toDouble();
        p.aipuCores = s.value("cores").toString();
        p.classMapPath = s.value("class_map").toString();
        p.embeddingPath = s.value("embedding").toString();
        p.modelZooName = s.value("zoo_name").toString();
        p.pipelineType = s.value("pipeline", "gst").toString();
        p.displayMode  = s.value("display", "none").toString();
        
        p.edgexDeviceName = s.value("edgex_device").toString();
        p.edgexProfileName = s.value("edgex_profile").toString();
        p.senmlBn = s.value("senml_bn").toString();
        m_profiles.append(p);
        ui->comboProfiles->addItem(p.name);
        ui->comboActiveProfile->addItem(p.name);
    }
    s.endArray();
    
    // ENSURE CLEAN STATE: Kill any orphaned backends from previous sessions
    QProcess::execute("pkill", QStringList() << "-9" << "-f" << "axelera_server.py");
    
    fetchEdgeXMetaData();

    // Load global EdgeX defaults
    m_requestedDeviceName = s.value("edgex/last_device").toString();

    // Restore last profile index
    int lastIdx = s.value("last_profile_index", 0).toInt();
    if (!m_profiles.isEmpty()) {
        if (lastIdx < 0 || lastIdx >= m_profiles.size()) lastIdx = 0;
        ui->comboProfiles->setCurrentIndex(lastIdx);
        ui->comboActiveProfile->setCurrentIndex(lastIdx);
        onProfileSelectionChanged(lastIdx);
    }
}

void AIRuntimeView::saveSettings()
{
    QSettings s;
    
    // Save Sources
    s.beginWriteArray("sources");
    for (int i = 0; i < m_sources.size(); ++i) {
        s.setArrayIndex(i);
        s.setValue("name", m_sources[i].name);
        s.setValue("type", m_sources[i].type);
        s.setValue("value", m_sources[i].value);
    }
    s.endArray();

    // Save Profiles
    s.beginWriteArray("profiles");
    for (int i=0; i<m_profiles.size(); ++i) {
        s.setArrayIndex(i);
        const auto &p = m_profiles[i];
        s.setValue("name", p.name);
        s.setValue("model", p.modelYamlPath);
        s.setValue("camera", p.cameraSourceName);
        s.setValue("senml_bn", p.senmlBn);
        s.setValue("conf", p.confidence);
        s.setValue("cores", p.aipuCores);
        s.setValue("class_map", p.classMapPath);
        s.setValue("embedding", p.embeddingPath);
        s.setValue("zoo_name", p.modelZooName);
        s.setValue("pipeline", p.pipelineType);
        s.setValue("display", p.displayMode);
        
        s.setValue("edgex_device", p.edgexDeviceName);
        s.setValue("edgex_profile", p.edgexProfileName);
    }
    s.endArray();
    s.setValue("last_profile_index", ui->comboProfiles->currentIndex());
    s.sync();
}

void AIRuntimeView::fetchEdgeXMetaData()
{
    if (m_metadataClient) { m_metadataClient->fetchDevices(); m_metadataClient->fetchDeviceProfiles(); }
    QTimer::singleShot(2000, this, [this]() {
        if (m_discoveredDevices.isEmpty()) {
            QJsonArray mockDevs; QJsonObject od; od["name"] = "Brio_100"; mockDevs.append(od);
            emit m_metadataClient->devicesReceived(mockDevs);
        }
        if (m_discoveredProfiles.isEmpty()) {
            QJsonArray mockProfs; QJsonObject op; op["name"] = "Camera_Profile"; mockProfs.append(op);
            emit m_metadataClient->deviceProfilesReceived(mockProfs);
        }
    });
}

// Minimal stubs for remaining slots
void AIRuntimeView::addSource() { 
    m_sources.append({"New", "USB Camera", "usb:0"}); 
    updateSourcesUI(); 
    // Automatically select the new source
    ui->tableSources->setCurrentCell(m_sources.size() - 1, 0);
    ui->groupSourceEdit->setEnabled(true);
}
void AIRuntimeView::editSource() { ui->groupSourceEdit->setEnabled(true); }
void AIRuntimeView::onSourceSelectionChanged() { 
    int r = ui->tableSources->currentRow();
    if (r>=0 && r<m_sources.size()) {
        const auto &s = m_sources[r];
        ui->editSourceName->setText(s.name);
        ui->comboSourceType->setCurrentText(s.type);
        
        // Populate configuration widgets based on type and value
        if (s.type == "USB Camera") {
            int idx = 0;
            if (s.value.startsWith("usb:")) idx = s.value.mid(4).toInt();
            else if (s.value.contains("/video")) idx = s.value.mid(s.value.indexOf("video") + 5).toInt();
            ui->spinUSBIndex->setValue(idx);
        } else if (s.type == "RTSP Stream") {
            ui->editRTSPUrl->setText(s.value);
        } else if (s.type == "Video File") {
            ui->editFilePath->setText(s.value);
        }
        
        ui->groupSourceEdit->setEnabled(true);
    }
}
void AIRuntimeView::onFrameReceived(const QImage& frame)
{
    AIInferenceWorker* worker = qobject_cast<AIInferenceWorker*>(sender());
    if (!worker) return;

    QString profile = worker->profileName();
    if (profile == m_activeProfile) {
        if (m_liveWidget) {
            m_liveWidget->updateNativeFrame(0, frame, m_latestDetectionsMap[profile]);
        }
    }
}

void AIRuntimeView::onMetadataReceived(const QJsonObject& meta)
{
    AIInferenceWorker* worker = qobject_cast<AIInferenceWorker*>(sender());
    if (!worker) return;

    QString profile = worker->profileName();
    QJsonArray detections = meta["detections"].toArray();
    m_latestDetectionsMap[profile] = detections;

    if (profile == m_activeProfile) {
        // Option to display metadata text if needed
    }
}

void AIRuntimeView::onStatusChanged(const QString &status)
{
    AIInferenceWorker* worker = qobject_cast<AIInferenceWorker*>(sender());
    if (worker && worker->profileName() == m_activeProfile) {
        ui->lblAipuStatus->setText(QString("Status: %1").arg(status));
    }
}

void AIRuntimeView::onWorkerLog(const QString &msg) { qDebug() << "[InferenceWorker]" << msg; }
void AIRuntimeView::onErrorOccurred(const QString &e) { QMessageBox::critical(this, "Error", e); }

void AIRuntimeView::onNewProfile() { 
    InferenceProfile p; 
    p.name = "New Profile " + QString::number(m_profiles.size() + 1); 
    m_profiles.append(p); 
    ui->comboProfiles->addItem(p.name); 
    ui->comboActiveProfile->addItem(p.name); 
    ui->comboProfiles->setCurrentIndex(m_profiles.size()-1); 
}

void AIRuntimeView::onDeleteProfile() { 
    int i = ui->comboProfiles->currentIndex(); 
    if (i>=0 && i < m_profiles.size()) { 
        m_profiles.removeAt(i); 
        ui->comboProfiles->removeItem(i); 
        ui->comboActiveProfile->removeItem(i); 
    }
}
void AIRuntimeView::onBrowseOutput() { QString d = QFileDialog::getExistingDirectory(this); if (!d.isEmpty()) ui->editSaveOutputPath->setText(d); }
void AIRuntimeView::onBrowseClassMap() { QFileDialog diag(this, "Select Class Map", m_sdkPath); diag.setOption(QFileDialog::DontUseNativeDialog); diag.setLabelText(QFileDialog::Accept, "Select"); if (diag.exec()) ui->editClassMapPath->setText(diag.selectedFiles().first()); }
void AIRuntimeView::onBrowseEmbedding() { QFileDialog diag(this, "Select Embedding", m_sdkPath); diag.setOption(QFileDialog::DontUseNativeDialog); diag.setLabelText(QFileDialog::Accept, "Select"); if (diag.exec()) ui->editEmbeddingPath->setText(diag.selectedFiles().first()); }
void AIRuntimeView::updateAipuStatus() {}
void AIRuntimeView::onSourceDataChanged() {}
void AIRuntimeView::browseVideoFile() { QString f = QFileDialog::getOpenFileName(this, "Select Video File"); if (!f.isEmpty()) ui->editFilePath->setText(f); }  
void AIRuntimeView::onSourceTypeChanged(int index) { ui->stackSourceConfig->setCurrentIndex(index); }
void AIRuntimeView::deleteSource() {
    int r = ui->tableSources->currentRow();
    if (r>=0 && r<m_sources.size()) {
        m_sources.removeAt(r);
        updateSourcesUI();
    }
}
void AIRuntimeView::onEdgeXDeviceChanged(const QString &text)
{
    QString name = text.trimmed();
    m_requestedDeviceName = name;
    
    int profIdx = ui->comboProfiles->currentIndex();
    if (profIdx >= 0 && profIdx < m_profiles.size()) {
        m_profiles[profIdx].edgexDeviceName = name;
    }
}

void AIRuntimeView::onEdgeXProfileChanged(const QString &text)
{
    QString name = text.trimmed();
    m_requestedProfileName = name;
    
    int profIdx = ui->comboProfiles->currentIndex();
    if (profIdx >= 0 && profIdx < m_profiles.size()) {
        m_profiles[profIdx].edgexProfileName = name;
    }
}


