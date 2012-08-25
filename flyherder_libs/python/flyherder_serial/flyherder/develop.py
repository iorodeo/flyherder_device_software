from __future__ import print_function
import time
from flyherder import FlyHerder

debug = False
dev = FlyHerder(port='/dev/ttyUSB0',timeout=2.0,debug=debug)

if 0:
    print(dev.isDrivePowerOn())
    dev.setDrivePowerOn()
    print(dev.isDrivePowerOn())
    dev.setDrivePowerOff()
    print(dev.isDrivePowerOn())

if 0:
    pos = dev.getPosition()
    print(pos)

    axisNames = dev.getAxisNames()
    for name in axisNames:
        pos = dev.getAxisPosition(name)
        print(name, pos)

if 0:
    print(dev.getPosition())
    dev.setSpeed(100.0)
    dev.setOrientation('+','+','+','+')
    #dev.setOrientation('-','-','-','-')
    print(dev.getOrientation())
    dev.setDrivePowerOn()
    time.sleep(1.0) # Required
    dev.enable()

    #pos_list = [10,]
    pos_list = [50,0]
    for pos in pos_list:
        print('move to pos', pos)
        #dev.moveToPosition(pos,pos,pos,pos)
        #dev.setAxisOrientation('x0','-')
        dev.moveAxisToPosition('x0',pos)
        while dev.isRunning(): 
            time.sleep(0.2)
        time.sleep(5.0)
    print(dev.getPosition())
    dev.disable()
    dev.setDrivePowerOff()

if 0:
    dev.setOrientation('+','+','+','+')
    #dev.setOrientation('-','-','-','-')
    dev.setSpeed(100.0)
    dev.setDrivePowerOn()
    time.sleep(1.0) # Required
    dev.enable()

    dev.moveAxisToHome('x0')
    while dev.isRunning():
        time.sleep(0.5)
        print('sleeping')
    dev.disable()
    dev.setDrivePowerOff()

if 1:
    print(dev.getPosition())
    dev.setSpeed(100.0)
    dev.setOrientation('+','+','+','+')
    dev.setDrivePowerOn()
    time.sleep(1.0) # Required
    dev.enable()
    pos_list = [50,0]
    for pos in pos_list:
        dev.moveAxisToPosition('x0',pos)
        while dev.isRunning():
            time.sleep(0.5)
        print(dev.getPosition())
        time.sleep(5.0)
    dev.disable()
    dev.setDrivePowerOff()
    print(dev.getPosition())




    
