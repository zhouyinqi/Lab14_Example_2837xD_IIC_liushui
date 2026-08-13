#include "F28x_Project.h"
#include "Board_Adc_Test.h"
#include "Board_Can_Test.h"
#include "Board_Dido_Test.h"
#include "Board_Di_Test.h"
#include "Board_Emif_Test.h"
#include "Board_Ethernet_Test.h"
#include "Board_W5500_Test.h"
#include "Board_Fpga_Test.h"
#include "Board_Gpio_Test.h"
#include "Board_Hdo_Test.h"
#include "Board_Host_Protocol.h"
#include "Board_I2c_Test.h"
#include "Board_Pwm_Test.h"
#include "Board_Profile.h"
#include "Board_Sci_Test.h"
#include "Board_Spi_Test.h"
#include "Board_System_Test.h"
#include "Board_Test_Target.h"
#include "Hpd_Test_Limits.h"

extern void Led_Test(void);
extern volatile Uint16 RtcStatus;

volatile BoardTest_CommandMailbox gBoardTestCommandMailbox =
{
    BOARD_TEST_COMMAND_NONE,
    BOARD_TEST_INVALID_ID,
    BOARD_TEST_STAGE_BOARD_ONLY,
    BOARD_TEST_HPD_INPUT_SOFTWARE_PHYSICAL,
    HPD_BUS_VOLTAGE_TARGET_V,
    HPD_LINE_VOLTAGE_TARGET_V,
    HPD_LINE_FREQUENCY_TARGET_HZ,
    HPD_CURRENT_TARGET_A,
    HPD_CURRENT_FREQUENCY_TARGET_HZ,
    25.0F,
    BOARD_TEST_COMM_STANDBY_ALL_MASK,
    BOARD_TEST_RESULT_NOT_RUN,
    0U,
    BOARD_DIDO_DEFAULT_CHANNEL_MASK,
    0U,
    BOARD_HDO_DEFAULT_CHANNEL_MASK,
    BOARD_DI_DEFAULT_CHANNEL_MASK
};

BoardTest_CommunicationStandbyStatus
    gBoardTestCommunicationStandbyStatus =
{
    0U,
    0U,
    0U,
    0U,
    0U,
    {BOARD_TEST_STANDBY_DISABLED, BOARD_TEST_ERROR_NONE, 0U, 0U},
    {BOARD_TEST_STANDBY_DISABLED, BOARD_TEST_ERROR_NONE, 0U, 0U},
    {BOARD_TEST_STANDBY_DISABLED, BOARD_TEST_ERROR_NONE, 0U, 0U}
};

static BoardTest_U16 BoardTest_TargetCommunicationStandbyMask = 0U;
static BoardTest_U16 BoardTest_TargetCommunicationStandbyPending = 0U;

#define BOARD_TEST_HOST_BACKEND_W5300  1U
#define BOARD_TEST_HOST_BACKEND_W5500  2U

/* W5500 VERSIONR is definitive; a failed SPI probe falls back to W5300. */
static BoardTest_U16 BoardTest_TargetHostBackend =
    BOARD_TEST_HOST_BACKEND_W5500;

static void BoardTest_TargetKeepHostProtocolForManualTest(void);

static BoardTest_U16 BoardHostProtocol_ReadU16(
    const BoardTest_U16 *bytes,
    BoardTest_U16 offset)
{
    return (BoardTest_U16)(((bytes[offset] & 0x00FFU) << 8U) |
                           (bytes[offset + 1U] & 0x00FFU));
}

static void BoardHostProtocol_WriteU16(
    BoardTest_U16 *bytes,
    BoardTest_U16 offset,
    BoardTest_U16 value)
{
    bytes[offset] = (value >> 8U) & 0x00FFU;
    bytes[offset + 1U] = value & 0x00FFU;
}

static void BoardHostProtocol_WriteU32(
    BoardTest_U16 *bytes,
    BoardTest_U16 offset,
    BoardTest_U32 value)
{
    bytes[offset] = (BoardTest_U16)((value >> 24U) & 0x00FFUL);
    bytes[offset + 1U] = (BoardTest_U16)((value >> 16U) & 0x00FFUL);
    bytes[offset + 2U] = (BoardTest_U16)((value >> 8U) & 0x00FFUL);
    bytes[offset + 3U] = (BoardTest_U16)(value & 0x00FFUL);
}

static BoardTest_U16 BoardHostProtocol_CalculateCrc(
    const BoardTest_U16 *bytes,
    BoardTest_U16 length)
{
    BoardTest_U16 crc;
    BoardTest_U16 index;
    BoardTest_U16 bit;

    crc = 0xFFFFU;
    for(index = 0U; index < length; index++)
    {
        crc ^= (BoardTest_U16)((bytes[index] & 0x00FFU) << 8U);
        for(bit = 0U; bit < 8U; bit++)
        {
            if((crc & 0x8000U) != 0U)
            {
                crc = (BoardTest_U16)((crc << 1U) ^ 0x1021U);
            }
            else
            {
                crc = (BoardTest_U16)(crc << 1U);
            }
        }
    }

    return crc;
}

static void BoardHostProtocol_WriteFloat(
    BoardTest_U16 *bytes,
    BoardTest_U16 offset,
    float value)
{
    union
    {
        float floatValue;
        BoardTest_U32 rawValue;
    } converter;

    converter.floatValue = value;
    BoardHostProtocol_WriteU32(bytes, offset, converter.rawValue);
}

