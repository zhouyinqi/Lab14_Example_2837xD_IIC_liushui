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
#define BOARD_ETHERNET_W5300_REG_S0_TX_WRSR 0x0220U
#define BOARD_ETHERNET_W5300_REG_S0_TX_FSR  0x0224U
#define BOARD_ETHERNET_W5300_REG_S0_RX_RSR  0x0228U
#define BOARD_ETHERNET_W5300_REG_S0_TX_FIFO 0x022EU
#define BOARD_ETHERNET_W5300_REG_S0_RX_FIFO 0x0230U
#define BOARD_ETHERNET_W5300_REG_S0_DPORT   0x0212U
#define BOARD_ETHERNET_W5300_REG_S0_DIPR0   0x0214U
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
#define BOARD_ETHERNET_SOCKET_CMD_SEND      0x0020U
#define BOARD_ETHERNET_SOCKET_CMD_RECV      0x0040U

#define BOARD_ETHERNET_SOCKET_IR_RECV       0x0004U
#define BOARD_ETHERNET_SOCKET_IR_SENDOK     0x0010U

#define BOARD_ETHERNET_SOCKET_SR_CLOSED     0x0000U
#define BOARD_ETHERNET_SOCKET_SR_INIT       0x0013U
#define BOARD_ETHERNET_SOCKET_SR_LISTEN     0x0014U
#define BOARD_ETHERNET_SOCKET_SR_ESTABLISHED 0x0017U
#define BOARD_ETHERNET_SOCKET_SR_CLOSE_WAIT 0x001CU
#define BOARD_ETHERNET_SOCKET_POLL_LIMIT    60000UL
#define BOARD_ETHERNET_TCP_LINK_STATE_IDLE  0x0000U
#define BOARD_ETHERNET_TCP_LINK_STATE_WAIT  0x0001U
#define BOARD_ETHERNET_TCP_ECHO_STATE_IDLE         0x0000U
#define BOARD_ETHERNET_TCP_ECHO_STATE_WAIT_CONNECT 0x0001U
#define BOARD_ETHERNET_TCP_ECHO_STATE_WAIT_RX      0x0002U
#define BOARD_ETHERNET_TCP_ECHO_STATE_WAIT_SENDOK  0x0003U
#define BOARD_ETHERNET_TCP_STABILITY_STATE_IDLE  0x0000U
#define BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_CONNECT 0x0001U
#define BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_RX 0x0002U
#define BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_SENDOK 0x0003U

#define BOARD_ETHERNET_TCP_ECHO_REQUEST_WORD0  0x4254U
#define BOARD_ETHERNET_TCP_ECHO_REQUEST_WORD1  0x5354U
#define BOARD_ETHERNET_TCP_ECHO_RESPONSE_WORD0 0x4254U
#define BOARD_ETHERNET_TCP_ECHO_RESPONSE_WORD1 0x4F4BU
#define BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES  4U
#define BOARD_ETHERNET_TCP_STABILITY_TARGET_COUNT 5U

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

volatile BoardEthernet_TcpLinkSnapshot gBoardEthernetTcpLinkSnapshot =
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

