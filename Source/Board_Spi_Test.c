#include "Board_Spi_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#include "Board_Pinmap.h"
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

volatile BoardSpi_ExternalSnapshot gBoardSpiExternalSnapshot =
{
    0U,
    BOARD_SPI_EXTERNAL_FAIL_NONE,
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
    0U,
    0U,
    1U
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

BoardTest_Result BoardSpi_EvaluateExternalStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U16 failCode,
                                                 BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)failCode & 0xFFFFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_SPI_EXTERNAL_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_SPI_EXTERNAL_DIAGNOSTIC_MASK;

    if(((statusMask & BOARD_SPI_EXTERNAL_REQUIRED_MASK) ==
        BOARD_SPI_EXTERNAL_REQUIRED_MASK) &&
       (failCode == BOARD_SPI_EXTERNAL_FAIL_NONE))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_SPI_EXTERNAL;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST

#define BOARD_SPI_TIMEOUT   200000UL
#define BOARD_SPI_BRR       0x0063U

#define BOARD_SPIC_MUX_VALUE             6U
#define BOARD_SPIC_DEFAULT_POLARITY      0U
#define BOARD_SPIC_DEFAULT_PHASE         0U
#define BOARD_SPIC_DEFAULT_BRR           49U
#define BOARD_SPIC_FRAM_POLARITY         0U
#define BOARD_SPIC_FRAM_PHASE            1U
#define BOARD_SPIC_FRAM_BRR              49U
#define BOARD_SPIC_FRAM_CS_DELAY_US      2U
#define BOARD_SPIC_XFER_TIMEOUT          60000UL

#define BOARD_SPIC_CMD_WREN              0x06U
#define BOARD_SPIC_CMD_RDSR              0x05U
#define BOARD_SPIC_CMD_READ              0x03U
#define BOARD_SPIC_CMD_WRITE             0x02U
#define BOARD_SPIC_CMD_RDID              0x9FU

#define BOARD_SPIC_FRAM_TEST_ADDR        0x00000000UL
#define BOARD_SPIC_FRAM_ADDR_BYTES       3U
#define BOARD_SPIC_FRAM_TEST_PATTERN     0xA5U

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

static void BoardSpi_SpicCsHigh(BoardTest_U16 pin)
{
    GPIO_WritePin(pin, 1U);
}

static void BoardSpi_SpicCsLow(BoardTest_U16 pin)
{
    GPIO_WritePin(pin, 0U);
}

static void BoardSpi_SpicDeselectAll(void)
{
    BoardSpi_SpicCsHigh(BOARD_PIN_SPIC_FLASH_CS);
    BoardSpi_SpicCsHigh(BOARD_PIN_SPIC_FRAM_CS);
    BoardSpi_SpicCsHigh(BOARD_PIN_SPIC_FPGA_FLASH_CS);
}

static void BoardSpi_SpicDelayUs(BoardTest_U16 delayUs)
{
    if(delayUs <= 2U)
    {
        DELAY_US(2U);
    }
    else if(delayUs <= 10U)
    {
        DELAY_US(10U);
    }
    else
    {
        DELAY_US(50U);
    }
}

