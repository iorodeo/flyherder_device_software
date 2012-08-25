#include <Streaming.h>
#include <TimerOne.h>
#include "Stepper.h"
#include "MotorDrive.h"
#include "SerialReceiver.h"
#include "DictPrinter.h"
#include "Array.h"
#include "MessageHandler.h"
#include "SystemState.h"
#include "constants.h"

void setup() {
    Serial.begin(constants::baudrate);
    systemState.initialize();
}

void loop() { 
    messageHandler.processMsg(); 
}