volatile BoardEthernet_TcpEchoSnapshot gBoardEthernetTcpEchoSnapshot =
{
    BOARD_ETHERNET_BACKEND_W5300_EMIF,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

volatile BoardEthernet_TcpStabilitySnapshot gBoardEthernetTcpStabilitySnapshot =
{
    BOARD_ETHERNET_BACKEND_W5300_EMIF,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    BOARD_ETHERNET_TCP_STABILITY_TARGET_COUNT
};

#ifndef BOARD_TEST_HOST
static BoardTest_U16 BoardEthernet_TcpLinkState =
    BOARD_ETHERNET_TCP_LINK_STATE_IDLE;
static BoardTest_U16 BoardEthernet_TcpEchoState =
    BOARD_ETHERNET_TCP_ECHO_STATE_IDLE;
static BoardTest_U16 BoardEthernet_TcpStabilityState =
    BOARD_ETHERNET_TCP_STABILITY_STATE_IDLE;
static BoardTest_U16 BoardEthernet_TcpStabilityPassCount = 0U;
#endif

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

BoardTest_Result BoardEthernet_EvaluateW5300TcpLinkStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 listenStatus,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 closedStatus,
    BoardTest_U16 remotePort,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)connectedStatus & 0xFF00UL) |
                       ((BoardTest_U32)remotePort & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_ETHERNET_TCP_LINK_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_ETHERNET_TCP_LINK_DIAGNOSTIC_MASK;

    (void)listenStatus;
    (void)closedStatus;

    if((statusMask & BOARD_ETHERNET_TCP_LINK_REQUIRED_MASK) ==
       BOARD_ETHERNET_TCP_LINK_REQUIRED_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ETHERNET_TCP_LINK;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardEthernet_EvaluateW5300TcpEchoStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 rxFrameLength,
    BoardTest_U16 txFreeLow,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)connectedStatus & 0xFF00UL) |
                       ((BoardTest_U32)rxFrameLength & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_ETHERNET_TCP_ECHO_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_ETHERNET_TCP_ECHO_DIAGNOSTIC_MASK;

    (void)txFreeLow;

    if((statusMask & BOARD_ETHERNET_TCP_ECHO_REQUIRED_MASK) ==
       BOARD_ETHERNET_TCP_ECHO_REQUIRED_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ETHERNET_TCP_ECHO;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardEthernet_EvaluateW5300TcpStabilityStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 passCount,
    BoardTest_U16 targetCount,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)connectedStatus & 0xFF00UL) |
                       ((BoardTest_U32)passCount & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin =
        (float)BOARD_ETHERNET_TCP_STABILITY_REQUIRED_MASK;
    record->expectedMax =
        (float)BOARD_ETHERNET_TCP_STABILITY_DIAGNOSTIC_MASK;

    if(((statusMask & BOARD_ETHERNET_TCP_STABILITY_REQUIRED_MASK) ==
        BOARD_ETHERNET_TCP_STABILITY_REQUIRED_MASK) &&
       (passCount >= targetCount))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_ETHERNET_TCP_STABILITY;
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

static BoardTest_U32 BoardEthernet_W5300SocketByteCount(BoardTest_U16 reg)
{
    BoardTest_U16 high;
    BoardTest_U16 low;

    high = BoardEthernet_W5300Read(reg);
    low = BoardEthernet_W5300Read((BoardTest_U16)(reg + 0x0002U));

    return ((((BoardTest_U32)high << 8U) & 0x10000UL) |
            ((BoardTest_U32)low & 0xFFFFUL));
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

static void BoardEthernet_UpdateTcpLinkSnapshot(BoardTest_U16 statusMask,
                                                BoardTest_U16 listenStatus,
                                                BoardTest_U16 connectedStatus,
                                                BoardTest_U16 closedStatus,
                                                BoardTest_U16 remoteIpHigh,
                                                BoardTest_U16 remoteIpLow,
                                                BoardTest_U16 remotePort)
{
    gBoardEthernetTcpLinkSnapshot.backend =
        BOARD_ETHERNET_BACKEND_W5300_EMIF;
    gBoardEthernetTcpLinkSnapshot.statusMask = statusMask;
    gBoardEthernetTcpLinkSnapshot.listenStatus = listenStatus;
    gBoardEthernetTcpLinkSnapshot.connectedStatus = connectedStatus;
    gBoardEthernetTcpLinkSnapshot.closedStatus = closedStatus;
    gBoardEthernetTcpLinkSnapshot.remoteIpHigh = remoteIpHigh;
    gBoardEthernetTcpLinkSnapshot.remoteIpLow = remoteIpLow;
    gBoardEthernetTcpLinkSnapshot.remotePort = remotePort;
}

static void BoardEthernet_UpdateTcpLinkRunningRecord(
    BoardTest_U16 statusMask,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 remotePort,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)connectedStatus & 0xFF00UL) |
                       ((BoardTest_U32)remotePort & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_ETHERNET_TCP_LINK_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_ETHERNET_TCP_LINK_DIAGNOSTIC_MASK;
    record->errorCode = BOARD_TEST_ERROR_NONE;
}

static BoardTest_Result BoardEthernet_StartW5300TcpLinkTest(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 listenStatus;

    statusMask = BOARD_ETHERNET_TCP_LINK_BACKEND_EMIF;
    BoardEthernet_PrepareW5300Access();
    BoardEthernet_W5300ConfigureCommon();

    if((BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_MR) ==
        BOARD_ETHERNET_W5300_MODE_EXPECTED) &&
       (BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RTR) ==
        BOARD_ETHERNET_W5300_RTR_EXPECTED) &&
       (BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RCR) ==
        BOARD_ETHERNET_W5300_RCR_EXPECTED))
    {
        statusMask |= BOARD_ETHERNET_TCP_LINK_COMMON_READY;
    }

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    (void)BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);

    BoardEthernet_W5300ConfigureSocket0Tcp();
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_OPEN);
    (void)BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_INIT);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_LISTEN);
    listenStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_LISTEN);
    if(BoardEthernet_W5300SocketState(listenStatus) ==
       BOARD_ETHERNET_SOCKET_SR_LISTEN)
    {
        statusMask |= BOARD_ETHERNET_TCP_LINK_LISTEN;
    }

    BoardEthernet_UpdateTcpLinkSnapshot(statusMask,
                                        listenStatus,
                                        listenStatus,
                                        0xFFFFU,
                                        0U,
                                        0U,
                                        0U);
    BoardEthernet_UpdateTcpLinkRunningRecord(statusMask,
                                             listenStatus,
                                             0U,
                                             record);

    if((statusMask & (BOARD_ETHERNET_TCP_LINK_COMMON_READY |
                      BOARD_ETHERNET_TCP_LINK_LISTEN)) !=
       (BOARD_ETHERNET_TCP_LINK_COMMON_READY |
        BOARD_ETHERNET_TCP_LINK_LISTEN))
    {
        BoardEthernet_TcpLinkState = BOARD_ETHERNET_TCP_LINK_STATE_IDLE;
        record->errorCode = BOARD_TEST_ERROR_ETHERNET_TCP_LINK;
        return BOARD_TEST_RESULT_FAIL;
    }

    BoardEthernet_TcpLinkState = BOARD_ETHERNET_TCP_LINK_STATE_WAIT;
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardEthernet_PollW5300TcpLinkTest(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 connectedStatus;
    BoardTest_U16 closedStatus;
    BoardTest_U16 remoteIpHigh;
    BoardTest_U16 remoteIpLow;
    BoardTest_U16 remotePort;

    statusMask = gBoardEthernetTcpLinkSnapshot.statusMask;
    connectedStatus = BoardEthernet_W5300Socket0Status();
    closedStatus = 0xFFFFU;
    remoteIpHigh = gBoardEthernetTcpLinkSnapshot.remoteIpHigh;
    remoteIpLow = gBoardEthernetTcpLinkSnapshot.remoteIpLow;
    remotePort = gBoardEthernetTcpLinkSnapshot.remotePort;

    if(BoardEthernet_W5300SocketState(connectedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_ESTABLISHED)
    {
        statusMask |= BOARD_ETHERNET_TCP_LINK_ESTABLISHED;
        remotePort = BoardEthernet_W5300Read(
            BOARD_ETHERNET_W5300_REG_S0_DPORT);
        remoteIpHigh = BoardEthernet_W5300Read(
            BOARD_ETHERNET_W5300_REG_S0_DIPR0);
        remoteIpLow = BoardEthernet_W5300Read(
            (BoardTest_U16)(BOARD_ETHERNET_W5300_REG_S0_DIPR0 + 0x0002U));
        if((remotePort != 0U) || (remoteIpHigh != 0U) || (remoteIpLow != 0U))
        {
            statusMask |= BOARD_ETHERNET_TCP_LINK_PEER_CAPTURED;
        }

        BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                                 BOARD_ETHERNET_SOCKET_CMD_CLOSE);
        closedStatus = BoardEthernet_W5300WaitSocketState(
            BOARD_ETHERNET_SOCKET_SR_CLOSED);
        if(BoardEthernet_W5300SocketState(closedStatus) ==
           BOARD_ETHERNET_SOCKET_SR_CLOSED)
        {
            statusMask |= BOARD_ETHERNET_TCP_LINK_CLOSE_CLOSED;
        }

        BoardEthernet_UpdateTcpLinkSnapshot(
            statusMask,
            gBoardEthernetTcpLinkSnapshot.listenStatus,
            connectedStatus,
            closedStatus,
            remoteIpHigh,
            remoteIpLow,
            remotePort);
        BoardEthernet_TcpLinkState = BOARD_ETHERNET_TCP_LINK_STATE_IDLE;

        return BoardEthernet_EvaluateW5300TcpLinkStatus(
            statusMask,
            gBoardEthernetTcpLinkSnapshot.listenStatus,
            connectedStatus,
            closedStatus,
            remotePort,
            record);
    }

    if(BoardEthernet_W5300SocketState(connectedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_CLOSED)
    {
        BoardEthernet_TcpLinkState = BOARD_ETHERNET_TCP_LINK_STATE_IDLE;
        statusMask &= (BoardTest_U16)~BOARD_ETHERNET_TCP_LINK_LISTEN;
        BoardEthernet_UpdateTcpLinkSnapshot(
            statusMask,
            gBoardEthernetTcpLinkSnapshot.listenStatus,
            connectedStatus,
            closedStatus,
            remoteIpHigh,
            remoteIpLow,
            remotePort);
        record->errorCode = BOARD_TEST_ERROR_ETHERNET_TCP_LINK;
        return BOARD_TEST_RESULT_FAIL;
    }

    BoardEthernet_UpdateTcpLinkSnapshot(
        statusMask,
        gBoardEthernetTcpLinkSnapshot.listenStatus,
        connectedStatus,
        closedStatus,
        remoteIpHigh,
        remoteIpLow,
        remotePort);
    BoardEthernet_UpdateTcpLinkRunningRecord(statusMask,
                                             connectedStatus,
                                             remotePort,
                                             record);
    return BOARD_TEST_RESULT_RUNNING;
}

BoardTest_Result BoardEthernet_RunW5300TcpLinkTest(BoardTest_Record *record)
{
    if(BoardEthernet_TcpLinkState == BOARD_ETHERNET_TCP_LINK_STATE_IDLE)
    {
        return BoardEthernet_StartW5300TcpLinkTest(record);
    }

    return BoardEthernet_PollW5300TcpLinkTest(record);
}

void BoardEthernet_AbortW5300TcpLinkTest(void)
{
    if(BoardEthernet_TcpLinkState != BOARD_ETHERNET_TCP_LINK_STATE_IDLE)
    {
        BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                                 BOARD_ETHERNET_SOCKET_CMD_CLOSE);
        BoardEthernet_TcpLinkState = BOARD_ETHERNET_TCP_LINK_STATE_IDLE;
    }
}

