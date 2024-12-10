//
// Created by calebk on 10/15/24.
//

#include "Graph.h"

/**
 * Creates a Pose2D structure from a coordinate and heading angle.
 * 
 *  coords => Coordinate (x, y).
 *  heading => Heading angle in radians.
 *  returns the constructed Pose2D structure.
 */
Pose2D create_pose_from_coordinates(Coordinate coords, float heading) {
    return (Pose2D) {.xy = coords, .heading = heading};
}

/**
 * Creates a Pose2D structure from x, y values and a heading angle.
 * 
 *  x is X-coordinate.
 *  y is Y-coordinate.
 */

Pose2D create_pose_from_xy(float x, float y, float heading) {
    return (Pose2D) {.xy = {.x = x, .y = y}, .heading = heading};
}

/**
 * Creates a copy of a Pose2D structure.
 * 
 * pose is The original Pose2D.
 * returns The copied Pose2D.
 */
Pose2D copy_of_pose(Pose2D pose) {
    return (Pose2D) {.xy = pose.xy, .heading = pose.heading};
}

/**
 *  Creates a Pose2D using polar coordinates.
 * 
 *  "magnitude" is Distance from the origin.
 *  "angle" is Angle in radians.
 *  returns The constructed Pose2D.
 */
Pose2D create_pose_from_polar(float magnitude, float angle) {
  return (Pose2D) {.xy.x = magnitude * cos(angle), .xy.y = magnitude * sin(angle), .heading = 0};
}

/**
 * Creates a Field structure from an array of Pillars.
 * 
 * "pillars" is Array of Pillars.
 * "size" is Number of Pillars.
 * returns The constructed Field.
 */

Field create_field(Pillar* pillars, uint8_t size) {
    Field f;
    f.pillars = pillars; /* = calloc(sizeof(Pillar), size + 1); */ // maybe maybe maybe
    // memcpy(f.pillars, pillars, sizeof(Pillar) * (size));
    f.size = size;
    return f;
}

/**
 *  Adds a Pillar to a Field. * 
 * "field" is Pointer to the Field structure.
 * @param p The Pillar to add.
 */

void add_bump_to_field(Field* field, Pillar p) {
    field->pillars[field->size++] = p; // store it into the field object with guranteed space (maybe)
}

/**
 * Calculates the angle between two coordinates.
 * 
 * @param one First Coordinate.
 * @param two Second Coordinate.
 * @return float Angle in radians.
 */

float angleBetweenTwoPoints(Coordinate one, Coordinate two) {
    return atan2((two.y - one.y), (two.x - one.x));
}

/**
 * Calculates the distance between two coordinates.
 * @return float Distance between the points.
 */

float distanceBetweenTwoPoints(Coordinate one, Coordinate two) {
    return hypot(one.x - two.x, one.y - two.y);
}

/**
 * Free memory allocated for a Field's Pillars.
 * 
 * @param field Pointer to the Field structure.
 */

void deconstruct_field(Field* field) {
    free(field->pillars);
}

// Convert degrees to radians.
float degrees_to_radians(float degrees) {
    return degrees * M_PI / 180.0;
}

// Convert radians to degrees
float radians_to_degrees(float radians) {
    return radians * 180.0 / M_PI;
}


// Rotate a Pose2D by a given angle.
void rotatePoseByAngle(Pose2D* pose, float angle) {
    double newX = (pose->xy.x * cos(angle)) - (pose->xy.y * sin(angle));
    pose->xy.y = (pose->xy.x * sin(angle)) + (pose->xy.y * cos(angle));
    pose->xy.x = newX;
    pose->heading += angle;
}

// Rotate a Pose2D by another Pose2D's heading angle.
// "rotation" is The Pose2D providing the rotation angle.
void rotatePoseByPose(Pose2D* pose, Pose2D rotation) {
    rotatePoseByAngle(pose, rotation.heading);
}