static void BoardSpi_InitSpicGpio(void)
{
    GPIO_SetupPinMux(BOARD_PIN_SPIC_SIMO, GPIO_MUX_CPU1,
                     BOARD_SPIC_MUX_VALUE);
    GPIO_SetupPinOptions(BOARD_PIN_SPIC_SIMO, GPIO_OUTPUT, GPIO_ASYNC);

    GPIO_SetupPinMux(BOARD_PIN_SPIC_SOMI, GPIO_MUX_CPU1,
                     BOARD_SPIC_MUX_VALUE);
    GPIO_SetupPinOptions(BOARD_PIN_SPIC_SOMI, GPIO_INPUT,
                         GPIO_ASYNC | GPIO_PULLUP);

    GPIO_SetupPinMux(BOARD_PIN_SPIC_CLK, GPIO_MUX_CPU1,
                     BOARD_SPIC_MUX_VALUE);
    GPIO_SetupPinOptions(BOARD_PIN_SPIC_CLK, GPIO_OUTPUT, GPIO_ASYNC);

    GPIO_SetupPinMux(BOARD_PIN_SPIC_FLASH_CS, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_SPIC_FLASH_CS, GPIO_OUTPUT,
                         GPIO_PUSHPULL | GPIO_PULLUP);

    GPIO_SetupPinMux(BOARD_PIN_SPIC_FRAM_CS, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_SPIC_FRAM_CS, GPIO_OUTPUT,
                         GPIO_PUSHPULL | GPIO_PULLUP);

    GPIO_SetupPinMux(BOARD_PIN_SPIC_FPGA_FLASH_CS, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_SPIC_FPGA_FLASH_CS, GPIO_OUTPUT,
                         GPIO_PUSHPULL | GPIO_PULLUP);

    GPIO_SetupPinMux(BOARD_PIN_FPGA_PROGRAM, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_FPGA_PROGRAM, GPIO_OUTPUT,
                         GPIO_PUSHPULL | GPIO_PULLUP);
    GPIO_WritePin(BOARD_PIN_FPGA_PROGRAM, 1U);

    GPIO_SetupPinMux(BOARD_PIN_FPGA_DONE, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_FPGA_DONE, GPIO_INPUT,
                         GPIO_ASYNC | GPIO_PULLUP);

    BoardSpi_SpicDeselectAll();
}

static void BoardSpi_InitSpicMode(BoardTest_U16 polarity,
                                  BoardTest_U16 phase,
                                  BoardTest_U16 brr)
{
    EALLOW;
    DevCfgRegs.CPUSEL6.bit.SPI_C = 0U;
    CpuSysRegs.PCLKCR8.bit.SPI_C = 1U;
    EDIS;

    SpicRegs.SPICCR.bit.SPISWRESET = 0U;
    SpicRegs.SPICCR.all = 0x0007U |
                          (polarity ? 0x0040U : 0x0000U);
    SpicRegs.SPICTL.all = 0x0006U |
                          (phase ? 0x0008U : 0x0000U);
    SpicRegs.SPIBRR = brr;
    SpicRegs.SPIFFTX.all = 0xE040U;
    SpicRegs.SPIFFRX.all = 0x2044U;
    SpicRegs.SPIFFCT.all = 0x0000U;
    SpicRegs.SPIPRI.bit.FREE = 1U;
    SpicRegs.SPICCR.bit.SPISWRESET = 1U;
}

static void BoardSpi_InitSpicDefault(void)
{
    BoardSpi_InitSpicMode(BOARD_SPIC_DEFAULT_POLARITY,
                          BOARD_SPIC_DEFAULT_PHASE,
                          BOARD_SPIC_DEFAULT_BRR);
}

static BoardTest_U16 BoardSpi_SpicTransferByte(BoardTest_U16 data,
                                               BoardTest_U16 *ok)
{
    BoardTest_U32 timeout;

    *ok = 1U;

    timeout = BOARD_SPIC_XFER_TIMEOUT;
    while(SpicRegs.SPISTS.bit.BUFFULL_FLAG != 0U)
    {
        if(timeout-- == 0UL)
        {
            *ok = 0U;
            return 0U;
        }
    }

    SpicRegs.SPITXBUF = (data & 0x00FFU) << 8U;

    timeout = BOARD_SPIC_XFER_TIMEOUT;
    while(SpicRegs.SPIFFRX.bit.RXFFST == 0U)
    {
        if(timeout-- == 0UL)
        {
            *ok = 0U;
            return 0U;
        }
    }

    data = SpicRegs.SPIRXBUF & 0x00FFU;
    SpicRegs.SPIFFRX.bit.RXFFOVFCLR = 1U;
    SpicRegs.SPIFFRX.bit.RXFFINTCLR = 1U;

    return data;
}