static void BoardEthernet_UpdateTcpEchoSnapshot(BoardTest_U16 statusMask,
                                                BoardTest_U16 listenStatus,
                                                BoardTest_U16 connectedStatus,
                                                BoardTest_U16 rxSize,
                                                BoardTest_U16 rxFrameLength,
                                                BoardTest_U16 rxWord0,
                                                BoardTest_U16 rxWord1,
                                                BoardTest_U16 txFreeLow,
                                                BoardTest_U16 sendStatus,
                                                BoardTest_U16 closedStatus)
{
    gBoardEthernetTcpEchoSnapshot.backend =
        BOARD_ETHERNET_BACKEND_W5300_EMIF;
    gBoardEthernetTcpEchoSnapshot.statusMask = statusMask;
    gBoardEthernetTcpEchoSnapshot.listenStatus = listenStatus;
    gBoardEthernetTcpEchoSnapshot.connectedStatus = connectedStatus;
    gBoardEthernetTcpEchoSnapshot.rxSize = rxSize;
    gBoardEthernetTcpEchoSnapshot.rxFrameLength = rxFrameLength;
    gBoardEthernetTcpEchoSnapshot.rxWord0 = rxWord0;
    gBoardEthernetTcpEchoSnapshot.rxWord1 = rxWord1;
    gBoardEthernetTcpEchoSnapshot.txFreeLow = txFreeLow;
    gBoardEthernetTcpEchoSnapshot.sendStatus = sendStatus;
    gBoardEthernetTcpEchoSnapshot.closedStatus = closedStatus;
}

static void BoardEthernet_UpdateTcpEchoRunningRecord(
    BoardTest_U16 statusMask,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 rxFrameLength,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)connectedStatus & 0xFF00UL) |
                       ((BoardTest_U32)rxFrameLength & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_ETHERNET_TCP_ECHO_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_ETHERNET_TCP_ECHO_DIAGNOSTIC_MASK;
    record->errorCode = BOARD_TEST_ERROR_NONE;
}

static void BoardEthernet_W5300ReadTcpEchoPayload(
    BoardTest_U16 frameLength,
    BoardTest_U16 *rxWord0,
    BoardTest_U16 *rxWord1)
{
    BoardTest_U16 wordCount;
    BoardTest_U16 index;
    BoardTest_U16 word;

    wordCount = (BoardTest_U16)((frameLength + 1U) >> 1U);
    *rxWord0 = 0U;
    *rxWord1 = 0U;

    for(index = 0U; index < wordCount; index++)
    {
        word = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_S0_RX_FIFO);
        if(index == 0U)
        {
            *rxWord0 = word;
        }
        else if(index == 1U)
        {
            *rxWord1 = word;
        }
    }
}

static BoardTest_Result BoardEthernet_FailW5300TcpEchoTest(
    BoardTest_U16 statusMask,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 rxSize,
    BoardTest_U16 rxFrameLength,
    BoardTest_U16 rxWord0,
    BoardTest_U16 rxWord1,
    BoardTest_U16 txFreeLow,
    BoardTest_U16 sendStatus,
    BoardTest_Record *record)
{
    BoardTest_U16 closedStatus;

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    closedStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);
    if(BoardEthernet_W5300SocketState(closedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_CLOSED)
    {
        statusMask |= BOARD_ETHERNET_TCP_ECHO_CLOSE_CLOSED;
    }

    BoardEthernet_TcpEchoState = BOARD_ETHERNET_TCP_ECHO_STATE_IDLE;
    BoardEthernet_UpdateTcpEchoSnapshot(
        statusMask,
        gBoardEthernetTcpEchoSnapshot.listenStatus,
        connectedStatus,
        rxSize,
        rxFrameLength,
        rxWord0,
        rxWord1,
        txFreeLow,
        sendStatus,
        closedStatus);

    return BoardEthernet_EvaluateW5300TcpEchoStatus(statusMask,
                                                   connectedStatus,
                                                   rxFrameLength,
                                                   txFreeLow,
                                                   record);
}

