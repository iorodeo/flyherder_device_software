function basic_example(port)
% basic_example:  demonstrates basic use of the flyHerderSerial class to 
% control the flyherder device.
%
% Note, current assumes correct orientation is '-', '-', '+', '+' for
% x0, y0, x1, y1
%
% Usage: 
%
% basic_example('com5')
%
%

    homeSpeed = 80.0;
    moveSpeed = 30.0;

    maxSeparationX = 200;
    maxSeparationY = 200;

    dev = FlyHerderSerial(port);
    dev.open();
    dev.setDrivePowerOn();

    % Set the maximum (jaw open) separation for the x and y directions
    dev.setMaxSeparation(maxSeparationX, maxSeparationY);  

    % Set the axis orientations ('+' = normal, '-' = reversed)
    dev.setOrientation('-', '-', '+', '+');   % x0, y0, x1, y1

    % Move to the home position (uses limit switches to find home)
    % and wait until move is complete.
    fprintf('homing ... ');
    dev.setSpeed(homeSpeed);
    dev.moveToHome();
    dev.wait();  
    fprintf('done\n');

    % Move to the position
    fprintf('moving to position ...');
    dev.setSpeed(moveSpeed);
    posX0 = 2.0*maxSeparationX/5.0;
    posY0 = 2.0*maxSeparationY/5.0;
    posX1 = 3.0*maxSeparationX/5.0;
    posY1 = 3.0*maxSeparationY/5.0;
    dev.moveToPosition(posX0, posY0, posX1, posY1);
    dev.wait();
    fprintf('done\n');

    dev.close();
    delete(dev);
end
