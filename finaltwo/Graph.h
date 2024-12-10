//
// Created by calebk on 10/15/24.
//

#ifndef LAB5_288_GRAPH_H
#define LAB5_288_GRAPH_H


#include <stdio.h>
#include "filter.h"
#include "open_interface.h"
#include "uart.h"


// the radius of the bot in centimeters
#define BOT_RADIUS 7
// the items only the bump sensors can detect
#define SMALL_ITEM_RADIUS 7.0

// the difference in angle to create a new node in field discretization
#define ANGLE_STEP 0.174533 // about 10 degrees
#define EPSILON 8 // error

typedef struct {
    Vector v1;
    Vector v2;
    float middleDistance;
} Obj;

typedef struct {
    float x; // meters
    float y; // meters
} Coordinate;

typedef struct {
    Coordinate xy;
    float heading; // radians. 0 is the front of the bot
} Pose2D;

typedef struct {
    Pose2D position;
    float radius; // meters
} Pillar;

// assumes the bot is @ 0 0 and facing 0 degrees.
typedef struct {
    uint8_t size;
    Pillar* pillars;
} Field;

/**
 * Creates a new field object from a list of pillars and it's size
 * Does dynamic memory allocation and also allocates enough space for 5 more pillars
 * @param pillar a list of Pillar objects
 * @param size the size of the Pillar objects array
 * @return a struct of type Field that contains the pillars on the field
 */ 
Field create_field(Pillar* pillar, uint8_t size);

/**
 * Used to deconstruct a created field
 */
void deconstruct_field(Field* field);

/**
 * Adds a bumped pillar to the field
 */ 
void add_bump_to_field(Field* field, Pillar p);

/**
 * Matrix transforms an entire field by a passed in (x, y, theta) in the form of a pose2d
 * Modifies the field
 */ 
void transformFieldBy(Field* field, Pose2D pose);

/**
 * Constructor for a Pose2D based off of passed in coords struct and heading
 */ 
Pose2D create_pose_from_coordinates(Coordinate coords, float heading);

/**
 * Constructor for a Pose2D based off of x, y, heading
 */ 
Pose2D create_pose_from_xy(float x, float y, float heading);

/**
 * rotate a pose2d about another axis
 * Math is derived from matrix rotations {@linkplain https://en.wikipedia.org/wiki/Rotation_matrix}
 * @param pose the base position that's values will be rotated.
 */
void rotatePoseByPose(Pose2D* pose, Pose2D rotation);

/**
 * Same as rotate PoseByPose but takes in a raw angle
 */
void rotatePoseByAngle(Pose2D* pose, float angle);

/**
 * Math conversion
 * @param angle the angle in degrees
 * @return the angle in radians
 */ 
float degrees_to_radians(float degrees);

/**
 * Math angle conversion
 * @param radians the angle in radians
 * @return the angle in degrees
 */ 
float radians_to_degrees(float radians);

/**
 * @return the angle between two cartesian points
 */ 
float angleBetweenTwoPoints(Coordinate one, Coordinate two);

/**
 * @return the distance between two cartesian points
 */ 
float distanceBetweenTwoPoints(Coordinate one, Coordinate two);

/**
 * Translates a given position by each component of the translation.
 * For instance does base.x + translation.x
 * @param base the coordinates
 * @param translation the x and y values to alter the base by
 */
void translatePoseByCoordinate(Pose2D* base, Coordinate translation);

/**
 * Translates a position by a given magnitude in the direction of the current heading on the base position
 * @param base the position which gets translated and modified
 * @param magnitude the amount to translate the position by
 */ 
void translatePoseByMagnitude(Pose2D* base, float magnitude);

/**
 * Creates a copy of a Pose2D and returns it
 * @param pose the position to clone
 * @return the cloned pose passed in
 */ 
Pose2D copy_of_pose(Pose2D pose);

/**
 * @return the angle from 0, 0 to a given pillar
 */ 
float getAngleFromPillar(Pillar p);

/**
 * @return the distance to a given pillar from 0, 0
 */ 
float getDistanceToPillar(Pillar p);

/**
 * Does a translation based off of passed in pose coordinates
 * Does a rotation based off off passed in pose's rotation
 */
void transformPose(Pose2D* base, Pose2D modifier);

/**
 * Takes in a given object and returns it as a pillar
 */ 
Pillar getPillarFromObject(Obj object);

/**
 * sends the desired node index in fields through putty.
 * NOTE: this will likely be changing to a position on the field 
 */ 
void send_desired_through_putty(uint8_t indexSmallest);

/**
 * returns the smallest object from a list of objects
 * @param objects the list of objects to search through
 * @param size the size of the object
 * @return the object which is the smallest
 */ 
Obj getSmallestObject(Obj* objects, uint8_t size);

Pillar* getPillarsFromObject(Obj* objects, uint8_t size);

void send_pillar_through_putty(Pillar pillar);

void send_pillars_through_putty(Pillar* pillars, uint8_t size);

Pillar get_smallest_pillar(Pillar* pillars, uint8_t size);

uint8_t get_index_smallest_pillar(Field* field);

void send_objects_through_putty(Obj* objects, uint8_t size);

void send_object_raw(Obj object);

void swapObjects(Obj* objects, uint16_t one, uint16_t two);

void send_objects_raw(Obj* object, uint8_t size);

float getAngleFromPose(Pose2D p);
float getDistanceToPose(Pose2D p);

/**
* Takes a list of objects, removes objects that are too small.
* Modifies the respective size of the list and returns.
* @param STEP cooresponds to the step at which the measurements were taken
*/ 
void cleanse_small_width_objects(Obj* objects, uint8_t* size, uint8_t STEP);

Pose2D addTranslation(Pose2D* base, double modifier);

#endif //LAB5_288_GRAPH_H
