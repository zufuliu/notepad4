@ECHO OFF
@rem used for AppVeyor and GitHub Actions
SETLOCAL ENABLEEXTENSIONS

IF /I "%~1" == "latest" (
  SHIFT
  SET "FILE=LLVM-11.0.0-win64.exe"
  curl -fsSL -o "%FILE%" "https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/%FILE%"
  "%FILE%" /S
)

SET "FILE=LLVM_VS2017.zip"
curl -fsSL -o "%FILE%" "https://github.com/zufuliu/llvm-utils/releases/download/v20.03/%FILE%"
7z x -y "%FILE%" >NUL
CALL "LLVM_VS2017\install.bat" %1
