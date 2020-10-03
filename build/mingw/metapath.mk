# Makefile for metapath

PROJ = metapath
NAME = $(PROJ).exe
BINDIR = $(BINFOLDER)/$(PROJ)
SRCDIR = ../../$(PROJ)/src

c_src = $(wildcard $(SRCDIR)/*.c)
c_obj = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%.obj,$(c_src))

cpp_src = $(wildcard $(SRCDIR)/*.cpp)
cpp_obj = $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.obj,$(cpp_src))

rc_src = $(wildcard $(SRCDIR)/*.rc)
rc_obj = $(patsubst $(SRCDIR)/%.rc,$(BINDIR)/%.res,$(rc_src))

all : $(BINDIR) $(NAME)

$(BINDIR) :
	@mkdir -p $(BINDIR)

$(NAME) : $(c_obj) $(cpp_obj) $(rc_obj)
	$(CXX) $^ $(LDFLAGS) $(LDLIBS) -o $(BINDIR)/$@

$(c_obj): $(BINDIR)/%.obj: $(SRCDIR)/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $(BINDIR)/$*.obj

$(cpp_obj): $(BINDIR)/%.obj: $(SRCDIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(BINDIR)/$*.obj

$(rc_obj) : $(BINDIR)/%.res: $(SRCDIR)/%.rc
	$(RC) -c 65001 --preprocessor '$(CC) -E -xc $(RESFLAGS) $(CPPFLAGS)' -O coff $< $(BINDIR)/$*.res

clean :
	@$(RM) -r $(BINDIR)
