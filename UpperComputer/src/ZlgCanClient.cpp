#include "ZlgCanClient.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QThread>

namespace {

constexpr quint32 ZlgUsbCan1DeviceType = 3U;
constexpr quint32 ZlgUsbCan2DeviceType = 4U;
constexpr quint32 ZlgStatusOk = 1U;
constexpr quint32 CanRequestId = 0x321U;
constexpr quint32 CanResponseId = 0x001U;
constexpr quint8 CanTestData[] = {0xA5U, 0x5AU, 0xA5U, 0x5AU};
constexpr int CanResponseTimeoutMs = 2000;
constexpr int CanReceivePollIntervalMs = 20;

bool IsLegacyX86Dll(const QString &libraryPath)
{
    QFile libraryFile(libraryPath);
    QByteArray dosHeader;
    QByteArray machineBytes;
    quint32 peOffset;

    if(!libraryFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    dosHeader = libraryFile.read(64);
    if((dosHeader.size() != 64) || (dosHeader[0] != 'M') || (dosHeader[1] != 'Z'))
    {
        return false;
    }

    peOffset = static_cast<quint8>(dosHeader[60]) |
               (static_cast<quint32>(static_cast<quint8>(dosHeader[61])) << 8U) |
               (static_cast<quint32>(static_cast<quint8>(dosHeader[62])) << 16U) |
               (static_cast<quint32>(static_cast<quint8>(dosHeader[63])) << 24U);
    if(!libraryFile.seek(static_cast<qint64>(peOffset) + 4))
    {
        return false;
    }

    machineBytes = libraryFile.read(2);
    return (machineBytes.size() == 2) &&
           (static_cast<quint8>(machineBytes[0]) == 0x4CU) &&
           (static_cast<quint8>(machineBytes[1]) == 0x01U);
}

} // namespace

ZlgCanClient::ZlgCanClient(QObject *parent)
    : QObject(parent)
{
    m_pollTimer.setInterval(CanReceivePollIntervalMs);
    m_timeoutTimer.setSingleShot(true);

    connect(&m_pollTimer, &QTimer::timeout, this, &ZlgCanClient::pollReceive);
    connect(&m_timeoutTimer, &QTimer::timeout, this, [this] {
        finish(false, QStringLiteral("ZLG CAN 等待 DSP 应答帧超时。"));
    });
    connect(&m_bridgeProcess, &QProcess::readyReadStandardOutput, this, [this] {
        m_bridgeOutput += QString::fromLocal8Bit(m_bridgeProcess.readAllStandardOutput());
    });
    connect(&m_bridgeProcess, &QProcess::readyReadStandardError, this, [this] {
        m_bridgeOutput += QString::fromLocal8Bit(m_bridgeProcess.readAllStandardError());
    });
    connect(&m_bridgeProcess,
            qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
        m_bridgeOutput += QString::fromLocal8Bit(m_bridgeProcess.readAllStandardOutput());
        m_bridgeOutput += QString::fromLocal8Bit(m_bridgeProcess.readAllStandardError());
        if(!m_active || !m_useBridge)
        {
            return;
        }

        if((exitStatus == QProcess::NormalExit) && (exitCode == 0) &&
           m_bridgeOutput.contains(QStringLiteral("RESULT=PASS")))
        {
            finish(true, QStringLiteral("ZLG CAN 自动测试通过：0x0321 -> 0x0001。"));
            return;
        }

        finish(false,
               QStringLiteral("ZLG 32 位 CAN 桥接测试失败：%1")
                   .arg(m_bridgeOutput.trimmed()));
    });
}

bool ZlgCanClient::isActive() const
{
    return m_active;
}

bool ZlgCanClient::prepareLibrary(const QString &libraryPath, QString *error)
{
    if(m_active)
    {
        if(error != 0)
        {
            *error = QStringLiteral("ZLG CAN 测试正在进行，不能更换接口库。");
        }
        return false;
    }

    return loadLibrary(libraryPath, error);
}

