#include "constants.h"
namespace constants {
    // Communications parameters
    const unsigned int baudrate = 9600;
    const unsigned int deviceModelNumber = 1105;
    const unsigned int deviceSerialNumber = 1267;

    // Axes & dimension properties
    const char dimNames[numDim][nameSize]= {"x", "y"};
    const char axisNames[numAxis][nameSize] = {"x0", "y0", "x1", "y1"};
    const float stepsPerRev = 2000;
    const float threadLead = 0.75*25.4;       // (mm)
    const float maxSeparationDefault = 300;   // (mm)
    const float speedDefault = 10.0;          // (mm/s)
    const float minSpeed = 0.1;               // (mm/s)
    const float maxSpeed = 90.0;              // (mm/s)
    const float stepsPerMMDefault = stepsPerRev/threadLead;  
    const float homeSearchDistScaleFact = 1.5;

    // Orientation 
    const char allowedOrientation[numOrientation] = {'+', '-'};
    const char orientationDefault = '+';
    const char orientationNormal = '+';
    const char orientationInverted = '-';

    // Stepper params and pin assignment
    const int ledStatusPin = 6;
    const int drivePowerPin = 7;
#ifdef HAVE_ENABLE
    const int driveDisablePin = 5;
#endif
    const int driveFaultPin = 8;
    const int stepPinArray[numAxis] = {37,35,33,31};
    const int dirPinArray[numAxis] = {36,34,32,30};
    const int homePinArray[numAxis] = {2,3,18,19}; 
    const int homeInterruptArray[numAxis] = {0,1,5,4};
}
