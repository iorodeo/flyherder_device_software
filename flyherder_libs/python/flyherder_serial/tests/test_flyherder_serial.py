from __future__ import print_function
from flyherder_serial import FlyHerder
from pprint import pprint

TEST_FLOAT_PREC = 1.0e-6

debug = False
dev = FlyHerder(port='/dev/ttyUSB0',timeout=2.0,debug=debug)

def test_getDevInfo():
    rsp = dev.getDevInfo()
    print('\ndev.getDevInfo() = ')
    pprint(rsp)

def test_getCmds():
    rsp = dev.getCmds()
    print('\ndev.getCmds() = ')
    pprint(rsp)

def test_getRspCodes():
    rsp = dev.getRspCodes()
    print('\ndev.gettRspCodes() = ')
    pprint(rsp)

def test_getNumAxis():
    rsp = dev.getNumAxis()
    assert rsp > 0
    assert rsp%2==0
    print('\ndev.getNumAxis() = {0}'.format(rsp))

def test_getNumDim():
    rsp = dev.getNumDim()
    assert rsp > 0
    print('\ndev.getNumDim() = {0}'.format(rsp))

def test_getAxisNames():
    numAxis = dev.getNumAxis()
    rsp = dev.getAxisNames()
    assert len(rsp) == numAxis
    print('\ndev.getAxisNames() = ')
    pprint(rsp)

def test_getDimNames():
    numDim = dev.getNumDim()
    rsp = dev.getDimNames()
    assert len(rsp) == numDim
    print('\ndev.getDimNames() = ')
    pprint(rsp)

def test_getAxisOrder():
    numAxis = dev.getNumAxis()
    rsp = dev.getAxisOrder()
    values = rsp.values()
    values.sort()
    assert len(rsp) == numAxis
    assert values == range(len(values))
    print('\ndev.getAxisOrder() = ')
    pprint(rsp)

def test_getDimOrder():
    rsp = dev.getDimOrder()
    values = rsp.values()
    values.sort()
    assert values == range(len(values))
    print('\ndev.getDimOrder() = ')
    pprint(rsp)

def test_getAllowedOrientation():
    numDim  = dev.getNumDim()
    rsp = dev.getAllowedOrientation()
    assert len(rsp) == numDim
    print(rsp)

def test_setDrivePowerOn():
    rsp = dev.setDrivePowerOn()

def test_setDrivePowerOff():
    rsp = dev.setDrivePowerOff()

def test_isDrivePowerOn():
    rsp = dev.isDrivePowerOn()
    assert rsp in (0,1)

def test_stop(): 
    dev.stop()

def test_isRunning():
    rsp = dev.isRunning()
    assert rsp in (0,1)
    print('\ndev.isRunning() = ')
    pprint(rsp)


if 'enable' in dev.cmdDict:
    def test_enable():
        dev.enable()

if 'disable' in dev.cmdDict:
    def test_disable():
        dev.disable()

if 'isEnabled' in dev.cmdDict:
    def test_isEnabled():
        rsp = dev.isEnabled()
        print('\ndev.isEnabled() = ')
        pprint(rsp)
    
if 'enable' in dev.cmdDict: 
    if 'disable' in dev.cmdDict:  
        if 'isEnabled' in dev.cmdDict:
            def test_enable_disable():
                dev.enable()
                assert dev.isEnabled()
                dev.disable()
                assert not dev.isEnabled()

def test_moveToPosition():
    posTuple = (1.0,2.0,3.0,4.0)
    dev.moveToPosition(*posTuple)
    posDict = {'x0':2.0, 'y0': 3.0, 'x1':4.0, 'y1':5.0}
    dev.moveToPosition(posDict)

def test_moveAxisToPosition():
    axisDict = dev.getAxisOrder()
    for axisName in axisDict:
        dev.moveAxisToPosition(axisName,1.0)

def test_moveToHome():
    dev.moveToHome()

def test_isInHomePosition():
    rsp = dev.isInHomePosition()
    assert rsp in (0,1)
    print('\ndev.isInHomePosition() = ')
    pprint(rsp)

def test_getPosition():
    posDict = dev.getPosition()
    axisDict = dev.getAxisOrder()
    for ax in axisDict:
        assert ax in posDict
        assert type(posDict[ax]) is float
    print('\ndev.getPosition() = ')
    pprint(posDict)

def test_getAxisPosition():
    axisDict = dev.getAxisOrder()
    print()
    for ax in axisDict:
        pos = dev.getAxisPosition(ax)
        assert type(pos) is float
        print('dev.getAxisPosition({0}) = {1}'.format(ax,pos))

def test_setPosition():
    stepsPerMM = dev.getStepsPerMM()
    posWrite = {
            'x0' : 10, 
            'y0' : 20,
            'x1' : 30, 
            'y1' : 40, 
            }
    dev.setPosition(posWrite)
    posRead = dev.getPosition()
    assert posWrite.keys() == posRead.keys()
    print()
    print('posWrite = ')
    pprint(posWrite)
    print('posRead = ')
    pprint(posRead)
    for name in posWrite:
        valWrite = posWrite[name]
        valRead = posRead[name]
        assert abs(valWrite - valRead) < 1.0/stepsPerMM 

