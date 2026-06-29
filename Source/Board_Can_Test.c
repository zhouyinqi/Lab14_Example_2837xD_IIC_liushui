#include "Board_Can_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#include "Board_Pinmap.h"
#endif

#define BOARD_CAN_STATUS_ERROR_MASK    0x01E0U

volatile BoardCan_LoopbackSnapshot gBoardCanLoopbackSnapshot =
{
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

BoardTest_Result BoardCan_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U32 txData,
                                                 BoardTest_U32 rxData,
                                                 BoardTest_U16 errorStatus,
                                                 BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       (rxData & 0xFFFFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_CAN_LOOPBACK_EXPECTED_MASK;
    record->expectedMax = (float)BOARD_CAN_LOOPBACK_EXPECTED_MASK;

    if(((statusMask & BOARD_CAN_LOOPBACK_EXPECTED_MASK) ==
        BOARD_CAN_LOOPBACK_EXPECTED_MASK) &&
       (txData == rxData) &&
       ((errorStatus & BOARD_CAN_STATUS_ERROR_MASK) == 0U))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_CAN_LOOPBACK;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST

#define BOARD_CAN_CANB_BASE            0x0004A000UL
#define BOARD_CAN_TIMEOUT              200000UL
#define BOARD_CAN_TX_OBJECT            1U
#define BOARD_CAN_RX_OBJECT            2U
#define BOARD_CAN_DATA_BYTES           4U

#define BOARD_CAN_O_CTL                0x0000UL
#define BOARD_CAN_O_ES                 0x0004UL
#define BOARD_CAN_O_BTR                0x000CUL
#define BOARD_CAN_O_TEST               0x0014UL
#define BOARD_CAN_O_TXRQ_21            0x0088UL
#define BOARD_CAN_O_NDAT_21            0x009CUL
#define BOARD_CAN_O_IF1CMD             0x0100UL
#define BOARD_CAN_O_IF1MSK             0x0104UL
#define BOARD_CAN_O_IF1ARB             0x0108UL
#define BOARD_CAN_O_IF1MCTL            0x010CUL
#define BOARD_CAN_O_IF1DATA            0x0110UL
#define BOARD_CAN_O_IF2CMD             0x0120UL
#define BOARD_CAN_O_IF2MSK             0x0124UL
#define BOARD_CAN_O_IF2ARB             0x0128UL
#define BOARD_CAN_O_IF2MCTL            0x012CUL
#define BOARD_CAN_O_IF2DATA            0x0130UL

#define BOARD_CAN_CTL_INIT             0x0001U
#define BOARD_CAN_CTL_CCE              0x0040U
#define BOARD_CAN_CTL_TEST             0x0080U
#define BOARD_CAN_CTL_SWR              0x8000U

#define BOARD_CAN_ES_TXOK              0x0008U
#define BOARD_CAN_ES_RXOK              0x0010U
#define BOARD_CAN_ES_EPASS             0x0020U
#define BOARD_CAN_ES_EWARN             0x0040U
#define BOARD_CAN_ES_BOFF              0x0080U
#define BOARD_CAN_ES_PER               0x0100U

#define BOARD_CAN_TEST_LBACK           0x0010U

#define BOARD_CAN_IFCMD_BUSY           0x8000U
#define BOARD_CAN_IFCMD_DATA_B         0x00010000UL
#define BOARD_CAN_IFCMD_DATA_A         0x00020000UL
#define BOARD_CAN_IFCMD_TXRQST         0x00040000UL
#define BOARD_CAN_IFCMD_CLRINTPND      0x00080000UL
#define BOARD_CAN_IFCMD_CONTROL        0x00100000UL
#define BOARD_CAN_IFCMD_ARB            0x00200000UL
#define BOARD_CAN_IFCMD_MASK           0x00400000UL
#define BOARD_CAN_IFCMD_DIR            0x00800000UL

#define BOARD_CAN_IFARB_DIR            0x20000000UL
#define BOARD_CAN_IFARB_MSGVAL         0x80000000UL
#define BOARD_CAN_IFARB_STD_ID_SHIFT   18U
#define BOARD_CAN_IFARB_STD_ID_MASK    0x1FFC0000UL

#define BOARD_CAN_IFMCTL_EOB           0x0080U
#define BOARD_CAN_IFMCTL_TXRQST        0x0100U
#define BOARD_CAN_IFMCTL_DLC_MASK      0x000FU

#define BOARD_CAN_OBJECT2_MASK         0x0002UL

/* 200 MHz CAN clock, 500 kbit/s, same nominal timing used by TI Lab48. */
#define BOARD_CAN_BTR_500K_200MHZ      0x00003467UL

#define BoardCan_Reg16(address)        (*((volatile Uint16 *)(address)))

static BoardTest_U32 BoardCan_Read32(BoardTest_U32 address)
{
    BoardTest_U32 low;
    BoardTest_U32 high;

    low = (BoardTest_U32)BoardCan_Reg16(address);
    high = (BoardTest_U32)BoardCan_Reg16(address + 2UL);
    return low | (high << 16U);
}

static void BoardCan_Write32(BoardTest_U32 address, BoardTest_U32 value)
{
    BoardCan_Reg16(address) = (Uint16)(value & 0xFFFFUL);
    BoardCan_Reg16(address + 2UL) = (Uint16)((value >> 16U) & 0xFFFFUL);
}

static BoardTest_U16 BoardCan_WaitClear(BoardTest_U32 address,
                                        BoardTest_U16 mask)
{
    BoardTest_U32 timeout;

    timeout = BOARD_CAN_TIMEOUT;
    while((BoardCan_Reg16(address) & mask) != 0U)
    {
        timeout--;
        if(timeout == 0UL)
        {
            return 0U;
        }
    }
    return 1U;
}

static BoardTest_U16 BoardCan_WaitIf1(void)
{
    return BoardCan_WaitClear(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD,
                              BOARD_CAN_IFCMD_BUSY);
}

static BoardTest_U16 BoardCan_WaitIf2(void)
{
    return BoardCan_WaitClear(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF2CMD,
                              BOARD_CAN_IFCMD_BUSY);
}

static BoardTest_U16 BoardCan_ClearMessageObjects(void)
{
    BoardTest_U16 object;

    if(BoardCan_WaitIf1() == 0U)
    {
        return 0U;
    }

    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD + 2UL) =
        (Uint16)((BOARD_CAN_IFCMD_DIR |
                  BOARD_CAN_IFCMD_ARB |
                  BOARD_CAN_IFCMD_CONTROL) >> 16U);
    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1ARB, 0UL);
    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1MCTL, 0UL);

    for(object = 1U; object <= 32U; object++)
    {
        if(BoardCan_WaitIf1() == 0U)
        {
            return 0U;
        }
        BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD) = object;
    }

    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD + 2UL) =
        (Uint16)((BOARD_CAN_IFCMD_TXRQST |
                  BOARD_CAN_IFCMD_CLRINTPND) >> 16U);

    for(object = 1U; object <= 32U; object++)
    {
        if(BoardCan_WaitIf1() == 0U)
        {
            return 0U;
        }
        BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD) = object;
    }

    return BoardCan_WaitIf1();
}