/**
 * Translates a Pose2D by a given coordinate.
 * 
 * @param base Pointer to the Pose2D to translate.
 * @param translation The Coordinate by which to translate.
 */
void translatePoseByCoordinate(Pose2D* base, Coordinate translation) {
    base->xy.x += translation.x;
    base->xy.y += translation.y;
}


/**
 * Translates a Pose2D by another Pose2D.
 * 
 * @param base Pointer to the Pose2D to translate.
 * @param translation The Pose2D providing the translation.
 */
void translatePoseByPose(Pose2D* base, Pose2D translation) {
    translatePoseByCoordinate(base, translation.xy);
}

/**
 * Applies a transformation (rotation and translation) to a Pose2D.
 * 
 * @param base Pointer to the base Pose2D.
 * @param modifier The Pose2D providing the transformation.
 */
void transformPose(Pose2D* base, Pose2D modifier) {
    rotatePoseByPose(base, modifier);
    translatePoseByPose(base, modifier);
}


void transformFieldBy(Field* field, Pose2D pose) {
    uint8_t i;
    for (i = 0; i < field->size; i++) {
        transformPose(&(field->pillars[i].position), pose);
    }
}

/**
 * Translates a Pose2D along its heading by a given magnitude.
 * 
 * @param base Pointer to the Pose2D.
 * @param magnitude Distance to translate.
 */

void translatePoseByMagnitude(Pose2D* base, float magnitude) {
     base->xy.x += cos(base->heading) * magnitude;
     base->xy.y += sin(base->heading) * magnitude;
}


    /**
     * @brief Converts an Obj (detected object) to a Pillar.
     *
     * The pillar is created based on the average angle and distance of the object.
     * The linear width of the pillar is calculated using trigonometric functions.
     *
     * @param object Detected object containing angular and distance information.
     * @return Pillar The constructed Pillar.
     */
Pillar getPillarFromObject(Obj object) {
    float averageAngle = degrees_to_radians(fabs(object.v2.angle + object.v1.angle)) / 2.0 - degrees_to_radians(90);
    float angularWidth = degrees_to_radians(fabs(object.v2.angle - object.v1.angle));
    float linearWidth =  tan(angularWidth / 2.0f) * object.middleDistance * 2.0f;

    // uart_sendStr("\n\n");
    // char buff[50];
    // sprintf(buff, "%.2f,%.2f", averageAngle, object.middleDistance);
    // uart_sendStr(buff);
    // uart_sendStr("\n\n");

    return (Pillar) {
            .position.xy = {.x = object.middleDistance * cos(averageAngle), .y = object.middleDistance * sin(averageAngle) + 7}, // the ten is for the ten centimeters from the center of the bot to the center of the sonar
            .position.heading = 0,
            .radius = linearWidth / 2
    };
}

    /**
     * Finds the smallest object (based on linear width) from a list of objects.
     *
     * @param objects Array of objects to evaluate.
     * @param size Number of objects in the array.
     * @return Obj The smallest object.
     */
Obj getSmallestObject(Obj* objects, uint8_t size) {
    uint8_t i;
    uint8_t indexOfSmallestLinearWidth;
    indexOfSmallestLinearWidth = 0;
    float smallestLinearWidth = 4096;

    for (i = 0; i < size; i++) {
        // update smallestLinearWidth
        float linearWidth = getPillarFromObject(objects[i]).radius;
        if (smallestLinearWidth > linearWidth) {
            smallestLinearWidth = linearWidth;
            indexOfSmallestLinearWidth = i;
        }
    }

    return objects[indexOfSmallestLinearWidth];
}

Pillar* getPillarsFromObject(Obj* objects, uint8_t size) {
     /**
     * Converts an array of objects into an array of Pillars.
     *
     * Each object is converted to a Pillar using `getPillarFromObject`.
     * @return Pillar* Pointer to the array of Pillars.
     */
    uint8_t i;
    Pillar* pillars = calloc(sizeof(Pillar), size);
    for (i = 0; i < size; i++) {
        pillars[i] = getPillarFromObject(objects[i]);
    }
    return pillars;
}

