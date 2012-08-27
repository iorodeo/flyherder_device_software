#include <util/atomic.h>
#include "MessageHandler.h"
#include "Streaming.h"
#include "SystemState.h"
#include "Array.h"

enum {
    cmdGetDevInfo,             // Done
    cmdGetCmds,                // Done
    cmdGetRspCodes,            // Done

    cmdGetNumAxis,             // Done
    cmdGetNumDim,              // Done
    cmdGetAxisNames,           // Done  
    cmdGetDimNames,            // Done
    cmdGetAxisOrder,           // Done
    cmdGetDimOrder,            // Done
    cmdGetAllowedOrientation,  // Done

    cmdSetDrivePowerOn,        // Done  
    cmdSetDrivePowerOff,       // Done 
    cmdIsDrivePowerOn,         // Done
    
    cmdStop,                   // Done  
    cmdIsRunning,              // Done  

#ifdef HAVE_ENABLE
    cmdEnable,                 // Done 
    cmdDisable,                // Done  
    cmdIsEnabled,              // Done 
#endif

    cmdMoveToPosition,         // Done  
    cmdMoveAxisToPosition,     // Done 
    cmdMoveToHome,             // Done  
    cmdMoveAxisToHome,         // 
    cmdIsInHomePosition,       // Done 

    cmdGetPosition,            // Done 
    cmdGetAxisPosition,        // Done 
    cmdSetPosition,            //
    cmdSetAxisPosition,        //

    cmdSetMaxSeparation,       // Done
    cmdGetMaxSeparation,       // Done
    
    cmdSetSpeed,               // Done 
    cmdGetSpeed,               // Done 

    cmdSetOrientation,         // Done 
    cmdGetOrientation,         // Done 

    cmdSetAxisOrientation,     // Done 
    cmdGetAxisOrientation,     // Done

    cmdSetStepsPerMM,          // Done
    cmdGetStepsPerMM,          // Done

    cmdEnableBoundsCheck,      // Done
    cmdDisableBoundsCheck,     // Done
    cmdIsBoundsCheckEnabled,   // Done

    cmdSetSerialNumber,        //
    cmdGetSerialNumber,        // Done 

    cmdGetModelNumber,         // Done 

    // DEVELOPMENT
    cmdDebug, 
};


const int rspSuccess = 1;
const int rspError = 0;

void MessageHandler::processMsg() {
    while (Serial.available() > 0) {
        process(Serial.read());
        if (messageReady()) {
            msgSwitchYard();
            reset();
        }   
    }   
    return;
}

