#******************************************************************************
#*
#* Notepad2-mod
#*
#* makefile.deps.mak
#*   Contains the dependencies for makefile.mak
#*
#* See License.txt for details about distribution and modification.
#*
#*                                       (c) XhmikosR 2010-2011
#*                                       http://code.google.com/p/notepad2-mod/
#*
#*
#*
#******************************************************************************


########################
##  scintilla\lexers  ##
########################
LEX_HEADERS = \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\WordList.h \
    $(SCI_LIB)\CharacterSet.h \
    $(SCI_LIB)\LexAccessor.h \
    $(SCI_LIB)\Accessor.h \
    $(SCI_LIB)\StyleContext.h \
    $(SCI_LIB)\LexerModule.h

$(SCI_LEX_OBJDIR)\LexNull.obj: $(SCI_LEX)\LexNull.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexAsm.obj: $(SCI_LEX)\LexAsm.cxx $(LEX_HEADERS) $(SCI_LIB)
$(SCI_LEX_OBJDIR)\LexAutoIt3.obj: $(SCI_LEX)\LexAutoIt3.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexBash.obj: $(SCI_LEX)\LexBash.cxx $(LEX_HEADERS) $(SCI_LIB)\HereDoc.h
$(SCI_LEX_OBJDIR)\LexBatch.obj: $(SCI_LEX)\LexBatch.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexCLI.obj: $(SCI_LEX)\LexCLI.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexCMake.obj: $(SCI_LEX)\LexCMake.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexConfig.obj: $(SCI_LEX)\LexConfig.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexCPP.obj: $(SCI_LEX)\LexCPP.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexCSS.obj: $(SCI_LEX)\LexCSS.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexDiff.obj: $(SCI_LEX)\LexDiff.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexFortran.obj: $(SCI_LEX)\LexFortran.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexFSharp.obj: $(SCI_LEX)\LexFSharp.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexGraphViz.obj: $(SCI_LEX)\LexGraphViz.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexHTML.obj: $(SCI_LEX)\LexHTML.cxx $(LEX_HEADERS) $(SCI_LIB)\StringCopy.h
$(SCI_LEX_OBJDIR)\LexIni.obj: $(SCI_LEX)\LexIni.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexInno.obj: $(SCI_LEX)\LexInno.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexJSON.obj: $(SCI_LEX)\LexJSON.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexLaTeX.obj: $(SCI_LEX)\LexLaTeX.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexLisp.obj: $(SCI_LEX)\LexLisp.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexLLVM.obj: $(SCI_LEX)\LexLLVM.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexLua.obj: $(SCI_LEX)\LexLua.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexMakefile.obj: $(SCI_LEX)\LexMakefile.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexMatlab.obj: $(SCI_LEX)\LexMatlab.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexNSIS.obj: $(SCI_LEX)\LexNSIS.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexPascal.obj: $(SCI_LEX)\LexPascal.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexPerl.obj: $(SCI_LEX)\LexPerl.cxx $(LEX_HEADERS) $(SCI_LIB)\HereDoc.h
$(SCI_LEX_OBJDIR)\LexPowerShell.obj: $(SCI_LEX)\LexPowerShell.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexPython.obj: $(SCI_LEX)\LexPython.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexRuby.obj: $(SCI_LEX)\LexRuby.cxx $(LEX_HEADERS) $(SCI_LIB)\HereDoc.h
$(SCI_LEX_OBJDIR)\LexSmali.obj: $(SCI_LEX)\LexSmali.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexSQL.obj: $(SCI_LEX)\LexSQL.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexTCL.obj: $(SCI_LEX)\LexTCL.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexTexinfo.obj: $(SCI_LEX)\LexTexinfo.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexVB.obj: $(SCI_LEX)\LexVB.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexVerilog.obj: $(SCI_LEX)\LexVerilog.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexVHDL.obj: $(SCI_LEX)\LexVHDL.cxx $(LEX_HEADERS)
$(SCI_LEX_OBJDIR)\LexVim.obj: $(SCI_LEX)\LexVim.cxx $(LEX_HEADERS)


