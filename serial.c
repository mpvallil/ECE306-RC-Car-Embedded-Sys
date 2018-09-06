//------------------------------------------------------------------------------
//
//  Description: This file contains functions for serialization - receiving and transmitting
//               data using an IOT Wifi module
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
#include "serial.h"

extern volatile unsigned int usb_rx_ring_wr;
extern volatile char USB_Char_Rx[SMALL_RING_SIZE];
unsigned int usb_rx_ring_rd;
unsigned int usb_tx_ring_wr;
unsigned int usb_tx_ring_rd;

char received_string[SMALL_RING_SIZE];
extern char main_received_string[SMALL_RING_SIZE];
char received_changed;

unsigned int process_char_rd;
unsigned int usb_rx_ring_rd_main;
extern char clear_disp_line[DISPLAY_COL_NUMBER];
extern char main_string_disp[DISPLAY_COL_NUMBER];
char final_char_flag;
char receiving;
extern char ready_to_send;
char ready;
char command_state;
char command_char;
char command_ready;
int command_time;
char ip_addr_middle_2[DISPLAY_COL_NUMBER] = "          ";
char ip_addr_bottom[DISPLAY_COL_NUMBER] = "          ";
char ssid[DISPLAY_COL_NUMBER] = "          ";
char found_ip;
char ip_addr_state;


void Init_Serial(char baud) {
  
  Init_Serial_UCA0(baud); // Initialize Serial Port for USB
  Init_Serial_UCA3(baud); // Initialize Serial Port for USB
  
  
}



void Init_Serial_UCA0(char baud){ // intialize UCA0
  int i;
  for(i=ZERO; i<SMALL_RING_SIZE; i++){
    USB_Char_Rx[i] = ZERO; // USB Rx Buffer
  }
  usb_rx_ring_wr = BEGINNING;
  usb_rx_ring_rd = BEGINNING;
  //for(i=0; i<LARGE_RING_SIZE; i++) // May not use this
  //  USB_Char_Tx[i] = 0x00; // USB Tx Buffer
  //
  usb_tx_ring_wr = BEGINNING;
  usb_tx_ring_rd = BEGINNING;
  // Configure UART 0
  UCA0CTLW0 = ZERO; // Use word register
  UCA0CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
  UCA0CTLW0 |= UCSWRST; // Set Software reset enable
  // 9,600 Baud Rate
  // 1. Calculate N = fBRCLK / Baudrate
  //      N = SMCLK / 9,600 => 8,000,000 / 9,600 = 833.3333333
  //   if N > 16 continue with step 3, otherwise with step 2
  // 2. OS16 = 0, UCBRx = INT(N)
  //   continue with step 4
  // 3. OS16 = 1,
  //        UCx         = INT(N/16),
  //                    = INT(N/16) = 833.333/16 => 52
  //        UCFx        = INT([(N/16) – INT(N/16)] × 16)
  //                    = ([833.333/16-INT(833.333/16)]*16)
  //                    = (52.08333333-52)*16
  //                    = 0.083*16 = 1
  // 4. UCSx is found by looking up the fractional part of N (= N-INT(N)) in table Table 18-4
  //    Decimal of SMCLK / 8,000,000 / 9,600 = 833.3333333 => 0.333 yields 0x49 [Table]
  // 5. If OS16 = 0 was chosen, a detailed error calculation is recommended to be performed
  //                                                TX error (%) RX error (%)
  // BRCLK    Baudrate UCOS16  UCBRx  UCFx   UCSx     neg   pos  neg  pos
  // 8000000    9600      1     52      1    0x49    -0.08 0.04 -0.10 0.14
  
  // For 460800                                              TX error (%) RX error (%)
  // BRCLK    Baudrate UCOS16  UCBRx  UCFx   UCSx     neg   pos  neg  pos
  // 8000000   460800    1      1      1     0x4A    -0.08 0.04 -0.10 0.14
  
  // For 115200                                             TX error (%) RX error (%)
  // BRCLK    Baudrate UCOS16  UCBRx  UCFx   UCSx     neg   pos  neg  pos
  // 8000000   115200    1      4      5     0x49    -0.08 0.04 -0.10 0.14
  
  if (!baud) {
    UCA0BRW = FOUR; // 115200 Baud
    // UCA0MCTLW = UCSx concatenate UCFx concatenate UCOS16;
    // UCA0MCTLW = 0x49 concatenate 1 concatenate 1;
    UCA0MCTLW = BAUD115_MCTLW;
    UCA0CTL1 &= ~UCSWRST; // Release from reset
    UCA0IE |= UCRXIE; // Enable RX interrupt
    
  } else {
    UCA0BRW = ONE; // 460800 Baud
    // UCA0MCTLW = UCSx concatenate UCFx concatenate UCOS16;
    // UCA0MCTLW = 0x49 concatenate 1 concatenate 1;
    UCA0MCTLW = BAUD480_MCTLW;
    UCA0CTL1 &= ~UCSWRST; // Release from reset
    UCA0IE |= UCRXIE; // Enable RX interrupt
  }
}

