#ifndef _MOTOR_DRIVE_H_
#define _MOTOR_DRIVE_H_
#include "StepperMotor.h"
#include "Array.h"
#include "constants.h"

class MotorDrive {
    public:
        MotorDrive();
        MotorDrive(int powerPin, int disablePin, int faultPin);
        void initialize();
        void update();

        void enable();
        void disable();

        bool isEnabled();
        bool isPowerOn();
        bool isRunning();

        void setPowerOn();
        void setPowerOff();

        void stop(unsigned int i);
        void start(unsigned int i);
        void stopAll();
        void startAll();

        void setMaxSpeed(unsigned int i, float v);
        void setMaxSpeedAll(float v);

        void setAcceleration(unsigned int i, float a);
        void setAccelerationAll(float a);

        void setDirection(unsigned int i, char dir);
        void setDirectionAll(Array<char,constants::numAxis> dir);

        void setTargetPosAbs(unsigned int i, long posAbs);
        void setTargetPosRel(unsigned int i, long posRel);
        void setTargetPosAbsAll(Array<long,constants::numAxis> posAbs);
        void setTargetPosRelAll(Array<long,constants::numAxis> posRel);

        long currentPosition(unsigned int i);
        Array<long, constants::numAxis> currentPositionAll();

    private:
        Array<StepperMotor,constants::numAxis> _stepper;
        int _powerPin;
        int _disablePin;
        int _faultPin;
        bool _powerOnFlag;
        bool _enabledFlag;
};

#endif
