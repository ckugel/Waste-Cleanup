

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
    /*
    while (1) {
       lcd_printf("S: %d", sensor_data->cliffLeftSignal);
        oi_update(sensor_data);
        timer_waitMillis(1000);
    }

    while (getEdgeTouching(sensor_data) != 0) {

            lcd_printf("Touching Edge");
            oi_update(sensor_data);
            timer_waitMillis(1000);

        }

*/
    while (getHoleTouching(sensor_data) != 0) {
        oi_update(sensor_data);
        timer_waitMillis(1000);
                lcd_printf("In hole");



}
}
