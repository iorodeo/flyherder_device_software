#include <Streaming.h>
#include <TimerOne.h>
#include <AccelStepper.h>
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

    Array<float, constants::numAxis> pos;
    for (int i=0; i<constants::numAxis; i++) {
        pos[i] = 10000.0;
    }
    systemState.setDrivePowerOn();
    systemState.enable();
    systemState.moveToPosition(pos);
}

void loop() { 
    unsigned long t0;
    unsigned long t1;
    t0 = micros();
    systemState.update();
    t1 = micros();
    Serial << t1-t0 << endl;
    //messageHandler.processMsg(); 
}