static BoardTest_U16 BoardSpi_SpicReadBytesAfterCommand(
    BoardTest_U16 csPin,
    BoardTest_U16 cmd,
    BoardTest_U16 *data,
    BoardTest_U16 count,
    BoardTest_U16 csDelayUs)
{
    BoardTest_U16 ok;
    BoardTest_U16 index;

    for(index = 0U; index < count; index++)
    {
        data[index] = 0U;
    }

    BoardSpi_SpicDeselectAll();
    DELAY_US(10U);

    BoardSpi_SpicCsLow(csPin);
    BoardSpi_SpicDelayUs(csDelayUs);
    (void)BoardSpi_SpicTransferByte(cmd, &ok);
    if(ok == 0U)
    {
        BoardSpi_SpicDeselectAll();
        return 0U;
    }

    for(index = 0U; index < count; index++)
    {
        data[index] = BoardSpi_SpicTransferByte(0x00U, &ok);
        if(ok == 0U)
        {
            BoardSpi_SpicDeselectAll();
            return 0U;
        }
    }

    BoardSpi_SpicDelayUs(csDelayUs);
    BoardSpi_SpicDeselectAll();
    return 1U;
}

static BoardTest_U16 BoardSpi_SpiIdLooksValid(const BoardTest_U16 *id,
                                              BoardTest_U16 count)
{
    BoardTest_U16 allOr;
    BoardTest_U16 allAnd;
    BoardTest_U16 index;

    allOr = 0U;
    allAnd = 0x00FFU;
    for(index = 0U; index < count; index++)
    {
        allOr |= id[index];
        allAnd &= id[index];
    }

    if(allOr == 0U)
    {
        return 0U;
    }

    if((allAnd & 0x00FFU) == 0x00FFU)
    {
        return 0U;
    }

    return 1U;
}

static BoardTest_U16 BoardSpi_FlashIdIsW25Q64(const BoardTest_U16 *id)
{
    return ((id[0] == 0xEFU) &&
            ((id[1] == 0x40U) || (id[1] == 0x70U)) &&
            (id[2] == 0x17U)) ? 1U : 0U;
}

static BoardTest_U16 BoardSpi_FramIdIsFm25v20a(const BoardTest_U16 *id)
{
    BoardTest_U16 index;

    for(index = 0U; index < 6U; index++)
    {
        if(id[index] != 0x7FU)
        {
            return 0U;
        }
    }

    return ((id[6] == 0xC2U) &&
            (id[7] == 0x25U) &&
            (id[8] == 0x08U)) ? 1U : 0U;
}

static void BoardSpi_FramWake(void)
{
    BoardTest_U16 ok;

    BoardSpi_SpicDeselectAll();
    DELAY_US(1000U);
    BoardSpi_SpicCsLow(BOARD_PIN_SPIC_FRAM_CS);
    DELAY_US(2U);
    (void)BoardSpi_SpicTransferByte(0xFFU, &ok);
    DELAY_US(2U);
    BoardSpi_SpicDeselectAll();
    DELAY_US(1000U);
}

static BoardTest_U16 BoardSpi_FramReadStatus(BoardTest_U16 *status)
{
    BoardTest_U16 ok;

    *status = 0x00FFU;
    BoardSpi_SpicDeselectAll();
    DELAY_US(10U);
    BoardSpi_SpicCsLow(BOARD_PIN_SPIC_FRAM_CS);
    DELAY_US(2U);

    (void)BoardSpi_SpicTransferByte(BOARD_SPIC_CMD_RDSR, &ok);
    if(ok == 0U)
    {
        BoardSpi_SpicDeselectAll();
        return 0U;
    }

    *status = BoardSpi_SpicTransferByte(0x00U, &ok);
    DELAY_US(2U);
    BoardSpi_SpicDeselectAll();
    return ok;
}

static void BoardSpi_FramWriteEnable(BoardTest_U16 csDelayUs)
{
    BoardTest_U16 ok;

    BoardSpi_SpicDeselectAll();
    DELAY_US(10U);
    BoardSpi_SpicCsLow(BOARD_PIN_SPIC_FRAM_CS);
    BoardSpi_SpicDelayUs(csDelayUs);
    (void)BoardSpi_SpicTransferByte(BOARD_SPIC_CMD_WREN, &ok);
    BoardSpi_SpicDelayUs(csDelayUs);
    BoardSpi_SpicDeselectAll();
}

