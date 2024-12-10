#include "scan.h"

// THIS IS NOT HERE TO BE USED IN FINAL CODE. THIS IS ONLY HERE TO SHOW OUR APPROACH THROUGHOUT LABS

Field scan() {
    unsigned short angle;
    undefined_dist = 0xFFFF;
    STEP = 1;
    angles = 180;

    const uint8_t FILTER_SIZE = 9;

    MedianFilter mFilter = create_filter(FILTER_SIZE);

    float IR_Distances[ ANGLE_SIZE ];

    for (angle = 0; angle < angles; angle += STEP) {
        T value;
        IR_Distances[angle] = undefined_dist; // Guarantee a write of BS

        // perform 3 measurements at each location
        // cyBOT_Scan(angle, &scan);
        servo_mode(angle);
        // float lerped = getReal(calculator, scan.IR_raw_val);
        // add_new_data_to_filter(&mFilter, (T) {angle, scan.IR_raw_val});
        // float lerped = (float) scan.IR_raw_val;
        float lerped = adc_read();
        if (angle >= FILTER_SIZE * STEP) {
            if (lerped < 601) {
                lerped = 601;
            }
            value = add_new_data_to_filter(&mFilter, (T) {angle, lerped});
            IR_Distances[(uint16_t) value.angle] = value.distance;
            /*
            char buffer[50];
            sprintf(buffer, "%d         %.2f\r\n", value.angle, IR_Distances[(uint16_t) value.angle]);
            uart_sendStr(buffer);
        */
        }

    }

    IR_Distances[0] = 601;
    IR_Distances[angles - STEP] = 601;

    Obj detected[12];
    uint8_t curr_index = 0;

    Obj current = {undefined_dist, undefined_dist, undefined_dist, undefined_dist, undefined_dist};
    uint16_t lastValidValue = undefined_dist;

    const uint16_t threshold = 800;

    for (angle = 0; angle < angles; angle += STEP) {
        if (IR_Distances[angle] != undefined_dist) {
            // send what we are seeing over putty
            // char buffer[60];
            // sprintf(buffer, "%d             %.2f\r\n", angle, IR_Distances[angle]);
            // uart_sendStr(buffer);

            if (lastValidValue != undefined_dist) { // skip first valid value
                // looking for arriving
                if (current.v1.angle == undefined_dist && IR_Distances[angle] > threshold && IR_Distances[lastValidValue] < threshold) {
                    current.v1 = (T) {angle, IR_Distances[lastValidValue]};
                }
                    // looking for leaving
                else {
                    if (IR_Distances[lastValidValue] > threshold && IR_Distances[angle] < threshold) {
                        current.v2 = (T) {angle, IR_Distances[angle]};
                        detected[curr_index++] = current;
                        current = (Obj) {undefined_dist, undefined_dist, undefined_dist, undefined_dist, undefined_dist};
                    }
                }
            }
            lastValidValue = angle;
        }
    }

    // uart_sendStr("!!!!!!!!!!!!!!!!!!!!!!!!!!");
    cleanse_small_width_objects(detected, &curr_index, STEP);
    uint8_t i;

    for (i = 0; i < curr_index; i++) {
        uint8_t scans;
        MedianFilter filter = create_filter(7);
        uint8_t angleNew = (detected[i].v1.angle + detected[i].v2.angle) / 2;
        servo_mode(angleNew);
        timer_waitMillis(250);
        for (scans = 0; scans < 7; scans++) {
            // cyBOT_Scan(angleNew, &scan);
            add_new_data_to_filter_raw(&filter, angleNew, ping_read());
            // char buf[25];
            // sprintf(buf, "%d     %.2f\r\n", i, scan.sound_dist);
            // uart_sendStr(buf);
            timer_waitMillis(10);
        }
        deconstruct_filter(&filter);
        detected[i].middleDistance = getMedian(&filter).distance;
    }

   //  send_objects_through_putty(detected);
    deconstruct_filter(&mFilter);

    Pillar *pillars;
    pillars = getPillarsFromObject(detected, i);


    Field toReturn;
    toReturn = create_field(pillars, i);
    // free(pillars);
    return toReturn;
}
