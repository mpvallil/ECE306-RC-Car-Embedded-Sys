//------------------------------------------------------------------------------
//
//  Description: This file contains macros for the iot.c file
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


#define STARTUP          (0)
#define WIFI_SCANNING    (1)
#define FIND_IP          (2)
#define PING_COMPUTER    (3)
#define FIND_COMMANDS    (4)

#define PING_COMMAND     (25)