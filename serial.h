//------------------------------------------------------------------------------
//
//  Description: This file contains macros for the serial.c file
//
//
//  Matthew Vallillo
//  Jan 2018
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.11.2)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include "msp430.h"
#include "functions.h"
#include "macros.h"
#include <string.h>
#include "stdbool.h"

#define BAUD480_MCTLW             (0x4A11)
#define BAUD115_MCTLW             (0x4951)

// Command States
#define FIND_PERIOD                 (0)
#define FIND_ONE                    (1)
#define FIND_TWO1                   (2)
#define FIND_ZERO                   (3)
#define FIND_TWO2                   (4)
#define FIND_COMMAND                (5)
#define FIND_TIME_THOUSAND          (6)
#define FIND_TIME_HUNDRED           (7)
#define FIND_TIME_TEN               (8)
#define FIND_TIME_ONE               (9)
#define FIND_CHAR_RET              (10)
#define FIND_LN_FEED               (11)

#define TWELVE                     (12)

//Ip Address
#define FIND_UP                 (0)
#define FIND_MIDDLE_IP          (1)
#define FIND_BOTTOM_IP          (2)