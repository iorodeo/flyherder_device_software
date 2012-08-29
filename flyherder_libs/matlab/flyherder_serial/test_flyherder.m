function test_flyherder
    dev = FlyHerderSerial('com59');
    dev.open();
    axisNames = dev.getAxisNames();
    disp(axisNames);
    pos = dev.getPosition();
    disp(pos);
    order = dev.getAxisOrder();
    disp(order)
    delete(dev);
end
