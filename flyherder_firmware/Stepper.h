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
        void home();
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

        void setHomePosition(long position);
        long getHomePosition();
        bool isHome();

        void setHomeSearchNeg();
        void setHomeSearchPos();
        void setHomeSearchDir(char dir);
        char getHomeSearchDir();
        void setHomeSearchDist(long dist);
        long getHomeSearchDist();

        void updateDirPin();
        void setStepPinHigh();
        void setStepPinLow();
        void homeAction();

    private:

        uint8_t _stepPin; 
        uint8_t _dirPin; 
        uint8_t _homePin;

        volatile bool _dirInverted;
        volatile bool _stepInverted;

        char _homeSearchDir;
        long _homeSearchDist;

        uint8_t _stepBitMask;
        uint8_t _dirBitMask;
        uint8_t _stepPort;
        uint8_t _dirPort;
        volatile uint8_t *_dirPortReg;
        volatile uint8_t *_stepPortReg;

        volatile bool _running;
        volatile bool _homing;
        volatile long _currentPos;   // Steps  
        volatile long _targetPos;    // Steps
        volatile long _homePos;

};

inline void Stepper::updateDirPin() {
    if (_running) {
        if (_currentPos <= _targetPos) {
            if (_dirInverted) {
                *_dirPortReg &= ~ _dirBitMask;
            }
            else {
                *_dirPortReg |= _dirBitMask;
            }
            _currentPos += 1;
        }
        else if (_currentPos > _targetPos) {
            if (_dirInverted) {
                *_dirPortReg |= _dirBitMask;
            }
            else {
                *_dirPortReg &= ~ _dirBitMask;
            }
            _currentPos -= 1;
        }
    }
}

inline void Stepper::setStepPinHigh() {
    if (_running) {
        if (_stepInverted) {
            *_stepPortReg &= ~_stepBitMask;
        }
        else { 
            *_stepPortReg |= _stepBitMask;
        }
    }
}

inline void Stepper::setStepPinLow() {
    if (_stepInverted) {
        *_stepPortReg |= _stepBitMask;
    }
    else {
        *_stepPortReg &= ~_stepBitMask;
    }
    if (_currentPos == _targetPos) {
        _running = false;
        _homing = false;
    }
}

#endif 
