#include "MainWindow.h"
#include "FullTextComboBox.h"

#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QFormLayout>
#include <QFont>
#include <QGuiApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QSettings>
#include <QScreen>
#include <QSplitter>
#include <QStatusBar>
#include <QStandardPaths>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <utility>

namespace {

constexpr quint16 TestDido = 0x0313;
constexpr quint16 TestHdo = 0x0314;
constexpr quint16 TestDi = 0x0315;
constexpr quint16 TestCanExternal = 0x0307;
constexpr quint16 TestSciRs485External = 0x0309;
constexpr quint16 TestRs422External = 0x0316;
constexpr quint16 TestEthernetBasic = 0x0305;
constexpr quint16 TestEthernetSocket = 0x030D;
constexpr quint16 TestEthernetTcpLink = 0x030E;
constexpr quint16 TestEthernetTcpEcho = 0x030F;
constexpr quint16 TestEthernetTcpStability = 0x0310;
constexpr quint16 TestAdcExternalInjection = 0x0319;
constexpr quint16 TestAdcExternalGroup1 = 0x031A;
constexpr quint16 TestAdcExternalGroup2 = 0x031B;
constexpr quint16 TestAdcPt100Mux = 0x031C;
constexpr quint16 TestPwmDido = 0x031F;
constexpr quint16 TestDriverFaultDi = 0x0320;
constexpr quint16 TestProtectionFaultDi = 0x0321;
constexpr quint16 TestDriverResetDo = 0x0322;
constexpr quint16 TestLowVoltageDi = 0x0400;
constexpr quint16 TestLowVoltageDo = 0x0401;
constexpr quint16 TestLowVoltageHdo = 0x0402;
constexpr quint16 TestLowVoltageSto = 0x0403;
constexpr quint16 TestLowVoltageAdc = 0x0404;
constexpr quint16 TestLowVoltagePt100 = 0x0405;
constexpr quint16 TestLowVoltageNtc = 0x0406;
constexpr quint16 TestLowVoltagePwm = 0x0407;
constexpr quint16 TestLowVoltageProtection = 0x0408;
constexpr quint16 TestLowVoltageAo = 0x0409;
constexpr quint16 TestLowVoltageResolver = 0x040A;
constexpr quint8 AdcExternalSelectionGroup1 = 0x80;
constexpr quint8 AdcExternalSelectionGroup2 = 0x81;
constexpr quint8 AdcExternalSelectionPt100Base = 0x90;
constexpr quint16 Pt4TestExpectedMillivolts = 1688;
constexpr quint16 Pt4TestToleranceMillivolts = 100;
constexpr quint32 ProtectionRecordFormatMask = 0x0E000000U;
constexpr quint32 ProtectionRecordFormatV2 = 0x0A000000U;

QString hex16(quint16 value)
{
    return QStringLiteral("0x%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
}

QString hex32(quint32 value)
{
    return QStringLiteral("0x%1").arg(value, 8, 16, QLatin1Char('0')).toUpper();
}

quint16 protectionFaultBitForSelection(quint8 selection)
{
    if((selection <= 2U) || ((selection >= 6U) && (selection <= 8U))) {
        return 0x0001U;
    }
    if((selection <= 5U) || ((selection >= 9U) && (selection <= 11U))) {
        return 0x0002U;
    }
    return (selection == 12U) ? 0x0004U : 0x0008U;
}

QString protectionFaultInputName(quint16 faultBit)
{
    switch(faultBit) {
    case 0x0001U:
        return QStringLiteral("D_OC1/GPIO77");
    case 0x0002U:
        return QStringLiteral("D_OC2/GPIO76");
    case 0x0004U:
        return QStringLiteral("D_UOV1/GPIO82");
    case 0x0008U:
        return QStringLiteral("D_UOV2/GPIO83");
    default:
        return QStringLiteral("未知保护输入");
    }
}

QString boardProfileName(quint16 profileId)
{
    switch (profileId) {
    case 0x0001U:
        return QStringLiteral("SiC多传系统主控板");
    case 0x0002U:
        return QStringLiteral("低压工业变频器");
    case 0x0003U:
        return QStringLiteral("低空经济统型板");
    default:
        return QStringLiteral("未知板型");
    }
}

QString ethernetInterfaceText(DspTestProtocol::EthernetInterface interface)
{
    switch (interface) {
    case DspTestProtocol::EthernetInterface::EmifW5300:
        return QStringLiteral("EMIF + W5300");
    case DspTestProtocol::EthernetInterface::SpicW5500:
        return QStringLiteral("SPIC + W5500");
    case DspTestProtocol::EthernetInterface::None:
        return QStringLiteral("未配置以太网");
    }

    return QStringLiteral("未知以太网接口");
}

QString boardProfileStateText(DspTestProtocol::BoardProfileState state)
{
    switch (state) {
    case DspTestProtocol::BoardProfileState::BootSafe:
        return QStringLiteral("引导网络未就绪");
    case DspTestProtocol::BoardProfileState::NetworkReady:
        return QStringLiteral("请选择板型");
    case DspTestProtocol::BoardProfileState::SelectedUnconfirmed:
        return QStringLiteral("待人工确认");
    case DspTestProtocol::BoardProfileState::Confirmed:
        return QStringLiteral("板型已确认");
    }

    return QStringLiteral("未知状态");
}

QString boardCapabilityText(quint32 capabilities)
{
    QStringList items;

    if ((capabilities & 0x00000001U) != 0U) items << QStringLiteral("W5300");
    if ((capabilities & 0x00000002U) != 0U) items << QStringLiteral("W5500");
    if ((capabilities & 0x00000004U) != 0U) items << QStringLiteral("SRAM");
    if ((capabilities & 0x00000008U) != 0U) items << QStringLiteral("FPGA-EMIF2");
    if ((capabilities & 0x00000010U) != 0U) items << QStringLiteral("ADC");
    if ((capabilities & 0x00000020U) != 0U) items << QStringLiteral("PWM");
    if ((capabilities & 0x00000040U) != 0U) items << QStringLiteral("DSP-DI");
    if ((capabilities & 0x00000080U) != 0U) items << QStringLiteral("DSP-DO");
    if ((capabilities & 0x00000100U) != 0U) items << QStringLiteral("SCIA");
    if ((capabilities & 0x00000200U) != 0U) items << QStringLiteral("SCIB");
    if ((capabilities & 0x00000400U) != 0U) items << QStringLiteral("CAN-A");
    if ((capabilities & 0x00000800U) != 0U) items << QStringLiteral("CAN-B");
    if ((capabilities & 0x00001000U) != 0U) items << QStringLiteral("SPIA");
    if ((capabilities & 0x00002000U) != 0U) items << QStringLiteral("SPIB Flash/FRAM");
    if ((capabilities & 0x00004000U) != 0U) items << QStringLiteral("SPIC");
    if ((capabilities & 0x00008000U) != 0U) items << QStringLiteral("I2C RTC");
    if ((capabilities & 0x00010000U) != 0U) items << QStringLiteral("I2C TMP116");
    if ((capabilities & 0x00020000U) != 0U) items << QStringLiteral("外部I2C");
    if ((capabilities & 0x00040000U) != 0U) items << QStringLiteral("GPIO");
    if ((capabilities & 0x00080000U) != 0U) items << QStringLiteral("DSP-HDO");
    if ((capabilities & 0x00100000U) != 0U) items << QStringLiteral("FPGA-DIDO");
    if ((capabilities & 0x00200000U) != 0U) items << QStringLiteral("RS422");
    if ((capabilities & 0x00400000U) != 0U) items << QStringLiteral("SPIB");
    if ((capabilities & 0x00800000U) != 0U) items << QStringLiteral("SPIB Flash");
    if ((capabilities & 0x01000000U) != 0U) items << QStringLiteral("SPIB FRAM");
    if ((capabilities & 0x02000000U) != 0U) items << QStringLiteral("STO");
    if ((capabilities & 0x04000000U) != 0U) items << QStringLiteral("模拟量输出");
    if ((capabilities & 0x08000000U) != 0U) items << QStringLiteral("旋变接口");
    if ((capabilities & 0x10000000U) != 0U) items << QStringLiteral("保护与清除");

    return items.isEmpty() ? QStringLiteral("无") : items.join(QStringLiteral("、"));
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);
    auto *topSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    auto *bottomSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    topSplitter->setChildrenCollapsible(false);
    bottomSplitter->setChildrenCollapsible(false);
    topSplitter->setHandleWidth(5);
    bottomSplitter->setHandleWidth(5);

    auto *connectionGroup = new QGroupBox(QStringLiteral("端口与设备连接"), topSplitter);
    auto *connectionLayout = new QGridLayout(connectionGroup);
    m_hostEdit = new QLineEdit(QStringLiteral("192.168.1.111"), connectionGroup);
    m_portEdit = new QLineEdit(QStringLiteral("12001"), connectionGroup);
    m_connectButton = new QPushButton(QStringLiteral("连接"), connectionGroup);
    m_hostEdit->setFixedWidth(220);
    m_portEdit->setFixedWidth(80);
    m_connectButton->setFixedWidth(88);
    connectionLayout->addWidget(new QLabel(QStringLiteral("IP地址"), connectionGroup), 0, 0);
    connectionLayout->addWidget(m_hostEdit, 0, 1);
    connectionLayout->addWidget(new QLabel(QStringLiteral("端口"), connectionGroup), 0, 2);
    connectionLayout->addWidget(m_portEdit, 0, 3);
    connectionLayout->addWidget(m_connectButton, 0, 4);
    m_scibPortBox = new QComboBox(connectionGroup);
    m_refreshScibPortsButton = new QPushButton(QStringLiteral("刷新SCIB端口"), connectionGroup);
    m_scibPortBox->setSizeAdjustPolicy(
        QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_scibPortBox->setMinimumContentsLength(22);
    m_scibPortBox->setFixedWidth(220);
    m_refreshScibPortsButton->setVisible(false);
    connectionLayout->addWidget(new QLabel(QStringLiteral("COM"), connectionGroup), 1, 0);
    connectionLayout->addWidget(m_scibPortBox, 1, 1);
    connectionLayout->addWidget(new QLabel(QStringLiteral("9600"), connectionGroup), 1, 2);
    m_canLibraryEdit = new QLineEdit(connectionGroup);
    m_canLibraryEdit->setReadOnly(true);
    m_canLibraryEdit->setPlaceholderText(QStringLiteral("请选择 ZLG ControlCAN.dll"));
    m_canLibraryEdit->setFixedWidth(220);
    m_selectCanLibraryButton = new QPushButton(QStringLiteral("浏览"), connectionGroup);
    m_selectCanLibraryButton->setFixedWidth(64);
    m_canDeviceTypeBox = new QComboBox(connectionGroup);
    m_canDeviceTypeBox->addItem(QStringLiteral("USBCAN1（ZLG驱动类型）"), 3U);
    m_canDeviceTypeBox->addItem(QStringLiteral("USBCAN2"), 4U);
    m_canDeviceIndexBox = new QComboBox(connectionGroup);
    for (quint32 deviceIndex = 0U; deviceIndex < 4U; ++deviceIndex) {
        m_canDeviceIndexBox->addItem(
            QStringLiteral("设备%1").arg(deviceIndex), deviceIndex);
    }
    m_canChannelBox = new QComboBox(connectionGroup);
    m_canChannelBox->addItem(QStringLiteral("适配器CAN1（500 kbit/s）"), 0U);
    m_canChannelBox->addItem(QStringLiteral("适配器CAN2（500 kbit/s）"), 1U);
    m_canDeviceTypeBox->setToolTip(
        QStringLiteral("ZLG ControlCAN.dll 的设备类型，不代表板卡CAN1/CAN2接口。"));
    m_canChannelBox->setToolTip(
        QStringLiteral("USB-CAN盒子的通道；当前下位机使用DSP CAN-B，板卡接CAN2H/CAN2L。"));
    QSettings settings;
    m_canLibraryEdit->setText(
        settings.value(QStringLiteral("zlgCan/libraryPath")).toString());
    const quint32 canDeviceIndex =
        settings.value(QStringLiteral("zlgCan/deviceIndex"), 0U).toUInt();
    const quint32 canDeviceType =
        settings.value(QStringLiteral("zlgCan/deviceType"), 3U).toUInt();
    const int canDeviceTypeIndex = m_canDeviceTypeBox->findData(canDeviceType);
    if(canDeviceTypeIndex >= 0)
    {
        m_canDeviceTypeBox->setCurrentIndex(canDeviceTypeIndex);
    }
    m_canDeviceIndexBox->setCurrentIndex(
        qMin(static_cast<int>(canDeviceIndex), m_canDeviceIndexBox->count() - 1));
    connectionLayout->addWidget(new QLabel(QStringLiteral("CAN库"), connectionGroup), 2, 0);
    connectionLayout->addWidget(m_canLibraryEdit, 2, 1);
    connectionLayout->addWidget(m_selectCanLibraryButton, 2, 2);
    connectionLayout->addWidget(new QLabel(QStringLiteral("ZLG驱动类型"), connectionGroup), 3, 0);
    connectionLayout->addWidget(m_canDeviceTypeBox, 3, 1);
    connectionLayout->addWidget(new QLabel(QStringLiteral("设备索引"), connectionGroup), 3, 2);
    connectionLayout->addWidget(m_canDeviceIndexBox, 3, 3);
    connectionLayout->addWidget(new QLabel(QStringLiteral("USB-CAN通道"), connectionGroup), 3, 4);
    connectionLayout->addWidget(m_canChannelBox, 3, 5);
    connectionLayout->setColumnStretch(6, 1);

    auto *autoGroup = new QGroupBox(QStringLiteral("一键测试"), topSplitter);
    auto *autoLayout = new QGridLayout(autoGroup);
    m_boardButton = new QPushButton(QStringLiteral("一键板载自检"), autoGroup);
    m_externalButton = new QPushButton(QStringLiteral("外设连接后自检"), autoGroup);
    m_hpdButton = new QPushButton(QStringLiteral("试验台测试"), autoGroup);
    m_hpdSourceBox = new QComboBox(autoGroup);
    m_hpdSourceBox->addItem(QStringLiteral("软件模拟输入"), 0);
    m_hpdSourceBox->addItem(QStringLiteral("真实ADC采样"), 1);
    m_stopButton = new QPushButton(QStringLiteral("停止测试"), autoGroup);
    auto *boardProfileLabel = new QLabel(QStringLiteral("板型"), autoGroup);
    m_boardProfileBox = new QComboBox(autoGroup);
    m_confirmBoardProfileButton = new QPushButton(QStringLiteral("确认板型"), autoGroup);
    m_clearBoardProfileButton = new QPushButton(QStringLiteral("重新选择"), autoGroup);
    m_boardButton->setText(QStringLiteral("核心板测试"));
    m_externalButton->setText(QStringLiteral("外设测试"));
    autoLayout->setContentsMargins(8, 8, 8, 8);
    autoLayout->setHorizontalSpacing(8);
    autoLayout->setVerticalSpacing(4);
    boardProfileLabel->setFixedWidth(32);
    m_boardProfileBox->setMinimumWidth(190);
    m_boardProfileBox->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Fixed);
    m_confirmBoardProfileButton->setFixedWidth(72);
    m_clearBoardProfileButton->setFixedWidth(72);
    autoLayout->addWidget(boardProfileLabel, 0, 0);
    autoLayout->addWidget(m_boardProfileBox, 0, 1);
    autoLayout->addWidget(m_confirmBoardProfileButton, 0, 2);
    autoLayout->addWidget(m_clearBoardProfileButton, 0, 3);
    autoLayout->addWidget(m_boardButton, 1, 0, 1, 2);
    autoLayout->addWidget(m_externalButton, 1, 2, 1, 2);
    autoLayout->addWidget(m_hpdButton, 2, 0, 1, 2);
    autoLayout->addWidget(m_hpdSourceBox, 2, 2, 1, 2);
    autoLayout->addWidget(m_stopButton, 3, 3, Qt::AlignRight);
    autoLayout->setColumnStretch(0, 0);
    autoLayout->setColumnStretch(1, 1);
    autoLayout->setColumnStretch(2, 0);
    autoLayout->setColumnStretch(3, 0);
    m_communicationStatusLabel = new QLabel(
        QStringLiteral("通信待命：未获取"), autoGroup);
    m_communicationStatusLabel->setVisible(false);
    m_stopButton->setFixedWidth(72);

    const auto applyActionStyle = [](QPushButton *button,
                                     const QString &baseColor,
                                     const QString &hoverColor) {
        button->setMinimumHeight(30);
        button->setStyleSheet(QStringLiteral(
            "QPushButton { background-color: %1; color: white; border: 1px solid %1; "
            "border-radius: 4px; padding: 4px 8px; } "
            "QPushButton:hover { background-color: %2; border-color: %2; } "
            "QPushButton:pressed { padding-top: 5px; padding-bottom: 3px; }")
                                    .arg(baseColor, hoverColor));
    };
    applyActionStyle(m_boardButton, QStringLiteral("#1565C0"), QStringLiteral("#0D47A1"));
    applyActionStyle(m_externalButton, QStringLiteral("#1565C0"), QStringLiteral("#0D47A1"));
    applyActionStyle(m_hpdButton, QStringLiteral("#1565C0"), QStringLiteral("#0D47A1"));
    applyActionStyle(m_stopButton, QStringLiteral("#C62828"), QStringLiteral("#8E0000"));
    applyActionStyle(m_confirmBoardProfileButton,
                     QStringLiteral("#1565C0"), QStringLiteral("#0D47A1"));
    applyActionStyle(m_connectButton, QStringLiteral("#455A64"), QStringLiteral("#263238"));

    topSplitter->addWidget(autoGroup);
    topSplitter->addWidget(connectionGroup);
    topSplitter->setStretchFactor(0, 2);
    topSplitter->setStretchFactor(1, 3);
    topSplitter->setSizes({400, 600});
    topSplitter->setMaximumHeight(205);
    mainLayout->addWidget(topSplitter);

    auto *resultPanel = new QWidget(bottomSplitter);
    auto *resultLayout = new QVBoxLayout(resultPanel);
    resultLayout->setContentsMargins(0, 0, 0, 0);
    resultLayout->setSpacing(6);
    auto *singleGroup = new QGroupBox(QStringLiteral("单项测试"), resultPanel);
    auto *singleLayout = new QGridLayout(singleGroup);
    singleLayout->setContentsMargins(6, 6, 6, 6);
    singleLayout->setSpacing(4);
    m_singleTestBox = new FullTextComboBox(singleGroup);
    m_singleStageBox = new QComboBox(singleGroup);
    m_singleStageBox->addItem(QStringLiteral("板载/内部"), 0);
    m_singleStageBox->addItem(QStringLiteral("外设已连接"), 1);
    m_singleStageBox->addItem(QStringLiteral("HPD注入"), 2);
    m_singleButton = new QPushButton(QStringLiteral("开始单项测试"), singleGroup);
    m_singleStageBox->setFixedWidth(110);
    m_singleButton->setFixedWidth(108);
    applyActionStyle(m_singleButton, QStringLiteral("#1565C0"), QStringLiteral("#0D47A1"));
    singleLayout->addWidget(new QLabel(QStringLiteral("测试阶段"), singleGroup), 0, 0);
    singleLayout->addWidget(m_singleStageBox, 0, 1);
    singleLayout->addWidget(new QLabel(QStringLiteral("Test ID"), singleGroup), 0, 2);
    singleLayout->addWidget(m_singleTestBox, 0, 3);
    singleLayout->setColumnStretch(3, 1);
    singleLayout->addWidget(m_singleButton, 0, 5);
    m_adcInjectionPanel = new QWidget(singleGroup);
    auto *adcInjectionLayout = new QHBoxLayout(m_adcInjectionPanel);
    adcInjectionLayout->setContentsMargins(0, 0, 0, 0);
    adcInjectionLayout->setSpacing(6);
    m_adcInjectionChannelBox = new QComboBox(m_adcInjectionPanel);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电流] J_IU1 / X3-20 / ADCD0"), 0);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电流] J_IV1 / X3-46 / ADCC3"), 1);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电流] J_IW1 / X3-22 / ADCB3"), 2);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电流] J_IU2 / X4-20 / ADCA15"), 3);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电流] J_IV2 / X4-46 / ADCB2"), 4);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电流] J_IW2 / X4-22 / ADCC2"), 5);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[母线] J_UDC1 / X3-44 / ADCA14"), 6);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[母线] J_UDC2 / X4-44 / ADCD1"), 7);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电压反馈] J_Uuv1 / X3-48 / ADCD4"), 15);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电压反馈] J_Uvw1 / X3-24 / ADCC4"), 16);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电压反馈] J_Uuv2 / X4-48 / ADCB0"), 17);
    m_adcInjectionChannelBox->addItem(
        QStringLiteral("[电压反馈] J_Uvw2 / X4-24 / ADCA0"), 18);
    m_adcInjectionVoltageBox = new QDoubleSpinBox(m_adcInjectionPanel);
    m_adcInjectionVoltageBox->setRange(0.0, 5.0);
    m_adcInjectionVoltageBox->setDecimals(3);
    m_adcInjectionVoltageBox->setSingleStep(0.1);
    m_adcInjectionVoltageBox->setValue(1.0);
    m_adcInjectionVoltageBox->setSuffix(QStringLiteral(" V"));
    m_adcInjectionToleranceBox = new QDoubleSpinBox(m_adcInjectionPanel);
    m_adcInjectionToleranceBox->setRange(0.01, 0.50);
    m_adcInjectionToleranceBox->setDecimals(3);
    m_adcInjectionToleranceBox->setSingleStep(0.01);
    m_adcInjectionToleranceBox->setValue(0.10);
    m_adcInjectionToleranceBox->setSuffix(QStringLiteral(" V"));
    m_adcInjectionSelectionLabel = new QLabel(
        QStringLiteral("ADC通道"), m_adcInjectionPanel);
    adcInjectionLayout->addWidget(m_adcInjectionSelectionLabel);
    adcInjectionLayout->addWidget(m_adcInjectionChannelBox, 1);
    adcInjectionLayout->addWidget(new QLabel(QStringLiteral("目标值"), m_adcInjectionPanel));
    adcInjectionLayout->addWidget(m_adcInjectionVoltageBox);
    adcInjectionLayout->addWidget(new QLabel(QStringLiteral("容差"), m_adcInjectionPanel));
    adcInjectionLayout->addWidget(m_adcInjectionToleranceBox);
    singleLayout->addWidget(m_adcInjectionPanel, 1, 0, 1, 6);
    m_adcInjectionPanel->setVisible(false);

    m_pwmDidoPanel = new QWidget(singleGroup);
    auto *pwmDidoLayout = new QVBoxLayout(m_pwmDidoPanel);
    pwmDidoLayout->setContentsMargins(0, 0, 0, 0);
    pwmDidoLayout->setSpacing(6);
    auto *pwmDidoSelectionLayout = new QHBoxLayout;
    m_pwmDidoChannelBox = new QComboBox(m_pwmDidoPanel);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第一套 U相上管 / PWM_U1H / GPIO16"), 0);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第一套 U相下管 / PWM_U1L / GPIO17"), 1);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第一套 V相上管 / PWM_V1H / GPIO14"), 2);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第一套 V相下管 / PWM_V1L / GPIO15"), 3);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第一套 W相上管 / PWM_W1H / GPIO12"), 4);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第一套 W相下管 / PWM_W1L / GPIO13"), 5);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第二套 U相上管 / PWM_U2H / GPIO22"), 6);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第二套 U相下管 / PWM_U2L / GPIO23"), 7);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第二套 V相上管 / PWM_V2H / GPIO20"), 8);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第二套 V相下管 / PWM_V2L / GPIO21"), 9);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第二套 W相上管 / PWM_W2H / GPIO18"), 10);
    m_pwmDidoChannelBox->addItem(QStringLiteral("第二套 W相下管 / PWM_W2L / GPIO19"), 11);
    pwmDidoSelectionLayout->addWidget(
        new QLabel(QStringLiteral("PWM通道"), m_pwmDidoPanel));
    pwmDidoSelectionLayout->addWidget(m_pwmDidoChannelBox, 1);
    pwmDidoSelectionLayout->addWidget(new QLabel(
        QStringLiteral("端子：直流档，黑表笔接DGND"), m_pwmDidoPanel));
    pwmDidoLayout->addLayout(pwmDidoSelectionLayout);
    m_pwmDidoLiveValueLabel = new QLabel(
        QStringLiteral("待测试 | 端子：直流档，COM接DGND"),
        m_pwmDidoPanel);
    m_pwmDidoLiveValueLabel->setWordWrap(true);
    m_pwmDidoLiveValueLabel->setObjectName(
        QStringLiteral("pwmDidoLiveValueLabel"));
    m_pwmDidoLiveValueLabel->setStyleSheet(QStringLiteral(
        "QLabel { background: #F5F7FA; border: 1px solid #CDD5DF; "
        "border-radius: 4px; padding: 6px 8px; }"));
    pwmDidoLayout->addWidget(m_pwmDidoLiveValueLabel);
    singleLayout->addWidget(m_pwmDidoPanel, 1, 0, 1, 6);
    m_pwmDidoPanel->setVisible(false);

    m_driverFaultPanel = new QWidget(singleGroup);
    auto *driverFaultLayout = new QVBoxLayout(m_driverFaultPanel);
    driverFaultLayout->setContentsMargins(0, 0, 0, 0);
    driverFaultLayout->setSpacing(3);
    m_driverFaultGroupBox = new QComboBox(m_driverFaultPanel);
    m_driverFaultGroupBox->addItem(
        QStringLiteral("X3／第一套：D_FLT_1U + D_FLT_1D"), 0x0003U);
    m_driverFaultGroupBox->addItem(
        QStringLiteral("X4／第二套：D_FLT_2U + D_FLT_2D"), 0x000CU);
    m_driverFaultStatusLabel = new QLabel(m_driverFaultPanel);
    m_driverFaultStatusLabel->setWordWrap(true);
    driverFaultLayout->addWidget(m_driverFaultGroupBox);
    driverFaultLayout->addWidget(m_driverFaultStatusLabel);
    updateDriverFaultChannelStatus(0x0003U, 0U, false);
    singleLayout->addWidget(m_driverFaultPanel, 1, 0, 1, 6);
    m_driverFaultPanel->setVisible(false);
    connect(m_driverFaultGroupBox, &QComboBox::currentIndexChanged, this,
            [this] {
                updateDriverFaultChannelStatus(
                    static_cast<quint16>(
                        m_driverFaultGroupBox->currentData().toUInt()),
                    0U,
                    false);
            });

    m_protectionPanel = new QWidget(singleGroup);
    auto *protectionLayout = new QVBoxLayout(m_protectionPanel);
    protectionLayout->setContentsMargins(0, 0, 0, 0);
    protectionLayout->setSpacing(3);
    m_protectionSelectionBox = new FullTextComboBox(m_protectionPanel);
    m_protectionSelectionBox->setObjectName(QStringLiteral("protectionSelectionBox"));
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流高端：J_IU1 / X3-20 / 2.5V -> 4.9V / D_OC1"), 0U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流高端：J_IV1 / X3-46 / 2.5V -> 4.9V / D_OC1"), 1U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流高端：J_IW1 / X3-22 / 2.5V -> 4.9V / D_OC1"), 2U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流高端：J_IU2 / X4-20 / 2.5V -> 4.9V / D_OC2"), 3U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流高端：J_IV2 / X4-46 / 2.5V -> 4.9V / D_OC2"), 4U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流高端：J_IW2 / X4-22 / 2.5V -> 4.9V / D_OC2"), 5U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流低端：J_IU1 / X3-20 / 2.5V -> 0.1V / D_OC1"), 6U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流低端：J_IV1 / X3-46 / 2.5V -> 0.1V / D_OC1"), 7U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流低端：J_IW1 / X3-22 / 2.5V -> 0.1V / D_OC1"), 8U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流低端：J_IU2 / X4-20 / 2.5V -> 0.1V / D_OC2"), 9U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流低端：J_IV2 / X4-46 / 2.5V -> 0.1V / D_OC2"), 10U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过流低端：J_IW2 / X4-22 / 2.5V -> 0.1V / D_OC2"), 11U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过压：J_UDC1 / X3-44 / 3.0V -> 4.0V / D_UOV1"), 12U);
    m_protectionSelectionBox->addItem(
        QStringLiteral("过压：J_UDC2 / X4-44 / 3.0V -> 4.0V / D_UOV2"), 13U);
    m_protectionStatusLabel = new QLabel(
        QStringLiteral("待测试 | COM接同组AGND | 低电平=故障"),
        m_protectionPanel);
    m_protectionStatusLabel->setWordWrap(true);
    m_protectionStatusLabel->setStyleSheet(QStringLiteral(
        "QLabel { background: #F5F7FA; border: 1px solid #CDD5DF; "
        "border-radius: 4px; padding: 6px 8px; }"));
    protectionLayout->addWidget(m_protectionSelectionBox);
    protectionLayout->addWidget(m_protectionStatusLabel);
    singleLayout->addWidget(m_protectionPanel, 1, 0, 1, 6);
    m_protectionPanel->setVisible(false);

    m_driverResetPanel = new QWidget(singleGroup);
    auto *driverResetLayout = new QVBoxLayout(m_driverResetPanel);
    driverResetLayout->setContentsMargins(0, 0, 0, 0);
    driverResetLayout->setSpacing(6);
    auto *driverResetControls = new QHBoxLayout();
    m_driverResetChannelBox = new FullTextComboBox(m_driverResetPanel);
    m_driverResetChannelBox->setObjectName(QStringLiteral("driverResetChannelBox"));
    m_driverResetChannelBox->addItem(
        QStringLiteral("第一套上管复位 / D_URST1 / GPIO80"), 0U);
    m_driverResetChannelBox->addItem(
        QStringLiteral("第一套下管复位 / D_DRST1 / GPIO81"), 1U);
    m_driverResetChannelBox->addItem(
        QStringLiteral("第二套上管复位 / D_URST2 / GPIO78"), 2U);
    m_driverResetChannelBox->addItem(
        QStringLiteral("第二套下管复位 / D_DRST2 / GPIO79"), 3U);
    driverResetControls->addWidget(new QLabel(QStringLiteral("复位通道"), m_driverResetPanel));
    driverResetControls->addWidget(m_driverResetChannelBox, 1);
    driverResetControls->addWidget(new QLabel(
        QStringLiteral("仅万用表观察，不得外部注入5V"), m_driverResetPanel));
    driverResetLayout->addLayout(driverResetControls);
    m_driverResetLiveValueLabel = new QLabel(
        QStringLiteral("复位GPIO实时值：待测试（数字逻辑折算，不是端子电压实测值）"),
        m_driverResetPanel);
    m_driverResetLiveValueLabel->setWordWrap(true);
    driverResetLayout->addWidget(m_driverResetLiveValueLabel);
    singleLayout->addWidget(m_driverResetPanel, 1, 0, 1, 6);
    m_driverResetPanel->setVisible(false);

    const auto createRecordsTable = [resultPanel] {
        auto *table = new QTableWidget(0, 8, resultPanel);
        table->setHorizontalHeaderLabels({
            QStringLiteral("Test ID"), QStringLiteral("名称"), QStringLiteral("错误码"),
            QStringLiteral("原始值"), QStringLiteral("测量值"), QStringLiteral("最小值"),
            QStringLiteral("最大值"), QStringLiteral("结果")});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        table->setColumnWidth(1, 160);
        table->setAlternatingRowColors(false);
        return table;
    };
    m_resultTabs = new QTabWidget(resultPanel);
    m_boardRecordsTable = createRecordsTable();
    m_externalRecordsTable = createRecordsTable();
    m_hpdRecordsTable = createRecordsTable();
    m_resultTabs->addTab(m_boardRecordsTable, QStringLiteral("板载自检结果"));
    m_resultTabs->addTab(m_externalRecordsTable, QStringLiteral("外设连接后结果"));
    m_resultTabs->addTab(m_hpdRecordsTable, QStringLiteral("HPD注入结果"));
    m_resultTabs->setTabPosition(QTabWidget::North);
    m_resultTabs->setTabText(0, QStringLiteral("核心板测试结果"));
    m_resultTabs->setTabText(1, QStringLiteral("外设测试结果"));
    resultLayout->addWidget(singleGroup);
    resultLayout->addWidget(m_resultTabs, 1);

    auto *logGroup = new QGroupBox(QStringLiteral("运行日志"), bottomSplitter);
    auto *logLayout = new QVBoxLayout(logGroup);
    auto *logToolLayout = new QHBoxLayout;
    m_exportLogButton = new QPushButton(QStringLiteral("导出日志"), logGroup);
    logToolLayout->addStretch(1);
    logToolLayout->addWidget(m_exportLogButton);
    logLayout->addLayout(logToolLayout);
    m_log = new QPlainTextEdit(logGroup);
    m_log->setReadOnly(true);
    m_log->setMaximumBlockCount(500);
    m_log->setMinimumHeight(0);
    logLayout->addWidget(m_log);
    bottomSplitter->addWidget(resultPanel);
    bottomSplitter->addWidget(logGroup);
    bottomSplitter->setStretchFactor(0, 11);
    bottomSplitter->setStretchFactor(1, 9);
    bottomSplitter->setSizes({550, 450});
    mainLayout->addWidget(bottomSplitter, 1);
    setCentralWidget(centralWidget);
    setWindowTitle(QStringLiteral("HPD控制模组测试上位机"));
    const QRect availableGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    resize(qMin(1000, qMax(780, availableGeometry.width() - 40)),
           qMin(850, qMax(600, availableGeometry.height() - 60)));

    addTest(0x0100, QStringLiteral("SYS_STARTUP / 启动配置"), RefreshScope::Board);
    addTest(0x0101, QStringLiteral("SYS_CLOCK / 时钟配置"), RefreshScope::Board);
    addTest(0x0102, QStringLiteral("SYS_INTERRUPT / PIE配置"), RefreshScope::Board);
    addTest(0x0103, QStringLiteral("SYS_TIMER"), RefreshScope::Board);
    addTest(0x0104, QStringLiteral("SYS_WATCHDOG / 禁用状态"), RefreshScope::Board);
    addTest(0x0105, QStringLiteral("RAM / 专用测试区"), RefreshScope::Board);
    addTest(0x0106, QStringLiteral("FLASH / 配置检查"), RefreshScope::Board);
    addTest(0x0200, QStringLiteral("GPIO"), RefreshScope::Board);
    addTest(0x0201, QStringLiteral("ADC_BASIC"), RefreshScope::Board);
    addTest(0x0202, QStringLiteral("PWM_BASIC"), RefreshScope::Board);
    addTest(0x0203, QStringLiteral("EMIF_BASIC"), RefreshScope::Board);
    addTest(0x0300, QStringLiteral("CAN_INTERNAL"), RefreshScope::Board);
    addTest(0x0301, QStringLiteral("SCI_INTERNAL"), RefreshScope::Board);
    addTest(0x0303, QStringLiteral("I2C_RTC"), RefreshScope::Board);
    addTest(0x0306, QStringLiteral("I2C_TMP116"), RefreshScope::Board);
    addTest(0x030A, QStringLiteral("SPIA_INTERNAL"), RefreshScope::Board);
    addTest(0x030B, QStringLiteral("SPIB_INTERNAL"), RefreshScope::Board);
    addTest(0x030C, QStringLiteral("SPIC_INTERNAL"), RefreshScope::Board);
    addTest(0x0312, QStringLiteral("FPGA_EMIF2_BASIC"), RefreshScope::Board);

    addTest(0x0302, QStringLiteral("SPIC_EXTERNAL"), RefreshScope::External);
    addTest(0x0317, QStringLiteral("SPIB_FLASH_EXTERNAL"), RefreshScope::External);
    addTest(0x0318, QStringLiteral("SPIB_FRAM_EXTERNAL"), RefreshScope::External);
    addTest(0x0304, QStringLiteral("EMIF_SRAM_EXTERNAL"), RefreshScope::External);
    addTest(TestEthernetBasic, QStringLiteral("ETHERNET / BASIC"), RefreshScope::External);
    addTest(TestEthernetSocket, QStringLiteral("ETHERNET / SOCKET"), RefreshScope::External);
    addTest(TestEthernetTcpLink, QStringLiteral("ETHERNET / TCP_LINK"), RefreshScope::External);
    addTest(TestEthernetTcpEcho, QStringLiteral("ETHERNET / TCP_ECHO"), RefreshScope::External);
    addTest(TestEthernetTcpStability, QStringLiteral("ETHERNET / TCP_STABILITY"), RefreshScope::External);
    addTest(0x0307, QStringLiteral("CAN_EXTERNAL"), RefreshScope::External);
    addTest(0x0308, QStringLiteral("I2C_EXTERNAL"), RefreshScope::External);
    addTest(0x0309, QStringLiteral("SCI_RS485_EXTERNAL"), RefreshScope::External);
    addTest(TestRs422External, QStringLiteral("RS422_EXTERNAL"), RefreshScope::External);
    addTest(0x0311, QStringLiteral("SCIA_HANDHELD_EXTERNAL"), RefreshScope::External);
    addTest(TestDido, QStringLiteral("DO_EXTERNAL"), RefreshScope::External);
    addTest(TestHdo, QStringLiteral("HDO_EXTERNAL"), RefreshScope::External);
    addTest(TestDi, QStringLiteral("DI_EXTERNAL"), RefreshScope::External);
    addTest(TestDriverFaultDi,
            QStringLiteral("DRIVER_FAULT_DI_EXTERNAL"),
            RefreshScope::External);
    addTest(TestProtectionFaultDi,
            QStringLiteral("PROTECTION_THRESHOLD_XINT_CLEAR"),
            RefreshScope::External);
    addTest(TestDriverResetDo,
            QStringLiteral("DRIVER_RESET_DO_EXTERNAL"),
            RefreshScope::External);
    addTest(TestPwmDido,
            QStringLiteral("PWM_DIDO_EXTERNAL"),
            RefreshScope::External);
    addTest(TestAdcExternalInjection,
            QStringLiteral("ADC_EXTERNAL_SINGLE"),
            RefreshScope::External);
    addTest(TestAdcExternalGroup1,
            QStringLiteral("ADC_EXTERNAL_STEP1_X3"),
            RefreshScope::External);
    addTest(TestAdcExternalGroup2,
            QStringLiteral("ADC_EXTERNAL_STEP2_X4"),
            RefreshScope::External);
    addTest(TestAdcPt100Mux,
            QStringLiteral("ADC_PT100_MUX_EXTERNAL"),
            RefreshScope::External);
    addTest(TestLowVoltageDi,
            QStringLiteral("低压板 / 6路DI"),
            RefreshScope::External);
    addTest(TestLowVoltageDo,
            QStringLiteral("低压板 / 3路DO+软启动+风扇"),
            RefreshScope::External);
    addTest(TestLowVoltageHdo,
            QStringLiteral("低压板 / HDO"),
            RefreshScope::External);
    addTest(TestLowVoltageSto,
            QStringLiteral("低压板 / 2路STO"),
            RefreshScope::External);
    addTest(TestLowVoltageAdc,
            QStringLiteral("低压板 / 13路ADC"),
            RefreshScope::External);
    addTest(TestLowVoltagePt100,
            QStringLiteral("低压板 / PT100温度"),
            RefreshScope::External);
    addTest(TestLowVoltageNtc,
            QStringLiteral("低压板 / NTC温度"),
            RefreshScope::External);
    addTest(TestLowVoltagePwm,
            QStringLiteral("低压板 / 6路PWM"),
            RefreshScope::External);
    addTest(TestLowVoltageProtection,
            QStringLiteral("低压板 / 故障中断与清除"),
            RefreshScope::External);
    addTest(TestLowVoltageAo,
            QStringLiteral("低压板 / 模拟量输出"),
            RefreshScope::External);
    addTest(TestLowVoltageResolver,
            QStringLiteral("低压板 / 旋变接口"),
            RefreshScope::External);
    addTest(0x1000, QStringLiteral("HPD_ESTOP_DI3"), RefreshScope::External);
    addTest(0x1001, QStringLiteral("HPD_INDICATOR_DO3"), RefreshScope::External);

    addTest(0x1100, QStringLiteral("HPD_DRIVER_1"), RefreshScope::Hpd);
    addTest(0x1101, QStringLiteral("HPD_DRIVER_2"), RefreshScope::Hpd);
    addTest(0x1200, QStringLiteral("HPD_NTC_1"), RefreshScope::Hpd);
    addTest(0x1201, QStringLiteral("HPD_NTC_2"), RefreshScope::Hpd);
    addTest(0x1300, QStringLiteral("HPD_BUS_1"), RefreshScope::Hpd);
    addTest(0x1301, QStringLiteral("HPD_BUS_2"), RefreshScope::Hpd);
    addTest(0x1400, QStringLiteral("HPD_LINE_1"), RefreshScope::Hpd);
    addTest(0x1401, QStringLiteral("HPD_LINE_2"), RefreshScope::Hpd);
    addTest(0x1500, QStringLiteral("HPD_CURRENT_1"), RefreshScope::Hpd);
    addTest(0x1501, QStringLiteral("HPD_CURRENT_2"), RefreshScope::Hpd);
    addTest(0x1600, QStringLiteral("HPD_PHASE_1"), RefreshScope::Hpd);
    addTest(0x1601, QStringLiteral("HPD_PHASE_2"), RefreshScope::Hpd);
    updateSingleTestChoices(RefreshScope::Board);
    refreshScibPorts();
    updateTestCommandAvailability();

    connect(m_connectButton, &QPushButton::clicked, this, [this] {
        if (m_client.isConnected()) {
            m_client.disconnectFromDevice();
            return;
        }
        refreshScibPorts();
        m_client.connectToDevice(m_hostEdit->text().trimmed(),
                                 m_portEdit->text().toUShort());
    });
    connect(m_refreshScibPortsButton, &QPushButton::clicked,
            this, &MainWindow::refreshScibPorts);
    connect(m_selectCanLibraryButton, &QPushButton::clicked,
            this, &MainWindow::selectCanLibrary);
    connect(m_exportLogButton, &QPushButton::clicked,
            this, &MainWindow::exportLog);
    connect(m_boardProfileBox, &QComboBox::currentIndexChanged,
            this, [this](int) { updateTestCommandAvailability(); });
    connect(m_confirmBoardProfileButton, &QPushButton::clicked, this, [this] {
        const quint16 profileId =
            static_cast<quint16>(m_boardProfileBox->currentData().toUInt());

        if(profileId == 0U)
        {
            return;
        }
        if(QMessageBox::question(
               this,
               QStringLiteral("确认板型"),
               QStringLiteral("请确认当前实际连接的是“%1”。\n"
                              "DSP已识别网络硬件为“%2”，当前选项必须匹配已登记的板型和硬件版本。\n"
                              "确认前DSP保持安全锁定，确认后才允许执行测试。")
                   .arg(boardProfileName(profileId),
                        ethernetInterfaceText(m_detectedEthernetInterface)),
               QMessageBox::Yes | QMessageBox::No,
               QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }

        if(m_boardProfileState ==
           DspTestProtocol::BoardProfileState::SelectedUnconfirmed)
        {
            m_client.confirmBoardProfile();
        }
        else
        {
            m_client.selectBoardProfile(profileId);
            m_client.confirmBoardProfile();
        }
    });
    connect(m_clearBoardProfileButton, &QPushButton::clicked, this, [this] {
        if(QMessageBox::question(
               this,
               QStringLiteral("重新选择板型"),
               QStringLiteral("将停止测试、关闭测试输出并清除当前板型确认。\n"
                              "重新选择后，必须再次人工确认板型。"),
               QMessageBox::Yes | QMessageBox::No,
               QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
        m_client.clearBoardProfile();
    });
    connect(m_canDeviceIndexBox, &QComboBox::currentIndexChanged, this, [this](int) {
        QSettings().setValue(QStringLiteral("zlgCan/deviceIndex"),
                             m_canDeviceIndexBox->currentData().toUInt());
    });
    connect(m_canDeviceTypeBox, &QComboBox::currentIndexChanged, this, [this](int) {
        QSettings().setValue(QStringLiteral("zlgCan/deviceType"),
                             m_canDeviceTypeBox->currentData().toUInt());
    });
    connect(m_boardButton, &QPushButton::clicked, this, [this] {
        resetEthernetTcpSequence();
        m_scibAutomaticTestRequested = false;
        m_scibAutomaticTestStarted = false;
        m_scibSingleTestRequested = false;
        m_scibSingleTestStarted = false;
        m_canAutomaticTestRequested = false;
        m_canAutomaticTestStarted = false;
        m_canSingleTestRequested = false;
        m_canSingleTestStarted = false;
        m_resultTabs->setCurrentWidget(m_boardRecordsTable);
        prepareResultRefresh(RefreshScope::Board);
        m_client.startBoardAuto();
    });
    connect(m_externalButton, &QPushButton::clicked, this, [this] {
        const bool edoWillRun = isTestRunnable(TestDido) &&
                                isTestEnabledInAuto(TestDido);
        if (edoWillRun) {
            const bool lowAltitudeBoard = m_boardProfileId == 0x0003U;
            const QString safetyText = lowAltitudeBoard ?
                QStringLiteral("外设测试将检测 EX_DI1-3（D_DI1-3）并依次驱动"
                               " EX_DOP1-3（D_DO1-3）。"
                               "请确认 PLC DO1/DO2/DO3 已分别连接 EX_DI1/EX_DI2/EX_DI3，"
                               "且 PLC 输出程序按约1秒周期翻转。") :
                QStringLiteral("外设自检将依次驱动 EDO1-3。请确认 PLC 接线和设备状态安全。");
            if (QMessageBox::question(
                    this,
                    QStringLiteral("确认外设自检"),
                    safetyText) !=
                QMessageBox::Yes) {
                return;
            }
            const QString observationText = lowAltitudeBoard ?
                QStringLiteral("测试开始后，请观察与 EX_DOP1、EX_DOP2、EX_DOP3 相连的"
                               "PLC DI 指示灯。三路将依次点亮约2秒后熄灭。"
                               "确认已准备观察后，点击“确定”开始外设测试。") :
                QStringLiteral("测试开始后，请观察 PLC 的 DI3、DI4、DI5 指示灯。"
                               "EDO1、EDO2、EDO3 将依次点亮约2秒后熄灭。"
                               "确认已准备观察后，点击“确定”开始外设自检。");
            if (QMessageBox::information(
                    this,
                    QStringLiteral("DO目视确认准备"),
                    observationText,
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Ok) != QMessageBox::Ok) {
                return;
            }
        } else {
            appendLog(LogChannel::External,
                      QStringLiteral("当前板型的DO不在外设自动队列中，跳过DO目视确认。"));
        }
        m_resultTabs->setCurrentWidget(m_externalRecordsTable);
        resetEthernetTcpSequence();
        m_ethernetTcpExternalAuto = true;
        m_edoManualConfirmationExpected = edoWillRun;
        m_edoManualConfirmationAvailable = false;
        m_scibAutomaticTestRequested =
            !m_scibPortBox->currentData().toString().isEmpty();
        m_scibAutomaticTestStarted = false;
        m_scibSingleTestRequested = false;
        m_scibSingleTestStarted = false;
        m_canAutomaticTestRequested = !m_canLibraryEdit->text().isEmpty();
        if(m_canAutomaticTestRequested)
        {
            QString canLibraryError;
            if(!m_zlgCanClient.probeDevice(
                   m_canLibraryEdit->text(),
                   m_canDeviceTypeBox->currentData().toUInt(),
                   m_canDeviceIndexBox->currentData().toUInt(),
                   &canLibraryError))
            {
                m_canAutomaticTestRequested = false;
                appendLog(LogChannel::External,
                          QStringLiteral("CAN 自动测试未启用：%1")
                              .arg(canLibraryError));
            }
        }
        m_canAutomaticTestStarted = false;
        m_canSingleTestRequested = false;
        m_canSingleTestStarted = false;
        if (!m_scibAutomaticTestRequested) {
            appendLog(LogChannel::External,
                      QStringLiteral("未选择 COM 口：串行外设将保持待命，可连接后执行单项测试。"));
        }
        if (!m_canAutomaticTestRequested) {
            appendLog(LogChannel::External,
                      QStringLiteral("未选择 ZLG ControlCAN.dll：CAN 将保持待命，可稍后使用单项测试。"));
        }
        prepareResultRefresh(RefreshScope::External);
        m_client.startExternalAuto();
    });
    connect(m_hpdButton, &QPushButton::clicked, this, [this] {
        resetEthernetTcpSequence();
        m_scibAutomaticTestRequested = false;
        m_scibAutomaticTestStarted = false;
        m_scibSingleTestRequested = false;
        m_scibSingleTestStarted = false;
        m_canAutomaticTestRequested = false;
        m_canAutomaticTestStarted = false;
        m_canSingleTestRequested = false;
        m_canSingleTestStarted = false;
        m_resultTabs->setCurrentWidget(m_hpdRecordsTable);
        prepareResultRefresh(RefreshScope::Hpd);
        m_client.startHpdAuto(m_hpdSourceBox->currentData().toInt() != 0);
    });
    connect(m_stopButton, &QPushButton::clicked, this, [this] {
        resetEthernetTcpSequence();
        m_pwmDidoLiveTimer->stop();
        m_protectionResetLiveTimer->stop();
        m_liveRecordRequestPending = false;
        m_pwmDidoLastLiveLevel = -1;
        m_driverResetLastLiveLevel = -1;
        m_pwmDidoLiveValueLabel->setText(
            QStringLiteral("已停止 | GPIO 低 | 输出已关闭"));
        m_protectionStatusLabel->setText(QStringLiteral("已停止"));
        m_driverResetLiveValueLabel->setText(
            QStringLiteral("复位GPIO实时值：测试已停止，输出恢复为低"));
        m_refreshScope = RefreshScope::None;
        m_waitingForTestReconnect = false;
        m_singleTestRefreshPending = false;
        m_singleTestStartStatusRequested = false;
        m_singleTestObservedRunning = false;
        m_singleTestRecordRefreshInFlight = false;
        m_singleTestRefreshScope = RefreshScope::None;
        m_singleTestRefreshId = DspTestProtocol::InvalidTestId;
        m_edoManualConfirmationExpected = false;
        m_edoManualConfirmationAvailable = false;
        m_scibAutomaticTestRequested = false;
        m_scibAutomaticTestStarted = false;
        m_scibSingleTestRequested = false;
        m_scibSingleTestStarted = false;
        m_canAutomaticTestRequested = false;
        m_canAutomaticTestStarted = false;
        m_canSingleTestRequested = false;
        m_canSingleTestStarted = false;
        m_scibSerialClient.cancel();
        m_zlgCanClient.cancel();
        m_client.stop();
    });
    connect(m_singleStageBox, &QComboBox::currentIndexChanged, this, [this](int) {
        switch (static_cast<DspTestProtocol::Stage>(m_singleStageBox->currentData().toUInt())) {
        case DspTestProtocol::Stage::BoardOnly:
            updateSingleTestChoices(RefreshScope::Board);
            break;
        case DspTestProtocol::Stage::ExternalConnected:
            updateSingleTestChoices(RefreshScope::External);
            break;
        case DspTestProtocol::Stage::HpdInjection:
            updateSingleTestChoices(RefreshScope::Hpd);
            break;
        }
        updateSingleTestParameterControls();
    });
    connect(m_singleTestBox, &QComboBox::currentIndexChanged,
            this, [this](int) { updateSingleTestParameterControls(); });
    connect(m_singleButton, &QPushButton::clicked, this, [this] {
        const quint16 testId = static_cast<quint16>(m_singleTestBox->currentData().toUInt());
        const auto stage = static_cast<DspTestProtocol::Stage>(
            m_singleStageBox->currentData().toUInt());
        const bool isOutput = selectedTestIsOutput();
        if ((testId == TestDi) &&
            (m_boardProfileId == 0x0003U) &&
            (QMessageBox::information(
                 this,
                 QStringLiteral("确认DI输入"),
                 QStringLiteral("请确认 PLC DO1/DO2/DO3 已分别连接低空板"
                                " EX_DI1/EX_DI2/EX_DI3（D_DI1/2/3），"
                                "并按约1秒周期翻转。"),
                 QMessageBox::Ok | QMessageBox::Cancel,
                 QMessageBox::Ok) != QMessageBox::Ok)) {
            return;
        }
        if ((testId == TestDriverFaultDi) &&
            (m_boardProfileId == 0x0003U) &&
            (QMessageBox::warning(
                 this,
                 QStringLiteral("确认驱动故障DI接线"),
                 QStringLiteral("当前选择：%1。\n"
                                "信号发生器COM接DGND，OUT同时接所选组的两路板端5V输入。"
                                "点击确定后，将注入电压从0V切到5V（或反向切换），"
                                "并保持至结果返回。\n\n"
                                "只监测所选的X3或X4组；另一侧不会影响结果。所选组两路"
                                "分别显示PASS或未通过，只有两路都完成高、低电平和翻转"
                                "才判本组PASS；无变化最多等待30秒。\n\n"
                                "换线前先停止输出，再开始下一次单项测试。禁止输入24V、"
                                "禁止直接向DSP GPIO注入5V，也不要跨X3/X4并接。"
                                "未来PLC的24V输出必须经适配电路转换后使用。")
                     .arg(m_driverFaultGroupBox->currentText()),
                 QMessageBox::Ok | QMessageBox::Cancel,
                 QMessageBox::Cancel) != QMessageBox::Ok)) {
            return;
        }
        if ((testId == TestProtectionFaultDi) &&
            (m_boardProfileId == 0x0003U) &&
            (QMessageBox::warning(
                 this,
                 QStringLiteral("确认过流/过压保护测试"),
                 QStringLiteral("当前选择：\n%1\n\n"
                                "1. 信号发生器使用电压模式，OUT只接当前所选X3/X4模拟量端子，"
                                "COM接同一组的AGND。\n"
                                "2. 启动后有30秒用于设置正常值；界面进入“等待故障触发”后，"
                                "另有30秒用于切到故障值。ADC达到阈值后，硬件保护链有10秒响应时间。\n"
                                "3. DSP将依次核对ADC阈值、对应低有效故障位、D_BRK上升沿外部中断，"
                                "恢复正常后自动脉冲D_ERR_CLR并检查故障解除。\n\n"
                                "过流高端使用4.9V，不要超过5.0V；禁止向D_OC、D_UOV、D_BRK或"
                                "D_ERR_CLR等DSP数字管脚直接注入电压。")
                     .arg(m_protectionSelectionBox->currentText()),
                 QMessageBox::Ok | QMessageBox::Cancel,
                 QMessageBox::Cancel) != QMessageBox::Ok)) {
            return;
        }
        if ((testId == TestDriverResetDo) &&
            (m_boardProfileId == 0x0003U) &&
            (QMessageBox::warning(
                 this,
                 QStringLiteral("确认驱动复位输出测试"),
                 QStringLiteral("当前选择：\n%1\n\n"
                                "D_URST/D_DRST是高电平有效输出，不是DI。不得用信号发生器"
                                "向该端子注入5V。\n"
                                "请断开主功率母线和驱动功率供电，万用表使用直流电压档，"
                                "黑表笔接DGND、红表笔接当前复位端子。测试期间该一路输出"
                                "高电平约10秒，然后恢复低电平；其他三路始终保持低电平。")
                     .arg(m_driverResetChannelBox->currentText()),
                 QMessageBox::Ok | QMessageBox::Cancel,
                 QMessageBox::Cancel) != QMessageBox::Ok)) {
            return;
        }
        if ((testId == TestPwmDido) &&
            (QMessageBox::warning(
                 this,
                 QStringLiteral("确认PWM-DIDO接线"),
                 QStringLiteral("本测试不输出PWM波形，只测试当前选择的一路：\n%1\n\n"
                                "万用表使用直流电压档，黑表笔接DGND，红表笔接当前PWM端子。"
                                "S7-1200为24V DI，必须经过5V转24V隔离模块后才能连接。"
                                "禁止把多路输出并接，也不要向PWM输出脚注入5V。\n"
                                "当前一路输出约2秒高，随后回到低电平；输出期间"
                                "同步打开当前绕组的U/D高电平有效PWM_EN，四路POW_EN始终保持"
                                "安全低电平。")
                     .arg(m_pwmDidoChannelBox->currentText()),
                 QMessageBox::Ok | QMessageBox::Cancel,
                 QMessageBox::Cancel) != QMessageBox::Ok)) {
            return;
        }
        if (isOutput &&
            (testId != TestPwmDido) &&
            (testId != TestDriverResetDo) &&
            QMessageBox::question(
                            this,
                            QStringLiteral("确认DO输出"),
                            (m_boardProfileId == 0x0003U) ?
                                QStringLiteral("将依次驱动 EX_DOP1/EX_DOP2/EX_DOP3"
                                               "（D_DO1/2/3）。"
                                               "请确认已连接 PLC DI 且接线安全后继续。") :
                                QStringLiteral("将驱动外部DO。请确认接线无误后继续。")) != QMessageBox::Yes) {
            return;
        }
        m_scibAutomaticTestRequested = false;
        m_scibAutomaticTestStarted = false;
        m_scibSingleTestRequested =
            isSerialExternalTest(testId) &&
            (stage == DspTestProtocol::Stage::ExternalConnected) &&
            !m_scibPortBox->currentData().toString().isEmpty();
        m_scibSingleTestStarted = false;
        m_canAutomaticTestRequested = false;
        m_canAutomaticTestStarted = false;
        m_canSingleTestRequested =
            (testId == TestCanExternal) &&
            (stage == DspTestProtocol::Stage::ExternalConnected) &&
            !m_canLibraryEdit->text().isEmpty();
        m_canSingleTestStarted = false;
        if(m_canSingleTestRequested)
        {
            QString canLibraryError;
            if(!m_zlgCanClient.probeDevice(
                   m_canLibraryEdit->text(),
                   m_canDeviceTypeBox->currentData().toUInt(),
                   m_canDeviceIndexBox->currentData().toUInt(),
                   &canLibraryError))
            {
                m_canSingleTestRequested = false;
                appendLog(LogChannel::Communication,
                          QStringLiteral("CAN 单项测试未启动：%1")
                              .arg(canLibraryError));
                QMessageBox::warning(this,
                                     QStringLiteral("ZLG CAN 接口库"),
                                     canLibraryError);
                return;
            }
        }
        if (isSerialExternalTest(testId) &&
            (stage == DspTestProtocol::Stage::ExternalConnected) &&
            !m_scibSingleTestRequested) {
            appendLog(LogChannel::Communication,
                      QStringLiteral("未选择 COM 口：单项串行测试将等待外部工具发送 0xA5。"));
        }
        if ((testId == TestCanExternal) &&
            (stage == DspTestProtocol::Stage::ExternalConnected) &&
            !m_canSingleTestRequested) {
            appendLog(LogChannel::Communication,
                      QStringLiteral("未选择 ZLG ControlCAN.dll：单项 CAN 将等待外部 CAN 工具发送 ID=0x0321。"));
        }
        RefreshScope singleScope = RefreshScope::None;
        switch (stage) {
        case DspTestProtocol::Stage::BoardOnly:
            singleScope = RefreshScope::Board;
            break;
        case DspTestProtocol::Stage::ExternalConnected:
            singleScope = RefreshScope::External;
            break;
        case DspTestProtocol::Stage::HpdInjection:
            singleScope = RefreshScope::Hpd;
            break;
        }
        prepareSingleTestRefresh(singleScope, testId);
        if((stage == DspTestProtocol::Stage::ExternalConnected) &&
           (testId == TestEthernetBasic)) {
            resetEthernetTcpSequence();
            startEthernetTcpSequence(false, true);
            return;
        }
        if((stage == DspTestProtocol::Stage::ExternalConnected) &&
           ((testId == TestAdcExternalInjection) ||
            (testId == TestAdcExternalGroup1) ||
             (testId == TestAdcExternalGroup2) ||
               (testId == TestAdcPt100Mux))) {
            const bool isPt4ResistanceTest = testId == TestAdcPt100Mux;
            const quint16 expectedMillivolts = isPt4ResistanceTest
                ? Pt4TestExpectedMillivolts
                : static_cast<quint16>(
                      (m_adcInjectionVoltageBox->value() * 1000.0) + 0.5);
            const quint16 toleranceMillivolts = isPt4ResistanceTest
                ? Pt4TestToleranceMillivolts
                : static_cast<quint16>(
                      (m_adcInjectionToleranceBox->value() * 1000.0) + 0.5);
            quint8 selectionId = static_cast<quint8>(
                m_adcInjectionChannelBox->currentData().toUInt());
            QString selectionText = m_adcInjectionChannelBox->currentText();
            QString wiringText = QStringLiteral(
                "OUT接当前所选端子，COM接相邻AGND，每次只连接并测试一路。");
            if(testId == TestAdcExternalGroup1) {
                selectionId = AdcExternalSelectionGroup1;
                selectionText = QStringLiteral("步骤1：X3／1组七路");
                wiringText = QStringLiteral(
                    "OUT同时接J_IU1(X3-20)、J_IV1(X3-46)、J_IW1(X3-22)、"
                    "J_UDC1(X3-44)、AD_T_INV1对应的X3输入端子、J_Uuv1(X3-48)、"
                    "J_Uvw1(X3-24)，COM接X3的AGND。请勿连接X4组。");
            } else if(testId == TestAdcExternalGroup2) {
                selectionId = AdcExternalSelectionGroup2;
                selectionText = QStringLiteral("步骤2：X4／2组七路");
                wiringText = QStringLiteral(
                    "OUT同时接J_IU2(X4-20)、J_IV2(X4-46)、J_IW2(X4-22)、"
                    "J_UDC2(X4-44)、AD_T_INV2对应的X4输入端子、J_Uuv2(X4-48)、"
                    "J_Uvw2(X4-24)，COM接X4的AGND。请勿连接X3组。");
            } else if(testId == TestAdcPt100Mux) {
                wiringText = QStringLiteral(
                    "在X8的PT4+与PT4-之间接120Ω电阻，不要接信号发生器。"
                    "GPIO24/25固定输出S0/S1=11，读取AD_T_DJ(ADCINA3)；"
                    "按板卡公式计算，120Ω的目标电压为1.688V。");
            }
            const QString injectionAdvice = isPt4ResistanceTest
                ? QStringLiteral("允许范围1.588V~1.788V（等效120Ω附近）。")
                : QStringLiteral("首次验证建议从1.000V开始。");
            const QString sourceAdvice = isPt4ResistanceTest
                ? QStringLiteral("本项使用PT4两端的120Ω电阻。")
                : QStringLiteral("信号发生器请选择电压模式。");
            const auto decision = QMessageBox::warning(
                this,
                QStringLiteral("ADC外部测试确认"),
                QStringLiteral("%1\n\n%2\n"
                               "禁止把0~5V直接接到F28377D的ADCIN引脚。\n"
                               "%3确认接线后继续。")
                    .arg(wiringText, sourceAdvice, injectionAdvice),
                QMessageBox::Ok | QMessageBox::Cancel,
                QMessageBox::Cancel);
            if(decision != QMessageBox::Ok) {
                m_refreshScope = RefreshScope::None;
                return;
            }
            m_adcInjectionConfigurationPending = true;
            m_adcInjectionPendingTestId = testId;
            appendLog(LogChannel::External,
                      isPt4ResistanceTest
                          ? QStringLiteral("正在配置PT4电阻测试：%1，120Ω目标1.688V，容差±0.100V。")
                                .arg(selectionText)
                          : QStringLiteral("正在配置ADC注入：%1，目标%2V，容差±%3V。")
                                .arg(selectionText)
                                .arg(m_adcInjectionVoltageBox->value(), 0, 'f', 3)
                                .arg(m_adcInjectionToleranceBox->value(), 0, 'f', 3));
            m_client.configureAdcInjection(
                selectionId,
                expectedMillivolts,
                toleranceMillivolts);
            return;
        }
        quint8 singleSelection = 0U;
        if(testId == TestPwmDido) {
            singleSelection = static_cast<quint8>(
                m_pwmDidoChannelBox->currentData().toUInt());
            m_pwmDidoPendingChannelText = m_pwmDidoChannelBox->currentText();
            appendLog(LogChannel::External,
                      QStringLiteral("PWM-DIDO单路测试：%1。")
                          .arg(m_pwmDidoPendingChannelText));
        }
        if(testId == TestDriverFaultDi) {
            singleSelection = static_cast<quint8>(
                m_driverFaultGroupBox->currentData().toUInt());
            m_driverFaultPendingMask = singleSelection;
            appendLog(LogChannel::External,
                      QStringLiteral("驱动故障DI分组测试：%1，监测掩码=%2。")
                          .arg(m_driverFaultGroupBox->currentText(),
                               hex16(m_driverFaultPendingMask)));
        }
        if(testId == TestProtectionFaultDi) {
            singleSelection = static_cast<quint8>(
                m_protectionSelectionBox->currentData().toUInt());
            m_protectionPendingSelection = singleSelection;
            m_protectionPendingText = m_protectionSelectionBox->currentText();
            appendLog(LogChannel::External,
                      QStringLiteral("保护阈值测试：%1。等待正常值切换至故障值后再恢复正常。")
                          .arg(m_protectionPendingText));
        }
        if(testId == TestDriverResetDo) {
            singleSelection = static_cast<quint8>(
                m_driverResetChannelBox->currentData().toUInt());
            m_driverResetPendingChannelText =
                m_driverResetChannelBox->currentText();
            appendLog(LogChannel::External,
                      QStringLiteral("驱动复位单路输出测试：%1。")
                          .arg(m_driverResetPendingChannelText));
        }
        m_client.startSingle(testId,
                             stage,
                             isOutput,
                             singleSelection);
    });

    connect(&m_client, &DspTcpClient::connectionChanged, this,
            [this](bool connected, const QString &detail) {
                m_connectButton->setText(connected ? QStringLiteral("断开") : QStringLiteral("连接"));
                appendLog(LogChannel::Communication, detail);
                if (connected) {
                    m_waitingForStopCompletion = false;
                    m_client.requestBoardInfo();
                    m_client.requestStatus();
                } else {
                    m_pwmDidoLiveTimer->stop();
                    m_protectionResetLiveTimer->stop();
                    m_liveRecordRequestPending = false;
                    m_pwmDidoLastLiveLevel = -1;
                    m_driverResetLastLiveLevel = -1;
                    m_pwmDidoLiveValueLabel->setText(
                        QStringLiteral("连接已断开 | 输出状态未知"));
                    m_protectionStatusLabel->setText(
                        QStringLiteral("连接已断开"));
                    m_driverResetLiveValueLabel->setText(
                        QStringLiteral("复位GPIO实时值：连接已断开"));
                    m_waitingForStopCompletion = false;
                    m_nextRecordIndex = -1;
                    m_activeRecordIds.clear();
                    if (m_refreshScope != RefreshScope::None) {
                        m_waitingForTestReconnect = true;
                    }
                    m_detectedEthernetInterface =
                        DspTestProtocol::EthernetInterface::None;
                    m_boardProfileState =
                        DspTestProtocol::BoardProfileState::BootSafe;
                    m_boardProfileId = 0U;
                    // Tests can temporarily stop W5300/W5500 service. Keep the
                    // confirmed availability matrix so reconnect resumes the
                    // pending result refresh instead of starting a new sync.
                    updateTestCommandAvailability();
                }
            });
    connect(&m_client, &DspTcpClient::connectionProgress, this,
            [this](const QString &detail) {
                appendLog(LogChannel::Communication, detail);
            });
    connect(&m_client, &DspTcpClient::protocolError, this, [this](const QString &detail) {
        appendLog(LogChannel::Communication, QStringLiteral("协议错误：%1").arg(detail));
    });
    connect(&m_scibSerialClient, &ScibSerialClient::testProgress, this,
            [this](const QString &message) {
                appendLog(LogChannel::Communication, message);
            });
    connect(&m_scibSerialClient, &ScibSerialClient::testFinished, this,
            [this](bool passed, const QString &message) {
                appendLog(LogChannel::Communication,
                           passed ? message : QStringLiteral("%1 自动测试失败：%2")
                                                       .arg(activeSerialName(), message));
                if (!passed && m_scibSingleTestRequested && m_client.isConnected()) {
                    appendLog(LogChannel::Communication,
                              QStringLiteral("%1 单项测试未完成，正在停止 DSP 测试以解除忙状态。")
                                  .arg(activeSerialName()));
                    m_scibSingleTestRequested = false;
                    m_scibSingleTestStarted = false;
                    m_scibRecordRefreshPending = false;
                    m_waitingForStopCompletion = true;
                    m_client.stop();
                } else {
                    m_scibRecordRefreshPending = true;
                }
                if (m_client.isConnected() && !m_waitingForStopCompletion) {
                    m_client.requestStatus();
                }
            });
    connect(&m_zlgCanClient, &ZlgCanClient::testProgress, this,
            [this](const QString &message) {
                appendLog(LogChannel::Communication, message);
            });
    connect(&m_zlgCanClient, &ZlgCanClient::testFinished, this,
            [this](bool passed, const QString &message) {
                appendLog(LogChannel::Communication,
                          passed ? message : QStringLiteral("ZLG CAN 自动测试失败：%1").arg(message));
                if (!passed && m_canSingleTestRequested && m_client.isConnected()) {
                    appendLog(LogChannel::Communication,
                              QStringLiteral("CAN 单项测试未完成，正在停止 DSP 测试以解除忙状态。"));
                    m_canSingleTestRequested = false;
                    m_canSingleTestStarted = false;
                    m_waitingForStopCompletion = true;
                    m_client.stop();
                }
                if (m_client.isConnected() && !m_waitingForStopCompletion) {
                    m_client.requestStatus();
                }
            });
    connect(&m_ethernetTcpTestClient, &EthernetTcpTestClient::testProgress, this,
            [this](const QString &message) {
                appendLog(LogChannel::Communication, message);
            });
    connect(&m_ethernetTcpTestClient, &EthernetTcpTestClient::testFinished, this,
            [this](bool passed, const QString &message) {
                if(!m_ethernetTcpSequenceActive) {
                    return;
                }

                appendLog(LogChannel::Communication,
                          passed ? message :
                              QStringLiteral("Qt TCP 测试未完成：%1").arg(message));
                m_ethernetTcpAwaitingReconnect = true;
                QTimer::singleShot(150, this,
                                   &MainWindow::reconnectAfterEthernetTcpTest);
            });
    connect(&m_client, &DspTcpClient::responseReceived, this,
            [this](const DspTestProtocol::Response &response) {
                if(response.command == DspTestProtocol::Command::GetRecord) {
                    m_liveRecordRequestPending = false;
                }
                if (response.protocolStatus != DspTestProtocol::ProtocolStatus::Ok) {
                    appendLog(LogChannel::Communication,
                              QStringLiteral("DSP拒绝命令：%1")
                                  .arg(DspTestProtocol::protocolStatusText(response.protocolStatus)));
                    if (response.command == DspTestProtocol::Command::GetTestAvailability) {
                        resetTestAvailability();
                        appendLog(LogChannel::Communication,
                                  QStringLiteral("Test ID状态同步失败，测试功能保持锁定。请确认已下载支持命令0x23的新版本下位机固件。"));
                    }
                    if(response.command ==
                       DspTestProtocol::Command::ConfigureAdcInjection) {
                        m_adcInjectionConfigurationPending = false;
                        m_adcInjectionPendingTestId =
                            DspTestProtocol::InvalidTestId;
                        m_refreshScope = RefreshScope::None;
                    }
                    if ((response.command == DspTestProtocol::Command::StartBoard) ||
                        (response.command == DspTestProtocol::Command::StartExternal) ||
                        (response.command == DspTestProtocol::Command::StartHpd) ||
                        (response.command == DspTestProtocol::Command::StartSingle)) {
                        m_pwmDidoLiveTimer->stop();
                        m_protectionResetLiveTimer->stop();
                        m_liveRecordRequestPending = false;
                        m_pwmDidoLastLiveLevel = -1;
                        m_driverResetLastLiveLevel = -1;
                        m_refreshScope = RefreshScope::None;
                        m_waitingForTestReconnect = false;
                        m_scibSingleTestRequested = false;
                        m_scibSingleTestStarted = false;
                        m_canSingleTestRequested = false;
                        m_canSingleTestStarted = false;
                        m_singleTestRefreshPending = false;
                        m_singleTestStartStatusRequested = false;
                        m_singleTestObservedRunning = false;
                        m_singleTestRecordRefreshInFlight = false;
                        m_singleTestRefreshScope = RefreshScope::None;
                        m_singleTestRefreshId = DspTestProtocol::InvalidTestId;
                        resetEthernetTcpSequence();
                    }
                    if (response.command == DspTestProtocol::Command::StartExternal) {
                        m_edoManualConfirmationExpected = false;
                        m_edoManualConfirmationAvailable = false;
                        m_scibAutomaticTestRequested = false;
                        m_scibAutomaticTestStarted = false;
                        m_canAutomaticTestRequested = false;
                        m_canAutomaticTestStarted = false;
                        m_ethernetTcpExternalAuto = false;
                    }
                }
                if (((response.command == DspTestProtocol::Command::SelectBoardProfile) ||
                     (response.command == DspTestProtocol::Command::ConfirmBoardProfile) ||
                     (response.command == DspTestProtocol::Command::ClearBoardProfile)) &&
                    (response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok)) {
                    const QString action =
                        response.command == DspTestProtocol::Command::SelectBoardProfile ?
                            QStringLiteral("已选择板型，等待确认") :
                        response.command == DspTestProtocol::Command::ConfirmBoardProfile ?
                            QStringLiteral("板型已确认，测试功能已解锁") :
                            QStringLiteral("已清除板型确认，请重新选择");
                    appendLog(LogChannel::Communication, action);
                    m_client.requestBoardInfo();
                }
                if (response.command == DspTestProtocol::Command::StartBoard &&
                    response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) {
                    appendLog(LogChannel::Board,
                              QStringLiteral("板载自检已启动，等待 DSP 完成后刷新结果。"));
                    m_waitingForTestReconnect = true;
                }
                if (response.command == DspTestProtocol::Command::StartExternal &&
                    response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) {
                    appendLog(LogChannel::External,
                              QStringLiteral("外设自检已启动，等待 DSP 完成后刷新结果。"));
                    m_waitingForTestReconnect = true;
                }
                if (response.command == DspTestProtocol::Command::StartHpd &&
                    response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) {
                    appendLog(LogChannel::Hpd,
                              QStringLiteral("HPD 注入自检已启动，等待 DSP 完成后刷新结果。"));
                    m_waitingForTestReconnect = true;
                }
                if (response.command == DspTestProtocol::Command::GetBoardInfo &&
                    response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) {
                    updateBoardInfo(response);
                }
                if (response.command == DspTestProtocol::Command::GetTestAvailability &&
                    response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) {
                    updateTestAvailability(response);
                }
                if((response.command ==
                    DspTestProtocol::Command::ConfigureAdcInjection) &&
                   (response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) &&
                   m_adcInjectionConfigurationPending) {
                    const quint16 pendingTestId = m_adcInjectionPendingTestId;
                    m_adcInjectionConfigurationPending = false;
                    m_adcInjectionPendingTestId = DspTestProtocol::InvalidTestId;
                    m_singleTestRefreshScope = RefreshScope::External;
                    m_singleTestRefreshId = pendingTestId;
                    appendLog(LogChannel::External,
                              QStringLiteral("ADC注入参数已写入DSP，开始64次平均采样。"));
                    m_client.startSingle(
                        pendingTestId,
                        DspTestProtocol::Stage::ExternalConnected,
                        false);
                }
                if ((response.command == DspTestProtocol::Command::Stop) &&
                    (response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) &&
                    m_waitingForStopCompletion) {
                    m_waitingForStopCompletion = false;
                    QTimer::singleShot(150, this, [this] {
                        if (m_client.isConnected()) {
                            m_client.requestStatus();
                        }
                    });
                }
                if (response.command == DspTestProtocol::Command::StartSingle &&
                    response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) {
                    if(m_singleTestRefreshId == TestPwmDido) {
                        m_liveRecordRequestPending = false;
                        m_pwmDidoLastLiveLevel = -1;
                        m_pwmDidoLiveValueLabel->setText(
                            QStringLiteral("启动中 | 等待GPIO读回"));
                        m_pwmDidoLiveValueLabel->setStyleSheet(QStringLiteral(
                            "QLabel { background: #FFF8E1; border: 1px solid #FFB300; "
                            "border-radius: 4px; padding: 6px 8px; font-weight: 600; }"));
                        m_pwmDidoLiveTimer->start();
                    }
                    if (m_singleTestRefreshId == TestProtectionFaultDi) {
                        m_liveRecordRequestPending = false;
                        m_protectionStatusLabel->setText(
                            QStringLiteral("启动中 | 等待保护GPIO"));
                        m_protectionStatusLabel->setStyleSheet(QStringLiteral(
                            "QLabel { background: #FFF8E1; border: 1px solid #FFB300; "
                            "border-radius: 4px; padding: 6px 8px; font-weight: 600; }"));
                        m_protectionResetLiveTimer->start();
                        appendLog(
                            LogChannel::External,
                            QStringLiteral("保护测试已启动：30秒内设置正常值；进入“等待故障触发”后另有30秒切换到故障值。ADC达到阈值后硬件保护链有10秒响应时间，识别故障后请恢复正常值。"));
                    }
                    if(m_singleTestRefreshId == TestDriverResetDo) {
                        m_liveRecordRequestPending = false;
                        m_driverResetLastLiveLevel = -1;
                        m_driverResetLiveValueLabel->setText(
                            QStringLiteral("复位GPIO实时值：正在等待GPIO读回..."));
                        m_protectionResetLiveTimer->start();
                    }
                    if(m_ethernetTcpSequenceActive &&
                       m_ethernetTcpAwaitingStart) {
                        m_ethernetTcpAwaitingStart = false;
                        if(isEthernetTcpTest(m_ethernetTcpCurrentTestId)) {
                            QTimer::singleShot(50, this,
                                               &MainWindow::startEthernetTcpRawTest);
                        } else {
                            m_ethernetTcpAwaitingReconnect = true;
                            QTimer::singleShot(50, this, [this] {
                                if(m_ethernetTcpSequenceActive &&
                                   m_client.isConnected()) {
                                    m_client.requestStatus();
                                }
                            });
                        }
                    } else {
                        m_waitingForTestReconnect = true;
                        m_singleTestRefreshPending = true;
                        m_singleTestStartStatusRequested = false;
                        m_singleTestObservedRunning = false;
                        QTimer::singleShot(50, this, [this] {
                            if (m_singleTestRefreshPending && m_client.isConnected()) {
                                m_singleTestStartStatusRequested = true;
                                m_client.requestStatus();
                            }
                        });
                    }
                }
                const bool liveSingleRecordCompleted =
                    (response.command == DspTestProtocol::Command::GetRecord) &&
                    (response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) &&
                    m_singleTestRefreshPending &&
                    (response.recordId == m_singleTestRefreshId) &&
                    ((response.recordId == TestPwmDido) ||
                     (response.recordId == TestProtectionFaultDi) ||
                     (response.recordId == TestDriverResetDo)) &&
                    (response.recordResult != DspTestProtocol::Result::Running) &&
                    (response.recordResult != DspTestProtocol::Result::NotRun);
                if(liveSingleRecordCompleted) {
                    const RefreshScope singleScope = m_singleTestRefreshScope;
                    const quint16 singleTestId = m_singleTestRefreshId;
                    m_pwmDidoLiveTimer->stop();
                    m_protectionResetLiveTimer->stop();
                    m_waitingForTestReconnect = false;
                    m_singleTestRefreshPending = false;
                    m_singleTestStartStatusRequested = false;
                    m_singleTestObservedRunning = false;
                    m_singleTestRecordRefreshInFlight = true;
                    appendScopeLog(singleScope,
                                   QStringLiteral("单项测试已完成，正在刷新 %1。")
                                       .arg(hex16(singleTestId)));
                }
                updateStatus(response);
                if ((response.command != DspTestProtocol::Command::GetTestAvailability) &&
                    (response.recordId != DspTestProtocol::InvalidTestId)) {
                    updateRecord(response);
                }
                if ((response.command == DspTestProtocol::Command::GetStatus) &&
                    (response.activeTestId == TestProtectionFaultDi) &&
                    m_singleTestRefreshPending &&
                    !m_protectionResetLiveTimer->isActive() &&
                    !m_liveRecordRequestPending) {
                    m_liveRecordRequestPending = true;
                    m_client.requestRecord(TestProtectionFaultDi);
                }
                if((response.command == DspTestProtocol::Command::GetStatus) &&
                   m_ethernetTcpSequenceActive &&
                   m_ethernetTcpAwaitingReconnect &&
                   (response.managerMode == 1U) &&
                   (m_ethernetTcpCurrentTestId != DspTestProtocol::InvalidTestId)) {
                    m_ethernetTcpAwaitingReconnect = false;
                    m_ethernetTcpRecordRefreshPending = true;
                    m_waitingForTestReconnect = false;
                    appendLog(LogChannel::External,
                              QStringLiteral("%1 已完成，正在读取 DSP 测试记录。")
                                  .arg(hex16(m_ethernetTcpCurrentTestId)));
                    m_client.requestRecord(m_ethernetTcpCurrentTestId);
                } else if ((response.command == DspTestProtocol::Command::GetStatus) &&
                    m_singleTestRefreshPending &&
                    (response.activeTestId == m_singleTestRefreshId)) {
                    m_singleTestObservedRunning = true;
                    if (m_scibSingleTestRequested && !m_scibSingleTestStarted &&
                        isSerialExternalTest(response.activeTestId)) {
                        m_scibSingleTestStarted = true;
                        startScibTest(true);
                    }
                    if (m_canSingleTestRequested && !m_canSingleTestStarted &&
                        (response.activeTestId == TestCanExternal)) {
                        m_canSingleTestStarted = true;
                        startCanTest(true);
                    }
                }
                if ((response.command == DspTestProtocol::Command::GetStatus) &&
                    (response.managerMode == 1U) &&
                    m_singleTestRefreshPending &&
                    m_singleTestStartStatusRequested &&
                    (m_singleTestObservedRunning ||
                     ((!isSerialExternalTest(m_singleTestRefreshId)) &&
                      (m_singleTestRefreshId != TestCanExternal))) &&
                    m_waitingForTestReconnect &&
                    (m_nextRecordIndex < 0)) {
                    const RefreshScope singleScope = m_singleTestRefreshScope;
                    const quint16 singleTestId = m_singleTestRefreshId;

                    m_waitingForTestReconnect = false;
                    m_singleTestRefreshPending = false;
                    m_singleTestRecordRefreshInFlight = true;
                    m_scibRecordRefreshPending = false;
                    appendScopeLog(singleScope,
                                   QStringLiteral("单项测试已完成，正在刷新 %1。")
                                       .arg(hex16(singleTestId)));
                    if(!m_liveRecordRequestPending) {
                        m_client.requestRecord(singleTestId);
                    }
                } else if ((response.command == DspTestProtocol::Command::GetStatus) &&
                           (response.managerMode == 1U) &&
                           (m_refreshScope != RefreshScope::None) &&
                           !m_singleTestRefreshPending &&
                           !m_singleTestRecordRefreshInFlight &&
                           m_waitingForTestReconnect &&
                           m_testAvailabilityReady &&
                           (m_nextRecordIndex < 0)) {
                    m_waitingForTestReconnect = false;
                    if(m_ethernetTcpExternalAuto &&
                       !m_ethernetTcpSequenceActive) {
                        appendLog(LogChannel::External,
                                  QStringLiteral("外设基础自检完成，开始 Qt 自动 TCP_LINK/ECHO/STABILITY。"));
                        startEthernetTcpSequence(true, false);
                    } else {
                        appendScopeLog(m_refreshScope, QStringLiteral("DSP 已返回待命，正在刷新测试记录。"));
                        beginRecordRefresh(m_refreshScope);
                    }
                } else if (response.command == DspTestProtocol::Command::GetRecord && m_nextRecordIndex >= 0) {
                    requestNextRecord();
                }
            });

    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(1000);
    connect(m_pollTimer, &QTimer::timeout, this, [this] {
        if (m_client.isConnected() && m_nextRecordIndex < 0 &&
            !m_waitingForStopCompletion) {
            m_client.requestStatus();
        }
    });
    m_pollTimer->start();

    m_pwmDidoLiveTimer = new QTimer(this);
    m_pwmDidoLiveTimer->setInterval(200);
    connect(m_pwmDidoLiveTimer, &QTimer::timeout, this, [this] {
        if(m_client.isConnected() &&
           m_singleTestRefreshPending &&
           (m_singleTestRefreshId == TestPwmDido) &&
           !m_singleTestRecordRefreshInFlight &&
           !m_liveRecordRequestPending &&
           (m_nextRecordIndex < 0)) {
            m_liveRecordRequestPending = true;
            m_client.requestRecord(TestPwmDido);
        }
    });

    m_protectionResetLiveTimer = new QTimer(this);
    m_protectionResetLiveTimer->setInterval(200);
    connect(m_protectionResetLiveTimer, &QTimer::timeout, this, [this] {
        if(m_client.isConnected() &&
           m_singleTestRefreshPending &&
           ((m_singleTestRefreshId == TestProtectionFaultDi) ||
             (m_singleTestRefreshId == TestDriverResetDo)) &&
           !m_singleTestRecordRefreshInFlight &&
           !m_liveRecordRequestPending &&
           (m_nextRecordIndex < 0)) {
            m_liveRecordRequestPending = true;
            m_client.requestRecord(m_singleTestRefreshId);
        }
    });
}

void MainWindow::addTest(quint16 id, const QString &name, RefreshScope scope)
{
    QTableWidget *table = nullptr;
    QVector<quint16> *recordIds = nullptr;

    switch (scope) {
    case RefreshScope::Board:
        table = m_boardRecordsTable;
        recordIds = &m_boardRecordIds;
        break;
    case RefreshScope::External:
        table = m_externalRecordsTable;
        recordIds = &m_externalRecordIds;
        break;
    case RefreshScope::Hpd:
        table = m_hpdRecordsTable;
        recordIds = &m_hpdRecordIds;
        break;
    case RefreshScope::None:
        return;
    }

    const int row = table->rowCount();
    table->insertRow(row);
    table->setItem(row, 0, new QTableWidgetItem(hex16(id)));
    table->setItem(row, 1, new QTableWidgetItem(name));
    table->setItem(row, 2, new QTableWidgetItem(QStringLiteral("0x0000")));
    table->setItem(row, 3, new QTableWidgetItem(QStringLiteral("0x00000000")));
    table->setItem(row, 4, new QTableWidgetItem(QStringLiteral("0.000")));
    table->setItem(row, 5, new QTableWidgetItem(QStringLiteral("0.000")));
    table->setItem(row, 6, new QTableWidgetItem(QStringLiteral("0.000")));
    auto *resultItem = new QTableWidgetItem(QStringLiteral("NOT_RUN"));
    updateResultAppearance(resultItem, DspTestProtocol::Result::NotRun, false);
    table->setItem(row, 7, resultItem);
    recordIds->append(id);
    m_recordRows.insert(id, {table, row});
}

void MainWindow::prepareResultRefresh(RefreshScope scope)
{
    cancelManualOutputConfirmation();
    m_pwmDidoLiveTimer->stop();
    m_protectionResetLiveTimer->stop();
    m_liveRecordRequestPending = false;
    m_refreshScope = scope;
    m_waitingForTestReconnect = false;
    m_activeRecordIds.clear();
    m_nextRecordIndex = -1;
    m_singleTestRefreshPending = false;
    m_singleTestStartStatusRequested = false;
    m_singleTestObservedRunning = false;
    m_singleTestRecordRefreshInFlight = false;
    m_singleTestRefreshScope = RefreshScope::None;
    m_singleTestRefreshId = DspTestProtocol::InvalidTestId;
}

void MainWindow::prepareSingleTestRefresh(RefreshScope scope, quint16 testId)
{
    cancelManualOutputConfirmation();
    m_pwmDidoLiveTimer->stop();
    m_protectionResetLiveTimer->stop();
    m_liveRecordRequestPending = false;
    m_driverResetLastLiveLevel = -1;
    if (testId == TestDriverFaultDi) {
        m_driverFaultPendingMask = static_cast<quint16>(
            m_driverFaultGroupBox->currentData().toUInt());
        updateDriverFaultChannelStatus(m_driverFaultPendingMask, 0U, false);
    }
    m_protectionLivePhase = 0U;
    m_protectionRecordFormatMismatchReported = false;
    if(testId == TestProtectionFaultDi) {
        m_lastProtectionFinalLogKey.clear();
    }
    m_refreshScope = RefreshScope::None;
    m_waitingForTestReconnect = false;
    m_activeRecordIds.clear();
    m_nextRecordIndex = -1;
    m_singleTestRefreshPending = false;
    m_singleTestStartStatusRequested = false;
    m_singleTestObservedRunning = false;
    m_singleTestRecordRefreshInFlight = false;
    m_singleTestRefreshScope = scope;
    m_singleTestRefreshId = testId;
    m_edoManualConfirmationExpected = false;
    m_edoManualConfirmationAvailable = false;
}

void MainWindow::updateSingleTestChoices(RefreshScope scope)
{
    const QVector<quint16> *recordIds = nullptr;
    const quint16 previousId = static_cast<quint16>(m_singleTestBox->currentData().toUInt());

    switch (scope) {
    case RefreshScope::Board:
        recordIds = &m_boardRecordIds;
        break;
    case RefreshScope::External:
        recordIds = &m_externalRecordIds;
        break;
    case RefreshScope::Hpd:
        recordIds = &m_hpdRecordIds;
        break;
    case RefreshScope::None:
        return;
    }

    m_singleTestBox->blockSignals(true);
    m_singleTestBox->clear();
    for (const quint16 id : *recordIds) {
        if (!isTestRunnable(id)) {
            continue;
        }
        if((scope == RefreshScope::External) &&
           isSerialExternalTest(id) &&
           (id != activeSerialTestId())) {
            continue;
        }
        if((scope == RefreshScope::External) &&
           ((id == TestEthernetSocket) || isEthernetTcpTest(id))) {
            continue;
        }
        const RecordLocation location = m_recordRows.value(id);
        QString name = location.table->item(location.row, 1)->text();
        if((scope == RefreshScope::External) && (id == TestEthernetBasic)) {
            name = QStringLiteral("ETHERNET（完整自动测试）");
        }
        if (m_testAvailability.value(id) ==
            DspTestProtocol::TestAvailability::PendingValidation) {
            name += QStringLiteral(" [待验证]");
        }
        m_singleTestBox->addItem(QStringLiteral("%1  %2").arg(hex16(id), name), id);
    }

    const int previousIndex = m_singleTestBox->findData(previousId);
    if (previousIndex >= 0) {
        m_singleTestBox->setCurrentIndex(previousIndex);
    }
    m_singleTestBox->blockSignals(false);
    m_singleTestBox->setToolTip(m_singleTestBox->currentText());
    updateSingleTestParameterControls();
}

void MainWindow::resetEthernetTcpSequence()
{
    m_ethernetTcpSequenceActive = false;
    m_ethernetTcpExternalAuto = false;
    m_ethernetTcpAwaitingStart = false;
    m_ethernetTcpAwaitingReconnect = false;
    m_ethernetTcpRecordRefreshPending = false;
    m_ethernetTcpCurrentTestId = DspTestProtocol::InvalidTestId;
    m_ethernetTcpPendingTests.clear();
    m_ethernetTcpTestClient.cancel();
}

void MainWindow::startEthernetTcpSequence(bool externalAuto,
                                          bool includeBasicAndSocket)
{
    m_ethernetTcpSequenceActive = true;
    m_ethernetTcpExternalAuto = externalAuto;
    m_ethernetTcpAwaitingStart = false;
    m_ethernetTcpAwaitingReconnect = false;
    m_ethernetTcpRecordRefreshPending = false;
    m_ethernetTcpCurrentTestId = DspTestProtocol::InvalidTestId;
    m_ethernetTcpPendingTests.clear();
    if(includeBasicAndSocket) {
        m_ethernetTcpPendingTests << TestEthernetBasic
                                  << TestEthernetSocket;
    }
    m_ethernetTcpPendingTests << TestEthernetTcpLink
                              << TestEthernetTcpEcho
                              << TestEthernetTcpStability;
    startNextEthernetTcpTest();
}

void MainWindow::startNextEthernetTcpTest()
{
    if(!m_ethernetTcpSequenceActive)
    {
        return;
    }

    if(m_ethernetTcpPendingTests.isEmpty())
    {
        finishEthernetTcpSequence();
        return;
    }

    m_ethernetTcpCurrentTestId = m_ethernetTcpPendingTests.takeFirst();
    m_ethernetTcpAwaitingStart = true;
    m_ethernetTcpAwaitingReconnect = false;
    m_ethernetTcpRecordRefreshPending = false;
    m_waitingForTestReconnect = false;
    appendLog(LogChannel::External,
              isEthernetTcpTest(m_ethernetTcpCurrentTestId) ?
                  QStringLiteral("启动 %1，Qt 将自动执行 TCP 客户端收发。")
                      .arg(hex16(m_ethernetTcpCurrentTestId)) :
                  QStringLiteral("启动 %1，DSP 将按已探测的网络芯片执行基础或 Socket 检查。")
                      .arg(hex16(m_ethernetTcpCurrentTestId)));
    m_client.startSingle(m_ethernetTcpCurrentTestId,
                         DspTestProtocol::Stage::ExternalConnected,
                         false);
}

void MainWindow::startEthernetTcpRawTest()
{
    if(!m_ethernetTcpSequenceActive || m_ethernetTcpAwaitingStart ||
       (m_ethernetTcpCurrentTestId == DspTestProtocol::InvalidTestId))
    {
        return;
    }

    appendLog(LogChannel::Communication,
              QStringLiteral("释放主协议连接，开始 %1 的 Qt TCP 测试。")
                  .arg(hex16(m_ethernetTcpCurrentTestId)));
    m_client.disconnectFromDevice();
    m_ethernetTcpTestClient.start(m_hostEdit->text().trimmed(),
                                  m_portEdit->text().toUShort(),
                                  ethernetTcpMode(m_ethernetTcpCurrentTestId));
}

void MainWindow::reconnectAfterEthernetTcpTest()
{
    if(!m_ethernetTcpSequenceActive)
    {
        return;
    }

    appendLog(LogChannel::Communication,
              QStringLiteral("TCP 测试通信结束，正在重连 DSP 并读取结果。"));
    m_client.reconnectToDevice(m_hostEdit->text().trimmed(),
                               m_portEdit->text().toUShort());
}

void MainWindow::finishEthernetTcpSequence()
{
    const bool externalAuto = m_ethernetTcpExternalAuto;

    m_ethernetTcpSequenceActive = false;
    m_ethernetTcpExternalAuto = false;
    m_ethernetTcpAwaitingStart = false;
    m_ethernetTcpAwaitingReconnect = false;
    m_ethernetTcpRecordRefreshPending = false;
    m_ethernetTcpCurrentTestId = DspTestProtocol::InvalidTestId;
    m_ethernetTcpPendingTests.clear();

    if(externalAuto)
    {
        appendLog(LogChannel::External,
                  QStringLiteral("Qt 自动 TCP_LINK/ECHO/STABILITY 测试完成，正在刷新全部外设结果。"));
        beginRecordRefresh(RefreshScope::External);
    }
    else
    {
        appendLog(LogChannel::External,
                  QStringLiteral("ETHERNET 完整单项测试记录刷新完成。"));
        m_refreshScope = RefreshScope::None;
    }
}

bool MainWindow::isEthernetTcpTest(quint16 testId) const
{
    return (testId == TestEthernetTcpLink) ||
           (testId == TestEthernetTcpEcho) ||
           (testId == TestEthernetTcpStability);
}

EthernetTcpTestClient::Mode MainWindow::ethernetTcpMode(quint16 testId) const
{
    if(testId == TestEthernetTcpEcho)
    {
        return EthernetTcpTestClient::Mode::Echo;
    }
    if(testId == TestEthernetTcpStability)
    {
        return EthernetTcpTestClient::Mode::Stability;
    }
    return EthernetTcpTestClient::Mode::Link;
}

void MainWindow::beginRecordRefresh(RefreshScope scope)
{
    if (!m_testAvailabilityReady) {
        m_waitingForTestReconnect = true;
        appendScopeLog(scope,
                       QStringLiteral("等待 Test ID 状态同步完成后刷新测试记录。"));
        return;
    }

    switch (scope) {
    case RefreshScope::Board:
        m_activeRecordIds = m_boardRecordIds;
        break;
    case RefreshScope::External:
        m_activeRecordIds = m_externalRecordIds;
        break;
    case RefreshScope::Hpd:
        m_activeRecordIds = m_hpdRecordIds;
        break;
    case RefreshScope::None:
        return;
    }

    // Read every record in the selected stage. Availability controls row
    // visibility and single-test choices, but must not suppress result reads
    // after a reconnect or a fresh availability synchronization.
    m_nextRecordIndex = 0;
    requestNextRecord();
}

void MainWindow::requestNextRecord()
{
    if (m_nextRecordIndex >= m_activeRecordIds.size()) {
        const RefreshScope completedScope = m_refreshScope;

        appendScopeLog(m_refreshScope, QStringLiteral("测试记录刷新完成。"));
        m_nextRecordIndex = -1;
        m_activeRecordIds.clear();
        m_refreshScope = RefreshScope::None;
        requestPendingScibRecordRefresh();
        if (completedScope == RefreshScope::External) {
            requestEdoManualConfirmation();
        }
        return;
    }
    m_client.requestRecord(m_activeRecordIds.at(m_nextRecordIndex++));
}

void MainWindow::requestPendingScibRecordRefresh()
{
    if (!m_scibRecordRefreshPending ||
        !m_client.isConnected() ||
        m_nextRecordIndex >= 0) {
        return;
    }

    m_scibRecordRefreshPending = false;
    m_client.requestRecord(activeSerialTestId());
}

quint16 MainWindow::activeSerialTestId() const
{
    return (m_boardProfileId == 0x0003U) ?
        TestRs422External : TestSciRs485External;
}

bool MainWindow::isSerialExternalTest(quint16 testId) const
{
    return (testId == TestSciRs485External) ||
           (testId == TestRs422External);
}

QString MainWindow::activeSerialName() const
{
    return (m_boardProfileId == 0x0003U) ?
        QStringLiteral("RS422") : QStringLiteral("SCIB/RS485");
}

void MainWindow::updateRecord(const DspTestProtocol::Response &response)
{
    const auto rowIterator = m_recordRows.constFind(response.recordId);
    const bool isSingleTestRecord =
        m_singleTestRecordRefreshInFlight &&
        (response.recordId == m_singleTestRefreshId);
    const RefreshScope logScope = isSingleTestRecord ?
        m_singleTestRefreshScope : m_refreshScope;
    const bool isManualEdoConfirmation =
        (response.recordId == TestDido) &&
        (response.recordResult == DspTestProtocol::Result::Warn) &&
        (response.recordErrorCode == 0U);
    const bool isManualPwmConfirmation =
        (response.recordId == TestPwmDido) &&
        (response.recordResult == DspTestProtocol::Result::Warn) &&
        (response.recordErrorCode == 0U);
    const bool isManualDriverResetConfirmation =
        (response.recordId == TestDriverResetDo) &&
        (response.recordResult == DspTestProtocol::Result::Warn) &&
        (response.recordErrorCode == 0U);
    const bool isManualOutputConfirmation =
        isManualEdoConfirmation || isManualPwmConfirmation ||
        isManualDriverResetConfirmation;
    const QString resultText = isManualOutputConfirmation ?
        QStringLiteral("MANUAL_CONFIRM") :
        DspTestProtocol::resultText(response.recordResult);
    const bool isProtectionFinal =
        (response.recordId == TestProtectionFaultDi) &&
        (response.recordResult != DspTestProtocol::Result::Running) &&
        (response.recordResult != DspTestProtocol::Result::NotRun);
    const QString protectionFinalLogKey = isProtectionFinal ?
        QStringLiteral("%1/%2/%3/%4")
            .arg(static_cast<int>(response.recordResult))
            .arg(response.recordErrorCode)
            .arg(response.rawValue)
            .arg(response.measuredValue, 0, 'f', 3) :
        QString();
    const bool duplicateProtectionFinalLog =
        isProtectionFinal &&
        (protectionFinalLogKey == m_lastProtectionFinalLogKey);
    if(isProtectionFinal && !duplicateProtectionFinalLog) {
        m_lastProtectionFinalLogKey = protectionFinalLogKey;
    }

    if (rowIterator == m_recordRows.cend()) {
        return;
    }
    const RecordLocation location = rowIterator.value();
    location.table->item(location.row, 2)->setText(hex16(response.recordErrorCode));
    location.table->item(location.row, 3)->setText(hex32(response.rawValue));
    location.table->item(location.row, 4)->setText(QString::number(response.measuredValue, 'f', 3));
    location.table->item(location.row, 5)->setText(QString::number(response.expectedMin, 'f', 3));
    location.table->item(location.row, 6)->setText(QString::number(response.expectedMax, 'f', 3));
    auto *resultItem = location.table->item(location.row, 7);
    resultItem->setText(resultText);
    updateResultAppearance(resultItem,
                           response.recordResult,
                           isManualOutputConfirmation);
    if((response.recordId == TestPwmDido) &&
       (response.recordResult == DspTestProtocol::Result::Running)) {
        const quint16 liveOutputMask = static_cast<quint16>(
            (response.rawValue >> 16U) & 0x0FFFU);
        const quint16 requestedMask = static_cast<quint16>(
            response.expectedMin) & 0x0FFFU;
        const bool selectedGpioHigh =
            (requestedMask != 0U) &&
            ((liveOutputMask & requestedMask) == requestedMask);
        const QString liveText = selectedGpioHigh ?
            QStringLiteral("输出中 | GPIO 高 | 逻辑 %1V | %2/2.0s\n"
                           "端子电压：请看万用表")
                .arg(response.measuredValue, 0, 'f', 1)
                .arg(response.expectedMax / 1000.0F, 0, 'f', 1) :
            QStringLiteral("收尾中 | GPIO 低 | 输出已关闭\n"
                           "等待测试完成");
        m_pwmDidoLiveValueLabel->setText(liveText);
        m_pwmDidoLiveValueLabel->setStyleSheet(selectedGpioHigh ?
            QStringLiteral("QLabel { background: #E8F5E9; border: 1px solid #66BB6A; "
                           "border-radius: 4px; padding: 6px 8px; font-weight: 600; }") :
            QStringLiteral("QLabel { background: #FFF8E1; border: 1px solid #FFB300; "
                           "border-radius: 4px; padding: 6px 8px; font-weight: 600; }"));
        resultItem->setToolTip(QStringLiteral(
            "%1\nGPIO读回是数字逻辑折算值，不是端子ADC实测值。")
            .arg(liveText));
        const int liveLevel = selectedGpioHigh ? 1 : 0;
        if(liveLevel != m_pwmDidoLastLiveLevel) {
            m_pwmDidoLastLiveLevel = liveLevel;
            appendLog(
                LogChannel::External,
                QStringLiteral("PWM-DIDO实时读回：%1，DSP GPIO=%2（逻辑折算约%3V）。")
                    .arg(m_pwmDidoPendingChannelText,
                         selectedGpioHigh ? QStringLiteral("高") :
                                            QStringLiteral("低"))
                    .arg(response.measuredValue, 0, 'f', 1));
        }
        return;
    }
    if((response.recordId == TestDriverResetDo) &&
       (response.recordResult == DspTestProtocol::Result::Running)) {
        const quint16 liveOutputMask = static_cast<quint16>(
            (response.rawValue >> 16U) & 0x000FU);
        const quint16 requestedMask = static_cast<quint16>(
            response.expectedMin) & 0x000FU;
        const bool selectedGpioHigh =
            (requestedMask != 0U) &&
            ((liveOutputMask & requestedMask) == requestedMask);
        const QString liveText = QStringLiteral(
            "复位GPIO实时值：%1，DSP数字逻辑折算约%2V，已保持%3/10.0秒；"
            "其他三路应保持低电平，端子侧电压仍需用万用表确认。")
            .arg(selectedGpioHigh ? QStringLiteral("高") : QStringLiteral("低"))
            .arg(response.measuredValue, 0, 'f', 1)
            .arg(response.expectedMax / 1000.0F, 0, 'f', 1);
        m_driverResetLiveValueLabel->setText(liveText);
        resultItem->setToolTip(liveText);
        const int liveLevel = selectedGpioHigh ? 1 : 0;
        if(liveLevel != m_driverResetLastLiveLevel) {
            m_driverResetLastLiveLevel = liveLevel;
            appendLog(
                LogChannel::External,
                QStringLiteral("驱动复位实时读回：%1，DSP GPIO=%2（逻辑折算约%3V）。")
                    .arg(m_driverResetPendingChannelText,
                         selectedGpioHigh ? QStringLiteral("高") :
                                            QStringLiteral("低"))
                    .arg(response.measuredValue, 0, 'f', 1));
        }
        return;
    }
    if(((response.recordId == TestProtectionFaultDi) ||
        (response.recordId == TestDriverResetDo)) &&
       (response.recordResult != DspTestProtocol::Result::Running)) {
        m_protectionResetLiveTimer->stop();
        if(response.recordId == TestDriverResetDo) {
            const quint16 finalStatusMask = static_cast<quint16>(
                (response.rawValue >> 16U) & 0xFFFFU);
            m_driverResetLastLiveLevel = -1;
            m_driverResetLiveValueLabel->setText(
                ((finalStatusMask & 0x0008U) != 0U) ?
                    QStringLiteral("复位GPIO实时值：测试结束，已恢复低电平") :
                    QStringLiteral("复位GPIO实时值：测试结束，但安全低电平读回异常"));
        }
    }
    const bool protectionRecordFormatCurrent =
        (response.recordId != TestProtectionFaultDi) ||
        ((response.rawValue & ProtectionRecordFormatMask) ==
         ProtectionRecordFormatV2);
    if((response.recordId == TestProtectionFaultDi) &&
       (response.recordResult != DspTestProtocol::Result::NotRun) &&
       !protectionRecordFormatCurrent) {
        const QString mismatchText = QStringLiteral(
            "DSP固件版本不匹配：0X0321记录缺少V2格式标识。请下载HPD_BoardTest_DSP/Debug/HPD_BoardTest_DSP.out后重新测试。");
        m_protectionStatusLabel->setText(mismatchText);
        resultItem->setToolTip(mismatchText);
        if(!m_protectionRecordFormatMismatchReported) {
            m_protectionRecordFormatMismatchReported = true;
            appendScopeLog(RefreshScope::External, mismatchText);
            if((response.recordResult == DspTestProtocol::Result::Running) &&
               m_client.isConnected()) {
                appendLog(LogChannel::Communication,
                          QStringLiteral("正在停止旧固件启动的保护测试以解除忙状态。"));
                m_waitingForStopCompletion = true;
                m_client.stop();
            }
        }
        if(response.recordResult == DspTestProtocol::Result::Running) {
            return;
        }
    }
    if((response.recordId == TestProtectionFaultDi) &&
       (response.recordResult == DspTestProtocol::Result::Running) &&
       protectionRecordFormatCurrent) {
        const quint16 phase = static_cast<quint16>(
            (response.rawValue >> 28U) & 0x000FU);
        const quint16 statusMask = static_cast<quint16>(
            (response.rawValue >> 16U) & 0x01FFU);
        quint16 expectedFaultBit = static_cast<quint16>(
            (response.rawValue >> 12U) & 0x000FU);
        if(expectedFaultBit == 0U) {
            expectedFaultBit = protectionFaultBitForSelection(
                m_protectionPendingSelection);
        }
        const quint16 currentFaultMask = static_cast<quint16>(
            response.rawValue & 0x000FU);
        const bool currentBreakHigh =
            ((response.rawValue >> 4U) & 0x0001U) != 0U;
        const bool initialBreakHigh =
            ((response.rawValue >> 5U) & 0x0001U) != 0U;
        const bool clearOutputHigh =
            ((response.rawValue >> 6U) & 0x0001U) != 0U;
        const bool clearPulseSeen =
            ((response.rawValue >> 7U) & 0x0001U) != 0U;
        const auto faultLevelText = [currentFaultMask](quint16 bit) {
            return ((currentFaultMask & bit) != 0U) ?
                QStringLiteral("低") : QStringLiteral("高");
        };
        QString phaseText;
        if(phase == 1U) {
            phaseText = QStringLiteral("等待正常基线");
        } else if(phase == 2U) {
            phaseText = ((statusMask & 0x0004U) != 0U) ?
                QStringLiteral("等待保护GPIO响应") :
                QStringLiteral("等待故障触发");
        } else if(phase == 3U) {
            phaseText = QStringLiteral("等待恢复正常");
        } else {
            phaseText = QStringLiteral("未知阶段");
        }
        QStringList baselineBlockers;
        if((phase == 1U) &&
           ((currentFaultMask & expectedFaultBit) != 0U)) {
            baselineBlockers.append(
                protectionFaultInputName(expectedFaultBit) +
                QStringLiteral("当前为低电平故障态"));
        }
        const QString blockerText = baselineBlockers.isEmpty() ?
            QString() :
            QStringLiteral("\n阻塞：%1").arg(
                baselineBlockers.join(QStringLiteral("、")));
        const QString breakWarningText =
            ((phase >= 2U) && initialBreakHigh) ?
                QStringLiteral(" | BRK预置高") :
                QString();
        const QString liveText = QStringLiteral(
            "%1 | ADC %2V | %3/%4s | 阈值%5%6\n"
            "OC1 %7 | OC2 %8 | UOV1 %9 | UOV2 %10 | BRK %11 | CLR %12%13")
            .arg(phaseText)
            .arg(response.measuredValue, 0, 'f', 3)
            .arg(response.expectedMin, 0, 'f', 1)
            .arg(response.expectedMax, 0, 'f', 0)
            .arg((statusMask & 0x0004U) != 0U ? QStringLiteral("已到") : QStringLiteral("未到"))
            .arg(breakWarningText)
            .arg(faultLevelText(0x0001U))
            .arg(faultLevelText(0x0002U))
            .arg(faultLevelText(0x0004U))
            .arg(faultLevelText(0x0008U))
            .arg(currentBreakHigh ? QStringLiteral("高") : QStringLiteral("低"))
            .arg(clearOutputHigh ? QStringLiteral("高") : QStringLiteral("低"))
            .arg(blockerText);
        m_protectionStatusLabel->setText(liveText);
        m_protectionStatusLabel->setStyleSheet(QStringLiteral(
            "QLabel { background: #FFF8E1; border: 1px solid #FFB300; "
            "border-radius: 4px; padding: 6px 8px; font-weight: 600; }"));
        resultItem->setToolTip(QStringLiteral(
            "%1\n目标故障位：%2；外部中断：%3；清除脉冲：%4。")
            .arg(liveText,
                 (statusMask & 0x0008U) != 0U ? QStringLiteral("已触发") : QStringLiteral("未触发"),
                 (statusMask & 0x0020U) != 0U ? QStringLiteral("已捕获") : QStringLiteral("未捕获"),
                 clearPulseSeen ? QStringLiteral("已输出") : QStringLiteral("未输出")));
        if(phase != m_protectionLivePhase) {
            m_protectionLivePhase = phase;
            appendLog(LogChannel::External,
                      QStringLiteral("保护阶段：%1，ADC=%2V，故障掩码=%3，BRK=%4，CLR=%5。")
                          .arg(phaseText)
                          .arg(response.measuredValue, 0, 'f', 3)
                          .arg(hex16(currentFaultMask),
                               currentBreakHigh ? QStringLiteral("高") : QStringLiteral("低"),
                               clearOutputHigh ? QStringLiteral("高") : QStringLiteral("低")));
            if(phase == 3U) {
                const QString identifiedText = initialBreakHigh ?
                    QStringLiteral(
                        "DSP已识别ADC阈值和对应保护故障位。D_BRK在测试前已为高，本次不能验证其新上升沿和外部中断。\n") :
                    QStringLiteral(
                        "DSP已识别ADC阈值、对应故障位、D_BRK和外部中断。\n");
                QMessageBox::information(
                    this,
                    QStringLiteral("故障已识别"),
                    identifiedText + QStringLiteral(
                        "请立即把信号发生器恢复到当前项目的正常值，DSP将自动输出D_ERR_CLR并完成测试。"));
            }
        }
        return;
    }
    if(m_testAvailability.value(
           response.recordId,
           DspTestProtocol::TestAvailability::NotAvailable) !=
       DspTestProtocol::TestAvailability::NotAvailable &&
       !duplicateProtectionFinalLog) {
        appendScopeLog(logScope,
                       QStringLiteral("%1: %2，错误码=%3")
                           .arg(hex16(response.recordId),
                                resultText,
                                hex16(response.recordErrorCode)));
    }

    if(response.recordId == TestDriverFaultDi) {
        const quint16 highMask = static_cast<quint16>(
            (response.rawValue >> 16U) & 0x000FU);
        const quint16 lowMask = static_cast<quint16>(
            response.rawValue & 0x000FU);
        const quint16 transitionMask = static_cast<quint16>(
            static_cast<quint32>(response.measuredValue) & 0x000FU);
        const quint16 monitoredMask = static_cast<quint16>(response.expectedMin) & 0x000FU;
        const quint16 verifiedMask =
            highMask & lowMask & transitionMask & monitoredMask;
        updateDriverFaultChannelStatus(monitoredMask, verifiedMask, true);
        const QString summary = m_driverFaultStatusLabel->text();
        const bool requestMismatch =
            m_driverFaultPendingMask != 0U &&
            monitoredMask != m_driverFaultPendingMask;
        resultItem->setToolTip(summary + QStringLiteral(
            "\n主结果代表当前所选X3或X4组；高/低电平及翻转均为通道位掩码。"));
        appendScopeLog(
            RefreshScope::External,
            QStringLiteral("驱动故障DI诊断：监测=%1，高电平=%2，低电平=%3，翻转=%4。%5%6")
                .arg(hex16(monitoredMask),
                     hex16(highMask),
                     hex16(lowMask),
                     hex16(transitionMask),
                     summary,
                     requestMismatch ?
                         QStringLiteral("。DSP返回的监测掩码与Qt选择不一致，请重新下载最新DSP固件") :
                         QString()));
    }

    if((response.recordId == TestProtectionFaultDi) &&
       protectionRecordFormatCurrent &&
       (response.recordResult != DspTestProtocol::Result::NotRun)) {
        const quint8 returnedSelection = static_cast<quint8>(
            (response.rawValue >> 28U) & 0x000FU);
        const quint16 statusMask = static_cast<quint16>(
            (response.rawValue >> 16U) & 0x01FFU);
        const quint16 triggeredFaultMask = static_cast<quint16>(
            (response.rawValue >> 8U) & 0x000FU);
        quint16 expectedFaultBit = static_cast<quint16>(
            (response.rawValue >> 12U) & 0x000FU);
        if(expectedFaultBit == 0U) {
            expectedFaultBit = protectionFaultBitForSelection(returnedSelection);
        }
        const bool initialBreakHigh =
            ((response.rawValue >> 5U) & 0x0001U) != 0U;
        const bool finalBreakHigh =
            ((response.rawValue >> 4U) & 0x0001U) != 0U;
        const quint16 finalNormalMask = static_cast<quint16>(
            response.rawValue & 0x000FU);
        const QString statusText = QStringLiteral(
            "配置%1，初始正常%2，阈值%3，故障位%4，D_BRK%5，外部中断%6，"
            "清除脉冲%7，故障解除%8，安全结束%9")
            .arg((statusMask & 0x0001U) != 0U ? QStringLiteral("完成") : QStringLiteral("缺失"))
            .arg((statusMask & 0x0002U) != 0U ? QStringLiteral("通过") : QStringLiteral("未通过"))
            .arg((statusMask & 0x0004U) != 0U ? QStringLiteral("触发") : QStringLiteral("未触发"))
            .arg((statusMask & 0x0008U) != 0U ? QStringLiteral("触发") : QStringLiteral("未触发"))
            .arg((statusMask & 0x0010U) != 0U ? QStringLiteral("触发") : QStringLiteral("未触发"))
            .arg((statusMask & 0x0020U) != 0U ? QStringLiteral("已捕获") : QStringLiteral("未捕获"))
            .arg((statusMask & 0x0040U) != 0U ? QStringLiteral("已输出") : QStringLiteral("未输出"))
            .arg((statusMask & 0x0080U) != 0U ? QStringLiteral("通过") : QStringLiteral("未通过"))
            .arg((statusMask & 0x0100U) != 0U ? QStringLiteral("通过") : QStringLiteral("未通过"));
        const QString selectionText =
            (!m_protectionPendingText.isEmpty() &&
             returnedSelection == m_protectionPendingSelection) ?
                m_protectionPendingText :
                QStringLiteral("保护选择编号%1").arg(returnedSelection);
        QString reason = (response.recordResult == DspTestProtocol::Result::Pass) ?
            QStringLiteral("保护链完整") : QStringLiteral("保护链未完整通过");
        if((statusMask & 0x0002U) == 0U) {
            QStringList baselineFailures;
            if((finalNormalMask & expectedFaultBit) == 0U) {
                baselineFailures.append(
                    protectionFaultInputName(expectedFaultBit) +
                    QStringLiteral("为低电平故障态"));
            }
            if(baselineFailures.isEmpty()) {
                baselineFailures.append(QStringLiteral("正常电压未连续稳定成立"));
            }
            reason = QStringLiteral("基线失败：%1")
                .arg(baselineFailures.join(QStringLiteral("、")));
        }
        if((response.recordResult != DspTestProtocol::Result::Pass) &&
           ((statusMask & 0x0004U) != 0U)) {
            if(initialBreakHigh && ((statusMask & 0x0008U) != 0U)) {
                reason = QStringLiteral("BRK预置高，无法验证新边沿/XINT");
            } else {
                QStringList missingSignals;
                if((statusMask & 0x0008U) == 0U) {
                    missingSignals.append(QStringLiteral("保护故障位"));
                }
                if((statusMask & 0x0010U) == 0U) {
                    missingSignals.append(QStringLiteral("BRK"));
                }
                if((statusMask & 0x0020U) == 0U) {
                    missingSignals.append(QStringLiteral("XINT"));
                }
                reason = QStringLiteral("10秒响应超时：%1")
                    .arg(missingSignals.isEmpty() ?
                             QStringLiteral("完整保护链") :
                             missingSignals.join(QStringLiteral("、")));
            }
        }
        const QString compactStatus = QStringLiteral(
            "%1 | ADC %2V | 故障位 %3\nBRK %4 | XINT %5 | CLR %6 | %7")
            .arg(response.recordResult == DspTestProtocol::Result::Pass ?
                     QStringLiteral("通过") : QStringLiteral("未通过"))
            .arg(response.measuredValue, 0, 'f', 3)
            .arg((statusMask & 0x0008U) != 0U ? QStringLiteral("已触发") : QStringLiteral("未触发"),
                 (statusMask & 0x0010U) != 0U ? QStringLiteral("已触发") : QStringLiteral("未触发"),
                 (statusMask & 0x0020U) != 0U ? QStringLiteral("已捕获") : QStringLiteral("未捕获"),
                 (statusMask & 0x0040U) != 0U ? QStringLiteral("已输出") : QStringLiteral("未输出"),
                 reason);
        m_protectionStatusLabel->setText(compactStatus);
        m_protectionStatusLabel->setStyleSheet(
            response.recordResult == DspTestProtocol::Result::Pass ?
                QStringLiteral("QLabel { background: #E8F5E9; border: 1px solid #66BB6A; "
                               "border-radius: 4px; padding: 6px 8px; font-weight: 600; }") :
                QStringLiteral("QLabel { background: #FFEBEE; border: 1px solid #EF5350; "
                               "border-radius: 4px; padding: 6px 8px; font-weight: 600; }"));
        resultItem->setToolTip(QStringLiteral(
            "%1\n%2；目标=%3；触发掩码=%4；最终正常掩码=%5；BRK初始/最终=%6/%7。")
            .arg(compactStatus,
                 statusText,
                 protectionFaultInputName(expectedFaultBit),
                 hex16(triggeredFaultMask),
                 hex16(finalNormalMask),
                 initialBreakHigh ? QStringLiteral("高") : QStringLiteral("低"),
                 finalBreakHigh ? QStringLiteral("高") : QStringLiteral("低")));
        if(!duplicateProtectionFinalLog) {
            appendScopeLog(
                RefreshScope::External,
                QStringLiteral("保护诊断：%1 | %2 | ADC=%3V | %4。")
                    .arg(response.recordResult == DspTestProtocol::Result::Pass ?
                             QStringLiteral("PASS") : QStringLiteral("FAIL"),
                         selectionText)
                    .arg(response.measuredValue, 0, 'f', 3)
                    .arg(reason));
        }
    }

    if((response.recordId == TestAdcExternalGroup1) ||
       (response.recordId == TestAdcExternalGroup2)) {
        const quint16 passedMask = static_cast<quint16>(
            (response.rawValue >> 16U) & 0x007FU);
        const quint16 failedMask = static_cast<quint16>(
            response.rawValue & 0x007FU);
        const QStringList signalNames =
            (response.recordId == TestAdcExternalGroup1) ?
                QStringList{QStringLiteral("IU1"), QStringLiteral("IV1"),
                            QStringLiteral("IW1"), QStringLiteral("UDC1"),
                            QStringLiteral("T_INV1"), QStringLiteral("UUV1"),
                            QStringLiteral("UVW1")} :
                QStringList{QStringLiteral("IU2"), QStringLiteral("IV2"),
                            QStringLiteral("IW2"), QStringLiteral("UDC2"),
                            QStringLiteral("T_INV2"), QStringLiteral("UUV2"),
                            QStringLiteral("UVW2")};
        QStringList failedSignals;
        for(int index = 0; index < signalNames.size(); ++index) {
            if((failedMask & (1U << index)) != 0U) {
                failedSignals.append(signalNames.at(index));
            }
        }
        appendScopeLog(
            RefreshScope::External,
            QStringLiteral("ADC批量结果：通过掩码=%1，失败掩码=%2%3")
                .arg(hex16(passedMask),
                     hex16(failedMask),
                     failedSignals.isEmpty() ? QString() :
                         QStringLiteral("，失败通道=%1")
                              .arg(failedSignals.join(QStringLiteral("、")))));
    }

    if((response.recordId == TestAdcPt100Mux) &&
       (response.recordResult != DspTestProtocol::Result::NotRun)) {
        const quint8 selectionId = static_cast<quint8>(
            (response.rawValue >> 24U) & 0x00FFU);
        const quint8 signalId = static_cast<quint8>(
            (response.rawValue >> 16U) & 0x00FFU);
        const quint16 averageRaw = static_cast<quint16>(
            response.rawValue & 0xFFFFU);
        const bool recordFormatCurrent =
            (selectionId == (AdcExternalSelectionPt100Base + 3U)) &&
            (signalId == 8U);
        if(recordFormatCurrent) {
            const double ratio = (180.0 / 2180.0) -
                                 (response.measuredValue / 65.0);
            const double resistance =
                (ratio > 0.0 && ratio < 1.0) ?
                    (2000.0 * ratio) / (1.0 - ratio) : -1.0;
            const QString diagnostic = QStringLiteral(
                "PT4温度诊断：S1:S0=11，ADCINA3原始值=%1，实测=%2V，"
                "允许范围=%3~%4V，反算电阻=%5Ω。")
                .arg(averageRaw)
                .arg(response.measuredValue, 0, 'f', 3)
                .arg(response.expectedMin, 0, 'f', 3)
                .arg(response.expectedMax, 0, 'f', 3)
                .arg(resistance, 0, 'f', 1);
            resultItem->setToolTip(diagnostic);
            appendScopeLog(RefreshScope::External, diagnostic);
        } else {
            const QString mismatchText = QStringLiteral(
                "PT4记录格式仍为旧版：请下载最新HPD_BoardTest_DSP.out后重新测试。"
                "新版将直接显示ADCINA3实测电压和反算电阻。");
            resultItem->setToolTip(mismatchText);
            appendScopeLog(RefreshScope::External, mismatchText);
        }
    }

    if(response.recordId == TestPwmDido) {
        m_pwmDidoLiveTimer->stop();
        m_pwmDidoLastLiveLevel = -1;
        const quint16 gpioHighMask = static_cast<quint16>(
            (response.rawValue >> 16U) & 0x0FFFU);
        const quint16 statusMask = static_cast<quint16>(
            response.rawValue & 0xFFFFU);
        const quint16 requestedMask = static_cast<quint16>(
            response.expectedMin) & 0x0FFFU;
        if(response.recordResult == DspTestProtocol::Result::NotRun) {
            m_pwmDidoLiveValueLabel->setText(
                QStringLiteral("待测试 | 端子：直流档，COM接DGND"));
        } else {
            m_pwmDidoLiveValueLabel->setText(
                QStringLiteral("已结束 | GPIO 低 | 输出已关闭\n"
                               "高电平读回 %1 | 等待端子确认")
                    .arg(hex16(gpioHighMask)));
        }
        m_pwmDidoLiveValueLabel->setStyleSheet(QStringLiteral(
            "QLabel { background: #E3F2FD; border: 1px solid #42A5F5; "
            "border-radius: 4px; padding: 6px 8px; font-weight: 600; }"));
        appendScopeLog(
            RefreshScope::External,
            QStringLiteral("PWM-DIDO诊断：通道=%1，请求掩码=%2，DSP GPIO高电平读回=%3，状态=%4。%5")
                .arg(m_pwmDidoPendingChannelText,
                     hex16(requestedMask),
                      hex16(gpioHighMask),
                      hex16(statusMask),
                      (requestedMask != 0U && gpioHighMask == requestedMask) ?
                          QStringLiteral("DSP侧高电平读回符合请求，但不代表外部端子已有5V；先测U11/U12的D_ENPWM1/2（/OE）必须为低，再依次检查4245的5V侧PWMx和U34~U39输出。") :
                          QStringLiteral("DSP侧高电平读回未确认通过，请核对测试是否执行、GPIO复用和方向配置。")));
    }

    if(m_ethernetTcpRecordRefreshPending &&
       (response.recordId == m_ethernetTcpCurrentTestId)) {
        m_ethernetTcpRecordRefreshPending = false;
        QTimer::singleShot(100, this, &MainWindow::startNextEthernetTcpTest);
        return;
    }

    if (isManualEdoConfirmation) {
        if (m_edoManualConfirmationExpected) {
            m_edoManualConfirmationAvailable = true;
        }
        appendScopeLog(
            RefreshScope::External,
            (m_boardProfileId == 0x0003U) ?
                QStringLiteral("DO人工确认：请观察与 EX_DOP1、EX_DOP2、EX_DOP3 相连的"
                               "PLC DI 指示灯，应依次亮约2秒后熄灭。") :
                QStringLiteral("EDO人工确认：请观察PLC的DI3、DI4、DI5指示灯，"
                               "应依次亮约2秒后熄灭。三路均符合时，判定EDO外部输出通过。"));
        if (isSingleTestRecord) {
            showManualOutputConfirmation(
                TestDido,
                QStringLiteral("DO目视确认"),
                (m_boardProfileId == 0x0003U) ?
                    QStringLiteral("与 EX_DOP1、EX_DOP2、EX_DOP3 相连的 PLC DI 指示灯"
                                   "是否依次亮约2秒后熄灭？\n"
                                   "选择“是”将把本次DO结果标记为PASS。") :
                    QStringLiteral("PLC的DI3、DI4、DI5指示灯是否依次亮约2秒后熄灭？\n"
                                   "选择“是”将把本次EDO结果标记为PASS。"),
                QStringLiteral("DO"));
        }
    }

    if (isManualPwmConfirmation) {
        appendScopeLog(
            RefreshScope::External,
            QStringLiteral("PWM-DIDO人工确认：%1 测试期间应输出约2秒的5V并回到0V；确认框出现时输出已结束，请按刚才的实测结果判定。")
                .arg(m_pwmDidoPendingChannelText));
        if (isSingleTestRecord) {
            showManualOutputConfirmation(
                TestPwmDido,
                QStringLiteral("PWM-DIDO目视确认"),
                QStringLiteral("刚才测试期间，当前通道：\n%1\n"
                               "是否曾输出约5V并回到0V？\n"
                               "确认框出现时输出已结束，此时测到0V不能判断此前是否输出。\n"
                               "选择“是”只表示当前所选通道通过。")
                    .arg(m_pwmDidoPendingChannelText),
                QStringLiteral("PWM-DIDO"));
        }
    }

    if (isManualDriverResetConfirmation) {
        const quint16 statusMask = static_cast<quint16>(
            (response.rawValue >> 16U) & 0xFFFFU);
        const quint16 failCode = static_cast<quint16>(
            response.rawValue & 0xFFFFU);
        const quint16 requestedMask = static_cast<quint16>(
            response.expectedMin) & 0x000FU;
        m_driverResetLastLiveLevel = -1;
        m_driverResetLiveValueLabel->setText(
            ((statusMask & 0x0008U) != 0U) ?
                QStringLiteral("复位GPIO实时值：测试结束，已恢复低电平") :
                QStringLiteral("复位GPIO实时值：测试结束，但安全低电平读回异常"));
        appendScopeLog(
            RefreshScope::External,
            QStringLiteral("驱动复位诊断：通道=%1，请求掩码=%2，状态=%3，内部失败码=%4。"
                           "状态包含高/低电平读回，但仍需依据外部端子万用表实测确认。")
                .arg(m_driverResetPendingChannelText,
                     hex16(requestedMask),
                     hex16(statusMask),
                     hex16(failCode)));
        if (isSingleTestRecord) {
            showManualOutputConfirmation(
                TestDriverResetDo,
                QStringLiteral("驱动复位输出确认"),
                QStringLiteral("刚才测试期间，当前通道：\n%1\n"
                               "是否曾输出高电平约10秒并恢复到0V？\n"
                               "选择“是”只表示当前所选复位输出通过。")
                    .arg(m_driverResetPendingChannelText),
                QStringLiteral("驱动复位"));
        }
    }

    if (isSingleTestRecord) {
        appendScopeLog(logScope, QStringLiteral("单项测试记录刷新完成。"));
        m_singleTestRecordRefreshInFlight = false;
        m_liveRecordRequestPending = false;
        m_singleTestRefreshScope = RefreshScope::None;
        m_singleTestRefreshId = DspTestProtocol::InvalidTestId;
        m_refreshScope = RefreshScope::None;
    }
}

void MainWindow::requestEdoManualConfirmation()
{
    if (!m_edoManualConfirmationExpected) {
        return;
    }

    m_edoManualConfirmationExpected = false;
    if (!m_edoManualConfirmationAvailable) {
        appendLog(LogChannel::External,
                  QStringLiteral("EDO未进入人工确认：DSP未返回可供人工确认的EDO执行结果。"));
        return;
    }

    m_edoManualConfirmationAvailable = false;
    showManualOutputConfirmation(
        TestDido,
        QStringLiteral("DO目视确认"),
        (m_boardProfileId == 0x0003U) ?
            QStringLiteral("请确认与 EX_DOP1、EX_DOP2、EX_DOP3 相连的 PLC DI 指示灯"
                           "是否依次亮约2秒后熄灭。\n"
                           "确认通过将把本次DO结果标记为PASS。") :
            QStringLiteral("请确认PLC的DI3、DI4、DI5指示灯是否依次亮约2秒后熄灭。\n"
                           "确认通过将把本次EDO结果标记为PASS。"),
        QStringLiteral("DO"));
}

void MainWindow::showManualOutputConfirmation(quint16 testId,
                                               const QString &title,
                                               const QString &prompt,
                                               const QString &testName)
{
    if (m_manualOutputDialog) {
        return;
    }

    auto *dialog = new QMessageBox(QMessageBox::Question, title, prompt,
                                   QMessageBox::Yes | QMessageBox::No, this);
    dialog->setDefaultButton(QMessageBox::No);
    dialog->setEscapeButton(QMessageBox::No);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    m_manualOutputDialog = dialog;
    connect(dialog, &QDialog::finished, this,
            [this, dialog, testId, testName](int decision) {
                if (m_manualOutputDialog != dialog) {
                    return;
                }
                m_manualOutputDialog.clear();
                if (testId == TestDido) {
                    m_edoManualConfirmationExpected = false;
                    m_edoManualConfirmationAvailable = false;
                }
                const auto row = m_recordRows.constFind(testId);
                if (row == m_recordRows.cend() ||
                    row->table->item(row->row, 7)->text() !=
                        QStringLiteral("MANUAL_CONFIRM")) {
                    return;
                }
                setManualOutputResult(testId, decision == QMessageBox::Yes,
                                      testName);
            });
    // Called from the socket readyRead stack. A nested exec() prevents further
    // readyRead delivery while the status polling/response timers keep running.
    dialog->open();
}

void MainWindow::cancelManualOutputConfirmation()
{
    if (m_manualOutputDialog) {
        auto *dialog = m_manualOutputDialog.data();
        // Invalidate before closing so an old answer cannot affect a new run.
        m_manualOutputDialog.clear();
        dialog->reject();
    }
}

void MainWindow::setManualOutputResult(quint16 testId,
                                       bool passed,
                                       const QString &testName)
{
    const auto rowIterator = m_recordRows.constFind(testId);
    if (rowIterator == m_recordRows.cend()) {
        return;
    }

    const RecordLocation location = rowIterator.value();
    auto *resultItem = location.table->item(location.row, 7);
    resultItem->setText(passed ? QStringLiteral("PASS") : QStringLiteral("FAIL"));
    updateResultAppearance(resultItem,
                           passed ? DspTestProtocol::Result::Pass :
                                    DspTestProtocol::Result::Fail,
                           false);
    resultItem->setToolTip(
        passed ? QStringLiteral("人工目视确认通过；DSP原始结果为WARN，错误码为0。") :
                 QStringLiteral("人工目视确认未通过；DSP原始结果为WARN，错误码为0。"));
    appendLog(LogChannel::External,
               passed ? QStringLiteral("%1人工目视确认通过：上位机结果已标记为PASS。")
                            .arg(testName) :
                        QStringLiteral("%1人工目视确认未通过：上位机结果已标记为FAIL。")
                            .arg(testName));
}

void MainWindow::updateResultAppearance(QTableWidgetItem *item,
                                        DspTestProtocol::Result result,
                                        bool manualConfirmation) const
{
    QFont font = item->font();

    item->setBackground(QBrush());
    item->setForeground(QBrush());
    font.setBold(false);

    if(manualConfirmation)
    {
        item->setFont(font);
        return;
    }

    switch(result)
    {
    case DspTestProtocol::Result::Pass:
        item->setBackground(QBrush(QColor(232, 245, 233)));
        item->setForeground(QBrush(QColor(46, 125, 50)));
        font.setBold(true);
        break;

    case DspTestProtocol::Result::Fail:
    case DspTestProtocol::Result::Timeout:
    case DspTestProtocol::Result::SafetyLocked:
        item->setBackground(QBrush(QColor(255, 235, 238)));
        item->setForeground(QBrush(QColor(198, 40, 40)));
        font.setBold(true);
        break;

    case DspTestProtocol::Result::NotRun:
    case DspTestProtocol::Result::Skip:
    case DspTestProtocol::Result::NotSupported:
        item->setBackground(QBrush(QColor(245, 245, 245)));
        item->setForeground(QBrush(QColor(110, 110, 110)));
        break;

    case DspTestProtocol::Result::Running:
    case DspTestProtocol::Result::Warn:
        break;
    }

    item->setFont(font);
}

void MainWindow::updateStatus(const DspTestProtocol::Response &response)
{
    statusBar()->showMessage(
        QStringLiteral("模式=%1 阶段=%2 活动Test=%3 上次命令=%4")
            .arg(response.managerMode)
             .arg(response.activeStage)
             .arg(hex16(response.activeTestId))
             .arg(response.lastCommandResult));

    if (response.command == DspTestProtocol::Command::GetStatus) {
        updateCommunicationStandbyStatus(response);
    }
}

void MainWindow::updateBoardInfo(const DspTestProtocol::Response &response)
{
    m_detectedEthernetInterface = response.boardEthernetInterface;
    m_boardProfileState = response.boardProfileState;
    m_boardProfileId = response.boardProfileId;
    updateBoardProfileChoices(response);
    if(response.boardProfileState ==
       DspTestProtocol::BoardProfileState::Confirmed) {
        const bool resultBoardChanged =
            m_resultBoardContextValid &&
            ((m_resultBoardProfileId != response.boardProfileId) ||
             (m_resultBoardHardwareRevision !=
              response.boardHardwareRevision) ||
             (m_resultBoardPinMapRevision != response.boardPinMapRevision) ||
             (m_resultBoardCapabilities != response.boardCapabilities));
        if(resultBoardChanged) {
            resetDisplayedResultsForBoardChange();
            appendLog(LogChannel::Communication,
                      QStringLiteral("检测到板型或硬件版本已切换，已清除上一块板的测试结果并重新同步可测试项。"));
        }
        m_resultBoardContextValid = true;
        m_resultBoardProfileId = response.boardProfileId;
        m_resultBoardHardwareRevision = response.boardHardwareRevision;
        m_resultBoardPinMapRevision = response.boardPinMapRevision;
        m_resultBoardCapabilities = response.boardCapabilities;

        const bool profileChanged =
            (m_availabilityProfileId != response.boardProfileId) ||
            (m_availabilityHardwareRevision != response.boardHardwareRevision) ||
            (m_availabilityPinMapRevision != response.boardPinMapRevision) ||
            (m_availabilityCapabilities != response.boardCapabilities);
        const bool availabilitySyncInProgress =
            m_nextTestAvailabilityIndex >= 0;
        if(profileChanged ||
           (!m_testAvailabilityReady && !availabilitySyncInProgress)) {
            beginTestAvailabilityRefresh();
            m_availabilityProfileId = response.boardProfileId;
            m_availabilityHardwareRevision = response.boardHardwareRevision;
            m_availabilityPinMapRevision = response.boardPinMapRevision;
            m_availabilityCapabilities = response.boardCapabilities;
        } else if(m_testAvailabilityReady) {
            appendLog(LogChannel::Communication,
                      QStringLiteral("沿用本次软件运行期间已同步的 Test ID 状态和测试结果。"));
        }
    }

    appendLog(LogChannel::Communication,
              QStringLiteral("引导网络：%1；板型状态：%2。")
                  .arg(ethernetInterfaceText(response.boardEthernetInterface),
                       boardProfileStateText(response.boardProfileState)));
    if(response.boardProfileId != 0U) {
        appendLog(
            LogChannel::Communication,
            QStringLiteral("当前板型：%1（ID=%2，硬件版本=%3，引脚表修订=%4）。")
                .arg(boardProfileName(response.boardProfileId),
                     hex16(response.boardProfileId),
                     QString::number(response.boardHardwareRevision),
                     QString::number(response.boardPinMapRevision)));
        appendLog(LogChannel::Communication,
                  QStringLiteral("板型能力：%1。")
                      .arg(boardCapabilityText(response.boardCapabilities)));
    } else {
        appendLog(LogChannel::Communication,
                  QStringLiteral("DSP尚未确认板型，所有测试命令保持安全锁定。"));
    }
}

void MainWindow::resetDisplayedResultsForBoardChange()
{
    cancelManualOutputConfirmation();
    m_pwmDidoLiveTimer->stop();
    m_protectionResetLiveTimer->stop();
    m_liveRecordRequestPending = false;
    m_pwmDidoLastLiveLevel = -1;
    m_driverResetLastLiveLevel = -1;
    m_pwmDidoLiveValueLabel->setText(
        QStringLiteral("待测试 | 端子：直流档，COM接DGND"));
    m_pwmDidoLiveValueLabel->setStyleSheet(QStringLiteral(
        "QLabel { background: #F5F7FA; border: 1px solid #CDD5DF; "
        "border-radius: 4px; padding: 6px 8px; }"));
    m_protectionStatusLabel->setText(
        QStringLiteral("待测试 | COM接同组AGND | 低电平=故障"));
    m_protectionStatusLabel->setStyleSheet(QStringLiteral(
        "QLabel { background: #F5F7FA; border: 1px solid #CDD5DF; "
        "border-radius: 4px; padding: 6px 8px; }"));
    m_lastProtectionFinalLogKey.clear();
    m_driverResetLiveValueLabel->setText(
        QStringLiteral("复位GPIO实时值：待测试（数字逻辑折算，不是端子电压实测值）"));
    m_driverFaultPendingMask = 0U;
    updateDriverFaultChannelStatus(0U, 0U, false);
    for(auto iterator = m_recordRows.cbegin();
        iterator != m_recordRows.cend(); ++iterator) {
        const RecordLocation location = iterator.value();
        location.table->item(location.row, 2)->setText(
            QStringLiteral("0x0000"));
        location.table->item(location.row, 3)->setText(
            QStringLiteral("0x00000000"));
        location.table->item(location.row, 4)->setText(
            QStringLiteral("0.000"));
        location.table->item(location.row, 5)->setText(
            QStringLiteral("0.000"));
        location.table->item(location.row, 6)->setText(
            QStringLiteral("0.000"));
        auto *resultItem = location.table->item(location.row, 7);
        resultItem->setText(QStringLiteral("NOT_RUN"));
        updateResultAppearance(resultItem,
                               DspTestProtocol::Result::NotRun,
                               false);
    }

    m_refreshScope = RefreshScope::None;
    m_waitingForTestReconnect = false;
    m_activeRecordIds.clear();
    m_nextRecordIndex = -1;
    m_singleTestRefreshPending = false;
    m_singleTestStartStatusRequested = false;
    m_singleTestObservedRunning = false;
    m_singleTestRecordRefreshInFlight = false;
    m_singleTestRefreshScope = RefreshScope::None;
    m_singleTestRefreshId = DspTestProtocol::InvalidTestId;
    m_edoManualConfirmationExpected = false;
    m_edoManualConfirmationAvailable = false;
    resetEthernetTcpSequence();
}

void MainWindow::beginTestAvailabilityRefresh()
{
    resetTestAvailability();
    m_testAvailabilityQueryIds = m_boardRecordIds;
    m_testAvailabilityQueryIds += m_externalRecordIds;
    m_testAvailabilityQueryIds += m_hpdRecordIds;
    m_nextTestAvailabilityIndex = 0;
    appendLog(LogChannel::Communication,
              QStringLiteral("正在同步各 Test ID 的板型可用状态。"));
    requestNextTestAvailability();
}

void MainWindow::requestNextTestAvailability()
{
    if (!m_client.isConnected() || (m_nextTestAvailabilityIndex < 0)) {
        return;
    }

    if (m_nextTestAvailabilityIndex >= m_testAvailabilityQueryIds.size()) {
        int notAvailableCount = 0;
        int pendingDevelopmentCount = 0;
        int pendingValidationCount = 0;
        int testableCount = 0;

        for (auto iterator = m_testAvailability.cbegin();
             iterator != m_testAvailability.cend(); ++iterator) {
            switch (iterator.value()) {
            case DspTestProtocol::TestAvailability::NotAvailable:
                ++notAvailableCount;
                break;
            case DspTestProtocol::TestAvailability::PendingDevelopment:
                ++pendingDevelopmentCount;
                break;
            case DspTestProtocol::TestAvailability::PendingValidation:
                ++pendingValidationCount;
                break;
            case DspTestProtocol::TestAvailability::Testable:
                ++testableCount;
                break;
            }
        }

        m_nextTestAvailabilityIndex = -1;
        m_testAvailabilityReady = true;
        switch (static_cast<DspTestProtocol::Stage>(
                    m_singleStageBox->currentData().toUInt())) {
        case DspTestProtocol::Stage::BoardOnly:
            updateSingleTestChoices(RefreshScope::Board);
            break;
        case DspTestProtocol::Stage::ExternalConnected:
            updateSingleTestChoices(RefreshScope::External);
            break;
        case DspTestProtocol::Stage::HpdInjection:
            updateSingleTestChoices(RefreshScope::Hpd);
            break;
        }
        updateTestCommandAvailability();
        appendLog(LogChannel::Communication,
                  QStringLiteral("Test ID 状态同步完成：可测试 %1，待验证 %2，待开发 %3，不具备 %4。")
                      .arg(testableCount)
                      .arg(pendingValidationCount)
                      .arg(pendingDevelopmentCount)
                      .arg(notAvailableCount));
        if ((m_refreshScope != RefreshScope::None) &&
            m_waitingForTestReconnect && m_client.isConnected()) {
            m_client.requestStatus();
        }
        return;
    }

    m_client.requestTestAvailability(
        m_testAvailabilityQueryIds.at(m_nextTestAvailabilityIndex++));
}

void MainWindow::updateTestAvailability(
    const DspTestProtocol::Response &response)
{
    const auto locationIterator = m_recordRows.constFind(response.recordId);
    if (locationIterator == m_recordRows.cend()) {
        m_nextTestAvailabilityIndex = -1;
        return;
    }

    m_testAvailability.insert(response.recordId, response.testAvailability);
    m_testEnabledInAuto.insert(response.recordId,
                               response.testEnabledInAuto);
    const RecordLocation location = locationIterator.value();
    const bool notAvailable = response.testAvailability ==
        DspTestProtocol::TestAvailability::NotAvailable;
    location.table->setRowHidden(location.row, notAvailable);

    auto *resultItem = location.table->item(location.row, 7);
    const QString currentText = resultItem->text();
    bool showsAvailability = currentText == QStringLiteral("NOT_RUN");
    for(int value = static_cast<int>(
            DspTestProtocol::TestAvailability::NotAvailable);
        value <= static_cast<int>(
            DspTestProtocol::TestAvailability::Testable);
        ++value) {
        if(currentText == DspTestProtocol::testAvailabilityText(
                              static_cast<DspTestProtocol::TestAvailability>(
                                  value))) {
            showsAvailability = true;
            break;
        }
    }

    if(showsAvailability) {
        resultItem->setText(
            DspTestProtocol::testAvailabilityText(response.testAvailability));
        QFont font = resultItem->font();
        font.setBold(response.testAvailability !=
                     DspTestProtocol::TestAvailability::NotAvailable);
        resultItem->setFont(font);
        if(response.testAvailability ==
           DspTestProtocol::TestAvailability::PendingDevelopment) {
            resultItem->setBackground(QBrush(QColor(245, 245, 245)));
            resultItem->setForeground(QBrush(QColor(100, 100, 100)));
        } else if(response.testAvailability ==
                  DspTestProtocol::TestAvailability::PendingValidation) {
            resultItem->setBackground(QBrush(QColor(255, 248, 225)));
            resultItem->setForeground(QBrush(QColor(239, 108, 0)));
        } else if(response.testAvailability ==
                  DspTestProtocol::TestAvailability::Testable) {
            resultItem->setBackground(QBrush(QColor(227, 242, 253)));
            resultItem->setForeground(QBrush(QColor(21, 101, 192)));
        }
    }

    requestNextTestAvailability();
}

void MainWindow::resetTestAvailability()
{
    m_testAvailability.clear();
    m_testEnabledInAuto.clear();
    m_testAvailabilityQueryIds.clear();
    m_nextTestAvailabilityIndex = -1;
    m_testAvailabilityReady = false;
    m_availabilityProfileId = 0U;
    m_availabilityHardwareRevision = 0U;
    m_availabilityPinMapRevision = 0U;
    m_availabilityCapabilities = 0U;
    for (auto iterator = m_recordRows.cbegin();
         iterator != m_recordRows.cend(); ++iterator) {
        iterator.value().table->setRowHidden(iterator.value().row, false);
    }
    m_singleTestBox->clear();
    updateTestCommandAvailability();
}

bool MainWindow::isTestRunnable(quint16 testId) const
{
    if (!m_testAvailabilityReady) {
        return false;
    }

    const auto availability = m_testAvailability.value(
        testId,
        DspTestProtocol::TestAvailability::NotAvailable);
    return (availability == DspTestProtocol::TestAvailability::Testable) ||
           (availability ==
            DspTestProtocol::TestAvailability::PendingValidation);
}

bool MainWindow::isTestEnabledInAuto(quint16 testId) const
{
    return m_testAvailabilityReady &&
           m_testEnabledInAuto.value(testId, false);
}

void MainWindow::updateBoardProfileChoices(
    const DspTestProtocol::Response &response)
{
    const quint16 preferredId = response.boardProfileId;

    m_boardProfileBox->blockSignals(true);
    m_boardProfileBox->clear();
    m_boardProfileBox->addItem(QStringLiteral("请选择实际板型"), 0U);
    if(response.boardEthernetInterface ==
       DspTestProtocol::EthernetInterface::EmifW5300) {
        m_boardProfileBox->addItem(QStringLiteral("SiC多传系统主控板"), 0x0001U);
        m_boardProfileBox->addItem(QStringLiteral("低压工业变频器"), 0x0002U);
    }
    else if(response.boardEthernetInterface ==
            DspTestProtocol::EthernetInterface::SpicW5500) {
        m_boardProfileBox->addItem(QStringLiteral("低空经济统型板"), 0x0003U);
    }

    int selectedIndex = m_boardProfileBox->findData(preferredId);
    if(selectedIndex < 0) {
        selectedIndex = 0;
    }
    if(selectedIndex >= 0) {
        m_boardProfileBox->setCurrentIndex(selectedIndex);
    }
    m_boardProfileBox->blockSignals(false);
    updateTestCommandAvailability();
}

void MainWindow::updateTestCommandAvailability()
{
    const bool connected = m_client.isConnected();
    const bool confirmed =
        m_boardProfileState == DspTestProtocol::BoardProfileState::Confirmed;
    const bool selectable = connected &&
        (m_boardProfileState == DspTestProtocol::BoardProfileState::NetworkReady);
    const bool confirmable = connected &&
        ((m_boardProfileState == DspTestProtocol::BoardProfileState::NetworkReady) ||
         (m_boardProfileState ==
          DspTestProtocol::BoardProfileState::SelectedUnconfirmed)) &&
        (m_boardProfileBox->currentData().toUInt() != 0U);
    const bool clearable = connected &&
        ((m_boardProfileState ==
          DspTestProtocol::BoardProfileState::SelectedUnconfirmed) ||
         (m_boardProfileState == DspTestProtocol::BoardProfileState::Confirmed));

    m_boardProfileBox->setEnabled(selectable);
    m_confirmBoardProfileButton->setEnabled(confirmable);
    m_clearBoardProfileButton->setEnabled(clearable);
    const bool testsReady = confirmed && m_testAvailabilityReady;
    m_boardButton->setEnabled(testsReady);
    m_externalButton->setEnabled(testsReady);
    m_hpdButton->setEnabled(testsReady);
    m_hpdSourceBox->setEnabled(confirmed);
    m_singleStageBox->setEnabled(testsReady);
    m_singleTestBox->setEnabled(testsReady && (m_singleTestBox->count() > 0));
    m_singleButton->setEnabled(testsReady && (m_singleTestBox->count() > 0));
    m_stopButton->setEnabled(connected);
}

void MainWindow::refreshScibPorts()
{
    const QString previousPort = m_scibPortBox->currentData().toString();

    m_scibPortBox->clear();
    for (const QSerialPortInfo &portInfo : QSerialPortInfo::availablePorts()) {
        QString displayName = portInfo.portName();
        if (!portInfo.description().isEmpty()) {
            displayName += QStringLiteral(" (%1)").arg(portInfo.description());
        }
        m_scibPortBox->addItem(displayName, portInfo.portName());
    }

    int selectedIndex = m_scibPortBox->findData(previousPort);
    if (selectedIndex < 0) {
        selectedIndex = m_scibPortBox->findData(QStringLiteral("COM5"));
    }
    if (selectedIndex >= 0) {
        m_scibPortBox->setCurrentIndex(selectedIndex);
    }

    const bool hasPort = !m_scibPortBox->currentData().toString().isEmpty();
    appendLog(LogChannel::Communication,
              hasPort ? QStringLiteral("已刷新 COM 口：%1。")
                          .arg(m_scibPortBox->currentData().toString()) :
                        QStringLiteral("未发现可用的 COM 口。"));
}

bool MainWindow::startScibTest(bool automatic)
{
    const QString serialName = activeSerialName();
    const QString portName = m_scibPortBox->currentData().toString();
    if (portName.isEmpty()) {
        const QString message = QStringLiteral("未选择 %1 COM 口。请连接对应 USB 转换器后重试。")
                                    .arg(serialName);
        appendLog(LogChannel::Communication, message);
        if (!automatic) {
            QMessageBox::warning(this, serialName + QStringLiteral("测试"), message);
        }
        return false;
    }
    if (m_scibSerialClient.isActive()) {
        appendLog(LogChannel::Communication,
                  QStringLiteral("%1 测试正在进行。").arg(serialName));
        return false;
    }

    appendLog(LogChannel::Communication,
              automatic ? QStringLiteral("开始 Qt 自动 %1 收发测试。").arg(serialName) :
                          QStringLiteral("开始手动触发的 %1 Qt 收发测试。").arg(serialName));
    m_scibSerialClient.startTest(portName);
    return true;
}

void MainWindow::selectCanLibrary()
{
    const QString currentPath = m_canLibraryEdit->text();
    QSettings settings;
    const QString initialPath = QFileInfo(currentPath).isFile() ?
        currentPath :
        settings.value(QStringLiteral("zlgCan/libraryDirectory")).toString();
    const QString libraryPath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("选择 ZLG ControlCAN.dll"),
        initialPath,
        QStringLiteral("ZLG ControlCAN.dll (ControlCAN.dll)"));

    if (libraryPath.isEmpty()) {
        return;
    }

    m_canLibraryEdit->setText(libraryPath);
    settings.setValue(QStringLiteral("zlgCan/libraryPath"), libraryPath);
    settings.setValue(QStringLiteral("zlgCan/libraryDirectory"),
                      QFileInfo(libraryPath).absolutePath());
    settings.sync();
    appendLog(LogChannel::Communication,
              QStringLiteral("已选择 ZLG CAN 接口库：%1。").arg(libraryPath));
}

bool MainWindow::startCanTest(bool automatic)
{
    const QString libraryPath = m_canLibraryEdit->text();

    if (libraryPath.isEmpty()) {
        const QString message =
            QStringLiteral("未选择 ZLG ControlCAN.dll。请先选择 USBCAN-II 的 64 位接口库。");
        appendLog(LogChannel::Communication, message);
        if (!automatic) {
            QMessageBox::warning(this, QStringLiteral("ZLG CAN测试"), message);
        }
        return false;
    }

    if (m_zlgCanClient.isActive()) {
        appendLog(LogChannel::Communication, QStringLiteral("ZLG CAN 测试正在进行。"));
        return false;
    }

    QString canLibraryError;
    if(!m_zlgCanClient.prepareLibrary(libraryPath, &canLibraryError))
    {
        appendLog(LogChannel::Communication, canLibraryError);
        if(!automatic)
        {
            QMessageBox::warning(this, QStringLiteral("ZLG CAN 接口库"),
                                 canLibraryError);
        }
        return false;
    }

    appendLog(LogChannel::Communication,
              automatic ? QStringLiteral("开始 Qt 自动 ZLG CAN 收发测试。") :
                          QStringLiteral("开始手动触发的 ZLG CAN 收发测试。"));
    m_zlgCanClient.startTest(
        libraryPath,
        m_canDeviceTypeBox->currentData().toUInt(),
        m_canDeviceIndexBox->currentData().toUInt(),
        m_canChannelBox->currentData().toUInt());
    return true;
}

void MainWindow::updateCommunicationStandbyStatus(
    const DspTestProtocol::Response &response)
{
    const quint16 enabledMask = response.communicationStandbyEnabledMask;
    const quint16 waitingMask = response.communicationStandbyWaitingMask;
    const quint16 receivedMask = response.communicationStandbyReceivedMask;
    const quint16 repliedMask = response.communicationStandbyRepliedMask;
    const quint16 failedMask = response.communicationStandbyFailedMask;
    const bool stateChanged =
        (enabledMask != m_communicationEnabledMask) ||
        (waitingMask != m_communicationWaitingMask) ||
        (receivedMask != m_communicationReceivedMask) ||
        (repliedMask != m_communicationRepliedMask) ||
        (failedMask != m_communicationFailedMask);
    const quint16 canScibMask =
        DspTestProtocol::CommunicationStandbyCan |
        DspTestProtocol::CommunicationStandbyScib;
    const bool canChanged =
        (((enabledMask ^ m_communicationEnabledMask) |
          (waitingMask ^ m_communicationWaitingMask) |
          (receivedMask ^ m_communicationReceivedMask) |
          (repliedMask ^ m_communicationRepliedMask) |
          (failedMask ^ m_communicationFailedMask)) &
         DspTestProtocol::CommunicationStandbyCan) != 0U;
    const bool scibChanged =
        (((enabledMask ^ m_communicationEnabledMask) |
          (waitingMask ^ m_communicationWaitingMask) |
          (receivedMask ^ m_communicationReceivedMask) |
          (repliedMask ^ m_communicationRepliedMask) |
          (failedMask ^ m_communicationFailedMask)) &
         DspTestProtocol::CommunicationStandbyScib) != 0U;

    if (!stateChanged) {
        return;
    }

    m_communicationEnabledMask = enabledMask;
    m_communicationWaitingMask = waitingMask;
    m_communicationReceivedMask = receivedMask;
    m_communicationRepliedMask = repliedMask;
    m_communicationFailedMask = failedMask;

    const auto serviceText = [enabledMask, waitingMask, receivedMask,
                               repliedMask, failedMask](quint16 bit,
                                                         const QString &name) {
        if ((enabledMask & bit) == 0U) {
            return QString();
        }
        if ((failedMask & bit) != 0U) {
            return name + QStringLiteral("：失败");
        }
        if ((repliedMask & bit) != 0U) {
            return name + QStringLiteral("：已应答");
        }
        if ((receivedMask & bit) != 0U) {
            return name + QStringLiteral("：已接收");
        }
        if ((waitingMask & bit) != 0U) {
            return name + QStringLiteral("：等待中");
        }
        return name + QStringLiteral("：已启用");
    };

    QStringList serviceStates;
    const QString canState = serviceText(
        DspTestProtocol::CommunicationStandbyCan, QStringLiteral("CAN"));
    const QString scibState = serviceText(
        DspTestProtocol::CommunicationStandbyScib, activeSerialName());
    const QString ethernetState = serviceText(
        DspTestProtocol::CommunicationStandbyEthernet,
        QStringLiteral("Ethernet"));
    if (!canState.isEmpty()) {
        serviceStates.append(canState);
    }
    if (!scibState.isEmpty()) {
        serviceStates.append(scibState);
    }
    if (!ethernetState.isEmpty()) {
        serviceStates.append(ethernetState);
    }

    const QString summary = serviceStates.isEmpty() ?
        QStringLiteral("通信待命：未启用") :
        QStringLiteral("通信待命：%1").arg(serviceStates.join(QStringLiteral(" | ")));
    m_communicationStatusLabel->setText(summary);
    appendLog(LogChannel::Communication, summary);

    const bool canWaiting =
        ((enabledMask & DspTestProtocol::CommunicationStandbyCan) != 0U) &&
        ((waitingMask & DspTestProtocol::CommunicationStandbyCan) != 0U);
    const bool scibWaiting =
        ((enabledMask & DspTestProtocol::CommunicationStandbyScib) != 0U) &&
        ((waitingMask & DspTestProtocol::CommunicationStandbyScib) != 0U);
    if (m_canAutomaticTestRequested &&
        !m_canAutomaticTestStarted &&
        canWaiting) {
        m_canAutomaticTestStarted = true;
        startCanTest(true);
    }
    if (m_scibAutomaticTestRequested &&
        !m_scibAutomaticTestStarted &&
        scibWaiting) {
        m_scibAutomaticTestStarted = true;
        startScibTest(true);
    }

    if ((m_refreshScope == RefreshScope::None) &&
        (m_nextRecordIndex < 0) &&
        (canChanged || scibChanged) &&
        ((enabledMask & canScibMask) != 0U)) {
        if (canChanged) {
            m_client.requestRecord(TestCanExternal);
        }
        if (scibChanged) {
            m_scibRecordRefreshPending = false;
            m_client.requestRecord(activeSerialTestId());
        }
    } else if (scibChanged) {
        m_scibRecordRefreshPending = true;
    }
}

void MainWindow::appendLog(LogChannel channel, const QString &message)
{
    QString category;

    switch (channel) {
    case LogChannel::Communication:
        category = QStringLiteral("通信");
        break;
    case LogChannel::Board:
        category = QStringLiteral("板载");
        break;
    case LogChannel::External:
        category = QStringLiteral("外设");
        break;
    case LogChannel::Hpd:
        category = QStringLiteral("HPD");
        break;
    }

    m_log->appendPlainText(QStringLiteral("[%1][%2] %3")
                               .arg(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss")),
                                    category,
                                    message));
}

void MainWindow::exportLog()
{
    const QString logText = m_log->toPlainText();

    if(logText.isEmpty())
    {
        QMessageBox::information(this,
                                 QStringLiteral("导出日志"),
                                 QStringLiteral("当前没有可导出的日志。"));
        return;
    }

    const QString documentsPath =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString defaultFilePath = QDir(documentsPath).filePath(
        QStringLiteral("HPD_TestLog_%1.txt").arg(
            QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss"))));
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("导出运行日志"),
        defaultFilePath,
        QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)"));

    if(filePath.isEmpty())
    {
        return;
    }

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,
                             QStringLiteral("导出日志"),
                             QStringLiteral("无法写入日志文件：%1").arg(file.errorString()));
        return;
    }

    if(file.write(logText.toUtf8()) < 0)
    {
        QMessageBox::warning(this,
                             QStringLiteral("导出日志"),
                             QStringLiteral("日志写入失败：%1").arg(file.errorString()));
        return;
    }

    file.close();
    appendLog(LogChannel::Communication,
              QStringLiteral("日志已导出：%1").arg(filePath));
}

