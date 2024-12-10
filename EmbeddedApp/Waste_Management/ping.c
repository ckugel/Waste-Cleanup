/*
 * ping.c
 *
 *  Created on: Oct 29, 2024
 *      Author: dwitty7
 */


#include "ping.h"
#include "math.h"

volatile enum state; // set by ISR
volatile unsigned int rising_time; // Pulse start time: Set by ISR
volatile unsigned int falling_time; // Pulse end time: Set by ISR
int num_overflows;

/**
 * @brief Initializes the Ping sensor using Timer 3B.
 *
 * Configures GPIO Port B and Timer 3B to measure pulse width for distance calculations.
 */
void ping_init() {
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x08;

    GPIO_PORTB_DEN_R |= 0x08;
//    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_PCTL_R |= 0x7000;

    TIMER3_CTL_R &= ~0x0100;
    TIMER3_CFG_R |= 0x04;
    TIMER3_TBMR_R |= 0x07;
    TIMER3_CTL_R |= 0xC00;
    TIMER3_TBILR_R |= 0xFFFF;
    TIMER3_TBPR_R |= 0xFF;
    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0x100;
    state = LOW;
    num_overflows = 0;
}

/**
 * Initializes interrupts for Timer 3B.
 *
 * Registers the interrupt handler, enables interrupts in the NVIC, and initializes the ping sensor.
 */
void ping_interrupt_init() {
    IntRegister(INT_TIMER3B, TIMER3B_HANDLER);    // Register the ISR for Timer
    IntMasterEnable();                            // Enable global interrupts.
    NVIC_EN1_R |= 0x10;                            // Enable Timer 3B interrupt in NVIC.
//    NVIC_PRI9_R |= 0xE0;
    ping_init();            // Initialize the ping sensor.
    TIMER3_ICR_R |= 0x400;        // Clear any pending interrupt flags.


}


/**
 * Interrupt Service Routine (ISR) for Timer 3B.
 *
 * Handles edge-time capture events. Captures rising and falling edges to calculate pulse width.
 */
void TIMER3B_HANDLER() {
    if (TIMER3_MIS_R & 0x400) {   // Check if the interrupt was caused by a capture event.
        if (state == LOW) {
            rising_time = TIMER3_TBR_R;    // Capture the time of the rising edge.
            state = HIGH;                    // Update the state to HIGH.
        } else if (state == HIGH){
            falling_time = TIMER3_TBR_R;            // Capture the time of the falling edge.
            state = DONE;                    // Update the state to DONE.
        }
    }
    TIMER3_ICR_R |= 0x400;        // Clear the interrupt flag.
}

/**
 *  Sends a trigger pulse to the Ping sensor.
 *
 * Generates a 5 microsecond pulse on PB3 to initiate a distance measurement.
 */
void send_pulse() {
    TIMER3_IMR_R = 0;
    GPIO_PORTB_AFSEL_R &= ~0x08;
    GPIO_PORTB_DIR_R |= 0x08;
    GPIO_PORTB_DATA_R |= 0x08;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= ~0x08;
    GPIO_PORTB_DIR_R &= ~0x08;
    GPIO_PORTB_AFSEL_R |= 0x08;
    TIMER3_ICR_R |= 0x400;
    TIMER3_IMR_R |= 0x400;
}

/**
 * Reads the distance measured by the Ping sensor.
 *
 * Sends a trigger pulse, waits for the measurement to complete, and calculates the distance.
 *
 * @return int The measured distance in centimeters.
 */
int ping_read() {
    send_pulse();
    while(state != DONE){};
    state = LOW;
    int dist = ping_get_cm(rising_time, falling_time);
    return dist;
}



/**
 * Calculates the distance in centimeters based on timer readings.
 *
 * Converts the timer counts into a distance by accounting for clock speed and sound speed.
 *
 * @param rising The timer value at the rising edge.
 * @param falling The timer value at the falling edge.
 * @return int The calculated distance in centimeters.
 */
int ping_get_cm(int rising, int falling) {
    float total_clock_cycles;
    // Check for timer overflow and adjust the rising time if necessary.
    if (rising < falling) {
        num_overflows++;        // Increment overflow counter
        int larger_rising = rising + 0xFFFFFF;        // Adjust the rising time for overflow.
        total_clock_cycles = larger_rising - falling;
    } else {
        total_clock_cycles = rising- falling;        // Normal case: calculate clock cycles.
    }
    // Convert clock cycles to time (seconds) using a 16 MHz clock.
    float time = total_clock_cycles / (16 * pow(10, 6));
    
    // Calculate distance using the speed of sound (340 m/s or 34000 cm/s).
    // Divide by 2 to account for round-trip travel.
    int dist = (time * 34000) / 2;
    return dist;
}

