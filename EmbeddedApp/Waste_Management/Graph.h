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

// assumes the bot is @ 0 0 and facing 0 degrees
typedef struct {
    uint8_t size;
    Pillar* pillars;
} Field;

// nodes are points on the graph that the robot can safely travel to. These should be generated via a discretization function
typedef struct {
  // some sort of random access memory for the nodes
  Pose2D* nodes;
  // we need an adjacency matrix
  float* weights; // going to be formatted very goofy. weights should be square of distances
  // the amount of nodes as well as the square root of the number of weights ^ like I said it was goofy 
  uint8_t size;
} Graph;

Field create_field(Pillar* pillar, uint8_t size);
void deconstruct_field(Field* field);
void add_bump_to_field(Field* field, Pillar p);

void transformFieldBy(Field* field, Pose2D pose);

Pose2D create_pose_from_coordinates(Coordinate coords, float heading);
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

float degrees_to_radians(float degrees);
float radians_to_degrees(float radians);

float angleBetweenTwoPoints(Coordinate one, Coordinate two);
float distanceBetweenTwoPoints(Coordinate one, Coordinate two);

/**
 * @param base the coordinates
 * @param translation the x and y values to alter the base by
 * @return a copy of the base struct that has been translated.
 * //TODO: decide if this should instead mutate base. Ownership style
 */
void translatePoseByCoordinate(Pose2D* base, Coordinate translation);
Pose2D addTranslation(Pose2D* base, double modifier);

void translatePoseByMagnitude(Pose2D* base, float magnitude);

Pose2D copy_of_pose(Pose2D pose);

float getAngleFromPillar(Pillar p);
float getDistanceToPillar(Pillar p);

/**
 * Does a translation based off of passed in pose coordinates
 * Does a rotation based off off passed in pose's rotation
 */
void transformPose(Pose2D* base, Pose2D modifier);

Pillar getPillarFromObject(Obj object);

void send_desired_through_putty(uint8_t indexSmallest);

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

float inverse_length_raw(float x1, float y1, float x2, float y2);

bool line_intersects_circle(float x1, float y1, float x2, float y2, float cx, float cy, float radius);

/**
 * Calculates whether a line between start and end intersects with a pillar
 * Inspired by: https://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm
 */
bool intersects_pillar(Coordinate start, Coordinate end, Pillar* pillar);


/** uses the quake fast inverse square root algorithm to calculate the square root of value
 * https://betterexplained.com/articles/understanding-quakes-fast-inverse-square-root/
 *
 * Research Paper: https://www.lomont.org/papers/2003/InvSqrt.pdf
 * Yes I read this for this lab ^
 */
float fast_inverse_square_root(float value);

float inverse_length(Coordinate one, Coordinate two);

void cleanse_small_width_objects(Obj* objects, uint8_t* size, uint8_t STEP);

Pose2D addTranslation(Pose2D* base, double modifier);

#endif //LAB5_288_GRAPH_H
