#include "Board_Ethernet_Test.h"
#include "Board_Emif_Test.h"
#include "Board_Pinmap.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

#define BOARD_ETHERNET_W5300_EMIF_BASE      0x100000UL

#define BOARD_ETHERNET_W5300_REG_MR         0x0000U
#define BOARD_ETHERNET_W5300_REG_RTR        0x001CU
#define BOARD_ETHERNET_W5300_REG_RCR        0x001EU
#define BOARD_ETHERNET_W5300_REG_TMSR0      0x0020U
#define BOARD_ETHERNET_W5300_REG_RMSR0      0x0028U
#define BOARD_ETHERNET_W5300_REG_SHAR0      0x0008U
#define BOARD_ETHERNET_W5300_REG_GAR0       0x0010U
#define BOARD_ETHERNET_W5300_REG_SUBR0      0x0014U
#define BOARD_ETHERNET_W5300_REG_SIPR0      0x0018U
#define BOARD_ETHERNET_W5300_REG_MTYPER     0x0030U
#define BOARD_ETHERNET_W5300_REG_S0_MR      0x0200U
#define BOARD_ETHERNET_W5300_REG_S0_CR      0x0202U
#define BOARD_ETHERNET_W5300_REG_S0_IMR     0x0204U
#define BOARD_ETHERNET_W5300_REG_S0_IR      0x0206U
#define BOARD_ETHERNET_W5300_REG_S0_SSR     0x0208U
#define BOARD_ETHERNET_W5300_REG_S0_PORT    0x020AU
#define BOARD_ETHERNET_W5300_REG_S0_MTU     0x0218U
#define BOARD_ETHERNET_W5300_REG_S0_KEEP    0x021AU
#define BOARD_ETHERNET_W5300_REG_S0_TTL     0x021EU

#define BOARD_ETHERNET_SOCKET0_MODE_TCP     0x0021U
#define BOARD_ETHERNET_SOCKET0_PORT         12001U
#define BOARD_ETHERNET_SOCKET0_TTL          64U
#define BOARD_ETHERNET_SOCKET0_MTU          1460U
#define BOARD_ETHERNET_SOCKET0_KEEPALIVE    0x1400U
#define BOARD_ETHERNET_SOCKET0_RECV_INT     0x0004U

#define BOARD_ETHERNET_SOCKET_CMD_OPEN      0x0001U
#define BOARD_ETHERNET_SOCKET_CMD_LISTEN    0x0002U
#define BOARD_ETHERNET_SOCKET_CMD_CLOSE     0x0010U

#define BOARD_ETHERNET_SOCKET_SR_CLOSED     0x0000U
#define BOARD_ETHERNET_SOCKET_SR_INIT       0x0013U
#define BOARD_ETHERNET_SOCKET_SR_LISTEN     0x0014U
#define BOARD_ETHERNET_SOCKET_POLL_LIMIT    60000UL

#define BOARD_ETHERNET_ARRAY_LEN(array) \
    ((BoardTest_U16)(sizeof(array) / sizeof((array)[0])))

typedef struct
{
    BoardTest_U16 offset;
    BoardTest_U16 value;
} BoardEthernet_W5300RegValue;

