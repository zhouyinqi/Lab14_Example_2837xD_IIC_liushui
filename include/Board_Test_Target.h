#ifndef BOARD_TEST_TARGET_H
#define BOARD_TEST_TARGET_H

#include "Board_Test.h"

#define BOARD_TEST_COMMAND_NONE          0U
#define BOARD_TEST_COMMAND_START_AUTO    1U
#define BOARD_TEST_COMMAND_START_SINGLE  2U
#define BOARD_TEST_COMMAND_STOP          3U
#define BOARD_TEST_COMMAND_START_EXTERNAL_CONNECTED 4U
#define BOARD_TEST_COMMAND_START_HPD_INJECTION      5U
#define BOARD_TEST_COMMAND_START_STAGE_AUTO         6U

typedef struct
{
    volatile BoardTest_U16 command;
    volatile BoardTest_U16 testId;
    volatile BoardTest_U16 stage;
    volatile BoardTest_U16 hpdInputSource;
    volatile float hpdBusVoltageCommandV;
    volatile float hpdLineVoltageCommandV;
    volatile float hpdLineFrequencyCommandHz;
    volatile float hpdCurrentCommandA;
    volatile float hpdCurrentFrequencyCommandHz;
    volatile float hpdTemperatureCommandC;
    volatile BoardTest_U16 lastCommandResult;
} BoardTest_CommandMailbox;

extern volatile BoardTest_CommandMailbox gBoardTestCommandMailbox;

void BoardTest_TargetPoll(void);
BoardTest_Result BoardTest_TargetExecute(BoardTest_U16 testId,
                                         BoardTest_Record *record);

#endif
