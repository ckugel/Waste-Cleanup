

/**
 * main.c
 * Our main code for driving the CyBot
 */

#include "lcd.h"
#include "open_interface.h"
#include "Timer.h"
#include "cliff.h"

int main(void)
{
    timer_init();
    lcd_init();

    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    while (1) {
       lcd_printf("S: %d\nValue: %d", sensor_data->cliffLeftSignal, getHoleTouching(sensor_data));
        oi_update(sensor_data);
        timer_waitMillis(1000);
    }
/*
    while (getTargetTouching(sensor_data) != 0) {

            lcd_printf();
            oi_update(sensor_data);
            timer_waitMillis(1000);

        }

*/





}
