@ECHO OFF
COLOR 0A

SET QT=Qt_6_7_3_MinGW_64_bit
SET MODE=%1
IF %1.==. SET MODE=Debug
SET SOURCE_LOCATION=D:\Documents\Source_Code

REM Prepare Folders
MKDIR "%SOURCE_LOCATION%\Sequential_Archive\Sequential_Archive_Manager\build\Desktop_%QT%-%MODE%\Plugins" >NUL 2>&1

REM Delete Old Plugins
ERASE "%SOURCE_LOCATION%\Sequential_Archive\Sequential_Archive_Manager\build\Desktop_%QT%-%MODE%\Plugins\Sequential_Archive.dll" >NUL 2>&1

REM Install New Plugins
COPY "%SOURCE_LOCATION%\Sequential_Archive\Sequential_Archive\build\Desktop_%QT%-%MODE%\libSequential_Archive.dll" "%SOURCE_LOCATION%\Sequential_Archive\Sequential_Archive_Manager\build\Desktop_%QT%-%MODE%\Plugins\Sequential_Archive.dll"
