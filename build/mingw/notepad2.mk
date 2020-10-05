# Makefile for notepad2

PROJ = Notepad2
NAME = $(PROJ).exe
OBJDIR = $(BINFOLDER)/obj/$(PROJ)
SRCDIR = ../../src
editlexers_dir = $(SRCDIR)/EditLexers
scintilla_dir = ../../scintilla

INCDIR = \
	-I"../../src" \
	-I"../../src/EditLexers" \
	-I"$(scintilla_dir)/include"

LDFLAGS += -L"$(BINFOLDER)/obj"

LDLIBS += -limm32

editlexers_src = $(wildcard $(editlexers_dir)/*.c)
editlexers_obj = $(patsubst $(editlexers_dir)/%.c,$(OBJDIR)/%.obj,$(editlexers_src))

c_src = $(wildcard $(SRCDIR)/*.c)
c_obj = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.obj,$(c_src))

cpp_src = $(wildcard $(SRCDIR)/*.cpp)
cpp_obj = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.obj,$(cpp_src))

rc_src = $(wildcard $(SRCDIR)/*.rc)
rc_obj = $(patsubst $(SRCDIR)/%.rc,$(OBJDIR)/%.res,$(rc_src))

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(NAME): $(editlexers_obj) $(c_obj) $(cpp_obj) $(rc_obj)
	$(CXX) $^ $(LDFLAGS) -lscintilla $(LDLIBS) -o $(BINFOLDER)/$@

$(editlexers_obj): $(OBJDIR)/%.obj: $(editlexers_dir)/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(c_obj): $(OBJDIR)/%.obj: $(SRCDIR)/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(cpp_obj): $(OBJDIR)/%.obj: $(SRCDIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) $< -o $(OBJDIR)/$*.obj

$(rc_obj): $(OBJDIR)/%.res: $(SRCDIR)/%.rc
	$(RC) -c 65001 $(CPPFLAGS) $(RCFLAGS) $< $(OBJDIR)/$*.res

clean:
	@$(RM) -rf $(OBJDIR)
	@$(RM) -f $(BINFOLDER)/$(NAME)
