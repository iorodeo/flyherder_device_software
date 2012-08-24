#if defined(ARDUINO) && ARDUINO >= 100 
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <TimerOne.h>
#include "string.h"
#include "SystemState.h"

SystemState::SystemState() {
    setErrMsg("");
}

void SystemState::initialize() {
    Timer1.initialize(1000); // Dummpy period
    Timer1.attachInterrupt(timerUpdate);

    // Initialize motor drive
    motorDrive.initialize();
    setDrivePowerOff();
    disable();

    setStepsPerMMToDefault();
    setMaxSeparationToDefault();
    setOrientationToDefault();

    Timer1.start();
    setLedStatusOn();
}

void SystemState::setLedStatusOn() {
    pinMode(constants::ledStatusPin,OUTPUT);
    digitalWrite(constants::ledStatusPin,HIGH);
}

void SystemState::setLedStatusOff() {
    digitalWrite(constants::ledStatusPin,LOW);
}

void SystemState::setDrivePowerOn() {
    motorDrive.setPowerOn();
}

void SystemState::setDrivePowerOff() {
    motorDrive.setPowerOff();
}

bool SystemState::isDrivePowerOn() {
    return motorDrive.isPowerOn();
}

void SystemState::enable() {
    motorDrive.enable();
}

void SystemState::disable() {
    motorDrive.disable();
}

bool SystemState::isEnabled() {
    return motorDrive.isEnabled();
}

void SystemState::stop() {
    motorDrive.stopAll();
}

bool SystemState::isRunning() {
    return motorDrive.isRunning();
}


bool SystemState::moveToPosition(Array<float,constants::numAxis> posMM) {
    Array<long,constants::numAxis> posStep;
    for (int i=0; i<constants::numAxis; i++) {
        posStep[i] = (long)(_stepsPerMM*posMM[i]);
    }
    motorDrive.setTargetPositionAll(posStep);
    motorDrive.startAll();
    return true;
}

bool SystemState::moveAxisToPosition(int axis, float posMM) {
    long posStep = (long)(_stepsPerMM*posMM);
    if (!checkAxisArg(axis))  {return false;}
    motorDrive.setTargetPosition(axis,posStep);
    motorDrive.start(axis);
    return true;
}


bool SystemState::moveToHome() {
    // NOT DONE
    return true;
}

Array<float,constants::numAxis> SystemState::getPosition() {
    Array<long, constants::numAxis> posSteps;
    Array<float,constants::numAxis> posMM;
    posSteps = motorDrive.getCurrentPositionAll();
    for (int i=0; i<constants::numAxis; i++) {
        posMM[i] = ((float) posSteps[i])/_stepsPerMM;
    }
    return posMM;
}

float SystemState::getAxisPosition(int axis) {
    if (!checkAxisArg(axis)) {return 0.0;};
    return motorDrive.getCurrentPosition(axis);
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

bool SystemState::setSpeed(float v) {
    unsigned int vSteps = (unsigned int)(_stepsPerMM*v);
    motorDrive.setSpeed(vSteps);
    _speed = v;
    return true;
}

float SystemState::getSpeed() {
    return _speed;
}

bool SystemState::isInHomePosition() {
    // NOT DONE
    bool rtnVal = false;
    if (~isRunning()) {
    }
    return rtnVal;
}

bool SystemState::setOrientation(Array<char,constants::numAxis> orientation) {
    for (int i=0; i<constants::numAxis; i++) {
        if (!setAxisOrientation(i,orientation[i])) {
            return false;
        }
    }
}

bool SystemState::setAxisOrientation(int axis, char orientation) {
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
    motorDrive.setDirection((unsigned int) axis, orientation);
    _orientation[axis] = orientation;
    return true;
}

void SystemState::setOrientationToDefault() {
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

void timerUpdate() {
    systemState.motorDrive.update();
}

SystemState systemState;
