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

    //motorDrive.initialize();

    //for (int i=0; i<constants::numAxis; i++) {
    //    Serial << "stepper[" << i << "]" << endl;
    //    Serial << "  " << "_running " << motorDrive.stepper[i]._running << endl;
    //    Serial << "  " << "_dirInverted " << motorDrive.stepper[i]._dirInverted << endl;


    //}
}

void loop() { 
    messageHandler.processMsg(); 
}

