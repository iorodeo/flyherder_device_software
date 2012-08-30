%
% FlyHerderSerial - provides a serial interface to the flyherder hardware. 
%
% Notes: 
%
%  * The flyherder device has 4 axes - x0, y0, x1, y1.  
%  * The xi axes move in the x-direction and the yi axes move in the y-direction. 
%  * The positions of the x1, y1 axes  are assumed to be greater then or equal 
%    to the positions of the x0, y0 axes - i.e.  
%    pos(x1) >= pos(x0) and pos(y1) > pos(y0).
%  * In the home position the device has maxiumum separation between the axes i.e. 
%    dist(pos(x0), pos(x1)) = maxSeparationX, 
%    dist(pos(y0), pos(y1)) = maxSeparationY,
%    in the home position. In other words the home positions is 
%    homePostion = (0, 0, maxSeparationX, maxSeparationY)
%  * The x0, y0 axes always home in the negative direction and the x1, y1 axes always 
%    home in the positive direction.
%
% Usage:  
%
%   dev = FlyHerderSerial('com2')           % creates a flyherder device object
%   dev.open()                              % opens a serial connection to the device
%   dev.setMaxSeparation(300.0, 300.0)      % set max x0, x1 and y0, y1 axis separations
%   dev.setOrientation('-', '-', '+', '+')  % set axis orientations x0, y0, x1, y1
%   dev.setDrivePowerOn()                   % turn on drive power
%   dev.moveToHome()                        % move to home  - uses limit switches
%   dev.wait()                              % wait until homing move is complete
%   dev.moveToPosition(50,60,70,80)         % move to position where 
%                                           % pos(x0)=50, pos(y0)=60, pos(x1)=70, pos(y1)=80 
%   dev.wait()                              % wait until move is complete
%   pos = dev.getPosition()                 % get the current position
%   dev.moveToHome()                        % move to home position
%   dev.wait()                              % wait until move is complete
%   dev.setDrivePowerOff()                  % turn off drive power
%   dev.close()                             % close serial connection
%   delete(dev)                             % deletes the device
%
%
% Public properties
% ------------------
%
%   * debug = debug flag, turns on debug messages if true. 
%
%   (Dependent)
%   * isOpen    = true is serial connection to device is open, false otherwire
%   * devInfo   = device information structure (model number, serial number)
%   * cmdIds    = structre of command identification numbers retrieved from device. 
%   * rspCodes  = structure of response codes retreived from device.
%
%
% Note, in what follows 'dev' is assumed to be an instance of the FlyHerderSerial class.
% dev = FlyHerderSerial(portName)
%
% Regular (public) class methods
% -----------------------------
%
%   * open - opens serial connection to device
%     Usage: dev.open()
%
%   * close - closes serial connection to device
%     Usage: dev.close()
%
%   * delete - deletes instance of device object.
%     Usage: dev.delete() or delete(dev)
%
%   * wait - waits until all moves currently running on the device have stopped.
%     Usage: dev.wait()
%
%   * printDynamicMethods - prints the names of all dynamically generated class 
%     methods. Note, the device must be opened for this command to work.
%     Usage: dev.printDynamicMethods()
% 
% 
% Dynamically generated (public) class methods
% ---------------------------------------------
%
%  Note, the serial connection to the device must be open for these methods to exist.
%
%   * getDevInfo - returns structure containing device information, e.g. serial number, 
%     model number. 
%     Usage: infoStruct = dev.getDevInfo()
%
%   * getNumAxis - returns the number of axes 
%     Usage: numAxes = dev.getNumAxis()
%
%   * getNumDim - returns the number of dimension - should 1/2 the number of axes.
%     Usage: numDim = dev.detNumDim()
%
%   * getAxisNames - returns a cell array of the axis names
%     axisNames = dev.getAxisNames()
%
%   * getDimNames - returns a cell array of the dimensions names
%     dimNames = dev.getDimNames()
%
%   * getAxisOrder - returns a structure specifying the order (for function arguments) of 
%     the axes. Note, each axis name shows up as a fieldname of the structure.
%     Usage: axisOrderStruct = dev.getAxisOrder()
% 
%   * getDimOrder = returns a structure specifying the order (for function calls) of 
%     dimensions. Note, each dimension name shows up as a fieldname of the structure.
%     Usage: dimOrder = dev.getDimOrder()
%
%   * getAllowedOrientation - returns cell array containing the allowed axis orientations 
%     Usage:  allowedOrientations = dev.getAllowedOrientations()
%
%   * setDrivePowerOn - turns the power to the stepper motor drive on.
%     Usage: dev.setDrivePowerOn()
%
%   * setDrivePowerOff - turns the power to the stepper motor drive off.
%     Usage: dev.setDrivePowerOff()
%
%   * isDrivePowerOn - returns true or false based on whether or not drive power is on.
%     Usage: value = dev.isDrivePowerOn()
%     
%   * stop - stops all currently running moves.
%     Usage: dev.stop()
%    
%   * isRunning - return true or false based on whether or not any moves are currently in 
%     in progress.
%     Usage: value = dev.isRunning()
%
%   * moveToPosition - move system to given position
%     Usage: dev.moveToPosition(x0, y0, x1, y1) or dev.moveToPosition(pos) where
%      - x0, y0, x1, y1 are the position (mm) of the axes with the same name or 
%      - pos is a structure with fields x0, y0, x1, y1 specifying the axis positions (mm)
%
%   * moveAxisToPosition - moves the specified axis to the given position
%     Usage: dev.moveAxisToPosition(axisName, position)
%       - axisName = name of axis to move 'x0', 'y0', 'x1', or 'y1'
%       - position = position to which axis should be moved
%
%   * moveToHome - move the system to the home position. Uses the limit switches 
%     to determine whether or not the home positions has been reached.
%     Usage: dev.moveToHome()
%
%   * moveAxisToHome - move the specified axis to the home position. Uses the
%     limits switches
%     to determine home position.
%     Usage: dev.moveAxisToHome(axisName)
%      - axisName = 'x0', 'y0', 'x1', 'y1'
%
%   * isInHomePosition - returns true or false based on whether or not the system 
%     is in the home 
%     position
%     Usage: value = dev.isInHomePosition()
%
%   * setMaxSeparation - sets the jaws open (or maximum) separation for the x
%     and y directions. This value is used to determine how far to move in
%     subsequent  positioning moves. 
%     Usage:  dev.setMaxSeparation(dx, dy) or dev.setMaxSeparation(sepStruct) where
%      - dx, dy are the jaws open (or max) separations between the x0,x1 and y0,y1 
%        axes  respectively.
%      - sepStruct is structure with fields named 'x' and 'y' the values of which 
%        specify the maximum separations in the x and y directions.
%
%   * getMaxSeparation - returns a structure containing the current values for 
%     the maximum separation for the x and y  directions. 
%     Usage: maxSeparation = dev.getMaxSeparation()
%
%   * getPosition - returns a structure with fields 'x0', 'y0', 'x1', 'y1' whose 
%     values specify the current position of the device. 
%     Usage: pos = dev.getPosition()
%
%   * getAxisPosition - returns the position of the specified axis.
%     Usage:  pos = dev.getAxisPosition(axisName)
%      - axisName = 'x0', 'y0', 'x1', 'y1'
%
%   * setPosition - set the current position of the system to the current values. 
%     Note, does not move the system - just sets the position value.
%     Usage: dev.setPosition(x0,y0,x1,y1) or dev.setPosition(pos) where
%      - x0, y0, x1, y1 are the desired positions of the axes in mm
%      - pos is a structure with fields named 'x0', 'y0', 'x1', 'y1' which specify 
%        the desired positions in mm.
%
%   * setAxisPosition - sets the position of the specified axis. Note, does not move 
%     axis - just sets the position value.
%     Usage: dev.setPosition(axisName, pos)
%      - axisName = 'x0', 'y0', 'x1', 'y1'
%      - pos = position in mm
%
%   * setSpeed - sets the desired operating speed for the all axes. 
%     Usage: dev.setSpeed(speed)
%      - speed = desired speed mm/s, allowed arange 0.1mm/s to 90mm/s
%
%   * getSpeed - returns the current operating speed in in mm/s
%     Usage: speed in dev.getSpeed()
%
%   * setOrientation - sets the orientation values for all axis. Note, an axis can be in 
%     normal, '+', orientatin or in inverted, '-', orientation. The output of the direction
%     pin is inverted when an axis's orientatin is inverted. 
%     Usage: dev.setOrientation(val_x0, val_y0, val_x1, val_y1) or 
%            dev.setOrientation(orientationStruct) where
%      - val_x0, val_y0, val_x1, val_y1 are the orientations values for the 
%        x0, y0, x1, y1 axis with values or either '+', or '-'
%      - orientationStruct = structure with fields x0, y0, x1, y1 whose
%        values, either '+' or '-' specify the desired orientation value.
%
%   * getOrientation - returns a structure with fields 'x0', 'y0', 'x1', 'y1' whose 
%     values are the current orientation setting for the axis with that name.
%     Usage: orientationStruct = dev.getOrientation()
%
%   * setAxisOrientation - sets the orientation of the specified axis.
%     Usage: dev.setOrientation(axisName, orientation) where
%      - axisName = 'x0', 'y0', 'x1', or 'y1'
%      - orientation = desired orientation - either '+' for normal or '-' for inverted.
%
%   * getAxisOrientation - returns the current axis orientation setting - either '+' or
%     '-' (inverted).
%     Usage: orientation = dev.getAxisOrientation()
%     
%   * setStepsPerMM - sets the steps per millimeter value used to convert positions 
%     in mm to  stepper motor steps.
%     Usage: dev.setStepsPerMM(stepsPerMM)
%
%   * getStepsPerMM - returns the  current steps per millimeter value used to 
%     convert positions in mm to stepper motor steps.
%     Usage: stepsPerMM = dev.getStepsPerMM()
%
%   * setSerialNumber - sets the serial number of the device (NOT IMPLEMENTED)    
%     Usage: dev.setSerialNumber(serialNum)
% 
%   * getSerialNumber - returns the device serial number
%     Usage: serialNum = dev.getSerialNumber()
%
%   * getModelNumber - returns the device model number
%     Usage: modelNum = dev.getModelNumber()
%

