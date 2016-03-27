###############
##  Targets  ##
###############
BUILD:	PREBUILD $(EXE) POSTBUILD

PREBUILD:
	echo Building...
	IF NOT EXIST "$(SCI_LEX_OBJDIR)"    MD "$(SCI_LEX_OBJDIR)"
	IF NOT EXIST "$(SCI_LIB_OBJDIR)"    MD "$(SCI_LIB_OBJDIR)"
	IF NOT EXIST "$(SCI_SRC_OBJDIR)"    MD "$(SCI_SRC_OBJDIR)"
	IF NOT EXIST "$(SCI_WIN_OBJDIR)"    MD "$(SCI_WIN_OBJDIR)"
	IF NOT EXIST "$(NP2_SRC_OBJDIR)"    MD "$(NP2_SRC_OBJDIR)"
	IF NOT EXIST "$(NP2_STL_OBJDIR)"    MD "$(NP2_STL_OBJDIR)"
#	CD ..
#	CALL "update_version.bat"
#	CD "build"
#	ECHO.

POSTBUILD :
  echo Success.

CLEAN:
	ECHO Cleaning... & ECHO.
	IF EXIST "$(EXE)"                           DEL "$(EXE)"
	IF EXIST "$(NP2_SRC_OBJDIR)\*.obj"          DEL "$(NP2_SRC_OBJDIR)\*.obj"
	IF EXIST "$(NP2_STL_OBJDIR)\*.obj"          DEL "$(NP2_STL_OBJDIR)\*.obj"
	IF EXIST "$(SCI_LEX_OBJDIR)\*.obj"          DEL "$(SCI_LEX_OBJDIR)\*.obj"
	IF EXIST "$(SCI_LIB_OBJDIR)\*.obj"          DEL "$(SCI_LIB_OBJDIR)\*.obj"
	IF EXIST "$(SCI_SRC_OBJDIR)\*.obj"          DEL "$(SCI_SRC_OBJDIR)\*.obj"
	IF EXIST "$(SCI_WIN_OBJDIR)\*.obj"          DEL "$(SCI_WIN_OBJDIR)\*.obj"
	IF EXIST "$(NP2_SRC_OBJDIR)\Notepad2.res"   DEL "$(NP2_SRC_OBJDIR)\Notepad2.res"
	IF EXIST "$(BINDIR)\Notepad2.pdb"           DEL "$(BINDIR)\Notepad2.pdb"
	-IF EXIST "$(SCI_LEX_OBJDIR)"               RD /Q "$(SCI_LEX_OBJDIR)"
	-IF EXIST "$(SCI_LIB_OBJDIR)"               RD /Q "$(SCI_LIB_OBJDIR)"
	-IF EXIST "$(SCI_SRC_OBJDIR)"               RD /Q "$(SCI_SRC_OBJDIR)"
	-IF EXIST "$(SCI_WIN_OBJDIR)"               RD /Q "$(SCI_WIN_OBJDIR)"
	-IF EXIST "$(SCI_OBJDIR)"                   RD /Q "$(SCI_OBJDIR)"
	-IF EXIST "$(NP2_SRC_OBJDIR)"               RD /Q "$(NP2_SRC_OBJDIR)"
	-IF EXIST "$(NP2_STL_OBJDIR)"               RD /Q "$(NP2_STL_OBJDIR)"
	-IF EXIST "$(OBJDIR)"                       RD /Q "$(OBJDIR)"
	-IF EXIST "$(BINDIR)"                       RD /Q "$(BINDIR)"

REBUILD:	CLEAN BUILD