void send_bot_pose_through_putty(Pillar position) {
    /**
     * Sends the bot's position through UART (via PuTTY).
     *
     * Formats the position and radius as a string and transmits it.
     *
     * @param position Current position of the bot.
     */    
     char buffer[50];
    sprintf(buffer, " b %0.2f %0.2f %0.2f %0.2f ", position.position.xy.x, position.position.xy.y, position.position.heading, position.radius);
    uart_sendStr(buffer);
}

void send_desired_through_putty(uint8_t indexSmallest) {
    /**
     * Sends the index of the smallest object as the desired target via UART.
     *
     * @param indexSmallest Index of the smallest object.
     */
     char buffer[50];
    sprintf(buffer, " d %d ", indexSmallest);
    uart_sendStr(buffer);
}

void send_pillar_through_putty(Pillar pillar){
    /**
     * Sends information about a single Pillar through UART.
     *
     * "pillar" is The Pillar to send.
     */    
    char buffer[50];
    sprintf(buffer, " o %0.2f %0.2f %0.2f %0.2f ", pillar.position.xy.x, pillar.position.xy.y, pillar.position.heading, pillar.radius);
    uart_sendStr(buffer);

}

void send_pillars_through_putty(Pillar* pillars, uint8_t size) {
     /**
     * Sends an array of Pillars through UART.
     *
     * Starts with an identifier ('F') and ends with 'F '.
     *
     * @param pillars Array of Pillars to send.
     */
    uint8_t j;
    // uart_sendStr(" F");
    for(j = 0; j < size; j++) {
        send_pillar_through_putty(pillars[j]);
    }
    uart_sendStr("F ");

}

Pillar get_smallest_pillar(Pillar* pillars, uint8_t size) {
     /**
     *  Finds the smallest Pillar (based on radius) from an array of Pillars.
     *
     * @param pillars Array of Pillars to evaluate.
     * @param size Number of Pillars in the array.
     * @return Pillar The smallest Pillar.
     */
    uint8_t indexSmallest;
    indexSmallest = 0xFF;
    uint8_t i;
    float smallestFoundRadius = 4096;
    for (i = 0; i < size; i++) {
        if (pillars[i].radius < smallestFoundRadius) {
            indexSmallest = i;
            smallestFoundRadius = pillars[i].radius;
        }
    }
    return pillars[indexSmallest];
}

uint8_t get_index_smallest_pillar(Field* field) {
     /**
     * Finds the index of the smallest Pillar in a Field.
     *
     * @param field Pointer to the Field structure.
     * @return uint8_t Index of the smallest Pillar.
     */
    uint8_t indexSmallest;
    indexSmallest = 0xFF;
    uint8_t i;
    float smallestFoundRadius = 4096;
    for (i = 0; i < field->size; i++) {
        if (field->pillars[i].radius < smallestFoundRadius) {
            indexSmallest = i;
            smallestFoundRadius = field->pillars[i].radius;
        }
    }
    return indexSmallest;
}

void send_objects_through_putty(Obj* objects, uint8_t size) {
    uint8_t i;
    for (i = 0; i < size; i++) {
        send_pillar_through_putty(getPillarFromObject(objects[i]));
        timer_waitMillis(15);
    }

}

void send_object_raw(Obj object) {
     /**
     * Sends raw information about objects via UART.
     *
     * Loops through each object and transmits its data.
     *
     * @param object Array of objects to send.
     */
    char buffer[60];
    sprintf(buffer, "angle 1: %d angle 2: %d distance: %.2f\r\n", object.v1.angle, object.v2.angle, object.middleDistance);
    uart_sendStr(buffer);
}

