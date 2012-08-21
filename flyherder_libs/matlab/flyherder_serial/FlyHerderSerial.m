%
% FlyHerderSerial  - provides a serial interface to the fly herder hardware.
%
% Normal methods
% --------------
% 
% Dynamically generated Methods
% -----------------------------
%   getDevInfo
%   getCmds
%   getRspCodes
%   getNumAxis
%   getNumDim
%   getAxisNames
%   getDimNames
%   getAxisOrder
%   getDimOrder
%   getAllowedOrientation
%   setDrivePowerOn
%   setDrivePowerOff
%   isDrivePowerOn
%   stop
%   isRunning
%   enable
%   disable
%   isEnabled
%   moveToPosition
%   moveAxisToPosition
%   moveToHome
%   isInHomePosition
%   setMaxSeparation
%   getMaxSeparation
%   getPosition
%   getAxisPosition
%   setMaxSpeed
%   getMaxSpeed
%   setAcceleration
%   getAcceleration
%   setOrientation
%   getOrientation
%   setAxisOrientation
%   getAxisOrientation
%   setStepsPerMM
%   getStepsPerMM
%   setSerialNumber
%   getSerialNumber
%   getModelNumber
%
% Author: Will Dickson, IO Rodeo Inc.
% 

classdef FlyHerderSerial < handle 
    
    properties
        dev = [];
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

        %end
        %methods (Access=private)

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
                fprintf(obj.dev,'%c\n',cmdStr);

                % Get response as json string and parse
                rspStrJson = fscanf(obj.dev,'%c');
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
            cmdName = S(1).subs;
            cmdArgs = S(2).subs;
            cmdId = obj.cmdIdStruct.(cmdName);
            rspStruct = obj.sendCmd(cmdId,cmdArgs{:});
            rspFieldNames = fieldnames(rspStruct);
            if length(rspFieldNames) == 0
                rtnVal = [];
            elseif length(rspFieldNames) == 1;
                rtnVal = rspStruct.(rspFieldNames{1});
            else
                rtnVal = rspStruct;
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

    end
    
end
