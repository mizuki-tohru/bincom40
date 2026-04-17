# About the Binary Monitor bincom v40

## 1: Overview
The binary monitor bincom40 is a program that allows control of serial port input/output using the scripting language Lua. This enables emulation of serial-connected devices and automation of testing. Input/output can also be manually operated and confirmed in binary display. Simultaneously, ASCII characters can be displayed and confirmed.  
This program can operate measuring instruments with serial interfaces, HID USB, and LAN interfaces, provided they are defined in the device definition file, from a Lua script. In particular, for the OWON HDS2102S handheld oscilloscope, data and screen acquisition functions can be used from the Lua script.  
Up to 5 serial ports can be connected simultaneously. Of these, only ports defined in Config.txt can be manually operated.  

### 1.1: Manual Operation
This program can send data sequences defined in a separate file with simple operations. This program can send characters entered from the keyboard. However, inputs other than return are first stored in a buffer and are not immediately output. The characters you input are displayed at the bottom of the screen. Characters stored in the buffer are sent all at once when the return key is pressed.  
Pressing the return key repeatedly will send previously entered data repeatedly. All transmitted data characters are displayed at the bottom of the screen. This display shows only characters for keyboard input; other inputs, including repeated transmissions, are displayed in hexadecimal notation.  
This version provides setting options for controlling signals such as CTS, RTC, DTR, and DSR. Options are also available for automatically adding or removing CR and LF at the end of serial data.  

### 1.2: Script Operation
This program reads the Lua script file defined in Config.txt at startup and operates accordingly. The Lua script's functionality is extended by the functions described later. A predefined function, __Startup()__, is executed only once at startup, and a predefined function, __Loop()__, is executed at defined intervals.  
The extended function __mkframe()__ generates binary telemetry based on the fixed telemetry frame definition file defined in Config.txt. The register memory space defined within the internal register definition file defined in Config.txt can be freely accessed from Lua scripts using extension functions.  
These features make it easier to emulate devices with serial interfaces.  

## 2: Configuration  
This program requires two files for minimum operation: the executable file bincom40.exe and the configuration file config.txt.  

The transmit data definition file is read and executed at startup if the filename is specified in __config.txt__. If there is no definition in config.txt, it will search for and attempt to read a file named __command.txt__.  
The Lua script file is read and executed at startup if the filename is specified in config.txt. If the file does not exist, Lua will not run. If there is no definition in config.txt, it will search for and attempt to read a file named __script.lua__. If it does not exist, Lua will not run.  
The register definition file is read and executed at startup if the filename is specified in config.txt. This is a virtual register definition file that can be used by Lua scripts. If there is no definition in config.txt, it will search for and attempt to read a file named __reg.txt__. If it doesn't exist, Lua cannot use virtual registers.   
The instrument definition file is loaded at startup, based on the filename specified in config.txt. The instrument definition file further contains command definition files for each device.    

#### The file structure at the time of distribution is as follows:
- bincom40.exe
- config.txt
- readme.txt
- script.lua
- command.txt
- tlm.txt
- reg.txt
- device_config.txt
- DM7560_Config.txt
- PR100_Config.txt
- KX100L_Config.txt
- PAT80_Config.txt
- HDS2102S_Config.txt
- SPE6102_Config.txt
- DP100_Config.txt
- src.zip

readme.txt is this document. src.zip is the complete source code.  
This program can be compiled with Microsoft Visual C++ 2022 Express Edition.   

## 3: Settings
Before starting this program, you can configure one of the serial ports and its baud rate in Config.txt. You can also change the baud rate of this port at any time during program operation.  
Settings before program startup are done in a text file named config.txt, located in the same folder as the bincom executable file.  
Please save config.txt with UTF-8 encoding.  

The string enclosed in double quotes ("") in the __PROGRAM_TITLE__ line will be the program title.  
Please place one or more spaces between __PROGRAM_TITLE__ and the double quotes.  

The __Interval__ line allows you to specify the execution interval of the predefined function Loop in the Lua script (and the transmission interval unit for messages with the keyword INTERVAL defined in command.txt, common to both the execution interval and timing) in milliseconds.  

The __DATAFOLDER__ line allows you to specify the folder name for saving register snapshot data among the save data. This will be the name of the folder created above the folder where the program is located. Please create a folder with the same name beforehand.

Register snapshot data is generated once each time you press the [REGSAVE 0] button in the menu bar at the top of the program screen.

