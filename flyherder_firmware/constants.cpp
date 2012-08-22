#include "constants.h"
namespace constants {

    const long timerPeriod_us = 300;

    // Communications parameters
    const unsigned int baudrate = 9600;
    const unsigned int deviceModelNumber = 1105;
    const unsigned int deviceSerialNumber = 1267;

    // Axes & dimension properties
    const char dimNames[numDim][nameSize]= {"x", "y"};
    const char axisNames[numAxis][nameSize] = {"x0", "y0", "x1", "y1"};
    const float stepsPerRev = 2000;
    const float threadLead = 0.75*25.4;       // (mm)
    const float maxSeparationDefault = 100;   // (mm)
    const float maxSpeedDefault = 10.0;       // (mm/s)
    const float accelerationDefault = 5.0;    // (mm/s**2)
    const float stepsPerMMDefault = stepsPerRev/threadLead;  

    // Orientation 
    const char allowedOrientation[numOrientation] = {'+', '-'};
    const char orientationDefault = '+';
    const char orientationNormal = '+';
    const char orientationInverted = '-';

    // Stepper params and pin assignment
    const int stepperMode = 1;
    const int ledStatusPin = 6;
    const int drivePowerPin = 7;
    const int driveDisablePin = 5;
    const int driveFaultPin = 8;
    const int stepPinArray[numAxis] = {37,35,33,31};
    const int dirPinArray[numAxis] = {36,34,32,30};
    const int homePinArray[numAxis] = {2,3,18,19}; 

}
