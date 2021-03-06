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

//*************************************************
// Functions' declaration
//*************************************************
void interrupt high_priority isr_high(void);
void interrupt low_priority isr_low(void);
void clock_config();
void interr_config();
void ports_config();
void comparator_config();

//*************************************************
// Main
//*************************************************
void main(void)
{
    clock_config();
    interr_config();
    ports_config();
    comparator_config();
    
    int delay = 30000;

    while(1)
    {
        LED2 = 1;
        for(int i=0; i<delay; i++){}
        LED2 = 0;
        for(int i=0; i<delay; i++){}
        SLEEP();
    }

    return;
}

//*************************************************
// Functions' definition
//*************************************************

void interrupt high_priority isr_high(void) // Interrupt service routine high priority
{
	if (PIE2bits.CMIE && PIR2bits.CMIF)
	{
    	PIR2bits.CMIF = 0;
    	LED1 = C1OUT;
	}
}

void clock_config()
{
    OSCCONbits.IRCF = 7; // Internal Oscillator Frequency (8 MHz)
}

void interr_config()
{
    RCONbits.IPEN = 1; // Interrupt priority enable
    INTCONbits.PEIE = 1; // Enable low priority interrupts
    INTCONbits.GIE = 1; // Enable high priority interrupts
}

void ports_config()
{
    TRISBbits.TRISB0 = 0; // Output
    TRISBbits.TRISB1 = 0; // Output
    
    LED1 = 0; // Initialize in 0
    LED2 = 0; // Initialize in 0
}

void comparator_config()
{
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
}