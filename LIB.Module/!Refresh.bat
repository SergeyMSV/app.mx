del /Q *.cpp
del /Q *.h

set LIB_MODULE_GNSS=%LIB_MODULE%.GNSS

xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnss.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver*

pause
