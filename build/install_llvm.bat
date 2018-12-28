@ECHO OFF
@rem used on AppVeyor

curl -Ls -o "llvm-utils-master.zip" "https://github.com/zufuliu/llvm-utils/archive/master.zip"
7z x -y "llvm-utils-master.zip" >NUL
CALL "llvm-utils-master\VS2017\install.bat" %1
