/*
* caleb_array.h
* file for moving window array
* Created on: Sep 17, 2024
* 	Author: ckugel05
*/

#ifndef CALEB_ARRAY_H
#define CALEB_ARRAY_H

#include <stdlib.h>
#include "math.h"
#include "stdint.h"

typedef struct {
  uint16_t angle;
  float distance;
} Vector; 

typedef Vector T;

typedef struct {
  T* arr;
  long size;
  long capacity;
} CArray;


CArray create(char capacity);

void append(CArray* self, T newData);

// double getAverage(CArray* self);

T get(CArray* self, long index);

T get_last(CArray* self);

void deconstruct(CArray* self);

short bst(T* arr, T key, uint16_t size);

char get_corrected_size(CArray* self);

#endif /* CALEB_ARRAY_H */
