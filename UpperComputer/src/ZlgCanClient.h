#pragma once

#include <QLibrary>
#include <QObject>
#include <QProcess>
#include <QTimer>

class ZlgCanClient final : public QObject
{
    Q_OBJECT

public:
    explicit ZlgCanClient(QObject *parent = nullptr);

    bool isActive() const;
    bool prepareLibrary(const QString &libraryPath, QString *error);
    bool probeDevice(const QString &libraryPath,
                     quint32 deviceType,
                     quint32 deviceIndex,
                     QString *error);
    void startTest(const QString &libraryPath,
                   quint32 deviceType,
                   quint32 deviceIndex,
                   quint32 channelIndex);
    void cancel();

signals:
    void testProgress(const QString &message);
    void testFinished(bool passed, const QString &message);

private:
    friend class MainWindowTests;
    struct VciInitConfig {
        quint32 acceptanceCode;
        quint32 acceptanceMask;
        quint32 reserved;
        quint8 filter;
        quint8 timing0;
        quint8 timing1;
        quint8 mode;
    };

    struct VciCanObject {
        quint32 id;
        quint32 timeStamp;
        quint8 timeFlag;
        quint8 sendType;
        quint8 remoteFlag;
        quint8 externFlag;
        quint8 dataLength;
        quint8 data[8];
        quint8 reserved[3];
    };

    static_assert(sizeof(VciInitConfig) == 16U,
                  "VCI_INIT_CONFIG layout must match ControlCAN.dll");
    static_assert(sizeof(VciCanObject) == 24U,
                  "VCI_CAN_OBJ layout must match ControlCAN.dll");

    using VciOpenDevice = quint32 (*)(quint32, quint32, quint32);
    using VciCloseDevice = quint32 (*)(quint32, quint32);
    using VciInitCan = quint32 (*)(quint32, quint32, quint32, VciInitConfig *);
    using VciStartCan = quint32 (*)(quint32, quint32, quint32);
    using VciResetCan = quint32 (*)(quint32, quint32, quint32);
    using VciClearBuffer = quint32 (*)(quint32, quint32, quint32);
    using VciTransmit = quint32 (*)(quint32, quint32, quint32, VciCanObject *, quint32);
    using VciReceive = quint32 (*)(quint32, quint32, quint32, VciCanObject *, quint32, qint32);

    bool loadLibrary(const QString &libraryPath, QString *error);
    bool probeBridge(QString *error);
    bool openDevice(QString *error);
    void closeDevice();
    void startBridgeTest();
    QStringList bridgeArguments(const QString &mode) const;
    QString bridgeExecutablePath() const;
    void pollReceive();
    void finish(bool passed, const QString &message);

    QLibrary m_library;
    QProcess m_bridgeProcess;
    QString m_loadedLibraryPath;
    QString m_bridgeExecutable;
    QString m_bridgeOutput;
    QTimer m_pollTimer;
    QTimer m_timeoutTimer;
    VciOpenDevice m_openDevice = nullptr;
    VciCloseDevice m_closeDevice = nullptr;
    VciInitCan m_initCan = nullptr;
    VciStartCan m_startCan = nullptr;
    VciResetCan m_resetCan = nullptr;
    VciClearBuffer m_clearBuffer = nullptr;
    VciTransmit m_transmit = nullptr;
    VciReceive m_receive = nullptr;
    quint32 m_deviceType = 3U;
    quint32 m_deviceIndex = 0U;
    quint32 m_channelIndex = 0U;
    bool m_deviceOpened = false;
    bool m_channelStarted = false;
    bool m_useBridge = false;
    bool m_active = false;
};
