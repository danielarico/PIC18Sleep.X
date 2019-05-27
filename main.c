/*
 * File:   main.c
 * Author: daniela
 *
 * Created on May 14, 2019, 6:17 PM
 */

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "config.h"

//*************************************************
// Macros definition
//*************************************************
#define LED1 LATBbits.LATB0
#define LED2 LATBbits.LATB1


//*************************************************
// Global variables declaration
//*************************************************
bool timer0_flag = false;
bool tx_finish = false;
bool rx_finish = false;
const uint16_t timer0_start = 3036; // For prescaler 1:16
const uint8_t array_size = 20;
char rx_buffer[array_size];
char rx_char = '\0';
char rx_data = '\0';
char * ptr_rx = rx_buffer;
char * ptr_tx;


//*************************************************
// Functions' declaration
//*************************************************
void interrupt high_priority isr_high(void);
void interrupt low_priority isr_low(void);
void serial_config ();
void init_timer0(void);
void send(char * ptr_array);
void send_next();
void received();
void clean (char * ptr);

//*************************************************
// Main
//*************************************************
void main(void)
{
    OSCCONbits.IRCF = 7; // Internal Oscillator Frequency (8 MHz)

    //--------------------------------------
    // Interrupts configuration
    RCONbits.IPEN = 1; // Interrupt priority enable
    INTCONbits.PEIE = 1; // Enable low priority interrupts
    INTCONbits.GIE = 1; // Enable high priority interrupts

    //--------------------------------------
    // Ports configuration
    TRISBbits.TRISB0 = 0; // Output
    TRISBbits.TRISB1 = 0; // Output
    
    LED1 = 0; // Initialize in 0
    LED2 = 0; // Initialize in 0

    //--------------------------------------
    // Comparator
    
    // 4 inputs multiplexed to 2 comparators (CM2:CM0 = 110)
    CMCONbits.CM2 = 1;
    CMCONbits.CM1 = 1;
    CMCONbits.CM0 = 0;
    //-----------------------

    // For CVref = 2.5V (CVR3:CVR0 = 1100 = 12)
    CVRCONbits.CVR3 = 1;
    CVRCONbits.CVR2 = 1;
    CVRCONbits.CVR1 = 0;
    CVRCONbits.CVR0 = 0;
    //-----------------------
    
    CMCONbits.CIS = 0; // C1 Vin- connects to RA0; C2 Vin- to RA1
    PIE2bits.CMIE = 1; // Enables comparator interrupts

    CVRCONbits.CVREN = 1; // Comparator Voltage Reference enabled
    CVRCONbits.CVRR = 1; // Comparator Vref range selection (low range)
    CVRCONbits.CVRSS = 0; // Comparator ref source CVrsrc = Vdd - Vss

    CVRCONbits.CVROE = 1; // CVref voltage is also output on the RA2 pin
    
    // OSCCONbits.IDLEN = 0; // Sleep mode

    while(1)
    {
        LED2 = 1;
        for(int i=0; i<30000; i++){}
        LED2 = 0;
        for(int i=0; i<30000; i++){}
        SLEEP();
    }

    return;
}

//*************************************************
// Functions' definition
//*************************************************

void interrupt high_priority isr_high(void) // Interrupt service routine high priority
{
    if (INTCONbits.TMR0IF && INTCONbits.TMR0IE) // Timer0 interruption
    {
   	 INTCONbits.TMR0IF = 0;
   	 TMR0 = timer0_start;
   	 timer0_flag = true;
    }
    
	if (PIE2bits.CMIE && PIR2bits.CMIF)
	{
    	PIR2bits.CMIF = 0;
    	LED1 = C1OUT;
	}
}

void init_timer0 (void)
{
    T0CONbits.T08BIT = 0; // 16 bits
    T0CONbits.T0CS = 0; // Internal instruction cycle clock (8 MHz)
    T0CONbits.T0SE = 0; // Increment in rising edge (low to high)
    T0CONbits.PSA = 0; // Timer0 prescaler is assigned
    T0CONbits.T0PS = 3; // Select prescaler (1:256)
    INTCONbits.TMR0IF = 0; // Disable timer0 overflow flag
    INTCON2bits.TMR0IP = 1; // High priority timer0 interrupt
    TMR0 = timer0_start; // To count 0.5s
    T0CONbits.TMR0ON = 1; // Start timer
    INTCONbits.TMR0IE = 1; // Enable timer0 interrupts
}
