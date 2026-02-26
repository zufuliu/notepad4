@ECHO OFF
@rem used for GitHub Actions

@rem https://github.com/actions/runner-images/blob/main/images/windows/Windows2025-Readme.md#msys2
@rem https://github.com/actions/runner-images/blob/main/images/windows/Windows2022-Readme.md#msys2
SET "PATH=C:\msys64\usr\bin;%PATH%"

@rem we don't need gdb or lldb, which saved the time to install Python and it's dependencies.

IF /I "%~1" == "ucrt" (
  IF /I "%~2" == "Clang" (
    pacman -S --needed --noconfirm --noprogressbar mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-tools-git mingw-w64-ucrt-x86_64-clang mingw-w64-ucrt-x86_64-lld mingw-w64-ucrt-x86_64-llvm-tools
  ) ELSE (
    pacman -S --needed --noconfirm --noprogressbar mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-tools-git
  )
) ELSE IF /I "%~1" == "x86_64" (
  IF /I "%~2" == "Clang" (
    pacman -S --needed --noconfirm --noprogressbar mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-tools-git mingw-w64-x86_64-clang mingw-w64-x86_64-lld mingw-w64-x86_64-llvm-tools
  ) ELSE (
    pacman -S --needed --noconfirm --noprogressbar mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-tools-git
  )
) ELSE IF /I "%~1" == "i686" (
  IF /I "%~2" == "Clang" (
    pacman -S --needed --noconfirm --noprogressbar mingw-w64-i686-gcc mingw-w64-i686-make mingw-w64-i686-tools-git mingw-w64-i686-clang mingw-w64-i686-lld mingw-w64-i686-llvm-tools
  ) ELSE (
    pacman -S --needed --noconfirm --noprogressbar mingw-w64-i686-gcc mingw-w64-i686-make mingw-w64-i686-tools-git
  )
) ELSE IF /I "%~1" == "llvm" (
  @rem for CI purpose only, the result binary is dynamic linked against api-ms-win-crt*.dll instead of msvcrt.dll
  curl -fsSL -o "llvm-mingw-20260224-ucrt-x86_64.zip" "https://github.com/mstorsjo/llvm-mingw/releases/download/20260224/llvm-mingw-20260224-ucrt-x86_64.zip"
  7z x -y -o"C:\" "llvm-mingw-20260224-ucrt-x86_64.zip" >NUL
  move /Y "C:\llvm-mingw-20260224-ucrt-x86_64" "C:\llvm-mingw"
)
