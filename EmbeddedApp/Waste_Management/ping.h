/*
 * ping.h
 *
 *  Created on: Oct 29, 2024
 *      Author: dwitty7
 */

#ifndef PING_H_
#define PING_H_

#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

volatile enum {LOW, HIGH, DONE} state; // set by ISR
volatile unsigned int rising_time; // Pulse start time: Set by ISR
volatile unsigned int falling_time; // Pulse end time: Set by ISR
int num_overflows;

void ping_init();
void ping_interrupt_init();
void TIMER3B_HANDLER();
void send_pulse();
int ping_read();
int ping_get_cm(int, int);

#endif /* PING_H_ */
