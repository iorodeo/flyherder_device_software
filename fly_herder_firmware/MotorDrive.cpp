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
}

void MotorDrive::setPowerOn() {
    digitalWrite(_powerPin,HIGH);
}

void MotorDrive::setPowerOff() {
    digitalWrite(_powerPin,LOW);
}

void MotorDrive::update() {
    for (int i=0; i<constants::numAxis; i++) {
        stepper[i].update();
    }
}
