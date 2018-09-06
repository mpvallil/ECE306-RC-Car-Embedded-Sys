//------------------------------------------------------------------------------
//
//  Description: This file contains the Main Routine - "While" Operating System
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
#include "ports.h"
#include "switch.h"
#include "follow_line.h"

// Global Variables
extern char display_line[DISPLAY_ROW_NUMBER][DISPLAY_COL_NUMBER];
extern char *display[DISPLAY_ROW_NUMBER];
unsigned char display_mode;
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern char once;    
extern unsigned char one_time;
char is_interrupt;
extern char move;
extern unsigned int one_second_count;
extern unsigned int one_second;
extern unsigned char adc_char_left[FOUR];
extern unsigned char adc_char_right[FOUR];
extern unsigned char adc_char[DISPLAY_COL_NUMBER];

extern unsigned int ADC_Thumb;
extern unsigned int ADC_Right_Detector;
extern unsigned int ADC_Left_Detector;
extern unsigned int adc_black_value;
extern unsigned int adc_white_value;

unsigned char state;


extern char received_string[SMALL_RING_SIZE];
extern char received_changed;
extern char clear_disp_line[DISPLAY_COL_NUMBER];
char main_received_string[SMALL_RING_SIZE];
char main_string_disp[DISPLAY_COL_NUMBER] = "          ";

char calibrating_line;


void main(void){
//------------------------------------------------------------------------------
// Main Program
// This is the main routine for the program. Execution of code starts here.
// The operating system is Back Ground Fore Ground.
//
//------------------------------------------------------------------------------
  Init_All();                 // Initialize Clock, Conditions, Timers LCD
 
// Place the contents of what you want on the display, in between the quotes
// Limited to 10 characters per line
  update_screen(DISPLAY_LINE_TOP, " SCANNING ");
  update_screen(DISPLAY_LINE_MIDDLE, " ip_addr: ");
  update_screen(DISPLAY_LINE_MIDDLE_2, clear_disp_line);
  update_screen(DISPLAY_LINE_BOTTOM, clear_disp_line);
  
  lcd_4line();
  display_changed = true;
  
  Toggle_IR_LED(true); // turn on the IR LED initially
  
  
//------------------------------------------------------------------------------
// Begining of the "While" Operating System
//------------------------------------------------------------------------------
  while(ALWAYS) {                      // Can the Operating system run
    
    switch(state) {
      
      case CALIBRATING: // calibrating state to find white and black ADC values
        if(once) {
          once = RESET_STATE;
          HEXtoBCD(ADC_Right_Detector, RIGHT);
          HEXtoBCD(ADC_Left_Detector, LEFT);
          if (!calibrating_line) { // if not done, update thumbwheel values
            thumbwheel_select();
          } else {
            state = IDLE;
            Toggle_IR_LED(false);
          }
        }
    
        update_LCD_ADC();
        Display_Process();
        break;
      
      case IDLE: // IDLE state
        state = IOT;
        break;
      
      case IOT: // reads characters received from IOT, processes them
        read_character();
        process_iot();
        state = DISPLAY;
        
      case DISPLAY: // processes changes to the display
        Display_Process();
        state = LINE;
        break;
      
      case LINE: // processes the values coming in from the ADC detectors
        line_process(adc_black_value, adc_white_value);
        state = IDLE;
        break;
      
     
      
    }
    
  }
}
//------------------------------------------------------------------------------