static void BoardHostProtocol_BuildResponse(
    BoardTest_U16 opcode,
    BoardTest_U16 sequence,
    BoardTest_U16 status,
    const BoardTest_Record *record,
    BoardTest_U16 *response,
    BoardTest_U16 *responseLength)
{
    BoardTest_U16 crc;
    BoardTest_U16 recordId;
    BoardTest_U16 recordResult;
    BoardTest_U16 recordError;
    BoardTest_U32 rawValue;
    float measuredValue;
    float expectedMin;
    float expectedMax;
    const BoardProfile_Descriptor *profile;
    const BoardProfile_HardwareDescriptor *hardware;

    recordId = BOARD_TEST_INVALID_ID;
    recordResult = BOARD_TEST_RESULT_NOT_RUN;
    recordError = BOARD_TEST_ERROR_NONE;
    rawValue = 0UL;
    measuredValue = 0.0F;
    expectedMin = 0.0F;
    expectedMax = 0.0F;
    profile = 0;
    hardware = 0;

    if(record != 0)
    {
        recordId = record->id;
        recordResult = record->result;
        recordError = record->errorCode;
        rawValue = record->rawValue;
        measuredValue = record->measuredValue;
        expectedMin = record->expectedMin;
        expectedMax = record->expectedMax;
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_GET_STATUS)
    {
        rawValue =
            ((BoardTest_U32)gBoardTestCommunicationStandbyStatus.enabledMask <<
             16U) |
            (BoardTest_U32)gBoardTestCommunicationStandbyStatus.waitingMask;
        measuredValue =
            (float)gBoardTestCommunicationStandbyStatus.receivedMask;
        expectedMin =
            (float)gBoardTestCommunicationStandbyStatus.repliedMask;
        expectedMax =
            (float)gBoardTestCommunicationStandbyStatus.failedMask;
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_GET_BOARD_INFO)
    {
        profile = BoardProfile_GetCurrent();
        hardware = BoardProfile_GetCurrentHardware();
        if(profile != 0)
        {
            rawValue = BoardProfile_GetEffectiveCapabilities();
        }
        if(hardware != 0)
        {
            recordResult = hardware->hardwareRevision;
            recordError = hardware->pinMapRevision;
        }

        measuredValue = (float)gBoardProfileRuntimeStatus.selectedId;
        expectedMin =
            (float)gBoardProfileRuntimeStatus.detectedEthernetInterface;
        expectedMax = (float)gBoardProfileRuntimeStatus.state;
    }

    response[0U] = BOARD_HOST_PROTOCOL_RESPONSE_MAGIC_0;
    response[1U] = BOARD_HOST_PROTOCOL_RESPONSE_MAGIC_1;
    response[2U] = BOARD_HOST_PROTOCOL_VERSION;
    response[3U] = opcode & 0x00FFU;
    BoardHostProtocol_WriteU16(response, 4U, sequence);
    BoardHostProtocol_WriteU16(response, 6U, status);
    BoardHostProtocol_WriteU16(
        response,
        8U,
        gBoardTestCommandMailbox.lastCommandResult);
    BoardHostProtocol_WriteU16(response, 10U, gBoardTestManager.mode);
    BoardHostProtocol_WriteU16(response, 12U, gBoardTestManager.activeStage);
    BoardHostProtocol_WriteU16(response, 14U, gBoardTestManager.activeTestId);
    BoardHostProtocol_WriteU16(response, 16U, recordId);
    BoardHostProtocol_WriteU16(response, 18U, recordResult);
    BoardHostProtocol_WriteU16(response, 20U, recordError);
    BoardHostProtocol_WriteU32(response, 22U, rawValue);
    BoardHostProtocol_WriteFloat(response, 26U, measuredValue);
    BoardHostProtocol_WriteFloat(response, 30U, expectedMin);
    BoardHostProtocol_WriteFloat(response, 34U, expectedMax);
    crc = BoardHostProtocol_CalculateCrc(response, 38U);
    BoardHostProtocol_WriteU16(response, 38U, crc);
    *responseLength = BOARD_HOST_PROTOCOL_RESPONSE_SIZE;
}

