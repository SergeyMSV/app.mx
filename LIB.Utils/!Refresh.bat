del /Q *.cpp
del /Q *.h

xcopy /Y %LIB_UTILS%\utilsBase.*
xcopy /Y %LIB_UTILS%\utilsLinux.*
xcopy /Y %LIB_UTILS%\utilsPath.*

pause
