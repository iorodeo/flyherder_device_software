from __future__ import print_function
import time
import functools
from serial_device import SerialDevice, findDevices

class FlyHerder(SerialDevice):

    BAUDRATE = 9600 
    TIMEOUT = 8.0
    DEVICE_MODEL_NUMBER = 1105
    POWER_ON_SLEEP_T = 1.0
    WAIT_SLEEP_DT = 0.2

    def __init__(self,*args,**kwargs):
        kwargs.update({
            'baudrate': FlyHerder.BAUDRATE, 
            'timeout': FlyHerder.TIMEOUT,
            })
        super(FlyHerder,self).__init__(*args,**kwargs)
        self.deviceInfoDict = self.getDeviceInfoDict()
        self.cmdDict = self.getCmdDict()
        self.rspDict = self.getRspDict()
        self.cmdDictInv = dict([(v,k) for (k,v) in self.cmdDict.iteritems()])
        self.createCmds()
        self.dimNameSet = set(self.getDimNames())
        self.axisNameSet = set(self.getAxisNames())
        self.axisOrderDict = self.getAxisOrder()
        self.dimOrderDict = self.getDimOrder()

    def wait(self):
        while self.isRunning():
            time.sleep(FlyHerder.WAIT_SLEEP_DT)

    def cmdFuncBase(self,cmdName,*args):
        if len(args) == 1 and type(args[0]) is dict:
            argsDict = args[0]
            argsList = self.argsDictToList(argsDict)
        else:
            argsList = args
        rspDict = self.sendCmdByName(cmdName,*argsList)
        if cmdName == 'setDrivePowerOn':
            time.sleep(FlyHerder.POWER_ON_SLEEP_T)
        if rspDict:
            retValue = self.processRspDict(rspDict)
            return retValue

    def createCmds(self):
        self.cmdFuncDict = {}
        for cmdId, cmdName in sorted(self.cmdDictInv.items()):
            cmdFunc = functools.partial(self.cmdFuncBase, cmdName)
            setattr(self,cmdName,cmdFunc)
            self.cmdFuncDict[cmdName] = cmdFunc

    def printCommands(self):
        for k in self.cmdFuncDict:
            print(k)

    def processRspDict(self,rspDict):
        if len(rspDict) == 1:
            retValue = rspDict.values()[0]
        else:
            allValuesEmpty = True 
            for v in rspDict.values():
                if not type(v) == str or v:
                    allValuesEmpty = False
                    break
            if allValuesEmpty:
                retValue = sorted(rspDict.keys())
            else:
                retValue = rspDict
        return retValue

    def argsDictToList(self,argsDict): 
        keySet = set(argsDict.keys())
        if keySet == self.dimNameSet: 
            orderDict = self.dimOrderDict
        elif keySet == self.axisNameSet:
            orderDict = self.axisOrderDict
        else:
            raise ValueError, 'argument dictionary must have keyed by dimensions for axes'
        orderList = sorted([(num,name) for (name,num) in orderDict.iteritems()])
        argsList = [argsDict[name] for (num, name) in orderList] 
        return argsList

findFlyHerderDevices = functools.partial(
        findDevices,
        FlyHerder.BAUDRATE,
        FlyHerder.DEVICE_MODEL_NUMBER
        )
# -----------------------------------------------------------------------------------------
if __name__ == '__main__':

    debug = False
    dev = FlyHerder(port='/dev/ttyUSB0',timeout=2.0,debug=debug)
    dev.printCommands()
    


