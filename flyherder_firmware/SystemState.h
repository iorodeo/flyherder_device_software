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
        void update();

        void setDrivePowerOn();
        void setDrivePowerOff();
        bool isDrivePowerOn();

        void enable();
        void disable();
        bool isEnabled();

        void start();
        void stop();
        bool isRunning();

        bool moveToPosition(Array<float,constants::numAxis> pos);
        bool moveAxisToPosition(int axis, float pos);
        bool moveToHome();

        Array<float,constants::numAxis> getPosition();
        float getAxisPosition(int axis);

        void setMaxSeparationToDefault();
        bool setMaxSeparation(Array<float,constants::numDim> maxSeparation);
        Array<float,constants::numDim> getMaxSeparation();

        bool setMaxSpeed(float v);
        float getMaxSpeed();

        bool setAcceleration(float a);
        float getAcceleration();

        bool isInHomePosition();

        void setOrientationToDefault();
        bool setOrientation(Array<char,constants::numAxis> orientation);
        Array<char,constants::numAxis> getOrientation();

        bool setAxisOrientation(int axis, char orientation);
        char getAxisOrientation(int axis);

        void setStepsPerMMToDefault();
        bool setStepsPerMM(float stepsPerMM);
        float getStepsPerMM();

        void setLEDStatusPinOn();
        void setLEDStatusPinOff();

        void setErrMsg(char *);
        char errMsg[SYS_ERR_BUF_SZ];

        // DEVELOPMENT
        volatile long timerCount;

    private:

        bool checkAxisArg(int axis);

        volatile bool _enabled;
        Array<float,constants::numDim> _maxSeparation;
        Array<char,constants::numAxis> _orientation;
        float _stepsPerMM;   
        float _maxSpeed;
        float _acceleration;
        MotorDrive motorDrive;
        
};

extern SystemState systemState;
void timerUpdate();
#endif
