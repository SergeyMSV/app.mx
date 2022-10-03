del /Q *.cpp
del /Q *.h

xcopy /Y %LIB_UTILS%\utilsBase.*
xcopy /Y %LIB_UTILS%\utilsChrono.*
xcopy /Y %LIB_UTILS%\utilsLinux.*
xcopy /Y %LIB_UTILS%\utilsPath.*
xcopy /Y %LIB_UTILS%\utilsWeb.*

pause
