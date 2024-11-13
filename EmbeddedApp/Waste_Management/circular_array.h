/*
* caleb_array.h
* file for moving window array
* This is a circular array, the name caleb_array is bad
* Created on: Sep 17, 2024
* 	Author: ckugel05
*/

#ifndef CALEB_ARRAY_H
#define CALEB_ARRAY_H

#include <stdlib.h>
#include "math.h"
#include "stdint.h"

// The definition for vectors used throughout the program
typedef struct {
  uint16_t angle;
  float distance;
} Vector; 

// just here because during development we were changing around what we wanted this array to store
typedef Vector T;

// The definition of a circular array
typedef struct {
  T* arr;
  long size;
  long capacity;
} CArray;

/**
 * Creates a new circular array with a provided capacity.
 * Needs to be deconstructed with {@link #deconstruct}
 * @param capacity the size to make the array
 * @return 
 */ 
CArray create_circular_array(uint8_t capacity);

/**
 * Append a value to the circular array.
 * This will overwrite the oldest value in the array
 */ 
void append(CArray* self, T newData);

/**
 * Gets an element from the circular array 
 */ 
T get(CArray* self, long index);

/**
 * gets the last element of a circular array
 */ 
T get_last(CArray* self);

/**
 * Deconstructs a given circular array.
 * This frees the underlying array DS
 */ 
void deconstruct_circular_array(CArray* self);

/**
 * Performs a Binary Search across an array of type T (in this case vectors)
 */
short bst(T* arr, T key, uint16_t size);

/**
 * returns the wrapped size
 */ 
char get_corrected_size(CArray* self);

#endif /* CALEB_ARRAY_H */
