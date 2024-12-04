/*
 * adc.c
 *
 *  Created on: Oct 22, 2024
 *      Author: dwitty7
 */

#include "adc.h"

void adc_init() {
    SYSCTL_RCGCADC_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x02;

    GPIO_PORTB_AFSEL_R |= 0x10;
    GPIO_PORTB_DEN_R |= 0x10;
    GPIO_PORTB_DIR_R &= ~0x10;
    GPIO_PORTB_AMSEL_R |= 0x10;

    ADC0_ACTSS_R &= ~0x01;
    ADC0_EMUX_R = 0x00;
    ADC0_SSMUX0_R |= 0x000A;
    ADC0_SSCTL0_R |= 0x02;
    ADC0_ACTSS_R |= 0x01;
}

int adc_read() {
    ADC0_PSSI_R = 0x01;

    int raw_value = ADC0_SSFIFO0_R;

    return raw_value;
}
