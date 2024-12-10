

/**
 * main.c
 * Our main code for driving the CyBot
 */

#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "open_interface.h"
#include "Timer.h"
#include "lcd.h"
#include "movement.h"
#include "uart.h"
#include "adc.h"
#include "servo.h"
#include "ping.h"
#include "Graph.h"
#include "navigate.h"
#include "button.h"
#include <math.h>
#include <time.h>

int main(void)
{
    timer_init(); 	// Initialize timer for delays
    lcd_init();		// Initialize LCD display	
    uart_init();	// Initialize uart for communication
    servo_init();	// Servo motor
    button_init();
    adc_init();		// Initialize adc for sensor readings
    ping_interrupt_init();
    oi_t *sensor = oi_alloc();  	// Allocate memory for CyBot sensor data
    oi_init(sensor);
    servo_mode(0);		// Set the servo to the initial position (0 degrees)
    srand(time(0));


    // Variables for navigation and field mapping	
	
    int coordinate_count = 0;
    Coordinate interim_coord;
    bumpy bump;
    Field field;
    int result;
    /*

    In a loop:
        Find the interim coordinate
        Send the interim coordinate
        Recieve and execute instructions for how to move
        If routine not complete
            check what happened
            if cliff_sensor was hit
                communicate based on if hole or border
                backup
            if hit small object
                send field of that pillar
                backup

    */
//     Calibrate cliff-sensor
//    while (1) {
//        lcd_printf("S: %d\n", sensor->cliffLeftSignal);
//        oi_update(sensor);
//        timer_waitMillis(1000);
//    }
    int random_i_x;
    int random_i_y;
    Coordinate target_coord;
    while (1) {
        servo_mode(0);
        timer_waitMillis(5);
        random_i_x = rand() % 251;
        random_i_y = rand() % 251;
        set_cybot_coords(0, 0);
        set_cybot_heading(degrees_to_radians(45));
        send_bot_pos();		// Send Cybot's current position to the controller
        field = scan();		// Scan surroundings
        send_field(field);	// Send scanned field data
        // Set coordinate to random coordinate in front of the bot
        target_coord.x = random_i_x;
        target_coord.y = random_i_y;
        send_interim_coordinate(target_coord); // Send the coordinate to the GUI
        bump = receive_and_execute(sensor);		// Execute received movement instructions

        // Manage if the step was not complete
        if (!bump.complete) {
            uint8_t edge = getEdgeTouching(sensor);
            // Check if the Cybot is touching a field edge
            if (edge) {
                coordinate_count++; // Move to next coordinate
                move_backwards(sensor, 5, &cybot_pose);
                turn_to_angle(sensor, 160, &cybot_pose);
            } else {
                // Check for every other event
                result = manage_not_complete(sensor, interim_coord);
                // If the target was reached move into it and send x to the GUI
                if (result) {
                    move_forward(sensor, 10, &cybot_pose);
                    uart_sendChar('x');  // Notify the controller 
                    break;
                }
            }
        }
    }
    oi_free(sensor);    // Free memory allocated for Cybot sensors
	return 0;
}
