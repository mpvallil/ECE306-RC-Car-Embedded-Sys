//------------------------------------------------------------------------------
//
//  Description: This file contains the code to convert HEX to BCD and thumbwheel
//               menu select
//
//
//  Matthew Vallillo
//  Jan 2018
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.11.2)
//------------------------------------------------------------------------------

#include "macros.h"
#include <string.h>
#include "stdbool.h"
#include "ports.h"


char adc_char[DISPLAY_COL_NUMBER] = "0000  0000";
extern char display_line[DISPLAY_ROW_NUMBER][DISPLAY_COL_NUMBER];
extern char *display[DISPLAY_ROW_NUMBER];
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;

extern unsigned int ADC_Thumb;
unsigned int ADC_Thumb_prev;
extern unsigned int ADC_Right_Detector;
extern unsigned int ADC_Left_Detector;
unsigned char adc_store_values;
unsigned int adc_black_value = 920;
unsigned int adc_white_value = 50;
unsigned int adc_threshold_value;


// HEX to BCD for ADC values
void HEXtoBCD(int hex_value , char left) {
  
  int value;
  if (left) {
    adc_char[ZERO] = '0';
    adc_char[ONE] = '0';
    adc_char[TWO] = '0';
    adc_char[THREE] = '0';
  } else {
    adc_char[SIX] = '0';
    adc_char[SEVEN] = '0';
    adc_char[EIGHT] = '0';
    adc_char[NINE] = '0';
  }
  while ( hex_value > (ONE_THOUSAND - ONE_ONE)){
    hex_value = hex_value - ONE_THOUSAND ;
    value = value + ONE_ONE;
    if (left) {
      adc_char [ZERO] = HEX_38 + value;
    } else {
      adc_char[SIX] = HEX_38 + value;
    }
  }
  value = RESET_STATE;
  while ( hex_value > (ONE_HUNDRED - ONE_ONE)){
    hex_value = hex_value - ONE_HUNDRED;
    value = value + ONE_ONE;
    if (left) {
      adc_char [ONE] = HEX_30 + value;
    } else {
      adc_char[SEVEN] = HEX_30 + value;
    }
  }
  value = RESET_STATE;
  while ( hex_value > (ONE_TEN - ONE_ONE)){
    hex_value = hex_value - ONE_TEN;
    value = value + ONE_ONE;
    if (left) {
      adc_char [TWO] = HEX_30 + value;
    } else {
      adc_char[EIGHT] = HEX_30 + value;
    }
  }
  if (left) {
    adc_char [THREE] = HEX_30 + hex_value;
  } else {
    adc_char[NINE] = HEX_30 + hex_value;
  }
  adc_char[LAST_INDEX] = '\0';
  strcpy(display_line[DISPLAY_LINE_BOTTOM], adc_char);
}

// Updates LCD with ADC values
void update_LCD_ADC(void) {
  
  if (update_display) {
    update_string(display_line[DISPLAY_LINE_BOTTOM], DISPLAY_LINE_BOTTOM);
    display_changed = true;
  }
}

// thumbwheel values for calibration
void thumbwheel_select(void) {
  
  if (ADC_Thumb_prev >= (ADC_Thumb - 5) && ADC_Thumb_prev <= (ADC_Thumb + 5)) {
    
  } else {
    ADC_Thumb_prev = ADC_Thumb;
    if (ADC_Thumb <= HALF_THUMB_VAL) {
      adc_store_values = WHITE;
      strcpy(display_line[DISPLAY_LINE_MIDDLE_2], "SAVE WHT V");
      update_string(display_line[DISPLAY_LINE_MIDDLE_2], DISPLAY_LINE_MIDDLE_2);
      display_changed = true;
    } else {
      adc_store_values = BLACK;
      strcpy(display_line[DISPLAY_LINE_MIDDLE_2], "SAVE BLK V");
      update_string(display_line[DISPLAY_LINE_MIDDLE_2], DISPLAY_LINE_MIDDLE_2);
      display_changed = true;
    }
  }
}

// toggles the IR LED on or off
void Toggle_IR_LED(char on) {
  
  if (on) {
    P8OUT |= IR_LED;
  } else {
    P8OUT &= ~IR_LED;
  }
  
}