static BoardTest_Result BoardEthernet_StartW5300TcpEchoTest(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 listenStatus;

    statusMask = BOARD_ETHERNET_TCP_ECHO_BACKEND_EMIF;
    BoardEthernet_PrepareW5300Access();
    BoardEthernet_W5300ConfigureCommon();

    if((BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_MR) ==
        BOARD_ETHERNET_W5300_MODE_EXPECTED) &&
       (BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RTR) ==
        BOARD_ETHERNET_W5300_RTR_EXPECTED) &&
       (BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RCR) ==
        BOARD_ETHERNET_W5300_RCR_EXPECTED))
    {
        statusMask |= BOARD_ETHERNET_TCP_ECHO_COMMON_READY;
    }

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    (void)BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);

    BoardEthernet_W5300ConfigureSocket0Tcp();
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_IR,
                             BOARD_ETHERNET_SOCKET_IR_RECV |
                             BOARD_ETHERNET_SOCKET_IR_SENDOK);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_OPEN);
    (void)BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_INIT);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_LISTEN);
    listenStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_LISTEN);
    if(BoardEthernet_W5300SocketState(listenStatus) ==
       BOARD_ETHERNET_SOCKET_SR_LISTEN)
    {
        statusMask |= BOARD_ETHERNET_TCP_ECHO_LISTEN;
    }

    BoardEthernet_UpdateTcpEchoSnapshot(statusMask,
                                        listenStatus,
                                        listenStatus,
                                        0U,
                                        0U,
                                        0U,
                                        0U,
                                        0U,
                                        0U,
                                        0xFFFFU);
    BoardEthernet_UpdateTcpEchoRunningRecord(statusMask,
                                             listenStatus,
                                             0U,
                                             record);

    if((statusMask & (BOARD_ETHERNET_TCP_ECHO_COMMON_READY |
                      BOARD_ETHERNET_TCP_ECHO_LISTEN)) !=
       (BOARD_ETHERNET_TCP_ECHO_COMMON_READY |
        BOARD_ETHERNET_TCP_ECHO_LISTEN))
    {
        BoardEthernet_TcpEchoState = BOARD_ETHERNET_TCP_ECHO_STATE_IDLE;
        record->errorCode = BOARD_TEST_ERROR_ETHERNET_TCP_ECHO;
        return BOARD_TEST_RESULT_FAIL;
    }

    BoardEthernet_TcpEchoState =
        BOARD_ETHERNET_TCP_ECHO_STATE_WAIT_CONNECT;
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardEthernet_PollW5300TcpEchoConnect(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 connectedStatus;

    statusMask = gBoardEthernetTcpEchoSnapshot.statusMask;
    connectedStatus = BoardEthernet_W5300Socket0Status();

    if(BoardEthernet_W5300SocketState(connectedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_ESTABLISHED)
    {
        statusMask |= BOARD_ETHERNET_TCP_ECHO_ESTABLISHED;
        BoardEthernet_TcpEchoState = BOARD_ETHERNET_TCP_ECHO_STATE_WAIT_RX;
    }
    else if(BoardEthernet_W5300SocketState(connectedStatus) ==
            BOARD_ETHERNET_SOCKET_SR_CLOSED)
    {
        statusMask &= (BoardTest_U16)~BOARD_ETHERNET_TCP_ECHO_LISTEN;
        return BoardEthernet_FailW5300TcpEchoTest(statusMask,
                                                  connectedStatus,
                                                  0U,
                                                  0U,
                                                  0U,
                                                  0U,
                                                  0U,
                                                  0U,
                                                  record);
    }

    BoardEthernet_UpdateTcpEchoSnapshot(
        statusMask,
        gBoardEthernetTcpEchoSnapshot.listenStatus,
        connectedStatus,
        0U,
        0U,
        0U,
        0U,
        0U,
        connectedStatus,
        0xFFFFU);
    BoardEthernet_UpdateTcpEchoRunningRecord(statusMask,
                                             connectedStatus,
                                             0U,
                                             record);
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardEthernet_PollW5300TcpEchoRx(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 connectedStatus;
    BoardTest_U16 rxSize;
    BoardTest_U16 rxFrameLength;
    BoardTest_U16 rxWord0;
    BoardTest_U16 rxWord1;
    BoardTest_U16 txFreeLow;
    BoardTest_U32 rxByteCount;
    BoardTest_U32 txFree;

    statusMask = gBoardEthernetTcpEchoSnapshot.statusMask;
    connectedStatus = BoardEthernet_W5300Socket0Status();
    rxSize = gBoardEthernetTcpEchoSnapshot.rxSize;
    rxFrameLength = gBoardEthernetTcpEchoSnapshot.rxFrameLength;
    rxWord0 = gBoardEthernetTcpEchoSnapshot.rxWord0;
    rxWord1 = gBoardEthernetTcpEchoSnapshot.rxWord1;
    txFreeLow = gBoardEthernetTcpEchoSnapshot.txFreeLow;

    if(BoardEthernet_W5300SocketState(connectedStatus) !=
       BOARD_ETHERNET_SOCKET_SR_ESTABLISHED)
    {
        return BoardEthernet_FailW5300TcpEchoTest(statusMask,
                                                  connectedStatus,
                                                  rxSize,
                                                  rxFrameLength,
                                                  rxWord0,
                                                  rxWord1,
                                                  txFreeLow,
                                                  connectedStatus,
                                                  record);
    }

    rxByteCount = BoardEthernet_W5300SocketByteCount(
        BOARD_ETHERNET_W5300_REG_S0_RX_RSR);
    rxSize = (BoardTest_U16)(rxByteCount & 0xFFFFUL);
    if(rxByteCount < ((BoardTest_U32)BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES +
                      2UL))
    {
        BoardEthernet_UpdateTcpEchoSnapshot(
            statusMask,
            gBoardEthernetTcpEchoSnapshot.listenStatus,
            connectedStatus,
            rxSize,
            rxFrameLength,
            rxWord0,
            rxWord1,
            txFreeLow,
            connectedStatus,
            0xFFFFU);
        BoardEthernet_UpdateTcpEchoRunningRecord(statusMask,
                                                 connectedStatus,
                                                 rxFrameLength,
                                                 record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    statusMask |= BOARD_ETHERNET_TCP_ECHO_RX_AVAILABLE;
    rxFrameLength = BoardEthernet_W5300Read(
        BOARD_ETHERNET_W5300_REG_S0_RX_FIFO);
    if(rxFrameLength > 64U)
    {
        return BoardEthernet_FailW5300TcpEchoTest(statusMask,
                                                  connectedStatus,
                                                  rxSize,
                                                  rxFrameLength,
                                                  rxWord0,
                                                  rxWord1,
                                                  txFreeLow,
                                                  connectedStatus,
                                                  record);
    }

    BoardEthernet_W5300ReadTcpEchoPayload(rxFrameLength,
                                          &rxWord0,
                                          &rxWord1);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_IR,
                             BOARD_ETHERNET_SOCKET_IR_RECV);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_RECV);

    if((rxFrameLength == BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES) &&
       (rxWord0 == BOARD_ETHERNET_TCP_ECHO_REQUEST_WORD0) &&
       (rxWord1 == BOARD_ETHERNET_TCP_ECHO_REQUEST_WORD1))
    {
        statusMask |= BOARD_ETHERNET_TCP_ECHO_RX_MATCH;
    }
    else
    {
        return BoardEthernet_FailW5300TcpEchoTest(statusMask,
                                                  connectedStatus,
                                                  rxSize,
                                                  rxFrameLength,
                                                  rxWord0,
                                                  rxWord1,
                                                  txFreeLow,
                                                  connectedStatus,
                                                  record);
    }

    txFree = BoardEthernet_W5300SocketByteCount(
        BOARD_ETHERNET_W5300_REG_S0_TX_FSR);
    txFreeLow = (BoardTest_U16)(txFree & 0xFFFFUL);
    if(txFree < BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES)
    {
        return BoardEthernet_FailW5300TcpEchoTest(statusMask,
                                                  connectedStatus,
                                                  rxSize,
                                                  rxFrameLength,
                                                  rxWord0,
                                                  rxWord1,
                                                  txFreeLow,
                                                  connectedStatus,
                                                  record);
    }

    statusMask |= BOARD_ETHERNET_TCP_ECHO_TX_FREE;
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_TX_FIFO,
                             BOARD_ETHERNET_TCP_ECHO_RESPONSE_WORD0);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_TX_FIFO,
                             BOARD_ETHERNET_TCP_ECHO_RESPONSE_WORD1);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_TX_WRSR, 0U);
    BoardEthernet_W5300Write(
        (BoardTest_U16)(BOARD_ETHERNET_W5300_REG_S0_TX_WRSR + 0x0002U),
        BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_SEND);

    BoardEthernet_TcpEchoState =
        BOARD_ETHERNET_TCP_ECHO_STATE_WAIT_SENDOK;
    BoardEthernet_UpdateTcpEchoSnapshot(
        statusMask,
        gBoardEthernetTcpEchoSnapshot.listenStatus,
        connectedStatus,
        rxSize,
        rxFrameLength,
        rxWord0,
        rxWord1,
        txFreeLow,
        connectedStatus,
        0xFFFFU);
    BoardEthernet_UpdateTcpEchoRunningRecord(statusMask,
                                             connectedStatus,
                                             rxFrameLength,
                                             record);
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardEthernet_PollW5300TcpEchoSendOk(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 connectedStatus;
    BoardTest_U16 closedStatus;
    BoardTest_U16 socketState;

    statusMask = gBoardEthernetTcpEchoSnapshot.statusMask;
    connectedStatus = BoardEthernet_W5300Socket0Status();
    socketState = BoardEthernet_W5300SocketState(connectedStatus);

    if(((connectedStatus & BOARD_ETHERNET_SOCKET_IR_SENDOK) == 0U) &&
       (socketState == BOARD_ETHERNET_SOCKET_SR_ESTABLISHED))
    {
        BoardEthernet_UpdateTcpEchoSnapshot(
            statusMask,
            gBoardEthernetTcpEchoSnapshot.listenStatus,
            connectedStatus,
            gBoardEthernetTcpEchoSnapshot.rxSize,
            gBoardEthernetTcpEchoSnapshot.rxFrameLength,
            gBoardEthernetTcpEchoSnapshot.rxWord0,
            gBoardEthernetTcpEchoSnapshot.rxWord1,
            gBoardEthernetTcpEchoSnapshot.txFreeLow,
            connectedStatus,
            0xFFFFU);
        BoardEthernet_UpdateTcpEchoRunningRecord(
            statusMask,
            connectedStatus,
            gBoardEthernetTcpEchoSnapshot.rxFrameLength,
            record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    if(((connectedStatus & BOARD_ETHERNET_SOCKET_IR_SENDOK) == 0U) &&
       (socketState != BOARD_ETHERNET_SOCKET_SR_CLOSE_WAIT))
    {
        return BoardEthernet_FailW5300TcpEchoTest(
            statusMask,
            connectedStatus,
            gBoardEthernetTcpEchoSnapshot.rxSize,
            gBoardEthernetTcpEchoSnapshot.rxFrameLength,
            gBoardEthernetTcpEchoSnapshot.rxWord0,
            gBoardEthernetTcpEchoSnapshot.rxWord1,
            gBoardEthernetTcpEchoSnapshot.txFreeLow,
            connectedStatus,
            record);
    }

    /* The PC test closes immediately after receiving BTOK, so CLOSE_WAIT is
     * accepted as peer-side confirmation of the echo response. */
    statusMask |= BOARD_ETHERNET_TCP_ECHO_TX_SENT;
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_IR,
                             BOARD_ETHERNET_SOCKET_IR_SENDOK);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    closedStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);
    if(BoardEthernet_W5300SocketState(closedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_CLOSED)
    {
        statusMask |= BOARD_ETHERNET_TCP_ECHO_CLOSE_CLOSED;
    }

    BoardEthernet_UpdateTcpEchoSnapshot(
        statusMask,
        gBoardEthernetTcpEchoSnapshot.listenStatus,
        connectedStatus,
        gBoardEthernetTcpEchoSnapshot.rxSize,
        gBoardEthernetTcpEchoSnapshot.rxFrameLength,
        gBoardEthernetTcpEchoSnapshot.rxWord0,
        gBoardEthernetTcpEchoSnapshot.rxWord1,
        gBoardEthernetTcpEchoSnapshot.txFreeLow,
        connectedStatus,
        closedStatus);
    BoardEthernet_TcpEchoState = BOARD_ETHERNET_TCP_ECHO_STATE_IDLE;

    return BoardEthernet_EvaluateW5300TcpEchoStatus(
        statusMask,
        connectedStatus,
        gBoardEthernetTcpEchoSnapshot.rxFrameLength,
        gBoardEthernetTcpEchoSnapshot.txFreeLow,
        record);
}

BoardTest_Result BoardEthernet_RunW5300TcpEchoTest(BoardTest_Record *record)
{
    if(BoardEthernet_TcpEchoState == BOARD_ETHERNET_TCP_ECHO_STATE_IDLE)
    {
        return BoardEthernet_StartW5300TcpEchoTest(record);
    }

    if(BoardEthernet_TcpEchoState ==
       BOARD_ETHERNET_TCP_ECHO_STATE_WAIT_CONNECT)
    {
        return BoardEthernet_PollW5300TcpEchoConnect(record);
    }

    if(BoardEthernet_TcpEchoState == BOARD_ETHERNET_TCP_ECHO_STATE_WAIT_RX)
    {
        return BoardEthernet_PollW5300TcpEchoRx(record);
    }

    return BoardEthernet_PollW5300TcpEchoSendOk(record);
}

void BoardEthernet_AbortW5300TcpEchoTest(void)
{
    if(BoardEthernet_TcpEchoState != BOARD_ETHERNET_TCP_ECHO_STATE_IDLE)
    {
        BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                                 BOARD_ETHERNET_SOCKET_CMD_CLOSE);
        BoardEthernet_TcpEchoState = BOARD_ETHERNET_TCP_ECHO_STATE_IDLE;
    }
}

