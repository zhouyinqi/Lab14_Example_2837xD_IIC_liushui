#include "Board_W5500_Test.h"
#include "Board_Host_Protocol.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

#define BOARD_W5500_GPIO_RESET              68U
#define BOARD_W5500_GPIO_SIMO               69U
#define BOARD_W5500_GPIO_SOMI               70U
#define BOARD_W5500_GPIO_CLOCK              71U
#define BOARD_W5500_GPIO_CHIP_SELECT        72U
#define BOARD_W5500_GPIO_INTERRUPT          73U
#define BOARD_W5500_SPIC_HIGH_SPEED_MUX     15U
#define BOARD_W5500_SPIC_BRR                9U
#define BOARD_W5500_TRANSFER_TIMEOUT        60000UL
#define BOARD_W5500_POLL_LIMIT              60000UL

#define BOARD_W5500_BLOCK_COMMON            0x00U
#define BOARD_W5500_BLOCK_SOCKET0           0x01U
#define BOARD_W5500_BLOCK_SOCKET0_TX        0x02U
#define BOARD_W5500_BLOCK_SOCKET0_RX        0x03U
#define BOARD_W5500_CONTROL_WRITE           0x04U

#define BOARD_W5500_REG_MR                  0x0000U
#define BOARD_W5500_REG_GAR                 0x0001U
#define BOARD_W5500_REG_SUBR                0x0005U
#define BOARD_W5500_REG_SHAR                0x0009U
#define BOARD_W5500_REG_SIPR                0x000FU
#define BOARD_W5500_REG_RTR                 0x0019U
#define BOARD_W5500_REG_RCR                 0x001BU
#define BOARD_W5500_REG_PHYCFGR             0x002EU
#define BOARD_W5500_REG_VERSIONR            0x0039U

#define BOARD_W5500_SN_MR                   0x0000U
#define BOARD_W5500_SN_CR                   0x0001U
#define BOARD_W5500_SN_IR                   0x0002U
#define BOARD_W5500_SN_SR                   0x0003U
#define BOARD_W5500_SN_PORT                 0x0004U
#define BOARD_W5500_SN_RXBUF_SIZE           0x001EU
#define BOARD_W5500_SN_TXBUF_SIZE           0x001FU
#define BOARD_W5500_SN_TX_FSR               0x0020U
#define BOARD_W5500_SN_TX_WR                0x0024U
#define BOARD_W5500_SN_RX_RSR               0x0026U
#define BOARD_W5500_SN_RX_RD                0x0028U

#define BOARD_W5500_SOCKET_MODE_TCP         0x01U
#define BOARD_W5500_SOCKET_PORT             12001U
#define BOARD_W5500_SOCKET_BUFFER_SIZE      2U
#define BOARD_W5500_SOCKET_COMMAND_OPEN     0x01U
#define BOARD_W5500_SOCKET_COMMAND_LISTEN   0x02U
#define BOARD_W5500_SOCKET_COMMAND_CLOSE    0x10U
#define BOARD_W5500_SOCKET_COMMAND_SEND     0x20U
#define BOARD_W5500_SOCKET_COMMAND_RECV     0x40U
#define BOARD_W5500_SOCKET_STATUS_CLOSED    0x00U
#define BOARD_W5500_SOCKET_STATUS_INIT      0x13U
#define BOARD_W5500_SOCKET_STATUS_LISTEN    0x14U
#define BOARD_W5500_SOCKET_STATUS_ESTABLISHED 0x17U
#define BOARD_W5500_SOCKET_STATUS_CLOSE_WAIT 0x1CU

#define BOARD_W5500_STANDBY_DISABLED        0U
#define BOARD_W5500_STANDBY_START           1U
#define BOARD_W5500_STANDBY_WAIT_CONNECT    2U
#define BOARD_W5500_STANDBY_WAIT_RX         3U

