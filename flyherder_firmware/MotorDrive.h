#ifndef _MOTOR_DRIVE_H_
#define _MOTOR_DRIVE_H_
#include "Stepper.h"
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

        void setSpeed(unsigned int v);

        void setDirection(unsigned int i, char dir);
        void setDirectionAll(Array<char,constants::numAxis> dir);

        void setTargetPosition(unsigned int i, long pos);
        void setTargetPositionAll(Array<long,constants::numAxis> pos);

        long getCurrentPosition(unsigned int i);
        Array<long, constants::numAxis> getCurrentPositionAll();

    //private:
        Array<Stepper,constants::numAxis> _stepper;
        int _powerPin;
        int _disablePin;
        int _faultPin;
        bool _powerOnFlag;
        bool _enabledFlag;
};

#endif
