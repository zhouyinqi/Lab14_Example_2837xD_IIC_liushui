#ifndef BOARD_SCI_TEST_H
#define BOARD_SCI_TEST_H

#include "Board_Profile.h"

#define BOARD_SCI_EXTERNAL_TIMEOUT_US 30000000UL
BoardTest_U16 BoardSci_ExternalDeadlineExpired(BoardTest_U32 start,
                                              BoardTest_U32 now);

#define BOARD_SCI_LOOPBACK_SCIA_PASS       0x0001UL
#define BOARD_SCI_LOOPBACK_SCIB_PASS       0x0002UL
#define BOARD_SCI_LOOPBACK_EXPECTED_MASK   0x0003UL
#define BOARD_SCI_LOOPBACK_SCIA_PATTERN    0x005AU
#define BOARD_SCI_LOOPBACK_SCIB_PATTERN    0x00A5U

#define BOARD_SCI_RS485_EXTERNAL_CONFIGURED     0x0001U
#define BOARD_SCI_RS485_EXTERNAL_RX_ENABLE_LOW  0x0002U
#define BOARD_SCI_RS485_EXTERNAL_RX_READY       0x0004U
#define BOARD_SCI_RS485_EXTERNAL_RX_MATCH       0x0008U
#define BOARD_SCI_RS485_EXTERNAL_TX_ENABLE_HIGH 0x0010U
#define BOARD_SCI_RS485_EXTERNAL_TX_WRITTEN     0x0020U
#define BOARD_SCI_RS485_EXTERNAL_TX_DONE        0x0040U
#define BOARD_SCI_RS485_EXTERNAL_BACK_TO_RX     0x0080U

#define BOARD_SCI_RS485_EXTERNAL_REQUIRED_MASK \
    (BOARD_SCI_RS485_EXTERNAL_CONFIGURED |     \
     BOARD_SCI_RS485_EXTERNAL_RX_ENABLE_LOW |  \
     BOARD_SCI_RS485_EXTERNAL_RX_READY |       \
     BOARD_SCI_RS485_EXTERNAL_RX_MATCH |       \
     BOARD_SCI_RS485_EXTERNAL_TX_ENABLE_HIGH | \
     BOARD_SCI_RS485_EXTERNAL_TX_WRITTEN |     \
     BOARD_SCI_RS485_EXTERNAL_TX_DONE |        \
     BOARD_SCI_RS485_EXTERNAL_BACK_TO_RX)

#define BOARD_SCI_RS485_EXTERNAL_DIAGNOSTIC_MASK \
    BOARD_SCI_RS485_EXTERNAL_REQUIRED_MASK

#define BOARD_SCI_RS485_EXTERNAL_REQUEST   0x00A5U
#define BOARD_SCI_RS485_EXTERNAL_RESPONSE  0x005AU

#define BOARD_SCI_RS422_EXTERNAL_CONFIGURED 0x0001U
#define BOARD_SCI_RS422_EXTERNAL_RX_READY   0x0002U
#define BOARD_SCI_RS422_EXTERNAL_RX_MATCH   0x0004U
#define BOARD_SCI_RS422_EXTERNAL_TX_WRITTEN 0x0008U
#define BOARD_SCI_RS422_EXTERNAL_TX_DONE    0x0010U

#define BOARD_SCI_RS422_EXTERNAL_REQUIRED_MASK \
    (BOARD_SCI_RS422_EXTERNAL_CONFIGURED |     \
     BOARD_SCI_RS422_EXTERNAL_RX_READY |       \
     BOARD_SCI_RS422_EXTERNAL_RX_MATCH |       \
     BOARD_SCI_RS422_EXTERNAL_TX_WRITTEN |     \
     BOARD_SCI_RS422_EXTERNAL_TX_DONE)

#define BOARD_SCI_RS422_EXTERNAL_DIAGNOSTIC_MASK \
    BOARD_SCI_RS422_EXTERNAL_REQUIRED_MASK

