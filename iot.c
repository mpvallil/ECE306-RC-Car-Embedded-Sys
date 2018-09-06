//------------------------------------------------------------------------------
//
//  Description: This file contains the state machine to connect to the IOT 
//               Wifi module
//
//
//
//  Matthew Vallillo
//  April 2018
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.11.2)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include "msp430.h"
#include "functions.h"
#include "macros.h"
#include <string.h>
#include "stdbool.h"
#include "iot.h"
#include "ports.h"
#include "commands.h"



char iot_state;
char ping_lib_comp[PING_COMMAND] = "AT+S.PING=152.14.142.1\r\n";
extern char command_char_state;
char command_char_state_disp;
extern char ip_addr_middle_2[DISPLAY_COL_NUMBER];
extern char ip_addr_bottom[DISPLAY_COL_NUMBER];
extern char ssid[DISPLAY_COL_NUMBER];
extern char clear_disp_line[DISPLAY_COL_NUMBER];

// processes IOT state
void process_iot(void) {
  
  switch (iot_state) { 
    
    case STARTUP: // waits for the IOT to come out of reset
      if (!(P3OUT & IOT_RESET)) {
        iot_state = WIFI_SCANNING;
      }
      break;
    
    case WIFI_SCANNING: // waits for the SSID to be read and displayed
      if (process_character_SSID()) {
        iot_state = FIND_IP;
      }
      break;
    
    case FIND_IP: // waits for the IP Address to be read and displayed
      if (process_character_IPADDR()) {
        Init_IOT_Interrupt();
        iot_state = PING_COMPUTER;
      }
      break;
    
    case PING_COMPUTER: // pings a LAN computer to acccept wireless commands
      ping_computer_disconnect();
      iot_state = FIND_COMMANDS;
      
      break;
      
    case FIND_COMMANDS:
      
      find_commands(); // finds commands coming in through read_character
      process_commands(); // processes commands 
      commands_disp(); // displays commands on LCD
      break;
    
    
    
    
    
  }
  
}

void ping_computer_disconnect(void) { // pings LAN computer to acceot wireless commands
  if (iot_state == FIND_COMMANDS || iot_state == PING_COMPUTER) {
      for (int i = ZERO; i < PING_COMMAND; i++) {
        out_character(ping_lib_comp[i]);
      }
  }
}


void commands_disp(void) { //updates LCD with current commands
  
  if (command_char_state_disp != command_char_state) {
    command_char_state_disp = command_char_state;
    if (command_char_state_disp == FORWARD) {
      update_screen(DISPLAY_LINE_MIDDLE, " FORWARD  ");
      update_screen(DISPLAY_LINE_MIDDLE_2, clear_disp_line);
      lcd_BIG_mid();
    } if (command_char_state_disp == BACKWARD) {
      update_screen(DISPLAY_LINE_MIDDLE, " BACKWARD ");
      update_screen(DISPLAY_LINE_MIDDLE_2, clear_disp_line);
      lcd_BIG_mid();
    } if (command_char_state_disp == RIGHT_TURN) {
      update_screen(DISPLAY_LINE_MIDDLE, " RT TURN  ");
      update_screen(DISPLAY_LINE_MIDDLE_2, clear_disp_line);
      lcd_BIG_mid();
    } if (command_char_state_disp == LEFT_TURN) {
      update_screen(DISPLAY_LINE_MIDDLE, "LEFT TURN ");
      update_screen(DISPLAY_LINE_MIDDLE_2, clear_disp_line);
      lcd_BIG_mid();
    } if (command_char_state == WAITING) {
      update_screen(DISPLAY_LINE_MIDDLE, " ip_addr: ");
      update_screen(DISPLAY_LINE_MIDDLE_2, ip_addr_middle_2);
      update_screen(DISPLAY_LINE_BOTTOM, ip_addr_bottom);
      lcd_4line();
    }
  }
 
}

void scan_for_wifi(void) { // sends IOT back to initial state for a reset
  iot_state = STARTUP;
}