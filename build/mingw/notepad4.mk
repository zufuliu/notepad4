# Makefile for notepad4

PROJ = Notepad4
NAME = $(BINFOLDER)/$(PROJ).exe
OBJDIR = $(BINFOLDER)/obj/$(PROJ)
SRCDIR = ../../src
editlexers_dir = $(SRCDIR)/EditLexers
scintilla_dir = ../../scintilla
darkmodelib_dir = ../../darkmodelib

INCDIR = \
	-I"../../src" \
	-I"../../src/EditLexers" \
	-I"$(scintilla_dir)/include" \
	-I"$(darkmodelib_dir)"

CPPFLAGS += -D_DARKMODELIB_NO_INI_CONFIG

LDFLAGS += -L"$(BINFOLDER)/obj"

LDLIBS += -limm32 -ldwmapi

editlexers_src = $(wildcard $(editlexers_dir)/*.cpp)
editlexers_obj = $(patsubst $(editlexers_dir)/%.cpp,$(OBJDIR)/%.obj,$(editlexers_src))

darkmodelib_src = $(wildcard $(darkmodelib_dir)/*.cpp)
darkmodelib_obj = $(patsubst $(darkmodelib_dir)/%.cpp,$(OBJDIR)/dmlib_%.obj,$(darkmodelib_src))

# c_src = $(wildcard $(SRCDIR)/*.c)
# c_obj = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.obj,$(c_src))

cpp_src = $(wildcard $(SRCDIR)/*.cpp)
cpp_obj = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.obj,$(cpp_src))

rc_src = $(wildcard $(SRCDIR)/*.rc)
rc_obj = $(patsubst $(SRCDIR)/%.rc,$(OBJDIR)/%.res,$(rc_src))

all: $(NAME)

$(NAME): $(editlexers_obj) $(cpp_obj) $(darkmodelib_obj) $(rc_obj)
	$(CXX) $^ $(LDFLAGS) -lscintilla $(LDLIBS) -o $@

$(editlexers_obj): $(OBJDIR)/%.obj: $(editlexers_dir)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(darkmodelib_obj): $(OBJDIR)/dmlib_%.obj: $(darkmodelib_dir)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $@

# $(c_obj): $(OBJDIR)/%.obj: $(SRCDIR)/%.c
# 	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(cpp_obj): $(OBJDIR)/%.obj: $(SRCDIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(rc_obj): $(OBJDIR)/%.res: $(SRCDIR)/%.rc
	$(RC) -c 65001 $(CPPFLAGS) $(RCFLAGS) $< $(OBJDIR)/$*.res

clean:
	@$(RM) -rf $(OBJDIR)
	@$(RM) -f $(NAME)
