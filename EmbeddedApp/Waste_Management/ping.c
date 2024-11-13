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

void ping_interrupt_init() {
    IntRegister(INT_TIMER3B, TIMER3B_HANDLER);
    IntMasterEnable();
    NVIC_EN1_R |= 0x10;
//    NVIC_PRI9_R |= 0xE0;
    ping_init();
    TIMER3_ICR_R |= 0x400;


}

void TIMER3B_HANDLER() {
    if (TIMER3_MIS_R & 0x400) {
        if (state == LOW) {
            rising_time = TIMER3_TBR_R;
            state = HIGH;
        } else if (state == HIGH){
            falling_time = TIMER3_TBR_R;
            state = DONE;
        }
    }
    TIMER3_ICR_R |= 0x400;
}

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

int ping_read() {
    send_pulse();
    while(state != DONE){};
    state = LOW;
    int dist = ping_get_cm(rising_time, falling_time);
    return dist;
}

int ping_get_cm(int rising, int falling) {
    float total_clock_cycles;
    // Overflow check
    if (rising < falling) {
        num_overflows++;
        int larger_rising = rising + 0xFFFFFF;
        total_clock_cycles = larger_rising - falling;
    } else {
        total_clock_cycles = rising- falling;
    }
    float time = total_clock_cycles / (16 * pow(10, 6));
    int dist = (time * 34000) / 2;
    return dist;
}

