/*
 * @file ShiftLights.c
 *
 * @processor: PIC18F46K80
 * @compiler: Microchip C18
 * @author: Andrew Mass
 * @author: George Schwieters
 */
#include <stdlib.h>
#include "p18F46K80.h"
#include "timers.h"
#include "ShiftLights.h"
#include "ECAN.h"

//TODO: What to do if millis isn't updating.
//TODO: What to do if rmp isn't updating.
//TODO: Add "animations". (Scrolling, expanding, alternating, etc.)
//TODO: Add AMERICA sequence. Make it look 'murican.
//TODO: Use less rev ranges. Limited by distinct number of LEDs.
//TODO: Optimize everything.

/*******************************************
 * PIC18F46K80 Configuration Bits Settings *
 *******************************************/

// CONFIG1L
#pragma config RETEN = OFF      // VREG Sleep Enable bit (Ultra low-power regulator is Disabled (Controlled by REGSLP bit))
#pragma config INTOSCSEL = HIGH // LF-INTOSC Low-power Enable bit (LF-INTOSC in High-power mode during Sleep)
#pragma config SOSCSEL = HIGH   // SOSC Power Selection and mode Configuration bits (High Power SOSC circuit selected)
#pragma config XINST = OFF      // Extended Instruction Set (Disabled)

// CONFIG1H
#ifdef INTERNAL
  #pragma config FOSC = INTIO2  // Oscillator (Internal RC oscillator)
#else
  #pragma config FOSC = HS1     // Oscillator (HS oscillator (Medium power, 4 MHz - 16 MHz))
#endif

#pragma config PLLCFG = ON      // PLL x4 Enable bit (Enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = ON        // Internal External Oscillator Switch Over Mode (Enabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power Up Timer (Disabled)
#pragma config BOREN = OFF      // Brown Out Detect (Disabled in hardware, SBOREN disabled)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (1.8V)
#pragma config BORPWR = ZPBORMV // BORMV Power level (ZPBORMV instead of BORMV is selected)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WDTPS = 1048576  // Watchdog Postscaler (1:1048576)

// CONFIG3H
#pragma config CANMX = PORTB    // ECAN Mux bit (ECAN TX and RX pins are located on RB2 and RB3, respectively)
#pragma config MSSPMSK = MSK7   // MSSP address masking (7 Bit address masking mode)
#pragma config MCLRE = ON       // Master Clear Enable (MCLR Enabled, RE3 Disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Overflow Reset (Enabled)
#pragma config BBSIZ = BB2K     // Boot Block Size (2K word Boot Block size)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protect 00800-03FFF (Disabled)
#pragma config CP1 = OFF        // Code Protect 04000-07FFF (Disabled)
#pragma config CP2 = OFF        // Code Protect 08000-0BFFF (Disabled)
#pragma config CP3 = OFF        // Code Protect 0C000-0FFFF (Disabled)

// CONFIG5H
#pragma config CPB = OFF        // Code Protect Boot (Disabled)
#pragma config CPD = OFF        // Data EE Read Protect (Disabled)

// CONFIG6L
#pragma config WRT0 = OFF       // Table Write Protect 00800-03FFF (Disabled)
#pragma config WRT1 = OFF       // Table Write Protect 04000-07FFF (Disabled)
#pragma config WRT2 = OFF       // Table Write Protect 08000-0BFFF (Disabled)
#pragma config WRT3 = OFF       // Table Write Protect 0C000-0FFFF (Disabled)

// CONFIG6H
#pragma config WRTC = OFF       // Config. Write Protect (Disabled)
#pragma config WRTB = OFF       // Table Write Protect Boot (Disabled)
#pragma config WRTD = OFF       // Data EE Write Protect (Disabled)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protect 00800-03FFF (Disabled)
#pragma config EBTR1 = OFF      // Table Read Protect 04000-07FFF (Disabled)
#pragma config EBTR2 = OFF      // Table Read Protect 08000-0BFFF (Disabled)
#pragma config EBTR3 = OFF      // Table Read Protect 0C000-0FFFF (Disabled)

// CONFIG7H
#pragma config EBTRB = OFF      // Table Read Protect Boot (Disabled)