void MessageHandler::msgSwitchYard() {
    int cmd = readInt(0); 
    dprint.start();
    dprint.addIntItem("cmdId", cmd);

    switch (cmd) { 

        case cmdGetDevInfo:
            handleGetDevInfo();
            break;

        case cmdGetCmds:
            handleGetCmds();
            break;

        case cmdGetRspCodes:
            handleGetRspCodes();
            break;

        case cmdGetNumAxis:
            handleGetNumAxis();
            break;

        case cmdGetNumDim:
            handleGetNumDim();
            break;

        case cmdGetAxisNames:
            handleGetAxisNames();
            break;

        case cmdGetDimNames:
            handleGetDimNames();
            break;

        case cmdGetAxisOrder:
            handleGetAxisOrder();
            break;

        case cmdGetDimOrder:
            handleGetDimOrder();
            break;

        case cmdGetAllowedOrientation:
            handleGetAllowedOrientation();
            break;

        case cmdSetDrivePowerOn:
            handleSetDrivePowerOn();
            break;

        case cmdSetDrivePowerOff:
            handleSetDrivePowerOff();
            break;

        case cmdIsDrivePowerOn:
            handleIsDrivePowerOn();
            break;

        case cmdStop: 
            handleStop();
            break;

        case cmdIsRunning:
            handleIsRunning();
            break;

#ifdef HAVE_ENABLE
        case cmdEnable:
            handleEnable();
            break;

        case cmdDisable:
            handleDisable();
            break;

        case cmdIsEnabled:
            handleIsEnabled();
            break;
#endif

        case cmdMoveToPosition:
            handleMoveToPosition();
            break;

        case cmdMoveAxisToPosition:
            handleMoveAxisToPosition();
            break;

        case cmdMoveToHome:
            handleMoveToHome();
            break;

        case cmdMoveAxisToHome:
            handleMoveAxisToHome();
            break;

        case cmdGetPosition:
            handleGetPosition();
            break;

        case cmdGetAxisPosition:
            handleGetAxisPosition();
            break;

        case cmdSetPosition:
            handleSetPosition();
            break;

        case cmdSetAxisPosition:
            handleSetAxisPosition();
            break;

        case cmdSetMaxSeparation:
            handleSetMaxSeparation();
            break;

        case cmdGetMaxSeparation:
            handleGetMaxSeparation();
            break;

        case cmdSetSpeed:
            handleSetSpeed();
            break;

        case cmdGetSpeed:
            handleGetSpeed();
            break;

        case cmdIsInHomePosition:
            handleIsInHomePosition();
            break;

        case cmdSetOrientation:
            handleSetOrientation();
            break;

        case cmdGetOrientation:
            handleGetOrientation();
            break;

        case cmdSetAxisOrientation:
            handleSetAxisOrientation();
            break;

        case cmdGetAxisOrientation:
            handleGetAxisOrientation();
            break;

        case cmdSetStepsPerMM:
            handleSetStepsPerMM();
            break;

        case cmdGetStepsPerMM:
            handleGetStepsPerMM();
            break;

        case cmdEnableBoundsCheck: 
            handleEnableBoundsCheck();
            break;

        case cmdDisableBoundsCheck: 
            handleDisableBoundsCheck();
            break;

        case cmdIsBoundsCheckEnabled: 
            handleIsBoundsCheckEnabled();
            break;

        case cmdSetSerialNumber:
            handleSetSerialNumber();
            break;

        case cmdGetSerialNumber:
            handleGetSerialNumber();
            break;

        case cmdGetModelNumber:
            handleGetModelNumber();
            break;

        // DEVELOPMENT
        case cmdDebug:
            handleDebug();
            break;

        default:
            dprint.addIntItem("status", rspError);
            dprint.addStrItem("errMsg", "unknown command");
           break;
    }              
    dprint.stop();
}

bool MessageHandler::checkNumberOfArgs(int num) {
    bool flag = true;
    if (numberOfItems() != num) {
        dprint.addIntItem("status", rspError);
        dprint.addStrItem("errMsg", "incorrect number of arguments");
        flag = false;
    }
    return flag;
}

bool MessageHandler::checkAxisArg(int axis) {
    bool flag = true;
    if ((axis<0) || (axis>constants::numAxis)) {
        dprint.addIntItem("status", rspError);
        dprint.addStrItem("errMsg", "axis argument out of range");
        flag = false;
    }
    return flag;
}

bool MessageHandler::getAxisNumberFromName(char *name, int &number) {
    bool found = false;
    for (int i=0; i<constants::numAxis; i++) {
        if (strcmp(name, constants::axisNames[i]) == 0) {
            number = i;
            return true;
        }
    } 
    dprint.addIntItem("status", rspError);
    dprint.addStrItem("errMsg", "axis name not found");
    return false;
}

void MessageHandler::systemCmdRsp(bool flag) {
    if (flag) {
        dprint.addIntItem("status", rspSuccess);
    }
    else {
        dprint.addIntItem("status", rspError);
        dprint.addStrItem("errMsg", systemState.errMsg); 
    }
} 

void MessageHandler::handleGetDevInfo() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("ModelNumber",  constants::deviceModelNumber);
    dprint.addIntItem("SerialNumber", constants::deviceSerialNumber); 
}

void MessageHandler::handleGetCmds() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("getDevInfo", cmdGetDevInfo);
    dprint.addIntItem("getCmds", cmdGetCmds);        
    dprint.addIntItem("getRspCodes", cmdGetRspCodes);
    dprint.addIntItem("getNumAxis", cmdGetNumAxis);
    dprint.addIntItem("getNumDim", cmdGetNumDim);
    dprint.addIntItem("getAxisNames", cmdGetAxisNames);
    dprint.addIntItem("getDimNames", cmdGetDimNames);
    dprint.addIntItem("getAxisOrder", cmdGetAxisOrder);
    dprint.addIntItem("getDimOrder", cmdGetDimOrder);
    dprint.addIntItem("getAllowedOrientation", cmdGetAllowedOrientation);
    dprint.addIntItem("setDrivePowerOn", cmdSetDrivePowerOn);
    dprint.addIntItem("setDrivePowerOff", cmdSetDrivePowerOff);
    dprint.addIntItem("isDrivePowerOn", cmdIsDrivePowerOn);
    dprint.addIntItem("stop", cmdStop);             
    dprint.addIntItem("isRunning", cmdIsRunning);        
