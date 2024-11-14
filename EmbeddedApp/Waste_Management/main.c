

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
#include <math.h>

float dist_data[100];
int ir_data[100];
short toggle;
short setup;
short turned;
int turn_amount;
int forward_amount;
char* start;
char* end;
char raw_data[40];
typedef struct {
    int objectNum;
    int angle;
    int distance;
    int width;
	int radius;
} object;
typedef struct {
    int width;
    int angle;
} gap;
object objects[10];
int left_turn;
int right_turn;
int smallestIndex;
int count;
Coordinate self;
Coordinate target;
int angle_direction;
int gap_angle;
Pillar pillars[20];

void scan_for_objects() {
    short angle;
    short average_dist;
    int index;
    int i;
    int j;
    int start_angle;
    count = 0;
    float scan_one;
    float scan_two;
    float scan_three;
    int detecting_object = 0;
//    for (i = 0; i < strlen(start); i++) {
//        uart_sendChar(start[i]);
//    }
    for (i = 0; i <= 180; i += 2) {
        int sum = 0;
		// Reads the IR value 10 times then averages and converts it to cm
        for (j = 0; j < 10; j++) {
            servo_mode(i);
            sum += adc_read();
            timer_waitMillis(10);
        }
        index = i / 2;
        ir_data[index] = convert_IR_value(sum / 10);\
		// Send the IR data for the angle
        sprintf(raw_data, "%d      %d\n", i, ir_data[index]);
        for (j = 0; j < strlen(raw_data); j++) {
            uart_sendChar(raw_data[j]);
        }
        if (index > 2) {
            if (detecting_object == 0 && (ir_data[index] - ir_data[index - 3] < -25) && ir_data[index] < 70) {
                detecting_object = 1;
                start_angle = i;
            } else if (detecting_object == 1 && (ir_data[index] - ir_data[index - 3] > 25)) {
                detecting_object = 0;
                angle = (start_angle + i) / 2;
                servo_mode(angle);
                timer_waitMillis(500);
                scan_one = ping_read();
                timer_waitMillis(10);
                scan_two = ping_read();
                timer_waitMillis(10);
                scan_three = ping_read();
                timer_waitMillis(10);
                average_dist = (scan_one + scan_two + scan_three) / 3;
                if (average_dist < 75) {
                    objects[count].objectNum = count + 1;
                    objects[count].angle = angle;
                    objects[count].distance = average_dist;
                    objects[count].width = (objects[count].distance) * ((i - start_angle) * (3.14/180));
                    objects[count].radius = objects[count].width / 2;
					count++;
//                    sprintf(raw_data, "Object detected\n\r");
//                    for (j = 0; j < strlen(raw_data); j++) {
//                        uart_sendChar(raw_data[j]);
//                    }
                }
            }
        }
    }

//    for (i = 0; i < strlen(end); i++) {
//        uart_sendChar(end[i]);
//    }
    for (i = 0; i < count; i++) {
       sprintf(raw_data, "%d          %d         %d        %d		%d\n\r", objects[i].objectNum, objects[i].angle, objects[i].distance, objects[i].width, objects.radius);
       for (j = 0; j < strlen(raw_data); j++) {
           uart_sendChar(raw_data[j]);
       }
    }

    // TODO: use the mapping to find the target angle and set it to gap_angle
    // To decide where to turn after the scan


    if (count > 0) {
        setup = 1;
    } else {
//        sprintf(raw_data, "Did not detect any objects");
//        for (j = 0; j < strlen(raw_data); j++) {
//            uart_sendChar(raw_data[j]);
//        }
    }
    sprintf(raw_data, "END\n");
    for (j = 0; j < strlen(raw_data); j++) {
        uart_sendChar(raw_data[j]);
    }
    if (turn_amount == 0) {
        turned = 1;
    }
}
/*
    Find where the best gap to go through is
    Returns the angle of the gap
    Return -1 if no gap is large enough
*/
int identify_gap() {
    int arc;
    gap gaps[count + 1];
    gaps[0].width = objects[0].distance * ((objects[0].angle - (objects[0].width / 2)) * (3.14/180));
    gaps[0].angle = (objects[0].angle - (objects[0].width / 2)) / 2;
    int last_angle = objects[0].angle + (objects[0].width / 2);
    for (i = 1; i < count; i++) {
        arc = (objects[i].angle - (objects[i].width / 2)) - last_angle;
        gaps[i].width = arc * (3.14/180) * fmin(objects[i].distance, objects[i - 1].distance);
        gaps[i].angle = (((objects[i].angle - (objects[i].width / 2)) - last_angle) / 2) + last_angle;
        last_angle = objects[i].angle + (objects[i].width / 2);
    }
    gaps[count + 1].width = (180 - last_angle) * (3.14/180) * objects[count - 1].distance;
    gaps[count + 1].angle = ((180 - last_angle) / 2) + last_angle;

    //TODO: identify gap
}