classdef FlyHerderSerial < handle 
    
    properties
        dev = [];
        debug = false;
    end

    properties (Access=private)
        cmdIdStruct = [];
        devInfoStruct = [];
        rspCodeStruct = [];
        orderedAxisNames = {};
        orderedDimNames = {};

    end

    properties (Constant, Access=private) 

        % Serial communication parameters.
        baudrate = 9600;
        databits = 8;
        stopbits = 1;
        timeout = 1.0;
        terminator = 'LF';
        resetDelay = 2.0;
        inputBufferSize = 2048;
        waitPauseDt = 0.25;

        % Command ids for basic commands.
        cmdIdGetDevInfo = 0;
        cmdIdGetCmds = 1;
        cmdIdGetRspCodes = 2;

    end


    properties (Dependent)
        isOpen;
        devInfo;
        cmdIds;
        rspCodes;
    end

    methods 

        function obj = FlyHerderSerial(port)
            % FlyHerderSerial - class constructor.
            obj.dev = serial( ...
            port, ...
            'baudrate', obj.baudrate, ...
            'databits', obj.databits, ... 
            'stopbits', obj.stopbits, ...
            'timeout', obj.timeout, ...
            'terminator', obj.terminator,  ...
            'inputbuffersize', obj.inputBufferSize ...
            );

        end

        function open(obj)
            % Open - opens a serial connection to the fly herder hardware.
            if obj.isOpen == false
                fopen(obj.dev);
                pause(obj.resetDelay);
                obj.createRspCodeStruct();
                obj.createDevInfoStruct();
                obj.createCmdIdStruct();
                obj.createOrderedAxisNames();
                obj.createOrderedDimNames();
            end
        end

        function close(obj)
            % Close - closes the connection to the device.
            if obj.isOpen == true
                fclose(obj.dev);
            end
        end

        function delete(obj)
            % Delete - deletes the object.
            if obj.isOpen
                obj.close();
            end
            delete(obj.dev);
        end

        function isOpen = get.isOpen(obj)
            % get.isOpen - returns true/false depending on whether the connection
            % to the device is open.
            status = get(obj.dev,'Status');
            if strcmpi(status,'open')
                isOpen = true;
            else
                isOpen = false;
            end
        end

        function printDynamicMethods(obj)
            % printDyamicMethods - prints all dynamically generated class methods.
            cmdIdNames = fieldnames(obj.cmdIdStruct);
            fprintf('\n');
            fprintf('FlyHerder Dynamic Methods\n');
            fprintf('-------------------------\n');
            for i = 1:length(cmdIdNames)
                fprintf('%s\n',cmdIdNames{i});
            end
        end

        function devInfo = get.devInfo(obj)
            % get.devInfo - returns the device information structure.
            devInfo = obj.devInfoStruct;
        end


        function cmdIds = get.cmdIds(obj)
            % get.cmdIds - returns the sturcture of command Ids.
            cmdIds = obj.cmdIdStruct;
        end

        function rspCodes = get.rspCodes(obj)
            % get.rspCodes - returns the structure of device response codes.
            rspCodes = obj.rspCodeStruct;
        end

        function wait(obj)
            % wait - wait until the device is no longer running, i.e., until
            % isRunning is false.
            if obj.isOpen
                while true 
                    rsp = obj.sendCmd(obj.cmdIdStruct.isRunning)
                    if ~rsp.isRunning
                        break;
                    end
                    pause(obj.waitPauseDt);
                end
            end
        end

        function varargout = subsref(obj,S)
            % subsref - overloaded subsref function to enable dynamic generation of 
            % class methods from the cmdIdStruct structure. 
            val = [];
            if obj.isDynamicMethod(S)
                val = obj.dynamicMethodFcn(S);
            else
                if nargout == 0
                    builtin('subsref',obj,S);
                else
                    val = builtin('subsref',obj,S);
                end
            end
            if ~isempty(val)
                varargout = {val};
            end
        end


        function rspStruct = sendCmd(obj,cmdId,varargin)
            % sendCmd - sends a command to device and reads the device's response.
            % The device responds to all commands with a serialized json string.
            % This string is parsed into a Matlab structure.
            %
            % Arguments:
            %  cmdId    = the Id number of the command
            %  varagin  = any additional arguments required by the command
            if obj.isOpen

                % Send command to device
                cmdStr = obj.createCmdStr(cmdId, varargin);
                if obj.debug
                    fprintf('cmdStr: '); 
                    fprintf('%c',cmdStr);
                    fprintf('\n');
                end
                fprintf(obj.dev,'%c\n',cmdStr);

                % Get response as json string and parse
                rspStrJson = fscanf(obj.dev,'%c');
                if obj.debug
                    fprintf('rspStr: '); 
                    fprintf('%c',rspStrJson);
                    fprintf('\n');
                end
                try
                    rspStruct = loadjson(rspStrJson);
                catch ME
                    disp('Unable to parse device response');
                    rethrow(ME);
                end

                % Check the returned cmd Id 
                try
                    rspCmdId = rspStruct.cmdId;
                    rspStruct = rmfield(rspStruct, 'cmdId');
                catch ME
                    disp('Device response does not contain command Id')
                    rethrow(ME);
                end
                if rspCmdId ~= cmdId
                    errMsg = sprintf( ...
                        'Command Id returned, %d, does not match that sent, %d', ...
                        rspCmdId, ...
                        cmdId ...
                        );
                    error(errMsg);
                end

                % Get response status
                try
                    rspStatus = rspStruct.status;
                    rspStruct = rmfield(rspStruct,'status');
                catch ME
                    disp('Device response does not contain status');
                    rethrow(ME);
                end

                % Check response status 
                if ~isempty(obj.rspCodeStruct)
                    if rspStatus ~= obj.rspCodeStruct.rspSuccess
                        try
                            errMsg = rspStruct.errMsg;
                        catch ME
                            errMsg = 'repsonse status ~= rspSuccess &' 
                            errMsg = [errMsg, ' device response does not contain errMsg'];
                        end
                        error(errMsg);
                    end
                end

            else
                error('Connection must be open to send message');
            end
        end

    end

    methods (Access=private)

        function cmdStr = createCmdStr(obj, cmdId, cmdArgs) 
            % createCmdStr - create a command string for sending to the device given
            % the cmdId number and a cell array of the commands arguments.
            cmdStr = sprintf('[%d',uint16(cmdId));
            for i=1:length(cmdArgs)
                arg = cmdArgs{i};
                switch class(arg)
                    case 'double'
                        cmdStr = sprintf('%s, %f', cmdStr, arg);
                    case 'char'
                        cmdStr = sprintf('%s, %s', cmdStr, arg);
                    otherwise
                        error('unknown type %s',class(arg));
                end
            end
            cmdStr = sprintf('%s]',cmdStr);
        end

        function flag = isDynamicMethod(obj,S)
            % isDynamicMethod - used in the subsred function to determine whether
            % or not the method is dynamically generated. This is determined by
            % whether or not the name of the method given method is also the name
            % of a field in the cmdIdStruct. 
            %
            % Arguments:
            %  S = 'type' + 'subs' stucture passed to subsref function
            flag = false;
            if ~isempty(obj.cmdIdStruct)
                if S(1).type == '.' & isfield(obj.cmdIdStruct,S(1).subs)
                    flag= true;
                end
            end
        end

        function rtnVal = dynamicMethodFcn(obj,S)
            % dynamicMethodFcn - implements a the dynamically generated methods.

            % Get command name, command args and command id number
            cmdName = S(1).subs;
            try
                cmdArgs = S(2).subs;
            catch
                cmdArgs = {};
            end
            cmdId = obj.cmdIdStruct.(cmdName);
            % Convert command arguments from structure if required
            if length(cmdArgs) == 1 && strcmp(class(cmdArgs{1}), 'struct')
                cmdArgs = obj.convertArgStructToCell(cmdArgs{1});
            end

            % Send command and get response
            rspStruct = obj.sendCmd(cmdId,cmdArgs{:});

            % Convert response into return value.
            rspFieldNames = fieldnames(rspStruct);
            if length(rspFieldNames) == 0
                rtnVal = [];
            elseif length(rspFieldNames) == 1;
                rtnVal = rspStruct.(rspFieldNames{1});
            else
                emptyFlag = true;
                for i = 1:length(rspFieldNames)
                    name = rspFieldNames{i};
                    value = rspStruct.(name);
                    if ~isempty(value)
                        emptyFlag = false;
                    end
                end
                % Return structure or if only fieldnames return cell array of fieldnames
                if ~emptyFlag
                    rtnVal = rspStruct;
                else
                    rtnVal = rspFieldNames;
                    for i = 1:length(rtnVal)
                        name = rtnVal{i};
                        % Convert '-' character back to human readable
                        if strcmp(name, 'x0x2D_')
                            rtnVal{i} = '-';
                        end
                        % Convert '+' character back to human readable
                        if strcmp(name, 'x0x2B_');
                            rtnVal{i} = '+';
                        end
                    end
                end
            end
        end

        function createCmdIdStruct(obj)
            % createCmdIdStruct - gets structure of command Ids from device.
            obj.cmdIdStruct = obj.sendCmd(obj.cmdIdGetCmds);
        end

        function createRspCodeStruct(obj)
            % createRspCodeStruct - gets structure of response codes from the device.
            obj.rspCodeStruct = obj.sendCmd(obj.cmdIdGetRspCodes);
        end

        function createDevInfoStruct(obj)
            % createDevInfoStruct - get device information struture from device.
            obj.devInfoStruct = obj.sendCmd(obj.cmdIdGetDevInfo);
        end

        function createOrderedAxisNames(obj)
            % Creates cell array of ordered axis names. Designed to be
            % used in open call so can't use subsref. Should be called after
            % createCmdIdStruct.
            axisOrderStruct = obj.sendCmd(obj.cmdIdStruct.getAxisOrder);
            obj.orderedAxisNames = {};
            axisFields = fieldnames(axisOrderStruct);
            for i = 1:length(axisFields)
                name = axisFields{i};
                num = axisOrderStruct.(name);
                obj.orderedAxisNames{num+1} = name;
            end
        end

        function createOrderedDimNames(obj)
            % Creates cell array of ordered Dimension names.  Designed to be
            % used in open call so can't use subsref. Should be called after
            % createCmdIdStruct.
            dimOrderStruct = obj.sendCmd(obj.cmdIdStruct.getDimOrder);
            obj.orderedDimNames = {};
            dimFields = fieldnames(dimOrderStruct);
            for i = 1:length(dimFields)
                name = dimFields{i};
                num = dimOrderStruct.(name);
                obj.orderedDimNames{num+1} = name;
            end
        end

        function cmdArgs = convertArgStructToCell(obj,argStruct) 
            % Converts a command argument from a structure to a cell array. Note,
            % the structure must have fields which are either the axis names or
            % the dimension names.
            cmdArgs = {};
            argFieldNames = fieldnames(argStruct);
            if isCellEqual(obj.orderedAxisNames,argFieldNames)
                orderedNames = obj.orderedAxisNames;
            elseif isCellEqual(obj.orderedDimNames,argFieldNames)
                orderedNames = obj.orderedDimNames;
            else
                error('unknown structure as command argument');
            end
            for i = 1:length(orderedNames)
                name = orderedNames{i};
                cmdArgs{i} = argStruct.(name);
            end
        end

    end
end

% Utility functions
% -----------------------------------------------------------------------------
function flag = isCellEqual(cellArray1, cellArray2)
    % Tests whether or not two cell arrays of strings are eqaul.  
    % Returns false if both cell array don't consist entirely of strings 
    flag = true;
    for i = 1:length(cellArray1)
        string = cellArray1{i};
        if ~isInCell(string,cellArray2)
            flag = false;
        end
    end
end


function flag = isInCell(string, cellArray)
    % Test whether or not the given string is in the given cellArray.
    flag = false;
    if strcmp(class(string), 'char')
        for i = 1:length(cellArray)
            if strcmp(class(cellArray{i}),'char') && strcmp(cellArray{i}, string)
                flag = true;
                break;
            end
        end
    end
end