static BoardTest_U16 BoardSpi_FramReadByte(BoardTest_U32 addr,
                                           BoardTest_U16 csDelayUs,
                                           BoardTest_U16 *ok)
{
    BoardTest_U16 data;

    *ok = 1U;
    BoardSpi_SpicDeselectAll();
    DELAY_US(10U);
    BoardSpi_SpicCsLow(BOARD_PIN_SPIC_FRAM_CS);
    BoardSpi_SpicDelayUs(csDelayUs);

    (void)BoardSpi_SpicTransferByte(BOARD_SPIC_CMD_READ, ok);
    if(*ok == 0U)
    {
        BoardSpi_SpicDeselectAll();
        return 0U;
    }

    if(BOARD_SPIC_FRAM_ADDR_BYTES == 3U)
    {
        (void)BoardSpi_SpicTransferByte((BoardTest_U16)(addr >> 16U),
                                        ok);
        if(*ok == 0U)
        {
            BoardSpi_SpicDeselectAll();
            return 0U;
        }
    }

    (void)BoardSpi_SpicTransferByte((BoardTest_U16)(addr >> 8U), ok);
    if(*ok == 0U)
    {
        BoardSpi_SpicDeselectAll();
        return 0U;
    }

    (void)BoardSpi_SpicTransferByte((BoardTest_U16)addr, ok);
    if(*ok == 0U)
    {
        BoardSpi_SpicDeselectAll();
        return 0U;
    }

    data = BoardSpi_SpicTransferByte(0x00U, ok);
    BoardSpi_SpicDelayUs(csDelayUs);
    BoardSpi_SpicDeselectAll();
    return data;
}

static BoardTest_U16 BoardSpi_FramWriteByte(BoardTest_U32 addr,
                                            BoardTest_U16 data,
                                            BoardTest_U16 csDelayUs)
{
    BoardTest_U16 ok;

    BoardSpi_FramWriteEnable(csDelayUs);
    DELAY_US(10U);

    BoardSpi_SpicDeselectAll();
    DELAY_US(10U);
    BoardSpi_SpicCsLow(BOARD_PIN_SPIC_FRAM_CS);
    BoardSpi_SpicDelayUs(csDelayUs);

    (void)BoardSpi_SpicTransferByte(BOARD_SPIC_CMD_WRITE, &ok);
    if(ok == 0U)
    {
        BoardSpi_SpicDeselectAll();
        return 0U;
    }

    if(BOARD_SPIC_FRAM_ADDR_BYTES == 3U)
    {
        (void)BoardSpi_SpicTransferByte((BoardTest_U16)(addr >> 16U),
                                        &ok);
        if(ok == 0U)
        {
            BoardSpi_SpicDeselectAll();
            return 0U;
        }
    }

    (void)BoardSpi_SpicTransferByte((BoardTest_U16)(addr >> 8U), &ok);
    if(ok == 0U)
    {
        BoardSpi_SpicDeselectAll();
        return 0U;
    }

    (void)BoardSpi_SpicTransferByte((BoardTest_U16)addr, &ok);
    if(ok == 0U)
    {
        BoardSpi_SpicDeselectAll();
        return 0U;
    }

    (void)BoardSpi_SpicTransferByte(data, &ok);
    BoardSpi_SpicDelayUs(csDelayUs);
    BoardSpi_SpicDeselectAll();
    return ok;
}

