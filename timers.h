//------------------------------------------------------------------------------
//
//  Description: This file contains macros for the timers.h file
//
//
//  Matthew Vallillo
//  Jan 2018
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.11.2)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include "msp430.h"


// Variables
#define TIME_COUNT_MAX      (1200)     // Time_Count resets after 1 minute


// Timer_A0
#define TA0CCR0_INTERVAL   (12500)     // 50ms with ID__4 ,TAIDEX__8
#define TA0CCR1_INTERVAL   (25000)     // 100ms with ID__4,TAIDEX__8
#define TA0CCR2_INTERVAL   (12500)     // 50ms with ID__4 ,TAIDEX__8
#define TA1CCR0_INTERVAL    (1250)     // 5ms with ID__4 ,TAIDEX__8
#define TA1CCR1_INTERVAL   (25000)     // 100ms ith ID__4 ,TAIDEX__8
#define TA0CCR1_CASE           (2)   
#define TA0CCR2_CASE           (4)
#define TA0CCR3_CASE           (6)
#define TA0_OVERFLOW          (14)
#define TA1CCR1_CASE           (2)   
#define TA1CCR2_CASE           (4)
#define TA1CCR3_CASE           (6)
#define TA1_OVERFLOW          (14)
#define DEBOUNCE_TIME         (10)

// Timer_B0
#define WHEEL_OFF             (0)
#define WHEEL_PERIOD       (1000)
#define WHEEL_SPEED_10      (100)
#define WHEEL_SPEED_15      (150)
#define WHEEL_SPEED_20      (200)
#define WHEEL_SPEED_25      (250)
#define WHEEL_SPEED_30      (300)
#define WHEEL_SPEED_35      (350)
#define WHEEL_SPEED_40      (400)
#define WHEEL_SPEED_45      (450)
#define WHEEL_SPEED_50      (500)
#define WHEEL_SPEED_55      (550)
#define WHEEL_SPEED_60      (600)
#define WHEEL_SPEED_65      (650)
#define WHEEL_SPEED_70      (700)
#define WHEEL_SPEED_75      (750)
#define WHEEL_SPEED_80      (800)
#define WHEEL_SPEED_85      (850)
#define WHEEL_SPEED_FULL    (990)
#define LEFT_REVERSE_SPEED (TB0CCR3)
#define LEFT_FORWARD_SPEED (TB0CCR4)
#define RIGHT_REVERSE_SPEED (TB0CCR5)
#define RIGHT_FORWARD_SPEED (TB0CCR6)

#define DISPLAY_COUNT_200MS    (4)
#define DISPLAY_BLINK_COUNT   (10)

#define ONE_SECOND            (20)     // 1 second with ID__4 ,TAIDEX__8