@ECHO OFF
@rem ******************************************************************************
@rem *
@rem * Notepad2-mod
@rem *
@rem * build_vs2017.bat
@rem *   Batch file used to build Notepad2 with MSVC 2017, 2019
@rem *
@rem * See License.txt for details about distribution and modification.
@rem *
@rem *                                     (c) XhmikosR 2010-2015, 2017
@rem *                                     https://github.com/XhmikosR/Notepad2-mod
@rem *
@rem ******************************************************************************

SETLOCAL ENABLEEXTENSIONS
CD /D %~dp0

@rem Check for the help switches
IF /I "%~1" == "help"   GOTO SHOWHELP
IF /I "%~1" == "/help"  GOTO SHOWHELP
IF /I "%~1" == "-help"  GOTO SHOWHELP
IF /I "%~1" == "--help" GOTO SHOWHELP
IF /I "%~1" == "/?"     GOTO SHOWHELP

@rem default arguments
SET "BUILDTYPE=Build"
SET "ARCH=all"
SET "CONFIG=Release"

@rem Check for the first switch
IF "%~1" == "" GOTO START_WORK
IF /I "%~1" == "Build"     SET "BUILDTYPE=Build"   & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "/Build"    SET "BUILDTYPE=Build"   & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "-Build"    SET "BUILDTYPE=Build"   & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "--Build"   SET "BUILDTYPE=Build"   & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "Clean"     SET "BUILDTYPE=Clean"   & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "/Clean"    SET "BUILDTYPE=Clean"   & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "-Clean"    SET "BUILDTYPE=Clean"   & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "--Clean"   SET "BUILDTYPE=Clean"   & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "Rebuild"   SET "BUILDTYPE=Rebuild" & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "/Rebuild"  SET "BUILDTYPE=Rebuild" & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "-Rebuild"  SET "BUILDTYPE=Rebuild" & SHIFT & GOTO CHECKSECONDARG
IF /I "%~1" == "--Rebuild" SET "BUILDTYPE=Rebuild" & SHIFT & GOTO CHECKSECONDARG


:CHECKSECONDARG
@rem Check for the second switch
IF "%~1" == "" GOTO START_WORK
IF /I "%~1" == "x86"     SET "ARCH=Win32" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "/x86"    SET "ARCH=Win32" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "-x86"    SET "ARCH=Win32" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "--x86"   SET "ARCH=Win32" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "Win32"   SET "ARCH=Win32" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "/Win32"  SET "ARCH=Win32" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "-Win32"  SET "ARCH=Win32" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "--Win32" SET "ARCH=Win32" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "x64"     SET "ARCH=x64"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "/x64"    SET "ARCH=x64"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "-x64"    SET "ARCH=x64"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "--x64"   SET "ARCH=x64"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "AVX2"    SET "ARCH=AVX2"  & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "/AVX2"   SET "ARCH=AVX2"  & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "-AVX2"   SET "ARCH=AVX2"  & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "--AVX2"  SET "ARCH=AVX2"  & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "ARM64"   SET "ARCH=ARM64" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "/ARM64"  SET "ARCH=ARM64" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "-ARM64"  SET "ARCH=ARM64" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "--ARM64" SET "ARCH=ARM64" & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "ARM"     SET "ARCH=ARM"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "/ARM"    SET "ARCH=ARM"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "-ARM"    SET "ARCH=ARM"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "--ARM"   SET "ARCH=ARM"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "all"     SET "ARCH=all"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "/all"    SET "ARCH=all"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "-all"    SET "ARCH=all"   & SHIFT & GOTO CHECKTHIRDARG
IF /I "%~1" == "--all"   SET "ARCH=all"   & SHIFT & GOTO CHECKTHIRDARG