bool ZlgCanClient::probeDevice(const QString &libraryPath,
                               quint32 deviceType,
                               quint32 deviceIndex,
                               QString *error)
{
    if((deviceType != ZlgUsbCan1DeviceType) &&
       (deviceType != ZlgUsbCan2DeviceType))
    {
        if(error != 0)
        {
            *error = QStringLiteral("ZLG CAN 设备类型必须为 USBCAN1 或 USBCAN2。");
        }
        return false;
    }

    if(!prepareLibrary(libraryPath, error))
    {
        return false;
    }

    m_deviceType = deviceType;
    m_deviceIndex = deviceIndex;
    m_channelIndex = 0U;
    if(m_useBridge)
    {
        return probeBridge(error);
    }
    if(!openDevice(error))
    {
        return false;
    }

    closeDevice();
    return true;
}

void ZlgCanClient::startTest(const QString &libraryPath,
                             quint32 deviceType,
                             quint32 deviceIndex,
                             quint32 channelIndex)
{
    QString error;
    VciInitConfig config = {};
    VciCanObject request = {};

    if (m_active) {
        emit testProgress(QStringLiteral("ZLG CAN 测试正在进行。"));
        return;
    }

    if((deviceType != ZlgUsbCan1DeviceType) &&
       (deviceType != ZlgUsbCan2DeviceType)) {
        emit testFinished(false, QStringLiteral("ZLG CAN 设备类型必须为 USBCAN1 或 USBCAN2。"));
        return;
    }

    if (channelIndex > 1U) {
        emit testFinished(false, QStringLiteral("ZLG USBCAN-II 通道必须为 CAN1 或 CAN2。"));
        return;
    }

    if (!loadLibrary(libraryPath, &error)) {
        emit testFinished(false, error);
        return;
    }

    m_active = true;
    m_deviceType = deviceType;
    m_deviceIndex = deviceIndex;
    m_channelIndex = channelIndex;

    if(m_useBridge)
    {
        startBridgeTest();
        return;
    }

    if(!openDevice(&error)) {
        finish(false, error);
        return;
    }

    config.acceptanceCode = 0U;
    config.acceptanceMask = 0xFFFFFFFFU;
    config.reserved = 0U;
    config.filter = 1U;
    config.timing0 = 0x00U;
    config.timing1 = 0x1CU;
    config.mode = 0U;

    if (m_initCan(m_deviceType, m_deviceIndex, m_channelIndex, &config) != ZlgStatusOk) {
        finish(false, QStringLiteral("ZLG CAN 初始化失败。请确认通道和 500 kbit/s 配置。"));
        return;
    }

    if (m_clearBuffer(m_deviceType, m_deviceIndex, m_channelIndex) != ZlgStatusOk) {
        finish(false, QStringLiteral("ZLG CAN 清空接收缓冲区失败。"));
        return;
    }

    if (m_startCan(m_deviceType, m_deviceIndex, m_channelIndex) != ZlgStatusOk) {
        finish(false, QStringLiteral("ZLG CAN 启动失败。"));
        return;
    }
    m_channelStarted = true;
    QThread::msleep(100U);

    request.id = CanRequestId;
    request.sendType = 0U;
    request.remoteFlag = 0U;
    request.externFlag = 0U;
    request.dataLength = sizeof(CanTestData);
    for (int index = 0; index < request.dataLength; ++index) {
        request.data[index] = CanTestData[index];
    }

    quint32 transmitResult = 0U;
    int transmitAttempts = 0;
    for (; transmitAttempts < 3; ++transmitAttempts) {
        transmitResult = m_transmit(m_deviceType, m_deviceIndex, m_channelIndex,
                                    &request, 1U);
        if (transmitResult == ZlgStatusOk) {
            break;
        }
        QThread::msleep(100U);
    }

    if (transmitResult != ZlgStatusOk) {
        finish(false,
               QStringLiteral("ZLG CAN 发送 ID=0x0321 测试帧失败（已尝试 %1 次）。"
                              "请检查 CANH/CANL、终端电阻和总线 ACK。")
                   .arg(transmitAttempts));
        return;
    }

    emit testProgress(
        QStringLiteral("ZLG CAN 自动测试：CAN%1，500 kbit/s，已发送 ID=0x0321，"
                       "数据=A5 5A A5 5A，等待 ID=0x0001。")
            .arg(m_channelIndex + 1U));
    m_pollTimer.start();
    m_timeoutTimer.start(CanResponseTimeoutMs);
}

void ZlgCanClient::cancel()
{
    if (m_active) {
        if(m_useBridge && (m_bridgeProcess.state() != QProcess::NotRunning))
        {
            m_bridgeProcess.kill();
        }
        finish(false, QStringLiteral("ZLG CAN 测试已取消。"));
    }
}