########################
##  scintilla\lexlib  ##
########################
$(SCI_LIB_OBJDIR)\Accessor.obj: \
    $(SCI_LIB)\Accessor.cxx \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\PropSetSimple.h \
    $(SCI_LIB)\WordList.h \
    $(SCI_LIB)\LexAccessor.h \
    $(SCI_LIB)\Accessor.h

$(SCI_LIB_OBJDIR)\CharacterSet.obj: \
    $(SCI_LIB)\CharacterSet.cxx \
    $(SCI_LIB)\CharacterSet.h

$(SCI_LIB_OBJDIR)\HereDoc.obj: \
    $(SCI_LIB)\HereDoc.cxx \
    $(SCI_LIB)\HereDoc.h

$(SCI_LIB_OBJDIR)\LexAccessor.obj: \
    $(SCI_LIB)\LexAccessor.cxx \
    $(SCI_INC)\ILexer.h \
    $(SCI_LIB)\CharacterSet.h \
    $(SCI_LIB)\LexAccessor.h

$(SCI_LIB_OBJDIR)\LexerBase.obj: \
    $(SCI_LIB)\LexerBase.cxx \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\PropSetSimple.h \
    $(SCI_LIB)\WordList.h \
    $(SCI_LIB)\LexAccessor.h \
    $(SCI_LIB)\Accessor.h \
    $(SCI_LIB)\LexerModule.h \
    $(SCI_LIB)\LexerBase.h

$(SCI_LIB_OBJDIR)\LexerModule.obj: \
    $(SCI_LIB)\LexerModule.cxx \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\PropSetSimple.h \
    $(SCI_LIB)\WordList.h \
    $(SCI_LIB)\LexAccessor.h \
    $(SCI_LIB)\Accessor.h \
    $(SCI_LIB)\LexerModule.h \
    $(SCI_LIB)\LexerBase.h \
    $(SCI_LIB)\LexerSimple.h

$(SCI_LIB_OBJDIR)\LexerSimple.obj: \
    $(SCI_LIB)\LexerSimple.cxx \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\PropSetSimple.h \
    $(SCI_LIB)\WordList.h \
    $(SCI_LIB)\LexAccessor.h \
    $(SCI_LIB)\Accessor.h \
    $(SCI_LIB)\LexerModule.h \
    $(SCI_LIB)\LexerBase.h \
    $(SCI_LIB)\LexerSimple.h

$(SCI_LIB_OBJDIR)\PropSetSimple.obj: \
    $(SCI_LIB)\PropSetSimple.cxx \
    $(SCI_LIB)\PropSetSimple.h

$(SCI_LIB_OBJDIR)\StyleContext.obj: \
    $(SCI_LIB)\StyleContext.cxx \
    $(SCI_INC)\ILexer.h \
    $(SCI_LIB)\LexAccessor.h \
    $(SCI_LIB)\Accessor.h \
    $(SCI_LIB)\StyleContext.h

$(SCI_LIB_OBJDIR)\WordList.obj: \
    $(SCI_LIB)\WordList.cxx \
    $(SCI_LIB)\WordList.h


#####################
##  scintilla\src  ##
#####################
$(SCI_SRC_OBJDIR)\AutoComplete.obj: \
    $(SCI_SRC)\AutoComplete.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_LIB)\CharacterSet.h \
    $(SCI_SRC)\AutoComplete.h

$(SCI_SRC_OBJDIR)\CallTip.obj: \
    $(SCI_SRC)\CallTip.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\CallTip.h

$(SCI_SRC_OBJDIR)\CaseConvert.obj: \
    $(SCI_SRC)\CaseConvert.cxx \
    $(SCI_SRC)\CaseConvert.h \
    $(SCI_LIB)\StringCopy.h \
    $(SCI_SRC)\UniConversion.h \
    $(SCI_SRC)\UnicodeFromUTF8.h

