#include "Stepper.h"
#include "Streaming.h"

const uint8_t stepPinDefault = 2;
const uint8_t dirPinDefault  = 3;
const uint8_t homePinDefault = 4;
const long homeSearchDistDefault = 1000;

Stepper::Stepper() {
    Stepper(stepPinDefault, dirPinDefault, homePinDefault);
}

Stepper::Stepper(uint8_t stepPin, uint8_t dirPin, uint8_t homePin) {
    _stepPin = stepPin;
    _dirPin = dirPin;
    _homePin = homePin;

    _dirInverted = false;
    _stepInverted = false;

    _homeSearchDir = '+';
    _homeSearchDist = labs(homeSearchDistDefault);

    _running = false;
    _homing = false;
    _currentPos = 0;
    _targetPos = 0;
    _homePos = 0;
}

Stepper::~Stepper() {
    disableOutputs();
}

void Stepper::initialize() {

    pinMode(_homePin, INPUT);
    digitalWrite(_homePin, HIGH);
    enableOutputs();
    setPinsInverted(false,false);

    _stepBitMask = digitalPinToBitMask(_stepPin);
    _stepPort = digitalPinToPort(_stepPin);
    _stepPortReg = portOutputRegister(_stepPort);

    _dirBitMask = digitalPinToBitMask(_dirPin);
    _dirPort = digitalPinToPort(_dirPin);
    _dirPortReg = portOutputRegister(_dirPort);

}

void Stepper::start() {
    // Should be called in an atomic block
    _running = true;
}

void Stepper::stop() {
    // Should be called in an atomic block
    _running = false;
}

void Stepper::home() {
    int homeVal = digitalRead(_homePin);
    if (homeVal == LOW) {
        _homing = false;
        _running = false;
        _currentPos = _homePos;
    } 
    else {
        // Should be called in an atomic block
        if (_homeSearchDir == '+') {
            _targetPos = _currentPos + labs(_homeSearchDist);
        }
        else {
            _targetPos = _currentPos - labs(_homeSearchDist);
        }
        _homing = true;
        _running = true;
    }
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
    _targetPos = position;
    _currentPos = position;
}

void Stepper::setHomePosition(long position) {
    _homePos = position;
}

long Stepper::getHomePosition() {
    return _homePos;
}

void Stepper::setHomeSearchDir(char dir) {
    if ((dir == '+') || (dir == '-')) {
        _homeSearchDir = dir;
    }
}

char Stepper::getHomeSearchDir() {
    return _homeSearchDir;
}

void Stepper::setHomeSearchDist(long dist) {
    _homeSearchDist = labs(dist);
}

long Stepper::getHomeSearchDist() {
    return _homeSearchDist;
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

void Stepper::homeAction() {
    if (_homing) {
        _homing = false;
        _running = false;
        _currentPos = _homePos;
    }
}