The file name will be DATA0.dat. If a file with the same name already exists in the folder, it will be overwritten. The button in the menu bar will change to [REGSAVE 1], but pressing this button will save the register contents at that time to a file named DATA1.dat. In other words, the button number and the file name correspond.


The register snapshot file is output in text file format.

First, the defined register names from the register definition file are output, and then the register contents are output in ascending order of device ID.

The register contents are displayed with the address (hexadecimal notation) at the beginning of each line, followed by the sequence of data within the register (hexadecimal notation) separated by a colon (:). Each entry is separated by a comma (,).


The __LOGFOLDER__ line allows you to specify the folder name where the received log data will be saved. This will be the name of the folder created above the folder where the program is located. Please create a folder with the same name beforehand.

The received log data file is generated and saved by pressing the [LOGSAVE] button in the menu bar at the top of the program screen. The save format is binary. The file name starts with LOG0.dat, and a unique file name is generated within the folder, preventing file overwriting.
Pressing [STOP] in the menu bar stops saving.

The serial port is specified by adding one or more spaces after the string "port" written at the beginning of the line, followed by a number.

__port 1__

The baud rate is specified by adding one or more spaces after the string "baud" written at the beginning of the line, followed by a number.

__baud 19200__

When using a USB serial conversion device, specifying the USB VID, PID, and device-specific serial number will automatically connect to a matching device. This is particularly useful for devices with consistent, unchanging device-specific serial numbers, such as FTDI devices, as port numbers tend to change. When using this function, specify "USB" for the port.

__port USB__
~~~
USB_VID "0x0403"
USB_PID "0x6015"
USB_SERIAL "DK0G0GKM"
~~~
These parameters can be found in Device Manager.

From Ports (COM & LPT) -> USB Serial Port (COMX) -> Properties -> Details -> Device Instance Path Value (V), extract 0x0403 from VID_0403, 0x6015 from PID_6015, and DK0G0GKM from +DK0G0GKMA, and set these as the __USB_SERIAL__ parameter.

The parity bit is specified by placing one or more spaces after the string "parity" written at the beginning of the line, and using the following keywords.

~~~
parity no # No parity
parity odd # Odd parity
parity even # Even parity
~~~
Stop bits are specified by placing one or more spaces after the string "stopbits" written at the beginning of the line, and using the following keywords:
~~~
stopbits one # 1 bit
stopbits onehalf # 1.5 bits
stopbits two # 2 bits
~~~
Control signal control is specified by placing one or more spaces after the string "modem" written at the beginning of the line, and specifying either on or off:

__modem on__

Newline insertion is specified by placing one or more spaces after the string "newline" written at the beginning of the line, and using the following keywords:

~~~
newline none # Do not add a newline to the transmitted data
newline CRLF # Add CRLF
newline CR # Add CR
newline LF # Add LF
~~~
This program has a function to periodically check if a port is open and reopen it if it is closed. This function can be turned ON/OFF.
To specify this function, place one or more spaces after the string "Openport_mode" written at the beginning of the line, and then specify the following keywords.
~~~
Openport_mode startonce # Normal
Openport_mode check # Periodically check if the port is open
~~~
The character "#" indicates a comment. Everything from # to the end of the line is interpreted as a comment.
~~~
port 1 # This is a comment
#port 2 # Commented out
~~~
Baud rate during program operation

Changes are made from "Edit." in the program's menu bar. A dialog window will open; enter a sequence of numbers in half-width characters in the edit box and press the OK button.

The __Display__ line allows you to change the display format of the lower part of the screen.

__Display hex__ displays received data in hexadecimal.

__Display Text__ displays received data in text format.

__Display lua__ delegates the entire display to Lua.

The __FIXParam_Display__ line allows you to change the number of items displayed in the fixed parameter display section at the top of the screen, from none to 36.

The fixed parameter display section displays in two lines: the parameter name on the top line and the parameter value on the bottom line. These two lines constitute one parameter line.

The size of the central display section will decrease or increase by the amount of the parameters increased or decreased.

The __MEASURE_DEVICE__ line allows you to specify the filename of the instrument definition file to load. The __SCRIPT__ line allows you to specify the filename of the Lua script to load.  
This is useful when you have multiple scripts and want to switch between them.  
The __CMD__ line allows you to specify the filename of the transmit data definition to load.  
The __TLM__ line allows you to specify the filename of the telemetry definition to load.  
The __REG__ line allows you to specify the filename of the register definition to load.  