/********************************
 * Global Variable Declarations *
 ********************************/

volatile unsigned int millis; // holds timer0 rollover count
volatile unsigned int rpm; // Holds engine RPM data from CAN
typedef enum {true, false};
#define true 1;
#define false 0;
typedef char bool;
bool errorVolts = 0, errorRPM = 0, errorOilTemp = 0, errorEngineTemp = 0, changedErrorState;


// ECAN variables
unsigned long id; // holds CAN msgID
BYTE data[8]; // holds CAN data bytes
BYTE dataLen; // holds number of CAN data bytes
ECAN_RX_MSG_FLAGS flags; // holds information about recieved message

/**************
 * Interrupts *
 **************/

/*
 * Function to service interrupts.
 *
 * Transmits messages on CAN.
 * Reloads the timer0 registers and increments millis.
 * Resets the interrupt flags after servicing them.
 */
#pragma code high_vector = 0x08

void high_vector(void) {
  _asm goto high_isr _endasm
}
#pragma code

#pragma interrupt high_isr

void high_isr(void) {

  // Check for timer0 rollover indicating a millisecond has passed
  if(INTCONbits.TMR0IF) {
    INTCONbits.TMR0IF = 0;
    WriteTimer0(0x85); // Load timer registers (0xFF (max val) - 0x7D (125) = 0x82)
    millis++;
  }

  // Check for recieved CAN message
  if(PIR5bits.RXB1IF) {
    PIR5bits.RXB1IF = FALSE; // reset the flag
    // get data from recieve buffer
    ECANReceiveMessage(&id, data, &dataLen, &flags);
    if(id == RPM_ID) {
      ((BYTE*) & rpm)[0] = data[RPM_BYTE + 1];
      ((BYTE*) & rpm)[1] = data[RPM_BYTE];
    }
  }
}

/*************
 * Functions *
 *************/

/*
 * Sets specified LED to specified color.
 *
 * @param led The index of the led to change.
 * @param color The color value to change the LED to. Color values come
 *   from the header file.
 */
void setLedToColor(unsigned char led, unsigned char color) {
  if(led == 0) {
    RED0_LAT = color & RED ? 1 : 0;
    GREEN0_LAT = color & GREEN ? 1 : 0;
    BLUE0_LAT = color & BLUE ? 1 : 0;
  } else if(led == 1) {
    RED1_LAT = color & RED ? 1 : 0;
    GREEN1_LAT = color & GREEN ? 1 : 0;
    BLUE1_LAT = color & BLUE ? 1 : 0;
  } else if(led == 2) {
    RED2_LAT = color & RED ? 1 : 0;
    GREEN2_LAT = color & GREEN ? 1 : 0;
    BLUE2_LAT = color & BLUE ? 1 : 0;
  } else if(led == 3) {
    RED3_LAT = color & RED ? 1 : 0;
    GREEN3_LAT = color & GREEN ? 1 : 0;
    BLUE3_LAT = color & BLUE ? 1 : 0;
  } else if(led == 4) {
    RED4_LAT = color & RED ? 1 : 0;
    GREEN4_LAT = color & GREEN ? 1 : 0;
    BLUE4_LAT = color & BLUE ? 1 : 0;
  }
}

/*
 * Sets all LEDs to specified color.
 *
 * @param color The color value to change the LED to. Color values come
 *   from the header file.
 */
void set_all(unsigned char color) {
  unsigned char i = 0;
  for(; i < 5; i++) {
    setLedToColor(i, color);
  }
}

/*
 * Sets lights to NONE or REV_COLOR based on parameter.
 *
 * @param max The highest number LED to illuminate.
 */
void set_lights(unsigned char max) {
  /*
  if(max > 0) send(DIG0, BLUE);
  else send(DIG0, NONE);

  if(max > 1) send(DIG1, BLUE);
  else send(DIG1, NONE);

  if(max > 2) send(DIG2, BLUE);
  else send(DIG2, NONE);

  if(max > 3) send(DIG3, BLUE);
  else send(DIG3, NONE);

  if(max > 4) send(DIG4, BLUE);
  else send(DIG4, NONE);

  if(max > 5) send(DIG5, BLUE);
  else send(DIG5, NONE);

  if(max > 6) send(DIG6, BLUE);
  else send(DIG6, NONE);

  if(max > 7) send(DIG7, BLUE);
  else send(DIG7, NONE);
   */
}

