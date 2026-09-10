#ifndef BOARD_PROFILE_H
#define BOARD_PROFILE_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_PROFILE_ID_NONE                0x0000U
#define BOARD_PROFILE_ID_SYSTEM_MASTER       0x0001U
#define BOARD_PROFILE_ID_LOW_VOLTAGE_INVERTER 0x0002U
#define BOARD_PROFILE_ID_LOW_ALTITUDE_UNIFIED 0x0003U

#define BOARD_PROFILE_HARDWARE_REVISION_NONE   0U
#define BOARD_PROFILE_HARDWARE_REVISION_SYSTEM_MASTER_V01 1U
#define BOARD_PROFILE_HARDWARE_REVISION_LOW_VOLTAGE_V03 3U
#define BOARD_PROFILE_HARDWARE_REVISION_LOW_VOLTAGE_V04 4U
#define BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01 1U

#define BOARD_PROFILE_PIN_UNUSED              0xFFFFU

#define BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT   12U
#define BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT   4U
#define BOARD_PROFILE_DRIVER_FAULT_DI_COUNT   4U
#define BOARD_PROFILE_PROTECTION_FAULT_DI_COUNT 4U
#define BOARD_PROFILE_DRIVER_RESET_DO_COUNT   4U

#define BOARD_PROFILE_LOW_VOLTAGE_ADC_COUNT       13U
#define BOARD_PROFILE_LOW_VOLTAGE_STO_COUNT       2U
#define BOARD_PROFILE_LOW_VOLTAGE_DI_COUNT        6U
#define BOARD_PROFILE_LOW_VOLTAGE_DO_COUNT        3U
#define BOARD_PROFILE_LOW_VOLTAGE_PWM_COUNT       6U
#define BOARD_PROFILE_LOW_VOLTAGE_FAULT_COUNT     4U
#define BOARD_PROFILE_LOW_VOLTAGE_RESOLVER_ABZ_COUNT 3U
#define BOARD_PROFILE_LOW_VOLTAGE_RESOLUTION_COUNT 2U
#define BOARD_PROFILE_LOW_VOLTAGE_RESOLVER_SPI_COUNT 4U
#define BOARD_PROFILE_LOW_VOLTAGE_RESOLVER_FAULT_COUNT 2U

#define BOARD_PROFILE_ADC_MODULE_A             0U
#define BOARD_PROFILE_ADC_MODULE_B             1U
#define BOARD_PROFILE_ADC_MODULE_C             2U
#define BOARD_PROFILE_ADC_MODULE_D             3U

#define BOARD_PROFILE_SIGNAL_ACTIVE_LOW        0U
#define BOARD_PROFILE_SIGNAL_ACTIVE_HIGH       1U
#define BOARD_PROFILE_SIGNAL_TOGGLE            2U

#define BOARD_PROFILE_ETHERNET_NONE           0U
#define BOARD_PROFILE_ETHERNET_EMIF_W5300     1U
#define BOARD_PROFILE_ETHERNET_SPIC_W5500     2U

#define BOARD_PROFILE_CAP_EMIF_W5300          0x00000001UL
#define BOARD_PROFILE_CAP_SPIC_W5500          0x00000002UL
#define BOARD_PROFILE_CAP_EXTERNAL_SRAM       0x00000004UL
#define BOARD_PROFILE_CAP_FPGA_EMIF2          0x00000008UL
#define BOARD_PROFILE_CAP_ADC                 0x00000010UL
#define BOARD_PROFILE_CAP_PWM                 0x00000020UL
#define BOARD_PROFILE_CAP_DSP_DI              0x00000040UL
#define BOARD_PROFILE_CAP_DSP_DO              0x00000080UL
#define BOARD_PROFILE_CAP_SCIA                0x00000100UL
#define BOARD_PROFILE_CAP_SCIB                0x00000200UL
#define BOARD_PROFILE_CAP_CAN_A               0x00000400UL
#define BOARD_PROFILE_CAP_CAN_B               0x00000800UL
#define BOARD_PROFILE_CAP_SPIA                0x00001000UL
/* Legacy combined flag. Current hardware matrices use bus-specific flags. */
#define BOARD_PROFILE_CAP_SPIB_FLASH_FRAM     0x00002000UL
#define BOARD_PROFILE_CAP_SPIC_EXTERNAL       0x00004000UL
#define BOARD_PROFILE_CAP_I2C_RTC             0x00008000UL
#define BOARD_PROFILE_CAP_I2C_TMP116          0x00010000UL
#define BOARD_PROFILE_CAP_I2C_EXTERNAL        0x00020000UL
#define BOARD_PROFILE_CAP_GPIO                0x00040000UL
#define BOARD_PROFILE_CAP_DSP_HDO             0x00080000UL
#define BOARD_PROFILE_CAP_FPGA_DIDO           0x00100000UL
#define BOARD_PROFILE_CAP_RS422               0x00200000UL
#define BOARD_PROFILE_CAP_SPIB                0x00400000UL
#define BOARD_PROFILE_CAP_SPIB_FLASH          0x00800000UL
#define BOARD_PROFILE_CAP_SPIB_FRAM           0x01000000UL
#define BOARD_PROFILE_CAP_STO                  0x02000000UL
#define BOARD_PROFILE_CAP_ANALOG_OUTPUT        0x04000000UL
#define BOARD_PROFILE_CAP_RESOLVER             0x08000000UL
#define BOARD_PROFILE_CAP_PROTECTION_IO        0x10000000UL

