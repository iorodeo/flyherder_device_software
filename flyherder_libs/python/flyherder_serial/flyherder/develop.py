from __future__ import print_function
from flyherder import FlyHerder
from pprint import pprint

debug = False
dev = FlyHerder(port='/dev/ttyUSB0',timeout=2.0,debug=debug)
print(dev.isDrivePowerOn())
dev.setDrivePowerOn()
print(dev.isDrivePowerOn())
dev.setDrivePowerOff()
print(dev.isDrivePowerOn())
