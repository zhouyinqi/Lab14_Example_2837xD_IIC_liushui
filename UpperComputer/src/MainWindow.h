#pragma once

#include "DspTcpClient.h"
#include "EthernetTcpTestClient.h"
#include "ScibSerialClient.h"
#include "ZlgCanClient.h"

#include <QHash>
#include <QMainWindow>
#include <QVector>

class QComboBox;
class QLabel;
class QLineEdit;
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
    void updateSingleTestChoices(RefreshScope scope);
    void requestNextRecord();
    void beginRecordRefresh(RefreshScope scope);
    void updateRecord(const DspTestProtocol::Response &response);
    void updateStatus(const DspTestProtocol::Response &response);
    void updateBoardInfo(const DspTestProtocol::Response &response);
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
    void setEdoManualResult(bool passed);
    void updateResultAppearance(QTableWidgetItem *item,
                                DspTestProtocol::Result result,
                                bool manualConfirmation) const;
    void exportLog();
    void appendLog(LogChannel channel, const QString &message);
    void appendScopeLog(RefreshScope scope, const QString &message);
    bool selectedTestIsOutput() const;

    DspTcpClient m_client;
    EthernetTcpTestClient m_ethernetTcpTestClient;
    ScibSerialClient m_scibSerialClient;
    ZlgCanClient m_zlgCanClient;
    QVector<quint16> m_boardRecordIds;
    QVector<quint16> m_externalRecordIds;
    QVector<quint16> m_hpdRecordIds;
    QVector<quint16> m_activeRecordIds;
    QHash<quint16, RecordLocation> m_recordRows;
    int m_nextRecordIndex = -1;
    RefreshScope m_refreshScope = RefreshScope::None;
    bool m_waitingForTestReconnect = false;
    bool m_edoManualConfirmationExpected = false;
    bool m_edoManualConfirmationAvailable = false;
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
    bool m_waitingForStopCompletion = false;
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
    QTableWidget *m_boardRecordsTable = nullptr;
    QTableWidget *m_externalRecordsTable = nullptr;
    QTableWidget *m_hpdRecordsTable = nullptr;
    QTabWidget *m_resultTabs = nullptr;
    QPlainTextEdit *m_log = nullptr;
    QPushButton *m_exportLogButton = nullptr;
    QTimer *m_pollTimer = nullptr;
};
