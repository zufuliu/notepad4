#******************************************************************************
#*
#* Notepad2-mod
#*
#* makefile.mak
#*   makefile for building Notepad2 with WDK
#*
#* See License.txt for details about distribution and modification.
#*
#*                                       (c) XhmikosR 2010-2011
#*                                       http://code.google.com/p/notepad2-mod/
#*
#* Use build_wdk.bat and set there your WDK directory.
#*
#******************************************************************************


# Remove the .SILENT directive in order to display all the commands
.SILENT:


CC = clang-cl.exe
LD = link.exe
RC = rc.exe

!IFDEF x64
BINDIR  = ..\bin
!ELSE
BINDIR  = ..\bin
!ENDIF
OBJDIR  = $(BINDIR)\obj-cl
EXE     = $(BINDIR)\Notepad2.exe

SCI_OBJDIR      = $(OBJDIR)\scintilla
SCI_LEX_OBJDIR  = $(SCI_OBJDIR)\lexers
SCI_LIB_OBJDIR  = $(SCI_OBJDIR)\lexlib
SCI_SRC_OBJDIR  = $(SCI_OBJDIR)\src
SCI_WIN_OBJDIR  = $(SCI_OBJDIR)\win32
NP2_SRC_OBJDIR  = $(OBJDIR)\notepad2
NP2_STL_OBJDIR  = $(NP2_SRC_OBJDIR)\EditLexers


SCI_DIR         = ..\scintilla
SCI_INC         = $(SCI_DIR)\include
SCI_LEX         = $(SCI_DIR)\lexers
SCI_LIB         = $(SCI_DIR)\lexlib
SCI_SRC         = $(SCI_DIR)\src
SCI_WIN         = $(SCI_DIR)\win32
NP2_SRC         = ..\src
NP2_STL         = $(NP2_SRC)\EditLexers
NP2_RES         = ..\res

DEFINES       = /D "_WINDOWS" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" \
                /D "_CRT_SECURE_NO_WARNINGS" \
                /D "BOOKMARK_EDITION"
INCLUDEDIRS   = /I "$(SCI_INC)" /I "$(SCI_LEX)" /I "$(SCI_LIB)" /I "$(SCI_SRC)" \
                /I "$(SCI_WIN)" /I "$(NP2_SRC)"
CXXFLAGS      = /nologo /c -Wall -Wextra -Wno-deprecated-declarations -Wno-missing-braces -Wno-unused-parameter -Wno-missing-field-initializers \
				/MD /EHsc /arch:SSE2 /O2 /GF /GR- /GS- /Gy /Zc:wchar_t /Zc:forScope $(DEFINES) $(INCLUDEDIRS)
LDFLAGS       = /nologo /INCREMENTAL:NO /RELEASE /OPT:REF /OPT:ICF /DYNAMICBASE /NXCOMPAT /DEBUG
LIBS          = kernel32.lib gdi32.lib user32.lib advapi32.lib comctl32.lib comdlg32.lib imm32.lib \
                ole32.lib oleaut32.lib psapi.lib shell32.lib shlwapi.lib \
                winspool.lib msvcrt.lib
RFLAGS        = /l 0x0409 /d "_UNICODE" /d "UNICODE" /d "BOOKMARK_EDITION"
SCI_CXXFLAGS  = $(CXXFLAGS) /D "STATIC_BUILD" /D "SCI_LEXER" /D "DISABLE_D2D" /D "SCI_DISABLE_PROVISIONAL"

!IFDEF x64
DEFINES       = $(DEFINES) /D "_WIN64" /D "_WIN32_WINNT=0x0502"
MACHINE       = X64
LDFLAGS       = $(LDFLAGS) /SUBSYSTEM:WINDOWS,5.02 /MACHINE:$(MACHINE)
LIBS          = $(LIBS)
RFLAGS        = $(RFLAGS) /d "_WIN64"
!ELSE
DEFINES       = $(DEFINES) /D "WIN32" /D "_WIN32_WINNT=0x0501"
MACHINE       = X86
LDFLAGS       = $(LDFLAGS) /LARGEADDRESSAWARE /SUBSYSTEM:WINDOWS,5.01 /MACHINE:$(MACHINE)
LIBS          = $(LIBS)
RFLAGS        = $(RFLAGS) /d "WIN32"
!ENDIF


###############
##  Targets  ##
###############
!INCLUDE "makefile.objs.mak"

####################
##  Dependencies  ##
####################
!INCLUDE "makefile.deps.mak"
