#include "MainWindow.h"

#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QDateTime>
#include <QDir>
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

namespace {

constexpr quint16 TestDido = 0x0313;
constexpr quint16 TestHdo = 0x0314;
constexpr quint16 TestCanExternal = 0x0307;
constexpr quint16 TestSciRs485External = 0x0309;
constexpr quint16 TestRs422External = 0x0316;
constexpr quint16 TestEthernetBasic = 0x0305;
constexpr quint16 TestEthernetSocket = 0x030D;
constexpr quint16 TestEthernetTcpLink = 0x030E;
constexpr quint16 TestEthernetTcpEcho = 0x030F;
constexpr quint16 TestEthernetTcpStability = 0x0310;

QString hex16(quint16 value)
{
    return QStringLiteral("0x%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
}

QString hex32(quint32 value)
{
    return QStringLiteral("0x%1").arg(value, 8, 16, QLatin1Char('0')).toUpper();
}

QString boardProfileName(quint16 profileId)
{
    switch (profileId) {
    case 0x0001U:
        return QStringLiteral("系统主控板");
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
    m_canDeviceTypeBox->addItem(QStringLiteral("USBCAN1 (当前设备)"), 3U);
    m_canDeviceTypeBox->addItem(QStringLiteral("USBCAN2"), 4U);
    m_canDeviceIndexBox = new QComboBox(connectionGroup);
    for (quint32 deviceIndex = 0U; deviceIndex < 4U; ++deviceIndex) {
        m_canDeviceIndexBox->addItem(
            QStringLiteral("设备%1").arg(deviceIndex), deviceIndex);
    }
    m_canChannelBox = new QComboBox(connectionGroup);
    m_canChannelBox->addItem(QStringLiteral("CAN1 (500 kbit/s)"), 0U);
    m_canChannelBox->addItem(QStringLiteral("CAN2 (500 kbit/s)"), 1U);
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
    connectionLayout->addWidget(new QLabel(QStringLiteral("设备类型"), connectionGroup), 3, 0);
    connectionLayout->addWidget(m_canDeviceTypeBox, 3, 1);
    connectionLayout->addWidget(new QLabel(QStringLiteral("设备索引"), connectionGroup), 3, 2);
    connectionLayout->addWidget(m_canDeviceIndexBox, 3, 3);
    connectionLayout->addWidget(new QLabel(QStringLiteral("测试通道"), connectionGroup), 3, 4);
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
    m_singleTestBox = new QComboBox(singleGroup);
    m_singleStageBox = new QComboBox(singleGroup);
    m_singleStageBox->addItem(QStringLiteral("板载/内部"), 0);
    m_singleStageBox->addItem(QStringLiteral("外设已连接"), 1);
    m_singleStageBox->addItem(QStringLiteral("HPD注入"), 2);
    m_singleButton = new QPushButton(QStringLiteral("开始单项测试"), singleGroup);
    m_singleStageBox->setFixedWidth(110);
    m_singleTestBox->setFixedWidth(240);
    m_singleButton->setFixedWidth(108);
    applyActionStyle(m_singleButton, QStringLiteral("#1565C0"), QStringLiteral("#0D47A1"));
    singleLayout->addWidget(new QLabel(QStringLiteral("测试阶段"), singleGroup), 0, 0);
    singleLayout->addWidget(m_singleStageBox, 0, 1);
    singleLayout->addWidget(new QLabel(QStringLiteral("Test ID"), singleGroup), 0, 2);
    singleLayout->addWidget(m_singleTestBox, 0, 3);
    singleLayout->setColumnStretch(4, 1);
    singleLayout->addWidget(m_singleButton, 0, 5);

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

    addTest(0x0100, QStringLiteral("SYS_STARTUP"), RefreshScope::Board);
    addTest(0x0101, QStringLiteral("SYS_CLOCK"), RefreshScope::Board);
    addTest(0x0102, QStringLiteral("SYS_INTERRUPT"), RefreshScope::Board);
    addTest(0x0103, QStringLiteral("SYS_TIMER"), RefreshScope::Board);
    addTest(0x0104, QStringLiteral("SYS_WATCHDOG"), RefreshScope::Board);
    addTest(0x0105, QStringLiteral("RAM"), RefreshScope::Board);
    addTest(0x0106, QStringLiteral("FLASH"), RefreshScope::Board);
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
    addTest(TestDido, QStringLiteral("EDO_EXTERNAL"), RefreshScope::External);
    addTest(TestHdo, QStringLiteral("HDO_EXTERNAL"), RefreshScope::External);
    addTest(0x0315, QStringLiteral("DI_EXTERNAL"), RefreshScope::External);
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
        if (QMessageBox::question(
                this,
                QStringLiteral("确认外设自检"),
                QStringLiteral("外设自检将依次驱动 EDO1-3。请确认 PLC 接线和设备状态安全。")) !=
            QMessageBox::Yes) {
            return;
        }
        if (QMessageBox::information(
                this,
                QStringLiteral("EDO目视确认准备"),
                QStringLiteral("测试开始后，请观察 PLC 的 DI3、DI4、DI5 指示灯。"
                               "EDO1、EDO2、EDO3 将依次点亮约2秒后熄灭。"
                               "确认已准备观察后，点击“确定”开始外设自检。"),
                QMessageBox::Ok | QMessageBox::Cancel,
                QMessageBox::Ok) != QMessageBox::Ok) {
            return;
        }
        m_resultTabs->setCurrentWidget(m_externalRecordsTable);
        resetEthernetTcpSequence();
        m_ethernetTcpExternalAuto = true;
        m_edoManualConfirmationExpected = true;
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
    });
    connect(m_singleButton, &QPushButton::clicked, this, [this] {
        const quint16 testId = static_cast<quint16>(m_singleTestBox->currentData().toUInt());
        const auto stage = static_cast<DspTestProtocol::Stage>(
            m_singleStageBox->currentData().toUInt());
        const bool isOutput = selectedTestIsOutput();
        if (isOutput && QMessageBox::question(
                            this,
                            QStringLiteral("确认DO输出"),
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
        switch (stage) {
        case DspTestProtocol::Stage::BoardOnly:
            prepareResultRefresh(RefreshScope::Board);
            break;
        case DspTestProtocol::Stage::ExternalConnected:
            prepareResultRefresh(RefreshScope::External);
            break;
        case DspTestProtocol::Stage::HpdInjection:
            prepareResultRefresh(RefreshScope::Hpd);
            break;
        }
        if((stage == DspTestProtocol::Stage::ExternalConnected) &&
           (testId == TestEthernetBasic)) {
            resetEthernetTcpSequence();
            startEthernetTcpSequence(false, true);
            return;
        }
        m_singleTestRefreshScope = m_refreshScope;
        m_singleTestRefreshId = testId;
        m_client.startSingle(testId,
                             stage,
                             isOutput);
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
                if (response.protocolStatus != DspTestProtocol::ProtocolStatus::Ok) {
                    appendLog(LogChannel::Communication,
                              QStringLiteral("DSP拒绝命令：%1")
                                  .arg(DspTestProtocol::protocolStatusText(response.protocolStatus)));
                    if ((response.command == DspTestProtocol::Command::StartBoard) ||
                        (response.command == DspTestProtocol::Command::StartExternal) ||
                        (response.command == DspTestProtocol::Command::StartHpd) ||
                        (response.command == DspTestProtocol::Command::StartSingle)) {
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
                }
                if (response.command == DspTestProtocol::Command::GetBoardInfo &&
                    response.protocolStatus == DspTestProtocol::ProtocolStatus::Ok) {
                    updateBoardInfo(response);
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
                updateStatus(response);
                if (response.recordId != DspTestProtocol::InvalidTestId) {
                    updateRecord(response);
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
                    m_client.requestRecord(singleTestId);
                } else if ((response.command == DspTestProtocol::Command::GetStatus) &&
                           (response.managerMode == 1U) &&
                           (m_refreshScope != RefreshScope::None) &&
                           m_waitingForTestReconnect &&
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
        m_singleTestBox->addItem(QStringLiteral("%1  %2").arg(hex16(id), name), id);
    }

    const int previousIndex = m_singleTestBox->findData(previousId);
    if (previousIndex >= 0) {
        m_singleTestBox->setCurrentIndex(previousIndex);
    }
    m_singleTestBox->blockSignals(false);
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
    m_client.connectToDevice(m_hostEdit->text().trimmed(),
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
    const QString resultText = isManualEdoConfirmation ?
        QStringLiteral("MANUAL_CONFIRM") :
        DspTestProtocol::resultText(response.recordResult);

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
                           isManualEdoConfirmation);
    appendScopeLog(logScope,
                   QStringLiteral("%1: %2，错误码=%3")
                       .arg(hex16(response.recordId),
                            resultText,
                            hex16(response.recordErrorCode)));

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
        appendScopeLog(RefreshScope::External,
                       QStringLiteral("EDO人工确认：请观察PLC的DI3、DI4、DI5指示灯，"
                                      "应依次亮约2秒后熄灭。三路均符合时，判定EDO外部输出通过。"));
    }

    if (isSingleTestRecord) {
        appendScopeLog(logScope, QStringLiteral("单项测试记录刷新完成。"));
        m_singleTestRecordRefreshInFlight = false;
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

    const auto decision = QMessageBox::question(
        this,
        QStringLiteral("EDO目视确认"),
        QStringLiteral("请确认PLC的DI3、DI4、DI5指示灯是否依次亮约2秒后熄灭。\n"
                       "确认通过将把本次上位机EDO结果标记为PASS。"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    setEdoManualResult(decision == QMessageBox::Yes);
    m_edoManualConfirmationAvailable = false;
}

void MainWindow::setEdoManualResult(bool passed)
{
    const auto rowIterator = m_recordRows.constFind(TestDido);
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
              passed ? QStringLiteral("EDO人工目视确认通过：上位机结果已标记为PASS。") :
                       QStringLiteral("EDO人工目视确认未通过：上位机结果已标记为FAIL。"));
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
    if(static_cast<DspTestProtocol::Stage>(
           m_singleStageBox->currentData().toUInt()) ==
       DspTestProtocol::Stage::ExternalConnected) {
        updateSingleTestChoices(RefreshScope::External);
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

void MainWindow::updateBoardProfileChoices(
    const DspTestProtocol::Response &response)
{
    const quint16 preferredId = response.boardProfileId;

    m_boardProfileBox->blockSignals(true);
    m_boardProfileBox->clear();
    m_boardProfileBox->addItem(QStringLiteral("请选择实际板型"), 0U);
    if(response.boardEthernetInterface ==
       DspTestProtocol::EthernetInterface::EmifW5300) {
        m_boardProfileBox->addItem(QStringLiteral("系统主控板"), 0x0001U);
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
    m_boardButton->setEnabled(confirmed);
    m_externalButton->setEnabled(confirmed);
    m_hpdButton->setEnabled(confirmed);
    m_hpdSourceBox->setEnabled(confirmed);
    m_singleStageBox->setEnabled(confirmed);
    m_singleTestBox->setEnabled(confirmed);
    m_singleButton->setEnabled(confirmed);
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
    return (testId == TestDido) || (testId == TestHdo);
}