void face_target() {
    int angle_between = angleBetweenTwoPoints(self, target);
    int angle_to_move_to = angle_between - angle_direction;
    if (angle_to_move_to < 0) {
        turn_counter_clockwise(angle_to_move_to * -1);
    } else {
        turn_clockwise(angle_to_move_to);
    }
}

int main(void)
{
	// Start in a corner and go towards the target
    self.x = 0;
    self.y = 0;
	target.x = 100;
	target.y = 100;
	angle_direction = 0;
    timer_init();
    lcd_init();
    uart_init();
    servo_init();
    adc_init();
    ping_interrupt_init();
	int i;
    int j;
    char* start = "Degrees       IR Distance (cm)\n\r";
    char* end = "Object#    Angle       Distance  Width\n\r";
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    servo_mode(0);
    toggle = 0;
    setup = 0;
    turned = 0;
    char byte;
    while (1) {
        byte = 0;
        byte = uart_receive();
        else if (byte == 'h' && setup == 0) {
            toggle = 1;
            scan_for_objects();
        }
        else if (byte == 'h' && setup == 1 && turned == 0) {
            if (gap_angle > 90) {
                turn_counter_clockwise(sensor_data, gap_angle - 90);
            } else {
                turn_clockwise(sensor_data, gap_angle);
            }
            turned = 1;
        }

        else if (byte == 'h' && setup == 1 && turned == 1) {
            move_forward(sensor_data, 5);
			// Went forward 8 cm
            forward_count++;
            end = "Moved Forward\r\n";
            for (i = 0; i < strlen(end); i++) {
                uart_sendChar(end[i]);
            }
            turned = 0;
            setup = 0;
        }
        if (byte == 'q') {
           break;
        }
        oi_update(sensor_data);
       if (sensor_data->bumpLeft) {
           sprintf(raw_data, "Detected object in path\n\rGoing around it\n\r");
           for (j = 0; j < strlen(raw_data); j++) {
               uart_sendChar(raw_data[j]);
           }
		   // Moving around the object
           move_backward(sensor_data, 15);
           turn_clockwise(sensor_data, 64);
           move_forward(sensor_data, 25);
           turn_counter_clockwise(sensor_data, 64);
           move_forward(sensor_data, 40);
           turn_counter_clockwise(sensor_data, 32);
//            sprintf(raw_data, "Rescan\n\r");
//            for (j = 0; j < strlen(raw_data); j++) {
//                uart_sendChar(raw_data[j]);
//            }
			// Configure it to scan again
           setup = 0;
           turned = 0;
           forward_count = 0;
           turn_count = 0;
//            if (forward_count > forward_amount) {
//                turn_clockwise(sensor_data, 128);
//                forward_count = forward_count - forward_amount;
//            }
       }
       if (sensor_data->bumpRight) {
           sprintf(raw_data, "Detected object in path\n\rGoing around it\n\r");
           for (j = 0; j < strlen(raw_data); j++) {
               uart_sendChar(raw_data[j]);
           }
		   // Moving around the object
           move_backward(sensor_data, 15);
           turn_counter_clockwise(sensor_data, 64);
           move_forward(sensor_data, 25);
           turn_clockwise(sensor_data, 64);
           move_forward(sensor_data, 40);
           turn_clockwise(sensor_data, 32);
//            sprintf(raw_data, "Rescan\n\r");
//            for (j = 0; j < strlen(raw_data); j++) {
//                uart_sendChar(raw_data[j]);
//            }
			// Configure it to scan again
           setup = 0;
           turned = 0;
           forward_count = 0;
           turn_count = 0;
//            if (forward_count > forward_amount) {
//                turn_clockwise(sensor_data, 128);
//                forward_count = forward_count - forward_amount;
//            }
       }
        if (forward_amount <= forward_count) {
           sprintf(raw_data, "Reached Target\n\r");
           for (j = 0; j < strlen(raw_data); j++) {
               uart_sendChar(raw_data[j]);
           }
        }
    }
    oi_free(sensor_data);
	return 0;
}
