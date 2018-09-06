//------------------------------------------------------------------------------
//
//  Description: This file contains the state machine for autonomously following 
//               a black line
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
#include "follow_line.h"
#include "commands.h"
#include "timers.h"


unsigned int line_state;
extern unsigned char line_ready_black;
extern unsigned char line_ready_white;

extern unsigned int ADC_Right_Detector;
extern unsigned int ADC_Left_Detector;
extern char move;
char color_flag;
extern unsigned int count_5ms;
extern unsigned int adc_threshold_value;
extern unsigned int Time_Count;
unsigned char one_time;
extern unsigned int one_second;
char done;
char black_line_bitmask;
char follow_line_state_right;
char follow_line_state_left;

extern unsigned int left_wheel_PWM;
extern unsigned int right_wheel_PWM;


// function to follow black line
void line_process(unsigned int black_value, unsigned int white_value) {
  
  switch (line_state) {
    
    case IDLE_LINE:
      break;
    
    case START: // sets threshold value and resets line ready variables
        line_ready_black = RESET_STATE;
        line_ready_white = RESET_STATE;
        adc_threshold_value = (black_value + white_value) >> ONE;
        line_state = LOOK_FOR_WHITE;
        color_flag = RESET_STATE;
        one_time = true;
      break;
    
    case LOOK_FOR_WHITE: //moves forward until white is detected
      
      if (one_time) {
        Left_Motor_FWD(WHEEL_SPEED_45);
        Right_Motor_FWD(WHEEL_SPEED_35);
        one_time = RESET_STATE;
      }
      
      if (ADC_Right_Detector <= (white_value + COLOR_ERROR) && ADC_Left_Detector <= (white_value + COLOR_ERROR)) {
        line_state = LOOK_FOR_BLACK;
        one_time = true;
      }
      break;
      
    case LOOK_FOR_BLACK: // If black line is found, turn motors off
      
      if (ADC_Right_Detector >= adc_threshold_value) {
        color_flag |= RIGHT_BLACK;
        line_state = INITIAL;
        Time_Count = RESET_STATE;
        Init_one_second_timer();
        Motors_OFF();
      }
      if (ADC_Left_Detector >= adc_threshold_value) {
        color_flag |= LEFT_BLACK;
        line_state = INITIAL;
        Time_Count = RESET_STATE;
        Motors_OFF();
      }
      break;
      
    case INITIAL: // turns to folow black line
      if (color_flag & RIGHT_BLACK) { // turns in opposite direection of initial detector to be triggered
        done = Turn_for_5xms(LEFT, 30);
      } else if (color_flag & LEFT_BLACK) {
        done = Turn_for_5xms(RIGHT, 30);
      } else {
        Motors_OFF();
      }
      if (done == ONE) { // waits for turn to be done
        color_flag = RESET_STATE;
        Straight_for_5xms(FORWARD, 10); // moves slightly forward to detect black line
        one_time = true;
        done = RESET_STATE;
        line_state = LINE_FOLLOW;
        black_line_bitmask = RESET_STATE;
      }
      break;
      
    case LINE_FOLLOW: // state to stay on line
      if (one_time) {
        if (!follow_line_state_right && !follow_line_state_left) { // initial state sets PWM values
          change_pwm(RIGHT, right_wheel_PWM);
          change_pwm(LEFT, left_wheel_PWM);
          Motors_FWD(WHEEL_SPEED_60);
        }
      }
      follow_line_function(black_value, white_value); // function to stay on line
      break;
      
    case BOTH_WHITE: // if both detect white, move backwards until black line is detected
      
      if (count_5ms >= FIVE && one_time) {
        Right_Motor_REV(WHEEL_SPEED_35);
        Left_Motor_REV(WHEEL_SPEED_35);
        count_5ms = RESET_STATE;
        one_time = RESET_STATE;
      }
      if (ADC_Left_Detector >= (black_value - (THRESHOLD_VALUE*SIX)) || ADC_Right_Detector >= (black_value - (THRESHOLD_VALUE*SIX))) {  
        Motors_OFF();
        line_state = LINE_FOLLOW;
        one_time = true;
      }
      break;
      
    case EXIT_LINE: // case for exit line command, move forward for 3 seconds
      
      if (Straight_for_5xms(true, 600)) {
        line_state = IDLE_LINE;
        Toggle_IR_LED(false);
      }
      break;
  }
  
    
  
}