#define BOARD_W5500_ECHO_REQUEST_0          0x42U
#define BOARD_W5500_ECHO_REQUEST_1          0x54U
#define BOARD_W5500_ECHO_REQUEST_2          0x53U
#define BOARD_W5500_ECHO_REQUEST_3          0x54U
#define BOARD_W5500_ECHO_RESPONSE_0         0x42U
#define BOARD_W5500_ECHO_RESPONSE_1         0x54U
#define BOARD_W5500_ECHO_RESPONSE_2         0x4FU
#define BOARD_W5500_ECHO_RESPONSE_3         0x4BU

volatile BoardW5500_Snapshot gBoardW5500Snapshot =
{
    0U,
    0U,
    0U,
    0U,
    BOARD_TEST_ERROR_NONE,
    0U,
    0U
};

#ifdef BOARD_TEST_HOST

BoardTest_Result BoardW5500_RunBasicTest(BoardTest_Record *record)
{
    record->rawValue = 0UL;
    record->measuredValue = 0.0F;
    record->expectedMin = 0.0F;
    record->expectedMax = 0.0F;
    record->errorCode = BOARD_TEST_ERROR_UNSUPPORTED;
    return BOARD_TEST_RESULT_NOT_SUPPORTED;
}

void BoardW5500_EnableTcpStandby(void)
{
}

void BoardW5500_DisableTcpStandby(void)
{
}

void BoardW5500_ServiceTcpStandby(BoardTest_StandbyServiceStatus *status)
{
    (void)status;
}

#else

static BoardTest_U16 BoardW5500_TcpStandbyEnabled = 0U;
static BoardTest_U16 BoardW5500_TcpStandbyState =
    BOARD_W5500_STANDBY_DISABLED;

static void BoardW5500_ChipSelectLow(void)
{
    GPIO_WritePin(BOARD_W5500_GPIO_CHIP_SELECT, 0U);
}

static void BoardW5500_ChipSelectHigh(void)
{
    GPIO_WritePin(BOARD_W5500_GPIO_CHIP_SELECT, 1U);
}

static BoardTest_U16 BoardW5500_TransferByte(BoardTest_U16 value,
                                              BoardTest_U16 *ok)
{
    BoardTest_U32 timeout;

    *ok = 1U;
    timeout = BOARD_W5500_TRANSFER_TIMEOUT;
    while(SpicRegs.SPISTS.bit.BUFFULL_FLAG != 0U)
    {
        if(timeout-- == 0UL)
        {
            *ok = 0U;
            return 0U;
        }
    }

    SpicRegs.SPITXBUF = (value & 0x00FFU) << 8U;
    timeout = BOARD_W5500_TRANSFER_TIMEOUT;
    while(SpicRegs.SPIFFRX.bit.RXFFST == 0U)
    {
        if(timeout-- == 0UL)
        {
            *ok = 0U;
            return 0U;
        }
    }

    value = SpicRegs.SPIRXBUF & 0x00FFU;
    SpicRegs.SPIFFRX.bit.RXFFOVFCLR = 1U;
    SpicRegs.SPIFFRX.bit.RXFFINTCLR = 1U;
    return value;
}

static BoardTest_U16 BoardW5500_BeginAccess(BoardTest_U16 address,
                                             BoardTest_U16 block,
                                             BoardTest_U16 write)
{
    BoardTest_U16 ok;

    BoardW5500_ChipSelectLow();
    (void)BoardW5500_TransferByte(address >> 8U, &ok);
    if(ok == 0U)
    {
        BoardW5500_ChipSelectHigh();
        return 0U;
    }
    (void)BoardW5500_TransferByte(address, &ok);
    if(ok == 0U)
    {
        BoardW5500_ChipSelectHigh();
        return 0U;
    }
    (void)BoardW5500_TransferByte(
        (BoardTest_U16)((block << 3U) |
                        ((write != 0U) ? BOARD_W5500_CONTROL_WRITE : 0U)),
        &ok);
    if(ok == 0U)
    {
        BoardW5500_ChipSelectHigh();
    }
    return ok;
}

