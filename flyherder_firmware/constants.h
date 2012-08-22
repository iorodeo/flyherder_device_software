#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

namespace constants {
    enum {numDim=2};
    enum {numAxis=2*numDim};    
    enum {nameSize=3};
    enum {numOrientation=2};
    extern const long timerPeriod_us;
    extern const unsigned int baudrate;
    extern const unsigned int deviceModelNumber;
    extern const unsigned int deviceSerialNumber; 
    extern const char dimNames[numDim][nameSize];
    extern const char axisNames[numAxis][nameSize];
    extern const float stepsPerRev;
    extern const float threadLead; 
    extern const float maxSeparationDefault;
    extern const float maxSpeedDefault; 
    extern const float accelerationDefault;
    extern const float stepsPerMMDefault;
    extern const char allowedOrientation[numOrientation];
    extern const char orientationDefault;
    extern const char orientationNormal;
    extern const char orientationInverted;
    extern const int stepperMode;
    extern const int ledStatusPin;
    extern const int driveDisablePin;
    extern const int drivePowerPin;
    extern const int driveFaultPin;
    extern const int stepPinArray[numAxis];
    extern const int dirPinArray[numAxis];
    extern const int homePinArray[numAxis];
}
#endif
