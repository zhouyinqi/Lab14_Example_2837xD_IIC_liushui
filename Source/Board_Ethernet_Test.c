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
#define BOARD_ETHERNET_W5300_REG_S0_IR      0x0206U
#define BOARD_ETHERNET_W5300_REG_S0_SSR     0x0208U

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

#ifndef BOARD_TEST_HOST
extern void InitXintf16Gpio(void);

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

static BoardTest_U16 BoardEthernet_W5300Socket0Status(void)
{
    BoardTest_U16 sr;
    BoardTest_U16 ir;

    sr = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_S0_SSR);
    ir = BoardEthernet_W5300Read(BOARD_ETHERNET_W5300_REG_S0_IR);

    return (BoardTest_U16)(((sr & 0x00FFU) << 8U) | (ir & 0x00FFU));
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

    BoardEmif_ConfigureExternalAsync();
    InitXintf16Gpio();

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
#endif
