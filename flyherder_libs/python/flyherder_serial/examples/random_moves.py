from __future__ import print_function
import sys
import time
import random
from flyherder_serial import FlyHerder

debug = False
dev = FlyHerder(port='/dev/ttyUSB0',timeout=2.0,debug=debug)


homeSpeed = 90.0
moveSpeed = 30.0
maxSep = 200 

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
    dev.moveToHome()
    dev.wait()
    print('done')
    print('  isInHomePosition==', dev.isInHomePosition())

    time.sleep(1.0)

    delta = 20 
    pos_x0 = random.randint(delta+1,int(maxSep - (delta+1)))
    pos_y0 = random.randint(delta+1,int(maxSep - (delta+1)))
    pos_x1 = pos_x0 + delta
    pos_y1 = pos_y0 + delta
    pos = (pos_x0, pos_y0, pos_x1, pos_y1)
    print('  moving to position {0} ... '.format(pos), end='')
    sys.stdout.flush()
    dev.enableBoundsCheck()
    dev.setSpeed(moveSpeed)
    dev.moveToPosition(*pos)
    dev.wait()
    print('done')

    posDict = dev.getPosition()
    posTuple = posDict['x0'], posDict['y0'], posDict['x1'], posDict['y1']
    print('  pos {0}'.format(posTuple))
    sys.stdout.flush()
    time.sleep(1.0)

dev.setDrivePowerOff()



    
