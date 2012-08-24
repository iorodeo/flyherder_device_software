#ifndef _SYSTEM_STATE_H_
#define _SYSTEM_STATE_H_
#include "constants.h"
#include "Array.h"
#include "MotorDrive.h"

enum {SYS_ERR_BUF_SZ=50};

class SystemState {

    public:
        SystemState();
        void initialize();

        void setDrivePowerOn();
        void setDrivePowerOff();
        bool isDrivePowerOn();

        void enable();
        void disable();
        bool isEnabled();

        void stop();
        bool isRunning();

        bool moveToPosition(Array<float,constants::numAxis> posMM);
        bool moveAxisToPosition(int axis, float posMM);
        bool moveToHome();

        Array<float,constants::numAxis> getPosition();
        float getAxisPosition(int axis);

        void setMaxSeparationToDefault();
        bool setMaxSeparation(Array<float,constants::numDim> maxSeparation);
        Array<float,constants::numDim> getMaxSeparation();

        bool setSpeed(float v);
        float getSpeed();

        bool isInHomePosition();

        void setOrientationToDefault();
        bool setOrientation(Array<char,constants::numAxis> orientation);
        Array<char,constants::numAxis> getOrientation();

        bool setAxisOrientation(int axis, char orientation);
        char getAxisOrientation(int axis);

        void setStepsPerMMToDefault();
        bool setStepsPerMM(float stepsPerMM);
        float getStepsPerMM();

        void setLedStatusOn();
        void setLedStatusOff();

        void setErrMsg(char *);
        char errMsg[SYS_ERR_BUF_SZ];

    //private:

        bool checkAxisArg(int axis);
        Array<float,constants::numDim> _maxSeparation;
        Array<char,constants::numAxis> _orientation;
        float _stepsPerMM;   
        float _speed;
        float _acceleration;
        MotorDrive motorDrive;
        
};

extern SystemState systemState;
void timerUpdate();
#endif
