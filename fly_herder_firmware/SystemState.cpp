#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <util/atomic.h>
#include <TimerOne.h>
#include "SystemState.h"
#include "string.h"

SystemState::SystemState() {
    setErrMsg("");
    setStepsPerMMToDefault();
    setMaxSeparationToDefault();
    setOrientationToDefault();
    // DEVELOPMENT
    timerCount=0;
}

void SystemState::initialize() {
    // Turn on status LED
    pinMode(constants::ledStatusPin, OUTPUT);
    setLEDStatusPinOn();

    // Initialize motor drive
    motorDrive.initialize();
    setDrivePowerOff();
    disable();

    // Setup system state update timer
    Timer1.initialize(constants::timerPeriod_us);
    Timer1.attachInterrupt(timerUpdate);
    Timer1.start();

    // Turn on drive power and enable.
    setDrivePowerOn();
    enable();
}

void SystemState::enable() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        _enabled = true;
    }
}

void SystemState::setDrivePowerOn() {
    // NOT DONE
}

void SystemState::setDrivePowerOff() {
    //NOT DONE
}

bool SystemState::isDrivePowerOn() {
    // NOT DONE
    return true;
}

void SystemState::disable() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        _enabled = false;
    }
}

bool SystemState::isEnabled() {
    bool enabled;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        enabled = _enabled;
    }
    return enabled;
}

void SystemState::start() {
    // NOT DONE
}

void SystemState::stop() {
    // NOT DONE
}

bool SystemState::isRunning() {
    return true;
}

void SystemState::update() {
    if (_enabled) {
        // NOT DONE
    }
    timerCount++;
}

bool SystemState::moveToPosition(Array<float,constants::numAxis> pos) {
    /// NOT DONE
    return true;
}

bool SystemState::moveAxisToPosition(int axis, float pos) {
    // NOT DONE
    if (!checkAxisArg(axis))  {return false;}
    return true;
}


bool SystemState::moveToHome() {
    // NOT DONE
    return true;
}

Array<float,constants::numAxis> SystemState::getPosition() {
    // NOT DONE
    Array<float,4> pos;
    for (int i=0; i<constants::numAxis; i++) {
        pos[i] = (float)(i+1);
    }
    return pos;
}

float SystemState::getAxisPosition(int axis) {
    // NOT DONE
    if (!checkAxisArg(axis)) {return 0.0;};
    return 0.0;
}

void SystemState::setMaxSeparationToDefault() { 
    for (int i=0; i<constants::numDim; i++) {
        _maxSeparation[i] = constants::maxSeparationDefault;
    }
}

bool SystemState::setMaxSeparation(Array<float,constants::numDim> maxSeparation) {
    for (int i=0; i<constants::numDim; i++) {
        if (maxSeparation[i] <= 0) {
            setErrMsg("separation value <= 0");
            return false;
        }
    }
    for (int i=0; i<constants::numDim; i++) {
        _maxSeparation[i] = maxSeparation[i];
    }
    return true;
}

Array<float,constants::numDim> SystemState::getMaxSeparation() {
    return _maxSeparation;
}

bool SystemState::setMaxSpeed(float v) {
    // NOT DONE
    return true;
}

float SystemState::getMaxSpeed() {
    // NOT DONE
    return 0.0;
}

bool SystemState::setAcceleration(float a) {
    // NOT DONE
    return true;
}

float SystemState::getAcceleration() {
    // NOT DONE
    return 0.0;
}

bool SystemState::isInHomePosition() {
    // NOT DONE
    return false;
}


bool SystemState::setOrientation(Array<char,constants::numAxis> orientation) {
    // NOT DONE
    for (int i=0; i<constants::numAxis; i++) {
        if (!setAxisOrientation(i,orientation[i])) {
            return false;
        }
    }
}

bool SystemState::setAxisOrientation(int axis, char orientation) {
    // NOT DONE
    bool test = false;
    if (!checkAxisArg(axis)) {return false;}
    for (int i=0; i<constants::numOrientation; i++) {
        if (orientation == constants::allowedOrientation[i]) {
            test = true;
        }
    }
    if (!test) {
        setErrMsg("uknown value for orientation");
        return false;
    }
    _orientation[axis] = orientation;
    return true;
}

void SystemState::setOrientationToDefault() {
    // NOT DONE
    Array<char,constants::numAxis> orientation = 
        Array<char,constants::numAxis>(constants::orientationDefault);
    setOrientation(orientation);
}

Array<char,constants::numAxis> SystemState::getOrientation() {
    return _orientation;
}

char SystemState::getAxisOrientation(int axis) {
    if (!checkAxisArg(axis)) {return 'x';}
    return _orientation[axis];
}

void SystemState::setStepsPerMMToDefault() { 
    _stepsPerMM = constants::stepsPerMMDefault;
}

bool SystemState::setStepsPerMM(float stepsPerMM) {
    if (stepsPerMM <= 0) {
        setErrMsg("stepsPerMM <= 0");
        return false;
    }
    _stepsPerMM = stepsPerMM;
    return true;
}

float SystemState::getStepsPerMM() {
    return _stepsPerMM;
}

void SystemState::setLEDStatusPinOn() {
    digitalWrite(constants::ledStatusPin,HIGH);
}

void SystemState::setLEDStatusPinOff() {
    digitalWrite(constants::ledStatusPin,LOW);
}

void SystemState::setErrMsg(char *msg) {
    strncpy(errMsg,msg,SYS_ERR_BUF_SZ);
}

bool SystemState::checkAxisArg(int axis) {
    if ((axis<0) || (axis >= constants::numAxis)) {
        setErrMsg("axis argument out of range");
        return false;
    }
    else {
        return true;
    }
}


// ----------------------------------------------------------------------

void timerUpdate() {
    systemState.update();
}

SystemState systemState;
