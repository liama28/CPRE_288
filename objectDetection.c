/*
 * objectDetection.c
 *
 *  Created on: Apr 13, 2021
 *      Author: ianjohn
 */
#include "objectDetection.h"

/**
  * @brief Initializes ADC,Ping, and Servo.
  * Must be called before any distances can be captured.
  */
void objectDetection_init(void){
    ping_init();
    adc_init();
    servo_init();
}

/**
  * @brief Helper function for find objects. Rotate sensors to specified angle, and gets a scan values for IR and Ping
  * IR sensor uses an average of 20 samples.
  * @param angle: angle for servo to rotate to
  * @param getScan: pointer to Scan struct for storing scan data
  */
void servo_Scan(int angle, cyBOT_Scan_t* getScan){
    //moves Servo to desired angle
    move_servo(angle);
    //Gets ping distance
    getScan->sound_dist = ping_Scan();
    //ADC gathers data over 20 samples
    getScan->IR_raw_val = average_ADC_Raw(20);
}

/**
  * @brief Finds objects during 0 to 180 degree scan
  * @param getObjects[]: passed in to store the results of the scan
  * @param size_of_input: size of getObjects[] array that was passed in
  * @retval Returns the number of objects found
  */
int cyBot_FindObjects(cyBot_Object_t getObjects[], int size_of_input){
    //scan data gets updates every iteration
    cyBOT_Scan_t scanData = {0,0};
    //needs a pointer to pass to servo_Scan(int angle, cyBOT_Scan_t* getScan)
    cyBOT_Scan_t *scanData_p = &scanData;

    //bools used for detecting rising and falling edge of objects
    //this iteration there is an object
    bool objectIsDetected = false;
    //last iteration there was an object
    bool objectWasDetected = false;
    //when an object is found record the angle
    int initialAngle = 0;
    //distance to object being detected
    float dist = 0;
    //can be 2 or 1, used for the for loop. how quickly it scans
    float scanResolution = 2;
    //number of objects found
    int numObjFound = 0;

    //quick scan to get bugs out
    servo_Scan(0, scanData_p);

    //loop form 0-180 degrees scanning every scanResolution degrees, generally 2 or 1
    int angle = 0;
    for(angle = 0; angle < 180; angle += scanResolution){

        //populate scan data at given angle
        //timer_waitMicros(5);
        servo_Scan(angle, scanData_p);
        //timer_waitMicros(5);

        objectIsDetected = (scanData.IR_raw_val > 200);

        //if this is a new object being found record the angle
        if(objectIsDetected && !objectWasDetected){
            //increase number of objects count
            numObjFound++;
            //record initial angle
            initialAngle = angle;
            objectWasDetected = objectIsDetected;
            //set baseline dist
            //dist = getReliableDist(lastScans[1]);
            dist = getReliableDist(scanData);

        }
        //if in the middle of an object
        else if(objectIsDetected && objectWasDetected){
            //if distance is valid and closer than previous distances, dist should equal closest point on object.
            float currentDist = getReliableDist(scanData);
            //updates dist if needed
            if(currentDist > 0 && currentDist < dist){
                //set distance to be the closest value
                dist = currentDist;
            }
        }
        //if this is the end of the object record the angle
        else if(!objectIsDetected && objectWasDetected){
            //find difference of angles
            int radialSize = angle - initialAngle;

            objectWasDetected = objectIsDetected;

            //calculate arc length based on angle and dist
            float width = (radialSize * ((3.1415926)/180.0) * dist);

            //create new object
            cyBot_Object_t newObj = {angle - (radialSize / 2), dist, width};

            //place new object into array
            //if our array is not full, put Obj in array
            if(numObjFound <= size_of_input){
                getObjects[numObjFound-1] = newObj;
            }
        }
    }
    //return number of objects found
    return numObjFound;
}

/**
  * @brief Finds objects during 0 to 180 degree scan
  * @param getObjects[]: passed in to store the results of the scan
  * @param size_of_input: size of getObjects[] array that was passed in
  * @param initAngle: initial angle for the scan
  * @param finalAngle: final angle for scan
  * @retval Returns the number of objects found
  */
int cyBot_FindObjects_angle(cyBot_Object_t getObjects[], int size_of_input, int initAngle, int finalAngle){
    //scan data gets updates every iteration
    cyBOT_Scan_t scanData = {0,0};
    //needs a pointer to pass to servo_Scan(int angle, cyBOT_Scan_t* getScan)
    cyBOT_Scan_t *scanData_p = &scanData;

    //bools used for detecting rising and falling edge of objects
    //this iteration there is an object
    bool objectIsDetected = false;
    //last iteration there was an object
    bool objectWasDetected = false;
    //when an object is found record the angle
    int initialAngle = 0;
    //distance to object being detected
    float dist = 0;
    //can be 2 or 1, used for the for loop. how quickly it scans
    float scanResolution = 2;
    //number of objects found
    int numObjFound = 0;

    //quick scan to get bugs out
    servo_Scan(0, scanData_p);

    //loop form initAngle-finalAngle degrees scanning every scanResolution degrees, generally 2 or 1
    int angle = 0;
    for(angle = initAngle; angle < finalAngle; angle += scanResolution){

        //populate scan data at given angle
        //timer_waitMicros(5);
        servo_Scan(angle, scanData_p);
        //timer_waitMicros(5);

        objectIsDetected = (scanData.IR_raw_val > 200);

        //if this is a new object being found record the angle
        if(objectIsDetected && !objectWasDetected){
            //increase number of objects count
            numObjFound++;
            //record initial angle
            initialAngle = angle;
            objectWasDetected = objectIsDetected;
            //set baseline dist
            //dist = getReliableDist(lastScans[1]);
            dist = getReliableDist(scanData);

        }
        //if in the middle of an object
        else if(objectIsDetected && objectWasDetected){
            //if distance is valid and closer than previous distances, dist should equal closest point on object.
            float currentDist = getReliableDist(scanData);
            //updates dist if needed
            if(currentDist > 0 && currentDist < dist){
                //set distance to be the closest value
                dist = currentDist;
            }
        }
        //if this is the end of the object record the angle
        else if(!objectIsDetected && objectWasDetected){
            //find difference of angles
            int radialSize = angle - initialAngle;

            objectWasDetected = objectIsDetected;

            //calculate arc length based on angle and dist
            float width = (radialSize * ((3.1415926)/180.0) * dist);

            //create new object
            cyBot_Object_t newObj = {angle - (radialSize / 2), dist, width};

            //place new object into array
            //if our array is not full, put Obj in array
            if(numObjFound <= size_of_input){
                getObjects[numObjFound-1] = newObj;
            }
        }
    }
    //return number of objects found
    return numObjFound;
}