static BoardTest_U32 BoardCan_MakeStandardArb(BoardTest_U32 msgId,
                                             BoardTest_U32 direction)
{
    BoardTest_U32 arb;

    arb = ((msgId << BOARD_CAN_IFARB_STD_ID_SHIFT) &
           BOARD_CAN_IFARB_STD_ID_MASK);
    arb |= BOARD_CAN_IFARB_MSGVAL;
    arb |= direction;
    return arb;
}

static BoardTest_U16 BoardCan_ConfigureRxObject(void)
{
    BoardTest_U32 command;

    if(BoardCan_WaitIf1() == 0U)
    {
        return 0U;
    }

    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1MSK, 0UL);
    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1ARB,
                     BoardCan_MakeStandardArb(BOARD_CAN_LOOPBACK_MSG_ID, 0UL));
    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1MCTL,
                     BOARD_CAN_IFMCTL_EOB |
                     (BOARD_CAN_DATA_BYTES & BOARD_CAN_IFMCTL_DLC_MASK));

    command = BOARD_CAN_IFCMD_DIR |
              BOARD_CAN_IFCMD_MASK |
              BOARD_CAN_IFCMD_ARB |
              BOARD_CAN_IFCMD_CONTROL |
              BOARD_CAN_IFCMD_DATA_A |
              BOARD_CAN_IFCMD_DATA_B;

    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD + 2UL) =
        (Uint16)(command >> 16U);
    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD) =
        BOARD_CAN_RX_OBJECT;

    return BoardCan_WaitIf1();
}