static BoardTest_U16 BoardW5500_ReadByte(BoardTest_U16 address,
                                          BoardTest_U16 block,
                                          BoardTest_U16 *ok)
{
    BoardTest_U16 value;

    *ok = BoardW5500_BeginAccess(address, block, 0U);
    if(*ok == 0U)
    {
        return 0U;
    }
    value = BoardW5500_TransferByte(0U, ok);
    BoardW5500_ChipSelectHigh();
    return value;
}

static BoardTest_U16 BoardW5500_WriteByte(BoardTest_U16 address,
                                           BoardTest_U16 block,
                                           BoardTest_U16 value)
{
    BoardTest_U16 ok;

    ok = BoardW5500_BeginAccess(address, block, 1U);
    if(ok != 0U)
    {
        (void)BoardW5500_TransferByte(value, &ok);
        BoardW5500_ChipSelectHigh();
    }
    return ok;
}

static BoardTest_U16 BoardW5500_ReadBytes(BoardTest_U16 address,
                                           BoardTest_U16 block,
                                           BoardTest_U16 *data,
                                           BoardTest_U16 length)
{
    BoardTest_U16 index;
    BoardTest_U16 ok;

    ok = BoardW5500_BeginAccess(address, block, 0U);
    if(ok == 0U)
    {
        return 0U;
    }
    for(index = 0U; index < length; index++)
    {
        data[index] = BoardW5500_TransferByte(0U, &ok);
        if(ok == 0U)
        {
            break;
        }
    }
    BoardW5500_ChipSelectHigh();
    return ok;
}

static BoardTest_U16 BoardW5500_WriteBytes(BoardTest_U16 address,
                                            BoardTest_U16 block,
                                            const BoardTest_U16 *data,
                                            BoardTest_U16 length)
{
    BoardTest_U16 index;
    BoardTest_U16 ok;

    ok = BoardW5500_BeginAccess(address, block, 1U);
    if(ok == 0U)
    {
        return 0U;
    }
    for(index = 0U; index < length; index++)
    {
        (void)BoardW5500_TransferByte(data[index], &ok);
        if(ok == 0U)
        {
            break;
        }
    }
    BoardW5500_ChipSelectHigh();
    return ok;
}

static BoardTest_U16 BoardW5500_ReadU16(BoardTest_U16 address,
                                         BoardTest_U16 block,
                                         BoardTest_U16 *ok)
{
    BoardTest_U16 bytes[2];

    *ok = BoardW5500_ReadBytes(address, block, bytes, 2U);
    return (BoardTest_U16)((bytes[0U] << 8U) | bytes[1U]);
}

static BoardTest_U16 BoardW5500_WriteU16(BoardTest_U16 address,
                                          BoardTest_U16 block,
                                          BoardTest_U16 value)
{
    BoardTest_U16 bytes[2];

    bytes[0U] = value >> 8U;
    bytes[1U] = value & 0x00FFU;
    return BoardW5500_WriteBytes(address, block, bytes, 2U);
}

