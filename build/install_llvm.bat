@ECHO OFF
@rem used on AppVeyor

curl -Ls -o "LLVM_VS2017.zip" "https://github.com/zufuliu/llvm-utils/releases/download/v20.03/LLVM_VS2017.zip"
7z x -y "LLVM_VS2017.zip" >NUL
CALL "LLVM_VS2017\install.bat" %1