void BoardHostProtocol_HandleRequest(
    const BoardTest_U16 *request,
    BoardTest_U16 requestLength,
    BoardTest_U16 *response,
    BoardTest_U16 *responseLength)
{
    BoardTest_U16 opcode;
    BoardTest_U16 sequence;
    BoardTest_U16 stage;
    BoardTest_U16 flags;
    BoardTest_U16 argument;
    BoardTest_U16 requestCrc;
    BoardTest_U16 status;
    const BoardTest_Record *record;
    BoardTest_Result profileResult;

    opcode = 0U;
    sequence = 0U;
    stage = BOARD_TEST_STAGE_BOARD_ONLY;
    flags = 0U;
    argument = BOARD_TEST_INVALID_ID;
    status = BOARD_HOST_PROTOCOL_STATUS_FRAME;
    record = 0;
    profileResult = BOARD_TEST_RESULT_NOT_RUN;

    if((response == 0) || (responseLength == 0))
    {
        return;
    }

    if((request == 0) ||
       (requestLength != BOARD_HOST_PROTOCOL_REQUEST_SIZE))
    {
        BoardHostProtocol_BuildResponse(opcode,
                                        sequence,
                                        status,
                                        record,
                                        response,
                                        responseLength);
        return;
    }

    opcode = request[3U] & 0x00FFU;
    sequence = BoardHostProtocol_ReadU16(request, 4U);
    stage = request[6U] & 0x00FFU;
    flags = request[7U] & 0x00FFU;
    argument = BoardHostProtocol_ReadU16(request, 8U);
    requestCrc = BoardHostProtocol_ReadU16(request, 10U);

    if((request[0U] != BOARD_HOST_PROTOCOL_REQUEST_MAGIC_0) ||
       (request[1U] != BOARD_HOST_PROTOCOL_REQUEST_MAGIC_1) ||
       (requestCrc != BoardHostProtocol_CalculateCrc(request, 10U)))
    {
        status = BOARD_HOST_PROTOCOL_STATUS_FRAME;
    }
    else if(request[2U] != BOARD_HOST_PROTOCOL_VERSION)
    {
        status = BOARD_HOST_PROTOCOL_STATUS_VERSION;
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_PING)
    {
        status = BOARD_HOST_PROTOCOL_STATUS_OK;
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_GET_STATUS)
    {
        status = BOARD_HOST_PROTOCOL_STATUS_OK;
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_GET_BOARD_INFO)
    {
        status = BOARD_HOST_PROTOCOL_STATUS_OK;
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_GET_RECORD)
    {
        record = BoardTest_GetRecord(argument);
        status = (record != 0) ? BOARD_HOST_PROTOCOL_STATUS_OK :
                                 BOARD_HOST_PROTOCOL_STATUS_TEST_ID;
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_STOP)
    {
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_STOP;
        status = BOARD_HOST_PROTOCOL_STATUS_OK;
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_SELECT_BOARD_PROFILE)
    {
        if((gBoardTestManager.mode != BOARD_TEST_MODE_IDLE) ||
           (gBoardTestCommandMailbox.command != BOARD_TEST_COMMAND_NONE))
        {
            status = BOARD_HOST_PROTOCOL_STATUS_BUSY;
        }
        else
        {
            profileResult = BoardProfile_Select(argument);
            status = (profileResult == BOARD_TEST_RESULT_PASS) ?
                BOARD_HOST_PROTOCOL_STATUS_OK : BOARD_HOST_PROTOCOL_STATUS_PROFILE;
        }
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_CONFIRM_BOARD_PROFILE)
    {
        if((gBoardTestManager.mode != BOARD_TEST_MODE_IDLE) ||
           (gBoardTestCommandMailbox.command != BOARD_TEST_COMMAND_NONE))
        {
            status = BOARD_HOST_PROTOCOL_STATUS_BUSY;
        }
        else
        {
            profileResult = BoardProfile_Confirm();
            status = (profileResult == BOARD_TEST_RESULT_PASS) ?
                BOARD_HOST_PROTOCOL_STATUS_OK : BOARD_HOST_PROTOCOL_STATUS_PROFILE;
        }
    }
    else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_CLEAR_BOARD_PROFILE)
    {
        if((gBoardTestManager.mode != BOARD_TEST_MODE_IDLE) ||
           (gBoardTestCommandMailbox.command != BOARD_TEST_COMMAND_NONE))
        {
            status = BOARD_HOST_PROTOCOL_STATUS_BUSY;
        }
        else
        {
            BoardTest_TargetKeepHostProtocolForManualTest();
            BoardTest_Stop();
            BoardProfile_Clear();
            status = BOARD_HOST_PROTOCOL_STATUS_OK;
        }
    }
    else if((opcode == BOARD_HOST_PROTOCOL_COMMAND_START_BOARD) ||
            (opcode == BOARD_HOST_PROTOCOL_COMMAND_START_EXTERNAL) ||
            (opcode == BOARD_HOST_PROTOCOL_COMMAND_START_HPD) ||
            (opcode == BOARD_HOST_PROTOCOL_COMMAND_START_SINGLE))
    {
        if(BoardProfile_IsConfirmed() == 0U)
        {
            status = BOARD_HOST_PROTOCOL_STATUS_PROFILE;
        }
        else if((gBoardTestManager.mode != BOARD_TEST_MODE_IDLE) ||
           (gBoardTestCommandMailbox.command != BOARD_TEST_COMMAND_NONE))
        {
            status = BOARD_HOST_PROTOCOL_STATUS_BUSY;
        }
        else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_START_BOARD)
        {
            gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_START_AUTO;
            status = BOARD_HOST_PROTOCOL_STATUS_OK;
        }
        else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_START_EXTERNAL)
        {
            gBoardTestCommandMailbox.command =
                BOARD_TEST_COMMAND_START_EXTERNAL_CONNECTED;
            status = BOARD_HOST_PROTOCOL_STATUS_OK;
        }
        else if(opcode == BOARD_HOST_PROTOCOL_COMMAND_START_HPD)
        {
            if(stage > BOARD_TEST_HPD_INPUT_REAL_ADC)
            {
                status = BOARD_HOST_PROTOCOL_STATUS_ARGUMENT;
            }
            else
            {
                gBoardTestCommandMailbox.hpdInputSource = stage;
                gBoardTestCommandMailbox.command =
                    BOARD_TEST_COMMAND_START_HPD_INJECTION;
                status = BOARD_HOST_PROTOCOL_STATUS_OK;
            }
        }
        else if((stage > BOARD_TEST_STAGE_HPD_INJECTION) ||
                (BoardTest_GetRecord(argument) == 0))
        {
            status = (stage > BOARD_TEST_STAGE_HPD_INJECTION) ?
                BOARD_HOST_PROTOCOL_STATUS_ARGUMENT :
                BOARD_HOST_PROTOCOL_STATUS_TEST_ID;
        }
        else if(((argument == BOARD_TEST_ID_DIDO_FPGA_DO_EXTERNAL) ||
                 (argument == BOARD_TEST_ID_HDO_FPGA_EXTERNAL)) &&
                ((flags & BOARD_HOST_PROTOCOL_FLAG_OUTPUT_ARMED) == 0U))
        {
            status = BOARD_HOST_PROTOCOL_STATUS_ARGUMENT;
        }
        else
        {
            gBoardTestCommandMailbox.stage = stage;
            gBoardTestCommandMailbox.testId = argument;
            if(argument == BOARD_TEST_ID_DIDO_FPGA_DO_EXTERNAL)
            {
                gBoardTestCommandMailbox.fpgaDoTestArmKey =
                    BOARD_DIDO_TEST_ARM_KEY;
                gBoardTestCommandMailbox.fpgaDoChannelMask =
                    BOARD_DIDO_DEFAULT_CHANNEL_MASK;
            }
            else if(argument == BOARD_TEST_ID_HDO_FPGA_EXTERNAL)
            {
                gBoardTestCommandMailbox.fpgaHdoTestArmKey =
                    BOARD_HDO_TEST_ARM_KEY;
                gBoardTestCommandMailbox.fpgaHdoChannelMask =
                    BOARD_HDO_DEFAULT_CHANNEL_MASK;
            }
            gBoardTestCommandMailbox.command =
                BOARD_TEST_COMMAND_START_SINGLE;
            status = BOARD_HOST_PROTOCOL_STATUS_OK;
        }
    }
    else
    {
        status = BOARD_HOST_PROTOCOL_STATUS_COMMAND;
    }

    BoardHostProtocol_BuildResponse(opcode,
                                    sequence,
                                    status,
                                    record,
                                    response,
                                    responseLength);
}