####################
##  Object files  ##
####################
SCI_LEX_OBJ = \
    $(SCI_LEX_OBJDIR)\LexNull.obj \
    $(SCI_LEX_OBJDIR)\LexAsm.obj \
    $(SCI_LEX_OBJDIR)\LexAutoIt3.obj \
    $(SCI_LEX_OBJDIR)\LexBash.obj \
    $(SCI_LEX_OBJDIR)\LexBatch.obj \
    $(SCI_LEX_OBJDIR)\LexCLI.obj \
    $(SCI_LEX_OBJDIR)\LexCMake.obj \
    $(SCI_LEX_OBJDIR)\LexConfig.obj \
    $(SCI_LEX_OBJDIR)\LexCPP.obj \
    $(SCI_LEX_OBJDIR)\LexCSS.obj \
    $(SCI_LEX_OBJDIR)\LexDiff.obj \
    $(SCI_LEX_OBJDIR)\LexFortran.obj \
    $(SCI_LEX_OBJDIR)\LexFSharp.obj \
    $(SCI_LEX_OBJDIR)\LexGraphViz.obj \
    $(SCI_LEX_OBJDIR)\LexHTML.obj \
    $(SCI_LEX_OBJDIR)\LexIni.obj \
    $(SCI_LEX_OBJDIR)\LexInno.obj \
    $(SCI_LEX_OBJDIR)\LexJSON.obj \
    $(SCI_LEX_OBJDIR)\LexLaTeX.obj \
    $(SCI_LEX_OBJDIR)\LexLisp.obj \
    $(SCI_LEX_OBJDIR)\LexLLVM.obj \
    $(SCI_LEX_OBJDIR)\LexLua.obj \
    $(SCI_LEX_OBJDIR)\LexMakefile.obj \
    $(SCI_LEX_OBJDIR)\LexMatlab.obj \
    $(SCI_LEX_OBJDIR)\LexNSIS.obj \
    $(SCI_LEX_OBJDIR)\LexPascal.obj \
    $(SCI_LEX_OBJDIR)\LexPerl.obj \
    $(SCI_LEX_OBJDIR)\LexPowerShell.obj \
    $(SCI_LEX_OBJDIR)\LexPython.obj \
    $(SCI_LEX_OBJDIR)\LexRuby.obj \
    $(SCI_LEX_OBJDIR)\LexSmali.obj \
    $(SCI_LEX_OBJDIR)\LexSQL.obj \
    $(SCI_LEX_OBJDIR)\LexTCL.obj \
    $(SCI_LEX_OBJDIR)\LexTexinfo.obj \
    $(SCI_LEX_OBJDIR)\LexVB.obj \
    $(SCI_LEX_OBJDIR)\LexVerilog.obj \
    $(SCI_LEX_OBJDIR)\LexVHDL.obj \
    $(SCI_LEX_OBJDIR)\LexVim.obj

SCI_LIB_OBJ = \
    $(SCI_LIB_OBJDIR)\Accessor.obj \
    $(SCI_LIB_OBJDIR)\CharacterSet.obj \
    $(SCI_LIB_OBJDIR)\HereDoc.obj \
    $(SCI_LIB_OBJDIR)\LexAccessor.obj \
    $(SCI_LIB_OBJDIR)\LexerBase.obj \
    $(SCI_LIB_OBJDIR)\LexerModule.obj \
    $(SCI_LIB_OBJDIR)\LexerSimple.obj \
    $(SCI_LIB_OBJDIR)\PropSetSimple.obj \
    $(SCI_LIB_OBJDIR)\StyleContext.obj \
    $(SCI_LIB_OBJDIR)\WordList.obj

