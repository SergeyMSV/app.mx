del /Q *.cpp
del /Q *.h

set LIB_MODULE_CAM=%LIB_MODULE%.CameraVC0706

xcopy /Y %LIB_MODULE_CAM%\LIB.Module\modCameraVC0706*

set LIB_MODULE_GNSS=%LIB_MODULE%.GNSS

xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnss.*
xcopy /Y %LIB_MODULE_GNSS%\LIB.Module\modGnssReceiver*

pause