void Init_Serial_UCA3(char baud){ // Initialize UCA3
  int i;
  for(i=ZERO; i<SMALL_RING_SIZE; i++){
    USB_Char_Rx[i] = ZERO; // USB Rx Buffer
  }
  usb_rx_ring_wr = BEGINNING;
  usb_rx_ring_rd = BEGINNING;
  //for(i=0; i<LARGE_RING_SIZE; i++) // May not use this
  //  USB_Char_Tx[i] = 0x00; // USB Tx Buffer
  //
  usb_tx_ring_wr = BEGINNING;
  usb_tx_ring_rd = BEGINNING;
  // Configure UART 0
  UCA3CTLW0 = ZERO; // Use word register
  UCA3CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
  UCA3CTLW0 |= UCSWRST; // Set Software reset enable
  // 9,600 Baud Rate
  // 1. Calculate N = fBRCLK / Baudrate
  //      N = SMCLK / 9,600 => 8,000,000 / 9,600 = 833.3333333
  //   if N > 16 continue with step 3, otherwise with step 2
  // 2. OS16 = 0, UCBRx = INT(N)
  //   continue with step 4
  // 3. OS16 = 1,
  //        UCx         = INT(N/16),
  //                    = INT(N/16) = 833.333/16 => 52
  //        UCFx        = INT([(N/16) – INT(N/16)] × 16)
  //                    = ([833.333/16-INT(833.333/16)]*16)
  //                    = (52.08333333-52)*16
  //                    = 0.083*16 = 1
  // 4. UCSx is found by looking up the fractional part of N (= N-INT(N)) in table Table 18-4
  //    Decimal of SMCLK / 8,000,000 / 9,600 = 833.3333333 => 0.333 yields 0x49 [Table]
  // 5. If OS16 = 0 was chosen, a detailed error calculation is recommended to be performed
  //                                                TX error (%) RX error (%)
  // BRCLK    Baudrate UCOS16  UCBRx  UCFx   UCSx     neg   pos  neg  pos
  // 8000000    9600      1     52      1    0x49    -0.08 0.04 -0.10 0.14
  
  // For 460800                                              TX error (%) RX error (%)
  // BRCLK    Baudrate UCOS16  UCBRx  UCFx   UCSx     neg   pos  neg  pos
  // 8000000   460800    1      1      1     0x4A    -0.08 0.04 -0.10 0.14
  
  // For 115200                                             TX error (%) RX error (%)
  // BRCLK    Baudrate UCOS16  UCBRx  UCFx   UCSx     neg   pos  neg  pos
  // 8000000   115200    1      4      5     0x49    -0.08 0.04 -0.10 0.14
  
  if (!baud) {
    UCA3BRW = FOUR; // 115200 Baud
    // UCA3MCTLW = UCSx concatenate UCFx concatenate UCOS16;
    // UCA3MCTLW = 0x49 concatenate 1 concatenate 1;
    UCA3MCTLW = BAUD115_MCTLW;
    UCA3CTL1 &= ~UCSWRST; // Release from reset
    UCA3IE |= UCRXIE; // Enable RX interrupt
    
  } else {
    UCA3BRW = ONE; // 460800 Baud
    // UCA3MCTLW = UCSx concatenate UCFx concatenate UCOS16;
    // UCA3MCTLW = 0x49 concatenate 1 concatenate 1;
    UCA3MCTLW = BAUD480_MCTLW;
    UCA3CTL1 &= ~UCSWRST; // Release from reset
    UCA3IE |= UCRXIE; // Enable RX interrupt
  }
}





