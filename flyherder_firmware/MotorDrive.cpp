#include <util/atomic.h>
#include <TimerOne.h>
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
    unsigned int speedInSteps;
    // Set pin modes for drive control pins
    pinMode(_powerPin, OUTPUT);
    pinMode(_disablePin, OUTPUT);
    pinMode(_faultPin, INPUT);
    setPowerOff();
    disable();

    // Assign pins (step, dir, home) to motors
    for (int i=0; i<constants::numAxis; i++) {
        _stepper[i].disableOutputs();
        _stepper[i] = Stepper(
                constants::stepPinArray[i],
                constants::dirPinArray[i],
                constants::homePinArray[i]
                );
        _stepper[i].initialize();
    }

    // Initialize timer and set default speed
    speedInSteps = (unsigned int)(constants::speedDefault*constants::stepsPerMMDefault);  
    setSpeed(speedInSteps); 
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
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            _stepper[i].stop();
        } 
    }
}

void MotorDrive::start(unsigned int i) {
    if (i<constants::numAxis) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            _stepper[i].start();
        }
    }
}

void MotorDrive::stopAll() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        for (int i=0; i<constants::numAxis; i++) {
            _stepper[i].stop();
        }
    }
}

void MotorDrive::startAll() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        for (int i=0; i<constants::numAxis; i++) {
            _stepper[i].start();
        }
    }
}

bool MotorDrive::isPowerOn() {
    return _powerOnFlag;
}

bool MotorDrive::isEnabled() {
    return _enabledFlag;
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

void MotorDrive::setSpeed(unsigned int v) {
    long period = 1000000/v;
    Timer1.setPeriod(period);
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

Array<long, constants::numAxis> MotorDrive::getCurrentPositionAll() {
    Array<long, constants::numAxis> position;
    for (int i=0; i<constants::numAxis; i++) {
        position[i] = _stepper[i].getCurrentPosition();
    }
    return position;
}

long MotorDrive::getCurrentPosition(unsigned int i) {
    long rtnVal = 0;
    if (i < constants::numAxis) {
        rtnVal = _stepper[i].getCurrentPosition();
    }
    return rtnVal;
}

void MotorDrive::setTargetPosition(unsigned int i, long pos) {
    if (i < constants::numAxis) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            _stepper[i].setTargetPosition(pos);
        }
    }
}

void MotorDrive::setTargetPositionAll(Array<long,constants::numAxis> pos) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        for (int i=0; i<constants::numAxis; i++) {
            setTargetPosition(i,pos[i]);
        }
    }
}

void MotorDrive::update() {
    if (_enabledFlag && _powerOnFlag) {
        for (int i=0; i<constants::numAxis; i++) {
            _stepper[i].updateDirPin();
            _stepper[i].setStepPinHigh();
        }
        delayMicroseconds(1);
        for (int i=0; i<constants::numAxis; i++) {
            _stepper[i].setStepPinLow();
        }
    }
}

// ----------------------------------------------------------------------


