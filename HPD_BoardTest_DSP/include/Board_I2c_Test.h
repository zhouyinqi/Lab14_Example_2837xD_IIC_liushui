#ifndef BOARD_I2C_TEST_H
#define BOARD_I2C_TEST_H

#include "Board_Test.h"

#define BOARD_I2C_STATUS_SUCCESS        0x0000U
#define BOARD_I2C_TMP116_ADDR_MIN       0x48U
#define BOARD_I2C_TMP116_ADDR_MAX       0x4BU
#define BOARD_I2C_TMP116_REG_TEMP       0x00U
#define BOARD_I2C_TMP116_REG_DEVICE_ID  0x0FU
#define BOARD_I2C_TMP116_DEVICE_ID      0x1116U
#define BOARD_I2C_TMP116_LSB_C          0.0078125F

typedef struct
{
    volatile BoardTest_U16 address;
    volatile BoardTest_U16 deviceId;
    volatile BoardTest_U16 temperatureRaw;
    volatile BoardTest_U16 status;
} BoardI2c_Tmp116Snapshot;

BoardTest_Result BoardI2c_EvaluateTmp116Read(BoardTest_U16 status,
                                              BoardTest_U16 address,
                                              BoardTest_U16 deviceId,
                                              BoardTest_U16 temperatureRaw,
                                              BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
extern volatile BoardI2c_Tmp116Snapshot gBoardI2cTmp116Snapshot;

BoardTest_Result BoardI2c_RunTmp116Test(BoardTest_Record *record);
#endif

#endif
