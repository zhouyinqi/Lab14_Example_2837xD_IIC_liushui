#include "Board_Sci_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#include "Board_Pinmap.h"

#define BOARD_SCI_TIMEOUT       200000UL
#define BOARD_SCI_BRR_115200    53U
#define BOARD_SCI_BRR_9600_H    0x0002U
#define BOARD_SCI_BRR_9600_L    0x008BU

#define BOARD_SCI_RS485_STATE_IDLE      0x0000U
#define BOARD_SCI_RS485_STATE_WAIT_RX   0x0001U
#define BOARD_SCI_RS485_STATE_WAIT_TX   0x0002U

volatile BoardSci_LoopbackSnapshot gBoardSciLoopbackSnapshot =
{
    0U,
    0U,
    0U,
    0U,
    0U
};

volatile BoardSci_Rs485ExternalSnapshot gBoardSciRs485ExternalSnapshot =
{
    0U,
    0U,
    0U,
    0U,
    0U
};

static BoardTest_U16 BoardSci_Rs485ExternalState =
    BOARD_SCI_RS485_STATE_IDLE;

#endif

BoardTest_Result BoardSci_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                  BoardTest_U16 sciaRx,
                                                  BoardTest_U16 scibRx,
                                                  BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0x00FFUL) << 16U) |
                       (((BoardTest_U32)sciaRx & 0x00FFUL) << 8U) |
                       ((BoardTest_U32)scibRx & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_SCI_LOOPBACK_EXPECTED_MASK;
    record->expectedMax = (float)BOARD_SCI_LOOPBACK_EXPECTED_MASK;

    if((statusMask & BOARD_SCI_LOOPBACK_EXPECTED_MASK) ==
       BOARD_SCI_LOOPBACK_EXPECTED_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_SCI_LOOPBACK;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardSci_EvaluateRs485ExternalStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       (((BoardTest_U32)rxValue & 0x00FFUL) << 8U) |
                       ((BoardTest_U32)txValue & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin =
        (float)BOARD_SCI_RS485_EXTERNAL_REQUIRED_MASK;
    record->expectedMax =
        (float)BOARD_SCI_RS485_EXTERNAL_DIAGNOSTIC_MASK;

    if((statusMask & BOARD_SCI_RS485_EXTERNAL_REQUIRED_MASK) ==
       BOARD_SCI_RS485_EXTERNAL_REQUIRED_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_SCI_RS485_EXTERNAL;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
static void BoardSci_InitForLoopback(volatile struct SCI_REGS *regs)
{


    regs->SCICTL1.all = 0x0000U;
    regs->SCICCR.all = 0x0017U;
    regs->SCICTL1.all = 0x0003U;
    regs->SCICTL2.all = 0x0000U;
    regs->SCIHBAUD = 0U;
    regs->SCILBAUD = BOARD_SCI_BRR_115200;
    regs->SCIFFTX.all = 0xC040U;
    regs->SCIFFRX.all = 0x0041U;
    regs->SCIFFCT.all = 0x0000U;
    regs->SCIFFTX.all = 0xE040U;
    regs->SCIFFRX.all = 0x2041U;
    regs->SCIPRI.bit.FREE = 1U;
    regs->SCICTL1.all = 0x0023U;
}

static BoardTest_U16 BoardSci_SendReceive(volatile struct SCI_REGS *regs,
                                          BoardTest_U16 txValue,
                                          BoardTest_U16 *rxValue,
                                          BoardTest_U16 *detail)
{
    BoardTest_U32 timeout;
    BoardTest_U16 rxBuffer;

    *detail = 0U;
    regs->SCIFFRX.bit.RXFFOVRCLR = 1U;
    regs->SCIFFRX.bit.RXFFINTCLR = 1U;
    regs->SCIFFTX.bit.TXFFINTCLR = 1U;

    timeout = BOARD_SCI_TIMEOUT;
    while(regs->SCIFFTX.bit.TXFFST >= 16U)
    {
        if(timeout-- == 0UL)
        {
            *detail |= BOARD_SCI_DETAIL_TX_TIMEOUT;
            return 0U;
        }
    }

    *detail |= BOARD_SCI_DETAIL_TX_READY;
    regs->SCITXBUF = txValue & 0x00FFU;
    *detail |= BOARD_SCI_DETAIL_TX_WRITTEN;

    timeout = BOARD_SCI_TIMEOUT;
    while((regs->SCIFFRX.bit.RXFFST == 0U) &&
          (regs->SCIRXST.bit.RXRDY == 0U))
    {
        if(regs->SCIRXST.bit.RXERROR == 1U)
        {
            *detail |= BOARD_SCI_DETAIL_RX_ERROR;
            return 0U;
        }

        if(timeout-- == 0UL)
        {
            *detail |= BOARD_SCI_DETAIL_RX_TIMEOUT;
            return 0U;
        }
    }

    *detail |= BOARD_SCI_DETAIL_RX_READY;
    rxBuffer = regs->SCIRXBUF.all;
    *rxValue = rxBuffer & 0x00FFU;

    if((regs->SCIRXST.bit.RXERROR == 1U) ||
       ((rxBuffer & 0xC000U) != 0U))
    {
        *detail |= BOARD_SCI_DETAIL_RX_ERROR;
        return 0U;
    }

    if(*rxValue == (txValue & 0x00FFU))
    {
        *detail |= BOARD_SCI_DETAIL_RX_MATCH;
        return 1U;
    }

    return 0U;
}

static void BoardSci_SetRs485ReceiveSafe(void)
{
    GPIO_SetupPinMux(BOARD_PIN_SCIA_DIRECTION, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinMux(BOARD_PIN_SCIB_DIRECTION, GPIO_MUX_CPU1, 0U);
    GPIO_WritePin(BOARD_PIN_SCIA_DIRECTION, 0U);
    GPIO_WritePin(BOARD_PIN_SCIB_DIRECTION, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_SCIA_DIRECTION, GPIO_OUTPUT, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_SCIB_DIRECTION, GPIO_OUTPUT, 0U);
}

static void BoardSci_SetScibDirection(BoardTest_U16 level)
{
    GPIO_WritePin(BOARD_PIN_SCIB_DIRECTION, level);
    gBoardSciRs485ExternalSnapshot.directionLevel = level;
}

static void BoardSci_InitScibRs485External(void)
{
    GPIO_SetupPinMux(BOARD_PIN_SCIB_RX, GPIO_MUX_CPU1, 2U);
    GPIO_SetupPinOptions(BOARD_PIN_SCIB_RX, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(BOARD_PIN_SCIB_TX, GPIO_MUX_CPU1, 2U);
    GPIO_SetupPinOptions(BOARD_PIN_SCIB_TX, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(BOARD_PIN_SCIB_DIRECTION, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_SCIB_DIRECTION, GPIO_OUTPUT,
                         GPIO_PUSHPULL);
    BoardSci_SetScibDirection(0U);

    ScibRegs.SCICTL1.all = 0x0000U;
    ScibRegs.SCICCR.all = 0x0007U;
    ScibRegs.SCICTL1.all = 0x0003U;
    ScibRegs.SCICTL2.all = 0x0003U;
    ScibRegs.SCICTL2.bit.TXINTENA = 1U;
    ScibRegs.SCICTL2.bit.RXBKINTENA = 1U;
    ScibRegs.SCIHBAUD = BOARD_SCI_BRR_9600_H;
    ScibRegs.SCILBAUD = BOARD_SCI_BRR_9600_L;
    ScibRegs.SCIFFTX.all = 0xE040U;
    ScibRegs.SCIFFRX.all = 0x2044U;
    ScibRegs.SCIFFCT.all = 0x0000U;
    ScibRegs.SCIPRI.bit.FREE = 1U;
    ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;
    ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1U;
    ScibRegs.SCIFFTX.bit.TXFFINTCLR = 1U;
    ScibRegs.SCICTL1.all = 0x0023U;
}

static void BoardSci_UpdateRs485ExternalSnapshot(BoardTest_U16 statusMask,
                                                 BoardTest_U16 rxValue,
                                                 BoardTest_U16 txValue,
                                                 BoardTest_U16 detail)
{
    gBoardSciRs485ExternalSnapshot.statusMask = statusMask;
    gBoardSciRs485ExternalSnapshot.rxValue = rxValue;
    gBoardSciRs485ExternalSnapshot.txValue = txValue;
    gBoardSciRs485ExternalSnapshot.detail = detail;
}

static void BoardSci_UpdateRs485ExternalRunningRecord(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       (((BoardTest_U32)rxValue & 0x00FFUL) << 8U) |
                       ((BoardTest_U32)txValue & 0x00FFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin =
        (float)BOARD_SCI_RS485_EXTERNAL_REQUIRED_MASK;
    record->expectedMax =
        (float)BOARD_SCI_RS485_EXTERNAL_DIAGNOSTIC_MASK;
    record->errorCode = BOARD_TEST_ERROR_NONE;
}

static BoardTest_Result BoardSci_FailRs485ExternalTest(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_U16 detail,
    BoardTest_Record *record)
{
    BoardSci_SetScibDirection(0U);
    statusMask |= BOARD_SCI_RS485_EXTERNAL_BACK_TO_RX;
    BoardSci_Rs485ExternalState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                         rxValue,
                                         txValue,
                                         detail);
    return BoardSci_EvaluateRs485ExternalStatus(statusMask,
                                                rxValue,
                                                txValue,
                                                record);
}

static BoardTest_Result BoardSci_StartRs485ExternalTest(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 detail;

    statusMask = 0U;
    detail = 0U;

    BoardSci_InitScibRs485External();
    statusMask |= BOARD_SCI_RS485_EXTERNAL_CONFIGURED;
    statusMask |= BOARD_SCI_RS485_EXTERNAL_RX_ENABLE_LOW;
    BoardSci_Rs485ExternalState = BOARD_SCI_RS485_STATE_WAIT_RX;

    BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                         0U,
                                         BOARD_SCI_RS485_EXTERNAL_RESPONSE,
                                         detail);
    BoardSci_UpdateRs485ExternalRunningRecord(
        statusMask,
        0U,
        BOARD_SCI_RS485_EXTERNAL_RESPONSE,
        record);

    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardSci_PollRs485ExternalRx(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 rxValue;
    BoardTest_U16 txValue;
    BoardTest_U16 detail;
    BoardTest_U16 rxBuffer;

    statusMask = gBoardSciRs485ExternalSnapshot.statusMask;
    rxValue = gBoardSciRs485ExternalSnapshot.rxValue;
    txValue = BOARD_SCI_RS485_EXTERNAL_RESPONSE;
    detail = gBoardSciRs485ExternalSnapshot.detail;

    if((ScibRegs.SCIRXST.bit.RXERROR == 1U) ||
       (ScibRegs.SCIFFRX.bit.RXFFOVF == 1U))
    {
        ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        return BoardSci_FailRs485ExternalTest(statusMask,
                                              rxValue,
                                              txValue,
                                              detail,
                                              record);
    }

    if((ScibRegs.SCIFFRX.bit.RXFFST == 0U) &&
       (ScibRegs.SCIRXST.bit.RXRDY == 0U))
    {
        BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                             rxValue,
                                             txValue,
                                             detail);
        BoardSci_UpdateRs485ExternalRunningRecord(statusMask,
                                                  rxValue,
                                                  txValue,
                                                  record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    rxBuffer = ScibRegs.SCIRXBUF.all;
    rxValue = rxBuffer & 0x00FFU;
    statusMask |= BOARD_SCI_RS485_EXTERNAL_RX_READY;
    detail |= BOARD_SCI_DETAIL_RX_READY;

    if((rxBuffer & 0xC000U) != 0U)
    {
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        return BoardSci_FailRs485ExternalTest(statusMask,
                                              rxValue,
                                              txValue,
                                              detail,
                                              record);
    }

    if(rxValue != BOARD_SCI_RS485_EXTERNAL_REQUEST)
    {
        return BoardSci_FailRs485ExternalTest(statusMask,
                                              rxValue,
                                              txValue,
                                              detail,
                                              record);
    }

    statusMask |= BOARD_SCI_RS485_EXTERNAL_RX_MATCH;
    detail |= BOARD_SCI_DETAIL_RX_MATCH;

    if(ScibRegs.SCIFFTX.bit.TXFFST >= 16U)
    {
        detail |= BOARD_SCI_DETAIL_TX_TIMEOUT;
        return BoardSci_FailRs485ExternalTest(statusMask,
                                              rxValue,
                                              txValue,
                                              detail,
                                              record);
    }

    BoardSci_SetScibDirection(1U);
    statusMask |= BOARD_SCI_RS485_EXTERNAL_TX_ENABLE_HIGH;
    detail |= BOARD_SCI_DETAIL_TX_READY;
    ScibRegs.SCITXBUF = txValue & 0x00FFU;
    statusMask |= BOARD_SCI_RS485_EXTERNAL_TX_WRITTEN;
    detail |= BOARD_SCI_DETAIL_TX_WRITTEN;
    BoardSci_Rs485ExternalState = BOARD_SCI_RS485_STATE_WAIT_TX;

    BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                         rxValue,
                                         txValue,
                                         detail);
    BoardSci_UpdateRs485ExternalRunningRecord(statusMask,
                                              rxValue,
                                              txValue,
                                              record);
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardSci_PollRs485ExternalTxDone(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 rxValue;
    BoardTest_U16 txValue;
    BoardTest_U16 detail;

    statusMask = gBoardSciRs485ExternalSnapshot.statusMask;
    rxValue = gBoardSciRs485ExternalSnapshot.rxValue;
    txValue = gBoardSciRs485ExternalSnapshot.txValue;
    detail = gBoardSciRs485ExternalSnapshot.detail;

    if((ScibRegs.SCIFFTX.bit.TXFFST != 0U) ||
       (ScibRegs.SCICTL2.bit.TXEMPTY == 0U))
    {
        BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                             rxValue,
                                             txValue,
                                             detail);
        BoardSci_UpdateRs485ExternalRunningRecord(statusMask,
                                                  rxValue,
                                                  txValue,
                                                  record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    statusMask |= BOARD_SCI_RS485_EXTERNAL_TX_DONE;
    BoardSci_SetScibDirection(0U);
    statusMask |= BOARD_SCI_RS485_EXTERNAL_BACK_TO_RX;
    BoardSci_Rs485ExternalState = BOARD_SCI_RS485_STATE_IDLE;

    BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                         rxValue,
                                         txValue,
                                         detail);
    return BoardSci_EvaluateRs485ExternalStatus(statusMask,
                                                rxValue,
                                                txValue,
                                                record);
}

BoardTest_Result BoardSci_RunLoopbackTest(BoardTest_Record *record)
{
    BoardTest_U16 statusMask = 0U;
    BoardTest_U16 sciaRx = 0U;
    BoardTest_U16 scibRx = 0U;
    BoardTest_U16 sciaDetail = 0U;
    BoardTest_U16 scibDetail = 0U;

    BoardSci_SetRs485ReceiveSafe();

    BoardSci_InitForLoopback(&SciaRegs);
    if(BoardSci_SendReceive(&SciaRegs,
                            BOARD_SCI_LOOPBACK_SCIA_PATTERN,
                            &sciaRx,
                            &sciaDetail) != 0U)
    {
        statusMask |= (BoardTest_U16)BOARD_SCI_LOOPBACK_SCIA_PASS;
    }

    BoardSci_InitForLoopback(&ScibRegs);
    if(BoardSci_SendReceive(&ScibRegs,
                            BOARD_SCI_LOOPBACK_SCIB_PATTERN,
                            &scibRx,
                            &scibDetail) != 0U)
    {
        statusMask |= (BoardTest_U16)BOARD_SCI_LOOPBACK_SCIB_PASS;
    }

    gBoardSciLoopbackSnapshot.sciaRx = sciaRx;
    gBoardSciLoopbackSnapshot.scibRx = scibRx;
    gBoardSciLoopbackSnapshot.statusMask = statusMask;
    gBoardSciLoopbackSnapshot.sciaDetail = sciaDetail;
    gBoardSciLoopbackSnapshot.scibDetail = scibDetail;

    return BoardSci_EvaluateLoopbackStatus(statusMask, sciaRx, scibRx, record);
}

BoardTest_Result BoardSci_RunRs485ExternalTest(BoardTest_Record *record)
{
    if(BoardSci_Rs485ExternalState == BOARD_SCI_RS485_STATE_IDLE)
    {
        return BoardSci_StartRs485ExternalTest(record);
    }

    if(BoardSci_Rs485ExternalState == BOARD_SCI_RS485_STATE_WAIT_RX)
    {
        return BoardSci_PollRs485ExternalRx(record);
    }

    return BoardSci_PollRs485ExternalTxDone(record);
}

void BoardSci_AbortRs485ExternalTest(void)
{
    if(BoardSci_Rs485ExternalState != BOARD_SCI_RS485_STATE_IDLE)
    {
        BoardSci_SetScibDirection(0U);
        BoardSci_Rs485ExternalState = BOARD_SCI_RS485_STATE_IDLE;
    }
}
#endif
