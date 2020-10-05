# Makefile for scintilla

PROJ = scintilla
NAME = lib$(PROJ).a
OBJDIR = $(BINFOLDER)/obj/$(PROJ)

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
lexers_obj = $(patsubst $(lexers_dir)/%.cxx,$(OBJDIR)/%.obj,$(lexers_src))

lexlib_src = $(wildcard $(lexlib_dir)/*.cxx)
lexlib_obj = $(patsubst $(lexlib_dir)/%.cxx,$(OBJDIR)/%.obj,$(lexlib_src))

scisrc_src = $(wildcard $(scisrc_dir)/*.cxx)
scisrc_obj = $(patsubst $(scisrc_dir)/%.cxx,$(OBJDIR)/%.obj,$(scisrc_src))

sciwin32_src = $(wildcard $(sciwin32_dir)/*.cxx)
sciwin32_obj = $(patsubst $(sciwin32_dir)/%.cxx,$(OBJDIR)/%.obj,$(sciwin32_src))

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(NAME): $(lexers_obj) $(lexlib_obj) $(scisrc_obj) $(sciwin32_obj)
	$(AR) cru $(BINFOLDER)/obj/$@ $^

$(lexers_obj): $(OBJDIR)/%.obj: $(lexers_dir)/%.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(lexlib_obj): $(OBJDIR)/%.obj: $(lexlib_dir)/%.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(scisrc_obj): $(OBJDIR)/%.obj: $(scisrc_dir)/%.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(sciwin32_obj): $(OBJDIR)/%.obj: $(sciwin32_dir)/%.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

clean:
	@$(RM) -rf $(OBJDIR)
	@$(RM) -f $(BINFOLDER)/obj/$(NAME)
