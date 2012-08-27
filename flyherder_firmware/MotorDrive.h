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

#ifdef HAVE_ENABLE
        void enable();
        void disable();
        bool isEnabled();
#endif
        bool isPowerOn();
        bool isRunning();

        void setPowerOn();
        void setPowerOff();

        void stop(unsigned int i);
        void start(unsigned int i);
        void stopAll();
        void startAll();

        void home(unsigned int i);
        void homeAll();

        void setSpeed(unsigned int v);

        void setDirection(unsigned int i, char dir);
        void setDirectionAll(Array<char, constants::numAxis> dir);

        void setTargetPosition(unsigned int i, long pos);
        void setTargetPositionAll(Array<long, constants::numAxis> pos);

        long getCurrentPosition(unsigned int i);
        Array<long, constants::numAxis> getCurrentPositionAll();
        void setCurrentPosition(unsigned int i, long pos);
        void setCurrentPositionAll(Array<long, constants::numAxis> pos);

        void setHomePosition(unsigned int i, long pos);
        void setHomePositionAll(Array<long, constants::numAxis> pos);
        long getHomePosition(unsigned int i);
        Array<long, constants::numAxis> getHomePositionAll();

        void setHomeSearchDir(unsigned int i, char dir);
        void setHomeSearchDirAll(Array<char, constants::numAxis> dir);
        char getHomeSearchDir(unsigned int i);
        Array<char, constants::numAxis> getHomeSearchDir();

        void setHomeSearchDist(unsigned int i, long dist);
        void setHomeSearchDistAll(Array<long, constants::numAxis> dist);
        long getHomeSearchDist(unsigned int i);
        Array<long, constants::numAxis> getHomeSearchDist();

        void homeAction(unsigned int i);

    private:
        Array<Stepper,constants::numAxis> _stepper;
        int _powerPin;
#ifdef HAVE_ENABLE
        int _disablePin;
#endif
        int _faultPin;
        bool _powerOnFlag;
        bool _enabledFlag;
};


inline void MotorDrive::update() {
    if (_enabledFlag && _powerOnFlag) {
        for (int i=0; i<constants::numAxis; i++) {
            _stepper[i].updateDirPin();
            _stepper[i].setStepPinHigh();
        }
        for (int i=0; i<constants::numAxis; i++) {
            _stepper[i].setStepPinLow();
        }
    }
}

#endif
