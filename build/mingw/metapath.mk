# Makefile for metapath

PROJ = metapath
NAME = $(PROJ).exe
OBJDIR = $(BINFOLDER)/obj/$(PROJ)
SRCDIR = ../../$(PROJ)/src

LDLIBS += -lpsapi

c_src = $(wildcard $(SRCDIR)/*.c)
c_obj = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.obj,$(c_src))

cpp_src = $(wildcard $(SRCDIR)/*.cpp)
cpp_obj = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.obj,$(cpp_src))

rc_src = $(wildcard $(SRCDIR)/*.rc)
rc_obj = $(patsubst $(SRCDIR)/%.rc,$(OBJDIR)/%.res,$(rc_src))

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(NAME): $(c_obj) $(cpp_obj) $(rc_obj)
	$(CXX) $^ $(LDFLAGS) $(LDLIBS) -o $(BINFOLDER)/$@

$(c_obj): $(OBJDIR)/%.obj: $(SRCDIR)/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $(OBJDIR)/$*.obj

$(cpp_obj): $(OBJDIR)/%.obj: $(SRCDIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $(OBJDIR)/$*.obj

$(rc_obj): $(OBJDIR)/%.res: $(SRCDIR)/%.rc
	$(RC) -c 65001 $(CPPFLAGS) $(RCFLAGS) $< $(OBJDIR)/$*.res

clean:
	@$(RM) -rf $(OBJDIR)
	@$(RM) -f $(BINFOLDER)/$(NAME)
