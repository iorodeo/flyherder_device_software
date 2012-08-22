#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "MotorDrive.h"

MotorDrive::MotorDrive() 
{
    _powerPin = constants::drivePowerPin;
    _disablePin = constants::driveDisablePin;
    _faultPin = constants::driveFaultPin;
    _powerOnFlag = false;
    _enabledFlag = false;
}

MotorDrive::MotorDrive(int powerPin, int disablePin, int faultPin) {
    _powerPin = powerPin;
    _disablePin = disablePin;
    _faultPin = faultPin;
}

void MotorDrive::initialize() {
    // Set pin modes for drive control pins
    pinMode(_powerPin, OUTPUT);
    pinMode(_disablePin, OUTPUT);
    pinMode(_faultPin, INPUT);
    setPowerOff();
    disable();

    // Assign pins (step, dir, home) to motors
    for (int i=0; i<constants::numAxis; i++) {
        _stepper[i].disableOutputs();
        _stepper[i] = StepperMotor(
                constants::stepperMode,
                constants::stepPinArray[i],
                constants::dirPinArray[i],
                constants::homePinArray[i]
                );
        _stepper[i].initialize();
        _stepper[i].run();
    }
}

void MotorDrive::update() {
    if (isPowerOn() && isEnabled()) {
        for (int i=0; i<constants::numAxis; i++) {
            _stepper[i].update();
        }
    }
}

void MotorDrive::enable() {
    digitalWrite(_disablePin,LOW);
    _enabledFlag = true;
}

void MotorDrive::disable() {
    digitalWrite(_disablePin,HIGH);
    _enabledFlag = false;
    stopAll();
}


void MotorDrive::stop(unsigned int i) {
    if (i<constants::numAxis) {
        _stepper[i].stop();
    }
}

void MotorDrive::start(unsigned int i) {
    if (i<constants::numAxis) {
        _stepper[i].start();
    }
}

void MotorDrive::stopAll() {
    for (int i=0; i<constants::numAxis; i++) {
        stop(i);
    }
}

void MotorDrive::startAll() {
    for (int i=0; i<constants::numAxis; i++) {
        start(i);
    }
}

bool MotorDrive::isPowerOn() {
    return _powerOnFlag;
    //if (digitalRead(_powerPin) == HIGH) {
    //    return true;
    //}
    //else {
    //    return false;
    //}
}

bool MotorDrive::isEnabled() {
    return _enabledFlag;
    //if (digitalRead(_disablePin)==LOW) {
    //    return true;
    //}
    //else {
    //    return false;
    //}
}

bool MotorDrive::isRunning() {
    bool flag = false;
    for (int i=0; i<constants::numAxis; i++) {
        if (_stepper[i].isRunning()) {
            flag = true;
        }
    }
    return flag;
}
void MotorDrive::setPowerOn() {
    digitalWrite(_powerPin,HIGH);
    _powerOnFlag = true;
}

void MotorDrive::setPowerOff() {
    digitalWrite(_powerPin,LOW);
    _powerOnFlag = false;
    disable();
}

void MotorDrive::setMaxSpeed(unsigned int i, float v) {
    if (i < constants::numAxis) {
        _stepper[i].setMaxSpeed(v);
    }
}

void MotorDrive::setMaxSpeedAll(float v) {
    for (int i=0; i<constants::numAxis; i++) {
        setMaxSpeed(i,v);
    }
}

void MotorDrive::setAcceleration(unsigned int i, float a) {
    if (i < constants::numAxis) {
        _stepper[i].setAcceleration(a);
    }
}

void MotorDrive::setAccelerationAll(float a) {
    for (int i=0; i<constants::numAxis; i++) {
        setAcceleration(i,a);
    }
}

void MotorDrive::setDirection(unsigned int i, char dir) {
    if (i < constants::numAxis) {
        if (dir == constants::orientationInverted) {
            _stepper[i].setDirInverted();
        }
        else {
            _stepper[i].setDirNormal();
        }
    }
}
void MotorDrive::setDirectionAll(Array<char,constants::numAxis> dir) {
    for (int i=0; i<constants::numAxis; i++) {
        setDirection(i,dir[i]);
    }
}

Array<long, constants::numAxis> MotorDrive::currentPositionAll() {
    Array<long, constants::numAxis> position;
    for (int i=0; i<constants::numAxis; i++) {
        position[i] = _stepper[i].currentPosition();
    }
    return position;
}

long MotorDrive::currentPosition(unsigned int i) {
    long rtnVal = 0;
    if (i < constants::numAxis) {
        rtnVal = _stepper[i].currentPosition();
    }
    return rtnVal;
}

void MotorDrive::setTargetPosAbs(unsigned int i, long posAbs) {
    if (i < constants::numAxis) {
        _stepper[i].moveTo(posAbs);
    }
}

void MotorDrive::setTargetPosRel(unsigned int i, long posRel) {
    if (i < constants::numAxis) {
        _stepper[i].move(posRel);
    }
}

void MotorDrive::setTargetPosAbsAll(Array<long,constants::numAxis> posAbs) {
    for (int i=0; i<constants::numAxis; i++) {
        setTargetPosAbs(i,posAbs[i]);
    }
}

void MotorDrive::setTargetPosRelAll(Array<long,constants::numAxis> posRel) {
    for (int i=0; i<constants::numAxis; i++) {
        setTargetPosRel(i,posRel[i]);
    }
}