static BoardTest_U16 BoardCan_SendTxObject(BoardTest_U32 txData)
{
    BoardTest_U32 command;

    if(BoardCan_WaitIf1() == 0U)
    {
        return 0U;
    }

    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1MSK, 0UL);
    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1ARB,
                     BoardCan_MakeStandardArb(BOARD_CAN_LOOPBACK_MSG_ID,
                                              BOARD_CAN_IFARB_DIR));
    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1MCTL,
                     BOARD_CAN_IFMCTL_EOB |
                     BOARD_CAN_IFMCTL_TXRQST |
                     (BOARD_CAN_DATA_BYTES & BOARD_CAN_IFMCTL_DLC_MASK));
    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1DATA, txData);

    command = BOARD_CAN_IFCMD_DIR |
              BOARD_CAN_IFCMD_ARB |
              BOARD_CAN_IFCMD_CONTROL |
              BOARD_CAN_IFCMD_DATA_A |
              BOARD_CAN_IFCMD_DATA_B;

    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD + 2UL) =
        (Uint16)(command >> 16U);
    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF1CMD) =
        BOARD_CAN_TX_OBJECT;

    return BoardCan_WaitIf1();
}

static BoardTest_U16 BoardCan_ReadRxObject(BoardTest_U32 *rxData)
{
    BoardTest_U32 command;

    if(BoardCan_WaitIf2() == 0U)
    {
        return 0U;
    }

    command = BOARD_CAN_IFCMD_DATA_A |
              BOARD_CAN_IFCMD_DATA_B |
              BOARD_CAN_IFCMD_CONTROL |
              BOARD_CAN_IFCMD_ARB |
              BOARD_CAN_IFCMD_TXRQST |
              BOARD_CAN_IFCMD_CLRINTPND;

    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF2CMD + 2UL) =
        (Uint16)(command >> 16U);
    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF2CMD) =
        BOARD_CAN_RX_OBJECT;

    if(BoardCan_WaitIf2() == 0U)
    {
        return 0U;
    }

    *rxData = BoardCan_Read32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_IF2DATA);
    return 1U;
}

static BoardTest_U16 BoardCan_WaitForRx(void)
{
    BoardTest_U32 timeout;
    BoardTest_U32 newData;

    timeout = BOARD_CAN_TIMEOUT;
    do
    {
        newData = BoardCan_Read32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_NDAT_21);
        if((newData & BOARD_CAN_OBJECT2_MASK) != 0UL)
        {
            return 1U;
        }
        timeout--;
    } while(timeout != 0UL);

    return 0U;
}

