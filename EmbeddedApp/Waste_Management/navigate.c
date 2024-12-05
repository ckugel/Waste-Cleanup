
#include "navigate.h"

int manage_not_complete(oi_t* sensor, Coordinate interim_coord) {
    uint8_t hole = getHoleTouching(sensor);
    uint8_t target = getTargetTouching(sensor);
    // Set coordinates not accurate because don't know where it stops
    set_cybot_coords(((cybot_pose.xy.x + interim_coord.x) / 2) - 5, ((cybot_pose.xy.y + interim_coord.y) / 2) - 5);
    if (target) {
        return 1;
    } else if (hole) {
        send_hole_point(sensor);
        move_backwards(sensor, 5, &cybot_pose);
    } else if (sensor->bumpRight | sensor->bumpLeft) {
        Field field;
        field.size = 1;
        Pillar pillar[1];
        pillar[0].position = cybot_pose;
        pillar[0].radius = 7.0;
        field.pillars= pillar;
        send_field(field);
        move_backwards(sensor, 5, &cybot_pose);
    }
    return 0;
}

// To be done first
// Will move the Cybot to the east side of the field
void find_east(oi_t* sensor) {
    // Turn the cybot so heading is facing east
    bumpy bump;
    Field field;
    Coordinate interim;
    int result;
    while (1) {
        field = scan();
        send_field(field);
        interim.x = cybot_pose.xy.x + 50;
        interim.y = cybot_pose.xy.y;
        send_interim_coordinate(interim);
        bump = receive_and_execute(sensor);
        if (!bump.complete) {
            uint8_t edge = getEdgeTouching(sensor);
            // Check if edge
            if (edge) {
                // Check IMU for direction
                // If it's North or South need to continue with finding east
                // Also should turn cybot perpendicular to edge or change the heading based on the IMU
                north_found = 1;
                send_edge('N')
                // If it's east or west break
                break;
            } else {
                result = manage_not_complete(sensor, interim);
                if (result) {
                    uart_sendChar('x');
                    return;
                }
            }
        } else {
            set_cybot_coords(interim.x, interim.y);
        }
    }
    // If east was found
        set_cybot_coords(5, cybot_pose.xy.y);
        interim.x = cybot_pose.xy.x + 50;
        send_edge('E');
    // EDGE CASE: west was found
        set_cybot_coords(240, cybot_pose.xy.y);
        interim.x = cybot_pose.xy.x - 50;
        send_edge('W');
    interim.y = cybot_pose.xy.y;
    turn_clockwise(sensor, 180);
    set_cybot_heading(cybot_pose.heading - 3.14);
    // Move a little farther away from the edge
    send_bot_pos();
    field = scan();
    send_field(field);
    send_interim_coordinate(interim);
    bump = receive_and_execute(sensor);
    if (!bump.complete) {
        manage_not_complete(sensor, interim);
    } else {
        set_cybot_coords(interim.x, interim.y);
    }
}

// To be executed after finding the east side
// Needs the Cybot to be turned 
// Will move the Cybot to the north end of the field then send back that the position has been set
void find_north() {
    // Turn the cybot so heading is facing east
    bumpy bump;
    Field field;
    while (1) {
        field = scan();
        send_field(field);
        Coordinate interim;
        interim.x = cybot_pose.xy.x;
        interim.y = cybot_pose.xy.y + 50;
        send_interim_coordinate(interim);
        bump = receive_and_execute(sensor);
        if (!bump.complete) {
            uint8_t edge = getEdgeTouching(sensor);
            // Check if edge
            if (edge) {
                // Check IMU for direction
                // If it's north or south break
                break;
            } else {
                result = manage_not_complete(sensor, interim);
                if (result) {
                    uart_sendChar('x');
                    return;
                }
            }
        } else {
            set_cybot_coords(interim.x, interim.y);
        }
    }
    // If north was found
        set_cybot_coords(cybot_pose.xy.x, 5);
        send_edge('N');
    // EDGE CASE: south was found
        set_cybot_coords(cybot_pose.xy.y, 420);
        send_edge('S');
        turn_to_angle(sensor, 270, &cybot_pose);
}

