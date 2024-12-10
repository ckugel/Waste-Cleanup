/*
 * servo.c
 *
 *  Created on: Nov 5, 2024
 *      Author: dwitty7
 */

#include "servo.h"
#include "button.h"
#include "lcd.h"

/**
 * Initializes the servo by configuring Timer 1B and GPIO Port B.
 *
 * Sets up the timer for PWM mode to control the servo's position.
 */
void servo_init() {
    // Enable clock for GPIO Port B and Timer 1.
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x02;

   // Configure PB5 as a digital output with alternate function for Timer 1B
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x700000;

    // Configure Timer 1B for PWM mode.
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

/**
 * Sets the servo position based on the specified angle in degrees.
 *
 * Calculates the pulse width corresponding to the angle and updates the timer match value.
 *
 * @param degrees The desired angle (0 to 180 degrees).
 */
void servo_mode(float degrees) {
    // Convert the angle to a pulse width using a linear mapping.
    // The formula is derived from servo timing requirements:
    // - 0 degrees corresponds to ~1 ms pulse width.
    // - 180 degrees corresponds to ~2 ms pulse width.
    float width = degrees * 157.428889 + 7165.2;

    // Update the timer match value to set the pulse width.
    TIMER1_TBMATCHR_R = ((int) ((TIMER1_TBILR_R + (TIMER1_TBPR_R << 16)) - width)) & 0xFFFF;
    TIMER1_TBPMR_R = (((int) ((TIMER1_TBILR_R + (TIMER1_TBPR_R << 16)) - width)) & 0xFF0000) >> 16;
}

/**
 * Allows the user to manually calibrate the servo using buttons.
 *
 * Buttons are used to increase/decrease the servo angle in fine or coarse increments,
 * and reset to predefined positions (0 or 180 degrees).
 */
void servo_calibrate() {
    int button;
    float degrees = 90;    // Start calibration at 90 degrees (center position).
    servo_mode(degrees);    // Set the servo to the initial position.
    int flag = 0;            // Calibration mode flag (0 = increase, 1 = decrease).
    int button_flag = 1;      // Prevent repeated button presses.
    while (1) {
        button = button_getButton();     // Read the button input.
        if (button_flag) {
            if (flag) {    // Decrease mode.
                if (button == 1) {    // Decrease angle by 1 degree.
                    degrees -= 1;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;    // Prevent repeated presses.
                } else if (button == 2) {    // Decrease angle by 5 degrees.
                    degrees -= 5;
                    servo_mode(degrees);
                    button_flag = 0;
                    lcd_clear();
                } else if (button == 4) {    // Reset to 0 degrees.
                    degrees = 0;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;
                }
            }
            else {    // Increase mode.
                if (button == 1) {    // Increase angle by 1 degree.
                    degrees += 1;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;    // Prevent repeated presses.
                } else if (button == 2) {     // Increase angle by 5 degrees.
                    degrees += 5;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;
                } else if (button == 4) {    // Reset to 180 degrees.
                    degrees = 180;
                    servo_mode(degrees);
                    lcd_clear();
                    button_flag = 0;
                }
            }
            // Display the current pulse width and angle on the LCD.
            lcd_printf("%d, %d", (TIMER1_TBPMR_R << 16) + TIMER1_TBMATCHR_R, (int) degrees);
            
            if (button == 3) {    // Toggle between increase and decrease modes.
                if (flag == 0) {
                    flag = 1;
                } else {
                    flag = 0;
                }
                button_flag = 0;
            }
        }
        // Reset button_flag when no button is pressed.
        if (button == 0) {
            button_flag = 1;
        }
    }
}
