#include "EditLexer.h"
#include "EditStyleX.h"

// https://www.mathworks.com/help/pdf_doc/matlab/matlab_prog.pdf
// https://www.mathworks.com/help/matlab/matlab_oop/specifying-attributes.html
// https://www.mathworks.com/help/matlab/ref/matlab.system-class.html
// https://www.gnu.org/software/octave/
// https://www.scilab.org/
// https://gnuplot.info/

static KEYWORDLIST Keywords_Matlab = {{
"break case catch classdef continue else elseif end for function global if otherwise parfor persistent return spmd switch try while "
"double single char logical int8 uint8 int16 uin16 int32 uint32 int64 uint64 cell struct function_handle "
"methods properties events enumeration public protected private mutable immutable internal import "
"all and any false not or true xor bitand bitor bitxor "
"sym syms complex real vpa scalar class typecast cast handle throw rethrow "
"typeinfo shared inline do until endfor endfunction endif endswitch end_try_catch end_unwind_protect endwhile unwind_protect unwind_protect_cleanup select then "
"off on eps pi ans Inf inf NaN nan null nargin nargout varargin varargout gca gcf gco gcbf gcbo intmax intmin realmax realmin "

, // 1 Attribte
"AbortSet Abstract Access AllowedSubclasses Constant ConstructOnLoad Dependent GetAccess GetObservable HandleCompatible Hidden InferiorClasses ListenAccess NonCopyable NotifyAccess Sealed SetAccess SetObservable Static Transient "
"DiscreteState Logical Nontunable PositiveInteger "

, // 2 Command
"clc clear delete demo exit load open pause quit save test testif tic toc which who whos "
"axis box cla clf close grid hold pan shg zoom "

, // 3 Basic Function
"assert() deal() disp() display() error() eval() evalc() evalin() fail() feval() find() get() input() length() warning() message() set() size() find() full() sparse() "
"isa() iscell() iscellstr() ischar() iscolumn() iscom() isdir() isempty() isequal() isfield() isfloat() isfinite() isglobal() ishandle() ishold() isindex() isinf() isinterface() isinteger() isjava() islogical() ismatrix() ismethod() ismember() isnan() isnumeric() isobject() isprop() isreal() isrow() isscalar() issparse() issquare() isstr() isstruct() isvalid() isvector() "

, // 4 UI
"axes figure uicontrol uimenu "

, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Matlab[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_MAT_KEYWORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_MAT_ATTRIBUTE, NP2StyleX_Attribute, L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_MAT_INTERNALCOMMAND, SCE_MAT_COMMAND, 0, 0), NP2StyleX_Command, L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_MAT_FUNCTION1, SCE_MAT_FUNCTION2, 0, 0), NP2StyleX_BasicFunction, L"fore:#FF0080" },
	{ SCE_MAT_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ SCE_MAT_CALLBACK, NP2StyleX_Callback, L"fore:#FF0080" },
	{ MULTI_STYLE(SCE_MAT_COMMENT, SCE_MAT_COMMENTBLOCK, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_MAT_STRING, SCE_MAT_DOUBLEQUOTESTRING, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ MULTI_STYLE(SCE_MAT_NUMBER, SCE_MAT_HEXNUM, 0, 0), NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_MAT_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexMatlab = {
	SCLEX_MATLAB, NP2LEX_MATLAB,
	EDITLEXER_HOLE(L"MATLAB Code", Styles_Matlab),
	L"m; sce; sci",
	&Keywords_Matlab,
	Styles_Matlab
};
