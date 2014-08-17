@echo off
SET "WDKBASEDIR=D:\Dev\WinDDK"
SET "VCDIR=D:\Program Files\Microsoft Visual Studio 10.0\VC"
SET "PATH=%VCDIR%\bin;%PATH%"
SET "INCLUDE=%WDKBASEDIR%\inc\api;%WDKBASEDIR%\inc\stl70;%WDKBASEDIR%\inc\crt;%WDKBASEDIR%\inc\ddk"
SET "LIB=%WDKBASEDIR%\lib\crt\i386;%WDKBASEDIR%\lib\win7\i386"

if /I "%1" == "" (
	nmake /nologo /f makefile-vc10.mak build
) else (
	nmake /nologo /f makefile-vc10.mak %1
)
