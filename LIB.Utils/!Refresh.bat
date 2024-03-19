del /Q *.cpp
del /Q *.h

xcopy /Y %LIB_UTILS%\utilsBase.*
xcopy /Y %LIB_UTILS%\utilsChrono.*
xcopy /Y %LIB_UTILS%\utilsCRC.*
xcopy /Y %LIB_UTILS%\utilsFile.*
xcopy /Y %LIB_UTILS%\utilsLinux.*
xcopy /Y %LIB_UTILS%\utilsLog.*
xcopy /Y %LIB_UTILS%\utilsPacket.*
xcopy /Y %LIB_UTILS%\utilsPacketCameraVC0706.*
xcopy /Y %LIB_UTILS%\utilsPacketNMEA.*
xcopy /Y %LIB_UTILS%\utilsPacketNMEAPayload.*
xcopy /Y %LIB_UTILS%\utilsPacketNMEAPayloadPTWS.*
xcopy /Y %LIB_UTILS%\utilsPacketNMEAType.*
xcopy /Y %LIB_UTILS%\utilsPacketStar.*
xcopy /Y %LIB_UTILS%\utilsPacketTWR.*
xcopy /Y %LIB_UTILS%\utilsPath.*
xcopy /Y %LIB_UTILS%\utilsPortSerial.*
xcopy /Y %LIB_UTILS%\utilsShell.*
xcopy /Y %LIB_UTILS%\utilsWeb.*

set ARCH_7z2201="7z2201"
rmdir /S /Q %ARCH_7z2201%
mkdir %ARCH_7z2201%
xcopy /Y /S %LIB_UTILS%\%ARCH_7z2201%\* %ARCH_7z2201%\
del /Q %ARCH_7z2201%\*_Test.cpp
rmdir /S /Q %ARCH_7z2201%\C\Util

pause
