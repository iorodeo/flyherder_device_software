#ifndef _MOTOR_DRIVE_H_
#define _MOTOR_DRIVE_H_
#include "StepperMotor.h"
#include "constants.h"

class MotorDrive {
    public:
        MotorDrive();
        MotorDrive(int powerPin, int disablePin, int faultPin);
        void initialize();
        void enable();
        void disable();
        void setPowerOn();
        void setPowerOff();
        void update();

        StepperMotor stepper[constants::numAxis];

    //private:
        int _powerPin;
        int _disablePin;
        int _faultPin;
};

#endif
