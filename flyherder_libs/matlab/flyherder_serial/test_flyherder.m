function test_flyherder
    dev = FlyHerderSerial('com59');
    dev.open();

    dev.setMaxSeparation(200,200);
    dev.getMaxSeparation()

    sep.x = 200
    sep.y = 200

    dev.setMaxSeparation(sep);
    dev.getMaxSeparation()


    %axisNames = dev.getAxisNames();
    %disp(axisNames);
    %pos = dev.getPosition();
    %disp(pos);
    %order = dev.getAxisOrder();
    %disp(order)
    %allowedOrient = dev.getAllowedOrientation()
    delete(dev);
end
