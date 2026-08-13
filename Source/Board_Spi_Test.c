#include "Board_Spi_Test.h"
#include "Board_Profile.h"

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

volatile BoardSpi_FlashSnapshot gBoardSpiFlashSnapshot =
{
    0U,
    BOARD_SPIB_FLASH_FAIL_NONE,
    0U,
    0U,
    0U,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED
};

volatile BoardSpi_FramSnapshot gBoardSpiFramSnapshot =
{
    0U,
    BOARD_SPIB_FRAM_FAIL_NONE,
    0U,
    0U,
    0U,
    0U,
    0U,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_SPIB_FRAM_TEST_ADDRESS,
    0U,
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

BoardTest_Result BoardSpi_EvaluateSpibFlashStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 manufacturerId,
    BoardTest_U16 memoryType,
    BoardTest_U16 capacityCode,
    BoardTest_Record *record)
{
    record->rawValue =
        (((BoardTest_U32)statusMask & 0x00FFUL) << 24U) |
        (((BoardTest_U32)manufacturerId & 0x00FFUL) << 16U) |
        (((BoardTest_U32)memoryType & 0x00FFUL) << 8U) |
        ((BoardTest_U32)capacityCode & 0x00FFUL);
    record->measuredValue = (float)capacityCode;
    record->expectedMin = (float)BOARD_SPIB_FLASH_CAPACITY_64_MBIT;
    record->expectedMax = (float)BOARD_SPIB_FLASH_CAPACITY_64_MBIT;

    if(((statusMask & BOARD_SPIB_FLASH_REQUIRED_MASK) ==
        BOARD_SPIB_FLASH_REQUIRED_MASK) &&
       (failCode == BOARD_SPIB_FLASH_FAIL_NONE))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_SPIB_FLASH;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardSpi_EvaluateSpibFramStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 statusRegister,
    BoardTest_U16 manufacturerId,
    BoardTest_U16 productIdHigh,
    BoardTest_U16 productIdLow,
    BoardTest_Record *record)
{
    (void)statusRegister;
    (void)manufacturerId;
    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)failCode & 0xFFFFUL);
    record->measuredValue = (float)
        ((((BoardTest_U32)productIdHigh & 0x00FFUL) << 8U) |
         ((BoardTest_U32)productIdLow & 0x00FFUL));
    record->expectedMin = (float)BOARD_SPIB_FRAM_PRODUCT_CODE;
    record->expectedMax = (float)BOARD_SPIB_FRAM_PRODUCT_CODE;

    if(((statusMask & BOARD_SPIB_FRAM_REQUIRED_MASK) ==
        BOARD_SPIB_FRAM_REQUIRED_MASK) &&
       (failCode == BOARD_SPIB_FRAM_FAIL_NONE))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_SPIB_FRAM;
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
#define BOARD_SPIB_EXTERNAL_POLARITY     1U
#define BOARD_SPIB_EXTERNAL_PHASE        0U

#define BOARD_SPIC_CMD_WREN              0x06U
#define BOARD_SPIC_CMD_WRDI              0x04U
#define BOARD_SPIC_CMD_RDSR              0x05U
#define BOARD_SPIC_CMD_READ              0x03U
#define BOARD_SPIC_CMD_WRITE             0x02U
#define BOARD_SPIC_CMD_RDID              0x9FU

#define BOARD_SPIC_FRAM_TEST_ADDR        0x00000000UL
#define BOARD_SPIC_FRAM_ADDR_BYTES       3U
#define BOARD_SPIC_FRAM_TEST_PATTERN     0xA5U

#if BOARD_SPIB_FRAM_TEST_ADDRESS > BOARD_SPIB_FRAM_MAX_ADDRESS
#error BOARD_SPIB_FRAM_TEST_ADDRESS is outside the FM25V20A address range
#endif

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

static BoardTest_U16 BoardSpi_SpibFlashPinsAreValid(
    const BoardProfile_PinMap *pins)
{
    if((pins->spibSimo == BOARD_PROFILE_PIN_UNUSED) ||
       (pins->spibSomi == BOARD_PROFILE_PIN_UNUSED) ||
       (pins->spibClock == BOARD_PROFILE_PIN_UNUSED) ||
       (pins->spibFlashChipSelect == BOARD_PROFILE_PIN_UNUSED) ||
       (pins->spibMux == BOARD_PROFILE_PIN_UNUSED))
    {
        return 0U;
    }

    return 1U;
}

