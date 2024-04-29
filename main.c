
#include "Timer.h"
#include "lcd.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include <math.h>
#include "uart.h"
#include "objectDetection.h"
#include "open_interface.h"
#include "movement.h"
#include <ctype.h>

    //Stat Variables
    double distMoved = 0; //In centimeters
    int timesBumped = 0;
    void printStats();

    // Movement Error
    void movementError(int errorCode);

    int main(void)
    {

        timer_init();
        lcd_init();
        objectDetection_init();
        init_uart();
        oi_t *sensor_data = oi_alloc();
        oi_init(sensor_data);


        char buffer[60];
        char str[10] = "";
        char tempStr[3];
        char c = '+';
        int num = 0;
        double tempDist;

        //while not in zone
        while(1){

            uartChar = '+';
            while(uartChar == '+'){
                if((timer_getMillis() % 1000) == 0) {
                    printStats();
                }
            }


            if(uartChar == '\r'){
                uart_sendChar('\n');
                uart_sendChar('\r');
            }

            else if(uartChar == 'e'){
                break;
            }

            else if(uartChar == 'g'){
                uart_sendStr("\n");
                uart_sendStr("Enter distance to travel (cm):\n");
                // Disable interrupts
                toggleInterrupt();
                c = uart_receive();
                str[0] = '\0';
                sprintf(tempStr, "%c", c);
                while(c != '\r') {
                    // Is char c a int
                    if(isdigit((int)c) && strlen(str) < 8) {
                      strcat(str, tempStr);
                    }
                    else {
                      uart_sendStr("Invalid Input");
                    }
                    c = uart_receive();
                    sprintf(tempStr, "%c", c);
                }
                num = atoi(str);
                sprintf(buffer, "Traveling %d cm\n", num);
                uart_sendStr(buffer);
                toggleInterrupt();
                tempDist = moveForward(sensor_data, (double) (num * 10.0));
                distMoved += tempDist;
                movementError(getLastObstacle());
                sprintf(buffer, "Distance moved: %.3lf mm\n", tempDist);
                uart_sendStr(buffer);
                uart_sendStr("Done\n");

            }

            else if(uartChar == 'f'){
                uart_sendStr("\n");
                uart_sendStr("Scanning.....\n");
                cyBot_Object_t getObjects[10];
                int objectsFound = cyBot_FindObjects(getObjects, 7);
                sprintf(buffer, "%d objects found\n", objectsFound);
                uart_sendStr(buffer);
                sprintf(buffer, "%6s %5s %8s %6s\n", "Number", "Angle", "Distance", "Width");
                uart_sendStr(buffer);
                uart_sendStr("----------------------------------\n");
                int i;
                for(i = 0; i < objectsFound; i++) {
                  sprintf(buffer, "%6d %5d, %5.2f, %3.2f\n", i, getObjects[i].angle, getObjects[i].distance, getObjects[i].width);
                  uart_sendStr(buffer);
                }
                uart_sendStr("Done\n");
            }

            else if(uartChar == 't'){
                uart_sendStr("\n");
                uart_sendStr("Enter angle to turn(0-180):\n");
                // Disable interrupts
                toggleInterrupt();
                c = uart_receive();
                str[0] = '\0';
                sprintf(tempStr, "%c", c);
                while(c != '\r') {
                    // Is char c a int
                    if(isdigit((int)c) && strlen(str) < 8) {
                        strcat(str, tempStr);
                    }
                    else {
                        uart_sendStr("Invalid Input");
                    }
                    c = uart_receive();
                    sprintf(tempStr, "%c", c);
                }
                num = atoi(str);
                sprintf(buffer, "Turning to %d degree\n", num);
                uart_sendStr(buffer);
                toggleInterrupt();
                if(num < 90) {
                    turn_left(sensor_data, (90-num));
                }
                else if (num > 90) {
                    turn_right(sensor_data, (num-90));
                }
                uart_sendStr("Done\n");
            }

            else if(uartChar == '?'){
                uart_sendStr("\n");
                uart_GUI();
            }

            else if(uartChar == 'd') {
                uart_sendStr("\n");
                uart_sendStr("Turning RIGHT 45 degrees right....\n");
                turn_right(sensor_data, 45);
                uart_sendStr("Done\n");
            }
            else if(uartChar == 'a') {
                uart_sendStr("\n");
                uart_sendStr("Turning LEFT 45 degrees left....\n");
                turn_left(sensor_data, 45);
                uart_sendStr("Done\n");
            }
            else if(uartChar == 'x') {
                uart_sendStr("\n");
                uart_sendStr("Turning RIGHT 180 degrees right....\n");
                turn_right(sensor_data, 180);
                uart_sendStr("Done\n");
            }
            else if(uartChar == 'w') {
                uart_sendStr("\n");
                uart_sendStr("Moving 10 cm forwards....\n");
                tempDist = moveForward(sensor_data, 100);
                distMoved += tempDist;
                movementError(getLastObstacle());
                sprintf(buffer, "Distance moved: %.3lf mm\n", tempDist);
                uart_sendStr(buffer);

                uart_sendStr("Done\n");
            }
            else if(uartChar == 's') {
                uart_sendStr("\n");
                uart_sendStr("Moving 10 cm backwards....\n");
                tempDist = -backUp(sensor_data, 100);
                distMoved += tempDist;
                sprintf(buffer, "Distance moved: %.3lf mm\n", tempDist);
                uart_sendStr(buffer);
                uart_sendStr("Done\n");

            }
            else if (uartChar == 'q') {
                uart_sendStr("\n");
                uart_sendStr("Quick scanning....\n");
                float qs_num = quick_Scan_angle(90);
                if(qs_num != -1) {
                    sprintf(buffer, "Detected object at %.3f cm\n", qs_num);
                    uart_sendStr(buffer);
                }
                else {
                    uart_sendStr("No object detected\n");
                }
                uart_sendStr("Done\n");
            }

            else {
                uart_sendStr("\n");
                uart_sendStr("Unknown Command\n");
            }
        }
        //if out of while then in zone
        uart_sendStr("\n");
        uart_sendStr("In Zone\n");
        turn_right(sensor_data, 360);

        oi_free(sensor_data);
    }

    void printStats()
    {
        lcd_clear();
        lcd_printf("Current State:\nTime: %d \nDistance: %.3lf \nBumps: %d",timer_getMillis()/1000,distMoved/10,timesBumped);
    }

    void movementError(int errorCode) {
        char buffer[100];
        if(errorCode != 0){
            switch(errorCode){
            case 1:
                sprintf(buffer,"Movement Error: %d, Bump Left\n",getLastObstacle());
                timesBumped++;
                break;
            case 2:
                sprintf(buffer,"Movement Error: %d, Bump right\n",getLastObstacle());
                timesBumped++;
                break;
            case 3:
                sprintf(buffer,"Movement Error: %d, Cliff Left\n",getLastObstacle());
                break;
            case 4:
                sprintf(buffer,"Movement Error: %d, Cliff Right\n",getLastObstacle());
                break;
            case 5:
                sprintf(buffer,"Movement Error: %d, Out of Bounds Wall\n",getLastObstacle());
                break;
            }
            uart_sendStr(buffer);
        }
    }
