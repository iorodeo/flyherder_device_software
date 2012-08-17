from __future__ import print_function
import os
import serial
import time
import json
import functools

class SerialDevice(serial.Serial):

    CMD_GET_DEV_INFO = 0
    CMD_GET_CMDS = 1
    CMD_GET_RSP_CODES = 2
    RESET_SLEEP_T = 2.0

    def __init__(self, *args, **kwargs):
        try:
            debug = kwargs.pop('debug')
        except KeyError:
            debug = False 
        super(SerialDevice,self).__init__(*args,**kwargs)
        time.sleep(SerialDevice.RESET_SLEEP_T)
        self.deviceInfoDict = None
        self.rspDict = None
        self.cmdDict = None
        self.debug = debug

    def debugPrint(self, *args):
        if self.debug:
            print(*args)

    def sendCmd(self,*args):
        cmdList = ['[', ','.join(map(str,args)), ']']
        cmd = ''.join(cmdList)
        self.debugPrint('cmd', cmd)
        self.write(cmd)
        rspStr = self.readline()
        self.debugPrint('rspStr', rspStr)
        try:
            rspDict = jsonStrToDict(rspStr)
        except Exception, e:
            errMsg = 'unable to parse device response {0}'.format(str(e))
            raise IOError, errMsg
        try:
            status = rspDict.pop('status')
        except KeyError:
            errMsg = 'device response does not contain status'
            raise IOError, errMsg
        try:
            rspCmdId  = rspDict.pop('cmdId')
        except KeyError:
            errMsg = 'device response does not contain command Id'
            raise IOError, errMsg
        if not rspCmdId == args[0]:
            raise IOError, 'device response cmdId does not match that sent'
        if self.rspDict is not None:
            if status == self.rspDict['rspError']:
                try:
                    devErrMsg = '(from device) {0}'.format(rspDict['errMsg'])
                except KeyError:
                    devErrMsg = "error message missing"
                errMsg = '{0}'.format(devErrMsg)
                raise IOError, errMsg
        return rspDict

    def getDeviceInfoDict(self):
        infoDict = self.sendCmd(SerialDevice.CMD_GET_DEV_INFO)
        checkDictForKey(infoDict,'ModelNumber',dname='infoDict')
        checkDictForKey(infoDict,'SerialNumber',dname='infoDict')
        return infoDict

    def getCmdDict(self):
        cmdDict = self.sendCmd(SerialDevice.CMD_GET_CMDS)
        return cmdDict
    
    def getRspDict(self):
        rspDict = self.sendCmd(SerialDevice.CMD_GET_RSP_CODES)
        checkDictForKey(rspDict,'rspSuccess',dname='rspDict')
        checkDictForKey(rspDict,'rspError',dname='rspDict')
        return rspDict

    def sendCmdByName(self,name,*args):
        cmdId = self.cmdDict[name]
        cmdArgs = [cmdId]
        cmdArgs.extend(args)
        rsp = self.sendCmd(*cmdArgs)
        return rsp

    def printCmdDict(self):
        print('\nCommand Dictonary')
        print('-------------------')
        for k,v in self.cmdDictInv.iteritems():
            print(v,k)
        print()

    def sendAllCmds(self):
        print('\nSend All Commands')
        print('-------------------')
        for cmdId, cmdName in sorted(self.cmdDictInv.iteritems()):
            print('cmd: {0}, {1}'.format(cmdName,cmdId))
            rsp = self.sendCmd(cmdId) 
            print('rsp: {0}'.format(rsp))
            print()
        print()

class FlyHerder(SerialDevice):

    DEVICE_MODEL_NUMBER = 1105
    BAUDRATE = 9600 

    def __init__(self,*args,**kwargs):
        kwargs.update({'baudrate': FlyHerder.BAUDRATE})
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

    def cmdFuncBase(self,cmdName,*args):
        if len(args) == 1 and type(args[0]) is dict:
            argsDict = args[0]
            argsList = self.argsDictToList(argsDict)
        else:
            argsList = args
        rspDict = self.sendCmdByName(cmdName,*argsList)
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
                if v:
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
    

# -----------------------------------------------------------------------------

def findSerialDevices(modelNumber=None,serialNumber=None):
    devList = os.listdir('{0}dev'.format(os.path.sep))
    devList = [x for x in devList if 'ttyUSB' in x or 'ttyACM' in x]
    devList = ['{0}dev{0}{1}'.format(os.path.sep,x) for x in devList]
    return devList

def findDevices(baudrate, modelNumber,serialNumberList=None):
    serialList = findSerialDevices()
    matchingList = []
    for port in serialList:
        dev = SerialDevice(port=port,baudrate=baudrate)
        try:
            infoDict = dev.getDeviceInfoDict()
        except Exception:
            continue
        try:
            infoModelNumber = infoDict['ModelNumber']
        except KeyError:
            continue
        if infoModelNumber == modelNumber:
            if serialNumberList is None:
                matchingList.append(port)
            else:
                try:
                    infoSerialNumber = infoDict['SerialNumber']
                except KeyError:
                    continue
                if infoSerialNumber in serialNumberList:
                    matchingList.append(port)
        dev.close()
    return matchingList 

findFlyHerderDevices = functools.partial(
        findDevices,
        FlyHerder.BAUDRATE,
        FlyHerder.DEVICE_MODEL_NUMBER
        )

def checkDictForKey(d,k,dname=''):
    if not k in d:
        if not dname:
            dname = 'dictionary'
        raise IOError, '{0} does not contain {1}'.format(dname,k)

def jsonStrToDict(jsonStr): 
    jsonDict =  json.loads(jsonStr,object_hook=jsonDecodeDict) 
    return jsonDict

def jsonDecodeDict(data):
    """
    Object hook for decoding dictionaries from serialized json data. Ensures that
    all strings are unpacked as str objects rather than unicode.
    """
    rv = {}
    for key, value in data.iteritems():
        if isinstance(key, unicode):
           key = key.encode('utf-8')
        if isinstance(value, unicode):
           value = value.encode('utf-8')
        elif isinstance(value, list):
           value = jsonDecodeList(value)
        elif isinstance(value, dict):
           value = jsonDecodeDict(value)
        rv[key] = value
    return rv

def jsonDecodeList(data):
    """
    Object hook for decoding lists from serialized json data. Ensures that
    all strings are unpacked as str objects rather than unicode.
    """
    rv = []
    for item in data:
        if isinstance(item, unicode):
            item = item.encode('utf-8')
        elif isinstance(item, list):
            item = jsonDecodeList(item)
        elif isinstance(item, dict):
            item = jsonDecodeDict(item)
        rv.append(item)
    return rv


# -----------------------------------------------------------------------------
if __name__ == '__main__':

    #flyHerderList = findFlyHerderDevices(serialNumberList=[1267])
    #dev = FlyHerder(port=flyHerderList[0],timeout=2.0)
    debug = False 
    dev = FlyHerder(port='/dev/ttyUSB0',timeout=2.0,debug=debug)
    dev.printCmdDict()

    posDict = {'x0':0.0, 'y0':0.0, 'x1':0.2,'y1':0.3}
    dev.moveToPosition(posDict)

    sepDict = {'x': 10.0, 'y': 20.0}
    dev.setMaxSeparation(sepDict)