typedef enum
{
    BOARD_PROFILE_STATE_BOOT_SAFE = 0,
    BOARD_PROFILE_STATE_NETWORK_READY,
    BOARD_PROFILE_STATE_SELECTED_UNCONFIRMED,
    BOARD_PROFILE_STATE_CONFIRMED
} BoardProfile_State;

typedef enum
{
    BOARD_PROFILE_TEST_NOT_AVAILABLE = 0,
    BOARD_PROFILE_TEST_PENDING_DEVELOPMENT,
    BOARD_PROFILE_TEST_PENDING_VALIDATION,
    BOARD_PROFILE_TEST_TESTABLE
} BoardProfile_TestAvailability;

typedef enum
{
    BOARD_PROFILE_LOW_VOLTAGE_ADC_IA = 0,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_IB,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_IC,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_UUV,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_UVW,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_UDC,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_AI1,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_U5V,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_U15V,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_U10V,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_T_DJ,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_T_INV,
    BOARD_PROFILE_LOW_VOLTAGE_ADC_AI2
} BoardProfile_LowVoltageAdcIndex;

typedef struct
{
    BoardTest_U16 module;
    BoardTest_U16 channel;
} BoardProfile_AdcInput;

typedef struct
{
    BoardProfile_AdcInput adcInputs[BOARD_PROFILE_LOW_VOLTAGE_ADC_COUNT];
    BoardTest_U16 stoInputs[BOARD_PROFILE_LOW_VOLTAGE_STO_COUNT];
    BoardTest_U16 digitalInputs[BOARD_PROFILE_LOW_VOLTAGE_DI_COUNT];
    BoardTest_U16 digitalOutputs[BOARD_PROFILE_LOW_VOLTAGE_DO_COUNT];
    BoardTest_U16 highCurrentOutput;
    BoardTest_U16 softStartOutput;
    BoardTest_U16 fanOutput;
    BoardTest_U16 pwmSoftwareLock;
    BoardTest_U16 pwmOutputs[BOARD_PROFILE_LOW_VOLTAGE_PWM_COUNT];
    BoardTest_U16 faultInputs[BOARD_PROFILE_LOW_VOLTAGE_FAULT_COUNT];
    BoardTest_U16 faultActiveLevels[BOARD_PROFILE_LOW_VOLTAGE_FAULT_COUNT];
    BoardTest_U16 protectionBreakInput;
    BoardTest_U16 faultClearOutput;
    BoardTest_U16 analogOutput;
    BoardTest_U16 resolverAbz[BOARD_PROFILE_LOW_VOLTAGE_RESOLVER_ABZ_COUNT];
    BoardTest_U16 resolverResolution[
        BOARD_PROFILE_LOW_VOLTAGE_RESOLUTION_COUNT];
    BoardTest_U16 resolverMode;
    BoardTest_U16 resolverSpi[
        BOARD_PROFILE_LOW_VOLTAGE_RESOLVER_SPI_COUNT];
    BoardTest_U16 resolverFaultInputs[
        BOARD_PROFILE_LOW_VOLTAGE_RESOLVER_FAULT_COUNT];
    BoardTest_U16 rtcSda;
    BoardTest_U16 rtcScl;
    BoardTest_U16 rtcInterrupt;
} BoardProfile_LowVoltagePinMap;

