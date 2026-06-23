#ifndef BOARD_TEST_TARGET_H
#define BOARD_TEST_TARGET_H

#include "Board_Test.h"

#define BOARD_TEST_COMMAND_NONE          0U
#define BOARD_TEST_COMMAND_START_AUTO    1U
#define BOARD_TEST_COMMAND_START_SINGLE  2U
#define BOARD_TEST_COMMAND_STOP          3U

typedef struct
{
    volatile BoardTest_U16 command;
    volatile BoardTest_U16 testId;
    volatile BoardTest_U16 lastCommandResult;
} BoardTest_CommandMailbox;

extern volatile BoardTest_CommandMailbox gBoardTestCommandMailbox;

void BoardTest_TargetPoll(void);
BoardTest_Result BoardTest_TargetExecute(BoardTest_U16 testId,
                                         BoardTest_Record *record);

#endif
