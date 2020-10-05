@ECHO OFF
@rem used for GitHub Actions

@rem https://github.com/actions/virtual-environments/blob/main/images/win/Windows2019-Readme.md#msys2
SET PATH=C:\msys64\usr\bin;%PATH%;

@rem we don't need gdb or lldb, which saved the time to install Python and it's dependencies.

IF /I "%~1" == "x86_64" (
  SHIFT
  pacman -Sy --noconfirm --noprogressbar mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-tools-git mingw-w64-x86_64-clang mingw-w64-x86_64-lld
)

IF /I "%~1" == "i686" (
  SHIFT
  pacman -Sy --noconfirm --noprogressbar mingw-w64-i686-gcc mingw-w64-i686-make mingw-w64-i686-tools-git mingw-w64-i686-clang mingw-w64-i686-lld
)

IF /I "%~1" == "llvm" (
  SHIFT
  curl -fsSL -o "llvm-mingw-20200325-x86_64.zip" "https://github.com/mstorsjo/llvm-mingw/releases/download/20200325/llvm-mingw-20200325-x86_64.zip"
  7z x -y -o"C:\" "llvm-mingw-20200325-x86_64.zip" >NUL
)