static BoardTest_U16 BoardSpi_FramHardwareReadWrite(BoardTest_U16 *oldData,
                                                    BoardTest_U16 *testData,
                                                    BoardTest_U16 *restoreData)
{
    BoardTest_U16 ok;

    *oldData = BoardSpi_FramReadByte(BOARD_SPIC_FRAM_TEST_ADDR,
                                     BOARD_SPIC_FRAM_CS_DELAY_US,
                                     &ok);
    if(ok == 0U)
    {
        return 0U;
    }

    if(BoardSpi_FramWriteByte(BOARD_SPIC_FRAM_TEST_ADDR,
                              BOARD_SPIC_FRAM_TEST_PATTERN,
                              BOARD_SPIC_FRAM_CS_DELAY_US) == 0U)
    {
        return 0U;
    }

    *testData = BoardSpi_FramReadByte(BOARD_SPIC_FRAM_TEST_ADDR,
                                      BOARD_SPIC_FRAM_CS_DELAY_US,
                                      &ok);
    if(ok == 0U)
    {
        return 0U;
    }

    if(BoardSpi_FramWriteByte(BOARD_SPIC_FRAM_TEST_ADDR,
                              *oldData,
                              BOARD_SPIC_FRAM_CS_DELAY_US) == 0U)
    {
        return 0U;
    }

    *restoreData = BoardSpi_FramReadByte(BOARD_SPIC_FRAM_TEST_ADDR,
                                         BOARD_SPIC_FRAM_CS_DELAY_US,
                                         &ok);
    if(ok == 0U)
    {
        return 0U;
    }

    return ((*testData == BOARD_SPIC_FRAM_TEST_PATTERN) &&
            (*restoreData == *oldData)) ? 1U : 0U;
}

static void BoardSpi_UpdateExternalSnapshot(BoardTest_U16 statusMask,
                                            BoardTest_U16 failCode,
                                            const BoardTest_U16 *dspFlashId,
                                            BoardTest_U16 framStatus,
                                            const BoardTest_U16 *framId,
                                            BoardTest_U16 framOriginal,
                                            BoardTest_U16 framReadback,
                                            BoardTest_U16 framRestored,
                                            const BoardTest_U16 *fpgaFlashId)
{
    gBoardSpiExternalSnapshot.statusMask = statusMask;
    gBoardSpiExternalSnapshot.failCode = failCode;
    gBoardSpiExternalSnapshot.dspFlashId0 = dspFlashId[0];
    gBoardSpiExternalSnapshot.dspFlashId1 = dspFlashId[1];
    gBoardSpiExternalSnapshot.dspFlashId2 = dspFlashId[2];
    gBoardSpiExternalSnapshot.framStatus = framStatus;
    gBoardSpiExternalSnapshot.framId6 = framId[6];
    gBoardSpiExternalSnapshot.framId7 = framId[7];
    gBoardSpiExternalSnapshot.framId8 = framId[8];
    gBoardSpiExternalSnapshot.framOriginal = framOriginal;
    gBoardSpiExternalSnapshot.framReadback = framReadback;
    gBoardSpiExternalSnapshot.framRestored = framRestored;
    gBoardSpiExternalSnapshot.fpgaFlashId0 = fpgaFlashId[0];
    gBoardSpiExternalSnapshot.fpgaFlashId1 = fpgaFlashId[1];
    gBoardSpiExternalSnapshot.fpgaFlashId2 = fpgaFlashId[2];
    gBoardSpiExternalSnapshot.sdReserved = 1U;
}