#ifdef HAVE_ENABLE
    dprint.addIntItem("enable", cmdEnable);          
    dprint.addIntItem("disable", cmdDisable);          
    dprint.addIntItem("isEnabled", cmdIsEnabled);        
#endif
    dprint.addIntItem("moveToPosition", cmdMoveToPosition);        
    dprint.addIntItem("moveAxisToPosition", cmdMoveAxisToPosition);
    dprint.addIntItem("moveToHome", cmdMoveToHome);     
    dprint.addIntItem("moveAxisToHome", cmdMoveAxisToHome);
    dprint.addIntItem("isInHomePosition", cmdIsInHomePosition);   
    dprint.addIntItem("setMaxSeparation", cmdSetMaxSeparation);
    dprint.addIntItem("getMaxSeparation", cmdGetMaxSeparation);
    dprint.addIntItem("getPosition", cmdGetPosition);
    dprint.addIntItem("getAxisPosition", cmdGetAxisPosition);
    dprint.addIntItem("setPosition", cmdSetPosition);
    dprint.addIntItem("setAxisPosition", cmdSetAxisPosition);
    dprint.addIntItem("setSpeed", cmdSetSpeed);      
    dprint.addIntItem("getSpeed", cmdGetSpeed);      
    dprint.addIntItem("setOrientation", cmdSetOrientation);
    dprint.addIntItem("getOrientation", cmdGetOrientation);
    dprint.addIntItem("setAxisOrientation", cmdSetAxisOrientation);
    dprint.addIntItem("getAxisOrientation", cmdGetAxisOrientation);
    dprint.addIntItem("setStepsPerMM", cmdSetStepsPerMM);
    dprint.addIntItem("getStepsPerMM", cmdGetStepsPerMM);
    dprint.addIntItem("enableBoundsCheck", cmdEnableBoundsCheck);
    dprint.addIntItem("disableBoundsCheck", cmdDisableBoundsCheck);
    dprint.addIntItem("isBoundsCheckEnabled", cmdIsBoundsCheckEnabled);
    dprint.addIntItem("setSerialNumber", cmdSetSerialNumber);
    dprint.addIntItem("getSerialNumber", cmdGetSerialNumber);
    dprint.addIntItem("getModelNumber", cmdGetModelNumber);
    // DEVELOPMENT
    dprint.addIntItem("cmdDebug", cmdDebug);
} 

void MessageHandler::handleGetRspCodes() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("rspSuccess",rspSuccess);
    dprint.addIntItem("rspError", rspError);
}

void MessageHandler::handleGetNumAxis() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("numAxis", constants::numAxis);
}

void MessageHandler::handleGetNumDim() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("numDim", constants::numDim);
}

void MessageHandler::handleGetAxisNames() {
    dprint.addIntItem("status", rspSuccess);
    for (int i=0; i<constants::numAxis; i++) {
        dprint.addEmptyItem((char*) constants::axisNames[i]);
    }
}

void MessageHandler::handleGetDimNames() {
    dprint.addIntItem("status", rspSuccess);
    for (int i=0; i<constants::numDim; i++) {
        dprint.addEmptyItem((char*) constants::dimNames[i]);
    }
}

void MessageHandler::handleGetAxisOrder() {
    dprint.addIntItem("status", rspSuccess);
    for (int i=0; i<constants::numAxis; i++) {
        dprint.addIntItem((char*) constants::axisNames[i], i);
    }
}

void MessageHandler::handleGetDimOrder() {
    dprint.addIntItem("status", rspSuccess);
    for (int i=0; i<constants::numDim; i++) {
        dprint.addIntItem((char*) constants::dimNames[i], i);
    }
}

void MessageHandler::handleGetAllowedOrientation() {
    char orientationStr[2];
    dprint.addIntItem("status", rspSuccess);
    for (int i=0; i<constants::numOrientation; i++) {
        snprintf(orientationStr,2,"%c",constants::allowedOrientation[i]);
        dprint.addEmptyItem(orientationStr);
    }
}