bool ZlgCanClient::loadLibrary(const QString &libraryPath, QString *error)
{
    const QFileInfo fileInfo(libraryPath);

    if (libraryPath.isEmpty() || !fileInfo.isFile()) {
        *error = QStringLiteral("未选择有效的 ControlCAN.dll。请在上位机中选择 ZLG CAN 接口库。");
        return false;
    }

    if((m_loadedLibraryPath.compare(fileInfo.absoluteFilePath(), Qt::CaseInsensitive) == 0) &&
       ((m_useBridge && QFileInfo::exists(m_bridgeExecutable)) ||
        (!m_useBridge && m_library.isLoaded()))) {
        return true;
    }

    closeDevice();
    if (m_library.isLoaded()) {
        m_library.unload();
    }

    m_useBridge = false;
    m_bridgeExecutable.clear();
    if(IsLegacyX86Dll(fileInfo.absoluteFilePath()))
    {
        m_bridgeExecutable = bridgeExecutablePath();
        if(!QFileInfo(m_bridgeExecutable).isFile())
        {
            *error = QStringLiteral("已选择 32 位 ControlCAN.dll，但缺少 HpdCanBridge32.exe。请重新构建上位机程序。");
            return false;
        }

        m_loadedLibraryPath = fileInfo.absoluteFilePath();
        m_useBridge = true;
        return true;
    }

    m_library.setFileName(fileInfo.absoluteFilePath());
    if (!m_library.load()) {
        *error = QStringLiteral("加载 ControlCAN.dll 失败：%1").arg(m_library.errorString());
        return false;
    }

    m_openDevice = reinterpret_cast<VciOpenDevice>(m_library.resolve("VCI_OpenDevice"));
    m_closeDevice = reinterpret_cast<VciCloseDevice>(m_library.resolve("VCI_CloseDevice"));
    m_initCan = reinterpret_cast<VciInitCan>(m_library.resolve("VCI_InitCAN"));
    m_startCan = reinterpret_cast<VciStartCan>(m_library.resolve("VCI_StartCAN"));
    m_resetCan = reinterpret_cast<VciResetCan>(m_library.resolve("VCI_ResetCAN"));
    m_clearBuffer = reinterpret_cast<VciClearBuffer>(m_library.resolve("VCI_ClearBuffer"));
    m_transmit = reinterpret_cast<VciTransmit>(m_library.resolve("VCI_Transmit"));
    m_receive = reinterpret_cast<VciReceive>(m_library.resolve("VCI_Receive"));

    if ((m_openDevice == nullptr) || (m_closeDevice == nullptr) ||
        (m_initCan == nullptr) || (m_startCan == nullptr) ||
        (m_resetCan == nullptr) || (m_clearBuffer == nullptr) ||
        (m_transmit == nullptr) || (m_receive == nullptr)) {
        *error = QStringLiteral("ControlCAN.dll 缺少 USBCAN-II 所需接口函数。请使用 ZLG 官方 64 位库。");
        m_library.unload();
        return false;
    }

    m_loadedLibraryPath = fileInfo.absoluteFilePath();
    return true;
}

bool ZlgCanClient::openDevice(QString *error)
{
    const quint32 openResult =
        m_openDevice(m_deviceType, m_deviceIndex, 0U);

    if(openResult != ZlgStatusOk)
    {
        const QString openDetail = (openResult == 0xFFFFFFFFU) ?
            QStringLiteral("ControlCAN 驱动未识别到设备") :
            QStringLiteral("驱动返回设备打开失败状态");
        if(error != 0)
        {
            *error =
                QStringLiteral("无法打开 ZLG USB-CAN（设备类型=%1，设备索引=%2，返回值=%3 / 0x%4，%5）。"
                               "请确认驱动、CAN_TEST 占用状态和设备索引。")
                    .arg(m_deviceType)
                    .arg(m_deviceIndex)
                    .arg(openResult)
                    .arg(openResult, 8, 16, QLatin1Char('0')).toUpper()
                    .arg(openDetail);
        }
        return false;
    }

    m_deviceOpened = true;
    return true;
}