static void BoardW5500_InitGpio(void)
{
    GPIO_SetupPinMux(BOARD_W5500_GPIO_SIMO, GPIO_MUX_CPU1,
                     BOARD_W5500_SPIC_HIGH_SPEED_MUX);
    GPIO_SetupPinOptions(BOARD_W5500_GPIO_SIMO, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(BOARD_W5500_GPIO_SOMI, GPIO_MUX_CPU1,
                     BOARD_W5500_SPIC_HIGH_SPEED_MUX);
    GPIO_SetupPinOptions(BOARD_W5500_GPIO_SOMI, GPIO_INPUT,
                         GPIO_ASYNC | GPIO_PULLUP);
    GPIO_SetupPinMux(BOARD_W5500_GPIO_CLOCK, GPIO_MUX_CPU1,
                     BOARD_W5500_SPIC_HIGH_SPEED_MUX);
    GPIO_SetupPinOptions(BOARD_W5500_GPIO_CLOCK, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(BOARD_W5500_GPIO_CHIP_SELECT, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(BOARD_W5500_GPIO_CHIP_SELECT, GPIO_OUTPUT,
                         GPIO_PUSHPULL | GPIO_PULLUP);
    BoardW5500_ChipSelectHigh();
}

static void BoardW5500_InitSpic(void)
{
    EALLOW;
    DevCfgRegs.CPUSEL6.bit.SPI_C = 0U;
    CpuSysRegs.PCLKCR8.bit.SPI_C = 1U;
    EDIS;

    SpicRegs.SPICCR.bit.SPISWRESET = 0U;
    SpicRegs.SPICCR.all = 0x0007U;
    /* F28377D CLK_PHASE=1 maps to the W5500 conventional SPI mode 0. */
    SpicRegs.SPICTL.all = 0x000EU;
    SpicRegs.SPIBRR = BOARD_W5500_SPIC_BRR;
    SpicRegs.SPIFFTX.all = 0xE040U;
    SpicRegs.SPIFFRX.all = 0x2044U;
    SpicRegs.SPIFFCT.all = 0x0000U;
    SpicRegs.SPIPRI.bit.FREE = 1U;
    SpicRegs.SPICCR.bit.SPISWRESET = 1U;
}

static BoardTest_U16 BoardW5500_ResetAndProbe(void)
{
    BoardTest_U16 attempt;
    BoardTest_U16 ok;
    BoardTest_U16 version;

    BoardW5500_InitGpio();
    BoardW5500_InitSpic();
    version = 0U;
    ok = 0U;
    for(attempt = 0U; attempt < 3U; attempt++)
    {
        version = BoardW5500_ReadByte(BOARD_W5500_REG_VERSIONR,
                                      BOARD_W5500_BLOCK_COMMON,
                                      &ok);
        if((ok != 0U) && (version == BOARD_W5500_VERSION_EXPECTED))
        {
            break;
        }
        DELAY_US(10U);
    }
    gBoardW5500Snapshot.version = version;
    if((ok == 0U) || (version != BOARD_W5500_VERSION_EXPECTED))
    {
        return 0U;
    }

    gBoardW5500Snapshot.statusMask =
        BOARD_W5500_STATUS_GPIO_CONFIGURED |
        BOARD_W5500_STATUS_SPI_CONFIGURED |
        BOARD_W5500_STATUS_VERSION_VALID;
    return 1U;
}

static BoardTest_U16 BoardW5500_WaitCommand(void)
{
    BoardTest_U32 timeout;
    BoardTest_U16 ok;

    for(timeout = 0UL; timeout < BOARD_W5500_POLL_LIMIT; timeout++)
    {
        if(BoardW5500_ReadByte(BOARD_W5500_SN_CR,
                               BOARD_W5500_BLOCK_SOCKET0,
                               &ok) == 0U)
        {
            return ok;
        }
    }
    return 0U;
}

static BoardTest_U16 BoardW5500_WaitSocketState(BoardTest_U16 expectedState)
{
    BoardTest_U32 timeout;
    BoardTest_U16 socketStatus;
    BoardTest_U16 ok;

    socketStatus = 0U;
    for(timeout = 0UL; timeout < BOARD_W5500_POLL_LIMIT; timeout++)
    {
        socketStatus = BoardW5500_ReadByte(BOARD_W5500_SN_SR,
                                            BOARD_W5500_BLOCK_SOCKET0,
                                            &ok);
        if((ok == 0U) || (socketStatus == expectedState))
        {
            break;
        }
    }
    gBoardW5500Snapshot.socketStatus = socketStatus;
    return (socketStatus == expectedState) ? 1U : 0U;
}

static BoardTest_U16 BoardW5500_ConfigureCommon(void)
{
    static const BoardTest_U16 gateway[4] = {192U, 168U, 1U, 1U};
    static const BoardTest_U16 subnet[4] = {255U, 255U, 255U, 0U};
    static const BoardTest_U16 mac[6] =
        {0x1AU, 0x2CU, 0x11U, 0x01U, 0x03U, 0xA8U};
    static const BoardTest_U16 ip[4] = {192U, 168U, 1U, 111U};
    BoardTest_U16 ok;

    ok = BoardW5500_WriteByte(BOARD_W5500_REG_MR,
                               BOARD_W5500_BLOCK_COMMON,
                               0x80U);
    DELAY_US(1000U);
    if(ok == 0U)
    {
        return 0U;
    }
    if((BoardW5500_WriteBytes(BOARD_W5500_REG_GAR,
                               BOARD_W5500_BLOCK_COMMON,
                               gateway,
                               4U) == 0U) ||
       (BoardW5500_WriteBytes(BOARD_W5500_REG_SUBR,
                               BOARD_W5500_BLOCK_COMMON,
                               subnet,
                               4U) == 0U) ||
       (BoardW5500_WriteBytes(BOARD_W5500_REG_SHAR,
                               BOARD_W5500_BLOCK_COMMON,
                               mac,
                               6U) == 0U) ||
       (BoardW5500_WriteBytes(BOARD_W5500_REG_SIPR,
                               BOARD_W5500_BLOCK_COMMON,
                               ip,
                               4U) == 0U) ||
       (BoardW5500_WriteU16(BOARD_W5500_REG_RTR,
                             BOARD_W5500_BLOCK_COMMON,
                             2000U) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_REG_RCR,
                              BOARD_W5500_BLOCK_COMMON,
                              7U) == 0U))
    {
        return 0U;
    }

    /* Match the proven project: restart PHY after loading network values. */
    if((BoardW5500_WriteByte(BOARD_W5500_REG_PHYCFGR,
                              BOARD_W5500_BLOCK_COMMON,
                              0xF8U) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_REG_PHYCFGR,
                              BOARD_W5500_BLOCK_COMMON,
                              0x78U) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_REG_PHYCFGR,
                              BOARD_W5500_BLOCK_COMMON,
                              0xF8U) == 0U))
    {
        return 0U;
    }
    DELAY_US(1000U);

    gBoardW5500Snapshot.phyConfig =
        BoardW5500_ReadByte(BOARD_W5500_REG_PHYCFGR,
                             BOARD_W5500_BLOCK_COMMON,
                             &ok);
    if(ok != 0U)
    {
        gBoardW5500Snapshot.statusMask |= BOARD_W5500_STATUS_PHY_READABLE;
    }
    return ok;
}

