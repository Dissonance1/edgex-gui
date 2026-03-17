#ifndef AIRUNTIMEVIEW_H
#define AIRUNTIMEVIEW_H

#include <QWidget>
#include <QList>
#include <QImage>
#include <QTimer>
#include <QMap>
#include <QProcess>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "MetadataClient.h"
#include "AIInferenceWorker.h"

class LiveMonitoringWidget;

namespace Ui {
class AIRuntimeView;
}

struct CameraSourceInfo {
    QString name;
    QString type; // "USB", "RTSP", "File"
    QString value; // Index, URL, or Path
};

struct InferenceProfile {
    QString name;
    QString modelZooName;
    QString modelYamlPath;
    QString cameraSourceName;
    double confidence;
    QString pipelineType;
    int frameLimit;
    QString displayMode;
    QString windowSize;
    QString saveOutputPath;
    bool hwCodec;
    bool vaapi;
    bool opencl;
    bool opengl;
    QString aipuCores;
    bool showHostFps;
    bool showStreamTiming;
    // EdgeX Settings
    QString edgexDeviceName;
    QString edgexProfileName;
    QString edgexPayloadTemplate;
    // Metadata Files
    QString classMapPath;
    QString embeddingPath;
};

class AIRuntimeView : public QWidget
{
    Q_OBJECT
public:
    explicit AIRuntimeView(QWidget *parent = nullptr);
    ~AIRuntimeView();

public slots:
    void onNavItemChanged(int index);

private slots:
    // Inference Control
    void startInference();
    void stopInference();
    
    // Backend Signals
    void onFrameReceived(int streamId, const QImage &frame, const QJsonArray &detections);
    void onErrorOccurred(const QString &error);
    void onWorkerLog(const QString &msg);

    // Source Management
    void addSource();
    void editSource();
    void onSaveSource();
    void deleteSource();
    void browseVideoFile();
    void onSourceTypeChanged(int index);
    void onSourceSelectionChanged();
    void onSourceDataChanged(); // Internal helper
    
    // Model & Pipeline Settings
    void onBrowseOutput();
    void onModelZooChanged(int index);
    void onBrowseModelYaml();
    void onBrowseClassMap();
    void onBrowseEmbedding();
    void updateAipuStatus();
    
    // Profile Management
    void onNewProfile();
    void onSaveProfile();
    void onDeleteProfile();
    void onProfileSelectionChanged(int index);

    // EdgeX Configuration
    void onSaveEdgeXManual();
    void onValidateJSON();
    void onBrowseTemplate();
    void onValidateFile();
    void onUploadEdgeXTemplate();
    void onEdgeXDeviceChanged(const QString &text);

private:
    void setupLivePanel();
    void loadSettings();
    void saveSettings();
    void populateCameras();
    void updateSourcesUI();
    void scanModelZoo();
    void loadModelMetadata(const QString& yamlPath);
    void fetchEdgeXMetaData();
    void startBackendDaemon();
    
    Ui::AIRuntimeView *ui;
    QNetworkAccessManager *m_netManager;
    LiveMonitoringWidget *m_liveWidget;
    AIInferenceWorker *m_inferenceWorker;
    
    QList<CameraSourceInfo> m_sources;
    QList<InferenceProfile> m_profiles;
    QTimer* m_aipuTimer;
    QMap<QString, QString> m_modelZooPaths;
    QString m_sdkPath;

    MetadataClient *m_metadataClient;
    QJsonArray m_discoveredDevices;
    QJsonArray m_discoveredProfiles;

    QString m_requestedDeviceName;
};

#endif // AIRUNTIMEVIEW_H
