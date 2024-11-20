
#include "navigate.h"

// To be done first
// Will move the Cybot to the east side of the field
void find_east(oi_t* sensor) {
    if (cybot_heading != 0) {
        return;
    }
    int found = 0;
    char info;
    bumpy bump;
    while (!found) {
        Coordinate interim;
        interim.x = cybot_coords.x + 50;
        interim.y = cybot_coords.y;
        send_interim_coordinate(interim);
        bump = receive_and_execute();
        
        //TODO:
        // If edge is found check IMU direction to see which edge it is
    }
}
bumpy receive_and_execute() {
    Routine routine;
    int startRoutine = 0;
    int count = 0;
    char num_buffer[6];
    int buffer_loc = 0;
    while (true) {
        routine_str = uart_receive();
        if (routine_str == 'R' && !startRoutine) {
            startRoutine = 1;
        }
        if (routine_str == 'R' && startRoutine) {
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
    return execute_routine(sensor, routine, cybot_pose);
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
// To be executed after finding the east side
// Needs the Cybot to be turned 
// Will move the Cybot to the north end of the field then send back that the position has been set
void find_north() {
    if (cybot_pose.heading != (3.14 / 2)) {
       return;
    }
    int found = 0;
    while (!found) {
       Coordinate interim;
       interim.x = cybot_pose.xy.x;
       interim.y = cybot_pose.xy.y + 50;
       send_interim_coordinate(interim);
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
    sprintf(buffer, " b o %0.2f %0.2f %0.2f %0.2f ", cybot_coords.x, cybot_coords.y, cybot_pose.heading, 0);
    uart_sendStr(buffer);
}
void send_hole_point() {

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