// funciton  to stay on black line
void follow_line_function(unsigned int black_value, unsigned int white_value) {
  
  switch(follow_line_state_right) {
    
    case 0: // move forward, if right detector moves off line, slow down left wheel
      if (ADC_Right_Detector <= (black_value - THRESHOLD_VALUE)) {
        change_pwm(LEFT_PWM, left_wheel_PWM - 50);
        Left_Motor_FWD(WHEEL_SPEED_70);
        follow_line_state_right = 1;
      }
      break;
      
    case 1: // move forward, if right detector moves off line, slow down left wheel
      if (ADC_Right_Detector <= (black_value - (THRESHOLD_VALUE*TWO))) {
        change_pwm(LEFT_PWM, left_wheel_PWM - 100);
        Left_Motor_FWD(WHEEL_SPEED_60);
        follow_line_state_right = 2;
        
      // if right detector moves back on black line, speed up left wheel
      } else if (ADC_Right_Detector >= (black_value - THRESHOLD_VALUE)) {
        change_pwm(LEFT_PWM, left_wheel_PWM + 50);
        Left_Motor_FWD(WHEEL_SPEED_75);
        follow_line_state_right = 0;
      }
      break; 
      
    case 2: // move forward, if right detector moves off line, slow down left wheel
      if (ADC_Right_Detector <= (black_value - (THRESHOLD_VALUE*FIVE))) {
        change_pwm(LEFT_PWM, left_wheel_PWM - 50);
        Left_Motor_FWD(WHEEL_SPEED_50);
        follow_line_state_right = 3;
        
      // if right detector moves back on black line, speed up left wheel
      } else if (ADC_Right_Detector >= (black_value - (THRESHOLD_VALUE*TWO))) {
        follow_line_state_right = 1;
        change_pwm(LEFT_PWM, left_wheel_PWM + 100);
        Left_Motor_FWD(WHEEL_SPEED_70);
      }
      break;
      
    case 3: // move forward, if right detector moves off line, stop left wheel
      if (ADC_Right_Detector <= (black_value - (THRESHOLD_VALUE*EIGHT))) {
        if (!color_flag) {
          color_flag |= LEFT_BLACK;
        }
        Left_Motor_OFF();
        
      // if right detector moves back on black line, speed up left wheel
      } else if (ADC_Right_Detector >= (black_value - (THRESHOLD_VALUE*FIVE))) {
        change_pwm(LEFT_PWM, left_wheel_PWM + 50);
        Left_Motor_FWD(WHEEL_SPEED_60);
        follow_line_state_right = 2;
      }
      break;
  }
  
  switch(follow_line_state_left) {
    
    case 0: // move forward, if left detector moves off line, slow down right wheel
      if (ADC_Left_Detector <= (black_value - THRESHOLD_VALUE)) {
        change_pwm(RIGHT_PWM, right_wheel_PWM - 50);
        Right_Motor_FWD(WHEEL_SPEED_65);
        follow_line_state_left = 1;
      }
      break;
      
    case 1: // move forward, if left detector moves off line, slow down right wheel
      if (ADC_Left_Detector <= (black_value - (THRESHOLD_VALUE*TWO))) {
        change_pwm(RIGHT_PWM, right_wheel_PWM - 100);
        Right_Motor_FWD(WHEEL_SPEED_55);
        follow_line_state_left = 2;
        
      // if left detector moves back on black line, speed up right wheel
      } else if (ADC_Left_Detector >= (black_value - THRESHOLD_VALUE)) {
        change_pwm(RIGHT_PWM, right_wheel_PWM + 50);
        Right_Motor_FWD(WHEEL_SPEED_70);
        follow_line_state_left = 0;
      }
      break; 
      
    case 2: // move forward, if left detector moves off line, slow down right wheel
      if (ADC_Left_Detector <= (black_value - (THRESHOLD_VALUE*FIVE))) {
        change_pwm(RIGHT_PWM, right_wheel_PWM - 50);
        Right_Motor_FWD(WHEEL_SPEED_45);
        follow_line_state_left = 3;
        
      // if left detector moves back on black line, speed up right wheel
      } else if (ADC_Left_Detector >= (black_value - (THRESHOLD_VALUE*TWO))) {
        follow_line_state_left = 1;
        change_pwm(RIGHT_PWM, right_wheel_PWM + 100);
        Right_Motor_FWD(WHEEL_SPEED_65);
      }
      break;
      
    case 3: // move forward, if left detector moves off line, stop right wheel
      if (ADC_Left_Detector <= (black_value - (THRESHOLD_VALUE*EIGHT))) {
        if (!color_flag) {
          color_flag |= LEFT_BLACK;
        }
        Right_Motor_OFF();
        
      // if left detector moves back on black line, speed up right wheel
      } else if (ADC_Left_Detector >= (black_value - (THRESHOLD_VALUE*FIVE))) {
        change_pwm(RIGHT_PWM, right_wheel_PWM + 50);
        Right_Motor_FWD(WHEEL_SPEED_55);
        follow_line_state_left = 2;
      }
      break;
  }
      
  // If both are off, move to BOTH_WHITE case
  if (ADC_Left_Detector <= (black_value - (THRESHOLD_VALUE*EIGHT)) && ADC_Right_Detector <= (black_value - (THRESHOLD_VALUE*EIGHT))) {
    line_state = BOTH_WHITE;
    one_time = true;
    count_5ms = RESET_STATE;
  }
}



// function called when exit line command is receieved
void move_to_exit_line(void) {
  Motors_OFF();
  change_pwm(RIGHT_PWM, 700);
  change_pwm(LEFT_PWM, 750);
  line_state = EXIT_LINE;
}

//function called when follow line command is received
void start_follow_line(void) {
  line_state = START;
}