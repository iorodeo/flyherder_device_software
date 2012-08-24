#ifndef _STEPPER_MOTOR_H_
#define _STEPPER_MOTOR_H_
#include "AccelStepper.h"
#include "constants.h"


class StepperMotor : public AccelStepper {
    public:
        StepperMotor();
        StepperMotor(int mode, int stepPin, int dirPin, int homePin);
        ~StepperMotor();
        void initialize();
        void stop();
        void start();
        void setDirInverted();
        void setDirNormal();
        bool isRunning();
        void start();
        void stop();
        void update();
        void computeNewSpeed();
    //private:
        int _homePin;
        volatile bool _running;
};
#endif
