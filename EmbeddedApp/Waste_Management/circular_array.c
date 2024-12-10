/*
* caleb_array.c
*
* Created on: Sep 16, 2024
* 	Author ckugel05
*/

#include "circular_array.h"


/**
 * Creates a circular array with the specified capacity.
 * 
 * A circular array is a data structure that wraps around when it reaches its capacity,
 * effectively creating a "moving window" for the data.
 * 
 * @param capacity The maximum number of elements the array can hold.
 * @return CArray The created circular array structure.
 */
CArray create_circular_array(uint8_t capacity) {
  T* array = (T*) malloc(sizeof(T) * capacity); // Allocate memory for the array.
  CArray b;
  b.arr = array;  // Assign the allocated array to the structure.
  b.size = 0;    // Initialize the current size to 0.
  b.capacity = capacity; // Set the capacity of the circular array.
  return b;
}

/**
 * Appends a new element to the circular array.
 * 
 * The element is added to the next available slot, wrapping around if the capacity is exceeded.
 * 
 * @param self Pointer to the circular array.
 * @param newData The new element to be added.
 */
void append(CArray* self, T newData) {
      // Add the new data to the array at the current position,
    // wrapping around using the modulus operator.
  self->arr[(self->size++) % self->capacity] = newData;
}

// Retrieves an element at the specified index in the circular array.
T get(CArray* self, long index) {
  return self->arr[(index) % self->capacity];
}

// Frees the memory allocated for the circular array.
void deconstruct_circular_array(CArray* self) {
  free(self->arr);
}

// Retrieves the corrected size of the circular array.
char get_corrected_size(CArray* self) {
    return (self->size) % self->capacity;
}


// Retrieves the last element in the circular array.
T get_last(CArray* self) {
  // 0 -> 0
  // 1 -> 1
  // 5 -> 0
  // 6 -> 1
  return self->arr[self->size % self->capacity];
}



/**
 * Performs a binary search on a sorted array to find a specific key.
 * 
 * If the `distance` of the key matches but the `angle` does not, a linear search
 * is performed to resolve duplicates. Assumes the array is sorted by `distance`.
 * 
 * @param arr The sorted array to search.
 * @param key The key to search for (contains `distance` and `angle`).
 * @param size The size of the array.
 * @return short The index of the key if found, or -1 if not found.
 */
short bst(T* arr, T key, uint16_t size) {
  short low = 0;
  short high = size - 1;

  while (high >= low) {
    // Calculate the middle index.
    short mid = low + (high - low) / 2;
    T midt = arr[mid];
    
    // Case 1: Exact match found.
    if (midt.angle == key.angle && midt.distance == key.distance) {
      return mid;
    }
    // Case 2: Same distance, different angle - perform linear search.
    else if (midt.distance == key.distance) {
        // Search backward to check for matching angles.
        while (mid > 0 && arr[mid - 1].distance == key.distance) {
            mid--;
            if (arr[mid].angle == key.angle) {
                return mid;
            }
        }
         // Search forward to check for matching angles.
        while (mid < size - 1 && arr[mid + 1].distance == key.distance) {
                    mid++;
                    if (arr[mid].angle == key.angle) {
                        return mid;
                    }
                }
    }

      // Case 3: Target is in the upper half.
    else if (midt.distance < key.distance) {
      low = mid + 1;
    }
      // Case 4: Target is in the lower half.
    else {
      high = mid - 1;
    }
  }
  
  // If no match is found, return -1 to indicate an error.
  // if you hit this, thats a BIG issue
  return -1;
}