#define BOARD_SCI_RS422_EXTERNAL_REQUEST   0x00A5U
#define BOARD_SCI_RS422_EXTERNAL_RESPONSE  0x005AU

#define BOARD_SCI_HANDHELD_EXTERNAL_CONFIGURED     0x0001U
#define BOARD_SCI_HANDHELD_EXTERNAL_RX_ENABLE_LOW  0x0002U
#define BOARD_SCI_HANDHELD_EXTERNAL_RX_READY       0x0004U
#define BOARD_SCI_HANDHELD_EXTERNAL_RX_EXPECTED    0x0008U
#define BOARD_SCI_HANDHELD_EXTERNAL_TX_ENABLE_HIGH 0x0010U
#define BOARD_SCI_HANDHELD_EXTERNAL_TX_WRITTEN     0x0020U
#define BOARD_SCI_HANDHELD_EXTERNAL_TX_DONE        0x0040U
#define BOARD_SCI_HANDHELD_EXTERNAL_BACK_TO_RX     0x0080U

#define BOARD_SCI_HANDHELD_EXTERNAL_REQUIRED_MASK \
    (BOARD_SCI_HANDHELD_EXTERNAL_CONFIGURED |     \
     BOARD_SCI_HANDHELD_EXTERNAL_RX_ENABLE_LOW |  \
     BOARD_SCI_HANDHELD_EXTERNAL_RX_READY |       \
     BOARD_SCI_HANDHELD_EXTERNAL_TX_ENABLE_HIGH | \
     BOARD_SCI_HANDHELD_EXTERNAL_TX_WRITTEN |     \
     BOARD_SCI_HANDHELD_EXTERNAL_TX_DONE |        \
     BOARD_SCI_HANDHELD_EXTERNAL_BACK_TO_RX)

#define BOARD_SCI_HANDHELD_EXTERNAL_DIAGNOSTIC_MASK \
    (BOARD_SCI_HANDHELD_EXTERNAL_REQUIRED_MASK |     \
     BOARD_SCI_HANDHELD_EXTERNAL_RX_EXPECTED)

#define BOARD_SCI_HANDHELD_EXTERNAL_REQUEST   0x00A5U
#define BOARD_SCI_HANDHELD_EXTERNAL_RESPONSE  0x005AU

#define BOARD_SCI_DETAIL_TX_READY          0x0001U
#define BOARD_SCI_DETAIL_TX_WRITTEN        0x0002U
#define BOARD_SCI_DETAIL_RX_READY          0x0004U
#define BOARD_SCI_DETAIL_RX_MATCH          0x0008U
#define BOARD_SCI_DETAIL_RX_ERROR          0x1000U
#define BOARD_SCI_DETAIL_TX_TIMEOUT        0x2000U
#define BOARD_SCI_DETAIL_RX_TIMEOUT        0x4000U


typedef struct
{
    volatile BoardTest_U16 sciaRx;
    volatile BoardTest_U16 scibRx;
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 sciaDetail;
    volatile BoardTest_U16 scibDetail;
} BoardSci_LoopbackSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 rxValue;
    volatile BoardTest_U16 txValue;
    volatile BoardTest_U16 detail;
    volatile BoardTest_U16 directionLevel;
} BoardSci_Rs485ExternalSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 rxValue;
    volatile BoardTest_U16 txValue;
    volatile BoardTest_U16 detail;
} BoardSci_Rs422ExternalSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 rxValue;
    volatile BoardTest_U16 txValue;
    volatile BoardTest_U16 detail;
    volatile BoardTest_U16 directionLevel;
} BoardSci_SciaHandheldExternalSnapshot;

typedef struct
{
    BoardTest_U16 directionGpio;
    BoardTest_U16 txGpio;
    BoardTest_U16 rxGpio;
    BoardTest_U16 txMux;
    BoardTest_U16 rxMux;
} BoardSci_ScibPinConfiguration;