volatile BoardEthernet_BasicSnapshot gBoardEthernetBasicSnapshot =
{
    BOARD_ETHERNET_BACKEND_W5300_EMIF,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

volatile BoardEthernet_SocketSnapshot gBoardEthernetSocketSnapshot =
{
    BOARD_ETHERNET_BACKEND_W5300_EMIF,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

BoardTest_Result BoardEthernet_EvaluateW5300BasicStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 modeReg,
    BoardTest_U16 retryTimeReg,
    BoardTest_U16 retryCountReg,
    BoardTest_U16 txMemoryReg,
    BoardTest_U16 rxMemoryReg,
    BoardTest_U16 socket0Status,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       (((BoardTest_U32)(modeReg >> 8U) & 0x00FFUL) << 8U) |
                       ((BoardTest_U32)retryCountReg & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_ETHERNET_W5300_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_ETHERNET_W5300_DIAGNOSTIC_MASK;

    (void)retryTimeReg;
    (void)txMemoryReg;
    (void)rxMemoryReg;
    (void)socket0Status;

    if((statusMask & BOARD_ETHERNET_W5300_REQUIRED_MASK) ==
       BOARD_ETHERNET_W5300_REQUIRED_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ETHERNET;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardEthernet_EvaluateW5300SocketStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 socketModeReg,
    BoardTest_U16 socketPortReg,
    BoardTest_U16 initStatus,
    BoardTest_U16 listenStatus,
    BoardTest_U16 closedStatus,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)initStatus & 0xFF00UL) |
                       (((BoardTest_U32)listenStatus >> 8U) & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_ETHERNET_SOCKET_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_ETHERNET_SOCKET_DIAGNOSTIC_MASK;

    (void)socketModeReg;
    (void)socketPortReg;
    (void)closedStatus;

    if((statusMask & BOARD_ETHERNET_SOCKET_REQUIRED_MASK) ==
       BOARD_ETHERNET_SOCKET_REQUIRED_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ETHERNET_SOCKET;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
extern void InitXintf16Gpio(void);

static const BoardEthernet_W5300RegValue BoardEthernet_W5300CommonConfig[] =
{
    {BOARD_ETHERNET_W5300_REG_MR, BOARD_ETHERNET_W5300_MODE_EXPECTED},
    {BOARD_ETHERNET_W5300_REG_RTR, BOARD_ETHERNET_W5300_RTR_EXPECTED},
    {BOARD_ETHERNET_W5300_REG_RCR, BOARD_ETHERNET_W5300_RCR_EXPECTED},
    {BOARD_ETHERNET_W5300_REG_SHAR0, 0x1A2CU},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_SHAR0 + 0x0002U), 0x1101U},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_SHAR0 + 0x0004U), 0x03A8U},
    {BOARD_ETHERNET_W5300_REG_GAR0, 0xC0A8U},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_GAR0 + 0x0002U), 0x0101U},
    {BOARD_ETHERNET_W5300_REG_SUBR0, 0xFFFFU},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_SUBR0 + 0x0002U), 0xFF00U},
    {BOARD_ETHERNET_W5300_REG_SIPR0, 0xC0A8U},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_SIPR0 + 0x0002U), 0x016FU},
    {BOARD_ETHERNET_W5300_REG_MTYPER, 0x00FFU},
    {BOARD_ETHERNET_W5300_REG_TMSR0, BOARD_ETHERNET_W5300_MEM_EXPECTED},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_TMSR0 + 0x0002U), 0x0828U},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_TMSR0 + 0x0004U), 0x0000U},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_TMSR0 + 0x0006U), 0x0000U},
    {BOARD_ETHERNET_W5300_REG_RMSR0, BOARD_ETHERNET_W5300_MEM_EXPECTED},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_RMSR0 + 0x0002U), 0x0828U},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_RMSR0 + 0x0004U), 0x0000U},
    {(BoardTest_U16)(BOARD_ETHERNET_W5300_REG_RMSR0 + 0x0006U), 0x0000U}
};

static BoardTest_U16 BoardEthernet_IsCurrentPinmapValid(void)
{
    return (BOARD_PIN_EMIF1_W5300_CS2 == 34U) &&
           (BOARD_PIN_EMIF1_WE == 31U) &&
           (BOARD_PIN_EMIF1_OE == 37U) &&
           (BOARD_CPU2_PIN_ETH_RESET == 1U) &&
           (BOARD_CPU2_PIN_ETH_INT == 0U);
}

static volatile BoardTest_U16 *BoardEthernet_W5300Reg(BoardTest_U16 offset)
{
    return (volatile BoardTest_U16 *)
        (BOARD_ETHERNET_W5300_EMIF_BASE + (BoardTest_U32)offset);
}

static BoardTest_U16 BoardEthernet_W5300Read(BoardTest_U16 offset)
{
    return *BoardEthernet_W5300Reg(offset);
}

static void BoardEthernet_W5300Write(BoardTest_U16 offset,
                                     BoardTest_U16 value)
{
    *BoardEthernet_W5300Reg(offset) = value;
}

static void BoardEthernet_PrepareW5300Access(void)
{
    BoardEmif_ConfigureExternalAsync();
    InitXintf16Gpio();
}

static void BoardEthernet_W5300ConfigureCommon(void)
{
    BoardTest_U16 index;

    for(index = 0U;
        index < BOARD_ETHERNET_ARRAY_LEN(BoardEthernet_W5300CommonConfig);
        index++)
    {
        BoardEthernet_W5300Write(
            BoardEthernet_W5300CommonConfig[index].offset,
            BoardEthernet_W5300CommonConfig[index].value);
    }
}

