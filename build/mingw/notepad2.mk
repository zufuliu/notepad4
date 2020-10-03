# Makefile for notepad2

PROJ = notepad2
NAME = $(PROJ).exe
BINDIR = $(BINFOLDER)/$(PROJ)
SRCDIR = ../../src
editlexers_dir = $(SRCDIR)/EditLexers
scintilla_dir = ../../scintilla

INCDIR = \
-I"../../src" \
-I"../../src/EditLexers" \
-I"$(scintilla_dir)/include"

LDFLAGS += -L"$(BINFOLDER)/scintilla"

LDLIBS += -limm32

editlexers_src = $(wildcard $(editlexers_dir)/*.c)
editlexers_obj = $(patsubst $(editlexers_dir)/%.c,$(BINDIR)/%.obj,$(editlexers_src))

c_src = $(wildcard $(SRCDIR)/*.c)
c_obj = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%.obj,$(c_src))

cpp_src = $(wildcard $(SRCDIR)/*.cpp)
cpp_obj = $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.obj,$(cpp_src))

rc_src = $(wildcard $(SRCDIR)/*.rc)
rc_obj = $(patsubst $(SRCDIR)/%.rc,$(BINDIR)/%.res,$(rc_src))

all : $(BINDIR) $(NAME)

$(BINDIR) :
	@mkdir -p $(BINDIR)

$(NAME) : $(editlexers_obj) $(c_obj) $(cpp_obj) $(rc_obj)
	$(CXX) $^ $(LDFLAGS) -lscintilla $(LDLIBS) -o $(BINDIR)/$@

$(editlexers_obj): $(BINDIR)/%.obj: $(editlexers_dir)/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCDIR) $< -o $(BINDIR)/$*.obj

$(c_obj): $(BINDIR)/%.obj: $(SRCDIR)/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCDIR) $< -o $(BINDIR)/$*.obj

$(cpp_obj): $(BINDIR)/%.obj: $(SRCDIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(BINDIR)/$*.obj

$(rc_obj) : $(BINDIR)/%.res: $(SRCDIR)/%.rc
	$(RC) -c 65001 --preprocessor '$(CC) -E -xc $(RESFLAGS) $(CPPFLAGS)' -O coff $< $(BINDIR)/$*.res

clean :
	@$(RM) -r $(BINDIR)