static BoardTest_Result BoardTest_TargetRunFpgaDoExternal(
    BoardTest_Record *record)
{
    BoardTest_Result result;
    BoardTest_U16 armKey;
    BoardTest_U16 channelMask;

    armKey = gBoardTestCommandMailbox.fpgaDoTestArmKey;
    channelMask = gBoardTestCommandMailbox.fpgaDoChannelMask;
    if((gBoardTestManager.mode == BOARD_TEST_MODE_AUTO) &&
       (gBoardTestManager.activeStage ==
        BOARD_TEST_STAGE_EXTERNAL_CONNECTED))
    {
        armKey = BOARD_DIDO_TEST_ARM_KEY;
        channelMask = BOARD_DIDO_DEFAULT_CHANNEL_MASK;
    }

    result = BoardDido_RunFpgaDoExternalTest(
        record,
        armKey,
        channelMask);
    gBoardTestCommandMailbox.fpgaDoTestArmKey = 0U;
    return result;
}

static BoardTest_Result BoardTest_TargetRunFpgaHdoExternal(
    BoardTest_Record *record)
{
    BoardTest_Result result;

    result = BoardHdo_RunFpgaExternalTest(
        record,
        gBoardTestCommandMailbox.fpgaHdoTestArmKey,
        gBoardTestCommandMailbox.fpgaHdoChannelMask);
    gBoardTestCommandMailbox.fpgaHdoTestArmKey = 0U;
    return result;
}

static BoardTest_Result BoardTest_TargetRunFpgaDiExternal(
    BoardTest_Record *record)
{
    BoardTest_U16 channelMask;

    channelMask = gBoardTestCommandMailbox.fpgaDiChannelMask;
    if(gBoardTestManager.mode == BOARD_TEST_MODE_AUTO)
    {
        channelMask = BOARD_DI_DEFAULT_CHANNEL_MASK;
    }

    return BoardDi_RunFpgaExternalTest(
        record,
        channelMask);
}

static void BoardTest_TargetResetStandbyServiceStatus(
    BoardTest_StandbyServiceStatus *status)
{
    status->state = BOARD_TEST_STANDBY_DISABLED;
    status->errorCode = BOARD_TEST_ERROR_NONE;
    status->receiveCount = 0U;
    status->replyCount = 0U;
}

static void BoardTest_TargetResetCommunicationStandbyStatus(
    BoardTest_U16 enabledMask)
{
    gBoardTestCommunicationStandbyStatus.enabledMask = enabledMask;
    gBoardTestCommunicationStandbyStatus.waitingMask = 0U;
    gBoardTestCommunicationStandbyStatus.receivedMask = 0U;
    gBoardTestCommunicationStandbyStatus.repliedMask = 0U;
    gBoardTestCommunicationStandbyStatus.failedMask = 0U;
    BoardTest_TargetResetStandbyServiceStatus(
        &gBoardTestCommunicationStandbyStatus.can);
    BoardTest_TargetResetStandbyServiceStatus(
        &gBoardTestCommunicationStandbyStatus.scib);
    BoardTest_TargetResetStandbyServiceStatus(
        &gBoardTestCommunicationStandbyStatus.ethernet);
}

static void BoardTest_TargetEnableEthernetStandby(void)
{
    if(BoardTest_TargetHostBackend == BOARD_TEST_HOST_BACKEND_W5500)
    {
        BoardW5500_EnableTcpStandby();
    }
    else
    {
        BoardEthernet_EnableW5300TcpStandby();
    }
}

static void BoardTest_TargetDisableEthernetStandby(void)
{
    if(BoardTest_TargetHostBackend == BOARD_TEST_HOST_BACKEND_W5500)
    {
        BoardW5500_DisableTcpStandby();
    }
    else
    {
        BoardEthernet_DisableW5300TcpStandby();
    }
}