static void BoardEthernet_UpdateTcpStabilitySnapshot(
    BoardTest_U16 statusMask,
    BoardTest_U16 listenStatus,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 rxSize,
    BoardTest_U16 rxFrameLength,
    BoardTest_U16 rxWord0,
    BoardTest_U16 rxWord1,
    BoardTest_U16 txFreeLow,
    BoardTest_U16 sendStatus,
    BoardTest_U16 closedStatus,
    BoardTest_U16 passCount)
{
    gBoardEthernetTcpStabilitySnapshot.backend =
        BOARD_ETHERNET_BACKEND_W5300_EMIF;
    gBoardEthernetTcpStabilitySnapshot.statusMask = statusMask;
    gBoardEthernetTcpStabilitySnapshot.listenStatus = listenStatus;
    gBoardEthernetTcpStabilitySnapshot.connectedStatus = connectedStatus;
    gBoardEthernetTcpStabilitySnapshot.rxSize = rxSize;
    gBoardEthernetTcpStabilitySnapshot.rxFrameLength = rxFrameLength;
    gBoardEthernetTcpStabilitySnapshot.rxWord0 = rxWord0;
    gBoardEthernetTcpStabilitySnapshot.rxWord1 = rxWord1;
    gBoardEthernetTcpStabilitySnapshot.txFreeLow = txFreeLow;
    gBoardEthernetTcpStabilitySnapshot.sendStatus = sendStatus;
    gBoardEthernetTcpStabilitySnapshot.closedStatus = closedStatus;
    gBoardEthernetTcpStabilitySnapshot.passCount = passCount;
    gBoardEthernetTcpStabilitySnapshot.targetCount =
        BOARD_ETHERNET_TCP_STABILITY_TARGET_COUNT;
}