bumpy receive_and_execute(oi_t* sensor) {
    Routine routine;
    int startRoutine = 0;
    int count = 0;
    char num_buffer[6];
    int buffer_loc = 0;
    while (true) {
        char routine_str = uart_receive();
        if (routine_str == 'R' && !startRoutine) {
            startRoutine = 1;
        } else if (routine_str == 'R' && startRoutine) {
            break;
        }
        if (startRoutine) {
            if (routine_str == 'F') {
                routine.moves[count].type = MOVE_FORWARD;
            } else if (routine_str == 'S') {
                routine.moves[count].type = MOVE_FORWARD_SMART;
            } else if (routine_str == 'T') {
                routine.moves[count].type = TURN_TO_ANGLE;
            } else if (isdigit(routine_str)) {
                num_buffer[buffer_loc] = routine_str;
                buffer_loc++;
            }
            if (buffer_loc == 5) {
                routine.moves[count].quantity = atoi(num_buffer);
                count++;
            }
        }
    }
    routine.amount = count;
    return execute_routine(sensor, &routine, &cybot_pose);
}
// To be done when the edge is found
void send_edge(char edge_type) {
    char buffer[50];
    if (edge_type == 'W' || edge_type == 'E') {
        sprintf(buffer, " E %0.2f %c ", cybot_pose.xy.x, edge_type);
        uart_sendStr(buffer);
    } else {
        sprintf(buffer, " E %0.2f %c ", cybot_pose.xy.y, edge_type);
        uart_sendStr(buffer);
    }
}

// Sets the cybot coordinates
void set_cybot_coords(float x, float y) {
    cybot_pose.xy.x = x;
    cybot_pose.xy.y = y;
}
// When the Cybot reaches North it will be facing north and will set it's heading accordingly
void set_cybot_heading(float heading) {
    cybot_pose.heading = heading;
}

// Finds the next point where the Cybot needs to reach moving towards it's desired location
Coordinate get_interim_coordinate(Coordinate target_coord, Coordinate bot_pos) {
    Coordinate interim_coord;
    int x_dist = target_coord.x - bot_pos.x;
    int y_dist = target_coord.y - bot_pos.y;
    double dist = sqrt((x_dist * x_dist) + (y_dist * y_dist));
    if (dist < 60) {
        return target_coord;
    }
    int dist_divider = dist / 50;
    interim_coord.x = x_dist / dist_divider;
    interim_coord.y = y_dist / dist_divider;

    return interim_coord;
}
// Sends the interim coordinates to the App
void send_interim_coordinate(Coordinate interim_coord) {
    char buffer[50];
    sprintf(buffer, " d %0.2f %0.2f %0.2f %0.2f ", interim_coord.x, interim_coord.y, cybot_pose.heading, 0);
    uart_sendStr(buffer);
}
void send_bot_pos() {
    char buffer[50];
    sprintf(buffer, " b o %0.2f %0.2f %0.2f %0.2f ", cybot_pose.xy.x, cybot_pose.xy.y, cybot_pose.heading, 0);
    uart_sendStr(buffer);
}
void send_hole_point(oi_t* sensor) {
    uint16_t reading = getHoleTouching(sensor);
    float head = cybot_pose.heading;
    if (reading & 0x08) {
        head += 1.178;
    } else if (reading & 0x04) {
        head += 0.393;
    } else if (reading & 0x02) {
        head -= 0.393;
    } else if (reading & 0x01) {
        head -= 1.178;
    }
    char buffer[50];
    sprintf(buffer, " h %0.3f %0.3f %0.3f ", cybot_pose.xy.x, cybot_pose.xy.y, head);
    uart_sendStr(buffer);
}
void send_field(Field field) {
    send_pillars_through_putty(field.pillars, field.size);
}
bumpy execute_move(oi_t* sensor, Move move, Pose2D* robotPose) {
    switch (move.type) {
               case MOVE_FORWARD:
                   move_forward(sensor, move.quantity, robotPose);
                   break;
               case MOVE_BACKWARDS:
                   move_backwards(sensor, move.quantity, robotPose);
                   break;
               case MOVE_FORWARD_SMART:
               {
                   // the only issue with blindly returning here is that we lose the ability to continue through
                   // the routine if we did not hit a pillar, wait nvm we don't
                   return smart_move_forward(sensor, move.quantity, robotPose);
                   // somehow we need to add this to the field
                   // I think that we can blindly return the hit pillar from here, this would mean that we are at the result if the pillar is not effectively null
               }
               case TURN_TO_ANGLE:
                   // lcd_printf("Trying to turn %0.3f", move.quantity);
                   turn_to_angle(sensor, radians_to_degrees(move.quantity), robotPose);
                   break;
               case SCAN:
                   //TODO: Implement
                   break;
           }
    return (bumpy) {.complete = true};
    // return (Pillar) {.radius = -1};
}

bumpy execute_routine(oi_t* sensor, Routine* routine, Pose2D* botPose) {
    Move* moves = routine->moves;
    uint8_t head;

    // lcd_printf("quantity: %d", routine->amount);

    for (head = 0; head < routine->amount; head++) {
        // lcd_printf("type: %d", moves[head].type);
       bumpy b = execute_move(sensor, moves[head], botPose);
       if (!b.complete) {
           return b; // return pillar of hit
       }
    } // return robot pose somehow if we want persistent memory of the enviornment
    // wait we def do
    return (bumpy) {.complete = true};
}