SCI_SRC_OBJ = \
    $(SCI_SRC_OBJDIR)\AutoComplete.obj \
    $(SCI_SRC_OBJDIR)\CallTip.obj \
    $(SCI_SRC_OBJDIR)\CaseConvert.obj \
    $(SCI_SRC_OBJDIR)\CaseFolder.obj \
    $(SCI_SRC_OBJDIR)\Catalogue.obj \
    $(SCI_SRC_OBJDIR)\CellBuffer.obj \
    $(SCI_SRC_OBJDIR)\CharClassify.obj \
    $(SCI_SRC_OBJDIR)\ContractionState.obj \
    $(SCI_SRC_OBJDIR)\Decoration.obj \
    $(SCI_SRC_OBJDIR)\Document.obj \
    $(SCI_SRC_OBJDIR)\EditModel.obj \
    $(SCI_SRC_OBJDIR)\Editor.obj \
    $(SCI_SRC_OBJDIR)\EditView.obj \
    $(SCI_SRC_OBJDIR)\ExternalLexer.obj \
    $(SCI_SRC_OBJDIR)\Indicator.obj \
    $(SCI_SRC_OBJDIR)\KeyMap.obj \
    $(SCI_SRC_OBJDIR)\LineMarker.obj \
    $(SCI_SRC_OBJDIR)\MarginView.obj \
    $(SCI_SRC_OBJDIR)\PerLine.obj \
    $(SCI_SRC_OBJDIR)\PositionCache.obj \
    $(SCI_SRC_OBJDIR)\RESearch.obj \
    $(SCI_SRC_OBJDIR)\RunStyles.obj \
    $(SCI_SRC_OBJDIR)\ScintillaBase.obj \
    $(SCI_SRC_OBJDIR)\Selection.obj \
    $(SCI_SRC_OBJDIR)\Style.obj \
    $(SCI_SRC_OBJDIR)\UniConversion.obj \
    $(SCI_SRC_OBJDIR)\ViewStyle.obj \
    $(SCI_SRC_OBJDIR)\XPM.obj

SCI_WIN_OBJ = \
    $(SCI_WIN_OBJDIR)\PlatWin.obj \
    $(SCI_WIN_OBJDIR)\ScintillaWin.obj

NOTEPAD2_OBJ = \
    $(NP2_SRC_OBJDIR)\Dialogs.obj \
    $(NP2_SRC_OBJDIR)\Dlapi.obj \
    $(NP2_SRC_OBJDIR)\Edit.obj \
    $(NP2_SRC_OBJDIR)\EditAutoC.obj \
    $(NP2_SRC_OBJDIR)\EditEncoding.obj \
    $(NP2_SRC_OBJDIR)\Helpers.obj \
    $(NP2_SRC_OBJDIR)\Notepad2.obj \
    $(NP2_SRC_OBJDIR)\Notepad2.res \
    $(NP2_SRC_OBJDIR)\Print.obj \
    $(NP2_SRC_OBJDIR)\Styles.obj

NP2_STL_OBJ = \
    $(NP2_STL_OBJDIR)\stlActionScript.obj \
    $(NP2_STL_OBJDIR)\stlAsm.obj \
    $(NP2_STL_OBJDIR)\stlAsymptote.obj \
    $(NP2_STL_OBJDIR)\stlAutoIt3.obj \
    $(NP2_STL_OBJDIR)\stlAwk.obj \
    $(NP2_STL_OBJDIR)\stlBash.obj \
    $(NP2_STL_OBJDIR)\stlBatch.obj \
    $(NP2_STL_OBJDIR)\stlCLI.obj \
    $(NP2_STL_OBJDIR)\stlCMake.obj \
    $(NP2_STL_OBJDIR)\stlCPP.obj \
    $(NP2_STL_OBJDIR)\stlCSharp.obj \
    $(NP2_STL_OBJDIR)\stlCSS.obj \
    $(NP2_STL_OBJDIR)\stlD.obj \
    $(NP2_STL_OBJDIR)\stlDefault.obj \
    $(NP2_STL_OBJDIR)\stlFortran.obj \
    $(NP2_STL_OBJDIR)\stlFSharp.obj \
    $(NP2_STL_OBJDIR)\stlGo.obj \
    $(NP2_STL_OBJDIR)\stlGradle.obj \
    $(NP2_STL_OBJDIR)\stlGraphViz.obj \
    $(NP2_STL_OBJDIR)\stlGroovy.obj \
    $(NP2_STL_OBJDIR)\stlHaXe.obj \
    $(NP2_STL_OBJDIR)\stlHTML.obj \
    $(NP2_STL_OBJDIR)\stlInno.obj \
    $(NP2_STL_OBJDIR)\stlJamfile.obj \
    $(NP2_STL_OBJDIR)\stlJava.obj \
    $(NP2_STL_OBJDIR)\stlJavaScript.obj \
    $(NP2_STL_OBJDIR)\stlJSON.obj \
    $(NP2_STL_OBJDIR)\stlJulia.obj \
    $(NP2_STL_OBJDIR)\stlLaTeX.obj \
    $(NP2_STL_OBJDIR)\stlLisp.obj \
    $(NP2_STL_OBJDIR)\stlLLVM.obj \
    $(NP2_STL_OBJDIR)\stlLua.obj \
    $(NP2_STL_OBJDIR)\stlMake.obj \
    $(NP2_STL_OBJDIR)\stlMatlab.obj \
    $(NP2_STL_OBJDIR)\stlNsis.obj \
    $(NP2_STL_OBJDIR)\stlPascal.obj \
    $(NP2_STL_OBJDIR)\stlPerl.obj \
    $(NP2_STL_OBJDIR)\stlPHP.obj \
    $(NP2_STL_OBJDIR)\stlPowerShell.obj \
    $(NP2_STL_OBJDIR)\stlPython.obj \
    $(NP2_STL_OBJDIR)\stlResource.obj \
    $(NP2_STL_OBJDIR)\stlRuby.obj \
    $(NP2_STL_OBJDIR)\stlScala.obj \
    $(NP2_STL_OBJDIR)\stlSmali.obj \
    $(NP2_STL_OBJDIR)\stlSQL.obj \
    $(NP2_STL_OBJDIR)\stlTcl.obj \
    $(NP2_STL_OBJDIR)\stlVB.obj \
    $(NP2_STL_OBJDIR)\stlVBS.obj \
    $(NP2_STL_OBJDIR)\stlVerilog.obj \
    $(NP2_STL_OBJDIR)\stlVHDL.obj \
    $(NP2_STL_OBJDIR)\stlVim.obj \
    $(NP2_STL_OBJDIR)\stlXML.obj