static BoardTest_U16 BoardCan_RunHardwareLoopback(BoardTest_U32 txData,
                                                 BoardTest_U32 *rxData,
                                                 BoardTest_U16 *errorStatus,
                                                 BoardTest_U16 *statusMask)
{
    BoardTest_U16 es;
    BoardTest_U32 newData;
    BoardTest_U16 result;

    result = 1U;
    *statusMask = 0U;
    *rxData = 0UL;

    EALLOW;
    DevCfgRegs.CPUSEL8.bit.CAN_B = 0U;
    CpuSysRegs.PCLKCR10.bit.CAN_B = 1U;
    ClkCfgRegs.CLKSRCCTL2.bit.CANBBCLKSEL = 0U;
    EDIS;

    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_CTL) = BOARD_CAN_CTL_INIT;
    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_CTL) = BOARD_CAN_CTL_SWR;

    if(BoardCan_ClearMessageObjects() == 0U)
    {
        result = 0U;
    }

    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_CTL) =
        BOARD_CAN_CTL_INIT | BOARD_CAN_CTL_CCE | BOARD_CAN_CTL_TEST;
    BoardCan_Write32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_BTR,
                     BOARD_CAN_BTR_500K_200MHZ);
    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_TEST) =
        BOARD_CAN_TEST_LBACK;

    if((result != 0U) && (BoardCan_ConfigureRxObject() == 0U))
    {
        result = 0U;
    }

    BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_CTL) =
        BOARD_CAN_CTL_TEST;

    if((result != 0U) && (BoardCan_SendTxObject(txData) == 0U))
    {
        result = 0U;
    }

    if((result != 0U) && (BoardCan_WaitForRx() == 0U))
    {
        result = 0U;
    }

    es = BoardCan_Reg16(BOARD_CAN_CANB_BASE + BOARD_CAN_O_ES);
    *errorStatus = es;

    if((es & BOARD_CAN_ES_TXOK) != 0U)
    {
        *statusMask |= BOARD_CAN_LOOPBACK_TX_OK;
    }
    if((es & BOARD_CAN_ES_RXOK) != 0U)
    {
        *statusMask |= BOARD_CAN_LOOPBACK_RX_OK;
    }
    if((es & (BOARD_CAN_ES_EPASS |
              BOARD_CAN_ES_EWARN |
              BOARD_CAN_ES_BOFF |
              BOARD_CAN_ES_PER)) == 0U)
    {
        *statusMask |= BOARD_CAN_LOOPBACK_NO_ERROR;
    }

    newData = BoardCan_Read32(BOARD_CAN_CANB_BASE + BOARD_CAN_O_NDAT_21);
    if((newData & BOARD_CAN_OBJECT2_MASK) != 0UL)
    {
        *statusMask |= BOARD_CAN_LOOPBACK_NEW_DATA;
    }

    if(result != 0U)
    {
        result = BoardCan_ReadRxObject(rxData);
    }

    if(*rxData == txData)
    {
        *statusMask |= BOARD_CAN_LOOPBACK_DATA_MATCH;
    }

    gBoardCanLoopbackSnapshot.statusMask = *statusMask;
    gBoardCanLoopbackSnapshot.errorStatus = *errorStatus;
    gBoardCanLoopbackSnapshot.txLow = (BoardTest_U16)(txData & 0xFFFFUL);
    gBoardCanLoopbackSnapshot.txHigh =
        (BoardTest_U16)((txData >> 16U) & 0xFFFFUL);
    gBoardCanLoopbackSnapshot.rxLow = (BoardTest_U16)(*rxData & 0xFFFFUL);
    gBoardCanLoopbackSnapshot.rxHigh =
        (BoardTest_U16)((*rxData >> 16U) & 0xFFFFUL);
    gBoardCanLoopbackSnapshot.txRequestLow =
        (BoardTest_U16)(BoardCan_Read32(BOARD_CAN_CANB_BASE +
                                        BOARD_CAN_O_TXRQ_21) & 0xFFFFUL);
    gBoardCanLoopbackSnapshot.newDataLow =
        (BoardTest_U16)(newData & 0xFFFFUL);

    return result;
}

BoardTest_Result BoardCan_RunLoopbackTest(BoardTest_Record *record)
{
    BoardTest_U32 rxData;
    BoardTest_U16 errorStatus;
    BoardTest_U16 statusMask;

    if(BoardCan_RunHardwareLoopback(BOARD_CAN_LOOPBACK_TX_PATTERN,
                                    &rxData,
                                    &errorStatus,
                                    &statusMask) == 0U)
    {
        record->rawValue = ((BoardTest_U32)statusMask << 16U) |
                           (rxData & 0xFFFFUL);
        record->measuredValue = (float)statusMask;
        record->expectedMin = (float)BOARD_CAN_LOOPBACK_EXPECTED_MASK;
        record->expectedMax = (float)BOARD_CAN_LOOPBACK_EXPECTED_MASK;
        record->errorCode = BOARD_TEST_ERROR_CAN_LOOPBACK;
        return BOARD_TEST_RESULT_FAIL;
    }

    return BoardCan_EvaluateLoopbackStatus(statusMask,
                                           BOARD_CAN_LOOPBACK_TX_PATTERN,
                                           rxData,
                                           errorStatus,
                                           record);
}
#endif
