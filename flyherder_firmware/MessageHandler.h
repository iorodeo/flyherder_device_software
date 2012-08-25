#ifndef _MESSAGE_HANDER_H_
#define _MESSAGE_HANDER_H_
#include <SerialReceiver.h>
#include "DictPrinter.h"
#include "constants.h"

class MessageHandler : public SerialReceiver {

    public:
        void processMsg();

    private:
        DictPrinter dprint;
        void msgSwitchYard();
        bool checkNumberOfArgs(int num);
        bool checkAxisArg(int axis);
        bool getAxisNumberFromName(char *axisName, int &number);
        void systemCmdRsp(bool flag);

        void handleGetDevInfo();
        void handleGetCmds();
        void handleGetRspCodes();
        void handleGetAxisNames();
        void handleGetDimNames();
        void handleGetNumAxis();
        void handleGetNumDim();
        void handleGetAxisOrder();
        void handleGetDimOrder();
        void handleGetAllowedOrientation();

        void handleSetDrivePowerOn();
        void handleSetDrivePowerOff();
        void handleIsDrivePowerOn();

        void handleStop();
        void handleIsRunning();
        void handleEnable();
        void handleDisable();
        void handleIsEnabled();
        void handleMoveToPosition();
        void handleMoveAxisToPosition();
        void handleMoveToHome();
        void handleMoveAxisToHome();
        void handleGetPosition();
        void handleGetAxisPosition();
        void handleSetMaxSeparation();
        void handleGetMaxSeparation();
        void handleSetSpeed();
        void handleGetSpeed();
        void handleIsInHomePosition();
        void handleSetOrientation();
        void handleGetOrientation();
        void handleSetAxisOrientation();
        void handleGetAxisOrientation();
        void handleSetStepsPerMM();
        void handleGetStepsPerMM();
        void handleSetSerialNumber();
        void handleGetSerialNumber();
        void handleGetModelNumber();

        // Development
        void handleGetTimerCount();
        void handleDebug();
};

extern MessageHandler messageHandler;
#endif