void MessageHandler::handleSetDrivePowerOn() {
    dprint.addIntItem("status", rspSuccess);
    systemState.setDrivePowerOn();
}

void MessageHandler::handleSetDrivePowerOff() {
    dprint.addIntItem("status", rspSuccess);
    systemState.setDrivePowerOff();
}

void MessageHandler::handleIsDrivePowerOn() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("isDrivePowerOn", systemState.isDrivePowerOn());
}

void MessageHandler::handleStop() { 
    systemState.stop();
    dprint.addIntItem("status",rspSuccess);
}

void MessageHandler::handleIsRunning() {
    dprint.addIntItem("status",rspSuccess);
    dprint.addIntItem("isRunning", systemState.isRunning());
}


#ifdef HAVE_ENABLE
void MessageHandler::handleEnable() {
    systemState.enable();
    dprint.addIntItem("status",rspSuccess);
}

void MessageHandler::handleDisable() {
    systemState.disable();
    dprint.addIntItem("status",rspSuccess);
}

void MessageHandler::handleIsEnabled() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("isEnabled", systemState.isEnabled());
}
#endif

void MessageHandler::handleMoveToPosition() {
    Array<float,constants::numAxis> pos;
    if (!checkNumberOfArgs(constants::numAxis+1)) {return;}
    for (int i=0; i<constants::numAxis; i++) {
        pos[i] = readFloat(i+1);
    }
    systemCmdRsp(systemState.moveToPosition(pos));
}

void MessageHandler::handleMoveAxisToPosition() {
    char axisName[constants::nameSize];
    int axisNumber;
    float pos;
    if (!checkNumberOfArgs(3)) {return;}
    copyString(1,axisName,constants::nameSize);
    pos = readFloat(2);
    if (!getAxisNumberFromName(axisName,axisNumber)) {return;}
    systemCmdRsp(systemState.moveAxisToPosition(axisNumber,pos));
}

void MessageHandler::handleMoveToHome() {
    systemCmdRsp(systemState.moveToHome());
}

void MessageHandler::handleMoveAxisToHome() {
    char axisName[constants::nameSize];
    int axisNumber;
    if (!checkNumberOfArgs(2)) {return;}
    copyString(1,axisName,constants::nameSize);
    if (!getAxisNumberFromName(axisName,axisNumber)) {return;}
    systemCmdRsp(systemState.moveAxisToHome(axisNumber));
}

void MessageHandler::handleGetPosition() {
    Array<float,constants::numAxis> position;
    position = systemState.getPosition();
    dprint.addFltItem("status", rspSuccess);
    for (int i=0; i<constants::numAxis; i++) {
        dprint.addFltItem((char*)constants::axisNames[i],position[i]);
    }
}

void MessageHandler::handleGetAxisPosition() {
    char axisName[constants::nameSize];
    int axisNumber;
    float pos;
    if (!checkNumberOfArgs(2)) {return;}
    copyString(1,axisName,constants::nameSize);
    if (!getAxisNumberFromName(axisName,axisNumber)) {return;}
    pos = systemState.getAxisPosition(axisNumber);
    dprint.addIntItem("status", rspSuccess);
    dprint.addFltItem("position", pos);
}

void MessageHandler::handleSetPosition() {
    Array<float,constants::numAxis> pos;
    if (!checkNumberOfArgs(constants::numAxis+1)) {return;}
    for (int i=0; i<constants::numAxis; i++) {
        pos[i] = readFloat(i+1);
    }
    systemCmdRsp(systemState.setPosition(pos));
}

void MessageHandler::handleSetAxisPosition() {
    char axisName[constants::nameSize];
    int axisNumber;
    float pos;
    if (!checkNumberOfArgs(3)) {return;}
    copyString(1,axisName,constants::nameSize);
    pos = readFloat(2);
    if (!getAxisNumberFromName(axisName,axisNumber)) {return;}
    systemCmdRsp(systemState.setAxisPosition(axisNumber,pos));
}

void MessageHandler::handleSetMaxSeparation() {
    Array<float,constants::numDim> maxSeparation;
    if (!checkNumberOfArgs(constants::numDim+1)) {return;}
    for (int i=0; i<constants::numDim; i++) {
        maxSeparation[i] = readFloat(i+1);
    }
    systemCmdRsp(systemState.setMaxSeparation(maxSeparation));
}