static BoardTest_U16 BoardW5500_StartSocket0(void)
{
    BoardTest_U16 ok;

    ok = BoardW5500_WriteByte(BOARD_W5500_SN_CR,
                               BOARD_W5500_BLOCK_SOCKET0,
                               BOARD_W5500_SOCKET_COMMAND_CLOSE);
    if((ok == 0U) || (BoardW5500_WaitCommand() == 0U))
    {
        return 0U;
    }
    if((BoardW5500_WriteByte(BOARD_W5500_SN_MR,
                              BOARD_W5500_BLOCK_SOCKET0,
                              BOARD_W5500_SOCKET_MODE_TCP) == 0U) ||
       (BoardW5500_WriteU16(BOARD_W5500_SN_PORT,
                             BOARD_W5500_BLOCK_SOCKET0,
                             BOARD_W5500_SOCKET_PORT) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_SN_RXBUF_SIZE,
                              BOARD_W5500_BLOCK_SOCKET0,
                              BOARD_W5500_SOCKET_BUFFER_SIZE) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_SN_TXBUF_SIZE,
                              BOARD_W5500_BLOCK_SOCKET0,
                              BOARD_W5500_SOCKET_BUFFER_SIZE) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_SN_IR,
                              BOARD_W5500_BLOCK_SOCKET0,
                              0x1FU) == 0U))
    {
        return 0U;
    }
    if((BoardW5500_WriteByte(BOARD_W5500_SN_CR,
                              BOARD_W5500_BLOCK_SOCKET0,
                              BOARD_W5500_SOCKET_COMMAND_OPEN) == 0U) ||
       (BoardW5500_WaitCommand() == 0U) ||
       (BoardW5500_WaitSocketState(BOARD_W5500_SOCKET_STATUS_INIT) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_SN_CR,
                              BOARD_W5500_BLOCK_SOCKET0,
                              BOARD_W5500_SOCKET_COMMAND_LISTEN) == 0U) ||
       (BoardW5500_WaitCommand() == 0U) ||
       (BoardW5500_WaitSocketState(BOARD_W5500_SOCKET_STATUS_LISTEN) == 0U))
    {
        return 0U;
    }

    gBoardW5500Snapshot.statusMask |= BOARD_W5500_STATUS_SOCKET_LISTEN;
    return 1U;
}

