# Main Makefile for notepad2 project

.PHONY: all

# Default flags for all configurations and architectures

export CPPFLAGS += -D_WINDOWS -DUNICODE -D_UNICODE
export CFLAGS += -std=gnu11 -municode -Wall -Wextra -Wshadow -Wimplicit-fallthrough -Wformat=2
export CXXFLAGS += -std=gnu++17 -municode -Wall -Wextra -Wshadow -Wimplicit-fallthrough -Wformat=2
export LDFLAGS += -mwindows -municode
export LDLIBS += -lkernel32 -luser32 -lgdi32 -lcomctl32 -lcomdlg32 -ladvapi32 -lshlwapi \
-lshell32 -lole32 -loleaut32 -luuid -luxtheme -lpsapi
export RESFLAGS += -DRC_INVOKED -DUNICODE -D_UNICODE

# Check if debug mode

ifdef DEBUG
  CONFIG := debug
  export CPPFLAGS += -D_DEBUG -D_WIN32_WINNT=0x0502 -DWINVER=0x0502
  export CFLAGS += -g -O0
  export CXXFALGS += -g -O0
  export LDFLAGS += -O0 -Wl,--major-os-version,5 -Wl,--minor-os-version,2 -Wl,--subsystem,windows:5.02
  export RESFLAGS += -D_DEBUG
else
  CONFIG := release
  export CPPFLAGS += -DNDEBUG -D_WIN32_WINNT=0x0502 -DWINVER=0x0502
  export CFLAGS += -O2 -ffast-math -fomit-frame-pointer
  export CXXFALGS += -O2 -ffast-math -fno-rtti -fomit-frame-pointer
  export LDFLAGS += -s -O2 -static -Wl,--gc-sections -ffast-math -fno-rtti \
    -fomit-frame-pointer -Wl,--major-os-version,5 -Wl,--minor-os-version,2 -Wl,--subsystem,windows:5.02
endif

# Detect host architecture

ARCH = $(shell $(CC) -dumpmachine)

ifeq ($(ARCH), x86_64-w64-mingw32)
  ARCH := x86_64
  export CPPFLAGS += -D_WIN64
  export RESFLAGS += -D_WIN64
else ifeq ($(ARCH), i686-w64-mingw32)
  ARCH := x86
  export CPPFLAGS += -DWIN32
  export RESFLAGS += -DWIN32
else
  $(error '$(ARCH)' is not supported)
endif

# Host triplet if specified. e.g. x86_64-w64-mingw32 for 64bit.

ifdef TRIPLET
  export RC := $(TRIPLET)-windres
else
  export RC := windres
endif

# Name root build folder

export BINFOLDER := $(CONFIG)_$(ARCH)

all : metapath scintilla notepad2

metapath :
	$(MAKE) -f metapath.mk

scintilla:
	$(MAKE) -f scintilla.mk

notepad2 :
	$(MAKE) -f scintilla.mk
	$(MAKE) -f notepad2.mk

clean :
	$(MAKE) -f metapath.mk clean
	$(MAKE) -f scintilla.mk clean
	$(MAKE) -f notepad2.mk clean