bool ZlgCanClient::probeBridge(QString *error)
{
    QProcess bridgeProcess;

    bridgeProcess.setProgram(m_bridgeExecutable);
    bridgeProcess.setArguments(bridgeArguments(QStringLiteral("probe")));
    bridgeProcess.start();
    if(!bridgeProcess.waitForStarted(2000))
    {
        *error = QStringLiteral("无法启动 ZLG 32 位 CAN 桥接程序：%1")
                     .arg(bridgeProcess.errorString());
        return false;
    }
    if(!bridgeProcess.waitForFinished(3000))
    {
        bridgeProcess.kill();
        bridgeProcess.waitForFinished();
        *error = QStringLiteral("ZLG 32 位 CAN 桥接程序打开设备超时。");
        return false;
    }

    const QString output = QString::fromLocal8Bit(bridgeProcess.readAllStandardOutput()) +
                           QString::fromLocal8Bit(bridgeProcess.readAllStandardError());
    if((bridgeProcess.exitStatus() != QProcess::NormalExit) ||
       (bridgeProcess.exitCode() != 0) ||
       !output.contains(QStringLiteral("RESULT=READY")))
    {
        *error = QStringLiteral("ZLG 32 位 CAN 桥接程序无法打开设备：%1")
                     .arg(output.trimmed());
        return false;
    }

    return true;
}

void ZlgCanClient::closeDevice()
{
    if (m_channelStarted && (m_resetCan != nullptr)) {
        m_resetCan(m_deviceType, m_deviceIndex, m_channelIndex);
    }
    if (m_deviceOpened && (m_closeDevice != nullptr)) {
        m_closeDevice(m_deviceType, m_deviceIndex);
    }

    m_channelStarted = false;
    m_deviceOpened = false;
}

void ZlgCanClient::startBridgeTest()
{
    m_bridgeOutput.clear();
    m_bridgeProcess.setProgram(m_bridgeExecutable);
    m_bridgeProcess.setArguments(bridgeArguments(QStringLiteral("test")));
    m_bridgeProcess.start();
    if(!m_bridgeProcess.waitForStarted(2000))
    {
        finish(false,
               QStringLiteral("无法启动 ZLG 32 位 CAN 桥接程序：%1")
                   .arg(m_bridgeProcess.errorString()));
        return;
    }

    emit testProgress(
        QStringLiteral("ZLG CAN 自动测试：通过 32 位驱动桥接发送 ID=0x0321，等待 ID=0x0001。"));
}

QStringList ZlgCanClient::bridgeArguments(const QString &mode) const
{
    return QStringList {
        QStringLiteral("--mode"), mode,
        QStringLiteral("--dll"), m_loadedLibraryPath,
        QStringLiteral("--type"), QString::number(m_deviceType),
        QStringLiteral("--index"), QString::number(m_deviceIndex),
        QStringLiteral("--channel"), QString::number(m_channelIndex),
        QStringLiteral("--timeout-ms"), QString::number(CanResponseTimeoutMs)
    };
}

QString ZlgCanClient::bridgeExecutablePath() const
{
    return QCoreApplication::applicationDirPath() +
           QLatin1String("/HpdCanBridge32.exe");
}

void ZlgCanClient::pollReceive()
{
    VciCanObject frames[16] = {};
    const quint32 receiveCount = m_receive(m_deviceType,
                                           m_deviceIndex,
                                           m_channelIndex,
                                           frames,
                                           16U,
                                           0);

    if (receiveCount == 0U) {
        return;
    }

    if (receiveCount == 0xFFFFFFFFU) {
        finish(false, QStringLiteral("ZLG CAN 接收接口返回错误。"));
        return;
    }

    for (quint32 index = 0U; index < receiveCount && index < 16U; ++index) {
        const VciCanObject &frame = frames[index];
        bool dataMatches = (frame.dataLength == sizeof(CanTestData));

        for (int dataIndex = 0; dataMatches && dataIndex < frame.dataLength; ++dataIndex) {
            dataMatches = (frame.data[dataIndex] == CanTestData[dataIndex]);
        }

        if ((frame.externFlag == 0U) && (frame.remoteFlag == 0U) &&
            (frame.id == CanResponseId) && dataMatches) {
            finish(true, QStringLiteral("ZLG CAN 自动测试通过：0x0321 -> 0x0001。"));
            return;
        }
    }
}

void ZlgCanClient::finish(bool passed, const QString &message)
{
    if (!m_active) {
        return;
    }

    m_active = false;
    m_pollTimer.stop();
    m_timeoutTimer.stop();
    if(!m_useBridge)
    {
        closeDevice();
    }
    emit testFinished(passed, message);
}