static void BoardW5500_SetFailure(BoardTest_StandbyServiceStatus *status)
{
    gBoardW5500Snapshot.lastError = BOARD_TEST_ERROR_ETHERNET;
    if(status != 0)
    {
        status->state = BOARD_TEST_STANDBY_FAILED;
        status->errorCode = BOARD_TEST_ERROR_ETHERNET;
    }
}

static BoardTest_U16 BoardW5500_SendBytes(const BoardTest_U16 *data,
                                           BoardTest_U16 length)
{
    BoardTest_U16 ok;
    BoardTest_U16 writePointer;
    BoardTest_U16 txFree;

    txFree = BoardW5500_ReadU16(BOARD_W5500_SN_TX_FSR,
                                 BOARD_W5500_BLOCK_SOCKET0,
                                 &ok);
    if((ok == 0U) || (txFree < length))
    {
        return 0U;
    }
    writePointer = BoardW5500_ReadU16(BOARD_W5500_SN_TX_WR,
                                      BOARD_W5500_BLOCK_SOCKET0,
                                      &ok);
    if((ok == 0U) ||
       (BoardW5500_WriteBytes(writePointer,
                               BOARD_W5500_BLOCK_SOCKET0_TX,
                               data,
                               length) == 0U) ||
       (BoardW5500_WriteU16(BOARD_W5500_SN_TX_WR,
                             BOARD_W5500_BLOCK_SOCKET0,
                             (BoardTest_U16)(writePointer + length)) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_SN_CR,
                              BOARD_W5500_BLOCK_SOCKET0,
                              BOARD_W5500_SOCKET_COMMAND_SEND) == 0U) ||
       (BoardW5500_WaitCommand() == 0U))
    {
        return 0U;
    }
    return 1U;
}

