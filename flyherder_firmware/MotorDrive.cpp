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
    _faultPin = constants::driveFaultPin;
    _powerOnFlag = false;
#ifdef HAVE_ENABLE
    _enabledFlag = false;
    _disablePin = constants::driveDisablePin;
#else
    _enabledFlag = true;
#endif

}

MotorDrive::MotorDrive(int powerPin, int disablePin, int faultPin) {
    _powerPin = powerPin;
#ifdef HAVE_ENABLE 
    _disablePin = disablePin;
#endif
    _faultPin = faultPin;
}

void MotorDrive::initialize() {
    unsigned int speedInSteps;
    // Set pin modes for drive control pins
    pinMode(_powerPin, OUTPUT);
    pinMode(_faultPin, INPUT);
    setPowerOff();
#ifdef HAVE_ENABLE
    pinMode(_disablePin, OUTPUT); 
    disable();
#endif

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


#ifdef HAVE_ENABLE
void MotorDrive::enable() {
    digitalWrite(_disablePin,LOW);
    _enabledFlag = true;
}

void MotorDrive::disable() {
    digitalWrite(_disablePin,HIGH);
    _enabledFlag = false;
    stopAll();
}

bool MotorDrive::isEnabled() {
    return _enabledFlag;
}
#endif

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

void MotorDrive::home(unsigned int i) {
    if (i < constants::numAxis) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { 
            _stepper[i].home(); 
        }
    }
}

void MotorDrive::homeAll() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        for (int i=0; i<constants::numAxis; i++) {
            _stepper[i].home();
        }
    }
}

bool MotorDrive::isPowerOn() {
    return _powerOnFlag;
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
#ifdef HAVE_ENABLE
    disable();
#endif
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

void MotorDrive::setCurrentPosition(unsigned int i, long pos) {
    if (i < constants::numAxis) {
        _stepper[i].setCurrentPosition(pos);
    }
}

void MotorDrive::setCurrentPositionAll(Array<long, constants::numAxis> pos) {
    for (int i=0; i<constants::numAxis; i++) {
        setCurrentPosition(i,pos[i]);
    }
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
            _stepper[i].setTargetPosition(pos[i]);
        }
    }
}

void MotorDrive::setHomePosition(unsigned int i, long pos) {
    if (i < constants::numAxis) {
        _stepper[i].setHomePosition(pos);
    }
}

void MotorDrive::setHomePositionAll(Array<long, constants::numAxis> pos) {
    for (int i=0; i<constants::numAxis; i++) {
        setHomePosition(i,pos[i]);
    }
}
    
long MotorDrive::getHomePosition(unsigned int i) {
    if (i < constants::numAxis) {
        return _stepper[i].getHomePosition();
    }
    else {
        return 0;
    }
}

Array<long, constants::numAxis> MotorDrive::getHomePositionAll() {
    Array<long, constants::numAxis> pos;
    for (int i=0; i<constants::numAxis; i++) {
        pos[i] = getHomePosition(i);
    }
    return pos;
}

bool MotorDrive::isHome(unsigned int i) {
    if (i<constants::numAxis) {
        return _stepper[i].isHome();
    }
    else {
        return false;
    }
}

bool MotorDrive::isHomeAll() {
    bool homeFlag = true;
    for (int i=0; i<constants::numAxis; i++) {
        homeFlag &= isHome(i);
    }
    return homeFlag;
}

void MotorDrive::setHomeSearchDir(unsigned int i, char dir) {
    if (i < constants::numAxis)  {
        _stepper[i].setHomeSearchDir(dir);
    }
}

void MotorDrive::setHomeSearchDirAll(Array<char, constants::numAxis> dir) {
    for (int i=0; i<constants::numAxis; i++) {
        setHomeSearchDir(i,dir[i]);
    }
}

char MotorDrive::getHomeSearchDir(unsigned int i) {
    if (i<constants::numAxis) {
        return _stepper[i].getHomeSearchDir();
    }
    else {
        return ' ';
    }
}

Array<char, constants::numAxis> MotorDrive::getHomeSearchDir() {
    Array<char, constants::numAxis> dir;
    for (int i=0; i<constants::numAxis; i++) {
        dir[i] = getHomeSearchDir(i); 
    }
}

void MotorDrive::setHomeSearchDist(unsigned int i, long dist) {
    if (i < constants::numAxis) {
        _stepper[i].setHomeSearchDist(dist);
    }
}

void MotorDrive::setHomeSearchDistAll(Array<long, constants::numAxis> dist) {
    for (int i=0; i<constants::numAxis; i++) {
        setHomeSearchDist(i,dist[i]);
    }
}

long MotorDrive::getHomeSearchDist(unsigned int i) {
    if (i < constants::numAxis) {
        return _stepper[i].getHomeSearchDist();
    }
    else {
        return 0;
    }
}

Array<long, constants::numAxis> MotorDrive::getHomeSearchDist() {
    Array<long, constants::numAxis> dist;
    for (int i=0; i<constants::numAxis; i++) {
        dist[i] = getHomeSearchDist(i);
    }
}

void MotorDrive::homeAction(unsigned int i) {
    if (_enabledFlag && _powerOnFlag) {
        if (i < constants::numAxis) {
            _stepper[i].homeAction();
        }
    }
}


