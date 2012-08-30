function random_moves(port)
% random_moves: performs a homing move followed by a positioning move
% to a random location in the arena as a test of the system.
%
% Note, make sure to set the proper x and y separation when using this
% function. 
%
% Usage: 
%
% random_moves('com5')  % replace com5 with the port of your device.
%
%
    numTrials = 25;
    homeSpeed = 90.0;
    moveSpeed = 50.0;

    maxSeparation.x = 200;
    maxSeparation.y = 200;

    orientation.x0 = '-';
    orientation.y0 = '-';
    orientation.x1 = '+';
    orientation.y1 = '+';

    dev = FlyHerderSerial(port);
    dev.open();
    dev.setDrivePowerOn();

    dev.setMaxSeparation(maxSeparation);  
    dev.setOrientation(orientation);   

    for i = 1:numTrials
        fprintf('trial %d \n',i);
        fprintf('  homing ... ');
        dev.setSpeed(homeSpeed);
        dev.moveToHome();
        dev.wait();  
        fprintf('done\n');

        % Pause for a little bit for added dramma
        pause(1.0)

        % Move to the position
        deltaMax = 0.25*min(maxSeparation.x, maxSeparation.y);
        deltaMin = 0.05*min(maxSeparation.x, maxSeparation.y);
        delta = randi([deltaMin,deltaMax],1);
        pos.x0 = randi([10,maxSeparation.x-(delta+1)],1);
        pos.y0 = randi([10,maxSeparation.y-(delta+1)],1);
        pos.x1 = pos.x0 + delta;
        pos.y1 = pos.y0 + delta;

        fprintf( ... 
            '  moving to position (x0,y0,x1,y1) = (%1.1f,%1.1f,%1.1f,%1.1f) ... ', ... 
            pos.x0, ...
            pos.y0, ...
            pos.x1, ...
            pos.y1  ...
            );

        dev.setSpeed(moveSpeed);
        dev.moveToPosition(pos.x0, pos.y0, pos.x1, pos.y1);
        dev.wait();
        fprintf('done\n');
        pos = dev.getPosition();
        fprintf( ... 
            '  current position   (x0,y0,x1,y1) = (%1.1f,%1.1f,%1.1f,%1.1f)\n', ... 
            pos.x0, ...
            pos.y0, ...
            pos.x1, ...
            pos.y1  ...
            );
    end

    % Clean up - 
    dev.setDrivePowerOff();
    dev.close();
    delete(dev);

end
