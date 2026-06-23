#ifndef BOARD_TEST_H
#define BOARD_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned short BoardTest_U16;
typedef unsigned long BoardTest_U32;

#define BOARD_TEST_INVALID_ID       0xFFFFU
#define BOARD_TEST_ERROR_NONE       0x0000U
#define BOARD_TEST_ERROR_ABORTED    0x0001U
#define BOARD_TEST_ERROR_UNSUPPORTED 0x0002U
#define BOARD_TEST_ERROR_SAFETY_LOCK 0x0003U
#define BOARD_TEST_ERROR_GPIO_READBACK 0x0100U
#define BOARD_TEST_ERROR_SYS_CLOCK   0x0110U
#define BOARD_TEST_ERROR_SYS_TIMER   0x0111U
#define BOARD_TEST_ERROR_RAM_PATTERN 0x0112U
#define BOARD_TEST_ERROR_FLASH_CFG   0x0113U
#define BOARD_TEST_ERROR_WATCHDOG_CFG 0x0114U
#define BOARD_TEST_ERROR_I2C_B_TMP116 0x0120U
#define BOARD_TEST_ERROR_SCI_LOOPBACK 0x0130U

#define BOARD_TEST_CAP_IMPLEMENTED      0x0001U
#define BOARD_TEST_CAP_SOFTWARE_MOCK    0x0002U
#define BOARD_TEST_CAP_EXTERNAL_INJECT  0x0004U
#define BOARD_TEST_CAP_TEST_BENCH       0x0008U

typedef enum
{
    BOARD_TEST_MODE_BOOT_SAFE = 0,
    BOARD_TEST_MODE_IDLE,
    BOARD_TEST_MODE_AUTO,
    BOARD_TEST_MODE_MANUAL,
    BOARD_TEST_MODE_FAULT
} BoardTest_Mode;

typedef enum
{
    BOARD_TEST_RESULT_NOT_RUN = 0,
    BOARD_TEST_RESULT_RUNNING,
    BOARD_TEST_RESULT_PASS,
    BOARD_TEST_RESULT_FAIL,
    BOARD_TEST_RESULT_WARN,
    BOARD_TEST_RESULT_SKIP,
    BOARD_TEST_RESULT_TIMEOUT,
    BOARD_TEST_RESULT_NOT_SUPPORTED,
    BOARD_TEST_RESULT_SAFETY_LOCKED
} BoardTest_Result;

typedef enum
{
    BOARD_TEST_RISK_SAFE = 0,
    BOARD_TEST_RISK_LOW_VOLTAGE,
    BOARD_TEST_RISK_HIGH_POWER
} BoardTest_Risk;

typedef enum
{
    BOARD_TEST_ID_SYS_STARTUP = 0x0100U,
    BOARD_TEST_ID_SYS_CLOCK = 0x0101U,
    BOARD_TEST_ID_SYS_INTERRUPT = 0x0102U,
    BOARD_TEST_ID_SYS_TIMER = 0x0103U,
    BOARD_TEST_ID_SYS_WATCHDOG = 0x0104U,
    BOARD_TEST_ID_RAM = 0x0105U,
    BOARD_TEST_ID_FLASH = 0x0106U,
    BOARD_TEST_ID_GPIO = 0x0200U,
    BOARD_TEST_ID_ADC_BASIC = 0x0201U,
    BOARD_TEST_ID_PWM_BASIC = 0x0202U,
    BOARD_TEST_ID_CAN_HOST = 0x0300U,
    BOARD_TEST_ID_SCI_HOST = 0x0301U,
    BOARD_TEST_ID_SPI_EXTERNAL = 0x0302U,
    BOARD_TEST_ID_I2C_RTC = 0x0303U,
    BOARD_TEST_ID_EMIF_EXTERNAL = 0x0304U,
    BOARD_TEST_ID_ETHERNET = 0x0305U,
    BOARD_TEST_ID_I2C_B_TMP116 = 0x0306U,
    BOARD_TEST_ID_HPD_ESTOP_DI3 = 0x1000U,
    BOARD_TEST_ID_HPD_INDICATOR_DO3 = 0x1001U,
    BOARD_TEST_ID_HPD_DRIVER_1 = 0x1100U,
    BOARD_TEST_ID_HPD_DRIVER_2 = 0x1101U,
    BOARD_TEST_ID_HPD_NTC_1 = 0x1200U,
    BOARD_TEST_ID_HPD_NTC_2 = 0x1201U,
    BOARD_TEST_ID_HPD_BUS_1 = 0x1300U,
    BOARD_TEST_ID_HPD_BUS_2 = 0x1301U,
    BOARD_TEST_ID_HPD_LINE_1 = 0x1400U,
    BOARD_TEST_ID_HPD_LINE_2 = 0x1401U,
    BOARD_TEST_ID_HPD_CURRENT_1 = 0x1500U,
    BOARD_TEST_ID_HPD_CURRENT_2 = 0x1501U,
    BOARD_TEST_ID_HPD_PHASE_1 = 0x1600U,
    BOARD_TEST_ID_HPD_PHASE_2 = 0x1601U
} BoardTest_Id;

#define BOARD_TEST_ITEM_COUNT 31U

typedef struct
{
    BoardTest_U16 id;
    const char *name;
    BoardTest_U16 capabilities;
    BoardTest_U16 risk;
    BoardTest_U16 enabledInAuto;
} BoardTest_Descriptor;

typedef struct
{
    BoardTest_U16 id;
    BoardTest_U16 result;
    BoardTest_U16 errorCode;
    BoardTest_U32 rawValue;
    float measuredValue;
    float expectedMin;
    float expectedMax;
} BoardTest_Record;

typedef struct
{
    BoardTest_U16 mode;
    BoardTest_U16 activeTestId;
    BoardTest_U16 autoIndex;
    BoardTest_U16 pwmAllowed;
    BoardTest_U16 driverAllowed;
    BoardTest_Record records[BOARD_TEST_ITEM_COUNT];
} BoardTest_Manager;

typedef BoardTest_Result (*BoardTest_Executor)(BoardTest_U16 testId,
                                               BoardTest_Record *record);

extern BoardTest_Manager gBoardTestManager;

void BoardTest_Init(void);
BoardTest_Result BoardTest_StartAuto(void);
BoardTest_Result BoardTest_StartSingle(BoardTest_U16 testId);
void BoardTest_Stop(void);
void BoardTest_Process(BoardTest_Executor executor);

BoardTest_U16 BoardTest_GetCount(void);
const BoardTest_Descriptor *BoardTest_GetDescriptor(BoardTest_U16 index);
const BoardTest_Record *BoardTest_GetRecord(BoardTest_U16 testId);

#ifdef __cplusplus
}
#endif

#endif
