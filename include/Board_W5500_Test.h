#ifndef BOARD_W5500_TEST_H
#define BOARD_W5500_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_W5500_VERSION_EXPECTED       0x04U

#define BOARD_W5500_STATUS_GPIO_CONFIGURED 0x0001U
#define BOARD_W5500_STATUS_SPI_CONFIGURED  0x0002U
#define BOARD_W5500_STATUS_VERSION_VALID   0x0004U
#define BOARD_W5500_STATUS_SOCKET_LISTEN   0x0008U
#define BOARD_W5500_STATUS_PHY_READABLE    0x0010U

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

extern volatile BoardW5500_Snapshot gBoardW5500Snapshot;

BoardTest_Result BoardW5500_RunBasicTest(BoardTest_Record *record);
void BoardW5500_EnableTcpStandby(void);
void BoardW5500_DisableTcpStandby(void);
void BoardW5500_ServiceTcpStandby(BoardTest_StandbyServiceStatus *status);

#ifdef __cplusplus
}
#endif

#endif