static void BoardEthernet_UpdateTcpStabilityRunningRecord(
    BoardTest_U16 statusMask,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 passCount,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)connectedStatus & 0xFF00UL) |
                       ((BoardTest_U32)passCount & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin =
        (float)BOARD_ETHERNET_TCP_STABILITY_REQUIRED_MASK;
    record->expectedMax =
        (float)BOARD_ETHERNET_TCP_STABILITY_DIAGNOSTIC_MASK;
    record->errorCode = BOARD_TEST_ERROR_NONE;
}

static BoardTest_Result BoardEthernet_FailW5300TcpStabilityTest(
    BoardTest_U16 statusMask,
    BoardTest_U16 connectedStatus,
    BoardTest_U16 rxSize,
    BoardTest_U16 rxFrameLength,
    BoardTest_U16 rxWord0,
    BoardTest_U16 rxWord1,
    BoardTest_U16 txFreeLow,
    BoardTest_U16 sendStatus,
    BoardTest_Record *record)
{
    BoardTest_U16 closedStatus;

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    closedStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);
    if(BoardEthernet_W5300SocketState(closedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_CLOSED)
    {
        statusMask |= BOARD_ETHERNET_TCP_STABILITY_CLOSE_CLOSED;
    }

    BoardEthernet_TcpStabilityState =
        BOARD_ETHERNET_TCP_STABILITY_STATE_IDLE;
    BoardEthernet_UpdateTcpStabilitySnapshot(
        statusMask,
        gBoardEthernetTcpStabilitySnapshot.listenStatus,
        connectedStatus,
        rxSize,
        rxFrameLength,
        rxWord0,
        rxWord1,
        txFreeLow,
        sendStatus,
        closedStatus,
        BoardEthernet_TcpStabilityPassCount);

    return BoardEthernet_EvaluateW5300TcpStabilityStatus(
        statusMask,
        connectedStatus,
        BoardEthernet_TcpStabilityPassCount,
        BOARD_ETHERNET_TCP_STABILITY_TARGET_COUNT,
        record);
}

