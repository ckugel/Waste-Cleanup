/*
* caleb_array.c
*
* Created on: Sep 16, 2024
* 	Author ckugel05
*/

#include "caleb_array.h"

CArray create(char capacity) {
  T* array = (T*) malloc(sizeof(T) * capacity);
  CArray b;
  b.arr = array;
  b.size = 0;
  b.capacity = capacity;
  return b;
}

void append(CArray* self, T newData) {
  // add to moving window
  self->arr[(self->size++) % self->capacity] = newData;
}

T get(CArray* self, long index) {
  return self->arr[(index) % self->capacity];
}

/*
double getAverage(CArray* self) {
  float sum = 0;
  int i;
  for (i = 0; i < self->capacity; i++) {
    sum += self->arr[i].distance;
  }
  return (double) (sum) / (double) (self->capacity);
}
*/

void deconstruct(CArray* self) {
  free(self->arr);
}

char get_corrected_size(CArray* self) {
    return (self->size) % self->capacity;
}


T get_last(CArray* self) {
  // 0 -> 0
  // 1 -> 1
  // 5 -> 0
  // 6 -> 1
  return self->arr[self->size % self->capacity];
}


short bst(T* arr, T key, uint16_t size) {
  short low = 0;
  short high = size - 1;

  while (high >= low) {
    short mid = low + (high - low) / 2;
    T midt = arr[mid];
    
    if (midt.angle == key.angle && midt.distance == key.distance) {
      return mid;
    }
    // same distances, different angles
    else if (midt.distance == key.distance) {
        // have to do linear search
        while (mid > 0 && arr[mid - 1].distance == key.distance) {
            mid--;
            if (arr[mid].angle == key.angle) {
                return mid;
            }
        }
        while (mid < size - 1 && arr[mid + 1].distance == key.distance) {
                    mid++;
                    if (arr[mid].angle == key.angle) {
                        return mid;
                    }
                }
    }
    else if (midt.distance < key.distance) {
      low = mid + 1;
    }
    else {
      high = mid - 1;
    }
  }

  // if you hit this, thats a BIG issue
  return -1;
}
