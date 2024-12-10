

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
#include <math.h>

int main(void)
{

	// Start in a corner and go towards the target
    timer_init(); 	// Initialize timer for delays
    lcd_init();		// Initialize LCD display	
    uart_init();	// Initialize uart for communication
    servo_init();	// Servo motor
    adc_init();		// Initialize adc for sensor readings
    ping_interrupt_init();
    oi_t *sensor = oi_alloc();  	// Allocate memory for CyBot sensor data
    oi_init(sensor);
    servo_mode(0);		// Set the servo to the initial position (0 degrees)
    set_cybot_coords(120, 211);		// Set initial coordinates for the CyBot in the field
    // cybot.pose.heading = ; //Needs to check the IMU

    Coordinate target_coords[16] = {
            {5, 5},
            {240, 422},
            {5, 422},
            {240, 5},
            {5, 141},
            {240, 282},
            {5, 422},
            {240, 422},
            {5, 282},
            {240, 141},
            {5, 5},
            {5, 422},
            {120, 5},
            {240, 422},
            {240, 5},
            {5, 5}
        };
//    find_east(sensor);


    if (!north_found) {
//        find_north(sensor);
    }

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
    while (1) {
        send_bot_pos();		// Send Cybot's current position to the controller
        field = scan();		// Scan surroundings
        send_field(field);	// Send scanned field data
        interim_coord = get_interim_coordinate(cybot_pose.xy, target_coords[coordinate_count]);		// Calculate coordinate
        send_interim_coordinate(interim_coord);
        bump = receive_and_execute(sensor);		// Execute received movement instructions

	    
        if (!bump.complete) {
            uint8_t edge = getEdgeTouching(sensor);
            // Check if the Cybot is touching a field edge
            if (edge) {
                coordinate_count++; // Move to next coordinate
            } else {
                result = manage_not_complete(sensor, interim_coord);
                if (result) {
                    uart_sendChar('x');  // Notify the controller 
                    break;
                }
            }
        } else {
		// Update Cybot's position after a successful move
            set_cybot_coords(interim_coord.x, interim_coord.y);
        }
        if (abs(cybot_pose.xy.x - target_coords[coordinate_count].x) < 10 && abs(cybot_pose.xy.y - target_coords[coordinate_count].y) < 10) {
            coordinate_count++;  // Move to next target coordinate
        }
    }
    oi_free(sensor);    // Free memory allocated for Cybot sensors
	return 0;
}