static void BoardW5500_ServiceRx(BoardTest_StandbyServiceStatus *status)
{
    BoardTest_U16 ok;
    BoardTest_U16 readPointer;
    BoardTest_U16 rxSize;
    BoardTest_U16 request[BOARD_HOST_PROTOCOL_REQUEST_SIZE];
    BoardTest_U16 response[BOARD_HOST_PROTOCOL_RESPONSE_SIZE];
    BoardTest_U16 responseLength;
    BoardTest_U16 frameLength;
    BoardTest_U16 echoResponse[4];

    rxSize = BoardW5500_ReadU16(BOARD_W5500_SN_RX_RSR,
                                 BOARD_W5500_BLOCK_SOCKET0,
                                 &ok);
    if((ok == 0U) || (rxSize < 4U))
    {
        return;
    }
    readPointer = BoardW5500_ReadU16(BOARD_W5500_SN_RX_RD,
                                     BOARD_W5500_BLOCK_SOCKET0,
                                     &ok);
    if((ok == 0U) ||
       (BoardW5500_ReadBytes(readPointer,
                              BOARD_W5500_BLOCK_SOCKET0_RX,
                              request,
                              4U) == 0U))
    {
        BoardW5500_SetFailure(status);
        return;
    }

    responseLength = 0U;
    frameLength = 0U;
    if((request[0U] == BOARD_W5500_ECHO_REQUEST_0) &&
       (request[1U] == BOARD_W5500_ECHO_REQUEST_1) &&
       (request[2U] == BOARD_W5500_ECHO_REQUEST_2) &&
       (request[3U] == BOARD_W5500_ECHO_REQUEST_3))
    {
        echoResponse[0U] = BOARD_W5500_ECHO_RESPONSE_0;
        echoResponse[1U] = BOARD_W5500_ECHO_RESPONSE_1;
        echoResponse[2U] = BOARD_W5500_ECHO_RESPONSE_2;
        echoResponse[3U] = BOARD_W5500_ECHO_RESPONSE_3;
        frameLength = 4U;
    }
    else if(rxSize >= BOARD_HOST_PROTOCOL_REQUEST_SIZE)
    {
        if(BoardW5500_ReadBytes((BoardTest_U16)(readPointer + 4U),
                                 BOARD_W5500_BLOCK_SOCKET0_RX,
                                 &request[4U],
                                 (BoardTest_U16)
                                 (BOARD_HOST_PROTOCOL_REQUEST_SIZE - 4U)) == 0U)
        {
            BoardW5500_SetFailure(status);
            return;
        }
        BoardHostProtocol_HandleRequest(request,
                                        BOARD_HOST_PROTOCOL_REQUEST_SIZE,
                                        response,
                                        &responseLength);
        frameLength = BOARD_HOST_PROTOCOL_REQUEST_SIZE;
    }
    else
    {
        return;
    }

    if((BoardW5500_WriteU16(BOARD_W5500_SN_RX_RD,
                             BOARD_W5500_BLOCK_SOCKET0,
                             (BoardTest_U16)(readPointer + frameLength)) == 0U) ||
       (BoardW5500_WriteByte(BOARD_W5500_SN_CR,
                              BOARD_W5500_BLOCK_SOCKET0,
                              BOARD_W5500_SOCKET_COMMAND_RECV) == 0U) ||
       (BoardW5500_WaitCommand() == 0U))
    {
        BoardW5500_SetFailure(status);
        return;
    }

    if(status != 0)
    {
        status->state = BOARD_TEST_STANDBY_RECEIVED;
        status->receiveCount++;
    }
    gBoardW5500Snapshot.receiveCount++;
    if((frameLength == 4U && BoardW5500_SendBytes(echoResponse, 4U) == 0U) ||
       ((frameLength != 4U) &&
        BoardW5500_SendBytes(response, responseLength) == 0U))
    {
        BoardW5500_SetFailure(status);
        return;
    }
    if(status != 0)
    {
        status->state = BOARD_TEST_STANDBY_REPLIED;
        status->replyCount++;
    }
    gBoardW5500Snapshot.replyCount++;
}

BoardTest_Result BoardW5500_RunBasicTest(BoardTest_Record *record)
{
    BoardTest_U16 version;
    BoardTest_U16 ok;
    BoardTest_U16 statusMask;

    statusMask = 0U;
    if(BoardW5500_ResetAndProbe() != 0U)
    {
        statusMask = gBoardW5500Snapshot.statusMask;
        version = gBoardW5500Snapshot.version;
        (void)BoardW5500_ConfigureCommon();
        statusMask = gBoardW5500Snapshot.statusMask;
    }
    else
    {
        version = gBoardW5500Snapshot.version;
    }
    gBoardW5500Snapshot.socketStatus = BoardW5500_ReadByte(
        BOARD_W5500_SN_SR,
        BOARD_W5500_BLOCK_SOCKET0,
        &ok);
    record->rawValue = ((BoardTest_U32)statusMask << 16U) |
                       (BoardTest_U32)version;
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)(BOARD_W5500_STATUS_GPIO_CONFIGURED |
                                  BOARD_W5500_STATUS_SPI_CONFIGURED |
                                  BOARD_W5500_STATUS_VERSION_VALID);
    record->expectedMax = (float)(BOARD_W5500_STATUS_GPIO_CONFIGURED |
                                  BOARD_W5500_STATUS_SPI_CONFIGURED |
                                  BOARD_W5500_STATUS_VERSION_VALID |
                                  BOARD_W5500_STATUS_PHY_READABLE);
    if((statusMask & (BOARD_W5500_STATUS_GPIO_CONFIGURED |
                      BOARD_W5500_STATUS_SPI_CONFIGURED |
                      BOARD_W5500_STATUS_VERSION_VALID)) ==
       (BOARD_W5500_STATUS_GPIO_CONFIGURED |
        BOARD_W5500_STATUS_SPI_CONFIGURED |
        BOARD_W5500_STATUS_VERSION_VALID))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }
    record->errorCode = BOARD_TEST_ERROR_ETHERNET;
    return BOARD_TEST_RESULT_FAIL;
}