// Swap objects
void swapObjects(Obj* objects, uint16_t one, uint16_t two) {
    Obj temp = objects[one];
    objects[one] = objects[two];
    objects[two] = temp;
}

void send_objects_raw(Obj* object, uint8_t size) {

    uint8_t i;
    for (i = 0; i < size; i++) {
        send_object_raw(object[i]);
    }
}

float getDistanceToPose(Pose2D p) {
    return sqrt((pow(p.xy.x, 2) + pow(p.xy.y, 2)));
}

float getAngleFromPose(Pose2D p) {
    return atan2(p.xy.y, p.xy.x);
}


// good luck
float fast_inverse_square_root(float value) {
    float halfX = 0.5f * value;
    int i = *(int*)&value;
    i = 0x5f3759df - (i >> 1); 
    value = *(float*)&i;
    return value*(1.5f - halfX*value*value);
}

float inverse_length_raw(float x1, float y1, float x2, float y2) {
   return fast_inverse_square_root(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

float inverse_length(Coordinate one, Coordinate two) {
     /**
     * Calculates the inverse length between two coordinates.
     *
     * Uses a fast inverse square root for efficiency.
     * @return float The inverse length.
     */
   return fast_inverse_square_root(pow(one.x - two.x, 2) + pow(one.y - two.y, 2));
}

bool line_intersects_circle(float x1, float y1, float x2, float y2, float cx, float cy, float radius) {
    float inverseLength = inverse_length_raw(x1, y1, x2, y2);
    float dy = (y2 - y1) * inverseLength;
    float dx = (x2 - x1) * inverseLength;

    float t = dx * (cx - x1) + dy * (cy - y1);

    float Ex = t * dx + x1;
    float Ey = t * dy + y1;

    // technically we should calculate the euclidean distance between E and the center of the circle

    // however all we would do with that is compare if it is less than the radius of the circle
    // Because sqrt is a monotonic function (order is perserved) we can compare L with the square of R
    // the square of R is much faster to calculate here
    float L = pow(Ex - cx, 2) + pow(Ey - cy, 2);

    return L < pow(radius, 2);

}

bool intersects_pillar(Coordinate start, Coordinate end, Pillar* pillar) {
     /**
     * Checks if a line segment intersects a pillar.
     *
     * Uses geometric calculations to determine intersection.
     *
     * @param start Starting coordinate of the line.
     * @param end Ending coordinate of the line.
     * @return bool True if the line intersects the pillar, false otherwise.
     */
    return line_intersects_circle(start.x, start.y, end.x, end.y, pillar->position.xy.x, pillar->position.xy.y, pillar->radius + BOT_RADIUS);
}


float getDistanceToPoseFrom(Pose2D to, Pose2D from) {
  return hypot(to.xy.x - from.xy.x, to.xy.y - from.xy.y);
}

float getAngleFromPillar(Pillar p) {
    return getAngleFromPose(p.position);
}

float getDistanceToPillar(Pillar p) {
    return getDistanceToPose(p.position);
}

void cleanse_small_width_objects(Obj* objects, uint8_t* size, const uint8_t STEP) {
    uint8_t i;
    for (i = 0; i < (*size); i++) {
        if (fabs(objects[i].v1.angle - objects[i].v2.angle) < STEP * 6) {
            // remove the object as it is too thing
            swapObjects(objects, i, (*size) - 1);
            (*size)--;
            i--;
        }
    }
}



Pose2D addTranslation(Pose2D* base, double modifier) {
    Pose2D toReturn = copy_of_pose(*base);
    double magnitude = hypot(toReturn.xy.x, toReturn.xy.y);
    double newValues = magnitude - modifier;
    toReturn.xy = (Coordinate) {
        .x = newValues * cos(atan2(base->xy.y, base->xy.y)),
        .y = sin(atan2(base->xy.y, base->xy.x))
    };
    return toReturn;
}

  