$(SCI_SRC_OBJDIR)\CaseFolder.obj: \
    $(SCI_SRC)\CaseFolder.cxx \
    $(SCI_SRC)\CaseConvert.h \
    $(SCI_SRC)\UniConversion.h \
    $(SCI_SRC)\CaseFolder.h

$(SCI_SRC_OBJDIR)\Catalogue.obj: \
    $(SCI_SRC)\Catalogue.cxx \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\LexerModule.h \
    $(SCI_SRC)\Catalogue.h

$(SCI_SRC_OBJDIR)\CellBuffer.obj: \
    $(SCI_SRC)\CellBuffer.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\UniConversion.h

$(SCI_SRC_OBJDIR)\CharClassify.obj: \
    $(SCI_SRC)\CharClassify.cxx \
    $(SCI_SRC)\CharClassify.h

$(SCI_SRC_OBJDIR)\ContractionState.obj: \
    $(SCI_SRC)\ContractionState.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\ContractionState.h

$(SCI_SRC_OBJDIR)\Decoration.obj: \
    $(SCI_SRC)\Decoration.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\Decoration.h

$(SCI_SRC_OBJDIR)\Document.obj: \
    $(SCI_SRC)\Document.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\PerLine.h \
    $(SCI_SRC)\CharClassify.h \
    $(SCI_LIB)\CharacterSet.h \
    $(SCI_SRC)\Decoration.h \
    $(SCI_SRC)\CaseFolder.h \
    $(SCI_SRC)\Document.h \
    $(SCI_SRC)\RESearch.h \
    $(SCI_SRC)\UniConversion.h

$(SCI_SRC_OBJDIR)\EditModel.obj: \
    $(SCI_SRC)\EditModel.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_LIB)\StringCopy.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\ContractionState.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\KeyMap.h \
    $(SCI_SRC)\Indicator.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\LineMarker.h \
    $(SCI_SRC)\Style.h \
    $(SCI_SRC)\ViewStyle.h \
    $(SCI_SRC)\CharClassify.h \
    $(SCI_SRC)\Decoration.h \
    $(SCI_SRC)\CaseFolder.h \
    $(SCI_SRC)\Document.h \
    $(SCI_SRC)\UniConversion.h \
    $(SCI_SRC)\Selection.h \
    $(SCI_SRC)\PositionCache.h \
    $(SCI_SRC)\EditModel.h

$(SCI_SRC_OBJDIR)\Editor.obj: \
    $(SCI_SRC)\Editor.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_LIB)\StringCopy.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\ContractionState.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\KeyMap.h \
    $(SCI_SRC)\Indicator.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\LineMarker.h \
    $(SCI_SRC)\Style.h \
    $(SCI_SRC)\ViewStyle.h \
    $(SCI_SRC)\CharClassify.h \
    $(SCI_SRC)\Decoration.h \
    $(SCI_SRC)\CaseFolder.h \
    $(SCI_SRC)\Document.h \
    $(SCI_SRC)\UniConversion.h \
    $(SCI_SRC)\Selection.h \
    $(SCI_SRC)\PositionCache.h \
    $(SCI_SRC)\EditModel.h \
    $(SCI_SRC)\MarginView.h \
    $(SCI_SRC)\EditView.h \
    $(SCI_SRC)\Editor.h

$(SCI_SRC_OBJDIR)\EditView.obj: \
    $(SCI_SRC)\EditView.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_LIB)\StringCopy.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\ContractionState.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\KeyMap.h \
    $(SCI_SRC)\Indicator.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\LineMarker.h \
    $(SCI_SRC)\Style.h \
    $(SCI_SRC)\ViewStyle.h \
    $(SCI_SRC)\CharClassify.h \
    $(SCI_SRC)\Decoration.h \
    $(SCI_SRC)\CaseFolder.h \
    $(SCI_SRC)\Document.h \
    $(SCI_SRC)\UniConversion.h \
    $(SCI_SRC)\Selection.h \
    $(SCI_SRC)\PositionCache.h \
    $(SCI_SRC)\EditModel.h \
    $(SCI_SRC)\MarginView.h \
    $(SCI_SRC)\EditView.h

