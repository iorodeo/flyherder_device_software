from __future__ import print_function
import sys
import time
import random
from flyherder_serial import FlyHerder

debug = False
dev = FlyHerder(port='/dev/ttyUSB0',timeout=2.0,debug=debug)


if 0:
    print(dev.getPosition())
    dev.setSpeed(100.0)
    dev.setOrientation('-','-','+','+')
    print(dev.getOrientation())
    dev.setDrivePowerOn()

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
    axis = 'x0'
    maxSep = 300.0
    dev.setMaxSeparation(maxSep,maxSep)
    dev.setOrientation('-','-','+','+')
    dev.setSpeed(50.0)
    dev.setDrivePowerOn()
    for i in range(0,2):
        print('trial {0}'.format(i))
        sys.stdout.flush()

        print('  homing ... ', end = '')
        sys.stdout.flush()
        dev.wait()
        print('done')
        sys.stdout.flush()
        time.sleep(1.0)

        print('  moving to position ... ', end = '')
        sys.stdout.flush()
        dev.moveAxisToPosition(axis,100)
        dev.wait()
        print('done')
        sys.stdout.flush()
        time.sleep(1.0)

    dev.setDrivePowerOff()

if 1:
    homeSpeed = 90.0
    moveSpeed = 90.0
    maxSep = 25.4*(7.0 + 7.0/8.0)
    print()
    print('homeSpeed {0}'.format(homeSpeed))
    print('moveSpeed {0}'.format(moveSpeed))
    print('maxSep {0}'.format(maxSep))
    dev.setMaxSeparation(maxSep,maxSep)
    dev.setOrientation('-','-','+','+')
    dev.setDrivePowerOn()
    for i in range(0,2):

        print()
        print('trial {0}'.format(i))
        sys.stdout.flush()

        print('  homing ... ',end='')
        sys.stdout.flush()
        dev.setSpeed(homeSpeed)
        #dev.moveToHome()
        dev.setPosition(0,0,maxSep, maxSep)
        dev.wait()
        print('done')
        print('  isInHomePosition==', dev.isInHomePosition())
        posDict = dev.getPosition()
        posTuple = posDict['x0'], posDict['y0'], posDict['x1'], posDict['y1']
        print('  pos {0}'.format(posTuple))
        print()
        dev.enableBoundsCheck()

        sys.stdout.flush()
        time.sleep(1.0)
        delta = 20 
        pos_x0 = random.randint(delta+1,int(maxSep - (delta+1)))
        pos_y0 = random.randint(delta+1,int(maxSep - (delta+1)))
        pos_x1 = pos_x0 + delta
        pos_y1 = pos_y0 + delta
        pos = (pos_x0, pos_y0, pos_x1, pos_y1)
        #pos = (100,100,100,100)
        print('  moving to position {0} ... '.format(pos), end='')
        sys.stdout.flush()
        dev.setSpeed(moveSpeed)
        dev.moveToPosition(*pos)
        dev.wait()
        print('done')
        print('  isInHomePosition==', dev.isInHomePosition())
        posDict = dev.getPosition()
        posTuple = posDict['x0'], posDict['y0'], posDict['x1'], posDict['y1']
        print('  pos {0}'.format(posTuple))
        sys.stdout.flush()
        time.sleep(1.0)

    dev.setDrivePowerOff()


if 0:
    axis = 'y1'
    direction = '+'
    print(dev.getPosition())
    dev.setSpeed(50.0)
    dev.setAxisOrientation(axis, direction)
    dev.setDrivePowerOn()
    pos_list = [50,0]
    for pos in pos_list:
        dev.moveAxisToPosition(axis,pos)
        while dev.isRunning():
            time.sleep(0.2)
        print(dev.getPosition())
        time.sleep(1.0)
    dev.setDrivePowerOff()
    print(dev.getPosition())





    
