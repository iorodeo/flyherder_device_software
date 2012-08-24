#include "Stepper.h"

const uint8_t stepPinDefault = 2;
const uint8_t dirPinDefault = 3;
const uint8_t homePinDefault = 4;

Stepper::Stepper() {
    Stepper(stepPinDefault, dirPinDefault, homePinDefault);
}

Stepper::Stepper(uint8_t stepPin, uint8_t dirPin, uint8_t homePin) {
    _stepPin = stepPin;
    _dirPin = dirPin;
    _homePin = homePin;

    _dirInverted = false;
    _stepInverted = false;

    _running = false;
    _currentPos = 0;
    _targetPos = 0;
}

Stepper::~Stepper() {
    disableOutputs();
}

void Stepper::initialize() {
    pinMode(_homePin, INPUT);
    enableOutputs();
    setPinsInverted(false,false);
}

void Stepper::start() {
    _running = true;
}

void Stepper::stop() {
    _running = false;
}

bool Stepper::isRunning() {
    return _running;
}

void Stepper::setTargetPosition(long position) {
    _targetPos = position;
}

long Stepper::getTargetPosition() {
    return _targetPos;
}

long Stepper::getCurrentPosition() {
    return _currentPos;
}

void Stepper::setCurrentPosition(long position) {
    _targetPos = _currentPos = position;
}

void Stepper::disableOutputs() {   
    digitalWrite(_stepPin, LOW ^ _stepInverted); 
    digitalWrite(_dirPin,  LOW ^ _dirInverted);
}

void Stepper::enableOutputs() {
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin,  OUTPUT);
}

void Stepper::setPinsInverted(bool direction, bool step) {
    _dirInverted = direction;
    _stepInverted = step;
}

void Stepper::setDirInverted() {
    setPinsInverted(true, false);
}

void Stepper::setDirNormal() {
    setPinsInverted(false,false);
}

void Stepper::updateDirPin() {
    if (_running) {
        if (_currentPos <= _targetPos) {
            digitalWrite(_dirPin, HIGH ^ _dirInverted);
            _currentPos += 1;
        }
        else if (_currentPos > _targetPos) {
            digitalWrite(_dirPin, LOW ^ _dirInverted);
            _currentPos -= 1;
        }
    }
}

void Stepper::setStepPinHigh() {
    if (_running) {
    digitalWrite(_stepPin, HIGH ^ _stepInverted);
    }
}

void Stepper::setStepPinLow() {
    digitalWrite(_stepPin, LOW ^ _stepInverted);
    if (_currentPos == _targetPos) {
        _running = false;
    }
}