void MessageHandler::handleGetMaxSeparation() {
    Array<float,constants::numDim> maxSeparation;
    maxSeparation = systemState.getMaxSeparation();
    dprint.addIntItem("status", rspSuccess);
    for (int i=0; i<constants::numDim; i++) {
        dprint.addFltItem((char *)constants::dimNames[i], maxSeparation[i]);
    }
}

void MessageHandler::handleSetSpeed() {
    if (!checkNumberOfArgs(2)) {return;}
    float maxSpeed = readFloat(1);
    systemCmdRsp(systemState.setSpeed(maxSpeed));
}

void MessageHandler::handleGetSpeed() {
    float maxSpeed = systemState.getSpeed();
    dprint.addIntItem("status", rspSuccess);
    dprint.addFltItem("maxSpeed", maxSpeed);
}

void MessageHandler::handleIsInHomePosition() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("isInHomePosition", systemState.isInHomePosition());
}

void MessageHandler::handleSetOrientation() {
    Array<char,constants::numAxis> orientation;
    if (!checkNumberOfArgs(constants::numAxis+1)) {return;}
    for (int i=0; i<constants::numAxis; i++) {
        orientation[i] = readChar(i+1,0);
    }
    systemCmdRsp(systemState.setOrientation(orientation));
}

void MessageHandler::handleGetOrientation() {
    Array<char,constants::numAxis> orientation = systemState.getOrientation();
    dprint.addIntItem("status", rspSuccess);
    for (int i=0; i<constants::numAxis; i++) {
        dprint.addCharItem((char*)constants::axisNames[i],orientation[i]);
    }
}

void MessageHandler::handleSetAxisOrientation() {
    char axisName[constants::nameSize];
    int axisNumber;
    char orientation;
    if (!checkNumberOfArgs(3)) {return;}
    copyString(1,axisName,constants::nameSize);
    orientation = readChar(2,0);
    if (!getAxisNumberFromName(axisName,axisNumber)) {return;}
    systemCmdRsp(systemState.setAxisOrientation(axisNumber,orientation));
    dprint.addIntItem("status", rspSuccess);
}

void MessageHandler::handleGetAxisOrientation() {
    char axisName[constants::nameSize];
    int axisNumber;
    char orientation;
    if (!checkNumberOfArgs(2)) {return;}
    copyString(1,axisName,constants::nameSize);
    if (!getAxisNumberFromName(axisName, axisNumber)) {return;}
    orientation = systemState.getAxisOrientation(axisNumber);
    dprint.addIntItem("status", rspSuccess);
    dprint.addCharItem("orientation",orientation);
}

void MessageHandler::handleSetStepsPerMM() {
    float stepsPerMM;
    if (!checkNumberOfArgs(2)) {return;}
    stepsPerMM = readFloat(1);
    systemCmdRsp(systemState.setStepsPerMM(stepsPerMM));
}

void MessageHandler::handleGetStepsPerMM() {
    float stepsPerMM = systemState.getStepsPerMM();
    dprint.addIntItem("status", rspSuccess);
    dprint.addFltItem("stepsPerMillimeter", stepsPerMM);
}

void MessageHandler::handleEnableBoundsCheck() {
    systemCmdRsp(systemState.enableBoundsCheck());
}

void MessageHandler::handleDisableBoundsCheck() {
    systemState.disableBoundsCheck();
    dprint.addIntItem("status", rspSuccess);
}

void MessageHandler::handleIsBoundsCheckEnabled() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("isBoundsCheckEnabled", systemState.isBoundsCheckEnabled());
}

void MessageHandler::handleSetSerialNumber() {
    // NOT DONE
    dprint.addIntItem("status", rspSuccess);
}

void MessageHandler::handleGetSerialNumber() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("serialNumber", (int) constants::deviceSerialNumber);
}

void MessageHandler::handleGetModelNumber() {
    dprint.addIntItem("status", rspSuccess);
    dprint.addIntItem("modelNumber", (int) constants::deviceModelNumber);
}

// -------------------------------------------------


void MessageHandler::handleDebug() {
    char name[20];
    dprint.addIntItem("status", rspSuccess);
}


MessageHandler messageHandler;
