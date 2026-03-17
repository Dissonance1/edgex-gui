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

AIRuntimeView::AIRuntimeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIRuntimeView),
    m_netManager(new QNetworkAccessManager(this)),
    m_metadataClient(new MetadataClient(this)),
    m_liveWidget(nullptr),
    m_inferenceWorker(nullptr)
{
    m_metadataClient->setBaseUrl(ConfigManager::instance().metadataUrl());
    ui->setupUi(this);

    // Connect metadata client signals
    connect(m_metadataClient, &MetadataClient::devicesReceived, this, [this](const QJsonArray &devices) {
        m_discoveredDevices = devices;
        // Restore saved device from QSettings into profile field
        QSettings s;
        QString savedDevice = s.value("edgex/last_device").toString();
        if (!savedDevice.isEmpty()) {
            ui->editEdgeXDeviceName->setText(savedDevice);
        }
    });

    connect(m_metadataClient, &MetadataClient::deviceProfilesReceived, this, [this](const QJsonArray &profiles) {
        m_discoveredProfiles = profiles;
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
    connect(ui->editEdgeXDeviceName, &QLineEdit::textChanged, this, &AIRuntimeView::onEdgeXDeviceChanged);

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
    int profileIdx = ui->comboActiveProfile->currentIndex();
    fprintf(stderr, "AIRuntimeView: Starting inference. Profile Index: %d\n", profileIdx);
    
    fflush(stderr);

    // Camera Source — prefer profile, then selected camera, then first source
    QString sourceValue = "usb:20";  // safe fallback
    if (profileIdx >= 0 && profileIdx < m_profiles.size()) {
        QString camName = m_profiles[profileIdx].cameraSourceName;
        for (const auto &src : m_sources) {
            if (src.name == camName) { sourceValue = src.value; break; }
        }
    } else {
        // Use currently selected camera from comboProfileCamera
        QString camName = ui->comboProfileCamera->currentText();
        for (const auto &src : m_sources) {
            if (src.name == camName) { sourceValue = src.value; break; }
        }
        // Last resort: first source in table
        if (sourceValue == "usb:20" && !m_sources.isEmpty()) {
            sourceValue = m_sources[0].value;
        }
    }
    // Clean up old worker if present
    stopInference();

    // Build full config from profile UI
    QJsonObject configJson;
    configJson["modelPath"]           = ui->editModelPath->text();
    configJson["confidenceThreshold"] = ui->spinConfidence->value() / 100.0;
    // Send AIPU cores — backend accepts "0,1,2,3" or "4" (count)
    configJson["aipuCores"]           = ui->editAipuCores->text();
    configJson["cameraSource"]        = sourceValue;

    // EdgeX Device Name (bn) is now stored in the inference profile
    if (profileIdx >= 0 && profileIdx < m_profiles.size()) {
        configJson["edgexDeviceName"]  = m_profiles[profileIdx].edgexDeviceName;
        configJson["edgexProfileName"] = m_profiles[profileIdx].edgexProfileName;
    } else {
        configJson["edgexDeviceName"]  = "";
        configJson["edgexProfileName"] = "";
    }

    configJson["classMapPath"]        = ui->editClassMapPath->text();
    configJson["embeddingPath"]       = ui->editEmbeddingPath->text();

    m_inferenceWorker = new AIInferenceWorker(configJson, this);
    
    connect(m_inferenceWorker, &AIInferenceWorker::frameReady, this, &AIRuntimeView::onFrameReceived);
    connect(m_inferenceWorker, &AIInferenceWorker::errorOccurred, this, &AIRuntimeView::onErrorOccurred);
    connect(m_inferenceWorker, &AIInferenceWorker::logMessage, this, &AIRuntimeView::onWorkerLog);
    
    // UI Update
    ui->btnLaunch->setEnabled(false);
    ui->btnStop->setEnabled(true);
    ui->comboActiveProfile->setEnabled(false);

    if (m_liveWidget) {
        m_liveWidget->play("cam_1");
    }

    m_inferenceWorker->start();
}

void AIRuntimeView::stopInference()
{
    if (m_inferenceWorker) {
        qDebug() << "AIRuntimeView: Stopping inference worker thread...";
        m_inferenceWorker->stop();
        m_inferenceWorker->deleteLater();
        m_inferenceWorker = nullptr;
    }
    
    if (m_liveWidget) m_liveWidget->stop();
    ui->btnLaunch->setEnabled(true);
    ui->btnStop->setEnabled(false);
    ui->comboActiveProfile->setEnabled(true);
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
        if (src.type == "USB Camera") src.value = "usb:" + QString::number(ui->spinUSBIndex->value());
        else if (src.type == "RTSP Stream") src.value = ui->editRTSPUrl->text();
        else src.value = ui->editFilePath->text();
        updateSourcesUI();
        ui->groupSourceEdit->setEnabled(false);
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
    ui->editProfileName->setText(p.name);
    ui->editModelPath->setText(p.modelYamlPath);
    ui->comboProfileCamera->setCurrentText(p.cameraSourceName);
    ui->spinConfidence->setValue(p.confidence);
    ui->editAipuCores->setText(p.aipuCores);
    ui->editClassMapPath->setText(p.classMapPath);
    ui->editEmbeddingPath->setText(p.embeddingPath);
    ui->comboModelZoo->setCurrentText(p.modelZooName);
    m_requestedDeviceName = p.edgexDeviceName;

    if (!m_requestedDeviceName.isEmpty()) {
        ui->editEdgeXDeviceName->setText(m_requestedDeviceName);
    } else {
        ui->editEdgeXDeviceName->clear();
    }
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
    p.edgexDeviceName = ui->editEdgeXDeviceName->text();
    
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
        p.edgexDeviceName = s.value("edgex_device").toString();
        p.edgexProfileName = s.value("edgex_profile").toString();
        m_profiles.append(p);
        ui->comboProfiles->addItem(p.name);
        ui->comboActiveProfile->addItem(p.name);
    }
    s.endArray();
    
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
    s.beginWriteArray("profiles");
    for (int i=0; i<m_profiles.size(); ++i) {
        s.setArrayIndex(i);
        const auto &p = m_profiles[i];
        s.setValue("name", p.name);
        s.setValue("model", p.modelYamlPath);
        s.setValue("camera", p.cameraSourceName);
        s.setValue("conf", p.confidence);
        s.setValue("cores", p.aipuCores);
        s.setValue("class_map", p.classMapPath);
        s.setValue("embedding", p.embeddingPath);
        s.setValue("zoo_name", p.modelZooName);
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
void AIRuntimeView::onFrameReceived(int streamId, const QImage &frame, const QJsonArray &detections) 
{
    if (m_liveWidget) {
        m_liveWidget->updateNativeFrame(streamId, frame, detections);
    }
}
void AIRuntimeView::onWorkerLog(const QString &msg) { qDebug() << "[InferenceWorker]" << msg; }
void AIRuntimeView::onErrorOccurred(const QString &e) { QMessageBox::critical(this, "Error", e); stopInference(); }
void AIRuntimeView::addSource() { m_sources.append({"New", "USB Camera", "/dev/video20"}); updateSourcesUI(); }
void AIRuntimeView::editSource() { ui->groupSourceEdit->setEnabled(true); }
void AIRuntimeView::deleteSource() { int r = ui->tableSources->currentRow(); if (r>=0) { m_sources.removeAt(r); updateSourcesUI(); }}
void AIRuntimeView::browseVideoFile() { QString f = QFileDialog::getOpenFileName(this); if (!f.isEmpty()) ui->editFilePath->setText(f); }
void AIRuntimeView::onSourceTypeChanged(int i) { ui->stackSourceConfig->setCurrentIndex(i); }
void AIRuntimeView::onSourceSelectionChanged() { 
    int r = ui->tableSources->currentRow();
    if (r>=0 && r<m_sources.size()) {
        const auto &s = m_sources[r];
        ui->editSourceName->setText(s.name);
        ui->comboSourceType->setCurrentText(s.type);
    }
}
void AIRuntimeView::onSourceDataChanged() {}
void AIRuntimeView::onBrowseOutput() { QString d = QFileDialog::getExistingDirectory(this); if (!d.isEmpty()) ui->editSaveOutputPath->setText(d); }
void AIRuntimeView::onBrowseClassMap() { QFileDialog diag(this, "Select Class Map", m_sdkPath); diag.setOption(QFileDialog::DontUseNativeDialog); diag.setLabelText(QFileDialog::Accept, "Select"); if (diag.exec()) ui->editClassMapPath->setText(diag.selectedFiles().first()); }
void AIRuntimeView::onBrowseEmbedding() { QFileDialog diag(this, "Select Embedding", m_sdkPath); diag.setOption(QFileDialog::DontUseNativeDialog); diag.setLabelText(QFileDialog::Accept, "Select"); if (diag.exec()) ui->editEmbeddingPath->setText(diag.selectedFiles().first()); }
void AIRuntimeView::updateAipuStatus() { 
    // In a real scenario, we would parse this from backend metadata or run axdevice
    // For now, we show a status indicating the hardware is ready or active
    if (ui->btnStop->isEnabled()) {
        ui->lblAipuStatus->setText("<font color='green'>AIPU: Active (4 Cores)</font>");
    } else {
        ui->lblAipuStatus->setText("<font color='gray'>AIPU: Standby</font>");
    }
}
void AIRuntimeView::onNewProfile() { InferenceProfile p; p.name="New Profile"; m_profiles.append(p); ui->comboProfiles->addItem(p.name); ui->comboActiveProfile->addItem(p.name); ui->comboProfiles->setCurrentIndex(m_profiles.size()-1); }
void AIRuntimeView::onDeleteProfile() { int i = ui->comboProfiles->currentIndex(); if (i>=0) { m_profiles.removeAt(i); ui->comboProfiles->removeItem(i); ui->comboActiveProfile->removeItem(i); }}
void AIRuntimeView::onSaveEdgeXManual() { saveSettings(); }
void AIRuntimeView::onValidateJSON()
{
    QString content = ui->textPayloadTemplate->toPlainText();
    if (content.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Template is empty.");
        return;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8(), &err);
    if (doc.isNull()) {
        QMessageBox::critical(this, "JSON Error", "Invalid JSON: " + err.errorString());
        return;
    }

    // Extract resource names (n) from template
    QStringList templateResources;
    if (doc.isArray()) {
        QJsonArray arr = doc.array();
        for (int i=0; i<arr.size(); ++i) {
            QString n = arr[i].toObject()["n"].toString();
            if (!n.isEmpty()) templateResources << n;
        }
    } else if (doc.isObject()) {
        QString n = doc.object()["n"].toString();
        if (!n.isEmpty()) templateResources << n;
    }

    if (templateResources.isEmpty()) {
        QMessageBox::warning(this, "Validation", "No resource names ('n' field) found in template.");
        return;
    }

    // Check resources
    QJsonArray devResources = selectedProfile["deviceResources"].toArray();
    QStringList availableResources;
    for (int i=0; i<devResources.size(); ++i) {
        availableResources << devResources[i].toObject()["name"].toString();
    }

    QStringList missing;
    for (const QString &res : templateResources) {
        // Skip placeholders like <string>, <integer> if they appear in 'n'
        if (res.startsWith("<") && res.endsWith(">")) continue;
        if (!availableResources.contains(res)) missing << res;
    }

    if (missing.isEmpty()) {
        QMessageBox::information(this, "Success", QString("Template is valid for profile '%1'.\nResources checked: %2").arg(profileName).arg(templateResources.join(", ")));
    } else {
        QMessageBox::critical(this, "Validation Failed", 
            QString("The following resources in the template are NOT defined in profile '%1':\n\n%2")
            .arg(profileName).arg(missing.join("\n")));
    }
}
void AIRuntimeView::onBrowseTemplate()
{
    QString f = QFileDialog::getOpenFileName(this, "Open SenML Template", m_sdkPath, "JSON Files (*.json);;All Files (*)");
    if (!f.isEmpty()) ui->editTemplatePath->setText(f);
}

void AIRuntimeView::onValidateFile()
{
    QString path = ui->editTemplatePath->text();
    if (path.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Please select a template file first.");
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open file: " + file.errorString());
        return;
    }

    QString content = file.readAll();
    file.close();

    // Use shared validation logic (refactored or duplicated for brevity here)
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8(), &err);
    if (doc.isNull()) {
        QMessageBox::critical(this, "JSON Error", "Invalid JSON in file: " + err.errorString());
        return;
    }

    // Extract resource names (n)
    QStringList templateResources;
    QJsonArray arr = doc.isArray() ? doc.array() : QJsonArray();
    if (doc.isObject()) arr.append(doc.object());
    
    for (int i=0; i<arr.size(); ++i) {
        QString n = arr[i].toObject()["n"].toString();
        if (!n.isEmpty()) templateResources << n;
    }

    if (templateResources.isEmpty()) {
        QMessageBox::warning(this, "Validation", "No resource names ('n') found in file.");
        return;
    }

    // Check against selected profile
    int activeIdx = ui->comboActiveProfile->currentIndex();
    QString profileName;
    if (activeIdx >= 0 && activeIdx < m_profiles.size()) {
        profileName = m_profiles[activeIdx].edgexProfileName;
    }

    QJsonObject selectedProfile;
    for (int i=0; i<m_discoveredProfiles.size(); ++i) {
        if (m_discoveredProfiles[i].toObject()["name"].toString() == profileName) {
            selectedProfile = m_discoveredProfiles[i].toObject();
            break;
        }
    }

    if (selectedProfile.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Selected profile not found.");
        return;
    }

    QJsonArray devResources = selectedProfile["deviceResources"].toArray();
    QStringList availableResources;
    for (int i=0; i<devResources.size(); ++i) availableResources << devResources[i].toObject()["name"].toString();

    QStringList missing;
    for (const QString &res : templateResources) {
        if (res.startsWith("<") && res.endsWith(">")) continue;
        if (!availableResources.contains(res)) missing << res;
    }

    if (missing.isEmpty()) {
        QMessageBox::information(this, "Success", "File template is valid for current profile.");
    } else {
        QMessageBox::critical(this, "Validation Failed", "Missing resources:\n" + missing.join("\n"));
    }
}

void AIRuntimeView::onUploadEdgeXTemplate()
{
    QString path = ui->editTemplatePath->text();
    if (path.isEmpty()) return;
    QFile f(path);
    if (f.open(QIODevice::ReadOnly)) {
        ui->textPayloadTemplate->setPlainText(f.readAll());
        f.close();
        onValidateJSON();
    } else {
        QMessageBox::critical(this, "Error", "Failed to load template file.");
    }
}

void AIRuntimeView::onEdgeXDeviceChanged(const QString &text)
{
    QString name = text.trimmed();
    m_requestedDeviceName = name;

    QSettings s;
    s.setValue("edgex/last_device", name);
    s.sync();

    int profIdx = ui->comboProfiles->currentIndex();
    if (profIdx >= 0 && profIdx < m_profiles.size()) {
        m_profiles[profIdx].edgexDeviceName = name;
    }
}


