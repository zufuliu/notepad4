#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikibooks.org/wiki/Fortran

static KEYWORDLIST Keywords_Fortran = {
"access action advance allocatable allocate apostrophe assign assignment associate asynchronous backspace bind blank blockdata call case character class common complex contains continue cycle data deallocate decimal delim default dimension direct do dowhile double doubleprecision else elseif elsewhere encoding end endassociate endblockdata enddo endfile endforall endfunction endif endinterface endmodule endprogram endselect endsubroutine endtype endwhere entry eor equivalence err errmsg exist exit external file flush fmt forall form formatted function go goto id if implicit in include inout integer inquire intent interface intrinsic iomsg iolength iostat kind len logical module name named namelist nextrec nml none nullify number only opened operator optional out pad parameter pass pause pending pointer pos position precision private program protected public quote readwrite real rec recl recursive result return rewind save select selectcase selecttype sequential sign size stat status stop stream subroutine target then to type unformatted unit use value volatile wait where while "

"mwpointer mwsize ",
"close format open print read write",
"", "", "", "", "", "", ""

#if NUMKEYWORD == 16
,"","","","","","",""
#endif
};

EDITLEXER lexFortran = { SCLEX_FORTRAN, NP2LEX_FORTRAN, L"Fortran Source", L"f; for; f90; f95; f03; f08; hf", L"", &Keywords_Fortran,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_F_DEFAULT, L"Default", L"", L"" },
	{ SCE_F_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_F_WORD2, NP2STYLE_BasicFunction, L"Basic Function", L"fore:#0080FF", L"" },
	{ SCE_F_COMMENT, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_F_STRING1,SCE_F_STRING2,SCE_F_STRINGEOL,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_F_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ MULTI_STYLE(SCE_F_OPERATOR,SCE_F_OPERATOR2,0,0), NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_F_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF8000", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