typedef struct
{
    BoardTest_U16 ethernetInterrupt;
    BoardTest_U16 ethernetReset;
    BoardTest_U16 ethernetSimo;
    BoardTest_U16 ethernetSomi;
    BoardTest_U16 ethernetClock;
    BoardTest_U16 ethernetChipSelect;
    BoardTest_U16 sciaDirection;
    BoardTest_U16 sciaTransmit;
    BoardTest_U16 sciaReceive;
    BoardTest_U16 scibDirection;
    BoardTest_U16 scibTransmit;
    BoardTest_U16 scibReceive;
    BoardTest_U16 canATransmit;
    BoardTest_U16 canAReceive;
    BoardTest_U16 canBTransmit;
    BoardTest_U16 canBReceive;
    BoardTest_U16 led1;
    BoardTest_U16 led2;
    BoardTest_U16 canATransmitMux;
    BoardTest_U16 canAReceiveMux;
    BoardTest_U16 canBTransmitMux;
    BoardTest_U16 canBReceiveMux;
    BoardTest_U16 scibTransmitMux;
    BoardTest_U16 scibReceiveMux;
    BoardTest_U16 rs422Transmit;
    BoardTest_U16 rs422Receive;
    BoardTest_U16 rs422TransmitMux;
    BoardTest_U16 rs422ReceiveMux;
    BoardTest_U16 spibSimo;
    BoardTest_U16 spibSomi;
    BoardTest_U16 spibClock;
    BoardTest_U16 spibFlashChipSelect;
    BoardTest_U16 spibFramChipSelect;
    BoardTest_U16 spibMux;
    BoardTest_U16 pt100Select0;
    BoardTest_U16 pt100Select1;
    BoardTest_U16 ntcFaultSelect0;
    BoardTest_U16 ntcFaultSelect1;
    BoardTest_U16 ntcFaultSelect2;
    BoardTest_U16 dspDi1;
    BoardTest_U16 dspDi2;
    BoardTest_U16 dspDi3;
    BoardTest_U16 dspDo1;
    BoardTest_U16 dspDo2;
    BoardTest_U16 dspDo3;
    BoardTest_U16 pwmDidoOutputs[BOARD_PROFILE_PWM_DIDO_OUTPUT_COUNT];
    BoardTest_U16 pwmDidoEnables[BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT];
    BoardTest_U16 pwmDidoPowerEnables[BOARD_PROFILE_PWM_DIDO_ENABLE_COUNT];
    BoardTest_U16 driverFaultInputs[BOARD_PROFILE_DRIVER_FAULT_DI_COUNT];
    BoardTest_U16 protectionFaultInputs[
        BOARD_PROFILE_PROTECTION_FAULT_DI_COUNT];
    BoardTest_U16 driverResetOutputs[
        BOARD_PROFILE_DRIVER_RESET_DO_COUNT];
    BoardTest_U16 protectionBreakInput;
    BoardTest_U16 faultClearOutput;
} BoardProfile_PinMap;

typedef struct
{
    BoardTest_U16 id;
    BoardTest_U16 dspPartNumber;
    BoardTest_U16 defaultHardwareRevision;
    BoardTest_U32 capabilities;
    const char *name;
} BoardProfile_Descriptor;

typedef struct
{
    BoardTest_U16 boardId;
    BoardTest_U16 ethernetInterface;
    BoardTest_U16 hardwareRevision;
    BoardTest_U16 pinMapRevision;
    BoardTest_U32 capabilities;
    BoardTest_U32 implementedCapabilities;
    BoardTest_U32 validatedCapabilities;
    const char *name;
    BoardProfile_PinMap pins;
    const BoardProfile_LowVoltagePinMap *lowVoltagePins;
} BoardProfile_HardwareDescriptor;

typedef struct
{
    volatile BoardTest_U16 state;
    volatile BoardTest_U16 selectedId;
    volatile BoardTest_U16 selectedHardwareRevision;
    volatile BoardTest_U16 detectedEthernetInterface;
    volatile BoardTest_U16 selectedEthernetInterface;
} BoardProfile_RuntimeStatus;

extern volatile BoardProfile_RuntimeStatus gBoardProfileRuntimeStatus;

void BoardProfile_Init(BoardTest_U16 detectedEthernetInterface);
void BoardProfile_SetDetectedEthernet(
    BoardTest_U16 detectedEthernetInterface);
const BoardProfile_Descriptor *BoardProfile_GetCurrent(void);
const BoardProfile_Descriptor *BoardProfile_GetById(BoardTest_U16 boardId);
const BoardProfile_HardwareDescriptor *BoardProfile_GetHardwareDescriptor(
    BoardTest_U16 boardId,
    BoardTest_U16 hardwareRevision,
    BoardTest_U16 ethernetInterface);
const BoardProfile_HardwareDescriptor *BoardProfile_GetCurrentHardware(void);
BoardTest_U32 BoardProfile_GetEffectiveCapabilities(void);
BoardTest_U32 BoardProfile_GetImplementedCapabilities(void);
BoardTest_U32 BoardProfile_GetValidatedCapabilities(void);
BoardTest_U16 BoardProfile_IsCapabilityValidated(BoardTest_U32 capability);
BoardTest_Result BoardProfile_Select(BoardTest_U16 boardId);
BoardTest_Result BoardProfile_SelectVersion(
    BoardTest_U16 boardId,
    BoardTest_U16 hardwareRevision);
BoardTest_Result BoardProfile_Confirm(void);
void BoardProfile_Clear(void);
BoardTest_U16 BoardProfile_IsConfirmed(void);
BoardTest_U16 BoardProfile_IsTestSupported(BoardTest_U16 testId);
BoardProfile_TestAvailability BoardProfile_GetTestAvailability(
    BoardTest_U16 testId,
    BoardTest_U16 testCapabilities);

#ifdef __cplusplus
}
#endif

#endif