static BoardTest_Result BoardEthernet_StartW5300TcpStabilityTest(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 listenStatus;

    statusMask = BOARD_ETHERNET_TCP_STABILITY_BACKEND_EMIF;
    BoardEthernet_TcpStabilityPassCount = 0U;
    BoardEthernet_PrepareW5300Access();
    BoardEthernet_W5300ConfigureCommon();

    if((BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_MR) ==
        BOARD_ETHERNET_W5300_MODE_EXPECTED) &&
       (BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RTR) ==
        BOARD_ETHERNET_W5300_RTR_EXPECTED) &&
       (BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_RCR) ==
        BOARD_ETHERNET_W5300_RCR_EXPECTED))
    {
        statusMask |= BOARD_ETHERNET_TCP_STABILITY_COMMON_READY;
    }

    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    (void)BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);

    BoardEthernet_W5300ConfigureSocket0Tcp();
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_IR,
                             BOARD_ETHERNET_SOCKET_IR_RECV |
                             BOARD_ETHERNET_SOCKET_IR_SENDOK);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_OPEN);
    (void)BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_INIT);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_LISTEN);
    listenStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_LISTEN);
    if(BoardEthernet_W5300SocketState(listenStatus) ==
       BOARD_ETHERNET_SOCKET_SR_LISTEN)
    {
        statusMask |= BOARD_ETHERNET_TCP_STABILITY_LISTEN;
    }

    BoardEthernet_UpdateTcpStabilitySnapshot(statusMask,
                                             listenStatus,
                                             listenStatus,
                                             0U,
                                             0U,
                                             0U,
                                             0U,
                                             0U,
                                             0U,
                                             0xFFFFU,
                                             0U);
    BoardEthernet_UpdateTcpStabilityRunningRecord(statusMask,
                                                  listenStatus,
                                                  0U,
                                                  record);

    if((statusMask & (BOARD_ETHERNET_TCP_STABILITY_COMMON_READY |
                      BOARD_ETHERNET_TCP_STABILITY_LISTEN)) !=
       (BOARD_ETHERNET_TCP_STABILITY_COMMON_READY |
        BOARD_ETHERNET_TCP_STABILITY_LISTEN))
    {
        BoardEthernet_TcpStabilityState =
            BOARD_ETHERNET_TCP_STABILITY_STATE_IDLE;
        record->errorCode = BOARD_TEST_ERROR_ETHERNET_TCP_STABILITY;
        return BOARD_TEST_RESULT_FAIL;
    }

    BoardEthernet_TcpStabilityState =
        BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_CONNECT;
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardEthernet_PollW5300TcpStabilityConnect(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 connectedStatus;

    statusMask = gBoardEthernetTcpStabilitySnapshot.statusMask;
    connectedStatus = BoardEthernet_W5300Socket0Status();

    if(BoardEthernet_W5300SocketState(connectedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_ESTABLISHED)
    {
        statusMask |= BOARD_ETHERNET_TCP_STABILITY_ESTABLISHED;
        BoardEthernet_TcpStabilityState =
            BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_RX;
    }
    else if(BoardEthernet_W5300SocketState(connectedStatus) ==
            BOARD_ETHERNET_SOCKET_SR_CLOSED)
    {
        statusMask &= (BoardTest_U16)~BOARD_ETHERNET_TCP_STABILITY_LISTEN;
        return BoardEthernet_FailW5300TcpStabilityTest(statusMask,
                                                       connectedStatus,
                                                       0U,
                                                       0U,
                                                       0U,
                                                       0U,
                                                       0U,
                                                       connectedStatus,
                                                       record);
    }

    BoardEthernet_UpdateTcpStabilitySnapshot(
        statusMask,
        gBoardEthernetTcpStabilitySnapshot.listenStatus,
        connectedStatus,
        0U,
        0U,
        0U,
        0U,
        0U,
        connectedStatus,
        0xFFFFU,
        BoardEthernet_TcpStabilityPassCount);
    BoardEthernet_UpdateTcpStabilityRunningRecord(
        statusMask,
        connectedStatus,
        BoardEthernet_TcpStabilityPassCount,
        record);
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardEthernet_PollW5300TcpStabilityRx(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 connectedStatus;
    BoardTest_U16 rxSize;
    BoardTest_U16 rxFrameLength;
    BoardTest_U16 rxWord0;
    BoardTest_U16 rxWord1;
    BoardTest_U16 txFreeLow;
    BoardTest_U32 rxByteCount;
    BoardTest_U32 txFree;

    statusMask = gBoardEthernetTcpStabilitySnapshot.statusMask;
    connectedStatus = BoardEthernet_W5300Socket0Status();
    rxSize = gBoardEthernetTcpStabilitySnapshot.rxSize;
    rxFrameLength = gBoardEthernetTcpStabilitySnapshot.rxFrameLength;
    rxWord0 = gBoardEthernetTcpStabilitySnapshot.rxWord0;
    rxWord1 = gBoardEthernetTcpStabilitySnapshot.rxWord1;
    txFreeLow = gBoardEthernetTcpStabilitySnapshot.txFreeLow;

    if(BoardEthernet_W5300SocketState(connectedStatus) !=
       BOARD_ETHERNET_SOCKET_SR_ESTABLISHED)
    {
        return BoardEthernet_FailW5300TcpStabilityTest(statusMask,
                                                       connectedStatus,
                                                       rxSize,
                                                       rxFrameLength,
                                                       rxWord0,
                                                       rxWord1,
                                                       txFreeLow,
                                                       connectedStatus,
                                                       record);
    }

    rxByteCount = BoardEthernet_W5300SocketByteCount(
        BOARD_ETHERNET_W5300_REG_S0_RX_RSR);
    rxSize = (BoardTest_U16)(rxByteCount & 0xFFFFUL);
    if(rxByteCount < ((BoardTest_U32)BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES +
                      2UL))
    {
        BoardEthernet_UpdateTcpStabilitySnapshot(
            statusMask,
            gBoardEthernetTcpStabilitySnapshot.listenStatus,
            connectedStatus,
            rxSize,
            rxFrameLength,
            rxWord0,
            rxWord1,
            txFreeLow,
            connectedStatus,
            0xFFFFU,
            BoardEthernet_TcpStabilityPassCount);
        BoardEthernet_UpdateTcpStabilityRunningRecord(
            statusMask,
            connectedStatus,
            BoardEthernet_TcpStabilityPassCount,
            record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    statusMask |= BOARD_ETHERNET_TCP_STABILITY_RX_AVAILABLE;
    rxFrameLength = BoardEthernet_W5300Read(
        BOARD_ETHERNET_W5300_REG_S0_RX_FIFO);
    if(rxFrameLength > 64U)
    {
        return BoardEthernet_FailW5300TcpStabilityTest(statusMask,
                                                       connectedStatus,
                                                       rxSize,
                                                       rxFrameLength,
                                                       rxWord0,
                                                       rxWord1,
                                                       txFreeLow,
                                                       connectedStatus,
                                                       record);
    }

    BoardEthernet_W5300ReadTcpEchoPayload(rxFrameLength,
                                          &rxWord0,
                                          &rxWord1);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_IR,
                             BOARD_ETHERNET_SOCKET_IR_RECV);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_RECV);

    if((rxFrameLength == BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES) &&
       (rxWord0 == BOARD_ETHERNET_TCP_ECHO_REQUEST_WORD0) &&
       (rxWord1 == BOARD_ETHERNET_TCP_ECHO_REQUEST_WORD1))
    {
        statusMask |= BOARD_ETHERNET_TCP_STABILITY_RX_MATCH;
    }
    else
    {
        return BoardEthernet_FailW5300TcpStabilityTest(statusMask,
                                                       connectedStatus,
                                                       rxSize,
                                                       rxFrameLength,
                                                       rxWord0,
                                                       rxWord1,
                                                       txFreeLow,
                                                       connectedStatus,
                                                       record);
    }

    txFree = BoardEthernet_W5300SocketByteCount(
        BOARD_ETHERNET_W5300_REG_S0_TX_FSR);
    txFreeLow = (BoardTest_U16)(txFree & 0xFFFFUL);
    if(txFree < BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES)
    {
        return BoardEthernet_FailW5300TcpStabilityTest(statusMask,
                                                       connectedStatus,
                                                       rxSize,
                                                       rxFrameLength,
                                                       rxWord0,
                                                       rxWord1,
                                                       txFreeLow,
                                                       connectedStatus,
                                                       record);
    }

    statusMask |= BOARD_ETHERNET_TCP_STABILITY_TX_FREE;
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_IR,
                             BOARD_ETHERNET_SOCKET_IR_SENDOK);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_TX_FIFO,
                             BOARD_ETHERNET_TCP_ECHO_RESPONSE_WORD0);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_TX_FIFO,
                             BOARD_ETHERNET_TCP_ECHO_RESPONSE_WORD1);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_TX_WRSR, 0U);
    BoardEthernet_W5300Write(
        (BoardTest_U16)(BOARD_ETHERNET_W5300_REG_S0_TX_WRSR + 0x0002U),
        BOARD_ETHERNET_TCP_ECHO_PAYLOAD_BYTES);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_SEND);

    statusMask |= BOARD_ETHERNET_TCP_STABILITY_TX_SENT;
    BoardEthernet_TcpStabilityPassCount++;
    if(BoardEthernet_TcpStabilityPassCount >=
       BOARD_ETHERNET_TCP_STABILITY_TARGET_COUNT)
    {
        statusMask |= BOARD_ETHERNET_TCP_STABILITY_TARGET_REACHED;
        BoardEthernet_TcpStabilityState =
            BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_SENDOK;
    }
    else
    {
        BoardEthernet_TcpStabilityState =
            BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_RX;
    }

    BoardEthernet_UpdateTcpStabilitySnapshot(
        statusMask,
        gBoardEthernetTcpStabilitySnapshot.listenStatus,
        connectedStatus,
        rxSize,
        rxFrameLength,
        rxWord0,
        rxWord1,
        txFreeLow,
        connectedStatus,
        0xFFFFU,
        BoardEthernet_TcpStabilityPassCount);
    BoardEthernet_UpdateTcpStabilityRunningRecord(
        statusMask,
        connectedStatus,
        BoardEthernet_TcpStabilityPassCount,
        record);
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardEthernet_PollW5300TcpStabilitySendOk(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 connectedStatus;
    BoardTest_U16 closedStatus;
    BoardTest_U16 socketState;

    statusMask = gBoardEthernetTcpStabilitySnapshot.statusMask;
    connectedStatus = BoardEthernet_W5300Socket0Status();
    socketState = BoardEthernet_W5300SocketState(connectedStatus);

    if(((connectedStatus & BOARD_ETHERNET_SOCKET_IR_SENDOK) == 0U) &&
       (socketState == BOARD_ETHERNET_SOCKET_SR_ESTABLISHED))
    {
        BoardEthernet_UpdateTcpStabilitySnapshot(
            statusMask,
            gBoardEthernetTcpStabilitySnapshot.listenStatus,
            connectedStatus,
            gBoardEthernetTcpStabilitySnapshot.rxSize,
            gBoardEthernetTcpStabilitySnapshot.rxFrameLength,
            gBoardEthernetTcpStabilitySnapshot.rxWord0,
            gBoardEthernetTcpStabilitySnapshot.rxWord1,
            gBoardEthernetTcpStabilitySnapshot.txFreeLow,
            connectedStatus,
            0xFFFFU,
            BoardEthernet_TcpStabilityPassCount);
        BoardEthernet_UpdateTcpStabilityRunningRecord(
            statusMask,
            connectedStatus,
            BoardEthernet_TcpStabilityPassCount,
            record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    if(((connectedStatus & BOARD_ETHERNET_SOCKET_IR_SENDOK) == 0U) &&
       (socketState != BOARD_ETHERNET_SOCKET_SR_CLOSE_WAIT))
    {
        return BoardEthernet_FailW5300TcpStabilityTest(
            statusMask,
            connectedStatus,
            gBoardEthernetTcpStabilitySnapshot.rxSize,
            gBoardEthernetTcpStabilitySnapshot.rxFrameLength,
            gBoardEthernetTcpStabilitySnapshot.rxWord0,
            gBoardEthernetTcpStabilitySnapshot.rxWord1,
            gBoardEthernetTcpStabilitySnapshot.txFreeLow,
            connectedStatus,
            record);
    }

    statusMask |= BOARD_ETHERNET_TCP_STABILITY_TX_SENT;
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_IR,
                             BOARD_ETHERNET_SOCKET_IR_SENDOK);
    BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                             BOARD_ETHERNET_SOCKET_CMD_CLOSE);
    closedStatus = BoardEthernet_W5300WaitSocketState(
        BOARD_ETHERNET_SOCKET_SR_CLOSED);
    if(BoardEthernet_W5300SocketState(closedStatus) ==
       BOARD_ETHERNET_SOCKET_SR_CLOSED)
    {
        statusMask |= BOARD_ETHERNET_TCP_STABILITY_CLOSE_CLOSED;
    }

    BoardEthernet_UpdateTcpStabilitySnapshot(
        statusMask,
        gBoardEthernetTcpStabilitySnapshot.listenStatus,
        connectedStatus,
        gBoardEthernetTcpStabilitySnapshot.rxSize,
        gBoardEthernetTcpStabilitySnapshot.rxFrameLength,
        gBoardEthernetTcpStabilitySnapshot.rxWord0,
        gBoardEthernetTcpStabilitySnapshot.rxWord1,
        gBoardEthernetTcpStabilitySnapshot.txFreeLow,
        connectedStatus,
        closedStatus,
        BoardEthernet_TcpStabilityPassCount);
    BoardEthernet_TcpStabilityState =
        BOARD_ETHERNET_TCP_STABILITY_STATE_IDLE;

    return BoardEthernet_EvaluateW5300TcpStabilityStatus(
        statusMask,
        connectedStatus,
        BoardEthernet_TcpStabilityPassCount,
        BOARD_ETHERNET_TCP_STABILITY_TARGET_COUNT,
        record);
}

