# Makefile for scintilla

PROJ = scintilla
NAME = lib$(PROJ).a
BINDIR = $(BINFOLDER)/$(PROJ)

CPPFLAGS += -DSCI_LEXER -DNO_CXX11_REGEX

scintilla_dir = ../../scintilla
lexers_dir = $(scintilla_dir)/lexers
lexlib_dir = $(scintilla_dir)/lexlib
scisrc_dir = $(scintilla_dir)/src
sciwin32_dir = $(scintilla_dir)/win32

INCDIR = \
-I"$(scintilla_dir)/include" \
-I"$(scintilla_dir)/lexlib" \
-I"$(scintilla_dir)/src"

lexers_src = $(wildcard $(lexers_dir)/*.cxx)
lexers_obj = $(patsubst $(lexers_dir)/%.cxx,$(BINDIR)/%.obj,$(lexers_src))

lexlib_src = $(wildcard $(lexlib_dir)/*.cxx)
lexlib_obj = $(patsubst $(lexlib_dir)/%.cxx,$(BINDIR)/%.obj,$(lexlib_src))

scisrc_src = $(wildcard $(scisrc_dir)/*.cxx)
scisrc_obj = $(patsubst $(scisrc_dir)/%.cxx,$(BINDIR)/%.obj,$(scisrc_src))

sciwin32_src = $(wildcard $(sciwin32_dir)/*.cxx)
sciwin32_obj = $(patsubst $(sciwin32_dir)/%.cxx,$(BINDIR)/%.obj,$(sciwin32_src))

all : $(BINDIR) $(NAME)

$(BINDIR) :
	@mkdir -p $(BINDIR)

$(NAME) : $(lexers_obj) $(lexlib_obj) $(scisrc_obj) $(sciwin32_obj)
	$(AR) cru $(BINDIR)/$@ $^

$(lexers_obj): $(BINDIR)/%.obj: $(lexers_dir)/%.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(BINDIR)/$*.obj

$(lexlib_obj): $(BINDIR)/%.obj: $(lexlib_dir)/%.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(BINDIR)/$*.obj

$(scisrc_obj): $(BINDIR)/%.obj: $(scisrc_dir)/%.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(BINDIR)/$*.obj

$(sciwin32_obj): $(BINDIR)/%.obj: $(sciwin32_dir)/%.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(BINDIR)/$*.obj

clean :
	@$(RM) -r $(BINDIR)
