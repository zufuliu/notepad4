@ECHO OFF
@rem used for AppVeyor and GitHub Actions

IF NOT EXIST tools (
  MKDIR tools
)

IF NOT EXIST "tools/LLVM-12.0.0-win64.exe" (
  curl -fsSL -o "tools/LLVM-12.0.0-win64.exe" "https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.0/LLVM-12.0.0-win64.exe"
)
"tools/LLVM-12.0.0-win64.exe" /S

IF NOT EXIST "tools/LLVM_VS2017-v20.03.zip" (
  curl -fsSL -o "tools/LLVM_VS2017-v20.03.zip" "https://github.com/zufuliu/llvm-utils/releases/download/v20.03/LLVM_VS2017.zip"
)
7z x -y -o"." "tools/LLVM_VS2017-v20.03.zip" >NUL
CALL "LLVM_VS2017\install.bat" %1
