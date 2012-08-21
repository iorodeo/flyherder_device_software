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
        stepper[i].disableOutputs();
        stepper[i] = StepperMotor(
                constants::stepperMode,
                constants::stepPinArray[i],
                constants::dirPinArray[i],
                constants::homePinArray[i]
                );
        stepper[i].initialize();
    }
}

void MotorDrive::enable() {
    digitalWrite(_disablePin,LOW);
}

void MotorDrive::disable() {
    digitalWrite(_disablePin,HIGH);
    stop();
}

void MotorDrive::stop() {
    for (int i=0; i<constants::numAxis; i++) {
        stepper[i].stop();
    }
}

void MotorDrive::setPowerOn() {
    digitalWrite(_powerPin,HIGH);
}

void MotorDrive::setPowerOff() {
    digitalWrite(_powerPin,LOW);
    stop();
}

void MotorDrive::setMaxSpeed(unsigned int i, float v) {
    if (i < constants::numAxis) {
        stepper[i].setMaxSpeed(v);
    }
}

void MotorDrive::setMaxSpeedAll(float v) {
    for (int i=0; i<constants::numAxis; i++) {
        setMaxSpeed(i,v);
    }
}

void MotorDrive::setAcceleration(unsigned int i, float a) {
    if (i < constants::numAxis) {
        stepper[i].setAcceleration(a);
    }
}

void MotorDrive::setAccelerationAll(float a) {
    for (int i=0; i<constants::numAxis; i++) {
        setAcceleration(i,a);
    }
}

bool MotorDrive::isPowerOn() {
    if (digitalRead(_powerPin) == HIGH) {
        return true;
    }
    else {
        return false;
    }
}

bool MotorDrive::isEnabled() {
    if (digitalRead(_disablePin)==LOW) {
        return true;
    }
    else {
        return false;
    }
}

bool MotorDrive::isRunning() {
    bool flag = false;
    for (int i=0; i<constants::numAxis; i++) {
        if (stepper[i].isRunning()) {
            flag = true;
        }
    }
    return flag;
}

void MotorDrive::update() {
    if (isPowerOn() && isEnabled()) {
        for (int i=0; i<constants::numAxis; i++) {
            stepper[i].update();
        }
    }
}