void out_character(char character) {
//------------------------------------------------------------------------------
// The while loop will stall as long as the Flag is not set [port is busy]
  while (!(UCA3IFG & UCTXIFG)); // USCI_A3 TX buffer ready?
    UCA3TXBUF = character;
//  while (!(UCA0IFG & UCTXIFG)); // USCI_A3 TX buffer ready?
//    UCA0TXBUF = character;
//------------------------------------------------------------------------------
}

void read_character(void) { // reads characters from UCA3
  UCA3IE &= ~UCRXIE; // Disable RX interrupt
  if (usb_rx_ring_wr != usb_rx_ring_rd) {
    received_changed = true;
    received_string[usb_rx_ring_rd] = USB_Char_Rx[usb_rx_ring_rd]; // USB_Char_Rx -> received_String
    if (++usb_rx_ring_rd >= (SMALL_RING_SIZE)){
      usb_rx_ring_rd = BEGINNING; // Circular buffer back to beginning
    }
  }
  UCA3IE |= UCRXIE; // Enable RX interrupt
}

void find_commands(void) { // finds commands through the buffer from read_charcter
  
  if (usb_rx_ring_rd != usb_rx_ring_rd_main) {
    main_received_string[process_char_rd] = received_string[usb_rx_ring_rd_main];
     
    switch (command_state) { // Finds each part of the commands in the structure ".1202XXXXX"
      case FIND_PERIOD:
        if (main_received_string[process_char_rd++] == '.') {
          command_state = FIND_ONE;
        } else {
          command_state = FIND_PERIOD;
          process_char_rd = RESET_STATE;
        }
        break;
        
      case FIND_ONE:
        if (main_received_string[process_char_rd++] == '1') {
          command_state = FIND_TWO1;
        } else {
          command_state = FIND_PERIOD;
          process_char_rd = RESET_STATE;
        }
        break;
        
      case FIND_TWO1:
        if (main_received_string[process_char_rd++] == '2') {
          command_state = FIND_ZERO;
        } else {
          command_state = FIND_PERIOD;
          process_char_rd = RESET_STATE;
        }
        break;
        
      case FIND_ZERO:
        if (main_received_string[process_char_rd++] == '0') {
          command_state = FIND_TWO2;
        } else {
          command_state = FIND_PERIOD;
          process_char_rd = RESET_STATE;
        }
        break;
        
      case FIND_TWO2:
        if (main_received_string[process_char_rd++] == '2') {
          command_state = FIND_COMMAND;
        } else {
          command_state = FIND_PERIOD;
          process_char_rd = RESET_STATE;
        }
        break;  
        
      case FIND_COMMAND:
        if (main_received_string[process_char_rd] >= 'A' && main_received_string[process_char_rd] <= 'Z') {
          command_char = main_received_string[process_char_rd++];
          command_state = FIND_TIME_THOUSAND;
        } else {
          command_state = FIND_PERIOD;
          process_char_rd = RESET_STATE;
        }
        break;
      
      case FIND_TIME_THOUSAND:
        command_time = (main_received_string[process_char_rd++] - HEX_30) * ONE_THOUSAND;
        command_state = FIND_TIME_HUNDRED;
        
        break;  
        
      case FIND_TIME_HUNDRED:
        command_time += (main_received_string[process_char_rd++] - HEX_30) * ONE_HUNDRED;
        command_state = FIND_TIME_TEN;
        break;  
        
      case FIND_TIME_TEN:
        command_time += (main_received_string[process_char_rd++] - HEX_30) * ONE_TEN;
        command_state = FIND_TIME_ONE;
        break;  
        
      case FIND_TIME_ONE:
        command_time += (main_received_string[process_char_rd++] - HEX_30);
        command_state = FIND_CHAR_RET;
        break;
        
      case FIND_CHAR_RET:
        process_char_rd++;
        command_state = FIND_LN_FEED;
        break;
        
      case FIND_LN_FEED:
        command_ready = true;
        process_char_rd = RESET_STATE;
        command_state = FIND_PERIOD;
        break;
        
      default:
        break;
        
    }
    if (++usb_rx_ring_rd_main >= (SMALL_RING_SIZE)){
      usb_rx_ring_rd_main = BEGINNING; // Circular buffer back to beginning
    }
  }
}


