


#ifndef PwmRead_h
#define PwmRead_h

#include "Arduino.h"
/*
 * Saul Castro
 * 
 * analog smoothing
 * 
  */

class analog{

    public:
    analog(int p){ pin = p; flipa = false; }
    
    analog(int p,bool flip){ pin = p;  flipa = flip; }
    
    analog(int p,bool flip, int n){ pin = p;  flipa = flip; }
    
    int getVal( ){   
      
        int a = analogRead(pin);
        if(flipa)   a = map(a, 0, 1023, 0, 255 );
        else        a = map(a, 0, 1023, 255, 0);

        return getVal(a);
    }
    
    int getVal(int a){
        total = total - readings[readIndex];
      
        // read from the sensor:
        readings[readIndex] = a;
        // add the reading to the total:
        total = total + readings[readIndex];
        // advance to the next position in the array:
        readIndex = readIndex + 1;
      
        // if we're at the end of the array...
        if (readIndex >= numReadings) {
          // ...wrap around to the beginning:
          readIndex = 0;
        }
      
        // calculate the average:
        average = total / numReadings;
  
       return average; }

   private:
   //analog smoothing 
    int pin;
    bool flipa; //flip the analog direction
    static const int numReadings = 7;    //more readings for a slower fade.
    int readings[numReadings];      // the readings from the analog input
    int readIndex = 0;              // the index of the current reading
    int total = 0;                  // the running total
    int average = 0;                // the average

  };


  #endif
