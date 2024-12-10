/*
 * adc.c
 *
 *  Created on: Oct 23, 2024
 *      Author: ckugel05
 */

#include "adc.h"

void adc_init(){

    SYSCTL_RCGCADC_R |= 0x1;
    SYSCTL_RCGCGPIO_R |= 0x2;

    GPIO_PORTB_AFSEL_R |= 0x10;

    GPIO_PORTB_DIR_R   &= ~0x10;
    GPIO_PORTB_AMSEL_R |= 0x10;

    ADC0_ACTSS_R &= ~0xF;
    ADC0_EMUX_R &= ~0xF;
    ADC0_SSMUX0_R = (ADC0_SSMUX0_R & ~0xF) | 0xA;
    ADC0_SSCTL0_R = (ADC0_SSCTL0_R & ~0xF) | 0x6;



    ADC0_ACTSS_R |= 0x1;
    ADC0_PSSI_R |= 0x1;
}

int adc_read(){
    ADC0_PSSI_R |= 0x1;
    int adc_output = ADC0_SSFIFO0_R;
    return adc_output;
}