static void BoardTest_TargetServiceEthernetStandby(
    BoardTest_StandbyServiceStatus *status)
{
    if(BoardTest_TargetHostBackend == BOARD_TEST_HOST_BACKEND_W5500)
    {
        BoardW5500_ServiceTcpStandby(status);
    }
    else
    {
        BoardEthernet_ServiceW5300TcpStandby(status);
    }
}

static void BoardTest_TargetDisableCommunicationStandbyServices(void)
{
    BoardSci_DisableRs485ExternalStandby();
    BoardSci_DisableRs422ExternalStandby();
    BoardCan_DisableExternalStandby();
    BoardTest_TargetDisableEthernetStandby();
    gBoardTestCommunicationStandbyStatus.can.state =
        BOARD_TEST_STANDBY_DISABLED;
    gBoardTestCommunicationStandbyStatus.scib.state =
        BOARD_TEST_STANDBY_DISABLED;
    gBoardTestCommunicationStandbyStatus.ethernet.state =
        BOARD_TEST_STANDBY_DISABLED;
}

/* Keep Socket0 available for the Qt host while a non-Ethernet manual test runs. */
static void BoardTest_TargetKeepHostProtocolForManualTest(void)
{
    BoardTest_U16 ethernetWasEnabled;

    ethernetWasEnabled = BoardTest_TargetCommunicationStandbyMask &
                         BOARD_TEST_COMM_STANDBY_ETHERNET;
    BoardSci_DisableRs485ExternalStandby();
    BoardSci_DisableRs422ExternalStandby();
    BoardCan_DisableExternalStandby();
    gBoardTestCommunicationStandbyStatus.can.state =
        BOARD_TEST_STANDBY_DISABLED;
    gBoardTestCommunicationStandbyStatus.scib.state =
        BOARD_TEST_STANDBY_DISABLED;
    BoardTest_TargetCommunicationStandbyMask =
        BOARD_TEST_COMM_STANDBY_ETHERNET;
    BoardTest_TargetCommunicationStandbyPending = 0U;

    if(ethernetWasEnabled == 0U)
    {
        BoardTest_TargetEnableEthernetStandby();
        gBoardTestCommunicationStandbyStatus.ethernet.state =
            BOARD_TEST_STANDBY_WAITING;
    }
}

static BoardTest_U16 BoardTest_TargetIsEthernetTest(BoardTest_U16 testId)
{
    return (testId == BOARD_TEST_ID_ETHERNET) ||
           (testId == BOARD_TEST_ID_ETHERNET_SOCKET) ||
           (testId == BOARD_TEST_ID_ETHERNET_TCP_LINK) ||
           (testId == BOARD_TEST_ID_ETHERNET_TCP_ECHO) ||
           (testId == BOARD_TEST_ID_ETHERNET_TCP_STABILITY);
}

static void BoardTest_TargetStopCommunicationStandby(void)
{
    BoardTest_TargetDisableCommunicationStandbyServices();
    BoardTest_TargetCommunicationStandbyMask = 0U;
    BoardTest_TargetCommunicationStandbyPending = 0U;
    BoardTest_TargetResetCommunicationStandbyStatus(0U);
}

static BoardTest_U16 BoardTest_TargetFilterCommunicationStandbyMask(
    BoardTest_U16 mask)
{
    if(BoardProfile_IsTestSupported(BOARD_TEST_ID_CAN_EXTERNAL) == 0U)
    {
        mask &= (BoardTest_U16)(~BOARD_TEST_COMM_STANDBY_CAN);
    }
    if((BoardProfile_IsTestSupported(
            BOARD_TEST_ID_SCI_RS485_EXTERNAL) == 0U) &&
       (BoardProfile_IsTestSupported(
            BOARD_TEST_ID_RS422_EXTERNAL) == 0U))
    {
        mask &= (BoardTest_U16)(~BOARD_TEST_COMM_STANDBY_SCIB);
    }
    return mask;
}

static void BoardTest_TargetStartCommunicationStandbyServices(void)
{
    BoardTest_U16 mask = BoardTest_TargetCommunicationStandbyMask;

    if((mask & BOARD_TEST_COMM_STANDBY_CAN) != 0U)
    {
        if(BoardCan_EnableExternalStandby() != 0U)
        {
            gBoardTestCommunicationStandbyStatus.can.state =
                BOARD_TEST_STANDBY_WAITING;
        }
        else
        {
            gBoardTestCommunicationStandbyStatus.can.state =
                BOARD_TEST_STANDBY_FAILED;
            gBoardTestCommunicationStandbyStatus.can.errorCode =
                BOARD_TEST_ERROR_CAN_EXTERNAL;
        }
    }

    if((mask & BOARD_TEST_COMM_STANDBY_SCIB) != 0U)
    {
        BoardTest_U16 serialEnabled;

        if(BoardProfile_IsTestSupported(BOARD_TEST_ID_RS422_EXTERNAL) != 0U)
        {
            serialEnabled = BoardSci_EnableRs422ExternalStandby();
        }
        else
        {
            serialEnabled = BoardSci_EnableRs485ExternalStandby();
        }

        if(serialEnabled != 0U)
        {
            gBoardTestCommunicationStandbyStatus.scib.state =
                BOARD_TEST_STANDBY_WAITING;
        }
        else
        {
            gBoardTestCommunicationStandbyStatus.scib.state =
                BOARD_TEST_STANDBY_FAILED;
            gBoardTestCommunicationStandbyStatus.scib.errorCode =
                BOARD_TEST_ERROR_PROFILE_PINMAP;
        }
    }

    if((mask & BOARD_TEST_COMM_STANDBY_ETHERNET) != 0U)
    {
        BoardTest_TargetEnableEthernetStandby();
        gBoardTestCommunicationStandbyStatus.ethernet.state =
            BOARD_TEST_STANDBY_WAITING;
    }
}

