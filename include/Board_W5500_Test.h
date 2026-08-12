#ifndef BOARD_W5500_TEST_H
#define BOARD_W5500_TEST_H

#include "Board_Profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_W5500_VERSION_EXPECTED       0x04U

#define BOARD_W5500_STATUS_GPIO_CONFIGURED 0x0001U
#define BOARD_W5500_STATUS_SPI_CONFIGURED  0x0002U
#define BOARD_W5500_STATUS_VERSION_VALID   0x0004U
#define BOARD_W5500_STATUS_SOCKET_LISTEN   0x0008U
#define BOARD_W5500_STATUS_PHY_READABLE    0x0010U

#define BOARD_W5500_BOOT_PIN_RESET         68U
#define BOARD_W5500_BOOT_PIN_SIMO          69U
#define BOARD_W5500_BOOT_PIN_SOMI          70U
#define BOARD_W5500_BOOT_PIN_CLOCK         71U
#define BOARD_W5500_BOOT_PIN_CHIP_SELECT   72U
#define BOARD_W5500_BOOT_PIN_INTERRUPT     73U

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 version;
    volatile BoardTest_U16 phyConfig;
    volatile BoardTest_U16 socketStatus;
    volatile BoardTest_U16 lastError;
    volatile BoardTest_U16 receiveCount;
    volatile BoardTest_U16 replyCount;
} BoardW5500_Snapshot;

typedef struct
{
    volatile BoardTest_U16 interruptGpio;
    volatile BoardTest_U16 resetGpio;
    volatile BoardTest_U16 simoGpio;
    volatile BoardTest_U16 somiGpio;
    volatile BoardTest_U16 clockGpio;
    volatile BoardTest_U16 chipSelectGpio;
    volatile BoardTest_U16 pinMapRevision;
    volatile BoardTest_U16 valid;
} BoardW5500_BootPinSnapshot;

extern volatile BoardW5500_Snapshot gBoardW5500Snapshot;
extern volatile BoardW5500_BootPinSnapshot gBoardW5500BootPinSnapshot;

BoardTest_U16 BoardW5500_IsBootPinMapCompatible(
    const BoardProfile_HardwareDescriptor *hardware);
BoardTest_Result BoardW5500_RunBasicTest(BoardTest_Record *record);
void BoardW5500_EnableTcpStandby(void);
void BoardW5500_DisableTcpStandby(void);
void BoardW5500_ServiceTcpStandby(BoardTest_StandbyServiceStatus *status);

#ifdef __cplusplus
}
#endif

#endif
