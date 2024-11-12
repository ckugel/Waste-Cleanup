/*
 * navigate.c
 *
 *  Created on: Oct 18, 2024
 *      Author: ckugel05
 */

#include <stdlib.h>
#include "navigate.h"

#define AROUND_ROUTINE_LENGTH 5

Routine plan_route_after_collision(Pose2D* botLocation, bool hitLeft) {
    // basically we can generate one of two trapezoids to go around the object
    // assume heading may be maintained
    // rescan at end


    Move* moves = calloc(sizeof(Move), AROUND_ROUTINE_LENGTH);
    // uint8_t i;

    float turn;
    if (hitLeft) {
        // make route to the right
        turn = -degrees_to_radians(60);
    }
    else {
        // make route to the left
        turn = degrees_to_radians(60);
    }

    Pose2D predicted = copy_of_pose(*botLocation);

    predicted.heading += turn;
    translatePoseByMagnitude(&predicted, ASIDE_LENGTH);

    moves[0] = (Move) {.quantity = turn, .type = TURN_TO_ANGLE};
    moves[1] = (Move) {.quantity = ASIDE_LENGTH, .type = MOVE_FORWARD};
    moves[2] = (Move) {.quantity = -turn, .type = TURN_TO_ANGLE};
    moves[3] = (Move) {.quantity = FORWARD_LENGTH, .type = MOVE_FORWARD};
    moves[4] = (Move) {.quantity = -turn, .type = TURN_TO_ANGLE};

    return (Routine) {.moves = moves, .amount = AROUND_ROUTINE_LENGTH};

/*
    Vector pathToObject = {.angle = angleBetweenTwoPoints(field->pillars[desired].position.xy, botLocation->xy),
                           .distance =  distanceBetweenTwoPoints(field->pillars[desired].position.xy, botLocation->xy)};


    // assume pillar is right in front of us
    // check left and right


    uint8_t i = 0;
    // trapezoid has turn drive turn drive turn
    Move* moves1 = calloc(sizeof(Move), 5);
    Move* moves2 = calloc(sizeof(Move), 5);

    Routine option1 = {};
    Coordinate Start1, Start2, End1, End2;

    Pose2D botOne = copy_of_pose(*botLocation);
    Pose2D botTwo = copy_of_pose(*botLocation);

    botOne.heading += degrees_to_radians(90);
    botTwo.heading -= degrees_to_radians(90);

    Start1 = botOne.xy;
    Start2 = botTwo.xy;

    translatePoseByMagnitude(&botOne, ASIDE_LENGTH + BOT_RADIUS);
    translatePoseByMagnitude(&botTwo, ASIDE_LENGTH + BOT_RADIUS);

    End1 = botOne.xy;
    End2 = botTwo.xy;

    // see if intersects pillar

    Start1 = End1;
    Start2 = End2;

    // rotate
    botOne.heading -= degrees_to_radians(90);
    botTwo.heading += degrees_to_radians(90);

    translatePoseByMagnitude(&botOne, FORWARD_LENGTH + BOT_RADIUS);
    translatePoseByMagnitude(&botTwo, FORWARD_LENGTH + BOT_RADIUS);

    End1 = botOne.xy;
    End2 = botTwo.xy;

    // make sure we don't hit on this

    free(moves1);
    free(moves2);
     */
}

/*
Routine plan_route_after_collision(Field* field, const uint8_t desired, Pose2D* botLocation) {
    // the array of pillars could be interpreted as a graph
    uint8_t i;
    // Pillar cyBotLocation = {.position = create_pose_from_xy(0, 0, 0), .radius = 7};

    Pose2D predictedPosition = copy_of_pose(*botLocation);

    // greedy try straight line
    // if the path ends up intersecting any pillars then we will move over and try again

    // left of the bot is negative y
    // right of the bot is negative y
    float offset;
    float turn;
    bool appliedModifier = false;
    if (field->pillars[desired].position.xy.y < predictedPosition.xy.y) {
        offset = -2.5;
        turn = -60;
    }
    else {
        offset = 2.5;
        turn = 60;
    }
    Coordinate modifier = {.x = 0, .y = offset};
    bool madeThrough;
    madeThrough = false;

    uint8_t attemtps;
    attemtps = 0;

    while (!madeThrough) {
        if (attemtps == 100) {
            modifier.y = -offset;
            turn = -turn;
        }
        madeThrough = true;
        for (i = 0; i < field->size; i++) {
            if (i != desired) {
                if (intersects_pillar(predictedPosition.xy, field->pillars[desired].position.xy, &field->pillars[i])) {
                    madeThrough = false;
                }
            }
        }
        if (!madeThrough) {
            appliedModifier = true;
            translatePoseByCoordinate(&predictedPosition, modifier);
        }
        attemtps++;
    }

    // guarantee that a straight line between cyBotLocation and the pillars[desire] has no collisions
    Move* moves;
    uint8_t head = 0;
    uint8_t moves_size; // turn and go default
    if (appliedModifier) {
        moves_size = 4; // turn, move, turn, move
        moves = calloc(sizeof(Move), moves_size);
        head = 2;
        moves[0] = (Move) {.quantity = turn, .type = TURN_TO_ANGLE};
        moves[1] = (Move) {.quantity = fabs(predictedPosition.xy.y - botLocation->xy.y), .type = MOVE_FORWARD};
       // Pose2D modifierLower = (Pose2D) {.xy.x = 0, .xy.y = predictedPosition.xy.y, .heading = turn};
       predictedPosition.xy.y += modifier.y;
       predictedPosition.heading += turn;
    } else {
        moves_size = 2;
        moves = calloc(sizeof(Move), moves_size);
    }
    float dy;
    float dx;
    // bot location should hold the location of the bot right before we are ready to point and shoot
    dy = predictedPosition.xy.y - field->pillars[desired].position.xy.y;
    dx = predictedPosition.xy.x - field->pillars[desired].position.xy.x;
    turn = -(predictedPosition.heading) + atan2(dy, dx)  /*angleBetweenTwoPoints(cyBotLocation.position.xy, pillars[desired].position.xy)*/;
    /*
    moves[head++] = (Move) {.quantity = turn, .type = TURN_TO_ANGLE};
    float magnitude = sqrt(pow(dy, 2) + pow(dx, 2));
    moves[head] = (Move) {.quantity = magnitude, .type = MOVE_FORWARD};

    return (Routine) {.moves = moves, .amount = moves_size};
}
*/

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