$(SCI_SRC_OBJDIR)\ExternalLexer.obj: \
    $(SCI_SRC)\ExternalLexer.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\LexerModule.h \
    $(SCI_SRC)\Catalogue.h \
    $(SCI_SRC)\ExternalLexer.h

$(SCI_SRC_OBJDIR)\Indicator.obj: \
    $(SCI_SRC)\Indicator.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\Indicator.h

$(SCI_SRC_OBJDIR)\KeyMap.obj: \
    $(SCI_SRC)\KeyMap.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\KeyMap.h

$(SCI_SRC_OBJDIR)\LineMarker.obj: \
    $(SCI_SRC)\LineMarker.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\LineMarker.h

$(SCI_SRC_OBJDIR)\MarginView.obj: \
    $(SCI_SRC)\MarginView.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_LIB)\StringCopy.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\ContractionState.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\KeyMap.h \
    $(SCI_SRC)\Indicator.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\LineMarker.h \
    $(SCI_SRC)\Style.h \
    $(SCI_SRC)\ViewStyle.h \
    $(SCI_SRC)\CharClassify.h \
    $(SCI_SRC)\Decoration.h \
    $(SCI_SRC)\CaseFolder.h \
    $(SCI_SRC)\Document.h \
    $(SCI_SRC)\UniConversion.h \
    $(SCI_SRC)\Selection.h \
    $(SCI_SRC)\PositionCache.h \
    $(SCI_SRC)\EditModel.h \
    $(SCI_SRC)\MarginView.h \
    $(SCI_SRC)\EditView.h

$(SCI_SRC_OBJDIR)\PerLine.obj: \
    $(SCI_SRC)\PerLine.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\PerLine.h

$(SCI_SRC_OBJDIR)\PositionCache.obj: \
    $(SCI_SRC)\PositionCache.cxx \
    $(SCI_INC)\Platform.h  \
    $(SCI_INC)\ILexer.h  \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\SplitVector.h  \
    $(SCI_SRC)\Partitioning.h  \
    $(SCI_SRC)\RunStyles.h  \
    $(SCI_SRC)\ContractionState.h  \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\KeyMap.h  \
    $(SCI_SRC)\Indicator.h  \
    $(SCI_SRC)\XPM.h  \
    $(SCI_SRC)\LineMarker.h  \
    $(SCI_SRC)\Style.h \
    $(SCI_SRC)\ViewStyle.h \
    $(SCI_SRC)\CharClassify.h  \
    $(SCI_SRC)\Decoration.h  \
    $(SCI_SRC)\CaseFolder.h  \
    $(SCI_SRC)\Document.h  \
    $(SCI_SRC)\Selection.h \
    $(SCI_SRC)\PositionCache.h

$(SCI_SRC_OBJDIR)\RESearch.obj: \
    $(SCI_SRC)\RESearch.cxx \
    $(SCI_SRC)\CharClassify.h \
    $(SCI_SRC)\RESearch.h

$(SCI_SRC_OBJDIR)\RunStyles.obj: \
    $(SCI_SRC)\RunStyles.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h

