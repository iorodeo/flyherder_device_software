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

if 1:
    print(dev.getPosition())
    dev.setMaxSpeed(200.0)
    dev.setAcceleration(100.0)
    #dev.setOrientation('+','+','+','+')
    dev.setOrientation('-','-','-','-')
    print(dev.getOrientation())
    dev.setDrivePowerOn()
    dev.enable()
    time.sleep(2.0)

    pos_list = [-50,0]
    for pos in pos_list:
        print('move to pos', pos)
        dev.moveToPosition(pos,pos,pos,pos)
        #dev.setAxisOrientation('x0','-')
        #dev.moveAxisToPosition('x0',pos)
        while 1:
            try:
                if not dev.isRunning():
                    break
            except IOError:
                print('IOError 1')
                pass
            try:
                print(dev.getPosition())
            except Exception:
                print('IOError 2')
                pass
            time.sleep(0.2)
        time.sleep(1.0)
    print(dev.getPosition())
    dev.disable()
    dev.setDrivePowerOff()



    
