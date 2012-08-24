// Stepper.h
#ifndef Stepper_h
#define Stepper_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif


class Stepper {

    public:
        Stepper();
        Stepper(uint8_t stepPin, uint8_t dirPin, uint8_t homePin);
        ~Stepper();

        void initialize();
        void start();
        void stop();
        bool isRunning();
        void disableOutputs();
        void enableOutputs();
        void setTargetPosition(long position);
        long getTargetPosition();
        long getCurrentPosition();  
        void setCurrentPosition(long position);  
        void setPinsInverted(bool direction, bool step);
        void setDirInverted();
        void setDirNormal();

        void updateDirPin();
        void setStepPinHigh();
        void setStepPinLow();

    private:
        uint8_t _stepPin; 
        uint8_t _dirPin; 
        uint8_t _homePin;

        bool _dirInverted;
        bool _stepInverted;

        volatile bool _running;
        volatile long _currentPos;   // Steps  
        volatile long _targetPos;    // Steps
};

#endif 