static void BoardEthernet_W5300ConfigureSocket0Tcp(void)
{
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_MR,
                             BOARD_ETHERNET_SOCKET0_MODE_TCP);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_PORT,
                             BOARD_ETHERNET_SOCKET0_PORT);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_TTL,
                             BOARD_ETHERNET_SOCKET0_TTL);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_MTU,
                             BOARD_ETHERNET_SOCKET0_MTU);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_KEEP,
                             BOARD_ETHERNET_SOCKET0_KEEPALIVE);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_IMR,
                             BOARD_ETHERNET_SOCKET0_RECV_INT);
}

static BoardTest_U16 BoardEthernet_W5300Socket0Status(void)
{
    BoardTest_U16 sr;
    BoardTest_U16 ir;

    sr = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_S0_SSR);
    ir = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_S0_IR);

    return (BoardTest_U16)(((sr & 0x00FFU) << 8U) | (ir & 0x00FFU));
}

static BoardTest_U16 BoardEthernet_W5300SocketState(BoardTest_U16 status)
{
    return (BoardTest_U16)((status >> 8U) & 0x00FFU);
}

static BoardTest_U16 BoardEthernet_W5300WaitSocketState(
    BoardTest_U16 expectedState)
{
    BoardTest_U32 timeout;
    BoardTest_U16 status;

    status = BoardEthernet_W5300Socket0Status();
    for(timeout = 0UL;
        timeout < BOARD_ETHERNET_SOCKET_POLL_LIMIT;
        timeout++)
    {
        status = BoardEthernet_W5300Socket0Status();
        if(BoardEthernet_W5300SocketState(status) == expectedState)
        {
            break;
        }
    }

    return status;
}

BoardTest_Result BoardEthernet_RunW5300BasicTest(BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 modeReg;
    BoardTest_U16 retryTimeReg;
    BoardTest_U16 retryCountReg;
    BoardTest_U16 txMemoryReg;
    BoardTest_U16 rxMemoryReg;
    BoardTest_U16 socket0Status;

    statusMask = BOARD_ETHERNET_W5300_BACKEND_EMIF;

    BoardEthernet_PrepareW5300Access();

    if(BoardEthernet_IsCurrentPinmapValid() != 0U)
    {
        statusMask |= BOARD_ETHERNET_W5300_PINMAP_VALID;
    }

    /*
     * ETH_RESET and ETH_INT are CPU2 pins on this board. This DSP1 test
     * only verifies EMIF access to an already released W5300.
     */
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_MR,
                             BOARD_ETHERNET_W5300_MODE_EXPECTED);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_RTR,
                             BOARD_ETHERNET_W5300_RTR_EXPECTED);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_RCR,
                             BOARD_ETHERNET_W5300_RCR_EXPECTED);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_TMSR0,
                             BOARD_ETHERNET_W5300_MEM_EXPECTED);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_RMSR0,
                             BOARD_ETHERNET_W5300_MEM_EXPECTED);

    modeReg = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_MR);
    retryTimeReg = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RTR);
    retryCountReg = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RCR);
    txMemoryReg = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_TMSR0);
    rxMemoryReg = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RMSR0);
    socket0Status = BoardEthernet_W5300Socket0Status();

    if(modeReg == BOARD_ETHERNET_W5300_MODE_EXPECTED)
    {
        statusMask |= BOARD_ETHERNET_W5300_MODE_READBACK;
    }

    if(retryTimeReg == BOARD_ETHERNET_W5300_RTR_EXPECTED)
    {
        statusMask |= BOARD_ETHERNET_W5300_RTR_READBACK;
    }

    if(retryCountReg == BOARD_ETHERNET_W5300_RCR_EXPECTED)
    {
        statusMask |= BOARD_ETHERNET_W5300_RCR_READBACK;
    }

    if((txMemoryReg == BOARD_ETHERNET_W5300_MEM_EXPECTED) &&
       (rxMemoryReg == BOARD_ETHERNET_W5300_MEM_EXPECTED))
    {
        statusMask |= BOARD_ETHERNET_W5300_MEMORY_READBACK;
    }

    if(socket0Status != 0xFFFFU)
    {
        statusMask |= BOARD_ETHERNET_W5300_SOCKET0_READABLE;
    }

    gBoardEthernetBasicSnapshot.backend =
        BOARD_ETHERNET_BACKEND_W5300_EMIF;
    gBoardEthernetBasicSnapshot.statusMask = statusMask;
    gBoardEthernetBasicSnapshot.modeReg = modeReg;
    gBoardEthernetBasicSnapshot.retryTimeReg = retryTimeReg;
    gBoardEthernetBasicSnapshot.retryCountReg = retryCountReg;
    gBoardEthernetBasicSnapshot.txMemoryReg = txMemoryReg;
    gBoardEthernetBasicSnapshot.rxMemoryReg = rxMemoryReg;
    gBoardEthernetBasicSnapshot.socket0Status = socket0Status;

    return BoardEthernet_EvaluateW5300BasicStatus(statusMask,
                                                 modeReg,
                                                 retryTimeReg,
                                                 retryCountReg,
                                                 txMemoryReg,
                                                 rxMemoryReg,
                                                 socket0Status,
                                                 record);
}

