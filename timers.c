//------------------------------------------------------------------------------
//
//  Description: This file contains the timer functions - initializations and logic
//
//
//  Matthew Vallillo
//  Mar 2018
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.11.2)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include "msp430.h"
#include "functions.h"
#include "timers.h"
#include "macros.h"
#include "ports.h"
#include "stdbool.h"




extern char button_has_pressed;
extern char button_debounce;
extern char debounce_count_B1;
extern char debounce_count_B2;

extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern volatile unsigned char display_changed;

unsigned int Time_Count;                       // Updates every 50ms with A0 clock
unsigned int one_second_count;
unsigned int one_second;
extern char is_interrupt;
char once;
extern unsigned char state;
unsigned int count_5ms;
extern unsigned char state;

// Initializes Timers
void Init_Timers(void) {
  
  Init_Timer_A0();
  Init_Timer_A1();
  Init_Timer_B0();
  
}

//Initialize TimerA0 
void Init_Timer_A0(void) {                    
  
  TA0CTL = TASSEL__SMCLK;                      // SMCLK source
  TA0CTL |= TACLR;                             // Resets TA0R, divider
  TA0CTL |= MC__CONTINUOUS;                    // Continuous up
  TA0CTL |= ID__4;                             // Divide clock by 4
  
  TA0EX0 = TAIDEX__8;                          // Divide clock by 8
  
  TA0CCR0 = TA0CCR0_INTERVAL;                  // CCR0
  TA0CCTL0 |= CCIE;                            // CCR0 enable interrupt
  
  TA0CCR1 = TA0CCR1_INTERVAL;                  // CCR1
  TA0CCTL1 &= ~CCIE;                           // CCR1 disable interrupt
  
  TA0CCR2 = TA0CCR2_INTERVAL;                  // CCR2
  TA0CCTL2 |= CCIE;                            // CCR2 enable interrupt
  
  TA0CTL &= ~TAIE;                             // Disable Overflow interrupt
  TA0CTL &= ~TAIFG;                            // Clear overflow interrupt flag
}

//Initializes TimerA1
void Init_Timer_A1(void) {
  
  TA1CTL = TASSEL__SMCLK;                      // SMCLK source
  TA1CTL |= TACLR;                             // Resets TA1R, divider
  TA1CTL |= MC__CONTINUOUS;                    // Continuous up
  TA1CTL |= ID__4;                             // Divide clock by 4
  
  TA1EX0 = TAIDEX__8;                          // Divide clock by 8
  
  TA1CCR0 = TA1CCR0_INTERVAL;                  // CCR0
  TA1CCTL0 |= CCIE;                            // CCR0 enable interrupt
  
  //TA1CCR1 = TA1CCR1_INTERVAL;                  // CCR1
  //TA1CCTL1 &= ~CCIE;                           // CCR1 disable interrupt
  
  //TA1CCR2 = TA1CCR2_INTERVAL;                  // CCR2
  //TA1CCTL2 |= CCIE;                            // CCR2 enable interrupt
  
  TA1CTL &= ~TAIE;                             // Disable Overflow interrupt
  TA1CTL &= ~TAIFG;                            // Clear overflow interrupt flag
}

//Initialize TimerB0
void Init_Timer_B0(void) {
  
  TB0CTL = TASSEL__SMCLK;                      // SMCLK source
  TB0CTL |= TBCLR;                             // Resets TB0R, divider
  TB0CTL |= MC__UP;                            // Continuous up
  //TB0CTL |= ID__4;                             // Divide clock by 4
  
  //TB0EX0 = TAIDEX__8;                        // Divide clock by 8
  
  TB0CCR0 = WHEEL_PERIOD;
  
  TB0CCTL3 = OUTMOD_7;                         // CCR1 reset/set
  LEFT_REVERSE_SPEED = WHEEL_OFF;
  
  TB0CCTL4 = OUTMOD_7;                         // CCR2 reset/set
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  
  TB0CCTL5 = OUTMOD_7;                         // CCR1 reset/set
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  
  TB0CCTL6 = OUTMOD_7;                         // CCR2 reset/set
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR(void) {     // 50ms with ID__4 ,TAIDEX__8
  
  // TimerA0_CCR0 Interrupt
  
  
  Time_Count++;
  if (Time_Count == TIME_COUNT_MAX >> ONE) { 
    ping_computer_disconnect();        // ping computer after 30 seconds
  }
  if (Time_Count >= TIME_COUNT_MAX) {  // Resets after 1 Minute
    ping_computer_disconnect();        // ping computer after 1 minute
    Time_Count = RESET_STATE; 
  }
  one_second_count++;
  if (one_second_count >= ONE_SECOND) { // Resets after one second
    one_second_count = RESET_STATE;
    one_second++;
  }
  
  once = ALWAYS;
  
  TA0CCR0 += TA0CCR0_INTERVAL;
  
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void) {  // 100ms with ID__4,TAIDEX__8
  
  // TimerA1_CCR0 Interrupt
  
  if (state == IDLE) {
    state = DISPLAY;
  }
   count_5ms++;
  
  TA1CCR0 += TA1CCR0_INTERVAL;
  
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) {
  
  switch(__even_in_range(TA0IV,TA0_OVERFLOW)) {
    
    case TA0CCR1_CASE:
      is_interrupt = ALWAYS;
      if (button_has_pressed & BUTTON1) {   // Button 1 debounce logic
        debounce_count_B1++;
        if (debounce_count_B1 == DEBOUNCE_TIME) {
          button_has_pressed &= ~BUTTON1;
          button_debounce &= ~BUTTON1;
          P5IFG &= ~BUTTON1;
          P5IE |= BUTTON1;
          P1OUT |= RED_LED;
          TA0CCTL0 |= CCIE;
        }
      }
      if (button_has_pressed & BUTTON2) {   // Button 2 debounce logic
        debounce_count_B2++;
        if (debounce_count_B2 == DEBOUNCE_TIME) {
          button_has_pressed &= ~BUTTON2;
          button_debounce &= ~BUTTON2;
          P5IFG &= ~BUTTON2;
          P5IE |= BUTTON2;
          P1OUT |= GRN_LED;
          TA0CCTL0 |= CCIE;
        }
      }
      if (button_debounce == RESET_STATE) {
        TA0CCTL1 &= ~CCIE;               // Disable debounce timer
        is_interrupt = RESET_STATE;
      }
      TA0CCR1 += TA0CCR1_INTERVAL;
      break;
      
      case TA0CCR2_CASE:
        update_display_count++;
        if (update_display_count >= DISPLAY_COUNT_200MS) { // LCD display updates every 200ms
          update_display = ALWAYS;
          update_display_count = RESET_STATE;
          if ((!(P3OUT & IOT_RESET)) && state != CALIBRATING) { // If the IOT module is in reset, then reset it
            Init_IOT(true);
          }
        }
        break;
        
      
    default:
      break;
    
  }
  
}

void Init_one_second_timer(void) { // initialize a timer that counts every second
  
  one_second_count = RESET_STATE;
  one_second = RESET_STATE;
}