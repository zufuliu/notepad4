@ECHO OFF
@rem ******************************************************************************
@rem *
@rem * Notepad2-mod
@rem *
@rem * make_zip.bat
@rem *   Batch file for creating the zip packages
@rem *
@rem * See License.txt for details about distribution and modification.
@rem *
@rem *                                     (c) XhmikosR 2010-2017
@rem *                                     https://github.com/XhmikosR/notepad2-mod
@rem *
@rem ******************************************************************************

SETLOCAL ENABLEEXTENSIONS
CD /D %~dp0

SET "EXIT_ON_ERROR=%~4"

@rem Check for the help switches
IF /I "%~1" == "help"   GOTO SHOWHELP
IF /I "%~1" == "/help"  GOTO SHOWHELP
IF /I "%~1" == "-help"  GOTO SHOWHELP
IF /I "%~1" == "--help" GOTO SHOWHELP
IF /I "%~1" == "/?"     GOTO SHOWHELP


@rem Check for the first switch
IF "%~1" == "" (
  SET "COMPILER=MSVC"
) ELSE (
  IF /I "%~1" == "MSVC"         SET "COMPILER=MSVC"     & GOTO CHECKSECONDARG
  IF /I "%~1" == "/MSVC"        SET "COMPILER=MSVC"     & GOTO CHECKSECONDARG
  IF /I "%~1" == "-MSVC"        SET "COMPILER=MSVC"     & GOTO CHECKSECONDARG
  IF /I "%~1" == "--MSVC"       SET "COMPILER=MSVC"     & GOTO CHECKSECONDARG
  IF /I "%~1" == "GCC"          SET "COMPILER=GCC"      & GOTO CHECKSECONDARG
  IF /I "%~1" == "/GCC"         SET "COMPILER=GCC"      & GOTO CHECKSECONDARG
  IF /I "%~1" == "-GCC"         SET "COMPILER=GCC"      & GOTO CHECKSECONDARG
  IF /I "%~1" == "--GCC"        SET "COMPILER=GCC"      & GOTO CHECKSECONDARG
  IF /I "%~1" == "Clang"        SET "COMPILER=Clang"    & GOTO CHECKSECONDARG
  IF /I "%~1" == "/Clang"       SET "COMPILER=Clang"    & GOTO CHECKSECONDARG
  IF /I "%~1" == "-Clang"       SET "COMPILER=Clang"    & GOTO CHECKSECONDARG
  IF /I "%~1" == "--Clang"      SET "COMPILER=Clang"    & GOTO CHECKSECONDARG
  IF /I "%~1" == "LLVM"         SET "COMPILER=Clang"    & GOTO CHECKSECONDARG
  IF /I "%~1" == "/LLVM"        SET "COMPILER=Clang"    & GOTO CHECKSECONDARG
  IF /I "%~1" == "-LLVM"        SET "COMPILER=Clang"    & GOTO CHECKSECONDARG
  IF /I "%~1" == "--LLVM"       SET "COMPILER=Clang"    & GOTO CHECKSECONDARG

  ECHO.
  ECHO Unsupported commandline switch!
  ECHO Run "%~nx0 help" for details about the commandline switches.
  CALL :SUBMSG "ERROR" "Compilation failed!"
)

:CHECKSECONDARG
@rem Check for the second switch
IF "%~2" == "" (
  SET "ARCH=all"
) ELSE (
  IF /I "%~2" == "x86"   SET "ARCH=Win32" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "/x86"  SET "ARCH=Win32" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "-x86"  SET "ARCH=Win32" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "--x86" SET "ARCH=Win32" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "Win32"   SET "ARCH=Win32" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "/Win32"  SET "ARCH=Win32" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "-Win32"  SET "ARCH=Win32" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "--Win32" SET "ARCH=Win32" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "x64"   SET "ARCH=x64" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "/x64"  SET "ARCH=x64" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "-x64"  SET "ARCH=x64" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "--x64" SET "ARCH=x64" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "AVX2"   SET "ARCH=AVX2" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "/AVX2"  SET "ARCH=AVX2" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "-AVX2"  SET "ARCH=AVX2" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "--AVX2" SET "ARCH=AVX2" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "ARM64"   SET "ARCH=ARM64" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "/ARM64"  SET "ARCH=ARM64" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "-ARM64"  SET "ARCH=ARM64" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "--ARM64" SET "ARCH=ARM64" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "ARM"   SET "ARCH=ARM" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "/ARM"  SET "ARCH=ARM" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "-ARM"  SET "ARCH=ARM" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "--ARM" SET "ARCH=ARM" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "all"   SET "ARCH=all" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "/all"  SET "ARCH=all" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "-all"  SET "ARCH=all" & GOTO CHECKTHIRDARG
  IF /I "%~2" == "--all" SET "ARCH=all" & GOTO CHECKTHIRDARG

  ECHO.
  ECHO Unsupported commandline switch!
  ECHO Run "%~nx0 help" for details about the commandline switches.
  CALL :SUBMSG "ERROR" "Compilation failed!"
)