//typedef enum {Dtrue, false} _Bool;
/*
 Function checks the volts from the battery.
 Outputs: True (1) if volts are in balance, above the threshold.
          False(0) if volts are below the threshold.
 */
void checkVolts(){
    int level = 13; //get the value here.
    if(level < 13){
        errorVolts = 1; //Battery is low, error flag set to true
    }
    return 0;
}
void excess(int type){
    switch(type){
        case 0:
            //excess oil temp
            errorOilTemp = 0;
            break;
        case 1:
            //excess engine temp
            errorEngineTemp = 0;
            break;
        default:
            //code error
            break;

    }
}
void display(){
    if(changedErrorState){
        if(errorRPM && (!errorOilTemp && !errorEngineTemp && !errorVolts)){
            //Only RPM Error
        }else if(errorOilTemp && (!errorRPM && !errorEngineTemp && !errorVolts)){
            //only Oil Temp error
        }else if(errorEngineTemp && (!errorOilTemp && !errorRPM && !errorVolts)){
            //only Engine Temp
        }else if(errorVolts && (!errorOilTemp && !errorEngineTemp && !errorRPM)){
            //only Battery error
        }else{
            //Multiple Error Singles
        }
    }else{
        //Normal RPM display here
    }
}

/*
 * Function to assert unused pins to eliminate sources of noise and reduce MCU
 * power draw.
 *
 * Configuration registers will be modified.
 */
void init_unused_pins() {

  // Configure to outputs.

  // TRISAbits.TRISA0 = OUTPUT; BLUE0
  // TRISAbits.TRISA1 = OUTPUT; GREEN0
  // TRISAbits.TRISA2 = OUTPUT; RED0
  TRISAbits.TRISA3 = OUTPUT;
  TRISAbits.TRISA5 = OUTPUT;
  // TRISAbits.TRISA6 = OUTPUT; OSC2
  // TRISAbits.TRISA7 = OUTPUT; OSC1

  TRISBbits.TRISB0 = OUTPUT;
  TRISBbits.TRISB1 = OUTPUT;
  // TRISBbits.TRISB2 = OUTPUT; CANTX
  // TRISBbits.TRISB3 = OUTPUT; CANRX
  TRISBbits.TRISB4 = OUTPUT;
  TRISBbits.TRISB5 = OUTPUT;
  // TRISBbits.TRISB6 = OUTPUT; PGC
  // TRISBbits.TRISB7 = OUTPUT; PGD

  // TRISCbits.TRISC0 = OUTPUT; SOSC0
  // TRISCbits.TRISC1 = OUTPUT; SOSC1
  // TRISCbits.TRISC2 = OUTPUT; BLUE1
  // TRISCbits.TRISC3 = OUTPUT; GREEN1
  // TRISCbits.TRISC4 = OUTPUT; BLUE3
  // TRISCbits.TRISC5 = OUTPUT; GREEN3
  // TRISCbits.TRISC6 = OUTPUT; TERM
  // TRISCbits.TRISC7 = OUTPUT; RED3

  // TRISDbits.TRISD0 = OUTPUT; RED1
  // TRISDbits.TRISD1 = OUTPUT; BLUE2
  // TRISDbits.TRISD2 = OUTPUT; GREEN2
  // TRISDbits.TRISD3 = OUTPUT; RED2
  // TRISDbits.TRISD4 = OUTPUT; BLUE4
  // TRISDbits.TRISD5 = OUTPUT; GREEN4
  // TRISDbits.TRISD6 = OUTPUT; RED4
  TRISDbits.TRISD7 = OUTPUT;

  TRISEbits.TRISE0 = OUTPUT;
  TRISEbits.TRISE1 = OUTPUT;
  TRISEbits.TRISE2 = OUTPUT;
  // TRISEbits.TRISE3 = OUTPUT; MCLR

  // Set pins low.

  // LATAbits.LATA0 = 0;
  // LATAbits.LATA1 = 0;
  // LATAbits.LATA2 = 0;
  LATAbits.LATA3 = 0;
  LATAbits.LATA5 = 0;
  // LATAbits.LATA6 = 0;
  // LATAbits.LATA7 = 0;

  LATBbits.LATB0 = 0;
  LATBbits.LATB1 = 0;
  // LATBbits.LATB2 = 0;
  // LATBbits.LATB3 = 0;
  LATBbits.LATB4 = 0;
  LATBbits.LATB5 = 0;
  // LATBbits.LATB6 = 0;
  // LATBbits.LATB7 = 0;

  // LATCbits.LATC0 = 0;
  // LATCbits.LATC1 = 0;
  // LATCbits.LATC2 = 0;
  // LATCbits.LATC3 = 0;
  // LATCbits.LATC4 = 0;
  // LATCbits.LATC5 = 0;
  // LATCbits.LATC6 = 0;
  // LATCbits.LATC7 = 0;

  // LATDbits.LATD0 = 0;
  // LATDbits.LATD1 = 0;
  // LATDbits.LATD2 = 0;
  // LATDbits.LATD3 = 0;
  // LATDbits.LATD4 = 0;
  // LATDbits.LATD5 = 0;
  // LATDbits.LATD6 = 0;
  LATDbits.LATD7 = 0;

  LATEbits.LATE0 = 0;
  LATEbits.LATE1 = 0;
  LATEbits.LATE2 = 0;
  // LATEbits.LATE3 = 0;
}

