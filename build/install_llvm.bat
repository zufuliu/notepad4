@ECHO OFF
@rem used for AppVeyor and GitHub Actions

curl -fsSL -o "LLVM-13.0.0-win64.exe" "https://github.com/llvm/llvm-project/releases/download/llvmorg-13.0.0/LLVM-13.0.0-win64.exe"
"LLVM-13.0.0-win64.exe" /S

IF /I "%~1" == "latest" (
git clone -q --depth=1 --branch=main https://github.com/zufuliu/llvm-utils.git
CALL "llvm-utils\VS2017\install.bat" %2
) ELSE (
curl -fsSL -o "LLVM_VS2017.zip" "https://github.com/zufuliu/llvm-utils/releases/download/v21.11/LLVM_VS2017.zip"
7z x -y "LLVM_VS2017.zip" >NUL
CALL "LLVM_VS2017\install.bat" %1
)