$(SCI_SRC_OBJDIR)\ScintillaBase.obj: \
    $(SCI_SRC)\ScintillaBase.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\PropSetSimple.h \
    $(SCI_LIB)\LexerModule.h \
    $(SCI_SRC)\Catalogue.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\ContractionState.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\CallTip.h \
    $(SCI_SRC)\KeyMap.h \
    $(SCI_SRC)\Indicator.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\LineMarker.h \
    $(SCI_SRC)\Style.h \
    $(SCI_SRC)\ViewStyle.h \
    $(SCI_SRC)\AutoComplete.h \
    $(SCI_SRC)\CharClassify.h \
    $(SCI_SRC)\Decoration.h \
    $(SCI_SRC)\CaseFolder.h \
    $(SCI_SRC)\Document.h \
    $(SCI_SRC)\Selection.h \
    $(SCI_SRC)\PositionCache.h \
    $(SCI_SRC)\EditModel.h \
    $(SCI_SRC)\MarginView.h \
    $(SCI_SRC)\EditView.h \
    $(SCI_SRC)\Editor.h \
    $(SCI_SRC)\ScintillaBase.h

$(SCI_SRC_OBJDIR)\Selection.obj: \
    $(SCI_SRC)\Selection.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\Selection.h

$(SCI_SRC_OBJDIR)\Style.obj: \
    $(SCI_SRC)\Style.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\Style.h

$(SCI_SRC_OBJDIR)\UniConversion.obj: \
    $(SCI_SRC)\UniConversion.cxx \
    $(SCI_SRC)\UniConversion.h

$(SCI_SRC_OBJDIR)\ViewStyle.obj: \
    $(SCI_SRC)\ViewStyle.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\Indicator.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\LineMarker.h \
    $(SCI_SRC)\Style.h \
    $(SCI_SRC)\ViewStyle.h

$(SCI_SRC_OBJDIR)\XPM.obj: \
    $(SCI_SRC)\XPM.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_SRC)\XPM.h


#######################
##  scintilla\win32  ##
#######################
$(SCI_WIN_OBJDIR)\PlatWin.obj: \
    $(SCI_WIN)\PlatWin.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_LIB)\StringCopy.h \
    $(SCI_SRC)\UniConversion.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\FontQuality.h

$(SCI_WIN_OBJDIR)\ScintillaWin.obj: \
    $(SCI_WIN)\ScintillaWin.cxx \
    $(SCI_INC)\Platform.h \
    $(SCI_INC)\ILexer.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h \
    $(SCI_LIB)\StringCopy.h \
    $(SCI_LIB)\LexerModule.h \
    $(SCI_SRC)\SplitVector.h \
    $(SCI_SRC)\Partitioning.h \
    $(SCI_SRC)\RunStyles.h \
    $(SCI_SRC)\ContractionState.h \
    $(SCI_SRC)\CellBuffer.h \
    $(SCI_SRC)\CallTip.h \
    $(SCI_SRC)\KeyMap.h \
    $(SCI_SRC)\Indicator.h \
    $(SCI_SRC)\XPM.h \
    $(SCI_SRC)\LineMarker.h \
    $(SCI_SRC)\Style.h \
    $(SCI_SRC)\AutoComplete.h \
    $(SCI_SRC)\ViewStyle.h \
    $(SCI_SRC)\CharClassify.h \
    $(SCI_SRC)\Decoration.h \
    $(SCI_SRC)\Document.h \
    $(SCI_SRC)\Selection.h \
    $(SCI_SRC)\PositionCache.h \
    $(SCI_SRC)\Editor.h \
    $(SCI_SRC)\ScintillaBase.h \
    $(SCI_SRC)\UniConversion.h \
    $(SCI_SRC)\CaseConvert.h \
    $(SCI_WIN)\PlatWin.h \
    $(SCI_SRC)\ExternalLexer.h


###########
##  src  ##
###########
STL_HEADERS = \
    $(NP2_SRC)\EditLexer.h \
    $(NP2_STL)\EditStyle.h \
    $(SCI_INC)\Scintilla.h \
    $(SCI_INC)\SciLexer.h

