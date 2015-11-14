.SILENT :

!IF "$(CFG)" != "i386" && "$(CFG)" != "amd64"
CFG=i386
!ENDIF

!IF "$(CFG)" == "i386"
OUTPUT=..\..\bin
CFLAGS=/D "WIN32" /D "_WIN32_WINNT=0x0501" /c /W4 /EHsc /MD /arch:SSE2 /O1 /Oy /Oi /Os /GT /GL /GF /Gm- /GR- /GS- /Gy /GA /Zc:wchar_t /Zc:forScope
RCCFG=/d "_M_IX86"
LDCFG=/VERSION:5.1 /SUBSYSTEM:WINDOWS,5.01 /OSVERSION:5.1 /MACHINE:X86
LIBS2=msvcrt_winxp.obj
!ELSE
OUTPUT=..\..\bin\amd64
CFLAGS=/D "WIN64" /D "_WIN32_WINNT=0x0502" /c /W4 /EHsc /MD /arch:SSE2 /O1 /Oy /Oi /Os /GT /GL /GF /Gm- /GR- /GS- /Gy /GA /Zc:wchar_t /Zc:forScope
RCCFG=/d "_M_AMD64"
LDCFG=/VERSION:5.2 /SUBSYSTEM:WINDOWS,5.2 /OSVERSION:5.2 /MACHINE:AMD64
LIBS2=msvcrt_win2003.obj
!ENDIF

CC=cl.exe /nologo
RC=rc.exe
LD=link.exe /nologo

SRC=..\src
RES=..\res
EXE=$(OUTPUT)\metapath.exe

CDEF=/D "_WINDOWS" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" \
     /D "_STL70_" /D "_STATIC_CPPLIB" /D "_CRT_SECURE_NO_WARNINGS" /D "WDK_BUILD"
CINC=
RCFLAGS=/l 0x0409 /d "_UNICODE" /d "UNICODE" 
LDFLAGS=/nologo /INCREMENTAL:NO /RELEASE /OPT:REF /OPT:ICF /LTCG /LARGEADDRESSAWARE /DYNAMICBASE /NXCOMPAT /MERGE:.rdata=.text /DEBUG
LIBS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib shlwapi.lib comdlg32.lib \
  comctl32.lib winspool.lib imm32.lib ole32.lib oleaut32.lib psapi.lib ntstc_msvcrt.lib

BUILD : PREBUILD $(EXE) POSTBUILD

PREBUILD :
  echo Building...
  -md "$(OUTPUT)" >nul 2>&1

POSTBUILD :
  echo Success.

CLEAN :
  echo Cleaning...
  -del /s "$(OUTPUT)\*.exe" "$(OUTPUT)\*.obj" "$(OUTPUT)\*.res" "$(OUTPUT)\*.pdb" "$(OUTPUT)\*.idb" >nul 2>&1

REBUILD:	CLEAN BUILD

OBJ= \
  $(OUTPUT)\Dialogs.obj \
  $(OUTPUT)\Dlapi.obj \
  $(OUTPUT)\Dropsource.obj \
  $(OUTPUT)\Helpers.obj \
  $(OUTPUT)\metapath.obj \
  $(OUTPUT)\metapath.res

{$(SRC)\}.c{$(OUTPUT)}.obj::
  $(CC) /Fo$(OUTPUT)\ $(CINC) $(CDEF) $(CFLAGS) $<

{$(SRC)\}.cpp{$(OUTPUT)}.obj::
  $(CC) /Fo$(OUTPUT)\ $(CINC) $(CDEF) $(CFLAGS) $<

$(EXE): $(OBJ)
	$(RC) $(RFLAGS) $(RCCFG) /fo"$(OUTPUT)\metapath.res" "$(SRC)\metapath.rc" >NUL
	$(LD) $(LDFLAGS) $(LDCFG) $(LIBS) $(LIBS2) $(OBJ) /OUT:"$(EXE)"

# depends
$(OUTPUT)\Dialogs.obj : \
  $(SRC)\Dialogs.c \
  $(SRC)\metapath.h \
  $(SRC)\Dlapi.h \
  $(SRC)\Helpers.h \
  $(SRC)\Dialogs.h \
  $(SRC)\resource.h \
  $(SRC)\version.h

$(OUTPUT)\Dlapi.obj : \
$(SRC)\Dlapi.c \
$(SRC)\Dlapi.h

$(OUTPUT)\Helpers.obj : \
  $(SRC)\Helpers.c \
  $(SRC)\Dlapi.h \
  $(SRC)\Helpers.h \
  $(SRC)\resource.h

$(OUTPUT)\metapath.obj : \
  $(SRC)\metapath.c \
  $(SRC)\Helpers.h \
  $(SRC)\Dlapi.h \
  $(SRC)\Dialogs.h \
  $(SRC)\metapath.h \
  $(SRC)\resource.h

$(OUTPUT)\Dropsource.obj : \
  $(SRC)\Dropsource.cpp \
  $(SRC)\Dropsource.h

$(OUTPUT)\metapath.res : \
  $(SRC)\metapath.rc \
  $(SRC)\resource.h \
  $(SRC)\version.h \
  $(SRC)\metapath.ver \
  $(RES)\Open.bmp \
  $(RES)\Toolbar.bmp \
  $(RES)\crosshair.cur \
  $(RES)\cross1.ico \
  $(RES)\cross2.ico \
  $(RES)\Folder.ico \
  $(RES)\Goto.ico \
  $(RES)\Items.ico \
  $(RES)\metapath.ico \
  $(RES)\Options.ico \
  $(RES)\Options2.ico \
  $(RES)\Progs.ico \
  $(RES)\Run.ico \
  $(RES)\metapath.exe.manifest
