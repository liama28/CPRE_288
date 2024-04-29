/*
 * objectDetection.h
 *
 *  Created on: Apr 13, 2021
 *      Author: ianjohn
 */

/**
 * Combines ADC, Ping, and Servo classes into one
 * The idea is that you should talk through objectDetection for any distance sensor, instead of using them directly.
 */

#ifndef OBJECTDETECTION_H_
#define OBJECTDETECTION_H_

#include <stdint.h>
#include <math.h>
#include <inc/tm4c123gh6pm.h>
#include "ping.h"
#include "servo.h"
#include "ADC.h"
#include "open_interface.h"
#include "movement.h"

//struct for find objects
typedef struct{
    int angle; //degrees
    float distance; //cm
    float width; //cm
} cyBot_Object_t;

// struct for scanning and receiving sensor values
//Scan value
typedef struct{
    float sound_dist;  // Distance from Ping Sensor
    int IR_raw_val;    // Raw ADC value from IR sensor
} cyBOT_Scan_t;

/**
  * @brief Initializes ADC,Ping, and Servo.
  * Must be called before any distances can be captured.
  */
void objectDetection_init(void);

/**
  * @brief Helper function for find objects. Rotate sensors to specified angle, and gets a scan values for IR and Ping
  * IR sensor uses an average of 20 samples.
  * @param angle: angle for servo to rotate to
  * @param getScan: pointer to Scan struct for storing scan data
 */
void servo_Scan(int angle, cyBOT_Scan_t* getScan);

/**
  * @brief Finds objects during 0 to 180 degree scan
  * @param getObjects[]: passed in to store the results of the scan
  * @param size_of_input: size of getObjects[] array that was passed in
  * @retval Returns the number of objects found
  */
int cyBot_FindObjects(cyBot_Object_t getObjects[], int size_of_input);

/**
  * @brief Finds objects during 0 to 180 degree scan
  * @param getObjects[]: passed in to store the results of the scan
  * @param size_of_input: size of getObjects[] array that was passed in
  * @param initAngle: initial angle for the scan
  * @param finalAngle: final angle for scan
  * @retval Returns the number of objects found
  */
int cyBot_FindObjects_angle(cyBot_Object_t getObjects[], int size_of_input, int initAngle, int finalAngle);

/**
  * @brief Helper function for findObjects. Uses ranges to find appropriate distance, between IR and Ping.
  * @param scanData: passed in values of IR and Ping. generally given by servo_Scan()
  * @retval Returns distance value (cm) based on best sensor range, or -1 if noise
 */
float getReliableDist(cyBOT_Scan_t scanData);

/**
  * @brief Quickly scans in front of the servo
  * @retval Returns distance value (cm) based on best sensor range, or -1 if noise
 */
float quick_Scan(void);

/**
  * @brief Quickly scans in front of the servo, moves servo to angle
  * @param angle: angle to scan
  * @retval Returns distance value (cm) based on best sensor range, or -1 if noise
 */
float quick_Scan_angle(float angle);

/**
 * @brief Calls ping_getDistance(). Uses Ping Sensor
 * @retval Rreturns distance (cm) to object, where servo is pointing.
 */
float ping_Scan(void);

/**
  * @brief Helper function for converting ADC raw values. Stores equation for average_ADC_Raw(int samples)
  * @param RawSensorVal: Raw data to be converted
  * @retval Returns converted distance (cm)
 */
float ADC_getDist(int RawSensorVal);

/**
  * @brief Gets raw ADC value. Averages values based on samples
  * @param samples: Number of samples taken
  * @retval Returns raw average ADC value
  */
int average_ADC_Raw(int samples);

/**
  * @brief Converts raw ADC average value to distance. Uses ADC_getDist(int RawSensorVal)
  * @param samples: Number of samples taken
  * @retval Returns ADC distance (cm)
  */
float average_ADC_Dist(int samples);

/**
  * @brief moves servo to chosen angle in degrees 
  * @param angle: Angle to move servo to
  * @retval Returns mask value on servo
  */
int move_servo(float angle);

/**
  * @brief points to smallest width after a 360 scan
  * @param sensor: sensor for rotating
  * @retval distance to smallest
  */
double pointToSmallest(cyBot_Object_t getObjects[], int size_of_input, oi_t *sensor);

/**
  * @brief Helper function for checkForZone. Gets smallest width objects
  * @param Objects: objects included in average
  * @param numObjects: length of object array
  * @retval Returns object with smallest width
  */
cyBot_Object_t findSmallest(cyBot_Object_t Objects[], int numObjects);

#endif /* OBJECTDETECTION_H_ */