:CHECKTHIRDARG
@rem Check for the third switch
IF "%~3" == "" (
  SET "CONFIG=Release"
) ELSE (
  IF /I "%~3" == "Release"      SET "CONFIG=Release"    & GOTO START
  IF /I "%~3" == "/Release"     SET "CONFIG=Release"    & GOTO START
  IF /I "%~3" == "-Release"     SET "CONFIG=Release"    & GOTO START
  IF /I "%~3" == "--Release"    SET "CONFIG=Release"    & GOTO START
  IF /I "%~3" == "Debug"        SET "CONFIG=Debug"      & GOTO START
  IF /I "%~3" == "/Debug"       SET "CONFIG=Debug"      & GOTO START
  IF /I "%~3" == "-Debug"       SET "CONFIG=Debug"      & GOTO START
  IF /I "%~3" == "--Debug"      SET "CONFIG=Debug"      & GOTO START

  ECHO.
  ECHO Unsupported commandline switch!
  ECHO Run "%~nx0 help" for details about the commandline switches.
  CALL :SUBMSG "ERROR" "Compilation failed!"
)

:START
CALL :SubGetVersion
CALL :SubDetectSevenzipPath

IF /I "%SEVENZIP%" == "" CALL :SUBMSG "ERROR" "7za wasn't found!"

IF /I "%COMPILER%" == "GCC" GOTO GCC

:MSVC
IF /I "%COMPILER%" == "Clang" SET "CONFIG=LLVM%CONFIG%"
SET INPUTDIR_AVX2=bin\%CONFIG%\AVX2
SET INPUTDIR_x64=bin\%CONFIG%\x64
SET INPUTDIR_Win32=bin\%CONFIG%\Win32
SET INPUTDIR_ARM64=bin\%CONFIG%\ARM64
SET INPUTDIR_ARM=bin\%CONFIG%\ARM

IF /I "%ARCH%" == "AVX2" GOTO MSVC_AVX2
IF /I "%ARCH%" == "x64" GOTO MSVC_x64
IF /I "%ARCH%" == "Win32" GOTO MSVC_Win32
IF /I "%ARCH%" == "ARM64" GOTO MSVC_ARM64
IF /I "%ARCH%" == "ARM" GOTO MSVC_ARM

:MSVC_AVX2
IF EXIST "%INPUTDIR_AVX2%" CALL :SubZipFiles %INPUTDIR_AVX2% AVX2
IF /I "%ARCH%" == "AVX2" GOTO END_MSVC

:MSVC_x64
IF EXIST "%INPUTDIR_x64%" CALL :SubZipFiles %INPUTDIR_x64% x64
IF /I "%ARCH%" == "x64" GOTO END_MSVC

:MSVC_Win32
IF EXIST "%INPUTDIR_Win32%" CALL :SubZipFiles %INPUTDIR_Win32% Win32
IF /I "%ARCH%" == "Win32" GOTO END_MSVC

:MSVC_ARM64
IF EXIST "%INPUTDIR_ARM64%" CALL :SubZipFiles %INPUTDIR_ARM64% ARM64

:MSVC_ARM
IF EXIST "%INPUTDIR_ARM%" CALL :SubZipFiles %INPUTDIR_ARM% ARM

:END_MSVC
TITLE Make ZIP For %COMPILER% %ARCH% %CONFIG% Finished!
ECHO.
ENDLOCAL
EXIT /B


:GCC
SET INPUTDIR_AVX2=bin\%CONFIG%_AVX2
SET INPUTDIR_x64=bin\%CONFIG%_x64
SET INPUTDIR_Win32=bin\%CONFIG%_Win32

IF /I "%ARCH%" == "AVX2" GOTO GCC_AVX2
IF /I "%ARCH%" == "x64" GOTO GCC_x64
IF /I "%ARCH%" == "Win32" GOTO GCC_Win32

:GCC_AVX2
IF EXIST "%INPUTDIR_AVX2%" CALL :SubZipFiles %INPUTDIR_AVX2% AVX2
IF /I "%ARCH%" == "AVX2" GOTO END_GCC

:GCC_x64
IF EXIST "%INPUTDIR_x64%" CALL :SubZipFiles %INPUTDIR_x64% x64
IF /I "%ARCH%" == "x64" GOTO END_GCC

:GCC_Win32
IF EXIST "%INPUTDIR_Win32%" CALL :SubZipFiles %INPUTDIR_Win32% Win32