static void BoardTest_TargetEnsureHostProtocolListener(void)
{
    if((gBoardTestManager.mode == BOARD_TEST_MODE_IDLE) &&
       (BoardTest_TargetHostBackend == BOARD_TEST_HOST_BACKEND_W5500) &&
       (gBoardTestCommunicationStandbyStatus.ethernet.state ==
        BOARD_TEST_STANDBY_FAILED))
    {
        /* No W5500 VERSIONR response: try the EMIF W5300 path. */
        BoardTest_TargetDisableEthernetStandby();
        BoardTest_TargetHostBackend = BOARD_TEST_HOST_BACKEND_W5300;
        BoardTest_TargetCommunicationStandbyMask =
            BOARD_TEST_COMM_STANDBY_ETHERNET;
        BoardTest_TargetCommunicationStandbyPending = 1U;
        BoardTest_TargetResetCommunicationStandbyStatus(
            BOARD_TEST_COMM_STANDBY_ETHERNET);
        return;
    }

    if((gBoardTestManager.mode == BOARD_TEST_MODE_IDLE) &&
       (BoardTest_TargetCommunicationStandbyMask == 0U) &&
       (BoardTest_TargetCommunicationStandbyPending == 0U))
    {
        BoardTest_TargetCommunicationStandbyMask =
            BOARD_TEST_COMM_STANDBY_ETHERNET;
        BoardTest_TargetCommunicationStandbyPending = 1U;
        BoardTest_TargetResetCommunicationStandbyStatus(
            BOARD_TEST_COMM_STANDBY_ETHERNET);
    }

    if(gBoardTestCommunicationStandbyStatus.ethernet.state ==
       BOARD_TEST_STANDBY_WAITING)
    {
        if(BoardTest_TargetHostBackend == BOARD_TEST_HOST_BACKEND_W5500)
        {
            BoardProfile_SetDetectedEthernet(
                BOARD_PROFILE_ETHERNET_SPIC_W5500);
        }
        else
        {
            BoardProfile_SetDetectedEthernet(
                BOARD_PROFILE_ETHERNET_EMIF_W5300);
        }
    }
}

static void BoardTest_TargetRefreshCommunicationStandbyMasks(void)
{
    BoardTest_U16 waitingMask = 0U;
    BoardTest_U16 receivedMask = 0U;
    BoardTest_U16 repliedMask = 0U;
    BoardTest_U16 failedMask = 0U;

#define BOARD_TEST_UPDATE_STANDBY_MASKS(bit, service)                 \
    if((BoardTest_TargetCommunicationStandbyMask & (bit)) != 0U)     \
    {                                                                \
        if((service).state == BOARD_TEST_STANDBY_WAITING)            \
        {                                                            \
            waitingMask |= (bit);                                    \
        }                                                            \
        if((service).receiveCount != 0U)                             \
        {                                                            \
            receivedMask |= (bit);                                   \
        }                                                            \
        if((service).replyCount != 0U)                               \
        {                                                            \
            repliedMask |= (bit);                                    \
        }                                                            \
        if(((service).state == BOARD_TEST_STANDBY_FAILED) ||         \
           ((service).errorCode != BOARD_TEST_ERROR_NONE))           \
        {                                                            \
            failedMask |= (bit);                                     \
        }                                                            \
    }

    BOARD_TEST_UPDATE_STANDBY_MASKS(
        BOARD_TEST_COMM_STANDBY_CAN,
        gBoardTestCommunicationStandbyStatus.can);
    BOARD_TEST_UPDATE_STANDBY_MASKS(
        BOARD_TEST_COMM_STANDBY_SCIB,
        gBoardTestCommunicationStandbyStatus.scib);
    BOARD_TEST_UPDATE_STANDBY_MASKS(
        BOARD_TEST_COMM_STANDBY_ETHERNET,
        gBoardTestCommunicationStandbyStatus.ethernet);

#undef BOARD_TEST_UPDATE_STANDBY_MASKS

    gBoardTestCommunicationStandbyStatus.enabledMask =
        BoardTest_TargetCommunicationStandbyMask;
    gBoardTestCommunicationStandbyStatus.waitingMask = waitingMask;
    gBoardTestCommunicationStandbyStatus.receivedMask = receivedMask;
    gBoardTestCommunicationStandbyStatus.repliedMask = repliedMask;
    gBoardTestCommunicationStandbyStatus.failedMask = failedMask;
}

