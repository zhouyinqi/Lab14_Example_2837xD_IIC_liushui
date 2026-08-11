#include "DspTestProtocol.h"

#include <cstring>

namespace {

void appendU16(QByteArray *data, quint16 value)
{
    data->append(static_cast<char>((value >> 8U) & 0xFFU));
    data->append(static_cast<char>(value & 0xFFU));
}

quint16 readU16(const QByteArray &data, int offset)
{
    return static_cast<quint16>(
        (static_cast<quint16>(static_cast<quint8>(data.at(offset))) << 8U) |
        static_cast<quint8>(data.at(offset + 1)));
}

quint32 readU32(const QByteArray &data, int offset)
{
    return (static_cast<quint32>(static_cast<quint8>(data.at(offset))) << 24U) |
           (static_cast<quint32>(static_cast<quint8>(data.at(offset + 1))) << 16U) |
           (static_cast<quint32>(static_cast<quint8>(data.at(offset + 2))) << 8U) |
           static_cast<quint8>(data.at(offset + 3));
}

float readFloat(const QByteArray &data, int offset)
{
    const quint32 rawValue = readU32(data, offset);
    float value = 0.0F;

    static_assert(sizeof(value) == sizeof(rawValue));
    std::memcpy(&value, &rawValue, sizeof(value));
    return value;
}

} // namespace

namespace DspTestProtocol {

quint16 crc16(const QByteArray &data, int length)
{
    quint16 crc = 0xFFFFU;

    for (int index = 0; index < length; ++index) {
        crc ^= static_cast<quint16>(static_cast<quint8>(data.at(index))) << 8U;
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x8000U) != 0U
                      ? static_cast<quint16>((crc << 1U) ^ 0x1021U)
                      : static_cast<quint16>(crc << 1U);
        }
    }

    return crc;
}

QByteArray makeRequest(Command command,
                       quint16 sequence,
                       quint8 stage,
                       quint8 flags,
                       quint16 argument)
{
    QByteArray request;
    request.reserve(RequestSize);
    request.append('B');
    request.append('T');
    request.append(static_cast<char>(Version));
    request.append(static_cast<char>(command));
    appendU16(&request, sequence);
    request.append(static_cast<char>(stage));
    request.append(static_cast<char>(flags));
    appendU16(&request, argument);
    appendU16(&request, crc16(request, 10));
    return request;
}

bool parseResponse(const QByteArray &frame, Response *response, QString *error)
{
    if (frame.size() != ResponseSize) {
        *error = QStringLiteral("响应长度错误: %1").arg(frame.size());
        return false;
    }

    if ((frame.at(0) != 'B') || (frame.at(1) != 'R')) {
        *error = QStringLiteral("响应帧头错误");
        return false;
    }

    if (static_cast<quint8>(frame.at(2)) != Version) {
        *error = QStringLiteral("协议版本不匹配");
        return false;
    }

    if (readU16(frame, 38) != crc16(frame, 38)) {
        *error = QStringLiteral("响应CRC错误");
        return false;
    }

    response->command = static_cast<Command>(static_cast<quint8>(frame.at(3)));
    response->sequence = readU16(frame, 4);
    response->protocolStatus = static_cast<ProtocolStatus>(readU16(frame, 6));
    response->lastCommandResult = readU16(frame, 8);
    response->managerMode = readU16(frame, 10);
    response->activeStage = readU16(frame, 12);
    response->activeTestId = readU16(frame, 14);
    response->recordId = readU16(frame, 16);
    response->recordResult = static_cast<Result>(readU16(frame, 18));
    response->recordErrorCode = readU16(frame, 20);
    response->rawValue = readU32(frame, 22);
    response->measuredValue = readFloat(frame, 26);
    response->expectedMin = readFloat(frame, 30);
    response->expectedMax = readFloat(frame, 34);
    if (response->command == Command::GetStatus) {
        response->communicationStandbyEnabledMask =
            static_cast<quint16>(response->rawValue >> 16U);
        response->communicationStandbyWaitingMask =
            static_cast<quint16>(response->rawValue & 0xFFFFU);
        response->communicationStandbyReceivedMask =
            static_cast<quint16>(response->measuredValue);
        response->communicationStandbyRepliedMask =
            static_cast<quint16>(response->expectedMin);
        response->communicationStandbyFailedMask =
            static_cast<quint16>(response->expectedMax);
    }
    if (response->command == Command::GetBoardInfo) {
        response->boardProfileId = static_cast<quint16>(response->measuredValue);
        response->boardPinMapRevision = response->recordErrorCode;
        response->boardCapabilities = response->rawValue;
        response->boardEthernetInterface =
            static_cast<EthernetInterface>(static_cast<quint16>(response->expectedMin));
        response->boardProfileState = static_cast<BoardProfileState>(
            static_cast<quint16>(response->expectedMax));
    }
    return true;
}

QString resultText(Result result)
{
    switch (result) {
    case Result::NotRun: return QStringLiteral("NOT_RUN");
    case Result::Running: return QStringLiteral("RUNNING");
    case Result::Pass: return QStringLiteral("PASS");
    case Result::Fail: return QStringLiteral("FAIL");
    case Result::Warn: return QStringLiteral("WARN");
    case Result::Skip: return QStringLiteral("SKIP");
    case Result::Timeout: return QStringLiteral("TIMEOUT");
    case Result::NotSupported: return QStringLiteral("NOT_SUPPORTED");
    case Result::SafetyLocked: return QStringLiteral("SAFETY_LOCKED");
    }

    return QStringLiteral("UNKNOWN");
}

QString protocolStatusText(ProtocolStatus status)
{
    switch (status) {
    case ProtocolStatus::Ok: return QStringLiteral("OK");
    case ProtocolStatus::FrameError: return QStringLiteral("帧错误");
    case ProtocolStatus::VersionError: return QStringLiteral("版本错误");
    case ProtocolStatus::CommandError: return QStringLiteral("命令错误");
    case ProtocolStatus::Busy: return QStringLiteral("下位机忙");
    case ProtocolStatus::ArgumentError: return QStringLiteral("参数错误");
    case ProtocolStatus::TestIdError: return QStringLiteral("Test ID错误");
    case ProtocolStatus::ProfileError: return QStringLiteral("板型尚未选择或确认");
    }

    return QStringLiteral("未知协议状态");
}

} // namespace DspTestProtocol
