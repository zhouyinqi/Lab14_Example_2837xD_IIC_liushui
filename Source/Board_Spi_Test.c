#include "Board_Spi_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardSpi_LoopbackSnapshot gBoardSpiLoopbackSnapshot =
{
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

BoardTest_Result BoardSpi_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U16 txValue,
                                                 BoardTest_U16 rxValue,
                                                 BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)rxValue & 0xFFFFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_SPI_LOOPBACK_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_SPI_LOOPBACK_DIAGNOSTIC_MASK;

    if(((statusMask & BOARD_SPI_LOOPBACK_REQUIRED_MASK) ==
        BOARD_SPI_LOOPBACK_REQUIRED_MASK) &&
       (txValue == rxValue))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_SPI_LOOPBACK;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST

#define BOARD_SPI_TIMEOUT   200000UL
#define BOARD_SPI_BRR       0x0063U

static void BoardSpi_EnableClock(BoardTest_U16 testId)
{
    EALLOW;
    if(testId == BOARD_TEST_ID_SPIA_INTERNAL)
    {
        DevCfgRegs.CPUSEL6.bit.SPI_A = 0U;
        CpuSysRegs.PCLKCR8.bit.SPI_A = 1U;
    }
    else if(testId == BOARD_TEST_ID_SPIB_INTERNAL)
    {
        DevCfgRegs.CPUSEL6.bit.SPI_B = 0U;
        CpuSysRegs.PCLKCR8.bit.SPI_B = 1U;
    }
    else
    {
        DevCfgRegs.CPUSEL6.bit.SPI_C = 0U;
        CpuSysRegs.PCLKCR8.bit.SPI_C = 1U;
    }
    EDIS;
}

static void BoardSpi_InitLoopback(volatile struct SPI_REGS *regs)
{
    regs->SPICCR.bit.SPISWRESET = 0U;
    regs->SPICCR.all = 0x001FU;
    regs->SPICTL.all = 0x0006U;
    regs->SPISTS.all = 0x0000U;
    regs->SPIBRR = BOARD_SPI_BRR;
    regs->SPIFFTX.all = 0xC000U;
    regs->SPIFFRX.all = 0x0001U;
    regs->SPIFFCT.all = 0x0000U;
    regs->SPIPRI.all = 0x0010U;
    regs->SPICCR.bit.SPISWRESET = 1U;
    regs->SPIFFTX.bit.TXFIFO = 1U;
    regs->SPIFFRX.bit.RXFIFORESET = 1U;
}

static void BoardSpi_StopLoopback(volatile struct SPI_REGS *regs)
{
    regs->SPICCR.bit.SPISWRESET = 0U;
    regs->SPICCR.bit.SPILBK = 0U;
    regs->SPIFFTX.bit.TXFIFO = 0U;
    regs->SPIFFRX.bit.RXFIFORESET = 0U;
}

static BoardTest_U16 BoardSpi_SendReceive(volatile struct SPI_REGS *regs,
                                          BoardTest_U16 txValue,
                                          BoardTest_U16 *rxValue)
{
    BoardTest_U32 timeout;
    BoardTest_U16 statusMask;

    statusMask = 0U;
    *rxValue = 0U;

    regs->SPIFFRX.bit.RXFFOVFCLR = 1U;
    regs->SPIFFRX.bit.RXFFINTCLR = 1U;
    regs->SPIFFTX.bit.TXFFINTCLR = 1U;

    timeout = BOARD_SPI_TIMEOUT;
    while(regs->SPIFFTX.bit.TXFFST >= 16U)
    {
        if(timeout-- == 0UL)
        {
            return statusMask;
        }
    }

    statusMask |= BOARD_SPI_LOOPBACK_TX_READY;
    regs->SPITXBUF = txValue;
    statusMask |= BOARD_SPI_LOOPBACK_TX_WRITTEN;

    timeout = BOARD_SPI_TIMEOUT;
    while(regs->SPIFFRX.bit.RXFFST == 0U)
    {
        if(timeout-- == 0UL)
        {
            return statusMask;
        }
    }

    statusMask |= BOARD_SPI_LOOPBACK_RX_READY;
    *rxValue = regs->SPIRXBUF;

    if((regs->SPISTS.bit.OVERRUN_FLAG == 0U) &&
       (regs->SPIFFRX.bit.RXFFOVF == 0U))
    {
        statusMask |= BOARD_SPI_LOOPBACK_NO_OVERRUN;
    }

    if(*rxValue == txValue)
    {
        statusMask |= BOARD_SPI_LOOPBACK_RX_MATCH;
    }

    return statusMask;
}

static BoardTest_Result BoardSpi_RunLoopbackTest(BoardTest_U16 testId,
                                                 volatile struct SPI_REGS *regs,
                                                 BoardTest_U16 txValue,
                                                 BoardTest_Record *record)
{
    BoardTest_U16 rxValue;
    BoardTest_U16 statusMask;

    BoardSpi_EnableClock(testId);
    BoardSpi_InitLoopback(regs);
    statusMask = BoardSpi_SendReceive(regs, txValue, &rxValue);
    BoardSpi_StopLoopback(regs);

    if(testId == BOARD_TEST_ID_SPIA_INTERNAL)
    {
        gBoardSpiLoopbackSnapshot.spiaStatusMask = statusMask;
        gBoardSpiLoopbackSnapshot.spiaRx = rxValue;
    }
    else if(testId == BOARD_TEST_ID_SPIB_INTERNAL)
    {
        gBoardSpiLoopbackSnapshot.spibStatusMask = statusMask;
        gBoardSpiLoopbackSnapshot.spibRx = rxValue;
    }
    else
    {
        gBoardSpiLoopbackSnapshot.spicStatusMask = statusMask;
        gBoardSpiLoopbackSnapshot.spicRx = rxValue;
    }

    return BoardSpi_EvaluateLoopbackStatus(statusMask,
                                           txValue,
                                           rxValue,
                                           record);
}

BoardTest_Result BoardSpi_RunSpiaLoopbackTest(BoardTest_Record *record)
{
    return BoardSpi_RunLoopbackTest(BOARD_TEST_ID_SPIA_INTERNAL,
                                    &SpiaRegs,
                                    BOARD_SPI_LOOPBACK_SPIA_PATTERN,
                                    record);
}

BoardTest_Result BoardSpi_RunSpibLoopbackTest(BoardTest_Record *record)
{
    return BoardSpi_RunLoopbackTest(BOARD_TEST_ID_SPIB_INTERNAL,
                                    &SpibRegs,
                                    BOARD_SPI_LOOPBACK_SPIB_PATTERN,
                                    record);
}

BoardTest_Result BoardSpi_RunSpicLoopbackTest(BoardTest_Record *record)
{
    return BoardSpi_RunLoopbackTest(BOARD_TEST_ID_SPIC_INTERNAL,
                                    &SpicRegs,
                                    BOARD_SPI_LOOPBACK_SPIC_PATTERN,
                                    record);
}
#endif
