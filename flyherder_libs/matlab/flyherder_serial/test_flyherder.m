function test_flyherder
    x = FlyHerderSerial('com67');
    x.open();
    x.getDevInfo()
    %x.rspCodes
    %x.cmdIds
    %x.devInfo
    %%rsp = x.sendCmd(1);
    %x.close();
    x.delete();
end
