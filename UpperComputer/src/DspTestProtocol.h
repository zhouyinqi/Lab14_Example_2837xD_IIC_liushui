#pragma once

#include <QByteArray>
#include <QString>

namespace DspTestProtocol {

constexpr quint8 Version = 1;
constexpr int RequestSize = 12;
constexpr int ResponseSize = 40;
constexpr quint16 InvalidTestId = 0xFFFF;

enum class Command : quint8 {
    Ping = 0x01,
    StartBoard = 0x10,
    StartExternal = 0x11,
    StartHpd = 0x12,
    StartSingle = 0x13,
    Stop = 0x14,
    SelectBoardProfile = 0x15,
    ConfirmBoardProfile = 0x16,
    ClearBoardProfile = 0x17,
    GetStatus = 0x20,
    GetRecord = 0x21,
    GetBoardInfo = 0x22
};

enum class EthernetInterface : quint16 {
    None = 0,
    EmifW5300 = 1,
    SpicW5500 = 2
};

enum class BoardProfileState : quint16 {
    BootSafe = 0,
    NetworkReady = 1,
    SelectedUnconfirmed = 2,
    Confirmed = 3
};

enum class Stage : quint8 {
    BoardOnly = 0,
    ExternalConnected = 1,
    HpdInjection = 2
};

enum class Result : quint16 {
    NotRun = 0,
    Running = 1,
    Pass = 2,
    Fail = 3,
    Warn = 4,
    Skip = 5,
    Timeout = 6,
    NotSupported = 7,
    SafetyLocked = 8
};

enum class ProtocolStatus : quint16 {
    Ok = 0,
    FrameError = 1,
    VersionError = 2,
    CommandError = 3,
    Busy = 4,
    ArgumentError = 5,
    TestIdError = 6,
    ProfileError = 7
};

constexpr quint8 FlagOutputArmed = 0x01;
constexpr quint16 CommunicationStandbyCan = 0x0001;
constexpr quint16 CommunicationStandbyScib = 0x0002;
constexpr quint16 CommunicationStandbyEthernet = 0x0004;

struct Response {
    Command command = Command::Ping;
    quint16 sequence = 0;
    ProtocolStatus protocolStatus = ProtocolStatus::FrameError;
    quint16 lastCommandResult = 0;
    quint16 managerMode = 0;
    quint16 activeStage = 0;
    quint16 activeTestId = InvalidTestId;
    quint16 communicationStandbyEnabledMask = 0;
    quint16 communicationStandbyWaitingMask = 0;
    quint16 communicationStandbyReceivedMask = 0;
    quint16 communicationStandbyRepliedMask = 0;
    quint16 communicationStandbyFailedMask = 0;
    quint16 recordId = InvalidTestId;
    Result recordResult = Result::NotRun;
    quint16 recordErrorCode = 0;
    quint32 rawValue = 0;
    float measuredValue = 0.0F;
    float expectedMin = 0.0F;
    float expectedMax = 0.0F;
    quint16 boardProfileId = 0;
    quint16 boardHardwareRevision = 0;
    quint16 boardPinMapRevision = 0;
    quint32 boardCapabilities = 0;
    EthernetInterface boardEthernetInterface = EthernetInterface::None;
    BoardProfileState boardProfileState = BoardProfileState::BootSafe;
};

QByteArray makeRequest(Command command,
                       quint16 sequence,
                       quint8 stage = 0,
                       quint8 flags = 0,
                       quint16 argument = 0);
bool parseResponse(const QByteArray &frame, Response *response, QString *error);

QString resultText(Result result);
QString protocolStatusText(ProtocolStatus status);
quint16 crc16(const QByteArray &data, int length);

} // namespace DspTestProtocol