$(NP2_SRC_OBJDIR)\Dialogs.obj: \
    $(NP2_SRC)\Dialogs.c \
    $(SCI_INC)\Scintilla.h \
    $(NP2_SRC)\Notepad2.h \
    $(NP2_SRC)\Edit.h \
    $(NP2_SRC)\Dlapi.h \
    $(NP2_SRC)\Dialogs.h \
    $(NP2_SRC)\Helpers.h \
    $(NP2_SRC)\Resource.h \
    $(NP2_SRC)\Version.h \
    $(NP2_SRC)\VersionRev.h

$(NP2_SRC_OBJDIR)\Dlapi.obj: \
    $(NP2_SRC)\Dlapi.c \
    $(NP2_SRC)\Dlapi.h

$(NP2_SRC_OBJDIR)\Edit.obj: \
    $(NP2_SRC)\Edit.c \
    $(NP2_SRC)\Notepad2.h \
    $(NP2_SRC)\Edit.h \
    $(NP2_SRC)\Styles.h \
    $(STL_HEADERS) \
    $(NP2_SRC)\Dialogs.h \
    $(NP2_SRC)\Helpers.h \
    $(NP2_SRC)\SciCall.h \
    $(NP2_SRC)\Resource.h

$(NP2_SRC_OBJDIR)\EditAutoC.obj: \
    $(NP2_SRC)\EditAutoC.c \
    $(NP2_SRC)\EditAutoC_Data0.c \
    $(NP2_SRC)\EditAutoC_WordList.c \
    $(NP2_SRC)\Edit.h \
    $(NP2_SRC)\Styles.h \
    $(NP2_SRC)\Helpers.h \
    $(STL_HEADERS) \
    $(NP2_SRC)\Resource.h

$(NP2_SRC_OBJDIR)\EditEncoding.obj: \
    $(NP2_SRC)\EditEncoding.c \
    $(NP2_SRC)\Notepad2.h \
    $(NP2_SRC)\Edit.h \
    $(NP2_SRC)\Styles.h \
    $(STL_HEADERS) \
    $(NP2_SRC)\Dialogs.h \
    $(NP2_SRC)\Helpers.h \
    $(NP2_SRC)\SciCall.h \
    $(NP2_SRC)\Resource.h

$(NP2_SRC_OBJDIR)\Helpers.obj: \
    $(NP2_SRC)\Helpers.c \
    $(SCI_INC)\Scintilla.h \
    $(NP2_SRC)\Helpers.h \
    $(NP2_SRC)\Resource.h

$(NP2_SRC_OBJDIR)\Notepad2.obj: \
    $(NP2_SRC)\Notepad2.c \
    $(NP2_SRC)\Notepad2.h \
    $(NP2_SRC)\Edit.h \
    $(NP2_SRC)\Styles.h \
    $(STL_HEADERS) \
    $(NP2_SRC)\Dialogs.h \
    $(NP2_SRC)\Helpers.h \
    $(NP2_SRC)\SciCall.h \
    $(NP2_SRC)\Resource.h

$(NP2_SRC_OBJDIR)\Notepad2.res: \
    $(NP2_SRC)\Notepad2.rc \
    $(NP2_SRC)\Notepad2.ver \
    $(NP2_SRC)\Version.h \
    $(NP2_SRC)\VersionRev.h \
    $(NP2_RES)\Copy.cur \
    $(NP2_RES)\Encoding.bmp \
    $(NP2_RES)\Next.bmp \
    $(NP2_RES)\Notepad2.exe.manifest \
    $(NP2_RES)\Notepad2.ico \
    $(NP2_RES)\Open.bmp \
    $(NP2_RES)\Pick.bmp \
    $(NP2_RES)\Prev.bmp \
    $(NP2_RES)\Run.ico \
    $(NP2_RES)\Styles.ico \
    $(NP2_RES)\Toolbar.bmp

$(NP2_SRC_OBJDIR)\Print.obj: \
    $(NP2_SRC)\Print.cpp \
    $(NP2_SRC)\Dialogs.h \
    $(NP2_SRC)\Helpers.h \
    $(NP2_SRC)\Resource.h