BoardTest_Result BoardSpi_RunSpicExternalTest(BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 dspFlashId[3];
    BoardTest_U16 fpgaFlashId[3];
    BoardTest_U16 framId[9];
    BoardTest_U16 framStatus;
    BoardTest_U16 framOriginal;
    BoardTest_U16 framReadback;
    BoardTest_U16 framRestored;
    BoardTest_U16 ok;
    BoardTest_U16 index;

    statusMask = 0U;
    failCode = BOARD_SPI_EXTERNAL_FAIL_NONE;
    framStatus = 0x00FFU;
    framOriginal = 0U;
    framReadback = 0U;
    framRestored = 0U;
    for(index = 0U; index < 3U; index++)
    {
        dspFlashId[index] = 0U;
        fpgaFlashId[index] = 0U;
    }
    for(index = 0U; index < 9U; index++)
    {
        framId[index] = 0U;
    }

    BoardSpi_InitSpicGpio();
    BoardSpi_InitSpicDefault();
    statusMask |= BOARD_SPI_EXTERNAL_CONFIGURED;
    statusMask |= BOARD_SPI_EXTERNAL_SD_RESERVED;

    ok = BoardSpi_SpicReadBytesAfterCommand(BOARD_PIN_SPIC_FLASH_CS,
                                            BOARD_SPIC_CMD_RDID,
                                            dspFlashId,
                                            3U,
                                            2U);
    if((ok != 0U) && (BoardSpi_FlashIdIsW25Q64(dspFlashId) != 0U))
    {
        statusMask |= BOARD_SPI_EXTERNAL_DSP_FLASH_ID;
    }
    else if(failCode == BOARD_SPI_EXTERNAL_FAIL_NONE)
    {
        failCode = BOARD_SPI_EXTERNAL_FAIL_DSP_FLASH;
    }

    BoardSpi_FramWake();
    BoardSpi_InitSpicMode(BOARD_SPIC_FRAM_POLARITY,
                          BOARD_SPIC_FRAM_PHASE,
                          BOARD_SPIC_FRAM_BRR);

    ok = BoardSpi_FramReadStatus(&framStatus);
    if((ok != 0U) && (framStatus != 0x00FFU))
    {
        statusMask |= BOARD_SPI_EXTERNAL_FRAM_STATUS;
    }
    else if(failCode == BOARD_SPI_EXTERNAL_FAIL_NONE)
    {
        failCode = BOARD_SPI_EXTERNAL_FAIL_FRAM_STATUS;
    }

    ok = BoardSpi_SpicReadBytesAfterCommand(BOARD_PIN_SPIC_FRAM_CS,
                                            BOARD_SPIC_CMD_RDID,
                                            framId,
                                            9U,
                                            BOARD_SPIC_FRAM_CS_DELAY_US);
    if((ok != 0U) && (BoardSpi_FramIdIsFm25v20a(framId) != 0U))
    {
        statusMask |= BOARD_SPI_EXTERNAL_FRAM_ID;
    }
    else if(failCode == BOARD_SPI_EXTERNAL_FAIL_NONE)
    {
        failCode = BOARD_SPI_EXTERNAL_FAIL_FRAM_ID;
    }

    if(BoardSpi_FramHardwareReadWrite(&framOriginal,
                                      &framReadback,
                                      &framRestored) != 0U)
    {
        statusMask |= BOARD_SPI_EXTERNAL_FRAM_RW;
    }
    else if(failCode == BOARD_SPI_EXTERNAL_FAIL_NONE)
    {
        failCode = BOARD_SPI_EXTERNAL_FAIL_FRAM_RW;
    }

    BoardSpi_InitSpicDefault();
    ok = BoardSpi_SpicReadBytesAfterCommand(BOARD_PIN_SPIC_FPGA_FLASH_CS,
                                            BOARD_SPIC_CMD_RDID,
                                            fpgaFlashId,
                                            3U,
                                            2U);
    if((ok != 0U) && (BoardSpi_SpiIdLooksValid(fpgaFlashId, 3U) != 0U))
    {
        statusMask |= BOARD_SPI_EXTERNAL_FPGA_FLASH_ID;
    }
    else if(failCode == BOARD_SPI_EXTERNAL_FAIL_NONE)
    {
        failCode = BOARD_SPI_EXTERNAL_FAIL_FPGA_FLASH;
    }

    if((SpicRegs.SPISTS.bit.OVERRUN_FLAG == 0U) &&
       (SpicRegs.SPIFFRX.bit.RXFFOVF == 0U))
    {
        statusMask |= BOARD_SPI_EXTERNAL_NO_OVERRUN;
    }

    BoardSpi_SpicDeselectAll();
    BoardSpi_UpdateExternalSnapshot(statusMask,
                                    failCode,
                                    dspFlashId,
                                    framStatus,
                                    framId,
                                    framOriginal,
                                    framReadback,
                                    framRestored,
                                    fpgaFlashId);

    return BoardSpi_EvaluateExternalStatus(statusMask, failCode, record);
}
#endif
