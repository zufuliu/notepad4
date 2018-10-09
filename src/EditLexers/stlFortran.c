#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikibooks.org/wiki/Fortran

static KEYWORDLIST Keywords_Fortran = {{
"access action advance allocatable allocate apostrophe assign assignment associate asynchronous backspace bind blank blockdata call case character class common complex contains continue cycle data deallocate decimal delim default dimension direct do dowhile double doubleprecision else elseif elsewhere encoding end endassociate endblockdata enddo endfile endforall endfunction endif endinterface endmodule endprogram endselect endsubroutine endtype endwhere entry eor equivalence err errmsg exist exit external file flush fmt forall form formatted function go goto id if implicit in include inout integer inquire intent interface intrinsic iomsg iolength iostat kind len logical module name named namelist nextrec nml none nullify number only opened operator optional out pad parameter pass pause pending pointer pos position precision private program protected public quote readwrite real rec recl recursive result return rewind save select selectcase selecttype sequential sign size stat status stop stream subroutine target then to type unformatted unit use value volatile wait where while "

"mwpointer mwsize "

, // 1 Basic Function
"close format open print read write"

, NULL, NULL, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};


EDITLEXER lexFortran = { SCLEX_FORTRAN, NP2LEX_FORTRAN, EDITLEXER_HOLE(L"Fortran Source"), L"f; for; ftn; fpp; f90; f95; f03; f08; hf", &Keywords_Fortran,
{
	EDITSTYLE_DEFAULT,
	{ SCE_F_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_F_WORD2, NP2STYLE_BasicFunction, EDITSTYLE_HOLE(L"Basic Function"), L"fore:#0080FF" },
	{ SCE_F_COMMENT, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_F_STRING1, SCE_F_STRING2, SCE_F_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_F_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ MULTI_STYLE(SCE_F_OPERATOR, SCE_F_OPERATOR2, 0, 0), NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_F_PREPROCESSOR, NP2STYLE_Preprocessor, EDITSTYLE_HOLE(L"Preprocessor"), L"fore:#FF8000" },
	EDITSTYLE_SENTINEL
}
};

