@echo off
SET "WDKBASEDIR=D:\Dev\WinDDK"
SET "PATH=%WDKBASEDIR%\bin;%PATH%"
SET "INCLUDE=%WDKBASEDIR%\inc\api;%WDKBASEDIR%\inc\stl70;%WDKBASEDIR%\inc\crt;%WDKBASEDIR%\inc\ddk"
SET "LIB=%WDKBASEDIR%\lib\crt\i386;%WDKBASEDIR%\lib\win7\i386"

if /I "%1" == "" (
	nmake /nologo /f metapath-vc7speed.mak "CFG=i386" build
) else (
	nmake /nologo /f metapath-vc7speed.mak "CFG=i386" %1
)