$(NP2_SRC_OBJDIR)\Styles.obj: \
    $(NP2_SRC)\Styles.c \
    $(NP2_SRC)\Notepad2.h \
    $(NP2_SRC)\Edit.h \
    $(NP2_SRC)\Styles.h \
    $(STL_HEADERS) \
    $(NP2_SRC)\Dialogs.h \
    $(NP2_SRC)\Helpers.h \
    $(NP2_SRC)\SciCall.h \
    $(NP2_SRC)\Resource.h

$(NP2_STL_OBJDIR)\stlActionScript.obj: $(NP2_STL)\stlActionScript.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlAsm.obj: $(NP2_STL)\stlAsm.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlAsymptote.obj: $(NP2_STL)\stlAsymptote.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlAutoIt3.obj: $(NP2_STL)\stlAutoIt3.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlAwk.obj: $(NP2_STL)\stlAwk.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlBash.obj: $(NP2_STL)\stlBash.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlBatch.obj: $(NP2_STL)\stlBatch.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlCLI.obj: $(NP2_STL)\stlCLI.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlCMake.obj: $(NP2_STL)\stlCMake.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlCPP.obj: $(NP2_STL)\stlCPP.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlCSharp.obj: $(NP2_STL)\stlCSharp.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlCSS.obj: $(NP2_STL)\stlCSS.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlD.obj: $(NP2_STL)\stlD.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlDefault.obj: $(NP2_STL)\stlDefault.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlFortran.obj: $(NP2_STL)\stlFortran.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlFSharp.obj: $(NP2_STL)\stlFSharp.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlGo.obj: $(NP2_STL)\stlGo.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlGradle.obj: $(NP2_STL)\stlGradle.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlGraphViz.obj: $(NP2_STL)\stlGraphViz.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlGroovy.obj: $(NP2_STL)\stlGroovy.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlHaXe.obj: $(NP2_STL)\stlHaXe.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlHTML.obj: $(NP2_STL)\stlHTML.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlInno.obj: $(NP2_STL)\stlInno.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlJamfile.obj: $(NP2_STL)\stlJamfile.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlJava.obj: $(NP2_STL)\stlJava.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlJavaScript.obj: $(NP2_STL)\stlJavaScript.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlJSON.obj: $(NP2_STL)\stlJSON.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlJulia.obj: $(NP2_STL)\stlJulia.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlLaTeX.obj: $(NP2_STL)\stlLaTeX.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlLisp.obj: $(NP2_STL)\stlLisp.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlLLVM.obj: $(NP2_STL)\stlLLVM.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlLua.obj: $(NP2_STL)\stlLua.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlMake.obj: $(NP2_STL)\stlMake.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlMatlab.obj: $(NP2_STL)\stlMatlab.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlNsis.obj: $(NP2_STL)\stlNsis.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlPascal.obj: $(NP2_STL)\stlPascal.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlPerl.obj: $(NP2_STL)\stlPerl.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlPHP.obj: $(NP2_STL)\stlPHP.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlPowerShell.obj: $(NP2_STL)\stlPowerShell.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlPython.obj: $(NP2_STL)\stlPython.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlResource.obj: $(NP2_STL)\stlResource.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlRuby.obj: $(NP2_STL)\stlRuby.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlScala.obj: $(NP2_STL)\stlScala.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlSmali.obj: $(NP2_STL)\stlSmali.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlSQL.obj: $(NP2_STL)\stlSQL.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlTcl.obj: $(NP2_STL)\stlTcl.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlVB.obj: $(NP2_STL)\stlVB.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlVBS.obj: $(NP2_STL)\stlVBS.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlVerilog.obj: $(NP2_STL)\stlVerilog.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlVHDL.obj: $(NP2_STL)\stlVHDL.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlVim.obj: $(NP2_STL)\stlVim.c $(STL_HEADERS)
$(NP2_STL_OBJDIR)\stlXML.obj: $(NP2_STL)\stlXML.c $(STL_HEADERS)
