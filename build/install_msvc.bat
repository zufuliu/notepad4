@ECHO OFF
@rem used for GitHub Actions
SetLocal EnableExtensions

SET "VSINSTALLER=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vs_installer.exe"
SET "VSPATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise"

IF /I "%~1" == "v141_xp" (
	SHIFT
	"%VSINSTALLER%" modify --installPath "%VSPATH%" --add Microsoft.VisualStudio.Component.WinXP --quiet --norestart --nocache --noUpdateInstaller
)
