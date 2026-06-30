#ifndef BOARD_ETHERNET_TEST_H
#define BOARD_ETHERNET_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    BOARD_ETHERNET_BACKEND_W5300_EMIF = 0,
    BOARD_ETHERNET_BACKEND_W5500_SPI
} BoardEthernet_Backend;

#define BOARD_ETHERNET_W5300_MODE_EXPECTED      0xB800U
#define BOARD_ETHERNET_W5300_RTR_EXPECTED       0x0032U
#define BOARD_ETHERNET_W5300_RCR_EXPECTED       0x0007U
#define BOARD_ETHERNET_W5300_MEM_EXPECTED       0x0808U

#define BOARD_ETHERNET_W5300_BACKEND_EMIF       0x0001U
#define BOARD_ETHERNET_W5300_PINMAP_VALID       0x0002U
#define BOARD_ETHERNET_W5300_MODE_READBACK      0x0004U
#define BOARD_ETHERNET_W5300_RTR_READBACK       0x0008U
#define BOARD_ETHERNET_W5300_RCR_READBACK       0x0010U
#define BOARD_ETHERNET_W5300_MEMORY_READBACK    0x0020U
#define BOARD_ETHERNET_W5300_SOCKET0_READABLE   0x0040U

#define BOARD_ETHERNET_W5300_REQUIRED_MASK \
    (BOARD_ETHERNET_W5300_BACKEND_EMIF |     \
     BOARD_ETHERNET_W5300_PINMAP_VALID |     \
     BOARD_ETHERNET_W5300_MODE_READBACK |    \
     BOARD_ETHERNET_W5300_RTR_READBACK |     \
     BOARD_ETHERNET_W5300_RCR_READBACK |     \
     BOARD_ETHERNET_W5300_MEMORY_READBACK |  \
     BOARD_ETHERNET_W5300_SOCKET0_READABLE)

#define BOARD_ETHERNET_W5300_DIAGNOSTIC_MASK \
    BOARD_ETHERNET_W5300_REQUIRED_MASK

typedef struct
{
    volatile BoardTest_U16 backend;
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 modeReg;
    volatile BoardTest_U16 retryTimeReg;
    volatile BoardTest_U16 retryCountReg;
    volatile BoardTest_U16 txMemoryReg;
    volatile BoardTest_U16 rxMemoryReg;
    volatile BoardTest_U16 socket0Status;
} BoardEthernet_BasicSnapshot;

extern volatile BoardEthernet_BasicSnapshot gBoardEthernetBasicSnapshot;

BoardTest_Result BoardEthernet_EvaluateW5300BasicStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 modeReg,
    BoardTest_U16 retryTimeReg,
    BoardTest_U16 retryCountReg,
    BoardTest_U16 txMemoryReg,
    BoardTest_U16 rxMemoryReg,
    BoardTest_U16 socket0Status,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardEthernet_RunW5300BasicTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
