!include "x64.nsh"
!include "LogicLib.nsh"
!include "MUI2.nsh"

; Request user level (not admin)
RequestExecutionLevel user

; Enable high DPI support
ManifestDPIAware true

!define PRODUCT_NAME "Notepad4"
!define PRODUCT_VERSION "${VERSION}"
!define PRODUCT_PUBLISHER "zufuliu"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Notepad4.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKCU"


; MUI General Settings
!define MUI_ABORTWARNING
!define MUI_ICON "..\res\Notepad4.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\nsis.bmp"
!define MUI_COMPONENTSPAGE_SMALLDESC

; Page Settings
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\License.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstall Page Settings
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Language Settings
!insertmacro MUI_LANGUAGE "English"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION} (${ARCH})"
OutFile "${OUTPUT_FILE}"

; Install to user directory (since we're using user execution level)
InstallDir "$LOCALAPPDATA\${PRODUCT_NAME}"

InstallDirRegKey HKCU "${PRODUCT_DIR_REGKEY}" "Path"

; Component page configuration
!define MUI_COMPONENTSPAGE_NODESC

Section "Notepad4 Main Program" SEC_MAIN
  SectionIn RO
  SetOutPath "$INSTDIR"
  !echo "Arch=${ARCH}"
  !echo "Output_file=${OUTPUT_FILE}"
  ; Use hardcoded path for testing
  File "bin\Release\${ARCH}\Notepad4.exe"
  File "bin\Release\${ARCH}\matepath.exe"
  SetOutPath "$INSTDIR\locale"
  File /r "bin\Release\${ARCH}\locale\*"
  SetOutPath "$INSTDIR"
  
  ; Create start menu folder
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  
  WriteRegStr HKCU "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Notepad4.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Create Desktop Shortcut" SEC_DESKTOP_SHORTCUT
  ; Create desktop shortcut
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\Notepad4.exe"
SectionEnd

Section "Create Start Menu Shortcut" SEC_START_MENU_SHORTCUT
  ; Create start menu shortcut
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\Notepad4.exe"
SectionEnd


Section "Uninstall"
  ; Delete shortcuts
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
  RMDir "$SMPROGRAMS\${PRODUCT_NAME}"
  
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\Notepad4.exe"
  Delete "$INSTDIR\matepath.exe"
  
  ; Delete locale folders
  RMDir /r "$INSTDIR\locale"
  
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKCU "${PRODUCT_DIR_REGKEY}"
  
  RMDir "$INSTDIR"
SectionEnd