typedef struct
{
    volatile BoardTest_U16 directionGpio;
    volatile BoardTest_U16 txGpio;
    volatile BoardTest_U16 rxGpio;
    volatile BoardTest_U16 txMux;
    volatile BoardTest_U16 rxMux;
    volatile BoardTest_U16 boardId;
    volatile BoardTest_U16 hardwareRevision;
    volatile BoardTest_U16 ethernetInterface;
    volatile BoardTest_U16 baudRate;
    volatile BoardTest_U16 valid;
} BoardSci_ScibPinSnapshot;

typedef struct
{
    BoardTest_U16 txGpio;
    BoardTest_U16 rxGpio;
    BoardTest_U16 txMux;
    BoardTest_U16 rxMux;
} BoardSci_Rs422PinConfiguration;

typedef struct
{
    volatile BoardTest_U16 txGpio;
    volatile BoardTest_U16 rxGpio;
    volatile BoardTest_U16 txMux;
    volatile BoardTest_U16 rxMux;
    volatile BoardTest_U16 boardId;
    volatile BoardTest_U16 hardwareRevision;
    volatile BoardTest_U16 ethernetInterface;
    volatile BoardTest_U16 baudRate;
    volatile BoardTest_U16 valid;
} BoardSci_Rs422PinSnapshot;

BoardTest_Result BoardSci_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                  BoardTest_U16 sciaRx,
                                                  BoardTest_U16 scibRx,
                                                  BoardTest_Record *record);

BoardTest_Result BoardSci_EvaluateRs485ExternalStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_Record *record);

BoardTest_Result BoardSci_EvaluateRs422ExternalStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_Record *record);

BoardTest_Result BoardSci_EvaluateSciaHandheldExternalStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_Record *record);

BoardTest_U16 BoardSci_ResolveScibPinConfiguration(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardSci_ScibPinConfiguration *configuration);

BoardTest_U16 BoardSci_ResolveSciaPinConfiguration(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardSci_ScibPinConfiguration *configuration);

BoardTest_U16 BoardSci_ResolveRs422PinConfiguration(
    const BoardProfile_HardwareDescriptor *hardware,
    BoardSci_Rs422PinConfiguration *configuration);

extern volatile BoardSci_ScibPinSnapshot gBoardSciScibPinSnapshot;
extern volatile BoardSci_Rs422PinSnapshot gBoardSciRs422PinSnapshot;

#ifndef BOARD_TEST_HOST
extern volatile BoardSci_LoopbackSnapshot gBoardSciLoopbackSnapshot;
extern volatile BoardSci_Rs485ExternalSnapshot
    gBoardSciRs485ExternalSnapshot;
extern volatile BoardSci_Rs422ExternalSnapshot
    gBoardSciRs422ExternalSnapshot;
extern volatile BoardSci_SciaHandheldExternalSnapshot
    gBoardSciSciaHandheldExternalSnapshot;

BoardTest_Result BoardSci_RunLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSci_RunRs485ExternalTest(BoardTest_Record *record);
BoardTest_Result BoardSci_RunRs422ExternalTest(BoardTest_Record *record);
BoardTest_Result BoardSci_RunSciaHandheldExternalTest(
    BoardTest_Record *record);
BoardTest_U16 BoardSci_EnableRs485ExternalStandby(void);
BoardTest_U16 BoardSci_EnableRs422ExternalStandby(void);
void BoardSci_DisableRs485ExternalStandby(void);
void BoardSci_DisableRs422ExternalStandby(void);
void BoardSci_ServiceRs485ExternalStandby(
    BoardTest_Record *record,
    BoardTest_StandbyServiceStatus *status);
void BoardSci_ServiceRs422ExternalStandby(
    BoardTest_Record *record,
    BoardTest_StandbyServiceStatus *status);
void BoardSci_AbortRs485ExternalTest(void);
void BoardSci_AbortRs422ExternalTest(void);
void BoardSci_AbortSciaHandheldExternalTest(void);
#endif

#endif