BoardTest_Result BoardEthernet_RunW5300TcpStabilityTest(
    BoardTest_Record *record)
{
    if(BoardEthernet_TcpStabilityState ==
       BOARD_ETHERNET_TCP_STABILITY_STATE_IDLE)
    {
        return BoardEthernet_StartW5300TcpStabilityTest(record);
    }

    if(BoardEthernet_TcpStabilityState ==
       BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_CONNECT)
    {
        return BoardEthernet_PollW5300TcpStabilityConnect(record);
    }

    if(BoardEthernet_TcpStabilityState ==
       BOARD_ETHERNET_TCP_STABILITY_STATE_WAIT_RX)
    {
        return BoardEthernet_PollW5300TcpStabilityRx(record);
    }

    return BoardEthernet_PollW5300TcpStabilitySendOk(record);
}

void BoardEthernet_AbortW5300TcpStabilityTest(void)
{
    if(BoardEthernet_TcpStabilityState !=
       BOARD_ETHERNET_TCP_STABILITY_STATE_IDLE)
    {
        BoardEthernet_W5300Write(BOARD_ETHERNET_W5300_REG_S0_CR,
                                 BOARD_ETHERNET_SOCKET_CMD_CLOSE);
        BoardEthernet_TcpStabilityState =
            BOARD_ETHERNET_TCP_STABILITY_STATE_IDLE;
    }
}
#endif
