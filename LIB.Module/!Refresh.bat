del /Q *.cpp
del /Q *.h

set LIB_MODULE_GNSS=%LIB_MODULE%.GNSS

xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnss.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_State.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_State_CmdGPI.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_State_CmdGPO.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_State_CmdREQ.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_StateError.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_StateHalt.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_StateOperation.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_StateStart.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver_StateStop.*

pause