:CHECKTHIRDARG
@rem Check for the third switch
IF "%~1" == "" GOTO START_WORK
IF /I "%~1" == "Debug"         SET "CONFIG=Debug"       & SHIFT & GOTO START_WORK
IF /I "%~1" == "/Debug"        SET "CONFIG=Debug"       & SHIFT & GOTO START_WORK
IF /I "%~1" == "-Debug"        SET "CONFIG=Debug"       & SHIFT & GOTO START_WORK
IF /I "%~1" == "--Debug"       SET "CONFIG=Debug"       & SHIFT & GOTO START_WORK
IF /I "%~1" == "Release"       SET "CONFIG=Release"     & SHIFT & GOTO START_WORK
IF /I "%~1" == "/Release"      SET "CONFIG=Release"     & SHIFT & GOTO START_WORK
IF /I "%~1" == "-Release"      SET "CONFIG=Release"     & SHIFT & GOTO START_WORK
IF /I "%~1" == "--Release"     SET "CONFIG=Release"     & SHIFT & GOTO START_WORK
IF /I "%~1" == "LLVMDebug"     SET "CONFIG=LLVMDebug"   & SHIFT & GOTO START_WORK
IF /I "%~1" == "/LLVMDebug"    SET "CONFIG=LLVMDebug"   & SHIFT & GOTO START_WORK
IF /I "%~1" == "-LLVMDebug"    SET "CONFIG=LLVMDebug"   & SHIFT & GOTO START_WORK
IF /I "%~1" == "--LLVMDebug"   SET "CONFIG=LLVMDebug"   & SHIFT & GOTO START_WORK
IF /I "%~1" == "LLVMRelease"   SET "CONFIG=LLVMRelease" & SHIFT & GOTO START_WORK
IF /I "%~1" == "/LLVMRelease"  SET "CONFIG=LLVMRelease" & SHIFT & GOTO START_WORK
IF /I "%~1" == "-LLVMRelease"  SET "CONFIG=LLVMRelease" & SHIFT & GOTO START_WORK
IF /I "%~1" == "--LLVMRelease" SET "CONFIG=LLVMRelease" & SHIFT & GOTO START_WORK
IF /I "%~1" == "all"           SET "CONFIG=all"         & SHIFT & GOTO START_WORK
IF /I "%~1" == "/all"          SET "CONFIG=all"         & SHIFT & GOTO START_WORK
IF /I "%~1" == "-all"          SET "CONFIG=all"         & SHIFT & GOTO START_WORK
IF /I "%~1" == "--all"         SET "CONFIG=all"         & SHIFT & GOTO START_WORK


:START_WORK
SET "EXIT_ON_ERROR=%~1"

SET NEED_ARM64=0
SET NEED_ARM=0
IF /I "%ARCH%" == "AVX2" (
	SET "ARCH=x64"
	IF /I NOT "%CONFIG%" == "all" SET "CONFIG=AVX2%CONFIG%"
)
IF /I "%ARCH%" == "all" SET NEED_ARM64=1
IF /I "%ARCH%" == "ARM64" SET NEED_ARM64=1
IF /I "%ARCH%" == "all" SET NEED_ARM=1
IF /I "%ARCH%" == "ARM" SET NEED_ARM=1
CALL :SubVSPath
IF NOT EXIST "%VS_PATH%" CALL :SUBMSG "ERROR" "Visual Studio 2017 or 2019 NOT FOUND, please check VS_PATH environment variable!"

IF /I "%processor_architecture%"=="AMD64" (
	SET "HOST_ARCH=amd64"
) ELSE (
	SET "HOST_ARCH=x86"
)

IF /I "%ARCH%" == "x64" GOTO x64
IF /I "%ARCH%" == "Win32" GOTO Win32
IF /I "%ARCH%" == "ARM64" GOTO ARM64
IF /I "%ARCH%" == "ARM" GOTO ARM


:Win32
CALL "%VS_PATH%\Common7\Tools\vsdevcmd" -no_logo -arch=x86 -host_arch=%HOST_ARCH%
IF /I "%CONFIG%" == "all" (CALL :SUBMSVC %BUILDTYPE% Debug Win32 && CALL :SUBMSVC %BUILDTYPE% Release Win32) ELSE (CALL :SUBMSVC %BUILDTYPE% %CONFIG% Win32)
IF /I "%ARCH%" == "Win32" GOTO END


:x64
CALL "%VS_PATH%\Common7\Tools\vsdevcmd" -no_logo -arch=amd64 -host_arch=%HOST_ARCH%
IF /I "%CONFIG%" == "all" (CALL :SUBMSVC %BUILDTYPE% Debug x64 && CALL :SUBMSVC %BUILDTYPE% Release x64) ELSE (CALL :SUBMSVC %BUILDTYPE% %CONFIG% x64)
IF /I "%ARCH%" == "x64" GOTO END


