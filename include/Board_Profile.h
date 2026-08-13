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
#define BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01 1U

#define BOARD_PROFILE_PIN_UNUSED              0xFFFFU

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

typedef enum
{
    BOARD_PROFILE_STATE_BOOT_SAFE = 0,
    BOARD_PROFILE_STATE_NETWORK_READY,
    BOARD_PROFILE_STATE_SELECTED_UNCONFIRMED,
    BOARD_PROFILE_STATE_CONFIRMED
} BoardProfile_State;

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

#ifdef __cplusplus
}
#endif

#endif
