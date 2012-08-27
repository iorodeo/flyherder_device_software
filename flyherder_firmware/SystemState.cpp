#if defined(ARDUINO) && ARDUINO >= 100 
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <TimerOne.h>
#include "string.h"
#include "SystemState.h"

// Homing function interrupt table
void (*homeFcnTable[constants::numAxis])(void) = {
        X0HomeFcn, 
        Y0HomeFcn, 
        X1HomeFcn, 
        Y1HomeFcn
        };

SystemState::SystemState() {
    setErrMsg("");
    disableBoundsCheck();
}

void SystemState::initialize() {
    Timer1.initialize(1000); // Dummpy period
    Timer1.attachInterrupt(timerUpdate);
    motorDrive.initialize();
    setDrivePowerOff();
#ifdef  HAVE_ENABLE
    disable();
#endif
    setStepsPerMMToDefault();
    setMaxSeparationToDefault();
    setOrientationToDefault();
    setupHoming();
    Timer1.start();
    setLedStatusOn();
}

void SystemState::setupHoming() {
    char homeSearchDir;
    float axisSepMM;
    float homePosMM;
    float homeSearchDistMM;
    long homePosSteps;
    long homeSearchDistSteps;
    for (int i=0; i<constants::numAxis; i++) {
        axisSepMM = getMaxSeparation(i%constants::numDim);
        if (i < constants::numDim) {
            homePosMM = 0.0;
            homeSearchDir = '-';
        }
        else {
            homePosMM = axisSepMM;
            homeSearchDir = '+';
        }
        homeSearchDistMM = constants::homeSearchDistScaleFact*axisSepMM;
        homePosSteps = convertMMToSteps(homePosMM);
        homeSearchDistSteps = convertMMToSteps(homeSearchDistMM);

        motorDrive.setHomeSearchDir(i, homeSearchDir);
        motorDrive.setHomePosition(i, homePosSteps);
        motorDrive.setHomeSearchDist(i, homeSearchDistSteps);
    }
}

bool SystemState::enableBoundsCheck() {
    Array<float, constants::numAxis> posMM;
    posMM = getPosition();
    if (!checkPosBounds(posMM)) {return false;} 
    _boundsCheck = true;
    return true;
}

void SystemState::disableBoundsCheck() {
    _boundsCheck = false;
}

bool SystemState::isBoundsCheckEnabled() {
    return _boundsCheck;
}

bool SystemState::checkPosBounds(Array<float, constants::numAxis> posMM) {
    Array<long, constants::numAxis> posStep;
    posStep = convertMMToSteps(posMM);
    for (int i=0; i<constants::numAxis; i++) {
        if (posStep[i] < 0) { 
            setErrMsg("position is less than 0");
            return false;
        }
        if (posStep[i] > convertMMToSteps(_maxSeparation[i%constants::numDim])) {
            setErrMsg("position is greater than max separation");
            return false;

        }
    }
    for (int i=0; i<constants::numDim; i++) {
        if (posStep[i] > posStep[i+constants::numDim]) {
            setErrMsg("position results in collision between axes");
            return false;
        }
    }
    return true;
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

#ifdef HAVE_ENABLE
void SystemState::enable() {
    motorDrive.enable();
}

void SystemState::disable() {
    motorDrive.disable();
}

bool SystemState::isEnabled() {
    return motorDrive.isEnabled();
}
#endif

void SystemState::stop() {
    motorDrive.stopAll();
}

bool SystemState::isRunning() {
    return motorDrive.isRunning();
}


bool SystemState::moveToPosition(Array<float,constants::numAxis> posMM) {
    Array<long,constants::numAxis> posStep;
    posStep = convertMMToSteps(posMM);
    motorDrive.setTargetPositionAll(posStep);
    motorDrive.startAll();
    return true;
}

bool SystemState::moveAxisToPosition(int axis, float posMM) {
    long posStep = convertMMToSteps(posMM);
    if (!checkAxisArg(axis))  {return false;}
    motorDrive.setTargetPosition(axis,posStep);
    motorDrive.start(axis);
    return true;
}

bool SystemState::moveToHome() {
    for (int i=0; i<constants::numAxis; i++) {
        attachInterrupt(constants::homeInterruptArray[i], homeFcnTable[i], FALLING);
    }
    motorDrive.homeAll();
    return true;
}

bool SystemState::moveAxisToHome(int axis) {
    if (!checkAxisArg(axis)) {return false;}
    attachInterrupt(constants::homeInterruptArray[axis], homeFcnTable[axis], FALLING);
    motorDrive.home(axis);
    return true;
}

Array<float,constants::numAxis> SystemState::getPosition() {
    Array<long, constants::numAxis> posSteps;
    Array<float,constants::numAxis> posMM;
    posSteps = motorDrive.getCurrentPositionAll();
    posMM = convertStepsToMM(posSteps);
    return posMM;
}

float SystemState::getAxisPosition(int axis) {
    long posStep;
    float posMM;
    if (!checkAxisArg(axis)) {return 0.0;}
    posStep = motorDrive.getCurrentPosition(axis);
    posMM = convertStepsToMM(posStep);
    return posMM;
}

bool SystemState::setPosition(Array<float, constants::numAxis> posMM) {
    Array<long, constants::numAxis> posStep;
    if (_boundsCheck) {
        if (!checkPosBounds(posMM)) {return false;}
    }
    posStep = convertMMToSteps(posMM);
    motorDrive.setCurrentPositionAll(posStep);
    return true;
}

bool SystemState::setAxisPosition(int axis, float posMM) {
    long posStep;
    if (!checkAxisArg(axis)) {return false;}
    if (_boundsCheck) {
        Array<float, constants::numAxis> newPosMM;
        newPosMM = getPosition();
        newPosMM[axis] = posMM;
        if (!checkPosBounds(newPosMM)) {return false;}
    }
    posStep = convertMMToSteps(posMM);
    motorDrive.setCurrentPosition(axis,posStep);
    return true;
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

float SystemState::getMaxSeparation(unsigned int dim) {
    if (dim < constants::numDim) {
        return _maxSeparation[dim];
    }
    else {
        return 0.0;
    }
}

bool SystemState::setSpeed(float v) {
    if (v < constants::minSpeed) {
        setErrMsg("speed <  min allowed value");
        return false;
    }
    if (v > constants::maxSpeed) {
        setErrMsg("speed > max allowed value");
        return false;
    }
    unsigned int vSteps = (unsigned int) convertMMToSteps(v);
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

long SystemState::convertMMToSteps(float x) {
    return (long)(_stepsPerMM*x);
}

float SystemState::convertStepsToMM(long x) {
    return ((float)x)/_stepsPerMM;
}

Array<long, constants::numAxis>  SystemState::convertMMToSteps(
        Array<float, constants::numAxis> posMM
        ) 
{
    Array<long, constants::numAxis> posSteps;
    for (int i=0; i<constants::numAxis; i++) {
        posSteps[i] = convertMMToSteps(posMM[i]);
    }
    return posSteps;
}

Array<float, constants::numAxis> SystemState::convertStepsToMM(
        Array<long, constants::numAxis> posSteps
        )
{
    Array<float, constants::numAxis> posMM;
    for (int i=0; i<constants::numAxis; i++) {
        posMM[i] = convertStepsToMM(posSteps[i]);
    }
    return posMM;
}

SystemState systemState;