BoardTest_Result BoardTest_TargetExecute(BoardTest_U16 testId,
                                         BoardTest_Record *record)
{
    switch(testId)
    {
        case BOARD_TEST_ID_SYS_STARTUP:
            record->measuredValue = 1.0F;
            record->expectedMin = 1.0F;
            record->expectedMax = 1.0F;
            return BOARD_TEST_RESULT_PASS;

        case BOARD_TEST_ID_SYS_CLOCK:
            return BoardSystem_RunClockTest(record);

        case BOARD_TEST_ID_SYS_TIMER:
            return BoardSystem_RunTimerTest(record);

        case BOARD_TEST_ID_SYS_WATCHDOG:
            return BoardSystem_RunWatchdogTest(record);

        case BOARD_TEST_ID_RAM:
            return BoardSystem_RunRamTest(record);

        case BOARD_TEST_ID_FLASH:
            return BoardSystem_RunFlashTest(record);

        case BOARD_TEST_ID_I2C_RTC:
            Led_Test();
            record->rawValue = (BoardTest_U32)RtcStatus;
            if(RtcStatus == 0U)
            {
                return BOARD_TEST_RESULT_PASS;
            }
            return BOARD_TEST_RESULT_FAIL;

        case BOARD_TEST_ID_I2C_B_TMP116:
            return BoardI2c_RunTmp116Test(record);

        case BOARD_TEST_ID_SCI_HOST:
            return BoardSci_RunLoopbackTest(record);

        case BOARD_TEST_ID_SCI_RS485_EXTERNAL:
            return BoardSci_RunRs485ExternalTest(record);

        case BOARD_TEST_ID_RS422_EXTERNAL:
            return BoardSci_RunRs422ExternalTest(record);

        case BOARD_TEST_ID_SCIA_HANDHELD_EXTERNAL:
            return BoardSci_RunSciaHandheldExternalTest(record);

        case BOARD_TEST_ID_CAN_HOST:
            return BoardCan_RunLoopbackTest(record);

        case BOARD_TEST_ID_CAN_EXTERNAL:
            return BoardCan_RunExternalTest(record);

        case BOARD_TEST_ID_SPIA_INTERNAL:
            return BoardSpi_RunSpiaLoopbackTest(record);

        case BOARD_TEST_ID_SPIB_INTERNAL:
            return BoardSpi_RunSpibLoopbackTest(record);

        case BOARD_TEST_ID_SPIC_INTERNAL:
            return BoardSpi_RunSpicLoopbackTest(record);

        case BOARD_TEST_ID_SPI_EXTERNAL:
            return BoardSpi_RunSpicExternalTest(record);

        case BOARD_TEST_ID_SPIB_FLASH_EXTERNAL:
            return BoardSpi_RunSpibFlashExternalTest(record);

        case BOARD_TEST_ID_SPIB_FRAM_EXTERNAL:
            return BoardSpi_RunSpibFramExternalTest(record);

        case BOARD_TEST_ID_GPIO:
            return BoardGpio_RunProgramLedTest(record);

        case BOARD_TEST_ID_ADC_BASIC:
            return BoardAdc_RunBasicSampleTest(record);

        case BOARD_TEST_ID_PWM_BASIC:
            return BoardPwm_RunSafetyConfigTest(record);

        case BOARD_TEST_ID_EMIF_BASIC:
            return BoardEmif_RunBasicConfigTest(record);

        case BOARD_TEST_ID_EMIF_EXTERNAL:
            return BoardEmif_RunSramExternalTest(record);

        case BOARD_TEST_ID_FPGA_EMIF2_BASIC:
            return BoardFpga_RunEmif2BasicTest(record);

        case BOARD_TEST_ID_DIDO_FPGA_DO_EXTERNAL:
            return BoardTest_TargetRunFpgaDoExternal(record);

        case BOARD_TEST_ID_HDO_FPGA_EXTERNAL:
            return BoardTest_TargetRunFpgaHdoExternal(record);

        case BOARD_TEST_ID_DI_FPGA_EXTERNAL:
            return BoardTest_TargetRunFpgaDiExternal(record);

        case BOARD_TEST_ID_ETHERNET:
            if(BoardTest_TargetHostBackend == BOARD_TEST_HOST_BACKEND_W5500)
            {
                return BoardW5500_RunBasicTest(record);
            }
            return BoardEthernet_RunW5300BasicTest(record);

        case BOARD_TEST_ID_ETHERNET_SOCKET:
            return BoardEthernet_RunW5300SocketTest(record);

        case BOARD_TEST_ID_ETHERNET_TCP_LINK:
            return BoardEthernet_RunW5300TcpLinkTest(record);

        case BOARD_TEST_ID_ETHERNET_TCP_ECHO:
            return BoardEthernet_RunW5300TcpEchoTest(record);

        case BOARD_TEST_ID_ETHERNET_TCP_STABILITY:
            return BoardEthernet_RunW5300TcpStabilityTest(record);

        default:
            record->errorCode = BOARD_TEST_ERROR_UNSUPPORTED;
            return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }
}