char process_character_SSID(void) { // function to find 
  
  if (usb_rx_ring_rd != usb_rx_ring_rd_main) {

    
    main_received_string[process_char_rd] = received_string[usb_rx_ring_rd_main]; // USB_Char_Rx -> received_String
    if (main_received_string[process_char_rd] == '\'' || ready) { // find the first '
      if (main_received_string[process_char_rd] == '\'' || (process_char_rd + TWO) == NINE) { // find the second '
        ready ^= ONE;
      }
      main_string_disp[process_char_rd + TWO] = main_received_string[process_char_rd];
      ssid[process_char_rd + TWO] = main_received_string[process_char_rd];
      process_char_rd++;
      if (!ready) {
        update_screen(DISPLAY_LINE_TOP, main_string_disp);
        process_char_rd = RESET_STATE;
        ready = RESET_STATE;
        if (++usb_rx_ring_rd_main >= (SMALL_RING_SIZE)){
          usb_rx_ring_rd_main = BEGINNING; // Circular buffer back to beginning
        } 
        return true;
      }
    }
    if (++usb_rx_ring_rd_main >= (SMALL_RING_SIZE)){
      usb_rx_ring_rd_main = BEGINNING; // Circular buffer back to beginning
    }     
  }

  return false;
}

char process_character_IPADDR(void) { // find the IP address from the intial startup
  
  if (usb_rx_ring_rd != usb_rx_ring_rd_main) {

    
    main_received_string[process_char_rd] = received_string[usb_rx_ring_rd_main]; // main recieved string ->received string
    
    switch(ip_addr_state) {
      
      case FIND_UP: // finds the phrase "Up at:"
        if (main_received_string[process_char_rd] == 'U' || ready) {
          ready = ONE;
          if (main_received_string[process_char_rd] == ':') {
            ip_addr_state = FIND_MIDDLE_IP;
            process_char_rd = RESET_STATE;
            ready = RESET_STATE;
          } else {
            process_char_rd++;
          }
        }
        break;
      
      case FIND_MIDDLE_IP: // finds the first two numbers in the IP
          ip_addr_middle_2[process_char_rd + TWO] = main_received_string[process_char_rd];
          
          if (main_received_string[process_char_rd] == '.' && process_char_rd >= FOUR) {
            ip_addr_state = FIND_BOTTOM_IP;
            process_char_rd = RESET_STATE;
            update_screen(DISPLAY_LINE_MIDDLE_2, ip_addr_middle_2);
          } 
          process_char_rd++;
        break;
      
      case FIND_BOTTOM_IP: // finds the last two numbers in the IP
        if (main_received_string[process_char_rd] == '\r') {
          update_screen(DISPLAY_LINE_BOTTOM, ip_addr_bottom);
          process_char_rd = RESET_STATE;
          ready = RESET_STATE;
          found_ip = true;
        } else {
          ip_addr_bottom[process_char_rd + ONE] = main_received_string[process_char_rd];
          process_char_rd++;
        }
        break;
    }
    if (++usb_rx_ring_rd_main >= (SMALL_RING_SIZE)){
      usb_rx_ring_rd_main = BEGINNING; // Circular buffer back to beginning
    } 
    if (found_ip) { // returns true if the IP was found, flase if otherwise
      return true;
    } else { 
      return false;
    }
  }
  return false;
}