:ARM64
CALL "%VS_PATH%\Common7\Tools\vsdevcmd" -no_logo -arch=arm64 -host_arch=%HOST_ARCH%
IF /I "%CONFIG%" == "all" (CALL :SUBMSVC %BUILDTYPE% Debug ARM64 && CALL :SUBMSVC %BUILDTYPE% Release ARM64) ELSE (CALL :SUBMSVC %BUILDTYPE% %CONFIG% ARM64)
IF /I "%ARCH%" == "ARM64" GOTO END


:ARM
CALL "%VS_PATH%\Common7\Tools\vsdevcmd" -no_logo -arch=arm -host_arch=%HOST_ARCH%
IF /I "%CONFIG%" == "all" (CALL :SUBMSVC %BUILDTYPE% Debug ARM && CALL :SUBMSVC %BUILDTYPE% Release ARM) ELSE (CALL :SUBMSVC %BUILDTYPE% %CONFIG% ARM)


:END
TITLE Building Notepad2 with MSVC2017 - Finished!
ENDLOCAL
EXIT /B


:SubVSPath
@rem Check the building environment
@rem VSINSTALLDIR is set by vsdevcmd_start.bat
IF EXIST "%VSINSTALLDIR%\Common7\IDE\VC\VCTargets\Platforms\%ARCH%\PlatformToolsets" (
	SET "VS_PATH=%VSINSTALLDIR%"
	EXIT /B
)

SET VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
SET "VS_COMPONENT=Microsoft.Component.MSBuild Microsoft.VisualStudio.Component.VC.Tools.x86.x64"
IF "%NEED_ARM64%" == 1 SET "VS_COMPONENT=%VS_COMPONENT% Microsoft.VisualStudio.Component.VC.Tools.ARM64"
IF "%NEED_ARM%" == 1 SET "VS_COMPONENT=%VS_COMPONENT% Microsoft.VisualStudio.Component.VC.Tools.ARM"
FOR /f "delims=" %%A IN ('"%VSWHERE%" -property installationPath -prerelease -version [15.0^,17.0^) -requires %VS_COMPONENT%') DO SET "VS_PATH=%%A"
IF EXIST "%VS_PATH%" SET "VSINSTALLDIR=%VS_PATH%\"
EXIT /B


:SUBMSVC
ECHO.
TITLE Building Notepad2 with MSVC2017 - %~1 "%~2|%~3"...
CD /D %~dp0
"MSBuild.exe" /nologo Notepad2.sln /target:Notepad2;%~1 /property:Configuration=%~2;Platform=%~3^ /consoleloggerparameters:Verbosity=minimal /maxcpucount /nodeReuse:true
IF %ERRORLEVEL% NEQ 0 CALL :SUBMSG "ERROR" "Compilation failed!"
EXIT /B


:SHOWHELP
TITLE %~nx0 %1
ECHO. & ECHO.
ECHO Usage: %~nx0 [Clean^|Build^|Rebuild] [Win32^|x64^|AVX2^|ARM64^|ARM^|all] [Debug^|Release^|LLVMDebug^|LLVMRelease^|all]
ECHO.
ECHO Notes: You can also prefix the commands with "-", "--" or "/".
ECHO        The arguments are not case sensitive.
ECHO. & ECHO.
ECHO Executing %~nx0 without any arguments is equivalent to "%~nx0 build all release"
ECHO.
ECHO If you skip the second argument the default one will be used.
ECHO The same goes for the third argument. Examples:
ECHO "%~nx0 rebuild" is the same as "%~nx0 rebuild all release"
ECHO "%~nx0 rebuild Win32" is the same as "%~nx0 rebuild Win32 release"
ECHO.
ECHO WARNING: "%~nx0 Win32" or "%~nx0 debug" won't work.
ECHO.
ENDLOCAL
EXIT /B


:SUBMSG
ECHO. & ECHO ______________________________
ECHO [%~1] %~2
ECHO ______________________________ & ECHO.
IF /I "%~1" == "ERROR" (
  IF "%EXIT_ON_ERROR%" == "" PAUSE
  EXIT /B
) ELSE (
  EXIT /B
)
