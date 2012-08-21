function test_flyherder
    x = FlyHerderSerial('com67');
    x.open();
    x.getAxisPosition('y0')
    x.isDrivePowerOn()
    x.delete();
end
