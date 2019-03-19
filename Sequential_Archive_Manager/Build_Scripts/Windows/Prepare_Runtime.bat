@ECHO OFF
COLOR 0A

SET QT=Qt_5_12_2_MinGW_32_bit
SET MODE=Debug
SET SOURCE_LOCATION=F:\Documents\Source_Code

REM Prepare Folders
MKDIR "%SOURCE_LOCATION%\Sequential_Archive\build-Sequential_Archive_Manager-Desktop_%QT%-%MODE%\%MODE%\Plugins" >NUL 2>&1

REM Delete Old Plugins
ERASE "%SOURCE_LOCATION%\Sequential_Archive\build-Sequential_Archive_Manager-Desktop_%QT%-%MODE%\%MODE%\Plugins\Sequential_Archive.dll" >NUL 2>&1

REM Install New Plugins
COPY "%SOURCE_LOCATION%\Sequential_Archive\build-Sequential_Archive-Desktop_%QT%-%MODE%\%MODE%\Sequential_Archive.dll" "%SOURCE_LOCATION%\Sequential_Archive\build-Sequential_Archive_Manager-Desktop_%QT%-%MODE%\%MODE%\Plugins\Sequential_Archive.dll"
