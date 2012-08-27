from __future__ import print_function
import time
from flyherder_serial import FlyHerder

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

if 1:
    print(dev.getPosition())
    dev.setSpeed(100.0)
    dev.setOrientation('+','+','+','+')
    #dev.setOrientation('-','-','-','-')
    print(dev.getOrientation())
    dev.setDrivePowerOn()
    time.sleep(1.0) # Required

    #pos_list = [10,]
    pos_list = [
            (30, 60, 90, 120), 
            ( 0,  0,  0,   0), 
            ]
    for pos0, pos1, pos2, pos3 in pos_list:
        print('move to pos', pos0, pos1, pos2, pos3)
        dev.moveToPosition(pos0,pos1,pos2,pos3)
        while dev.isRunning(): 
            time.sleep(0.2)
        time.sleep(1.0)
    print(dev.getPosition())
    dev.setDrivePowerOff()

if 0:
    dev.setOrientation('+','+','+','+')
    #dev.setOrientation('-','-','-','-')
    dev.setSpeed(100.0)
    dev.setDrivePowerOn()
    time.sleep(1.0) # Required

    dev.moveAxisToHome('x0')
    while dev.isRunning():
        time.sleep(0.5)
        print('sleeping')
    dev.setDrivePowerOff()

if 0:
    print(dev.getPosition())
    dev.setSpeed(100.0)
    dev.setOrientation('+','+','+','+')
    dev.setDrivePowerOn()
    time.sleep(1.0) # Required
    pos_list = [50,0]
    for pos in pos_list:
        dev.moveAxisToPosition('x0',pos)
        while dev.isRunning():
            time.sleep(0.5)
        print(dev.getPosition())
        time.sleep(5.0)
    dev.setDrivePowerOff()
    print(dev.getPosition())

if 0:

    print("turn on power")
    dev.setDrivePowerOn()
    for i in range(5):
        time.sleep(1.0)
        print('sleeping')
    dev.setDrivePowerOff()




    
