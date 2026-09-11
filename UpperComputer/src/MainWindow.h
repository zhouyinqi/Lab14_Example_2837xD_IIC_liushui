#pragma once

#include "DspTcpClient.h"
#include "EthernetTcpTestClient.h"
#include "ScibSerialClient.h"
#include "ZlgCanClient.h"

#include <QHash>
#include <QMainWindow>
#include <QPointer>
#include <QVector>

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QMessageBox;
class QPlainTextEdit;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QTabWidget;
class QTimer;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    friend class MainWindowTests;

    enum class RefreshScope : quint8 {
        None,
        Board,
        External,
        Hpd
    };

    enum class LogChannel : quint8 {
        Communication,
        Board,
        External,
        Hpd
    };

    struct RecordLocation {
        QTableWidget *table = nullptr;
        int row = -1;
    };

    void addTest(quint16 id, const QString &name, RefreshScope scope);
    void prepareResultRefresh(RefreshScope scope);
    void prepareSingleTestRefresh(RefreshScope scope, quint16 testId);
    void updateSingleTestChoices(RefreshScope scope);
    void requestNextRecord();
    void beginRecordRefresh(RefreshScope scope);
    void updateRecord(const DspTestProtocol::Response &response);
    void updateStatus(const DspTestProtocol::Response &response);
    void updateBoardInfo(const DspTestProtocol::Response &response);
    void resetDisplayedResultsForBoardChange();
    void beginTestAvailabilityRefresh();
    void requestNextTestAvailability();
    void updateTestAvailability(
        const DspTestProtocol::Response &response);
    void resetTestAvailability();
    bool isTestRunnable(quint16 testId) const;
    bool isTestEnabledInAuto(quint16 testId) const;
    void updateBoardProfileChoices(
        const DspTestProtocol::Response &response);
    void updateTestCommandAvailability();
    void updateCommunicationStandbyStatus(
        const DspTestProtocol::Response &response);
    void refreshScibPorts();
    bool startScibTest(bool automatic);
    void resetEthernetTcpSequence();
    void startEthernetTcpSequence(bool externalAuto,
                                  bool includeBasicAndSocket);
    void startNextEthernetTcpTest();
    void startEthernetTcpRawTest();
    void reconnectAfterEthernetTcpTest();
    void finishEthernetTcpSequence();
    bool isEthernetTcpTest(quint16 testId) const;
    EthernetTcpTestClient::Mode ethernetTcpMode(quint16 testId) const;
    void selectCanLibrary();
    bool startCanTest(bool automatic);
    void requestPendingScibRecordRefresh();
    quint16 activeSerialTestId() const;
    bool isSerialExternalTest(quint16 testId) const;
    QString activeSerialName() const;
    void requestEdoManualConfirmation();
    void showManualOutputConfirmation(quint16 testId,
                                      const QString &title,
                                      const QString &prompt,
                                      const QString &testName);
    void cancelManualOutputConfirmation();
    void setManualOutputResult(quint16 testId,
                               bool passed,
                               const QString &testName);
    void updateResultAppearance(QTableWidgetItem *item,
                                DspTestProtocol::Result result,
                                bool manualConfirmation) const;
    void exportLog();
    void appendLog(LogChannel channel, const QString &message);
    void appendScopeLog(RefreshScope scope, const QString &message);
    bool selectedTestIsOutput() const;
    void updateSingleTestParameterControls();
    void updateDriverFaultChannelStatus(quint16 monitoredMask,
                                        quint16 verifiedMask,
                                        bool resultAvailable);

    DspTcpClient m_client;
    EthernetTcpTestClient m_ethernetTcpTestClient;
    ScibSerialClient m_scibSerialClient;
    ZlgCanClient m_zlgCanClient;
    QVector<quint16> m_boardRecordIds;
    QVector<quint16> m_externalRecordIds;
    QVector<quint16> m_hpdRecordIds;
    QVector<quint16> m_activeRecordIds;
    QHash<quint16, RecordLocation> m_recordRows;
    QHash<quint16, DspTestProtocol::TestAvailability> m_testAvailability;
    QHash<quint16, bool> m_testEnabledInAuto;
    QVector<quint16> m_testAvailabilityQueryIds;
    int m_nextTestAvailabilityIndex = -1;
    bool m_testAvailabilityReady = false;
    quint16 m_availabilityProfileId = 0;
    quint16 m_availabilityHardwareRevision = 0;
    quint16 m_availabilityPinMapRevision = 0;
    quint32 m_availabilityCapabilities = 0;
    int m_nextRecordIndex = -1;
    RefreshScope m_refreshScope = RefreshScope::None;
    bool m_waitingForTestReconnect = false;
    bool m_edoManualConfirmationExpected = false;
    bool m_edoManualConfirmationAvailable = false;
    QPointer<QMessageBox> m_manualOutputDialog;
    bool m_scibAutomaticTestRequested = false;
    bool m_scibAutomaticTestStarted = false;
    bool m_scibSingleTestRequested = false;
    bool m_scibSingleTestStarted = false;
    bool m_canAutomaticTestRequested = false;
    bool m_canAutomaticTestStarted = false;
    bool m_canSingleTestRequested = false;
    bool m_canSingleTestStarted = false;
    bool m_scibRecordRefreshPending = false;
    bool m_singleTestRefreshPending = false;
    bool m_singleTestStartStatusRequested = false;
    bool m_singleTestObservedRunning = false;
    bool m_singleTestRecordRefreshInFlight = false;
    bool m_liveRecordRequestPending = false;
    bool m_waitingForStopCompletion = false;
    bool m_adcInjectionConfigurationPending = false;
    quint16 m_adcInjectionPendingTestId = DspTestProtocol::InvalidTestId;
    QString m_pwmDidoPendingChannelText;
    int m_pwmDidoLastLiveLevel = -1;
    quint16 m_driverFaultPendingMask = 0U;
    quint8 m_protectionPendingSelection = 0U;
    QString m_protectionPendingText;
    quint16 m_protectionLivePhase = 0U;
    bool m_protectionRecordFormatMismatchReported = false;
    QString m_lastProtectionFinalLogKey;
    QString m_driverResetPendingChannelText;
    int m_driverResetLastLiveLevel = -1;
    QVector<quint16> m_ethernetTcpPendingTests;
    quint16 m_ethernetTcpCurrentTestId = DspTestProtocol::InvalidTestId;
    bool m_ethernetTcpSequenceActive = false;
    bool m_ethernetTcpExternalAuto = false;
    bool m_ethernetTcpAwaitingStart = false;
    bool m_ethernetTcpAwaitingReconnect = false;
    bool m_ethernetTcpRecordRefreshPending = false;
    RefreshScope m_singleTestRefreshScope = RefreshScope::None;
    quint16 m_singleTestRefreshId = DspTestProtocol::InvalidTestId;
    quint16 m_communicationEnabledMask = 0;
    quint16 m_communicationWaitingMask = 0;
    quint16 m_communicationReceivedMask = 0;
    quint16 m_communicationRepliedMask = 0;
    quint16 m_communicationFailedMask = 0;
    DspTestProtocol::EthernetInterface m_detectedEthernetInterface =
        DspTestProtocol::EthernetInterface::None;
    DspTestProtocol::BoardProfileState m_boardProfileState =
        DspTestProtocol::BoardProfileState::BootSafe;
    quint16 m_boardProfileId = 0;
    bool m_resultBoardContextValid = false;
    quint16 m_resultBoardProfileId = 0;
    quint16 m_resultBoardHardwareRevision = 0;
    quint16 m_resultBoardPinMapRevision = 0;
    quint32 m_resultBoardCapabilities = 0;

    QLineEdit *m_hostEdit = nullptr;
    QLineEdit *m_portEdit = nullptr;
    QPushButton *m_connectButton = nullptr;
    QComboBox *m_scibPortBox = nullptr;
    QPushButton *m_refreshScibPortsButton = nullptr;
    QLineEdit *m_canLibraryEdit = nullptr;
    QPushButton *m_selectCanLibraryButton = nullptr;
    QComboBox *m_canDeviceTypeBox = nullptr;
    QComboBox *m_canDeviceIndexBox = nullptr;
    QComboBox *m_canChannelBox = nullptr;
    QPushButton *m_boardButton = nullptr;
    QPushButton *m_externalButton = nullptr;
    QPushButton *m_hpdButton = nullptr;
    QPushButton *m_stopButton = nullptr;
    QComboBox *m_boardProfileBox = nullptr;
    QPushButton *m_confirmBoardProfileButton = nullptr;
    QPushButton *m_clearBoardProfileButton = nullptr;
    QComboBox *m_hpdSourceBox = nullptr;
    QLabel *m_communicationStatusLabel = nullptr;
    QComboBox *m_singleTestBox = nullptr;
    QComboBox *m_singleStageBox = nullptr;
    QPushButton *m_singleButton = nullptr;
    QWidget *m_adcInjectionPanel = nullptr;
    QLabel *m_adcInjectionSelectionLabel = nullptr;
    QComboBox *m_adcInjectionChannelBox = nullptr;
    QDoubleSpinBox *m_adcInjectionVoltageBox = nullptr;
    QDoubleSpinBox *m_adcInjectionToleranceBox = nullptr;
    QWidget *m_pwmDidoPanel = nullptr;
    QComboBox *m_pwmDidoChannelBox = nullptr;
    QLabel *m_pwmDidoLiveValueLabel = nullptr;
    QWidget *m_driverFaultPanel = nullptr;
    QComboBox *m_driverFaultGroupBox = nullptr;
    QLabel *m_driverFaultStatusLabel = nullptr;
    QWidget *m_protectionPanel = nullptr;
    QComboBox *m_protectionSelectionBox = nullptr;
    QLabel *m_protectionStatusLabel = nullptr;
    QWidget *m_driverResetPanel = nullptr;
    QComboBox *m_driverResetChannelBox = nullptr;
    QLabel *m_driverResetLiveValueLabel = nullptr;
    QWidget *m_lowVoltageInputPanel = nullptr;
    QComboBox *m_lowVoltageInputBox = nullptr;
    QLabel *m_lowVoltageInputStatus = nullptr;
    QTableWidget *m_boardRecordsTable = nullptr;
    QTableWidget *m_externalRecordsTable = nullptr;
    QTableWidget *m_hpdRecordsTable = nullptr;
    QTabWidget *m_resultTabs = nullptr;
    QPlainTextEdit *m_log = nullptr;
    QPushButton *m_exportLogButton = nullptr;
    QTimer *m_pollTimer = nullptr;
    QTimer *m_pwmDidoLiveTimer = nullptr;
    QTimer *m_protectionResetLiveTimer = nullptr;
};
