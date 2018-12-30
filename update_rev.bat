@ECHO OFF
SETLOCAL

SET "EXIT_ON_ERROR=%~1"

PUSHD %~dp0

IF EXIST "build.user.bat" (CALL "build.user.bat")

SET PATH=%MSYS%\bin;%PATH%

FOR %%G IN (bash.exe) DO (SET FOUND=%%~$PATH:G)
IF NOT DEFINED FOUND GOTO MissingVar

bash.exe ./version.sh 1


:END
POPD
ENDLOCAL
EXIT /B


:MissingVar
COLOR 0C
TITLE ERROR
ECHO MSYS or MSYS2 (bash.exe) wasn't found. Create a file build.user.bat and set the variable there.
ECHO.
ECHO SET "MSYS=C:\MSYS64"
ECHO. & ECHO.
ECHO Press any key to exit...
IF "%EXIT_ON_ERROR%" == "" PAUSE
ENDLOCAL
EXIT /B
