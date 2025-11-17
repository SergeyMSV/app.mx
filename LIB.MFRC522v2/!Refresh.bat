del /Q *.cpp
del /Q *.h

xcopy /Y %LIB_MFRC522v2%\*.*
xcopy /Y %LIB_MFRC522v2%\..\Arduino.*

pause
