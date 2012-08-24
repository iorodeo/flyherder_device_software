#include <util/atomic.h>
#include "StepperMotor.h"

StepperMotor::StepperMotor() {
    _running = false;
}

StepperMotor::StepperMotor(
        int mode, 
        int stepPin, 
        int dirPin,
        int homePin
        ) : AccelStepper(mode,stepPin,dirPin) 
{ 
    _running = false;
    _homePin = homePin;
}

StepperMotor::~StepperMotor() {
    disableOutputs();
}

void StepperMotor::initialize() {
    enableOutputs();
    setDirNormal();
}

void StepperMotor::setDirInverted() {
   setPinsInverted(true, false, false); 
}

void StepperMotor::setDirNormal() {
   setPinsInverted(false, false, false);
}

bool StepperMotor::isRunning() {
    bool running;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        running = _running;
    }
    return running;
}

void StepperMotor::start() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        _running = true;
    }
}

void StepperMotor::stop() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { 
        _running = false;
    }
}

void StepperMotor::update() {
    if (_running) {
        if (currentPosition() == targetPosition() && speed()==0) {
            _running = false;
        } 
        else {
            run();
        }
    }
}

void StepperMotor::computeNewSpeed() {
    AccelStepper::computeNewSpeed();
}
