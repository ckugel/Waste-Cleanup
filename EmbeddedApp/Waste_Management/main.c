

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
    timer_init();
    lcd_init();
    uart_init();
    servo_init();
    adc_init();
    ping_interrupt_init();
    oi_t *sensor = oi_alloc();
    oi_init(sensor);
    servo_mode(0);
    set_cybot_coords(120, 211);
    // cybot.pose.heading = ; //Needs to check the IMU
    find_east(sensor);
    find_north(sensor);
    int coordinate_count = 0;
    Coordinate interim_coord;
    bumpy bump;
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
        interim_coord = get_interim_coordinate(cybot_pose.xy, target_coords[coordinate_count]);
        coordinate_count++;
        send_interim_coordinate(interim_coord);
        bump = receive_and_execute(sensor);
        if (!bump.complete) {
            uint8_t hole = getHoleTouching(sensor);
            uint8_t target = getTargetTouching(sensor);
            if (target) {
                
            } else if (hole) {
                send_hole_point(sensor);
                move_backwards(5);
            } else if (sensor->bumpRight | sensor->bumpLeft) {
                
                move_backwards(5);
            }
        }
    }
    oi_free(sensor);
	return 0;
}
