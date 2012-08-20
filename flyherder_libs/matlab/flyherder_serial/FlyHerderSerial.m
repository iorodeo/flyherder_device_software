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

        % serial communication parameters
        baudrate = 9600;
        databits = 8;
        stopbits = 1;
        timeout = 1.0;
        terminator = 'LF';
        resetDelay = 2.0;
        inputBufferSize = 2048;

        % command ids for basic commands
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
            if obj.isOpen == false
                fopen(obj.dev);
                pause(obj.resetDelay);
                obj.createRspCodeStruct();
                obj.createDevInfoStruct();
                obj.createCmdIdStruct();
            end
        end

        function close(obj)
            if obj.isOpen == true
                fclose(obj.dev);
            end
        end

        function delete(obj)
            if obj.isOpen
                obj.close();
            end
            delete(obj.dev);
        end

        function isOpen = get.isOpen(obj)
            status = get(obj.dev,'Status');
            if strcmpi(status,'open')
                isOpen = true;
            else
                isOpen = false;
            end
        end

        function devInfo = get.devInfo(obj)
            devInfo = obj.devInfoStruct;
        end


        function cmdIds = get.cmdIds(obj)
            cmdIds = obj.cmdIdStruct;
        end

        function rspCodes = get.rspCodes(obj)
            rspCodes = obj.rspCodeStruct;
        end

        %end
        %methods (Access=private)

        function rspStruct = sendCmd(obj,cmdId,varargin)
            if obj.isOpen
                % Send command to device
                cmdStr = obj.createCmdStr(cmdId, varargin);
                fprintf(obj.dev,'%s\n',cmdStr);

                % Get response as json string and parse
                rspStrJson = fscanf(obj.dev,'%s',1000);
                try
                    rspStruct = loadjson(rspStrJson);
                catch ME
                    disp('unable to parse device response');
                    rethrow(ME);
                end

                % Check the returned cmd Id 
                try
                    rspCmdId = rspStruct.cmdId;
                    rspStruct = rmfield(rspStruct, 'cmdId');
                catch ME
                    disp('device response does not contain command Id')
                    rethrow(ME);
                end
                if rspCmdId ~= cmdId
                    errMsg = sprintf( ...
                        'command Id returned, %d, does not match that sent, %d', ...
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
                    disp('device response does not contain status');
                    rethrow(ME);
                end

                % Check response status 
                if ~isempty(obj.rspCodeStruct)
                    if rspStatus ~= obj.rspCodeStruct.rspSuccess
                        try
                            errMsg = rspStruct.errMsg;
                        catch ME
                            errMsg = 'repsonse status ~= rspSuccess &' 
                            errMsg = [errMsg, 'device response does not contain errMsg'];
                        end
                        error(errMsg)
                    end
                end

            else
                error('connection must be open to send message');
            end
        end

        function cmdStr = createCmdStr(obj, cmdId, cmdArgCellArray) 
            cmdStr = sprintf('[%d',uint16(cmdId));
            for cmdArg = cmdArgCellArray 
                cmdArg = cmdArg{1}
                switch class(cmdArg)
                    case 'double'
                        cmdStr = sprintf('%s, %f', cmdStr, cmdArg);
                    case 'char'
                        cmdStr = sprintf('%s, %s', cmdStr, cmdArg);
                    otherwise
                        error('unknown type %s',class(cmdArg));
                end
            end
            cmdStr = sprintf('%s]',cmdStr);
        end

        function createCmdIdStruct(obj)
            % Get structure of command Ids from device.
            obj.cmdIdStruct = obj.sendCmd(obj.cmdIdGetCmds);
        end

        function createRspCodeStruct(obj)
            % Get structure of response codes from the device.
            obj.rspCodeStruct = obj.sendCmd(obj.cmdIdGetRspCodes);
        end

        function createDevInfoStruct(obj)
            % Get device information struture from device
            obj.devInfoStruct = obj.sendCmd(obj.cmdIdGetDevInfo);
        end

        function subsref(obj,S)
            % Overload subsref in order to creaet dynamic methods from cmdIdStruct.
            if obj.isDynamicMethod(S)
                disp('dynamic method')
                S(1).subs
            else
                builtin('subsref',obj,S);
            end
        end

        function flag = isDynamicMethod(obj,S)
            flag = false;
            if ~isempty(obj.cmdIdStruct)
                if S(1).type == '.' & isfield(obj.cmdIdStruct,S(1).subs)
                    flag= true;
                end
            end
        end


    end
    
end
