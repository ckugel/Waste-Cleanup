//
// Created by calebk on 10/2/24.
//



#include <stdlib.h>
#include "LinearInterp.h"

/**
 * Creates a Linear Interpolation (Lerp) structure.
 *
 * The Lerp structure contains measured values and their corresponding real values,
 * allowing interpolation between them.
 *
 * @param measuredValues Array of measured values (x-axis).
 * @param realValues Array of real values (y-axis).
 * @param size Number of elements in the arrays.
 * @return Lerp The created Lerp structure.
 */

Lerp create_lerp(float* measuredValues, float* realValues, unsigned char size) {
    /*float* measured = malloc(size * sizeof(float));
    float* real = malloc(size * sizeof(float));

    memcpy(measured, measuredValues, size * sizeof(float));
    memcpy(real, realValues, size * sizeof(float));
    */

    return (Lerp) {measuredValues, realValues, size};
}

/**
 * Deconstructs a Lerp structure by freeing its allocated memory.
 * @param self Pointer to the Lerp structure.
 */
void deconstruct_lerp(Lerp* self) {
    /*
    free(self->measured);
    free(self->real);

*/}

// measured value on IR sensor should be 3 - rawData
//TODO: filter out measurements < 0.5
float getReal(Lerp* self, float measured) {
    // unsigned char head = 0;

    unsigned char lower = find_index_range_binsearch(self, measured);
    if (lower == self->size - 1) {
        // Case: Measured value is at the upper boundary of the range.
        return calculateSlope(self, lower - 1, lower) * measured;
    }
    else if (lower == 255) {
        // Case: Measured value is below the range of the measured values.
        return calculateSlope(self, 0, 1) * measured;
    }
    else {
        // Normal case: Measured value falls between two points in the range.
        // Calculate the real value using interpolation formula:
        // real[lower] + slope * (measured - measured[lower]).

        //return self->real[lower] + ((measured - self->measured[lower]) * (self->real[lower + 1] - self->real[lower]) / (self->measured[lower + 1] - self->measured[lower]));
           return (self->real[lower]) + ((calculateSlope(self, lower, lower + 1)) * (measured - self->measured[lower]));
        // return calculateSlope(self, lower, lower + 1) * measured;
    }
}


/**
 * Calculates the slope between two points in the Lerp structure.
 * 
 * Slope formula: (y2 - y1) / (x2 - x1).
 *
 * @param self Pointer to the Lerp structure.
 * @param low Index of the first point.
 * @param high Index of the second point.
 * @return float The slope between the two points.
 */
float calculateSlope(Lerp* self, unsigned char low, unsigned char high) {
    return (self->real[high] - self->real[low]) / (self->measured[high] - self->measured[low]);
}



/**
 * Finds the index range where a given measurement falls.
 * 
 * Uses binary search to locate the interval [measured[low], measured[high]]
 * such that measured[low] <= measurement < measured[high].
 * Handles out-of-bound cases by returning a special value (255).
 *
 * @param measurement The measured value to find.
 * @return unsigned char The index of the lower bound of the range, or 255 if out of bounds.
 */
unsigned char find_index_range_binsearch(Lerp* self, float measurement) {
    unsigned char low = 0;
    unsigned char high = self->size - 1;
    unsigned char mid;
    
    // Check if the measurement is below the minimum value in the range.
    if (measurement < self->measured[0]) {
        return 255;
    }

    // Binary search to find the index range where the measurement fits
    while (low <= high) {
        mid = (low + high) / 2;

        // Check if the measurement is in the range [measured[mid], measured[mid + 1]].
        // always truncates so there should be a value greater than mid in the memory at all times (old high value)
        if (measurement >= self->measured[mid] && measurement < self->measured[mid + 1]) {
            // HIT
            return mid;
        }
        else if (measurement < self->measured[mid]) {
            high = mid - 1; // Narrow the range to the lower half.
        }
        else {
            low = mid + 1; // Narrow the range to the upper half.
        }
    }

    // is the low is the last element in the array return that as no suitable element was found
    // otherwise if you hit this point in execution and low is not the last element then something
    // went wrong and you need to return 255
    if (low == self->size - 1) {
        return low;   // Measurement matches the last element.
    }
    else {
        return 255;  // Measurement is out of bounds
    }
}


