//------------------------------------------------------------------------------
//
//  Description: This file contains the interrupts for the ADC sensors, serialization
//               and Wifi disconnect condition
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
#include "ports.h"
#include "stdbool.h"

//ADC vars
unsigned int ADC_Thumb;
unsigned int ADC_Right_Detector;
unsigned int ADC_Left_Detector;
char can_transmit;

//Serial Vars
volatile unsigned int usb_rx_ring_wr;
volatile char USB_Char_Rx[SMALL_RING_SIZE];


//ADC Interrupts

#pragma vector = ADC12_B_VECTOR
__interrupt void ADC12_ISR(void){
  switch(__even_in_range(ADC12IV, ADC12IV__ADC12RDYIFG)){
  case ADC12IV__NONE: break; // Vector 0: No interrupt
  case ADC12IV__ADC12OVIFG: break; // Vector 2: ADC12MEMx Overflow
  case ADC12IV__ADC12TOVIFG: break; // Vector 4: Conversion time overflow
  case ADC12IV__ADC12HIIFG: break; // Vector 6: ADC12BHI
  case ADC12IV__ADC12LOIFG: break; // Vector 8: ADC12BLO
  case ADC12IV__ADC12INIFG: break; // Vector 10: ADC12BIN
  case ADC12IV__ADC12IFG0: break; // Vector 12: ADC12MEM0 Interrupt
  case ADC12IV__ADC12IFG1: break; // Vector 14: ADC12MEM1 Interrupt
  case ADC12IV__ADC12IFG2: // Vector 16: ADC12MEM2 Interrupt
    
    ADC_Thumb = ADC12MEM0; // A02 ADC10INCH_2 - THumbwheel values
    ADC_Right_Detector = ADC12MEM1 >> 2; // A05 ADC10INCH_4 - Right Detector Values
    ADC_Left_Detector = ADC12MEM2 >> 2; // A04 ADC10INCH_5 - Left Detector Values
    break;
    
  case ADC12IV__ADC12IFG3: break; // Vector 18: ADC12MEM3
  case ADC12IV__ADC12IFG4: break; // Vector 20: ADC12MEM4
  case ADC12IV__ADC12IFG5: break; // Vector 22: ADC12MEM5
  case ADC12IV__ADC12IFG6: break; // Vector 24: ADC12MEM6
  case ADC12IV__ADC12IFG7: break; // Vector 26: ADC12MEM7
  case ADC12IV__ADC12IFG8: break; // Vector 28: ADC12MEM8
  }
}

//Serial Interrupts

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
  unsigned int temp;
  
  switch(__even_in_range(UCA0IV,HEX_EIGHT)){
    
    case ZERO: // Vector 0 - no interrupt
      break;
    case UCA0_RX_IFG: // Vector 2 - RXIFG
      if (!can_transmit) {
        can_transmit = true;
      }
      temp = usb_rx_ring_wr;
      USB_Char_Rx[temp] = UCA0RXBUF; // RX -> USB_Char_Rx character
      UCA3TXBUF = USB_Char_Rx[temp];
      if (++usb_rx_ring_wr >= (SMALL_RING_SIZE)){
        usb_rx_ring_wr = BEGINNING; // Circular buffer back to beginning
      }
      break;
    case FOUR: // Vector 4 – TXIFG
      break;
    default: break;
  }
}

#pragma vector=USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void){
  unsigned int temp;
  
  switch(__even_in_range(UCA3IV,HEX_EIGHT)){
    
    case ZERO: // Vector 0 - no interrupt
      break;
    case UCA3_RX_IFG: // Vector 2 - RXIFG
      temp = usb_rx_ring_wr;
      USB_Char_Rx[temp] = UCA3RXBUF; // RX -> USB_Char_Rx character
      UCA0TXBUF = USB_Char_Rx[temp];
      if (++usb_rx_ring_wr >= (SMALL_RING_SIZE)){
        usb_rx_ring_wr = BEGINNING; // Circular buffer back to beginning
      }
      break;
    case FOUR: // Vector 4 – TXIFG
      break;
    default: break;
  }
}


//IOT Link Interrupt
#pragma vector=PORT3_VECTOR
__interrupt void IOT_LINK_Interrupt(void) { //Interrupt for Wifi Disconnect
  
  if (P3IFG & IOT_RESET) {
    reset_motor_count();
    Motors_OFF();
    Init_IOT(false);
    scan_for_wifi();
  }
  
}