void MainWindow::appendScopeLog(RefreshScope scope, const QString &message)
{
    switch (scope) {
    case RefreshScope::Board:
        appendLog(LogChannel::Board, message);
        break;
    case RefreshScope::External:
        appendLog(LogChannel::External, message);
        break;
    case RefreshScope::Hpd:
        appendLog(LogChannel::Hpd, message);
        break;
    case RefreshScope::None:
        break;
    }
}

bool MainWindow::selectedTestIsOutput() const
{
    const quint16 testId = static_cast<quint16>(m_singleTestBox->currentData().toUInt());
    return (testId == TestDido) ||
           (testId == TestHdo) ||
           (testId == TestPwmDido) ||
           (testId == TestDriverResetDo);
}

void MainWindow::updateSingleTestParameterControls()
{
    if((m_adcInjectionPanel == nullptr) ||
       (m_adcInjectionSelectionLabel == nullptr) ||
       (m_pwmDidoPanel == nullptr) ||
       (m_driverFaultPanel == nullptr) ||
       (m_protectionPanel == nullptr) ||
       (m_driverResetPanel == nullptr))
    {
        return;
    }

    const quint16 testId = static_cast<quint16>(
        m_singleTestBox->currentData().toUInt());
    const auto stage = static_cast<DspTestProtocol::Stage>(
        m_singleStageBox->currentData().toUInt());
    const bool isSingle = testId == TestAdcExternalInjection;
    const bool isGroup1 = testId == TestAdcExternalGroup1;
    const bool isGroup2 = testId == TestAdcExternalGroup2;
    const bool isPt100Mux = testId == TestAdcPt100Mux;
    const bool usesSelectionBox = isSingle || isPt100Mux;

    if(m_adcInjectionChannelBox->property("selectionTestId").toUInt() != testId) {
        m_adcInjectionChannelBox->clear();
        if(isPt100Mux) {
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("PT4 / S1:S0=11 / 120Ω / AD_T_DJ(ADCINA3)"),
                AdcExternalSelectionPt100Base + 3);
        } else {
            m_adcInjectionVoltageBox->setSingleStep(0.1);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电流] J_IU1 / X3-20 / ADCD0"), 0);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电流] J_IV1 / X3-46 / ADCC3"), 1);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电流] J_IW1 / X3-22 / ADCB3"), 2);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电流] J_IU2 / X4-20 / ADCA15"), 3);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电流] J_IV2 / X4-46 / ADCB2"), 4);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电流] J_IW2 / X4-22 / ADCC2"), 5);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[母线] J_UDC1 / X3-44 / ADCA14"), 6);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[母线] J_UDC2 / X4-44 / ADCD1"), 7);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[驱动器温度] AD_T_INV1 / ADCA4"), 13);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[驱动器温度] AD_T_INV2 / ADCA5"), 14);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电压反馈] J_Uuv1 / X3-48 / ADCD4"), 15);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电压反馈] J_Uvw1 / X3-24 / ADCC4"), 16);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电压反馈] J_Uuv2 / X4-48 / ADCB0"), 17);
            m_adcInjectionChannelBox->addItem(
                QStringLiteral("[电压反馈] J_Uvw2 / X4-24 / ADCA0"), 18);
        }
        m_adcInjectionChannelBox->setProperty("selectionTestId", testId);
    }

    m_adcInjectionPanel->setVisible(
        (stage == DspTestProtocol::Stage::ExternalConnected) &&
        (isSingle || isGroup1 || isGroup2 || isPt100Mux));
    m_pwmDidoPanel->setVisible(
        (stage == DspTestProtocol::Stage::ExternalConnected) &&
        (testId == TestPwmDido));
    m_driverFaultPanel->setVisible(
        (stage == DspTestProtocol::Stage::ExternalConnected) &&
        (testId == TestDriverFaultDi));
    m_protectionPanel->setVisible(
        (stage == DspTestProtocol::Stage::ExternalConnected) &&
        (testId == TestProtectionFaultDi));
    m_driverResetPanel->setVisible(
        (stage == DspTestProtocol::Stage::ExternalConnected) &&
        (testId == TestDriverResetDo));
    m_adcInjectionChannelBox->setVisible(usesSelectionBox);
    const bool wasPt4ResistanceTest =
        m_adcInjectionVoltageBox->property("pt4ResistanceTest").toBool();
    if(isPt100Mux) {
        if(!wasPt4ResistanceTest) {
            m_adcInjectionVoltageBox->setProperty(
                "previousValue", m_adcInjectionVoltageBox->value());
            m_adcInjectionToleranceBox->setProperty(
                "previousValue", m_adcInjectionToleranceBox->value());
        }
        m_adcInjectionVoltageBox->setValue(
            static_cast<double>(Pt4TestExpectedMillivolts) / 1000.0);
        m_adcInjectionToleranceBox->setValue(
            static_cast<double>(Pt4TestToleranceMillivolts) / 1000.0);
        m_adcInjectionVoltageBox->setProperty("pt4ResistanceTest", true);
        m_adcInjectionVoltageBox->setEnabled(false);
        m_adcInjectionToleranceBox->setEnabled(false);
    } else {
        if(wasPt4ResistanceTest) {
            m_adcInjectionVoltageBox->setValue(
                m_adcInjectionVoltageBox->property("previousValue").toDouble());
            m_adcInjectionToleranceBox->setValue(
                m_adcInjectionToleranceBox->property("previousValue").toDouble());
        }
        m_adcInjectionVoltageBox->setProperty("pt4ResistanceTest", false);
        m_adcInjectionVoltageBox->setEnabled(true);
        m_adcInjectionToleranceBox->setEnabled(true);
    }
    if(isGroup1) {
        m_adcInjectionSelectionLabel->setText(
            QStringLiteral("步骤1：X3／1组七路同时注入"));
    } else if(isGroup2) {
        m_adcInjectionSelectionLabel->setText(
            QStringLiteral("步骤2：X4／2组七路同时注入"));
    } else if(isPt100Mux) {
        m_adcInjectionSelectionLabel->setText(QStringLiteral("PT4的120Ω温度测试"));
    } else {
        m_adcInjectionSelectionLabel->setText(QStringLiteral("ADC通道"));
    }
}

void MainWindow::updateDriverFaultChannelStatus(quint16 monitoredMask,
                                                 quint16 verifiedMask,
                                                 bool resultAvailable)
{
    const QStringList names{
        QStringLiteral("D_FLT_1U"), QStringLiteral("D_FLT_1D"),
        QStringLiteral("D_FLT_2U"), QStringLiteral("D_FLT_2D")};
    QStringList results;
    for (int index = 0; index < names.size(); ++index) {
        const quint16 bit = static_cast<quint16>(1U << index);
        QString state = QStringLiteral("未选择");
        if((monitoredMask & bit) != 0U) {
            state = !resultAvailable ? QStringLiteral("待测试") :
                ((verifiedMask & bit) != 0U ? QStringLiteral("PASS") :
                                              QStringLiteral("未通过"));
        }
        results.append(QStringLiteral("%1: %2").arg(names.at(index), state));
    }
    m_driverFaultStatusLabel->setText(
        QStringLiteral("分组结果：") + results.join(QStringLiteral("； ")));
}
