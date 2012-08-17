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
    return _running;
}

void StepperMotor::start() {
    _running = true;
}

void StepperMotor::stop() {
    _running = false;
}

void StepperMotor::update() {
    if (_running) {
        run();
    }
}