OBJECTS = \
    $(SCI_LEX_OBJ) \
    $(SCI_LIB_OBJ) \
    $(SCI_SRC_OBJ) \
    $(SCI_WIN_OBJ) \
    $(NOTEPAD2_OBJ) \
    $(NP2_STL_OBJ)


###################
##  Batch rules  ##
###################
{$(SCI_LEX)}.cxx{$(SCI_LEX_OBJDIR)}.obj::
    $(CC) $(SCI_CXXFLAGS) /Fo"$(SCI_LEX_OBJDIR)/" /Tp $<

{$(SCI_LIB)}.cxx{$(SCI_LIB_OBJDIR)}.obj::
    $(CC) $(SCI_CXXFLAGS) /Fo"$(SCI_LIB_OBJDIR)/" /Tp $<

{$(SCI_SRC)}.cxx{$(SCI_SRC_OBJDIR)}.obj::
    $(CC) $(SCI_CXXFLAGS) /Fo"$(SCI_SRC_OBJDIR)/" /Tp $<

{$(SCI_WIN)}.cxx{$(SCI_WIN_OBJDIR)}.obj::
    $(CC) $(SCI_CXXFLAGS) /Fo"$(SCI_WIN_OBJDIR)/" /Tp $<

{$(NP2_SRC)}.cpp{$(NP2_SRC_OBJDIR)}.obj::
    $(CC) $(CXXFLAGS) /Fo"$(NP2_SRC_OBJDIR)/" /Tp $<

{$(NP2_SRC)}.c{$(NP2_SRC_OBJDIR)}.obj::
    $(CC) $(CXXFLAGS) /Fo"$(NP2_SRC_OBJDIR)/" /Tc $<

{$(NP2_STL)}.c{$(NP2_STL_OBJDIR)}.obj::
    $(CC) $(CXXFLAGS) /Fo"$(NP2_STL_OBJDIR)/" /Tc $<

################
##  Commands  ##
################
$(EXE): $(OBJECTS)
	$(RC) $(RFLAGS) /fo"$(NP2_SRC_OBJDIR)\Notepad2.res" "$(NP2_SRC)\Notepad2.rc" >NUL
	$(LD) $(LDFLAGS) $(LIBS) $(OBJECTS) /OUT:"$(EXE)"
