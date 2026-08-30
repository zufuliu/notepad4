@ECHO OFF
@rem used for AppVeyor and GitHub Actions

curl -fsSL -o "LLVM-23.1.0-win64.msi" "https://github.com/llvm/llvm-project/releases/download/llvmorg-23.1.0/LLVM-23.1.0-win64.msi"
msiexec.exe /quiet /passive /qn /i "LLVM-23.1.0-win64.msi"
reg add HKLM\SOFTWARE\LLVM\LLVM /t REG_SZ /d "C:\Program Files\LLVM\"
del "LLVM-23.1.0-win64.msi"

IF /I "%~1" == "latest" (
git clone -q --depth=1 --branch=main https://github.com/zufuliu/llvm-utils.git
CALL "llvm-utils\VS2017\install.bat" %2
) ELSE (
curl -fsSL -o "LLVM_VS2017.zip" "https://github.com/zufuliu/llvm-utils/releases/download/v26.08/LLVM_VS2017.zip"
7z x -y "LLVM_VS2017.zip" >NUL
CALL "LLVM_VS2017\install.bat" %1
)