## 4: About Instrument Definition Files

You can operate on devices defined in the instrument definition file from within a Lua script.
Instrument definitions are grouped together from the keyword  __DEVICE__ at the beginning of a line to the keyword __DEVICE_END__ at the beginning of a line, and linked to the device name written after the keyword __DEVICE__. Furthermore, you can enable/disable the instrument definition using the __on / off__ keyword immediately following the device name.  
Various instrument definitions are defined using the following keywords:  

__DEVICE_SETTING__ Reads the file specified immediately after this line.  
__DEVICE_MAKER__ Specify the manufacturer's name.  
This is optional. The predefined manufacturer names are as follows:  
_YOKOKAWA, KIKUSUI, HP, KEYSIGHT, KEISOKU, FTDI, OWON, MATSUSADA, TAKASAGO, ALIENTEK, DUMMY_  
__DEVICE_TYPE__ Specifies the connection method by number.  
_1:serial 2:IVI 3:USB HID 4:LAN 5:GPIB 6:FTDI BitBang 7:HID DP100_  
This program does not support IVI, GPIB, or FTDI BitBang.  
__DEVICE_DELIMITER__ Specifies line break insertion  
The keyword for specification is the same as in Config.txt  
__DEVICE_COMPORT__ Specifies the serial port number  
Three types can be specified: numbers, uppercase letters, and none. For example, if __DEVICE_COMPORT A__ is specified, a connection will be made using the PID, VID, and device serial number described below.
If none is specified, the port specification from within the lua script takes precedence.  
In this case, the connection using the PID, VID, and device serial number takes precedence.  
__DEVICE_COMBAUD__ Specifies the communication speed. If __DEVICE_COMPORT__ is none, the speed specification within the lua script takes precedence.  
__DEVICE_VISA_ADDR__ Not supported in this program.  
__DEVICE_LANADDR__ Specifies the IPv4 address of the target device. Enclose it in "".  
__DEVICE_LANPORT__ Specifies the port number of the target device.  
__DEVICE_GPIBPORT__ This program does not support this.  
__DEVICE_VID__ Used to uniquely identify USB devices in the USB serial converter.  
__DEVICE_PID__ Used to uniquely identify USB devices in the USB serial converter.  
__DEVICE_SERNUM__ Used to uniquely identify USB devices in the USB serial converter. Please enclose it in quotation marks ( __"__ ).  
The defined devices can be accessed by specifying the device name written after the keyword DEVICE in the Lua script's extension functions. If an uppercase letter is specified for the serial port number, individual access is possible using that letter.  

In the file specified by the keyword __DEVICE_SETTING__, you can define commands that can be used with the Lua extension function DeviceSeq.  
The syntax is as follows:  

Write the device name at the beginning of the file, after the keyword __NAME__ at the start of the line. This must match the device name in the instrument definition file.  

Instructions are issued one line at a time by adding a keyword after the keyword __CMD__ at the beginning of the line. These multiple lines constitute a single instruction within the range enclosed by __CMD NAME__ to __CMD END__. Instructions can be specified using the command name specified by __CMD NAME__. Basically, the message sent to the device is written after __CMD__. It can also include up to three parameters of three types: __P1, P2, and P3__. Parameters can be written as the third and subsequent arguments of the DeviceSeq function.  
~~~
CMD SLEEP inserts a sleep for the specified number of seconds at the time of writing.
CMD MWAIT inserts a sleep for the specified number of milliseconds at the time of writing.
CMD VOLT, CMD STOP, CMD INFO, CMD ACT, and CMD READ are DP100-specific instructions.
~~~
The keyword __INIT__ at the beginning of the line is used to define the instruction configuration used for initialization, and only one such configuration can be used in a file. __INIT__ is written in the same way as __CMD__.

## 5: About the Transmission Data Definition File

By preparing a transmission data file written in a specified format, you can send various types of data.

The format is as follows:
~~~
SEND 0x55 0xaa 65000 0b01010011 CRC "hoge" INTERVAL 10 LABEL "hoge"
~~~
If the characters __"SEND"__ appear at the beginning of a line, that line is interpreted as transmission data. From there until the newline character, the description interpreted as transmission data is held as a single unit and sent simultaneously.

One or more spaces are required after the characters __SEND__. Tabs are also acceptable, but full-width spaces cannot be used. Spaces are also used as data separators from there onward.

The following are examples of valid definitions for the description:

- 1: Hexadecimal representation in 1-byte units. A prefix of 0x indicates hexadecimal representation.
- 2: Binary representation in 1-byte units. A prefix of 0b indicates binary representation.
- 3: Decimal representation for 1 to 4 bytes.
- 4: String. Strings enclosed in double quotes are valid.

The keyword __INTERVAL__ declares that the transmission data sequence will be sent periodically thereafter. The transmission interval is specified by the number after the keyword. The unit is seconds.

The keyword __LABEL__ allows you to change the display in the __SEND__ pull-down menu on the toolbar to the description indicated by __LABEL__. If __LABEL__ is not present, the pull-down menu will display the transmission data converted to hexadecimal; __LABEL__ replaces this.

Comments can also use Japanese (UTF-8).

The keyword __CRC__ replaces this with a 2-byte CCITT-16 CRC and inserts it into the transmitted data sequence. The byte order is lower-order, then higher-order.

The character __"#"__ indicates a comment. Everything from '#' to the end of the line is interpreted as a comment. Japanese (UTF-8) characters can also be used in comments.

If valid transmitted data definitions exist, a "SEND" menu will appear in the program's menu bar. From here, you can select the data you want to send from the submenu to enable transmission.

Note that all data in the submenu is displayed in hexadecimal.

This program can also read various other transmitted data definition files.

At the end of the submenu under the SEND menu in the menu bar, there is a "ReadCMDFile" menu. Selecting this allows you to choose other files.

## 6: About Telemetry Definition Files.

A telemetry definition file is a collection of definitions for each telemetry item. Each item corresponds to its order from the beginning of the parameter array passed to the extension function `mkframe`. It is recommended to assign sequential numbers starting from 1 to each item as comments.

Each item is defined by the keyword __TLM__ followed by the item name, enclosed in curly braces {}. The {} contains the item type specified by the keyword __TYPE__ and the elements specified by the keyword __PART__. __PART__ can contain multiple elements.

The __PART__ element within the {} contains the content specified by the keywords __MAJOR, MINOR, MASK, and SHIFT__.

The keyword __MAJOR__ specifies the position from the beginning of a fixed-length telemetry frame. This is in bytes.

The keyword __MINOR__ specifies the minor frame number when a minor frame is specified. While 16 minor frames are assumed, it supports 16 or fewer frames.

If minor frames are not used, it is recommended to set all bits to 1.

The keyword __MASK__ indicates the valid bit range when retrieving and storing data from a parameter array. This is easier to understand when combined with the following item, __SHIFT__. For example, suppose you want to store a 1-bit flag in telemetry, one bit at a time. The original flag variable is either 0 or 1. The __SHIFT__ instruction moves it to the telemetry storage location, and __MASK__ is used to prevent other values ​​from being placed there. The value is then stored using a logical AND operation.

For example, to place a value into a specified bit at a certain byte position in the telemetry:
~~~
A |= (flag_1bit << SHIFT) & MASK
~~~
The keyword __SHIFT__ specifies the amount of bit shift to be performed on the parameter array values.

Each __PART__ element describes telemetry storage of up to 8 bits. For example, if the original data was 32 bits, four __PART__ elements are required.

_The _PART__ and __TYPE__ elements should be indented.

Keywords used in the __TYPE__ field should be grouped together at the beginning of the file.

The format is as follows:

__keywordDEF,space,keywordname,space,value(real number)__

Example:
~~~
DEF SYS 0
~~~
Everything from '#' to the end of the line is commented out.

## 7: About Register Definition Files.

Details will be explained in 8.4.

## 8: Lua Script Settings

### 8.1: Simple Usage

#### 8.1.1:
Find the number of an available serial port and replace the part that says

__SerialInit(0,10,115200)__

(around line 104) with the number you found. 
~~~
Example: SerialInit(0,3,115200)
Example: SerialInit(0,A,115200)
~~~
If the port is already open, it will not be opened again. If the script attempts to open a different port and specifies a non-existent port, the port opening will fail.

#### 8.1.2:
If you start the program and see "Init OK" on the screen, it is working correctly.

#### 8.1.3:
Pressing [LOGSAVE] on the toolbar at the top of the screen will record data received from the serial port, such as commands, to a file. The file will be created in a folder named "LOG".

#### 8.1.4:
Pressing [REGSAVE 0] on the toolbar at the top of the screen will output the state of the device registers that are virtually held internally to a file. A file is created each time the button is pressed.
The file name will be numbered, such as DATA0.dat, and this number will automatically increment each time a file is created.

#### 8.1.5:
When a command is received, the received value is displayed on the screen in hexadecimal. Please check whether the registers were modified by the command by saving the register state as appropriate.

### 8.2: About the Lua Language

Lua ​​is a lightweight scripting language suitable for embedded systems.

The website URL is shown below.

https://www.lua.org/

The following site provides a summary of grammar explanations, etc.

https://ie.u-ryukyu.ac.jp/~e085739/lua.hajime.html

The version used is 5.4.1.

### 8.3: About Lua Language Extensions

This Lua interpreter has been uniquely extended as follows:

#### A: The following function has been added.

##### __DispStrP()__

No arguments, no return value. If you first put text and its length into the global variables Dispstr and Displen and call this function, the text will be displayed on the screen.

Example:
~~~
Dispstr = "Init OK"
Displen = 7
DispStrP( )
~~~

###### __DispParamlP(ch,Label, Data)__  
Arguments: __ch__ Display position, __Label__ String, __Data__ Integer. No return value.
The text is displayed in two lines at a fixed position at the top of the screen: a label string and data (integer value).
It is intended for use in displaying parameters, etc. The data is displayed in decimal.
The fixed display position has 4 horizontally and 3 rows, for a total of 12 types of displays. Therefore, the value of __ch__ is 0 to 11.
Example:
~~~
DispParamlP(0,"Count",count)
count = count + 1
~~~
However, all 12 channels will only be displayed when __FIXParam_Display__ is set to 12. If __FIXParam_Display__ is 0, no display will occur even when the function is executed.

##### __DispParamfP(ch,Label, Data)__  
Arguments: __ch__ Display position, __Label__ String, __Data__ Floating-point number. No return value.

Text is displayed in two lines at a fixed position at the top of the screen: a label string and data (decimal value).

It is intended for use in displaying parameters, etc. The data is displayed in decimal format.

The fixed display position has 4 horizontally and 3 horizontally, for a total of 12 display options. Therefore, the value of ch can be 0 to 11.

However, all 12 channels will only be displayed when __FIXParam_Display__ is set to 12. If __FIXParam_Display__ is 0, no display will occur even when the function is executed.

##### __rtn = SerialInit(ch,port,baud)__  
Arguments: __ch__ Serial port definition number, can be specified from 0 to 9. __port__ Serial port number. If an alphabet letter (A-J) is specified instead of a number, it searches for and opens a connected serial port with the VID, PID, and serial characteristics defined in __device_config.txt__. This ensures a reliable connection to a specific device.
You can also specify the device name defined in __device_config.txt__.
__baud__ Bitrate. Return value: 0 for success, 1 for failure. Data length, parity, stop bits, etc. are fixed at 8, N, 1.
Serial port initialization is performed within the Lua script.

Example:
~~~
SerialInit(0,10,115200)
or
SerialInit(0,A,115200)
or
SerialInit(0,"Insulation_Serial_1",115200)
~~~
Uppercase letters or device names are linked here to the serial port definition number, and the serial port definition number will be used from now on.

##### __SerialSend(ch, Buffer, len)__  
Arguments: __ch__ The channel number when the serial port is opened; fixed at 0 in this program. __Buffer__ The string to send (can be output in binary format without problems). __len__ The number of characters. This is a function for sending data via the serial port.

Example:
~~~
Send = string.format("%x \n",Count)
SerialSend(0,Send,string.len(Send))
~~~


##### __rtn = SerialWaitRecv(ch,Timeout)__
Arguments: __ch__ The channel number when the serial port was opened; fixed at 0 in this program.
__Timeout__ Waits for reception for the specified integer number of seconds.
Return value: 0 for reception, 1 for timeout
This function simply waits for a reception event and pauses processing; to find out what was received, you must call the SerialRecv() function separately.

##### __len,Buf = SerialRecv(ch,Buf)__
Arguments: __ch__ The channel number when the serial port was opened; fixed at 0 in this program.
__Buf__ Received string (even if it has content, it will be overwritten by the received string)
__len__ Number of characters received
This is a function for serial port reception.

Example:
~~~
len,Buf = SerialRecv(0,Buf)
if(len > 0) then
~~~

##### __Buf,len = SerialRecv2(ch)__
Argument: __ch__ Serial port definition number, can be specified from 0 to 9. This is specified during SerialInit.
__Buf__ Received string (defined here as a Lua string) __len__ Number of characters to receive
Example:
~~~
Buf,len = SerialRecv(0)
~~~
Buf is generated within the function and garbage collected after use.
Generally, the use of SerialRecv2 is recommended.

##### __rtn = HIDSend(ch,Buf,len)__
Argument: __ch__ Can be specified in the MDEV instrument definition. However, since it is difficult to specify this directly, it is recommended to specify it by the instrument name.  
__Buffer__ Transmission string __len__ Number of characters   __Buffer__ can transmit binary data including zeros.

##### __rtn = HIDSendS(ch,Buf)__
Argument: __ch__ Can be specified in the MDEV instrument definition. However, since it is difficult to specify this directly, it is recommended to specify it by the instrument name.  
__Buffer__ Transmission string This can be used if the transmission string does not contain zeros.

##### __rtn = HIDSendS3(ch,Buf)__
Argument: __ch__ Can be specified in the MDEV instrument definition. However, since it is difficult to specify this directly, it is recommended to specify it by the instrument name.  
__Buffer__ Transmission string This can be used if the transmission string does not contain zeros.

##### __len,Buf = HIDRecv(ch,Buffer,len)__
Argument: __ch__ Can be specified in the MDEV instrument definition. However, since it is difficult to specify this directly, it is recommended to specify it by the instrument name.  
__Buffer__ Received string __len__ Number of characters __Buffer__ can receive binary data including 0

##### __len,Buf = HIDRecv2(ch,Buffer,len)__
Argument: __ch__ Can be specified in the MDEV instrument definition. However, since it is difficult to specify this directly, it is recommended to specify it by the device name.  
__Buffer__ Received string __len__ Number of characters __Buffer__ can receive binary data including 0

##### __len,Buf = HIDRecv3(ch,Buffer,len)__
Argument: __ch__ Can be specified in the MDEV instrument definition. However, since it is difficult to specify this directly, it is recommended to specify it by the device name.  
__Buffer__ Received string __len__ Number of characters __Buffer__ can receive binary data including 0

##### __rtn = NetInitCheck(ch)__
Argument: __ch__ LAN connection instrument definition, can be specified from 0 to 4. Can also be specified by the device name.  
Return Value: 1 for initialized, 0 for uninitialized

##### __rtn = NetConnect(ch)__
Argument: __ch__ LAN connection instrument definition, can be specified from 0 to 4. Can also be specified by device name.  
Return Value: 1 for connected, 0 for not connected

##### __rtn = NetDisConnect(ch)__
Argument: __ch__ LAN connection instrument definition, can be specified from 0 to 4. Can also be specified by device name.  
Return Value: Always 0

##### __rtn = NetSend(ch,Buf,len)__
Argument: __ch__ LAN connection instrument definition, can be specified from 0 to 4. Can also be specified by device name.  
__Buf__ String to send  
__lan__ Number of characters to send  
Return Value: 1 for successful transmission, 0 for transmission failure  

##### __rtn = NetWaitRecv(ch,timeout)__
Argument: __ch__ LAN connection instrument definition  
The value can be specified from 0 to 4. It can also be specified by the device name.    
__timeout__ Waiting for reception Waits for __timeout__ seconds or until data is received  
Return value: 1 for timeout, 0 for reception  

##### __len,Buf = NetRecv(ch,Buf)__  
Argument: __ch__ LAN connection instrument definition, can be specified from 0 to 4. It can also be specified by the device name.
__Buf__ Received string
Return value __len__: Number of characters received, __Buf__: Received string

##### __Sleep(wait)__
Argument: __wait__ Pauses processing and waits for the specified integer number of seconds.
Example:
~~~
Sleep(10)
~~~
<br>

##### __uSleep(wait)__
Argument: __wait__ Pauses processing and waits for the specified integer number of microseconds.
Example:
~~~
uSleep(10)
~~~

<br>

##### __mSleep(wait)__
Argument: __wait__ Pauses processing and waits for the specified integer millisecond.

Example:
~~~
mSleep(10)
~~~

<br>

##### __Data = RegRead(RID,addr)__
Argument: __RID__ Region type __addr__ Register address Return value: Value in the register
This function accesses registers defined in the configuration file __Reg.txt__.  
Example:  
~~~
Send = string.char(MID,RegRead(0,ADDR),0,0,0,0,0,0,0,0,0,0,0)
~~~
This example directly generates a telemetry string.
The region type can be specified by its position in the order of region type definition (starting from 0), or by the region __REGION__ element name string.

##### __Data = RegRead("MAIN_MEM",Addr)__
The argument __addr__ can only be a number. If you want to access the address as a string, use a Lua table.
Example:
~~~
DefAddr ={
"CONTROL_REG" = 0x0100
"STATUS_REG" = 0x0104
}
Data = RegRead("REG_AREA","STATUS_REG")
~~~

<br>

##### __RegWrite(RID,addr,Data)__
Arguments: __RID__ Area type __addr__ Register address __Data__ Value to write to the register
This function accesses registers defined in the configuration file __Reg.txt__.  
Example:  
~~~
RegWrite(0,RADDR,string.byte(Buf,4,4))
~~~
The value to be written is directly extracted from the command string.  
The region type can be specified by the position in the order of region type definition (starting from 0) or by the region REGION element name string.

<br>

##### __RegWrite("MAIN_MEM",Addr,Data)__
The argument __addr__ can only be a number. If you want to access the address as a string, use a Lua table.  

<br>

##### __CRC = CRC16(BUF,len)__
Argument: Command/Telemetry Buffer. The last 2 bytes (len-2) are set to 0 before the CRC is inserted.  
Return value: __CRC__ unsigned short value  
For PCDU 8byte commands, len=8; for telemetry, len=385.   

<br>

##### __mkframe()__
Generates a fixed-length format telemetry binary.  
Arguments:  
- 1 String buffer for storing telemetry binary.  
- 2 Buffer size  
- 3 Lua table containing telemetry values
- Return value: String buffer for storing telemetry binary. Argument 1 itself.
The format is defined in the telemetry definition file defined in config.txt. If the definition name is not found, it searches for tlm.txt.

##### __rtn = DeviceSeq("device name","sequence name",param...)__
Arguments:
- "device name": Device name defined in device_config.txt
- "sequence name": Sequence name defined in the configuration pattern file defined in device_config.txt.
- param: Sequences may take parameters; use the values ​​written here.
- Return value: 0: Failure, 1: Success
Example:
~~~
DeviceSeq("SPE6102_1","SETV",param1)
~~~

<br>

##### __KeyWait()__
Pauses Lua execution and waits for a specific key input (return).
- Arguments: None
- Return value: None
The program calls and executes a Loop function in Lua every time a timer event occurs; this function stops that.

##### __SetC("variable name",flg,Value)__
Sets a value to a variable registered in the LM class.
Argument 1: Variable name
Argument 2: Type
Argument 3: Value
This passes the value from the Lua script to C++.
The variable name is defined in the register definition file after the DEF keyword.
Type: 0 Integer
Argument 1: Floating-point number

##### __Value = GetC("variable name",flg)__
Retrieves a value from a variable registered in the LM class.
Argument 1: Variable name
Argument 2: Type
The variable name is defined in the register definition file after the DEF keyword.
Argument 0 Integer
Argument 1: Floating-point number
Return value: Number; whether it's an integer or floating-point number is determined by argument 2.

::
B: The operation of the following function is hardcoded.
__function Startup()__
No arguments, no return value
Called only once when the program starts. Think of it as the same as the Arduino version.

__function Loop()__
This function is called at intervals of the milliseconds specified in __Interval__ in Config.txt (initial value is 1000).
Consider it the same as the Arduino function.
::
### 8.4: About Register Definition Files

The register definition file format allows you to specify the register type name after REGION_MAIN.

Example: __REGION REG {__

The __REGION__ element can be named, and its contents are written within {}.

The __REGION__ element can be accessed by number starting from 0 in the order of its definition as a region type.

The __REGION__ element has a memory space from the specified start address __START__ to the end address __END__. Its contents can be accessed in 16-bit units per word using address specification.

Access to the specified address is performed normally as long as it is between the start and end addresses.

The memory space is initialized to 0.

The __RegRead()__ and __RegWrite()__ functions allow access to these registers by specifying the region type and address.

For example, if you wanted to create an image capture trigger command, you could define a custom register in the register definition file and write to it to trigger the image capture.

You could write a Lua script that monitors this custom register every second and executes when a value is written to it.

### 8.5: About the Attached Sample Program

The attached Lua script __script.lua__ simulates the operation of a typical serial device.

First, the __Startup()__ function sets the initial values ​​of the registers. If the serial port initialization is successful, "__Init OK__" will be displayed on the screen.

The __Loop()__ function accepts input from the serial port, interprets it as a command, and returns telemetry if the read register is set. It also writes values ​​to the registers according to the command.

## 9: Display

The program displays data in a fixed screen size, divided into three sections.

The top section can have up to four fixed parameter display sections horizontally and up to three vertically. These sections can be defined and displayed using Lua scripts, and can display parameter labels, integer values, and floating-point numbers.

The number of display sections here can be changed using the __FIXParam_Display__ parameter in config.txt.

The middle section displays received data, and the display can be switched between hexadecimal and text display in config.txt. This cannot be changed while the program is running. Lua scripts can freely display text. If you want to display in hexadecimal using Lua, you must process the output in the program.

In hexadecimal display, ASCII characters are also displayed on the right edge. Characters that cannot be converted to ASCII are replaced with *.

The bottom six lines are the transmitted data display section, displayed in hexadecimal.


## 10: Post-Startup Settings

Port and baud rate settings can be changed after startup. If the port defined in Config.txt does not exist, the program will start with a dialog box indicating that the port could not be opened. The port will be displayed as COMX, indicating that the port is not connected.

If other connectable ports exist, clicking COMX will display a dropdown menu of selectable ports, allowing you to choose one.

Even if a port such as COM3 is already connected, clicking it will display a list of selectable ports, allowing you to select or change them.

You can change the baud rate by opening the dialog box from "Baud...".

## 11: Receiving
The program attempts to open the serial port set at startup. If it cannot be opened, a message will be displayed. The program will start but will not function, so terminate it and perform the correct settings.

If the program starts without an error message, the port is already open and in receiving mode. The port cannot be closed while the program is running.

In short, if configured correctly, it is almost operation-free.

## 12: Sending

This is done from "CMD..." in the program's menu bar. A dialog window will open. In the two edit boxes, enter the target board's I/O address and the data you want to write during the write operation, respectively, and then press the "WRITE" or "READ" button.

This operation requires support from the target board's software.

The expected operation of the target board is to write the specified data to the specified address, or to read one byte of data from the specified address and send it back to bincom via the serial port.

The transmitted command is 4 bytes, and the command format is as follows:

Position (bytes) Meaning Value
- 0 Command Type Operation type indication 0=Write 1=Read
- 1 Address Target board I/O address indication
- 2 Data During write operation

Write Data Instruction
- 3 Command End Always 0x00

When "Read" is specified as the command type, the second byte of data instruction is ignored.

Values ​​written to the edit box are interpreted as hexadecimal strings if prefixed with "0x" and as binary strings if prefixed with "0b". A string of half-width digits is treated as decimal.

If other characters such as letters are included, the default data definition will be used for transmission.

## 13: Recording

Clicking "SAVE" in the program's menu bar saves the received data to the same folder as the program binary. Simultaneously, the menu bar text changes to "NowSaving.."

All data received during this time is saved to the file. Clicking "NowSaving.." ends data saving.

The saved file name is automatically assigned a sequential filename. The recording start time is recorded at the beginning of the file, and the recording end time is recorded at the end.

Filenames are designed to be unique.

Select "CMD.." from the program's menu bar, then select "SaveSendData" at the bottom of the pull-down menu. The sent data will be saved to the fixed filename "SENDDATA.DAT" until this item is selected again.

While saving is in progress, the menu item will change to "NowSaving.."

## 14: License

This program is licensed under the MIT License.

## 15: Compilation

The program is compiled using Visual Studio 2022 C++ x64 Release.

When compiling from the attached source code, first compile the Lua source code to create the __LibLua.lib__ file. Extract the Lua source code into the lua folder created in the attached source folder.

The Lua source code can be downloaded from https://www.lua.org/download.html.

Building this program requires the installation of __libUSB__ and its libraries.

Please copy __libusb.h__ and __libusb-1.0.lib__ to the same folder as this source code before building the project.

## 16: Known Issues
Currently, no issues have been reported. If you notice any issues, please contact us at the address below.

## 17: Copyright Holder and Contact Information
Toru Mizuki | [twitter](https://twitter.com/mizuki_tohru)

