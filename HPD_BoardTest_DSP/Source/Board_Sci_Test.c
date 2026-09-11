#include "Board_Sci_Test.h"
#include "Board_Profile.h"

BoardTest_U16 BoardSci_ExternalDeadlineExpired(BoardTest_U32 start,
                                              BoardTest_U32 now)
{
    return (((start - now) & 0xFFFFFFFFUL) >=
            BOARD_SCI_EXTERNAL_TIMEOUT_US) ? 1U : 0U;
}

volatile BoardSci_ScibPinSnapshot gBoardSciScibPinSnapshot =
{
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_ID_NONE,
    BOARD_PROFILE_HARDWARE_REVISION_NONE,
    BOARD_PROFILE_ETHERNET_NONE,
    9600U,
    0U
};

volatile BoardSci_Rs422PinSnapshot gBoardSciRs422PinSnapshot =
{
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_PIN_UNUSED,
    BOARD_PROFILE_ID_NONE,
    BOARD_PROFILE_HARDWARE_REVISION_NONE,
    BOARD_PROFILE_ETHERNET_NONE,
    9600U,
    0U
};

BoardTest_U16 BoardSci_ResolveSciaPinConfiguration(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardSci_ScibPinConfiguration *configuration)
{
    if(configuration == 0) return 0U;
    configuration->directionGpio = BOARD_PROFILE_PIN_UNUSED;
    configuration->txGpio = BOARD_PROFILE_PIN_UNUSED;
    configuration->rxGpio = BOARD_PROFILE_PIN_UNUSED;
    configuration->txMux = BOARD_PROFILE_PIN_UNUSED;
    configuration->rxMux = BOARD_PROFILE_PIN_UNUSED;
    if((hardware == 0) ||
       ((hardware->implementedCapabilities & BOARD_PROFILE_CAP_SCIA) == 0UL))
        return 0U;
    /* Both known SCIA routes use mux 6; reject unknown pin tuples. */
    if(!(((hardware->boardId == BOARD_PROFILE_ID_LOW_VOLTAGE_INVERTER) &&
          (hardware->hardwareRevision == BOARD_PROFILE_HARDWARE_REVISION_LOW_VOLTAGE_V04) &&
          (hardware->pins.sciaDirection == 30U) &&
          (hardware->pins.sciaTransmit == 8U) && (hardware->pins.sciaReceive == 9U)) ||
         ((hardware->boardId == BOARD_PROFILE_ID_SYSTEM_MASTER) &&
          (hardware->pins.sciaDirection == 133U) &&
          (hardware->pins.sciaTransmit == 135U) && (hardware->pins.sciaReceive == 136U))))
        return 0U;
    configuration->directionGpio = hardware->pins.sciaDirection;
    configuration->txGpio = hardware->pins.sciaTransmit;
    configuration->rxGpio = hardware->pins.sciaReceive;
    configuration->txMux = 6U;
    configuration->rxMux = 6U;
    return 1U;
}

BoardTest_U16 BoardSci_ResolveScibPinConfiguration(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardSci_ScibPinConfiguration *configuration)
{
    if(configuration == 0)
    {
        return 0U;
    }

    configuration->directionGpio = BOARD_PROFILE_PIN_UNUSED;
    configuration->txGpio = BOARD_PROFILE_PIN_UNUSED;
    configuration->rxGpio = BOARD_PROFILE_PIN_UNUSED;
    configuration->txMux = BOARD_PROFILE_PIN_UNUSED;
    configuration->rxMux = BOARD_PROFILE_PIN_UNUSED;

    if((hardware == 0) ||
       ((hardware->implementedCapabilities & BOARD_PROFILE_CAP_SCIB) == 0UL) ||
       (hardware->pins.scibDirection == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.scibTransmit == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.scibReceive == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.scibTransmitMux == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.scibReceiveMux == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.scibDirection == hardware->pins.scibTransmit) ||
       (hardware->pins.scibDirection == hardware->pins.scibReceive) ||
       (hardware->pins.scibTransmit == hardware->pins.scibReceive) ||
       (hardware->pins.scibTransmitMux > 15U) ||
       (hardware->pins.scibReceiveMux > 15U))
    {
        return 0U;
    }

    configuration->directionGpio = hardware->pins.scibDirection;
    configuration->txGpio = hardware->pins.scibTransmit;
    configuration->rxGpio = hardware->pins.scibReceive;
    configuration->txMux = hardware->pins.scibTransmitMux;
    configuration->rxMux = hardware->pins.scibReceiveMux;
    return 1U;
}

BoardTest_U16 BoardSci_ResolveRs422PinConfiguration(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardSci_Rs422PinConfiguration *configuration)
{
    if(configuration == 0)
    {
        return 0U;
    }

    configuration->txGpio = BOARD_PROFILE_PIN_UNUSED;
    configuration->rxGpio = BOARD_PROFILE_PIN_UNUSED;
    configuration->txMux = BOARD_PROFILE_PIN_UNUSED;
    configuration->rxMux = BOARD_PROFILE_PIN_UNUSED;

    if((hardware == 0) ||
       ((hardware->implementedCapabilities & BOARD_PROFILE_CAP_RS422) == 0UL) ||
       (hardware->pins.rs422Transmit == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.rs422Receive == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.rs422TransmitMux == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.rs422ReceiveMux == BOARD_PROFILE_PIN_UNUSED) ||
       (hardware->pins.rs422Transmit == hardware->pins.rs422Receive) ||
       (hardware->pins.rs422TransmitMux > 15U) ||
       (hardware->pins.rs422ReceiveMux > 15U))
    {
        return 0U;
    }

    configuration->txGpio = hardware->pins.rs422Transmit;
    configuration->rxGpio = hardware->pins.rs422Receive;
    configuration->txMux = hardware->pins.rs422TransmitMux;
    configuration->rxMux = hardware->pins.rs422ReceiveMux;
    return 1U;
}

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

volatile BoardSci_Rs422ExternalSnapshot gBoardSciRs422ExternalSnapshot =
{
    0U,
    0U,
    0U,
    0U
};

volatile BoardSci_SciaHandheldExternalSnapshot
    gBoardSciSciaHandheldExternalSnapshot =
{
    0U,
    0U,
    0U,
    0U,
    0U
};

static BoardTest_U16 BoardSci_Rs485ExternalState =
    BOARD_SCI_RS485_STATE_IDLE;
static BoardTest_U16 BoardSci_SciaDirectionPin = BOARD_PROFILE_PIN_UNUSED;
static BoardTest_U16 BoardSci_SciaHandheldExternalState =
    BOARD_SCI_RS485_STATE_IDLE;
