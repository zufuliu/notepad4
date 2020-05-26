@ECHO OFF
@rem used on AppVeyor and GitHub

IF /I "%~1" == "latest" (
  SHIFT
  curl -fsSL -o "LLVM-10.0.0-win64.exe" "https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/LLVM-10.0.0-win64.exe"
  LLVM-10.0.0-win64.exe /S
)

curl -fsSL -o "LLVM_VS2017.zip" "https://github.com/zufuliu/llvm-utils/releases/download/v20.03/LLVM_VS2017.zip"
7z x -y "LLVM_VS2017.zip" >NUL
CALL "LLVM_VS2017\install.bat" %1