def test_setAxisPosition():
    stepsPerMM = dev.getStepsPerMM()
    posWrite = {
            'x0' : 10, 
            'y0' : 20,
            'x1' : 30, 
            'y1' : 40, 
            }
    posRead = {}
    for name, value in posWrite.iteritems():
        dev.setAxisPosition(name,value)
        posRead[name] = dev.getAxisPosition(name)
    print()
    print('posWrite = ')
    pprint(posWrite)
    print('posRead = ')
    pprint(posRead)
    for name in posWrite:
        valWrite = posWrite[name]
        valRead = posRead[name]
        assert abs(valWrite - valRead) < 1.0/stepsPerMM 
    



def test_getMaxSeparation():
    sepDict = dev.getMaxSeparation()
    dimDict = dev.getDimOrder()
    for d in dimDict:
        assert d in sepDict
        assert type(sepDict[d]) is float
    print('\ndev.getMaxSeparation() = ')
    pprint(sepDict)

def test_setMaxSeparation():
    writeValues = {}
    dimNames = dev.getDimNames()
    for i, d in enumerate(dimNames):
        writeValues[d] = float(i+0.1)
    dev.setMaxSeparation(writeValues)
    readValues = dev.getMaxSeparation()
    assert writeValues == readValues

def test_setSpeed():
    # NOT DONE
    dev.setSpeed(20.0)

def test_getSpeed():
    # NOT DONE
    speedWrite = 15.0
    dev.setSpeed(speedWrite)
    speedRead = dev.getSpeed()
    deltaSpeed = abs((speedWrite - speedRead)/speedWrite)
    assert deltaSpeed < TEST_FLOAT_PREC
   
def test_getOrientation():
    allowedOrientation = dev.getAllowedOrientation()
    orientDict = dev.getOrientation()
    for val in orientDict.values():
        assert val in allowedOrientation
    print('\ndev.getOrientation() = ')
    pprint(orientDict)

def test_setOrientation():
    writeDict = {'x0':'+', 'y0':'-','x1':'+', 'y1': '-'} 
    dev.setOrientation(writeDict)
    readDict = dev.getOrientation()
    assert writeDict == readDict

def test_getAxisOrientation():
    axisNames = dev.getAxisNames()
    allowedOrientation = dev.getAllowedOrientation()
    for name in axisNames:
        orientation = dev.getAxisOrientation(name)
        assert orientation in allowedOrientation
        print('\ndev.getAxisOrientation({0}) = {1}'.format(name,orientation))

def test_setAxisOrientation():
    axisNameList = dev.getAxisNames()
    allowedOrientationList = dev.getAllowedOrientation()
    for axisName in axisNameList:
        for orientationWrite in allowedOrientationList:
            dev.setAxisOrientation(axisName,orientationWrite)
            orientationRead = dev.getAxisOrientation(axisName)
            assert orientationRead == orientationWrite
    
def test_getStepsPerMM():
    rsp = dev.getStepsPerMM()
    assert type(rsp) is float
    print('\ndev.stepsPerMeter() = {0}'.format(rsp))

def test_setStepsPerMM():
    stepsPerMMWrite = 3.12
    dev.setStepsPerMM(stepsPerMMWrite)
    stepsPerMMRead = dev.getStepsPerMM()
    delta = abs((stepsPerMMWrite -stepsPerMMRead)/stepsPerMMWrite)
    assert  delta < TEST_FLOAT_PREC

def test_enableBoundsCheck():
    dev.setMaxSeparation({'x': 100, 'y':100})
    dev.setPosition({ 'x0' : 10, 'y0' : 20, 'x1' : 30, 'y1' : 40, })
    dev.enableBoundsCheck()
    dev.disableBoundsCheck()

def test_disableBoundsCheck():
    dev.disableBoundsCheck()
    dev.setMaxSeparation({'x': 10, 'y':10})
    dev.setPosition({ 'x0' : 100, 'y0' : 100, 'x1' : 300, 'y1' : 300, })
    dev.setPosition({ 'x0' : 0, 'y0' : 0, 'x1' : 0, 'y1' : 0, })

def test_isBoundsCheckEnabled():
    dev.setMaxSeparation({'x': 100, 'y':100})
    dev.setPosition({ 'x0' : 10, 'y0' : 20, 'x1' : 30, 'y1' : 40, })
    dev.enableBoundsCheck()
    assert dev.isBoundsCheckEnabled 
    dev.disableBoundsCheck()
    assert not dev.isBoundsCheckEnabled()

def test_getSerialNumber():
    rsp = dev.getSerialNumber()
    print('\ndev.getSerialNumber = {0}'.format(rsp))

def test_setSerialNumber():
    # NOT DONE
    rsp = dev.setSerialNumber(12123)

def test_getModelNumber():
    rsp = dev.getModelNumber()
    print('\ndev.getModelNumber = {0}'.format(rsp))

def test_debug():
    rsp = dev.cmdDebug()
    print('\ndev.getDebug() = ')
    pprint(rsp)
   