/*************
 * Main Loop *
 *************/

void main(void) {

  /*************************
   * Variable Declarations *
   *************************/

  long blink_tmr = 0;

  /*********************
   * Oscillator Set-Up *
   *********************/

#ifdef INTERNAL
  // OSCTUNE
  OSCTUNEbits.INTSRC = 0; // Internal Oscillator Low-Frequency Source Select (1 for 31.25 kHz from 16MHz/512 or 0 for internal 31kHz)
  OSCTUNEbits.PLLEN = 1; // Frequency Multiplier PLL Select (1 to enable)
  OSCTUNEbits.TUN5 = 0; // Fast RC Oscillator Frequency Tuning (seems to be 2's comp encoding)
  OSCTUNEbits.TUN4 = 0; // 011111 = max
  OSCTUNEbits.TUN3 = 0; // ... 000001
  OSCTUNEbits.TUN2 = 0; // 000000 = center (running at calibrated frequency)
  OSCTUNEbits.TUN1 = 0; // 111111 ...
  OSCTUNEbits.TUN0 = 0; // 100000

  // OSCCCON
  OSCCONbits.IDLEN = 1; // Idle Enable Bit (1 to enter idle mode after SLEEP instruction else sleep mode is entered)
  OSCCONbits.IRCF2 = 1; // Internal Oscillator Frequency Select Bits
  OSCCONbits.IRCF1 = 1; // When using HF, settings are:
  OSCCONbits.IRCF0 = 1; // 111 - 16 MHz, 110 - 8MHz (default), 101 - 4MHz, 100 - 2 MHz, 011 - 1 MHz
  OSCCONbits.SCS1 = 0;
  OSCCONbits.SCS0 = 0;

  // OSCCON2
  OSCCON2bits.MFIOSEL = 0;

  while(!OSCCONbits.HFIOFS); // Wait for stable clock
#else
  // OSCTUNE
  OSCTUNEbits.INTSRC = 0; // Internal Oscillator Low-Frequency Source Select (1 for 31.25 kHz from 16MHz/512 or 0 for internal 31kHz)
  OSCTUNEbits.PLLEN = 1; // Frequency Multiplier PLL Select (1 to enable)

  // OSCCCON
  OSCCONbits.SCS1 = 0; // Select configuration chosen oscillator
  OSCCONbits.SCS0 = 0; // SCS = 00

  // OSCCON2
  OSCCON2bits.MFIOSEL = 0;

  while(!OSCCONbits.OSTS); // Wait for stable external clock
#endif

  /*********************
   * Peripherals Setup *
   *********************/

  ANCON0 = 0x00; // Default all pins to digital
  ANCON1 = 0x00; // Default all pins to digital

  // Turn on and configure the TIMER1 oscillator
  OpenTimer0(TIMER_INT_ON & T0_8BIT & T0_SOURCE_INT & T0_PS_1_128);
  WriteTimer0(0x82); // Load timer register
  millis = 0; // Clear milliseconds count
  INTCONbits.TMR0IE = 1; // Turn on timer0 interupts

  TRISCbits.TRISC6 = OUTPUT; // programmable termination
  TERM_LAT = FALSE;

  ECANInitialize(); // setup ECAN

  // interrupts setup
  INTCONbits.GIE = 1; // Global Interrupt Enable (1 enables)
  INTCONbits.PEIE = 1; // Peripheral Interrupt Enable (1 enables)
  RCONbits.IPEN = 0; // Interrupt Priority Enable (1 enables)

  init_unused_pins();

  // Set LED pins as outputs.
  RED0_TRIS = OUTPUT;
  GREEN0_TRIS = OUTPUT;
  BLUE0_TRIS = OUTPUT;
  RED1_TRIS = OUTPUT;
  GREEN1_TRIS = OUTPUT;
  BLUE1_TRIS = OUTPUT;
  RED2_TRIS = OUTPUT;
  GREEN2_TRIS = OUTPUT;
  BLUE2_TRIS = OUTPUT;
  RED3_TRIS = OUTPUT;
  GREEN3_TRIS = OUTPUT;
  BLUE3_TRIS = OUTPUT;
  RED4_TRIS = OUTPUT;
  GREEN4_TRIS = OUTPUT;
  BLUE4_TRIS = OUTPUT;

  // Default all LEDs to off.
  RED0_LAT = 0;
  GREEN0_LAT = 0;
  BLUE0_LAT = 0;
  RED1_LAT = 0;
  GREEN1_LAT = 0;
  BLUE1_LAT = 0;
  RED2_LAT = 0;
  GREEN2_LAT = 0;
  BLUE2_LAT = 0;
  RED3_LAT = 0;
  GREEN3_LAT = 0;
  BLUE3_LAT = 0;
  RED4_LAT = 0;
  GREEN4_LAT = 0;
  BLUE4_LAT = 0;

  // Startup - Show full RED lights for a while before doing anything else.
  blink_tmr = millis;
  while(1) {
    if(millis - blink_tmr < BLINK_TIME)
      set_all(RED);
    else if(millis - blink_tmr < BLINK_TIME * 2)
      set_all(RGB);
    else if(millis - blink_tmr < BLINK_TIME * 3)
      set_all(BLUE);
    else if(millis - blink_tmr < BLINK_TIME * 4)
      set_all(RED);
    else if(millis - blink_tmr < BLINK_TIME * 5)
      set_all(RGB);
    else if(millis - blink_tmr < BLINK_TIME * 6)
      set_all(BLUE);
    else if(millis - blink_tmr < BLINK_TIME * 7)
      set_all(RED);
    else if(millis - blink_tmr < BLINK_TIME * 8)
      set_all(RGB);
    else if(millis - blink_tmr < BLINK_TIME * 9)
      set_all(BLUE);
    else
      break;
  }

  while(1) {
    // Sets certain lights to NONE or REV_COLOR based on rpm value.
    if(rpm >= REV_RANGE_LIMIT) {
      if(millis - blink_tmr < BLINK_TIME)
        set_all(REV_LIMIT_COLOR);
      else if(millis - blink_tmr < BLINK_TIME * 2)
        set_all(NONE);
      else
        blink_tmr = millis;
    } else if(rpm >= REV_RANGE_8) {
      set_lights(8);
    } else if(rpm >= REV_RANGE_7) {
      set_lights(7);
    } else if(rpm >= REV_RANGE_6) {
      set_lights(6);
    } else if(rpm >= REV_RANGE_5) {
      set_lights(5);
    } else if(rpm >= REV_RANGE_4) {
      set_lights(4);
    } else if(rpm >= REV_RANGE_3) {
      set_lights(3);
    } else if(rpm >= REV_RANGE_2) {
      set_lights(2);
    } else if(rpm >= REV_RANGE_1) {
      set_lights(1);
    } else {
      set_all(NONE);
    }
  }
}