BoardTest_Result BoardEthernet_RunW5300SocketTest(BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 socketModeReg;
    BoardTest_U16 socketPortReg;
    BoardTest_U16 initStatus;
    BoardTest_U16 listenStatus;
    BoardTest_U16 closedStatus;

    statusMask = BOARD_ETHERNET_SOCKET_BACKEND_EMIF;
    initStatus = 0xFFFFU;
    listenStatus = 0xFFFFU;
    closedStatus = 0xFFFFU;

    BoardEthernet_PrepareW5300Access();
    BoardEthernet_W5300ConfigureCommon();

    if((BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_MR) ==
        BOARD_ETHERNET_W5300_MODE_EXPECTED) &&
       (BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RTR) ==
        BOARD_ETHERNET_W5300_RTR_EXPECTED) &&
       (BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RCR) ==
        BOARD_ETHERNET_W5300_RCR_EXPECTED))
    {
        statusMask |= BOARD_ETHERNET_SOCKET_COMMON_READY;
    }

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    (void)BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);

    BoardEthernet_W5300ConfigureSocket0Tcp();
    socketModeReg = BoardEthernet_W5300Read(
        BOARD_ETHERNET_W5300_REG_S0_MR);
    socketPortReg = BoardEthernet_W5300Read(
        BOARD_ETHERNET_W5300_REG_S0_PORT);

    if((socketModeReg == BOARD_ETHERNET_SOCKET0_MODE_TCP) &&
       (socketPortReg == BOARD_ETHERNET_SOCKET0_PORT))
    {
        statusMask |= BOARD_ETHERNET_SOCKET_CONFIG_READBACK;
    }

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_OPEN);
    initStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_INIT);
    if(BoardEthernet_W5300SocketState(initStatus) ==
       BOARD_ETHERNET_SOCKET_SR_INIT)
    {
        statusMask |= BOARD_ETHERNET_SOCKET_OPEN_INIT;
    }

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_LISTEN);
    listenStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_LISTEN);
    if(BoardEthernet_W5300SocketState(listenStatus) ==
       BOARD_ETHERNET_SOCKET_SR_LISTEN)
    {
        statusMask |= BOARD_ETHERNET_SOCKET_LISTEN;
    }

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    closedStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);
    if(BoardEthernet_W5300SocketState(closedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_CLOSED)
    {
        statusMask |= BOARD_ETHERNET_SOCKET_CLOSE_CLOSED;
    }

    gBoardEthernetSocketSnapshot.backend =
        BOARD_ETHERNET_BACKEND_W5300_EMIF;
    gBoardEthernetSocketSnapshot.statusMask = statusMask;
    gBoardEthernetSocketSnapshot.socketModeReg = socketModeReg;
    gBoardEthernetSocketSnapshot.socketPortReg = socketPortReg;
    gBoardEthernetSocketSnapshot.initStatus = initStatus;
    gBoardEthernetSocketSnapshot.listenStatus = listenStatus;
    gBoardEthernetSocketSnapshot.closedStatus = closedStatus;

    return BoardEthernet_EvaluateW5300SocketStatus(statusMask,
                                                  socketModeReg,
                                                  socketPortReg,
                                                  initStatus,
                                                  listenStatus,
                                                  closedStatus,
                                                  record);
}
#endif
