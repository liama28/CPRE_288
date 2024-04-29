//Author: Tyler Duquette, Liam Anderson

#include "open_interface.h"
#include "collision.h"
#include "movement.h"

   int lastObstacle = 0;
   double moveForward(oi_t *sensor_data, double distance_mm)
     {
       double sum = 0;

       oi_setWheels(200,200); //Starting at full speed

       while(sum < distance_mm)
       {

           oi_update(sensor_data);
           sum += sensor_data -> distance;

           lastObstacle = collisionCheck(sensor_data);
           if(lastObstacle != 0)
           {
               break;
           }
       }
       oi_setWheels(10,10); //Deaccelerating
       oi_setWheels(0,0); //Stopping

       return sum;
     }

   double backUp(oi_t *sensor, double distance_mm)
   {
       double sum = 0;

             oi_setWheels(-200,-200); //Starting at full speed

             while(sum > -distance_mm)
             {
                 oi_update(sensor);
                 sum += sensor -> distance;
             }
             oi_setWheels(-10,-10); //Deaccelerating
             oi_setWheels(0,0); //Stopping

             return sum;
   }


   void turn_right(oi_t *sensor, double degrees)
   {
       double sum = 0;

       oi_setWheels(50,-50); //Starting at full speed

             while(sum < degrees)
             {
                 oi_update(sensor);
                 sum += sensor -> angle;
             }
       oi_setWheels(5,-5); //Deaccelerating
       oi_setWheels(0,0); //Stopping
   }

   void rightTurn(oi_t *sensor) //Standard 45 Degree Right Turn
   {
       turn_right(sensor,45);
   }

   void turn_left(oi_t *sensor, double degrees)
   {
       double sum = 0;

       oi_setWheels(-50,50); //Starting at full speed

             while(sum > -degrees)
             {
                 oi_update(sensor);
                 sum += sensor -> angle;
             }
       oi_setWheels(-5,5); //Deaccelerating
       oi_setWheels(0,0); //Stopping
   }

   void leftTurn(oi_t *sensor) //Standard 45 Degree Left Turn
   {
       turn_left(sensor,45);
   }

   int getLastObstacle()
   {
       return lastObstacle;
   }