/**
  * @brief Helper function for findObjects. Uses ranges to find appropriate distance, between IR and Ping.
  * @param scanData: passed in values of IR and Ping. generally given by servo_Scan()
  * @retval Returns distance value (cm) based on best sensor range, or -1 if noise
 */
float getReliableDist(cyBOT_Scan_t scanData){
    if(scanData.IR_raw_val > 200){
        //range < 8 cm is best for Ping
        if(scanData.sound_dist < 8){
            return scanData.sound_dist;
        }
        //best range for IR is between 8cm and 50cm
        else if(scanData.sound_dist >= 8 && scanData.sound_dist < 50){
            return ADC_getDist(scanData.IR_raw_val);
        }
        //else Ping is better for 50cm to ~300cm
        else if(scanData.sound_dist < 290){
            return scanData.sound_dist;
        }
    }
    //otherwise no distance. -1 is an invalid distance i.e noise
    return -1;
}

/**
  * @brief Quickly scans in front of the servo
  * @retval Returns distance value (cm) based on best sensor range, or -1 if noise
 */
float quick_Scan(void) {
    //Gets ping distance and ADC gathers data over 20 samples
    cyBOT_Scan_t quickScan = {ping_Scan(), average_ADC_Raw(20)};
    return getReliableDist(quickScan);
}

/**
  * @brief Quickly scans in front of the servo 
  * @param angle: angle to scan
  * @retval Returns distance value (cm) based on best sensor range, or -1 if noise
 */
float quick_Scan_angle(float angle) {
	//moves servo
	move_servo(angle);
    //Gets ping distance and ADC gathers data over 20 samples
    cyBOT_Scan_t quickScan = {ping_Scan(), average_ADC_Raw(20)};
    return getReliableDist(quickScan);
}

/**
 * @brief Calls ping_getDistance(). Uses Ping Sensor
 * @retval Rreturns distance (cm) to object, where servo is pointing.
 */
float ping_Scan(){
    //returns ping distance in cm
    return ping_getDistance();
}

/**
  * @brief Helper function for converting ADC raw values. Stores equation for average_ADC_Raw(int samples)
  * @param RawSensorVal: Raw data to be converted
  * @retval Returns converted distance (cm)
 */
float ADC_getDist(int RawSensorVal){
    //return converted sensorVal in cm
    return (float)(97616 * (pow(RawSensorVal, -1.14)));
}

/**
  * @brief Gets raw ADC value. Averages values based on samples
  * @param samples: Number of samples taken
  * @retval Returns raw average ADC value
  */
int average_ADC_Raw(int samples){
    int i;
    int total = 0;
    //loop over total samples
    for(i = 0; i < samples; i++){
        total += adc_read();
    }
    //return average raw data
    return (total / samples);
}

/**
  * @brief Converts raw ADC average value to distance. Uses ADC_getDist(int RawSensorVal)
  * @param samples: Number of samples taken
  * @retval Returns ADC distance (cm)
  */
float average_ADC_Dist(int samples){
    //return average ADC dist in cm
    return ADC_getDist(average_ADC_Raw(samples));
}

/**
  * @brief moves servo to chosen angle in degrees 
  * @param angle: Angle to move servo to
  * @retval Returns mask value on servo
  */
int move_servo(float angle){
	return servo_move(angle);
}

/**
  * @brief points to smallest width after a 360 scan
  * @param sensor: sensor for rotating
  * @retval distance to smallest
  */
double pointToSmallest(cyBot_Object_t getObjects[], int size_of_input, oi_t *sensor){
    cyBot_Object_t small = findSmallest(getObjects, size_of_input);
    turn_right(sensor, (double) small.angle);
    return (double) small.distance;
}

/**
  * @brief Helper function for checkForZone. Gets smallest width objects
  * @param Objects: objects included in average
  * @param numObjects: length of object array
  * @retval Returns object with smallest width
  */
cyBot_Object_t findSmallest(cyBot_Object_t Objects[], int numObjects){
    int i;

    cyBot_Object_t smallestWidth = Objects[0];
    for(i = 0; i < numObjects; i++){
        if(Objects[i].width < smallestWidth.width){
            if(Objects[i].angle <= 180 && Objects[i].distance < 300 && Objects[i].width < 20 && Objects[i].width > 0){
                smallestWidth = Objects[i];
            }
        }
    }
    cyBot_Object_t nullObj = {0,0,0};
    if(numObjects == 0 || smallestWidth.distance < 1 || smallestWidth.width < 1){
            return nullObj;
    }
    return smallestWidth;
}
