%
% FlyHerderSerial - provides a serial interface to the flyherder hardware. 
%
% Notes: 
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
%
% Usage:  
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
%
% Regular (public) class methods
% -----------------------------
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
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   * getAllowedOrientation - broken??
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   * setDrivePowerOn - turns the power to the stepper motor drive on.
%     Usage: dev.setDrivePowerOn()
%
%   * setDrivePowerOff - turns the power to the stepper motor drive off.
%     Usage: dev.setDrivePowerOff()
%
%   * isDrivePowerOn
%   * stop
%   * isRunning
%   * enable
%   * disable
%   * isEnabled
%   * moveToPosition
%   * moveAxisToPosition
%   * moveToHome
%   * isInHomePosition
%   * setMaxSeparation
%   * getMaxSeparation
%   * getPosition
%   * getAxisPosition
%   * setMaxSpeed
%   * getMaxSpeed
%   * setAcceleration
%   * getAcceleration
%   * setOrientation
%   * getOrientation
%   * setAxisOrientation
%   * getAxisOrientation
%   * setStepsPerMM - sets the device's steps per millimeter value
%     Usage: dev.setStepsPerMM(stepsPerMM)
%
%   * getStepsPerMM - returns the device's current steps per millimeter value
%     Usage: stepsPerMM = dev.getStepsPerMM()
%
%   * setSerialNumber - sets the devices serial number
%     Usage: dev.setSerialNumber(serialNum)
% 
%   * getSerialNumber - returns the device serial number
%     Usage: serialNum = dev.getSerialNumber()
%
%   * getModelNumber - returns the device model number
%     Usage: modelNum = dev.getModelNumber()
%
% Author: Will Dickson, IO Rodeo Inc.
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
    end

    properties (Constant) 

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
                while obj.isRunning()
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
            %if length(cmdArgs) == 1 && strcmp(class(cmdArgs{1}), 'struct')
            %end

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
        end

        function createOrderedDimNames(obj)
        end
    end
    
end
