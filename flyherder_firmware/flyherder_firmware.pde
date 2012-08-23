#include <Streaming.h>
#include <TimerOne.h>
#include "AccelStepper.h"
#include "StepperMotor.h"
#include "MotorDrive.h"
#include "SerialReceiver.h"
#include "DictPrinter.h"
#include "Array.h"
#include "MessageHandler.h"
#include "SystemState.h"
#include "constants.h"

MotorDrive motorDrive;

void setup() {
    Serial.begin(constants::baudrate);
    systemState.initialize();

    //Array<float, constants::numAxis> pos;
    //for (int i=0; i<constants::numAxis; i++) {
    //    pos[i] = 10000.0;
    //}
    //systemState.setDrivePowerOn();
    //systemState.enable();
    //systemState.moveToPosition(pos);
    //systemState.moveAxisToPosition(0, 10000.0);
}

void loop() { 
    //unsigned long t0;
    //unsigned long t1;
    //t0 = micros();
    //systemState.update();
    //t1 = micros();
    //Serial << t1-t0 << endl;
    //Serial << systemState.motorDrive._stepper[0]._speed << endl;
    //Serial << systemState.motorDrive._stepper[0]._stepInterval << endl;
    //Serial << systemState.motorDrive._stepper[0]._currentPos << endl;
    //Serial << endl;
    
    //systemState.computeNewMotorSpeeds();
    messageHandler.processMsg(); 
}