void BoardW5500_EnableTcpStandby(void)
{
    BoardW5500_TcpStandbyEnabled = 1U;
    BoardW5500_TcpStandbyState = BOARD_W5500_STANDBY_START;
}

void BoardW5500_DisableTcpStandby(void)
{
    if((BoardW5500_TcpStandbyEnabled != 0U) &&
       (BoardW5500_TcpStandbyState != BOARD_W5500_STANDBY_DISABLED))
    {
        (void)BoardW5500_WriteByte(BOARD_W5500_SN_CR,
                                    BOARD_W5500_BLOCK_SOCKET0,
                                    BOARD_W5500_SOCKET_COMMAND_CLOSE);
    }
    BoardW5500_TcpStandbyEnabled = 0U;
    BoardW5500_TcpStandbyState = BOARD_W5500_STANDBY_DISABLED;
}

void BoardW5500_ServiceTcpStandby(BoardTest_StandbyServiceStatus *status)
{
    BoardTest_U16 socketStatus;
    BoardTest_U16 ok;

    if(BoardW5500_TcpStandbyEnabled == 0U)
    {
        return;
    }
    if(BoardW5500_TcpStandbyState == BOARD_W5500_STANDBY_START)
    {
        if((BoardW5500_ResetAndProbe() == 0U) ||
           (BoardW5500_ConfigureCommon() == 0U) ||
           (BoardW5500_StartSocket0() == 0U))
        {
            BoardW5500_TcpStandbyState = BOARD_W5500_STANDBY_DISABLED;
            BoardW5500_SetFailure(status);
            return;
        }
        BoardW5500_TcpStandbyState = BOARD_W5500_STANDBY_WAIT_CONNECT;
        if(status != 0)
        {
            status->state = BOARD_TEST_STANDBY_WAITING;
            status->errorCode = BOARD_TEST_ERROR_NONE;
        }
        return;
    }
    socketStatus = BoardW5500_ReadByte(BOARD_W5500_SN_SR,
                                       BOARD_W5500_BLOCK_SOCKET0,
                                       &ok);
    gBoardW5500Snapshot.socketStatus = socketStatus;
    if(ok == 0U)
    {
        BoardW5500_SetFailure(status);
        return;
    }
    if((socketStatus == BOARD_W5500_SOCKET_STATUS_CLOSED) ||
       (socketStatus == BOARD_W5500_SOCKET_STATUS_CLOSE_WAIT))
    {
        BoardW5500_TcpStandbyState = BOARD_W5500_STANDBY_START;
        return;
    }
    if(socketStatus == BOARD_W5500_SOCKET_STATUS_LISTEN)
    {
        BoardW5500_TcpStandbyState = BOARD_W5500_STANDBY_WAIT_CONNECT;
        if((status != 0) && (status->errorCode == BOARD_TEST_ERROR_NONE))
        {
            status->state = BOARD_TEST_STANDBY_WAITING;
        }
        return;
    }
    if(socketStatus == BOARD_W5500_SOCKET_STATUS_ESTABLISHED)
    {
        BoardW5500_TcpStandbyState = BOARD_W5500_STANDBY_WAIT_RX;
        BoardW5500_ServiceRx(status);
    }
}

#endif