static BoardTest_U16 BoardSpi_SpibFramPinsAreValid(
    const BoardProfile_PinMap *pins)
{
    if((pins->spibSimo == BOARD_PROFILE_PIN_UNUSED) ||
       (pins->spibSomi == BOARD_PROFILE_PIN_UNUSED) ||
       (pins->spibClock == BOARD_PROFILE_PIN_UNUSED) ||
       (pins->spibFramChipSelect == BOARD_PROFILE_PIN_UNUSED) ||
       (pins->spibMux == BOARD_PROFILE_PIN_UNUSED))
    {
        return 0U;
    }

    return 1U;
}

static void BoardSpi_InitSpibExternalBus(
    const BoardProfile_PinMap *pins)
{
    GPIO_SetupPinMux(pins->spibSimo, GPIO_MUX_CPU1, pins->spibMux);
    GPIO_SetupPinOptions(pins->spibSimo, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(pins->spibSomi, GPIO_MUX_CPU1, pins->spibMux);
    GPIO_SetupPinOptions(pins->spibSomi, GPIO_INPUT,
                         GPIO_ASYNC | GPIO_PULLUP);
    GPIO_SetupPinMux(pins->spibClock, GPIO_MUX_CPU1, pins->spibMux);
    GPIO_SetupPinOptions(pins->spibClock, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(pins->spibFlashChipSelect, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(pins->spibFlashChipSelect, GPIO_OUTPUT,
                         GPIO_PUSHPULL | GPIO_PULLUP);
    GPIO_WritePin(pins->spibFlashChipSelect, 1U);
    if(pins->spibFramChipSelect != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_SetupPinMux(pins->spibFramChipSelect, GPIO_MUX_CPU1, 0U);
        GPIO_SetupPinOptions(pins->spibFramChipSelect, GPIO_OUTPUT,
                             GPIO_PUSHPULL | GPIO_PULLUP);
        GPIO_WritePin(pins->spibFramChipSelect, 1U);
    }

    EALLOW;
    DevCfgRegs.CPUSEL6.bit.SPI_B = 0U;
    CpuSysRegs.PCLKCR8.bit.SPI_B = 1U;
    EDIS;

    SpibRegs.SPICCR.bit.SPISWRESET = 0U;
    /* Match the validated board application: HCLK without delay. */
    SpibRegs.SPICCR.all = 0x0007U |
                          (BOARD_SPIB_EXTERNAL_POLARITY ? 0x0040U : 0x0000U);
    SpibRegs.SPICTL.all = 0x0006U |
                          (BOARD_SPIB_EXTERNAL_PHASE ? 0x0008U : 0x0000U);
    SpibRegs.SPIBRR = BOARD_SPIC_DEFAULT_BRR;
    SpibRegs.SPIFFTX.all = 0xE040U;
    SpibRegs.SPIFFRX.all = 0x2044U;
    SpibRegs.SPIFFCT.all = 0x0000U;
    SpibRegs.SPIPRI.bit.FREE = 1U;
    SpibRegs.SPIFFTX.bit.TXFIFO = 0U;
    SpibRegs.SPIFFRX.bit.RXFIFORESET = 0U;
    SpibRegs.SPIFFTX.bit.TXFIFO = 1U;
    SpibRegs.SPIFFRX.bit.RXFIFORESET = 1U;
    SpibRegs.SPIFFRX.bit.RXFFOVFCLR = 1U;
    SpibRegs.SPIFFRX.bit.RXFFINTCLR = 1U;
    SpibRegs.SPICCR.bit.SPISWRESET = 1U;
}

static BoardTest_U16 BoardSpi_SpibTransferByte(BoardTest_U16 data,
                                                BoardTest_U16 *ok)
{
    BoardTest_U32 timeout;

    *ok = 1U;
    timeout = BOARD_SPIC_XFER_TIMEOUT;
    while(SpibRegs.SPISTS.bit.BUFFULL_FLAG != 0U)
    {
        if(timeout-- == 0UL)
        {
            *ok = 0U;
            return 0U;
        }
    }

    SpibRegs.SPITXBUF = (data & 0x00FFU) << 8U;
    timeout = BOARD_SPIC_XFER_TIMEOUT;
    while(SpibRegs.SPIFFRX.bit.RXFFST == 0U)
    {
        if(timeout-- == 0UL)
        {
            *ok = 0U;
            return 0U;
        }
    }

    data = SpibRegs.SPIRXBUF & 0x00FFU;
    SpibRegs.SPIFFRX.bit.RXFFOVFCLR = 1U;
    SpibRegs.SPIFFRX.bit.RXFFINTCLR = 1U;
    return data;
}

static BoardTest_U16 BoardSpi_ReadSpibFlashId(
    BoardTest_U16 chipSelectPin,
    BoardTest_U16 *id)
{
    BoardTest_U16 index;
    BoardTest_U16 ok;

    for(index = 0U; index < 3U; index++)
    {
        id[index] = 0U;
    }

    GPIO_WritePin(chipSelectPin, 0U);
    DELAY_US(2U);
    (void)BoardSpi_SpibTransferByte(BOARD_SPIC_CMD_RDID, &ok);
    if(ok == 0U)
    {
        GPIO_WritePin(chipSelectPin, 1U);
        return 0U;
    }

    for(index = 0U; index < 3U; index++)
    {
        id[index] = BoardSpi_SpibTransferByte(0x00U, &ok);
        if(ok == 0U)
        {
            GPIO_WritePin(chipSelectPin, 1U);
            return 0U;
        }
    }

    DELAY_US(2U);
    GPIO_WritePin(chipSelectPin, 1U);
    return 1U;
}

static BoardTest_U16 BoardSpi_SpibFlashIdIsValid(
    const BoardTest_U16 *id)
{
    return ((id[0] == BOARD_SPIB_FLASH_MANUFACTURER_ID) &&
            (id[1] == BOARD_SPIB_FLASH_MEMORY_TYPE) &&
            (id[2] == BOARD_SPIB_FLASH_CAPACITY_64_MBIT)) ? 1U : 0U;
}

BoardTest_Result BoardSpi_RunSpibFlashExternalTest(BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardProfile_PinMap *pins;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 id[3];
    BoardTest_U16 ok;

    statusMask = 0U;
    failCode = BOARD_SPIB_FLASH_FAIL_NONE;
    id[0] = 0U;
    id[1] = 0U;
    id[2] = 0U;
    hardware = BoardProfile_GetCurrentHardware();
    pins = (hardware != 0) ? &hardware->pins : 0;

    if((pins == 0) || (BoardSpi_SpibFlashPinsAreValid(pins) == 0U))
    {
        failCode = BOARD_SPIB_FLASH_FAIL_PROFILE;
    }
    else
    {
        BoardSpi_InitSpibExternalBus(pins);
        statusMask |= BOARD_SPIB_FLASH_CONFIGURED;
        ok = BoardSpi_ReadSpibFlashId(pins->spibFlashChipSelect, id);
        if(ok != 0U)
        {
            statusMask |= BOARD_SPIB_FLASH_ID_READ;
            if(BoardSpi_SpibFlashIdIsValid(id) != 0U)
            {
                statusMask |= BOARD_SPIB_FLASH_ID_VALID;
            }
            else
            {
                failCode = BOARD_SPIB_FLASH_FAIL_ID;
            }
        }
        else
        {
            failCode = BOARD_SPIB_FLASH_FAIL_TRANSFER;
        }

        if((SpibRegs.SPISTS.bit.OVERRUN_FLAG == 0U) &&
           (SpibRegs.SPIFFRX.bit.RXFFOVF == 0U))
        {
            statusMask |= BOARD_SPIB_FLASH_NO_OVERRUN;
        }
        GPIO_WritePin(pins->spibFlashChipSelect, 1U);
    }

    gBoardSpiFlashSnapshot.statusMask = statusMask;
    gBoardSpiFlashSnapshot.failCode = failCode;
    gBoardSpiFlashSnapshot.manufacturerId = id[0];
    gBoardSpiFlashSnapshot.memoryType = id[1];
    gBoardSpiFlashSnapshot.capacityCode = id[2];
    gBoardSpiFlashSnapshot.simoPin =
        (pins != 0) ? pins->spibSimo : BOARD_PROFILE_PIN_UNUSED;
    gBoardSpiFlashSnapshot.somiPin =
        (pins != 0) ? pins->spibSomi : BOARD_PROFILE_PIN_UNUSED;
    gBoardSpiFlashSnapshot.clockPin =
        (pins != 0) ? pins->spibClock : BOARD_PROFILE_PIN_UNUSED;
    gBoardSpiFlashSnapshot.chipSelectPin =
        (pins != 0) ? pins->spibFlashChipSelect : BOARD_PROFILE_PIN_UNUSED;

    return BoardSpi_EvaluateSpibFlashStatus(statusMask,
                                            failCode,
                                            id[0],
                                            id[1],
                                            id[2],
                                            record);
}

static void BoardSpi_SpibDeselectAll(const BoardProfile_PinMap *pins)
{
    if(pins->spibFlashChipSelect != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_WritePin(pins->spibFlashChipSelect, 1U);
    }
    if(pins->spibFramChipSelect != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_WritePin(pins->spibFramChipSelect, 1U);
    }
}

static void BoardSpi_SpibFramWake(const BoardProfile_PinMap *pins)
{
    BoardTest_U16 ok;

    BoardSpi_SpibDeselectAll(pins);
    DELAY_US(1000U);
    GPIO_WritePin(pins->spibFramChipSelect, 0U);
    DELAY_US(2U);
    (void)BoardSpi_SpibTransferByte(0x00FFU, &ok);
    DELAY_US(2U);
    BoardSpi_SpibDeselectAll(pins);
    DELAY_US(1000U);
}

static BoardTest_U16 BoardSpi_ReadSpibFramStatus(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 *statusRegister)
{
    BoardTest_U16 ok;

    *statusRegister = 0x00FFU;
    BoardSpi_SpibDeselectAll(pins);
    GPIO_WritePin(pins->spibFramChipSelect, 0U);
    DELAY_US(2U);
    (void)BoardSpi_SpibTransferByte(BOARD_SPIC_CMD_RDSR, &ok);
    if(ok != 0U)
    {
        *statusRegister = BoardSpi_SpibTransferByte(0x0000U, &ok);
    }
    DELAY_US(2U);
    BoardSpi_SpibDeselectAll(pins);
    return ok;
}

static BoardTest_U16 BoardSpi_ReadSpibFramId(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 *id)
{
    BoardTest_U16 index;
    BoardTest_U16 ok;

    for(index = 0U; index < 9U; index++)
    {
        id[index] = 0U;
    }

    BoardSpi_SpibDeselectAll(pins);
    GPIO_WritePin(pins->spibFramChipSelect, 0U);
    DELAY_US(2U);
    (void)BoardSpi_SpibTransferByte(BOARD_SPIC_CMD_RDID, &ok);
    if(ok == 0U)
    {
        BoardSpi_SpibDeselectAll(pins);
        return 0U;
    }

    for(index = 0U; index < 9U; index++)
    {
        id[index] = BoardSpi_SpibTransferByte(0x0000U, &ok);
        if(ok == 0U)
        {
            BoardSpi_SpibDeselectAll(pins);
            return 0U;
        }
    }

    DELAY_US(2U);
    BoardSpi_SpibDeselectAll(pins);
    return 1U;
}

static BoardTest_U16 BoardSpi_SpibFramIdIsFm25v20a(
    const BoardTest_U16 *id)
{
    BoardTest_U16 index;

    for(index = 0U; index < 6U; index++)
    {
        if(id[index] != 0x007FU)
        {
            return 0U;
        }
    }

    return ((id[6] == BOARD_SPIB_FRAM_MANUFACTURER_ID) &&
            (id[7] == BOARD_SPIB_FRAM_PRODUCT_ID_HIGH) &&
            (id[8] == BOARD_SPIB_FRAM_PRODUCT_ID_LOW)) ? 1U : 0U;
}

static BoardTest_U16 BoardSpi_SpibFramSendCommand(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 command)
{
    BoardTest_U16 ok;

    BoardSpi_SpibDeselectAll(pins);
    GPIO_WritePin(pins->spibFramChipSelect, 0U);
    DELAY_US(2U);
    (void)BoardSpi_SpibTransferByte(command, &ok);
    DELAY_US(2U);
    BoardSpi_SpibDeselectAll(pins);
    return ok;
}

static BoardTest_U16 BoardSpi_SpibFramSendAddress(BoardTest_U32 address)
{
    BoardTest_U16 ok;

    (void)BoardSpi_SpibTransferByte(
        (BoardTest_U16)((address >> 16U) & 0x00FFUL), &ok);
    if(ok == 0U)
    {
        return 0U;
    }
    (void)BoardSpi_SpibTransferByte(
        (BoardTest_U16)((address >> 8U) & 0x00FFUL), &ok);
    if(ok == 0U)
    {
        return 0U;
    }
    (void)BoardSpi_SpibTransferByte(
        (BoardTest_U16)(address & 0x00FFUL), &ok);
    return ok;
}

static BoardTest_U16 BoardSpi_SpibFramReadByte(
    const BoardProfile_PinMap *pins,
    BoardTest_U32 address,
    BoardTest_U16 *value)
{
    BoardTest_U16 ok;

    *value = 0U;
    BoardSpi_SpibDeselectAll(pins);
    GPIO_WritePin(pins->spibFramChipSelect, 0U);
    DELAY_US(2U);
    (void)BoardSpi_SpibTransferByte(BOARD_SPIC_CMD_READ, &ok);
    if((ok == 0U) || (BoardSpi_SpibFramSendAddress(address) == 0U))
    {
        BoardSpi_SpibDeselectAll(pins);
        return 0U;
    }

    *value = BoardSpi_SpibTransferByte(0x0000U, &ok);
    DELAY_US(2U);
    BoardSpi_SpibDeselectAll(pins);
    return ok;
}

static BoardTest_U16 BoardSpi_SpibFramWriteByte(
    const BoardProfile_PinMap *pins,
    BoardTest_U32 address,
    BoardTest_U16 value)
{
    BoardTest_U16 ok;

    BoardSpi_SpibDeselectAll(pins);
    GPIO_WritePin(pins->spibFramChipSelect, 0U);
    DELAY_US(2U);
    (void)BoardSpi_SpibTransferByte(BOARD_SPIC_CMD_WRITE, &ok);
    if((ok == 0U) || (BoardSpi_SpibFramSendAddress(address) == 0U))
    {
        BoardSpi_SpibDeselectAll(pins);
        return 0U;
    }

    (void)BoardSpi_SpibTransferByte(value & 0x00FFU, &ok);
    DELAY_US(2U);
    BoardSpi_SpibDeselectAll(pins);
    DELAY_US(2U);
    return ok;
}

static BoardTest_U16 BoardSpi_SpibFramWriteEnable(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 *statusRegister)
{
    if(BoardSpi_SpibFramSendCommand(pins, BOARD_SPIC_CMD_WREN) == 0U)
    {
        return 0U;
    }
    if(BoardSpi_ReadSpibFramStatus(pins, statusRegister) == 0U)
    {
        return 0U;
    }
    return ((*statusRegister & BOARD_SPIB_FRAM_STATUS_WEL) != 0U) ? 1U : 0U;
}

static BoardTest_U16 BoardSpi_SpibFramWriteDisable(
    const BoardProfile_PinMap *pins,
    BoardTest_U16 *statusRegister)
{
    if(BoardSpi_SpibFramSendCommand(pins, BOARD_SPIC_CMD_WRDI) == 0U)
    {
        return 0U;
    }
    if(BoardSpi_ReadSpibFramStatus(pins, statusRegister) == 0U)
    {
        return 0U;
    }
    return ((*statusRegister & BOARD_SPIB_FRAM_STATUS_WEL) == 0U) ? 1U : 0U;
}

static BoardTest_U16 BoardSpi_SpibFramAddressIsWritable(
    BoardTest_U16 statusRegister,
    BoardTest_U32 address)
{
    switch(statusRegister & BOARD_SPIB_FRAM_STATUS_BP_MASK)
    {
        case 0x0000U:
            return 1U;

        case 0x0004U:
            return (address < 0x00030000UL) ? 1U : 0U;

        case 0x0008U:
            return (address < 0x00020000UL) ? 1U : 0U;

        default:
            return 0U;
    }
}

BoardTest_Result BoardSpi_RunSpibFramExternalTest(BoardTest_Record *record)
{
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardProfile_PinMap *pins;
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 statusRegister;
    BoardTest_U16 id[9];
    BoardTest_U16 ok;
    BoardTest_U16 index;
    BoardTest_U16 originalValue;
    BoardTest_U16 testValue;
    BoardTest_U16 testReadback;
    BoardTest_U16 restoreReadback;
    BoardTest_U16 statusAfterWriteEnable;
    BoardTest_U16 statusAfterRestoreEnable;
    BoardTest_U16 finalStatusRegister;
    BoardTest_U16 writeAttempted;

    statusMask = 0U;
    failCode = BOARD_SPIB_FRAM_FAIL_NONE;
    statusRegister = 0x00FFU;
    originalValue = 0U;
    testValue = 0U;
    testReadback = 0U;
    restoreReadback = 0U;
    statusAfterWriteEnable = 0x00FFU;
    statusAfterRestoreEnable = 0x00FFU;
    finalStatusRegister = 0x00FFU;
    writeAttempted = 0U;
    for(index = 0U; index < 9U; index++)
    {
        id[index] = 0U;
    }

    hardware = BoardProfile_GetCurrentHardware();
    pins = (hardware != 0) ? &hardware->pins : 0;
    if((pins == 0) || (BoardSpi_SpibFramPinsAreValid(pins) == 0U))
    {
        failCode = BOARD_SPIB_FRAM_FAIL_PROFILE;
    }
    else
    {
        BoardSpi_InitSpibExternalBus(pins);
        BoardSpi_SpibDeselectAll(pins);
        statusMask |= BOARD_SPIB_FRAM_CONFIGURED;
        BoardSpi_SpibFramWake(pins);

        ok = BoardSpi_ReadSpibFramStatus(pins, &statusRegister);
        if((ok != 0U) && (statusRegister != 0x00FFU))
        {
            statusMask |= BOARD_SPIB_FRAM_STATUS_READ;
            if(BoardSpi_SpibFramAddressIsWritable(
                   statusRegister, BOARD_SPIB_FRAM_TEST_ADDRESS) != 0U)
            {
                statusMask |= BOARD_SPIB_FRAM_ADDRESS_WRITABLE;
            }
            else
            {
                failCode = BOARD_SPIB_FRAM_FAIL_PROTECTED;
            }
        }
        else
        {
            failCode = BOARD_SPIB_FRAM_FAIL_STATUS;
        }

        ok = BoardSpi_ReadSpibFramId(pins, id);
        if(ok != 0U)
        {
            statusMask |= BOARD_SPIB_FRAM_ID_READ;
            if(BoardSpi_SpibFramIdIsFm25v20a(id) != 0U)
            {
                statusMask |= BOARD_SPIB_FRAM_ID_VALID;
            }
            else if(failCode == BOARD_SPIB_FRAM_FAIL_NONE)
            {
                failCode = BOARD_SPIB_FRAM_FAIL_ID;
            }
        }
        else if(failCode == BOARD_SPIB_FRAM_FAIL_NONE)
        {
            failCode = BOARD_SPIB_FRAM_FAIL_TRANSFER;
        }

        if(failCode == BOARD_SPIB_FRAM_FAIL_NONE)
        {
            ok = BoardSpi_SpibFramReadByte(
                pins, BOARD_SPIB_FRAM_TEST_ADDRESS, &originalValue);
            if(ok != 0U)
            {
                statusMask |= BOARD_SPIB_FRAM_ORIGINAL_READ;
                testValue = (originalValue ^
                             BOARD_SPIB_FRAM_TEST_XOR_PATTERN) & 0x00FFU;
            }
            else
            {
                failCode = BOARD_SPIB_FRAM_FAIL_ORIGINAL_READ;
            }
        }

        if(failCode == BOARD_SPIB_FRAM_FAIL_NONE)
        {
            ok = BoardSpi_SpibFramWriteEnable(
                pins, &statusAfterWriteEnable);
            if(ok != 0U)
            {
                statusMask |= BOARD_SPIB_FRAM_WRITE_ENABLED;
                writeAttempted = 1U;
                ok = BoardSpi_SpibFramWriteByte(
                    pins, BOARD_SPIB_FRAM_TEST_ADDRESS, testValue);
                if(ok != 0U)
                {
                    statusMask |= BOARD_SPIB_FRAM_TEST_WRITTEN;
                }
                else
                {
                    failCode = BOARD_SPIB_FRAM_FAIL_WRITE;
                }
            }
            else
            {
                failCode = BOARD_SPIB_FRAM_FAIL_WRITE_ENABLE;
            }
        }

        if(writeAttempted != 0U)
        {
            ok = BoardSpi_SpibFramReadByte(
                pins, BOARD_SPIB_FRAM_TEST_ADDRESS, &testReadback);
            if((ok != 0U) && (testReadback == testValue))
            {
                statusMask |= BOARD_SPIB_FRAM_TEST_READBACK;
            }
            else if(failCode == BOARD_SPIB_FRAM_FAIL_NONE)
            {
                failCode = BOARD_SPIB_FRAM_FAIL_TEST_READBACK;
            }

            ok = BoardSpi_SpibFramWriteEnable(
                pins, &statusAfterRestoreEnable);
            if(ok != 0U)
            {
                ok = BoardSpi_SpibFramWriteByte(
                    pins, BOARD_SPIB_FRAM_TEST_ADDRESS, originalValue);
                if(ok != 0U)
                {
                    statusMask |= BOARD_SPIB_FRAM_RESTORE_WRITTEN;
                }
                else
                {
                    failCode = BOARD_SPIB_FRAM_FAIL_RESTORE_WRITE;
                }
            }
            else
            {
                failCode = BOARD_SPIB_FRAM_FAIL_RESTORE_WRITE;
            }

            ok = BoardSpi_SpibFramReadByte(
                pins, BOARD_SPIB_FRAM_TEST_ADDRESS, &restoreReadback);
            if((ok != 0U) && (restoreReadback == originalValue))
            {
                statusMask |= BOARD_SPIB_FRAM_RESTORE_VERIFIED;
            }
            else
            {
                failCode = BOARD_SPIB_FRAM_FAIL_RESTORE_VERIFY;
            }
        }

        ok = BoardSpi_SpibFramWriteDisable(pins, &finalStatusRegister);
        if(ok != 0U)
        {
            statusMask |= BOARD_SPIB_FRAM_WRITE_DISABLED;
        }
        else if(failCode == BOARD_SPIB_FRAM_FAIL_NONE)
        {
            failCode = BOARD_SPIB_FRAM_FAIL_WRITE_DISABLE;
        }

        if((SpibRegs.SPISTS.bit.OVERRUN_FLAG == 0U) &&
           (SpibRegs.SPIFFRX.bit.RXFFOVF == 0U))
        {
            statusMask |= BOARD_SPIB_FRAM_NO_OVERRUN;
        }
        else if(failCode == BOARD_SPIB_FRAM_FAIL_NONE)
        {
            failCode = BOARD_SPIB_FRAM_FAIL_TRANSFER;
        }
        BoardSpi_SpibDeselectAll(pins);
    }

    gBoardSpiFramSnapshot.statusMask = statusMask;
    gBoardSpiFramSnapshot.failCode = failCode;
    gBoardSpiFramSnapshot.statusRegister = statusRegister;
    gBoardSpiFramSnapshot.manufacturerId = id[6];
    gBoardSpiFramSnapshot.productIdHigh = id[7];
    gBoardSpiFramSnapshot.productIdLow = id[8];
    gBoardSpiFramSnapshot.continuationCount = 0U;
    for(index = 0U; index < 6U; index++)
    {
        if(id[index] == 0x007FU)
        {
            gBoardSpiFramSnapshot.continuationCount++;
        }
    }
    gBoardSpiFramSnapshot.chipSelectPin =
        (pins != 0) ? pins->spibFramChipSelect : BOARD_PROFILE_PIN_UNUSED;
    gBoardSpiFramSnapshot.testAddress = BOARD_SPIB_FRAM_TEST_ADDRESS;
    gBoardSpiFramSnapshot.originalValue = originalValue;
    gBoardSpiFramSnapshot.testValue = testValue;
    gBoardSpiFramSnapshot.testReadback = testReadback;
    gBoardSpiFramSnapshot.restoreReadback = restoreReadback;
    gBoardSpiFramSnapshot.statusAfterWriteEnable = statusAfterWriteEnable;
    gBoardSpiFramSnapshot.statusAfterRestoreEnable = statusAfterRestoreEnable;
    gBoardSpiFramSnapshot.finalStatusRegister = finalStatusRegister;

    return BoardSpi_EvaluateSpibFramStatus(statusMask,
                                           failCode,
                                           statusRegister,
                                           id[6],
                                           id[7],
                                           id[8],
                                           record);
}
#endif