void BoardTest_TargetPoll(void)
{
    BoardTest_U16 command = gBoardTestCommandMailbox.command;
    BoardTest_U16 standbyMask;

    if(command == BOARD_TEST_COMMAND_START_AUTO)
    {
        BoardTest_TargetStopCommunicationStandby();
        gBoardTestCommandMailbox.lastCommandResult = BoardTest_StartAuto();
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_START_EXTERNAL_CONNECTED)
    {
        gBoardTestCommandMailbox.lastCommandResult =
            BoardTest_StartStageAuto(BOARD_TEST_STAGE_EXTERNAL_CONNECTED);
        if(gBoardTestCommandMailbox.lastCommandResult ==
           BOARD_TEST_RESULT_RUNNING)
        {
            BoardTest_TargetDisableCommunicationStandbyServices();
            standbyMask =
                gBoardTestCommandMailbox.communicationStandbyEnableMask &
                BOARD_TEST_COMM_STANDBY_ALL_MASK;
            standbyMask =
                BoardTest_TargetFilterCommunicationStandbyMask(standbyMask);
            BoardTest_TargetCommunicationStandbyMask = standbyMask;
            BoardTest_TargetCommunicationStandbyPending =
                (standbyMask != 0U) ? 1U : 0U;
            BoardTest_TargetResetCommunicationStandbyStatus(standbyMask);
        }
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_START_HPD_INJECTION)
    {
        BoardTest_TargetStopCommunicationStandby();
        gBoardTestCommandMailbox.lastCommandResult =
            BoardTest_StartHpdInjectionAuto(
                gBoardTestCommandMailbox.hpdInputSource);
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_START_STAGE_AUTO)
    {
        gBoardTestCommandMailbox.lastCommandResult =
            BoardTest_StartStageAuto(gBoardTestCommandMailbox.stage);
        if((gBoardTestCommandMailbox.lastCommandResult ==
            BOARD_TEST_RESULT_RUNNING) &&
           (gBoardTestCommandMailbox.stage ==
            BOARD_TEST_STAGE_EXTERNAL_CONNECTED))
        {
            BoardTest_TargetDisableCommunicationStandbyServices();
            standbyMask =
                gBoardTestCommandMailbox.communicationStandbyEnableMask &
                BOARD_TEST_COMM_STANDBY_ALL_MASK;
            standbyMask =
                BoardTest_TargetFilterCommunicationStandbyMask(standbyMask);
            BoardTest_TargetCommunicationStandbyMask = standbyMask;
            BoardTest_TargetCommunicationStandbyPending =
                (standbyMask != 0U) ? 1U : 0U;
            BoardTest_TargetResetCommunicationStandbyStatus(standbyMask);
        }
        else
        {
            BoardTest_TargetStopCommunicationStandby();
        }
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_START_SINGLE)
    {
        gBoardTestCommandMailbox.lastCommandResult =
            BoardTest_StartSingleInStage(gBoardTestCommandMailbox.testId,
                                         gBoardTestCommandMailbox.stage);
        if((gBoardTestCommandMailbox.lastCommandResult ==
            BOARD_TEST_RESULT_RUNNING))
        {
            if(BoardTest_TargetIsEthernetTest(
                   gBoardTestCommandMailbox.testId) != 0U)
            {
                BoardTest_TargetDisableCommunicationStandbyServices();
                BoardTest_TargetCommunicationStandbyPending = 1U;
            }
            else
            {
                BoardTest_TargetKeepHostProtocolForManualTest();
            }
        }
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_STOP)
    {
        if(BoardTest_TargetHostBackend == BOARD_TEST_HOST_BACKEND_W5300)
        {
            BoardEthernet_AbortW5300TcpLinkTest();
            BoardEthernet_AbortW5300TcpEchoTest();
            BoardEthernet_AbortW5300TcpStabilityTest();
        }
        BoardSci_AbortRs485ExternalTest();
        BoardSci_AbortRs422ExternalTest();
        BoardSci_AbortSciaHandheldExternalTest();
        BoardCan_AbortExternalTest();
        gBoardTestCommandMailbox.fpgaDoTestArmKey = 0U;
        gBoardTestCommandMailbox.fpgaHdoTestArmKey = 0U;
        BoardTest_TargetStopCommunicationStandby();
        BoardTest_Stop();
        gBoardTestCommandMailbox.lastCommandResult = BOARD_TEST_RESULT_PASS;
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }

    BoardTest_Process(BoardTest_TargetExecute);

    BoardTest_TargetEnsureHostProtocolListener();

    if((BoardTest_TargetCommunicationStandbyPending != 0U) &&
       (gBoardTestManager.mode == BOARD_TEST_MODE_IDLE))
    {
        BoardTest_TargetStartCommunicationStandbyServices();
        BoardTest_TargetCommunicationStandbyPending = 0U;
    }

    if((BoardTest_TargetCommunicationStandbyMask != 0U) &&
       (BoardTest_TargetCommunicationStandbyPending == 0U))
    {
        if((gBoardTestManager.mode == BOARD_TEST_MODE_IDLE) &&
           ((BoardTest_TargetCommunicationStandbyMask &
             BOARD_TEST_COMM_STANDBY_SCIB) != 0U))
        {
            if(BoardProfile_IsTestSupported(
                   BOARD_TEST_ID_RS422_EXTERNAL) != 0U)
            {
                BoardSci_ServiceRs422ExternalStandby(
                    BoardTest_GetMutableRecord(
                        BOARD_TEST_ID_RS422_EXTERNAL),
                    &gBoardTestCommunicationStandbyStatus.scib);
            }
            else
            {
                BoardSci_ServiceRs485ExternalStandby(
                    BoardTest_GetMutableRecord(
                        BOARD_TEST_ID_SCI_RS485_EXTERNAL),
                    &gBoardTestCommunicationStandbyStatus.scib);
            }
        }

        if((gBoardTestManager.mode == BOARD_TEST_MODE_IDLE) &&
           ((BoardTest_TargetCommunicationStandbyMask &
             BOARD_TEST_COMM_STANDBY_CAN) != 0U))
        {
            BoardCan_ServiceExternalStandby(
                BoardTest_GetMutableRecord(BOARD_TEST_ID_CAN_EXTERNAL),
                &gBoardTestCommunicationStandbyStatus.can);
        }

        if((BoardTest_TargetCommunicationStandbyMask &
            BOARD_TEST_COMM_STANDBY_ETHERNET) != 0U)
        {
            BoardTest_TargetServiceEthernetStandby(
                &gBoardTestCommunicationStandbyStatus.ethernet);
        }
    }

    BoardTest_TargetRefreshCommunicationStandbyMasks();
}