static BoardTest_U16 BoardSci_Rs485StandbyEnabled = 0U;
static BoardTest_U16 BoardSci_Rs485StandbyState =
    BOARD_SCI_RS485_STATE_IDLE;
static BoardTest_U16 BoardSci_Rs422ExternalState =
    BOARD_SCI_RS485_STATE_IDLE;
static BoardTest_U16 BoardSci_Rs422StandbyEnabled = 0U;
static BoardTest_U16 BoardSci_Rs422StandbyState =
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

BoardTest_Result BoardSci_EvaluateRs422ExternalStatus(
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
        (float)BOARD_SCI_RS422_EXTERNAL_REQUIRED_MASK;
    record->expectedMax =
        (float)BOARD_SCI_RS422_EXTERNAL_DIAGNOSTIC_MASK;

    if((statusMask & BOARD_SCI_RS422_EXTERNAL_REQUIRED_MASK) ==
       BOARD_SCI_RS422_EXTERNAL_REQUIRED_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_RS422_EXTERNAL;
    return BOARD_TEST_RESULT_FAIL;
}

BoardTest_Result BoardSci_EvaluateSciaHandheldExternalStatus(
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
        (float)BOARD_SCI_HANDHELD_EXTERNAL_REQUIRED_MASK;
    record->expectedMax =
        (float)BOARD_SCI_HANDHELD_EXTERNAL_DIAGNOSTIC_MASK;

    if((statusMask & BOARD_SCI_HANDHELD_EXTERNAL_REQUIRED_MASK) ==
       BOARD_SCI_HANDHELD_EXTERNAL_REQUIRED_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_SCIA_HANDHELD_EXTERNAL;
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
    const BoardProfile_HardwareDescriptor *hardware;

    hardware = BoardProfile_GetCurrentHardware();
    if((BoardProfile_IsConfirmed() == 0U) || (hardware == 0))
    {
        return;
    }

    if(hardware->pins.sciaDirection != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_SetupPinMux(hardware->pins.sciaDirection, GPIO_MUX_CPU1, 0U);
        GPIO_WritePin(hardware->pins.sciaDirection, 0U);
        GPIO_SetupPinOptions(hardware->pins.sciaDirection, GPIO_OUTPUT, 0U);
    }

    if(hardware->pins.scibDirection != BOARD_PROFILE_PIN_UNUSED)
    {
        GPIO_SetupPinMux(hardware->pins.scibDirection, GPIO_MUX_CPU1, 0U);
        GPIO_WritePin(hardware->pins.scibDirection, 0U);
        GPIO_SetupPinOptions(hardware->pins.scibDirection, GPIO_OUTPUT, 0U);
    }
}

static void BoardSci_SetScibDirection(BoardTest_U16 level)
{
    if(gBoardSciScibPinSnapshot.valid != 0U)
    {
        GPIO_WritePin(gBoardSciScibPinSnapshot.directionGpio, level);
    }
    gBoardSciRs485ExternalSnapshot.directionLevel = level;
}

static void BoardSci_SetSciaDirection(BoardTest_U16 level)
{
    if(BoardSci_SciaDirectionPin != BOARD_PROFILE_PIN_UNUSED)
        GPIO_WritePin(BoardSci_SciaDirectionPin, level);
    gBoardSciSciaHandheldExternalSnapshot.directionLevel = level;
}

static BoardTest_U16 BoardSci_ConfigureScibPins(void)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardSci_ScibPinConfiguration configuration;

    gBoardSciScibPinSnapshot.valid = 0U;
    gBoardSciScibPinSnapshot.directionGpio = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciScibPinSnapshot.txGpio = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciScibPinSnapshot.rxGpio = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciScibPinSnapshot.txMux = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciScibPinSnapshot.rxMux = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciScibPinSnapshot.boardId = BOARD_PROFILE_ID_NONE;
    gBoardSciScibPinSnapshot.hardwareRevision =
        BOARD_PROFILE_HARDWARE_REVISION_NONE;
    gBoardSciScibPinSnapshot.ethernetInterface = BOARD_PROFILE_ETHERNET_NONE;
    gBoardSciScibPinSnapshot.baudRate = 9600U;

    hardware = BoardProfile_GetCurrentHardware();
    if((BoardProfile_IsConfirmed() == 0U) ||
       (BoardSci_ResolveScibPinConfiguration(hardware, &configuration) == 0U))
    {
        return 0U;
    }

    gBoardSciScibPinSnapshot.directionGpio = configuration.directionGpio;
    gBoardSciScibPinSnapshot.txGpio = configuration.txGpio;
    gBoardSciScibPinSnapshot.rxGpio = configuration.rxGpio;
    gBoardSciScibPinSnapshot.txMux = configuration.txMux;
    gBoardSciScibPinSnapshot.rxMux = configuration.rxMux;
    gBoardSciScibPinSnapshot.boardId = hardware->boardId;
    gBoardSciScibPinSnapshot.hardwareRevision = hardware->hardwareRevision;
    gBoardSciScibPinSnapshot.ethernetInterface = hardware->ethernetInterface;

    GPIO_SetupPinMux(gBoardSciScibPinSnapshot.rxGpio,
                     GPIO_MUX_CPU1,
                     gBoardSciScibPinSnapshot.rxMux);
    GPIO_SetupPinOptions(gBoardSciScibPinSnapshot.rxGpio,
                         GPIO_INPUT,
                         GPIO_ASYNC);
    GPIO_SetupPinMux(gBoardSciScibPinSnapshot.txGpio,
                     GPIO_MUX_CPU1,
                     gBoardSciScibPinSnapshot.txMux);
    GPIO_SetupPinOptions(gBoardSciScibPinSnapshot.txGpio,
                         GPIO_OUTPUT,
                         GPIO_ASYNC);
    GPIO_SetupPinMux(gBoardSciScibPinSnapshot.directionGpio,
                     GPIO_MUX_CPU1,
                     0U);
    GPIO_SetupPinOptions(gBoardSciScibPinSnapshot.directionGpio, GPIO_OUTPUT,
                         GPIO_PUSHPULL);
    gBoardSciScibPinSnapshot.valid = 1U;
    BoardSci_SetScibDirection(0U);
    return 1U;
}

static BoardTest_U16 BoardSci_InitScibRs485External(void)
{
    if(BoardSci_ConfigureScibPins() == 0U)
    {
        return 0U;
    }

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
    return 1U;
}

static BoardTest_U16 BoardSci_ConfigureRs422Pins(void)
{
    const BoardProfile_HardwareDescriptor *hardware;
    BoardSci_Rs422PinConfiguration configuration;

    gBoardSciRs422PinSnapshot.valid = 0U;
    gBoardSciRs422PinSnapshot.txGpio = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciRs422PinSnapshot.rxGpio = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciRs422PinSnapshot.txMux = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciRs422PinSnapshot.rxMux = BOARD_PROFILE_PIN_UNUSED;
    gBoardSciRs422PinSnapshot.boardId = BOARD_PROFILE_ID_NONE;
    gBoardSciRs422PinSnapshot.hardwareRevision =
        BOARD_PROFILE_HARDWARE_REVISION_NONE;
    gBoardSciRs422PinSnapshot.ethernetInterface = BOARD_PROFILE_ETHERNET_NONE;
    gBoardSciRs422PinSnapshot.baudRate = 9600U;

    hardware = BoardProfile_GetCurrentHardware();
    if((BoardProfile_IsConfirmed() == 0U) ||
       (BoardSci_ResolveRs422PinConfiguration(hardware, &configuration) == 0U))
    {
        return 0U;
    }

    gBoardSciRs422PinSnapshot.txGpio = configuration.txGpio;
    gBoardSciRs422PinSnapshot.rxGpio = configuration.rxGpio;
    gBoardSciRs422PinSnapshot.txMux = configuration.txMux;
    gBoardSciRs422PinSnapshot.rxMux = configuration.rxMux;
    gBoardSciRs422PinSnapshot.boardId = hardware->boardId;
    gBoardSciRs422PinSnapshot.hardwareRevision = hardware->hardwareRevision;
    gBoardSciRs422PinSnapshot.ethernetInterface = hardware->ethernetInterface;

    GPIO_SetupPinMux(gBoardSciRs422PinSnapshot.rxGpio,
                     GPIO_MUX_CPU1,
                     gBoardSciRs422PinSnapshot.rxMux);
    GPIO_SetupPinOptions(gBoardSciRs422PinSnapshot.rxGpio,
                         GPIO_INPUT,
                         GPIO_ASYNC);
    GPIO_SetupPinMux(gBoardSciRs422PinSnapshot.txGpio,
                     GPIO_MUX_CPU1,
                     gBoardSciRs422PinSnapshot.txMux);
    GPIO_SetupPinOptions(gBoardSciRs422PinSnapshot.txGpio,
                         GPIO_OUTPUT,
                         GPIO_ASYNC);
    gBoardSciRs422PinSnapshot.valid = 1U;
    return 1U;
}

static BoardTest_U16 BoardSci_InitScicRs422External(void)
{
    if(BoardSci_ConfigureRs422Pins() == 0U)
    {
        return 0U;
    }

    ScicRegs.SCICTL1.all = 0x0000U;
    ScicRegs.SCICCR.all = 0x0007U;
    ScicRegs.SCICTL1.all = 0x0003U;
    ScicRegs.SCICTL2.all = 0x0003U;
    ScicRegs.SCICTL2.bit.TXINTENA = 1U;
    ScicRegs.SCICTL2.bit.RXBKINTENA = 1U;
    ScicRegs.SCIHBAUD = BOARD_SCI_BRR_9600_H;
    ScicRegs.SCILBAUD = BOARD_SCI_BRR_9600_L;
    ScicRegs.SCIFFTX.all = 0xE040U;
    ScicRegs.SCIFFRX.all = 0x2044U;
    ScicRegs.SCIFFCT.all = 0x0000U;
    ScicRegs.SCIPRI.bit.FREE = 1U;
    ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;
    ScicRegs.SCIFFRX.bit.RXFFINTCLR = 1U;
    ScicRegs.SCIFFTX.bit.TXFFINTCLR = 1U;
    ScicRegs.SCICTL1.all = 0x0023U;
    return 1U;
}

static BoardTest_U16 BoardSci_InitSciaHandheldExternal(void)
{
    BoardSci_ScibPinConfiguration pins;
    BoardSci_SciaDirectionPin = BOARD_PROFILE_PIN_UNUSED;
    if((BoardProfile_IsConfirmed() == 0U) ||
       (BoardSci_ResolveSciaPinConfiguration(BoardProfile_GetCurrentHardware(), &pins) == 0U))
        return 0U;
    GPIO_SetupPinMux(pins.rxGpio, GPIO_MUX_CPU1, pins.rxMux);
    GPIO_SetupPinOptions(pins.rxGpio, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(pins.txGpio, GPIO_MUX_CPU1, pins.txMux);
    GPIO_SetupPinOptions(pins.txGpio, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_WritePin(pins.directionGpio, 0U);
    GPIO_SetupPinMux(pins.directionGpio, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(pins.directionGpio, GPIO_OUTPUT,
                         GPIO_PUSHPULL);
    BoardSci_SciaDirectionPin = pins.directionGpio;
    BoardSci_SetSciaDirection(0U);

    SciaRegs.SCICTL1.all = 0x0000U;
    SciaRegs.SCICCR.all = 0x0007U;
    SciaRegs.SCICTL1.all = 0x0003U;
    SciaRegs.SCICTL2.all = 0x0003U;
    SciaRegs.SCICTL2.bit.TXINTENA = 1U;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1U;
    SciaRegs.SCIHBAUD = BOARD_SCI_BRR_9600_H;
    SciaRegs.SCILBAUD = BOARD_SCI_BRR_9600_L;
    SciaRegs.SCIFFTX.all = 0xE040U;
    SciaRegs.SCIFFRX.all = 0x2044U;
    SciaRegs.SCIFFCT.all = 0x0000U;
    SciaRegs.SCIPRI.bit.FREE = 1U;
    SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1U;
    SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1U;
    SciaRegs.SCICTL1.all = 0x0023U;
    return 1U;
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

    if(BoardSci_InitScibRs485External() == 0U)
    {
        BoardSci_Rs485ExternalState = BOARD_SCI_RS485_STATE_IDLE;
        record->rawValue = 0UL;
        record->measuredValue = 0.0F;
        record->expectedMin = 0.0F;
        record->expectedMax = 0.0F;
        record->errorCode = BOARD_TEST_ERROR_PROFILE_PINMAP;
        return BOARD_TEST_RESULT_FAIL;
    }
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

static void BoardSci_UpdateRs422ExternalSnapshot(BoardTest_U16 statusMask,
                                                  BoardTest_U16 rxValue,
                                                  BoardTest_U16 txValue,
                                                  BoardTest_U16 detail)
{
    gBoardSciRs422ExternalSnapshot.statusMask = statusMask;
    gBoardSciRs422ExternalSnapshot.rxValue = rxValue;
    gBoardSciRs422ExternalSnapshot.txValue = txValue;
    gBoardSciRs422ExternalSnapshot.detail = detail;
}

static void BoardSci_UpdateRs422ExternalRunningRecord(
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
        (float)BOARD_SCI_RS422_EXTERNAL_REQUIRED_MASK;
    record->expectedMax =
        (float)BOARD_SCI_RS422_EXTERNAL_DIAGNOSTIC_MASK;
    record->errorCode = BOARD_TEST_ERROR_NONE;
}

static BoardTest_Result BoardSci_FailRs422ExternalTest(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_U16 detail,
    BoardTest_Record *record)
{
    BoardSci_Rs422ExternalState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_UpdateRs422ExternalSnapshot(statusMask,
                                         rxValue,
                                         txValue,
                                         detail);
    return BoardSci_EvaluateRs422ExternalStatus(statusMask,
                                                rxValue,
                                                txValue,
                                                record);
}

static BoardTest_Result BoardSci_StartRs422ExternalTest(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask = 0U;

    if(BoardSci_InitScicRs422External() == 0U)
    {
        BoardSci_Rs422ExternalState = BOARD_SCI_RS485_STATE_IDLE;
        record->rawValue = 0UL;
        record->measuredValue = 0.0F;
        record->expectedMin = 0.0F;
        record->expectedMax = 0.0F;
        record->errorCode = BOARD_TEST_ERROR_PROFILE_PINMAP;
        return BOARD_TEST_RESULT_FAIL;
    }

    statusMask |= BOARD_SCI_RS422_EXTERNAL_CONFIGURED;
    BoardSci_Rs422ExternalState = BOARD_SCI_RS485_STATE_WAIT_RX;
    BoardSci_UpdateRs422ExternalSnapshot(
        statusMask, 0U, BOARD_SCI_RS422_EXTERNAL_RESPONSE, 0U);
    BoardSci_UpdateRs422ExternalRunningRecord(
        statusMask, 0U, BOARD_SCI_RS422_EXTERNAL_RESPONSE, record);
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardSci_PollRs422ExternalRx(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask = gBoardSciRs422ExternalSnapshot.statusMask;
    BoardTest_U16 rxValue = gBoardSciRs422ExternalSnapshot.rxValue;
    BoardTest_U16 txValue = BOARD_SCI_RS422_EXTERNAL_RESPONSE;
    BoardTest_U16 detail = gBoardSciRs422ExternalSnapshot.detail;
    BoardTest_U16 rxBuffer;

    if((ScicRegs.SCIRXST.bit.RXERROR == 1U) ||
       (ScicRegs.SCIFFRX.bit.RXFFOVF == 1U))
    {
        ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        return BoardSci_FailRs422ExternalTest(
            statusMask, rxValue, txValue, detail, record);
    }

    if((ScicRegs.SCIFFRX.bit.RXFFST == 0U) &&
       (ScicRegs.SCIRXST.bit.RXRDY == 0U))
    {
        BoardSci_UpdateRs422ExternalRunningRecord(
            statusMask, rxValue, txValue, record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    rxBuffer = ScicRegs.SCIRXBUF.all;
    rxValue = rxBuffer & 0x00FFU;
    statusMask |= BOARD_SCI_RS422_EXTERNAL_RX_READY;
    detail |= BOARD_SCI_DETAIL_RX_READY;

    if(((rxBuffer & 0xC000U) != 0U) ||
       (rxValue != BOARD_SCI_RS422_EXTERNAL_REQUEST))
    {
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        return BoardSci_FailRs422ExternalTest(
            statusMask, rxValue, txValue, detail, record);
    }

    statusMask |= BOARD_SCI_RS422_EXTERNAL_RX_MATCH;
    detail |= BOARD_SCI_DETAIL_RX_MATCH;
    if(ScicRegs.SCIFFTX.bit.TXFFST >= 16U)
    {
        detail |= BOARD_SCI_DETAIL_TX_TIMEOUT;
        return BoardSci_FailRs422ExternalTest(
            statusMask, rxValue, txValue, detail, record);
    }

    detail |= BOARD_SCI_DETAIL_TX_READY;
    ScicRegs.SCITXBUF = txValue & 0x00FFU;
    statusMask |= BOARD_SCI_RS422_EXTERNAL_TX_WRITTEN;
    detail |= BOARD_SCI_DETAIL_TX_WRITTEN;
    BoardSci_Rs422ExternalState = BOARD_SCI_RS485_STATE_WAIT_TX;
    BoardSci_UpdateRs422ExternalSnapshot(
        statusMask, rxValue, txValue, detail);
    BoardSci_UpdateRs422ExternalRunningRecord(
        statusMask, rxValue, txValue, record);
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardSci_PollRs422ExternalTxDone(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask = gBoardSciRs422ExternalSnapshot.statusMask;
    BoardTest_U16 rxValue = gBoardSciRs422ExternalSnapshot.rxValue;
    BoardTest_U16 txValue = gBoardSciRs422ExternalSnapshot.txValue;
    BoardTest_U16 detail = gBoardSciRs422ExternalSnapshot.detail;

    if((ScicRegs.SCIFFTX.bit.TXFFST != 0U) ||
       (ScicRegs.SCICTL2.bit.TXEMPTY == 0U))
    {
        BoardSci_UpdateRs422ExternalRunningRecord(
            statusMask, rxValue, txValue, record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    statusMask |= BOARD_SCI_RS422_EXTERNAL_TX_DONE;
    BoardSci_Rs422ExternalState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_UpdateRs422ExternalSnapshot(
        statusMask, rxValue, txValue, detail);
    return BoardSci_EvaluateRs422ExternalStatus(
        statusMask, rxValue, txValue, record);
}

static void BoardSci_UpdateSciaHandheldExternalSnapshot(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_U16 detail)
{
    gBoardSciSciaHandheldExternalSnapshot.statusMask = statusMask;
    gBoardSciSciaHandheldExternalSnapshot.rxValue = rxValue;
    gBoardSciSciaHandheldExternalSnapshot.txValue = txValue;
    gBoardSciSciaHandheldExternalSnapshot.detail = detail;
}

static void BoardSci_UpdateSciaHandheldExternalRunningRecord(
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
        (float)BOARD_SCI_HANDHELD_EXTERNAL_REQUIRED_MASK;
    record->expectedMax =
        (float)BOARD_SCI_HANDHELD_EXTERNAL_DIAGNOSTIC_MASK;
    record->errorCode = BOARD_TEST_ERROR_NONE;
}

static BoardTest_Result BoardSci_FailSciaHandheldExternalTest(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_U16 detail,
    BoardTest_Record *record)
{
    BoardSci_SetSciaDirection(0U);
    statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_BACK_TO_RX;
    BoardSci_SciaHandheldExternalState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_UpdateSciaHandheldExternalSnapshot(statusMask,
                                                rxValue,
                                                txValue,
                                                detail);
    return BoardSci_EvaluateSciaHandheldExternalStatus(statusMask,
                                                       rxValue,
                                                       txValue,
                                                       record);
}

static BoardTest_Result BoardSci_StartSciaHandheldExternalTest(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 detail;

    statusMask = 0U;
    detail = 0U;

    if(BoardSci_InitSciaHandheldExternal() == 0U)
    {
        record->rawValue = 0UL;
        record->errorCode = BOARD_TEST_ERROR_PROFILE_PINMAP;
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }
    statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_CONFIGURED;
    statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_RX_ENABLE_LOW;
    BoardSci_SciaHandheldExternalState = BOARD_SCI_RS485_STATE_WAIT_RX;

    BoardSci_UpdateSciaHandheldExternalSnapshot(
        statusMask,
        0U,
        BOARD_SCI_HANDHELD_EXTERNAL_RESPONSE,
        detail);
    BoardSci_UpdateSciaHandheldExternalRunningRecord(
        statusMask,
        0U,
        BOARD_SCI_HANDHELD_EXTERNAL_RESPONSE,
        record);

    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardSci_PollSciaHandheldExternalRx(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 rxValue;
    BoardTest_U16 txValue;
    BoardTest_U16 detail;
    BoardTest_U16 rxBuffer;

    statusMask = gBoardSciSciaHandheldExternalSnapshot.statusMask;
    rxValue = gBoardSciSciaHandheldExternalSnapshot.rxValue;
    txValue = BOARD_SCI_HANDHELD_EXTERNAL_RESPONSE;
    detail = gBoardSciSciaHandheldExternalSnapshot.detail;

    if((SciaRegs.SCIRXST.bit.RXERROR == 1U) ||
       (SciaRegs.SCIFFRX.bit.RXFFOVF == 1U))
    {
        SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        return BoardSci_FailSciaHandheldExternalTest(statusMask,
                                                     rxValue,
                                                     txValue,
                                                     detail,
                                                     record);
    }

    if((SciaRegs.SCIFFRX.bit.RXFFST == 0U) &&
       (SciaRegs.SCIRXST.bit.RXRDY == 0U))
    {
        BoardSci_UpdateSciaHandheldExternalSnapshot(statusMask,
                                                   rxValue,
                                                   txValue,
                                                   detail);
        BoardSci_UpdateSciaHandheldExternalRunningRecord(statusMask,
                                                         rxValue,
                                                         txValue,
                                                         record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    rxBuffer = SciaRegs.SCIRXBUF.all;
    rxValue = rxBuffer & 0x00FFU;
    statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_RX_READY;
    detail |= BOARD_SCI_DETAIL_RX_READY;

    if((rxBuffer & 0xC000U) != 0U)
    {
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        return BoardSci_FailSciaHandheldExternalTest(statusMask,
                                                     rxValue,
                                                     txValue,
                                                     detail,
                                                     record);
    }

    if(rxValue == BOARD_SCI_HANDHELD_EXTERNAL_REQUEST)
    {
        statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_RX_EXPECTED;
        detail |= BOARD_SCI_DETAIL_RX_MATCH;
    }

    if(SciaRegs.SCIFFTX.bit.TXFFST >= 16U)
    {
        detail |= BOARD_SCI_DETAIL_TX_TIMEOUT;
        return BoardSci_FailSciaHandheldExternalTest(statusMask,
                                                     rxValue,
                                                     txValue,
                                                     detail,
                                                     record);
    }

    BoardSci_SetSciaDirection(1U);
    statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_TX_ENABLE_HIGH;
    detail |= BOARD_SCI_DETAIL_TX_READY;
    SciaRegs.SCITXBUF = txValue & 0x00FFU;
    statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_TX_WRITTEN;
    detail |= BOARD_SCI_DETAIL_TX_WRITTEN;
    BoardSci_SciaHandheldExternalState = BOARD_SCI_RS485_STATE_WAIT_TX;

    BoardSci_UpdateSciaHandheldExternalSnapshot(statusMask,
                                                rxValue,
                                                txValue,
                                                detail);
    BoardSci_UpdateSciaHandheldExternalRunningRecord(statusMask,
                                                     rxValue,
                                                     txValue,
                                                     record);
    return BOARD_TEST_RESULT_RUNNING;
}

static BoardTest_Result BoardSci_PollSciaHandheldExternalTxDone(
    BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 rxValue;
    BoardTest_U16 txValue;
    BoardTest_U16 detail;

    statusMask = gBoardSciSciaHandheldExternalSnapshot.statusMask;
    rxValue = gBoardSciSciaHandheldExternalSnapshot.rxValue;
    txValue = gBoardSciSciaHandheldExternalSnapshot.txValue;
    detail = gBoardSciSciaHandheldExternalSnapshot.detail;

    if((SciaRegs.SCIFFTX.bit.TXFFST != 0U) ||
       (SciaRegs.SCICTL2.bit.TXEMPTY == 0U))
    {
        BoardSci_UpdateSciaHandheldExternalSnapshot(statusMask,
                                                    rxValue,
                                                    txValue,
                                                    detail);
        BoardSci_UpdateSciaHandheldExternalRunningRecord(statusMask,
                                                         rxValue,
                                                         txValue,
                                                         record);
        return BOARD_TEST_RESULT_RUNNING;
    }

    statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_TX_DONE;
    BoardSci_SetSciaDirection(0U);
    statusMask |= BOARD_SCI_HANDHELD_EXTERNAL_BACK_TO_RX;
    BoardSci_SciaHandheldExternalState = BOARD_SCI_RS485_STATE_IDLE;

    BoardSci_UpdateSciaHandheldExternalSnapshot(statusMask,
                                                rxValue,
                                                txValue,
                                                detail);
    return BoardSci_EvaluateSciaHandheldExternalStatus(statusMask,
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

    /* Leave both peripherals quiescent. The next external test initializes
     * its own baud/FIFO; no internal loopback survives this test. */
    SciaRegs.SCICTL1.all = 0U;
    ScibRegs.SCICTL1.all = 0U;
    SciaRegs.SCICCR.bit.LOOPBKENA = 0U;
    ScibRegs.SCICCR.bit.LOOPBKENA = 0U;

    return BoardSci_EvaluateLoopbackStatus(statusMask, sciaRx, scibRx, record);
}

/* Single-test scheduling makes Timer2 exclusive with DI/STO capture.
 * Preserve the timer and restore it on completion, timeout and STOP. */
static BoardTest_U16 BoardSci_DeadlineOwner;
static BoardTest_U32 BoardSci_DeadlineStart;
static BoardTest_U32 BoardSci_OldPeriod, BoardSci_OldCounter;
static BoardTest_U16 BoardSci_OldTcr, BoardSci_OldTpr, BoardSci_OldTprh;

static void BoardSci_StopDeadline(BoardTest_U16 owner)
{
    if(BoardSci_DeadlineOwner != owner) return;
    CpuTimer2Regs.TCR.bit.TSS = 1U;
    CpuTimer2Regs.PRD.all = BoardSci_OldPeriod;
    CpuTimer2Regs.TIM.all = BoardSci_OldCounter;
    CpuTimer2Regs.TPR.all = BoardSci_OldTpr;
    CpuTimer2Regs.TPRH.all = BoardSci_OldTprh;
    CpuTimer2Regs.TCR.all = BoardSci_OldTcr;
    BoardSci_DeadlineOwner = 0U;
}

static void BoardSci_StartDeadline(BoardTest_U16 owner)
{
    const BoardProfile_HardwareDescriptor *hardware;
    hardware = BoardProfile_GetCurrentHardware();
    if((hardware == 0) ||
       (hardware->boardId != BOARD_PROFILE_ID_LOW_VOLTAGE_INVERTER) ||
       (BoardSci_DeadlineOwner != 0U)) return;
    BoardSci_OldPeriod = CpuTimer2Regs.PRD.all;
    BoardSci_OldCounter = CpuTimer2Regs.TIM.all;
    BoardSci_OldTcr = CpuTimer2Regs.TCR.all;
    BoardSci_OldTpr = CpuTimer2Regs.TPR.all;
    BoardSci_OldTprh = CpuTimer2Regs.TPRH.all;
    CpuTimer2Regs.TCR.bit.TSS = 1U;
    CpuTimer2Regs.PRD.all = 0xFFFFFFFFUL;
    CpuTimer2Regs.TPR.all = 199U;
    CpuTimer2Regs.TPRH.all = 0U;
    CpuTimer2Regs.TCR.bit.TIE = 0U;
    CpuTimer2Regs.TCR.bit.FREE = 1U;
    CpuTimer2Regs.TCR.bit.TRB = 1U;
    CpuTimer2Regs.TCR.bit.TSS = 0U;
    BoardSci_DeadlineStart = CpuTimer2Regs.TIM.all;
    BoardSci_DeadlineOwner = owner;
}

static BoardTest_Result BoardSci_RunTimedExternal(BoardTest_Record *record,
                                                  BoardTest_U16 scia)
{
    BoardTest_Result result;
    BoardTest_U16 state, owner, detail;
    owner = scia ? 1U : 2U;
    state = scia ? BoardSci_SciaHandheldExternalState :
                   BoardSci_Rs485ExternalState;
    if((BoardSci_DeadlineOwner == owner) &&
       BoardSci_ExternalDeadlineExpired(BoardSci_DeadlineStart,
                                        CpuTimer2Regs.TIM.all))
    {
        detail = (state == BOARD_SCI_RS485_STATE_WAIT_TX) ?
                 BOARD_SCI_DETAIL_TX_TIMEOUT : BOARD_SCI_DETAIL_RX_TIMEOUT;
        if(scia)
            BoardSci_FailSciaHandheldExternalTest(
                gBoardSciSciaHandheldExternalSnapshot.statusMask,
                gBoardSciSciaHandheldExternalSnapshot.rxValue,
                gBoardSciSciaHandheldExternalSnapshot.txValue,
                gBoardSciSciaHandheldExternalSnapshot.detail | detail, record);
        else
            BoardSci_FailRs485ExternalTest(
                gBoardSciRs485ExternalSnapshot.statusMask,
                gBoardSciRs485ExternalSnapshot.rxValue,
                gBoardSciRs485ExternalSnapshot.txValue,
                gBoardSciRs485ExternalSnapshot.detail | detail, record);
        BoardSci_StopDeadline(owner);
        record->errorCode = scia ? BOARD_TEST_ERROR_SCIA_HANDHELD_EXTERNAL :
                                   BOARD_TEST_ERROR_SCI_RS485_EXTERNAL;
        return BOARD_TEST_RESULT_TIMEOUT;
    }
    if(state == BOARD_SCI_RS485_STATE_IDLE)
    {
        result = scia ? BoardSci_StartSciaHandheldExternalTest(record) :
                        BoardSci_StartRs485ExternalTest(record);
        if(result == BOARD_TEST_RESULT_RUNNING) BoardSci_StartDeadline(owner);
    }
    else if(state == BOARD_SCI_RS485_STATE_WAIT_RX)
        result = scia ? BoardSci_PollSciaHandheldExternalRx(record) :
                        BoardSci_PollRs485ExternalRx(record);
    else
        result = scia ? BoardSci_PollSciaHandheldExternalTxDone(record) :
                        BoardSci_PollRs485ExternalTxDone(record);
    if(result != BOARD_TEST_RESULT_RUNNING) BoardSci_StopDeadline(owner);
    return result;
}

BoardTest_Result BoardSci_RunRs485ExternalTest(BoardTest_Record *record)
{
    return BoardSci_RunTimedExternal(record, 0U);
}

BoardTest_Result BoardSci_RunRs422ExternalTest(BoardTest_Record *record)
{
    if(BoardSci_Rs422ExternalState == BOARD_SCI_RS485_STATE_IDLE)
    {
        return BoardSci_StartRs422ExternalTest(record);
    }

    if(BoardSci_Rs422ExternalState == BOARD_SCI_RS485_STATE_WAIT_RX)
    {
        return BoardSci_PollRs422ExternalRx(record);
    }

    return BoardSci_PollRs422ExternalTxDone(record);
}

BoardTest_Result BoardSci_RunSciaHandheldExternalTest(
    BoardTest_Record *record)
{
    return BoardSci_RunTimedExternal(record, 1U);
}

BoardTest_U16 BoardSci_EnableRs485ExternalStandby(void)
{
    if(BoardSci_InitScibRs485External() == 0U)
    {
        BoardSci_Rs485StandbyEnabled = 0U;
        BoardSci_Rs485StandbyState = BOARD_SCI_RS485_STATE_IDLE;
        return 0U;
    }
    BoardSci_Rs485ExternalState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_Rs485StandbyState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_Rs485StandbyEnabled = 1U;
    BoardSci_UpdateRs485ExternalSnapshot(
        BOARD_SCI_RS485_EXTERNAL_CONFIGURED |
        BOARD_SCI_RS485_EXTERNAL_RX_ENABLE_LOW,
        0U,
        BOARD_SCI_RS485_EXTERNAL_RESPONSE,
        0U);
    return 1U;
}

void BoardSci_DisableRs485ExternalStandby(void)
{
    BoardSci_Rs485StandbyEnabled = 0U;
    BoardSci_Rs485StandbyState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_SetScibDirection(0U);
}

BoardTest_U16 BoardSci_EnableRs422ExternalStandby(void)
{
    if(BoardSci_InitScicRs422External() == 0U)
    {
        BoardSci_Rs422StandbyEnabled = 0U;
        BoardSci_Rs422StandbyState = BOARD_SCI_RS485_STATE_IDLE;
        return 0U;
    }

    BoardSci_Rs422ExternalState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_Rs422StandbyState = BOARD_SCI_RS485_STATE_IDLE;
    BoardSci_Rs422StandbyEnabled = 1U;
    BoardSci_UpdateRs422ExternalSnapshot(
        BOARD_SCI_RS422_EXTERNAL_CONFIGURED,
        0U,
        BOARD_SCI_RS422_EXTERNAL_RESPONSE,
        0U);
    return 1U;
}

void BoardSci_DisableRs422ExternalStandby(void)
{
    BoardSci_Rs422StandbyEnabled = 0U;
    BoardSci_Rs422StandbyState = BOARD_SCI_RS485_STATE_IDLE;
}

void BoardSci_ServiceRs485ExternalStandby(
    BoardTest_Record *record,
    BoardTest_StandbyServiceStatus *status)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 rxValue;
    BoardTest_U16 txValue;
    BoardTest_U16 detail;
    BoardTest_U16 rxBuffer;

    if(BoardSci_Rs485StandbyEnabled == 0U)
    {
        return;
    }

    if(BoardSci_Rs485ExternalState != BOARD_SCI_RS485_STATE_IDLE)
    {
        return;
    }

    statusMask = gBoardSciRs485ExternalSnapshot.statusMask;
    rxValue = gBoardSciRs485ExternalSnapshot.rxValue;
    txValue = BOARD_SCI_RS485_EXTERNAL_RESPONSE;
    detail = gBoardSciRs485ExternalSnapshot.detail;

    if(record != 0)
    {
        if((record->result != BOARD_TEST_RESULT_PASS) &&
           (BoardSci_Rs485StandbyState == BOARD_SCI_RS485_STATE_IDLE))
        {
            BoardSci_UpdateRs485ExternalRunningRecord(
                BOARD_SCI_RS485_EXTERNAL_CONFIGURED |
                BOARD_SCI_RS485_EXTERNAL_RX_ENABLE_LOW,
                rxValue,
                txValue,
                record);
            record->result = BOARD_TEST_RESULT_RUNNING;
        }
    }

    if(BoardSci_Rs485StandbyState == BOARD_SCI_RS485_STATE_WAIT_TX)
    {
        if((ScibRegs.SCIFFTX.bit.TXFFST != 0U) ||
           (ScibRegs.SCICTL2.bit.TXEMPTY == 0U))
        {
            return;
        }

        statusMask |= BOARD_SCI_RS485_EXTERNAL_TX_DONE;
        BoardSci_SetScibDirection(0U);
        statusMask |= BOARD_SCI_RS485_EXTERNAL_BACK_TO_RX;
        BoardSci_Rs485StandbyState = BOARD_SCI_RS485_STATE_IDLE;
        if(status != 0)
        {
            status->state = BOARD_TEST_STANDBY_REPLIED;
            status->replyCount++;
        }
        BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                             rxValue,
                                             txValue,
                                             detail);
        if(record != 0)
        {
            record->result = (BoardTest_U16)
                BoardSci_EvaluateRs485ExternalStatus(statusMask,
                                                     rxValue,
                                                     txValue,
                                                     record);
        }
        return;
    }

    if((ScibRegs.SCIRXST.bit.RXERROR == 1U) ||
       (ScibRegs.SCIFFRX.bit.RXFFOVF == 1U))
    {
        ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        if(status != 0)
        {
            status->state = BOARD_TEST_STANDBY_FAILED;
            status->errorCode = BOARD_TEST_ERROR_SCI_RS485_EXTERNAL;
        }
        BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                             rxValue,
                                             txValue,
                                             detail);
        if(record != 0)
        {
            record->result = (BoardTest_U16)
                BoardSci_EvaluateRs485ExternalStatus(statusMask,
                                                     rxValue,
                                                     txValue,
                                                     record);
        }
        return;
    }

    if((ScibRegs.SCIFFRX.bit.RXFFST == 0U) &&
       (ScibRegs.SCIRXST.bit.RXRDY == 0U))
    {
        if((status != 0) &&
           (status->errorCode == BOARD_TEST_ERROR_NONE))
        {
            status->state = BOARD_TEST_STANDBY_WAITING;
        }
        return;
    }

    rxBuffer = ScibRegs.SCIRXBUF.all;
    rxValue = rxBuffer & 0x00FFU;
    if(status != 0)
    {
        status->state = BOARD_TEST_STANDBY_RECEIVED;
        status->receiveCount++;
    }
    statusMask = BOARD_SCI_RS485_EXTERNAL_CONFIGURED |
                 BOARD_SCI_RS485_EXTERNAL_RX_ENABLE_LOW |
                 BOARD_SCI_RS485_EXTERNAL_RX_READY;
    detail = BOARD_SCI_DETAIL_RX_READY;

    if(((rxBuffer & 0xC000U) == 0U) &&
       (rxValue == BOARD_SCI_RS485_EXTERNAL_REQUEST))
    {
        statusMask |= BOARD_SCI_RS485_EXTERNAL_RX_MATCH;
        detail |= BOARD_SCI_DETAIL_RX_MATCH;

        if(ScibRegs.SCIFFTX.bit.TXFFST < 16U)
        {
            BoardSci_SetScibDirection(1U);
            statusMask |= BOARD_SCI_RS485_EXTERNAL_TX_ENABLE_HIGH;
            detail |= BOARD_SCI_DETAIL_TX_READY;
            ScibRegs.SCITXBUF = txValue & 0x00FFU;
            statusMask |= BOARD_SCI_RS485_EXTERNAL_TX_WRITTEN;
            detail |= BOARD_SCI_DETAIL_TX_WRITTEN;
            BoardSci_Rs485StandbyState = BOARD_SCI_RS485_STATE_WAIT_TX;
        }
        else
        {
            detail |= BOARD_SCI_DETAIL_TX_TIMEOUT;
            if(status != 0)
            {
                status->state = BOARD_TEST_STANDBY_FAILED;
                status->errorCode =
                    BOARD_TEST_ERROR_SCI_RS485_EXTERNAL;
            }
        }
    }
    else
    {
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        if(status != 0)
        {
            status->state = BOARD_TEST_STANDBY_FAILED;
            status->errorCode = BOARD_TEST_ERROR_SCI_RS485_EXTERNAL;
        }
    }

    BoardSci_UpdateRs485ExternalSnapshot(statusMask,
                                         rxValue,
                                         txValue,
                                         detail);
    if(record != 0)
    {
        BoardSci_UpdateRs485ExternalRunningRecord(statusMask,
                                                  rxValue,
                                                  txValue,
                                                  record);
        if(BoardSci_Rs485StandbyState != BOARD_SCI_RS485_STATE_WAIT_TX)
        {
            record->result = (BoardTest_U16)
                BoardSci_EvaluateRs485ExternalStatus(statusMask,
                                                     rxValue,
                                                     txValue,
                                                     record);
        }
        else
        {
            record->result = BOARD_TEST_RESULT_RUNNING;
        }
    }
}

void BoardSci_ServiceRs422ExternalStandby(
    BoardTest_Record *record,
    BoardTest_StandbyServiceStatus *status)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 rxValue;
    BoardTest_U16 txValue;
    BoardTest_U16 detail;
    BoardTest_U16 rxBuffer;

    if((BoardSci_Rs422StandbyEnabled == 0U) ||
       (BoardSci_Rs422ExternalState != BOARD_SCI_RS485_STATE_IDLE))
    {
        return;
    }

    statusMask = gBoardSciRs422ExternalSnapshot.statusMask;
    rxValue = gBoardSciRs422ExternalSnapshot.rxValue;
    txValue = BOARD_SCI_RS422_EXTERNAL_RESPONSE;
    detail = gBoardSciRs422ExternalSnapshot.detail;

    if((record != 0) &&
       (record->result != BOARD_TEST_RESULT_PASS) &&
       (BoardSci_Rs422StandbyState == BOARD_SCI_RS485_STATE_IDLE))
    {
        BoardSci_UpdateRs422ExternalRunningRecord(
            BOARD_SCI_RS422_EXTERNAL_CONFIGURED,
            rxValue, txValue, record);
        record->result = BOARD_TEST_RESULT_RUNNING;
    }

    if(BoardSci_Rs422StandbyState == BOARD_SCI_RS485_STATE_WAIT_TX)
    {
        if((ScicRegs.SCIFFTX.bit.TXFFST != 0U) ||
           (ScicRegs.SCICTL2.bit.TXEMPTY == 0U))
        {
            return;
        }

        statusMask |= BOARD_SCI_RS422_EXTERNAL_TX_DONE;
        BoardSci_Rs422StandbyState = BOARD_SCI_RS485_STATE_IDLE;
        if(status != 0)
        {
            status->state = BOARD_TEST_STANDBY_REPLIED;
            status->replyCount++;
        }
        BoardSci_UpdateRs422ExternalSnapshot(
            statusMask, rxValue, txValue, detail);
        if(record != 0)
        {
            record->result = (BoardTest_U16)
                BoardSci_EvaluateRs422ExternalStatus(
                    statusMask, rxValue, txValue, record);
        }
        return;
    }

    if((ScicRegs.SCIRXST.bit.RXERROR == 1U) ||
       (ScicRegs.SCIFFRX.bit.RXFFOVF == 1U))
    {
        ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1U;
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        if(status != 0)
        {
            status->state = BOARD_TEST_STANDBY_FAILED;
            status->errorCode = BOARD_TEST_ERROR_RS422_EXTERNAL;
        }
        BoardSci_UpdateRs422ExternalSnapshot(
            statusMask, rxValue, txValue, detail);
        if(record != 0)
        {
            record->result = (BoardTest_U16)
                BoardSci_EvaluateRs422ExternalStatus(
                    statusMask, rxValue, txValue, record);
        }
        return;
    }

    if((ScicRegs.SCIFFRX.bit.RXFFST == 0U) &&
       (ScicRegs.SCIRXST.bit.RXRDY == 0U))
    {
        if((status != 0) &&
           (status->errorCode == BOARD_TEST_ERROR_NONE))
        {
            status->state = BOARD_TEST_STANDBY_WAITING;
        }
        return;
    }

    rxBuffer = ScicRegs.SCIRXBUF.all;
    rxValue = rxBuffer & 0x00FFU;
    statusMask = BOARD_SCI_RS422_EXTERNAL_CONFIGURED |
                 BOARD_SCI_RS422_EXTERNAL_RX_READY;
    detail = BOARD_SCI_DETAIL_RX_READY;
    if(status != 0)
    {
        status->state = BOARD_TEST_STANDBY_RECEIVED;
        status->receiveCount++;
    }

    if(((rxBuffer & 0xC000U) == 0U) &&
       (rxValue == BOARD_SCI_RS422_EXTERNAL_REQUEST) &&
       (ScicRegs.SCIFFTX.bit.TXFFST < 16U))
    {
        statusMask |= BOARD_SCI_RS422_EXTERNAL_RX_MATCH;
        detail |= BOARD_SCI_DETAIL_RX_MATCH | BOARD_SCI_DETAIL_TX_READY;
        ScicRegs.SCITXBUF = txValue & 0x00FFU;
        statusMask |= BOARD_SCI_RS422_EXTERNAL_TX_WRITTEN;
        detail |= BOARD_SCI_DETAIL_TX_WRITTEN;
        BoardSci_Rs422StandbyState = BOARD_SCI_RS485_STATE_WAIT_TX;
    }
    else
    {
        detail |= BOARD_SCI_DETAIL_RX_ERROR;
        if(status != 0)
        {
            status->state = BOARD_TEST_STANDBY_FAILED;
            status->errorCode = BOARD_TEST_ERROR_RS422_EXTERNAL;
        }
    }

    BoardSci_UpdateRs422ExternalSnapshot(
        statusMask, rxValue, txValue, detail);
    if(record != 0)
    {
        BoardSci_UpdateRs422ExternalRunningRecord(
            statusMask, rxValue, txValue, record);
        if(BoardSci_Rs422StandbyState != BOARD_SCI_RS485_STATE_WAIT_TX)
        {
            record->result = (BoardTest_U16)
                BoardSci_EvaluateRs422ExternalStatus(
                    statusMask, rxValue, txValue, record);
        }
        else
        {
            record->result = BOARD_TEST_RESULT_RUNNING;
        }
    }
}

void BoardSci_AbortRs485ExternalTest(void)
{
    BoardSci_StopDeadline(2U);
    if(BoardSci_Rs485ExternalState != BOARD_SCI_RS485_STATE_IDLE)
    {
        BoardSci_SetScibDirection(0U);
        BoardSci_Rs485ExternalState = BOARD_SCI_RS485_STATE_IDLE;
    }
}

void BoardSci_AbortRs422ExternalTest(void)
{
    BoardSci_Rs422ExternalState = BOARD_SCI_RS485_STATE_IDLE;
}

void BoardSci_AbortSciaHandheldExternalTest(void)
{
    BoardSci_StopDeadline(1U);
    if(BoardSci_SciaHandheldExternalState != BOARD_SCI_RS485_STATE_IDLE)
    {
        BoardSci_SetSciaDirection(0U);
        BoardSci_SciaHandheldExternalState = BOARD_SCI_RS485_STATE_IDLE;
    }
}
#endif
