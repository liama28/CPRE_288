//Author: Tyler Duquette, Liam Anderson

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"
#include "collision.h"

double moveForward(oi_t *sensor_data, double distance_mm);

void turn_right(oi_t *sensor, double degrees);

void turn_left(oi_t *sensor, double degrees);

double backUp(oi_t *sensor, double distance_mm);

void rightTurn(oi_t *sensor);

void leftTurn(oi_t *sensor);

int getLastObstacle();

#endif /* MOVEMENT_H_ */