:END_GCC
TITLE Make ZIP For %COMPILER% %ARCH% %CONFIG% Finished!
ECHO.
ENDLOCAL
EXIT /B


:SubZipFiles
IF NOT EXIST "%1\Notepad2.exe" CALL :SUBMSG "ERROR" "%1\Notepad2.exe NOT found"
IF NOT EXIST "%1\metapath.exe" CALL :SUBMSG "ERROR" "%1\metapath.exe NOT found"

IF /I "%COMPILER%" == "MSVC" (
  SET "ZIP_NAME=Notepad2_%2_%NP2_VER%"
) ELSE (
  SET "ZIP_NAME=Notepad2_%COMPILER%_%2_%NP2_VER%"
)
TITLE Creating %ZIP_NAME%.zip...
CALL :SUBMSG "INFO" "Creating %ZIP_NAME%.zip..."

SET "TEMP_ZIP_DIR=temp_zip_dir"
IF EXIST "%TEMP_ZIP_DIR%"     RD /S /Q "%TEMP_ZIP_DIR%"
IF NOT EXIST "%TEMP_ZIP_DIR%" MD "%TEMP_ZIP_DIR%"

FOR %%A IN ( "..\License.txt"  "%1\Notepad2.exe"  "%1\metapath.exe" "..\doc\Notepad2.ini" "..\doc\Notepad2.reg" "..\metapath\doc\metapath.ini"
) DO COPY /Y /B /V "%%A" "%TEMP_ZIP_DIR%\"
COPY /Y /B /V "..\doc\Notepad2 DarkTheme.ini" "%TEMP_ZIP_DIR%\"

PUSHD "%TEMP_ZIP_DIR%"
"%SEVENZIP%" a -tzip -mx=9 "../%ZIP_NAME%.zip" "License.txt" "Notepad2.exe" "metapath.exe" "Notepad2.ini" "Notepad2 DarkTheme.ini" "Notepad2.reg" "metapath.ini" >NUL
POPD

IF %ERRORLEVEL% NEQ 0 CALL :SUBMSG "ERROR" "Compilation failed!"
CALL :SUBMSG "INFO" "%ZIP_NAME%.zip created successfully!"

IF EXIST "%TEMP_ZIP_DIR%"     RD /S /Q "%TEMP_ZIP_DIR%"
EXIT /B


:SubDetectSevenzipPath
FOR %%G IN (7z.exe) DO (SET "SEVENZIP_PATH=%%~$PATH:G")
IF EXIST "%SEVENZIP_PATH%" (SET "SEVENZIP=%SEVENZIP_PATH%" & EXIT /B)

FOR %%G IN (7za.exe) DO (SET "SEVENZIP_PATH=%%~$PATH:G")
IF EXIST "%SEVENZIP_PATH%" (SET "SEVENZIP=%SEVENZIP_PATH%" & EXIT /B)

FOR /F "tokens=2*" %%A IN (
  'REG QUERY "HKLM\SOFTWARE\7-Zip" /v "Path" 2^>NUL ^| FIND "REG_SZ" ^|^|
   REG QUERY "HKLM\SOFTWARE\Wow6432Node\7-Zip" /v "Path" 2^>NUL ^| FIND "REG_SZ"') DO SET "SEVENZIP=%%B\7z.exe"
EXIT /B


:SubGetVersion
rem Get the version
FOR /F "tokens=3,4" %%K IN (
  'FINDSTR /I /L /C:"define VERSION_MAJOR" "..\src\Version.h"') DO (SET "VerMajor=%%K")
FOR /F "tokens=3,4" %%K IN (
  'FINDSTR /I /L /C:"define VERSION_MINOR" "..\src\VersionRev.h"') DO (SET "VerMinor=%%K")
FOR /F "tokens=3,4" %%K IN (
  'FINDSTR /I /L /C:"define VERSION_BUILD" "..\src\VersionRev.h"') DO (SET "VerBuild=%%K")
FOR /F "tokens=3,4" %%K IN (
  'FINDSTR /I /L /C:"define VERSION_REV " "..\src\VersionRev.h"') DO (SET "VerRev=%%K")

SET NP2_VER=v%VerMajor%.%VerMinor%.%VerBuild%r%VerRev%
EXIT /B


:SHOWHELP
TITLE %~nx0 %1
ECHO. & ECHO.
ECHO Usage:  %~nx0 [MSVC^|GCC^|Clang^|LLVM] [Win32^|x64^|AVX2^|ARM64^|ARM^|all] [Release^|Debug]
ECHO.
ECHO Notes:  You can also prefix the commands with "-", "--" or "/".
ECHO         The arguments are not case sensitive.
ECHO. & ECHO.
ECHO Executing %~nx0 without any arguments is equivalent to "%~nx0 MSVC Release all"
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
