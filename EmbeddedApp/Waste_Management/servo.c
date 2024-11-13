/*
 * servo.c
 *
 *  Created on: Nov 5, 2024
 *      Author: dwitty7
 */

#include "servo.h"
#include "button.h"
#include "lcd.h"

void servo_init() {
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x02;

    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x700000;

    TIMER1_CTL_R &= ~0x0100;
    TIMER1_CFG_R |= 0x04;
    TIMER1_TBMR_R |= 0x0A;
    // How long period should be
    TIMER1_TBILR_R = 500000 & 0xFFFF;
    TIMER1_TBPR_R = (500000 & 0xFF0000) >> 16;
    // Configure pulse width
    TIMER1_TBMATCHR_R = ((TIMER1_TBILR_R + (TIMER1_TBPR_R << 16)) - 16000) & 0xFFFF;
    TIMER1_TBPMR_R = (((TIMER1_TBILR_R + (TIMER1_TBPR_R << 16)) - 16000) & 0xFF0000) >> 16;
    TIMER1_CTL_R |= 0x0100;
}

void servo_mode(float degrees) {
    float width = degrees * 157.428889 + 7165.2;
    TIMER1_TBMATCHR_R = ((int) ((TIMER1_TBILR_R + (TIMER1_TBPR_R << 16)) - width)) & 0xFFFF;
    TIMER1_TBPMR_R = (((int) ((TIMER1_TBILR_R + (TIMER1_TBPR_R << 16)) - width)) & 0xFF0000) >> 16;
}

void servo_calibrate() {
    int button;
    float degrees = 90;
    servo_mode(degrees);
    int flag = 0;
    int button_flag = 1;
    while (1) {
        button = button_getButton();
        if (button_flag) {
            if (flag) {
                if (button == 1) {
                    degrees -= 1;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;
                } else if (button == 2) {
                    degrees -= 5;
                    servo_mode(degrees);
                    button_flag = 0;
                    lcd_clear();
                } else if (button == 4) {
                    degrees = 0;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;
                }
            }
            else {
                if (button == 1) {
                    degrees += 1;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;
                } else if (button == 2) {
                    degrees += 5;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;
                } else if (button == 4) {
                    degrees = 180;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;
                }
            }
            lcd_printf("%d, %d", (TIMER1_TBPMR_R << 16) + TIMER1_TBMATCHR_R, (int) degrees);
            if (button == 3) {
                if (flag == 0) {
                    flag = 1;
                } else {
                    flag = 0;
                }
                button_flag = 0;
            }
        }
        if (button == 0) {
            button_flag = 1;
        }
    }
}
