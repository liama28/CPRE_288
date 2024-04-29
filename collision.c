/*
 * Collision.c
 *
 *  Created on: Apr 13, 2021
 *      Author: bwg
 */
#include "open_interface.h"
#include "Timer.h"
#include "lcd.h"
#include "collision.h"
#include "movement.h"



int collisionCheck(oi_t *sensor_data) {
        oi_update(sensor_data);
        if(sensor_data -> bumpLeft == 1){
           return 1;
        }
        if(sensor_data -> bumpRight == 1) {
           return 2;

        }
        if(sensor_data -> cliffFrontLeftSignal <= 600 || sensor_data -> cliffLeftSignal <= 600)  {
            return 3;

        }

        if(sensor_data -> cliffFrontRightSignal <= 600 || sensor_data -> cliffRightSignal <= 600) {
			return 4;
        }

        if(((sensor_data -> cliffFrontLeftSignal >= 1800)) || (sensor_data -> cliffFrontRightSignal >= 1800)) {
            return 5;
        }
		else{
			return 0;
		}

}



