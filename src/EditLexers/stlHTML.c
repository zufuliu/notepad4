#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_HTML = {{
// Tag
"!doctype !DOCTYPE a abbr address area article aside audio b base bdi bdo "
"blockquote body br button canvas caption cite code col colgroup command datalist "
"dd del details dfn dialog div dl dt em embed fieldset figcaption figure footer "
"form h1 h2 h3 h4 h5 h6 head header hgroup hr html i iframe img input ins kbd "
"keygen label legend li link map mark menu meta meter nav noscript object ol "
"optgroup option output p param pre progress q rp rt ruby s samp script section "
"select small source span strong style sub summary sup table tbody td textarea "
"tfoot th thead time title tr track u ul var video wbr "
"math svg "
// Other Tag
"acronym applet basefont big center font frame frameset isindex noframes strike tt "
"datagrid datatemplate dir eventsource "
// Attribute
"abbr accept accept-charset accesskey action alt async autocomplete autofocus autoplay "
"border challenge charset checked cite class cols colspan command content "
"contenteditable contextmenu controls coords crossorigin data datetime default "
"defer dir dirname disabled draggable dropzone enctype for form formaction "
"formenctype formmethod formnovalidate formtarget headers height hidden high href hreflang "
"http-equiv icon id ismap keytype kind label lang list loop low manifest max "
"maxlength media mediagroup method min multiple muted name novalidate open "
"optimum pattern placeholder poster preload radiogroup readonly rel required "
"reversed rows rowspan sandbox spellcheck scope scoped seamless selected shape size span "
"src srcdoc srclang start step style tabindex target title translate type "
"typemustmatch usemap value width wrap "
// Event Handle
"onabort onafterprint onbeforeprint onbeforeunload onblur oncancel oncanplay oncanplaythrough onchange onclick onclose "
"oncontextmenu oncuechange ondblclick ondrag ondragend ondragenter ondragleave "
"ondragover ondragstart ondrop ondurationchange onemptied onended onerror "
"onfocus onhashchange oninput oninvalid onkeydown onkeypress onkeyup onload onloadeddata "
"onloadedmetadata onloadstart onmousedown onmousemove onmouseout onmouseover "
"onmouseup onmousewheel onoffline ononline onpagehide onpageshow onpause onplay onplaying onprogress onratechange onreset "
"onscroll onseeked onseeking onselect onshow onstalled onstorage onsubmit onsuspend "
"ontimeupdate onvolumechange onunload onvolumechange onwaiting "
// Other Attribute
"align alink and archive axis background bb bgcolor bordercolor cellpadding "
"cellspacing char charoff classid clear codebase codetype color "
"compact datafld dataformatas datapagesize datasrc declare event face file "
"frameborder hspace image isindex language leftmargin longdesc "
"marginheight marginwidth noframes nohref noresize noshade nowrap "
"onformchange onforminput onmessage onpopstate onreadystatechange onredo onresize onundo "
"password ping profile prompt pubdate public  rev rules scheme "
"scrolling sizes standby strike text topmargin tt "
"valign valuetype version vlink vspace xml xmlns "
// Value
"on off true false anonymous use-credentials ltr rtl auto "
"application/x-www-form-urlencoded multipart/form-data text/plain GET POST "
"subtitles captions descriptions chapters metadata none "
"allow-forms allow-popups allow-same-origin allow-scripts allow-top-navigation "
"row col rowgroup colgroup circle default poly rect any yes no submit reset button "
"command checkbox radio context toolbar soft hard "

, // JavaScript
"async await break case catch class const continue debugger default delete do else export extends finally for function if import in "
"instanceof let new return static super switch this throw try typeof var void while with yield "
"null true false undefined "
"enum implements interface package private protected public "
, // VBScript
"Alias And As Attribute Begin Boolean ByRef Byte ByVal Call Case Class Compare Const Continue "
"Currency Date Declare Dim Do Double Each Else ElseIf Empty End Enum Eqv Erase Error Event Exit "
"Explicit False For Friend Function Get Global GoSub Goto If Imp Implement In Integer Is Let Lib "
"Load Long Loop LSet Me Mid Mod Module New Next Not Nothing Null Object On Option Optional Or "
"Preserve Private Property Public RaiseEvent ReDim Rem Resume Return RSet Select Set Single "
"Static Stop String Sub Then To True Type Unload Until Variant WEnd While With WithEvents Xor"
, // Python
""
, // PHP
"abstract and as break callable case catch class clone const continue declare default "
"do else elseif enddeclare endfor endforeach endif endswitch endwhile extends final "
"finally for foreach function global goto if implements instanceof insteadof interface "
"namespace new or parent private protected public return self static switch throw trait "
"try use var while xor yield "
"array bool boolean real double float int integer object string " // cast
"null true false NULL TRUE FALSE NAN " // literal
"define defined die echo empty eval exit include include_once isset list print "
"require require_once unset __halt_compiler "
// magic method
"__autoload __call __callStatic __clone __construct __destruct __get __invoke __isset "
"__set __set_state __sleep __toString __unset __wakeup "
// basic class/interface
"Traversable Iterator IteratorAggregate ArrayAccess Serializable Closure Generator "
"stdClass Exception ErrorException Directory php_user_filter DateTime "
//
"__CLASS__ __DIR__ __FILE__ __FUNCTION__ __LINE__ __METHOD__ __NAMESPACE__ __TRAIT__ "
, // SGML DTD
""
,
"", "", ""

#if NUMKEYWORD == 16
, "", "", "", "", "", "", ""
#endif
}};

EDITLEXER lexHTML = { SCLEX_HTML, NP2LEX_HTML, L"Web Source Code", L"html; htm; xhtml; shtml; asp; aspx; jsp; htd; htt; hta; htc", L"", &Keywords_HTML,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ MULTI_STYLE(SCE_H_TAG, SCE_H_TAGEND, 0, 0), 63161, L"HTML Tag", L"fore:#648000", L"" },
	{ SCE_H_TAGUNKNOWN, 63162, L"HTML Unknown Tag", L"fore:#C80000", L"" },
	{ SCE_H_ATTRIBUTE, 63163, L"HTML Attribute", L"fore:#FF4000", L"" },
	{ SCE_H_ATTRIBUTEUNKNOWN, 63164, L"HTML Unknown Attribute", L"fore:#C80000", L"" },
	{ SCE_H_VALUE, 63165, L"HTML Value", L"fore:#3A6EA5", L"" },
	{ MULTI_STYLE(SCE_H_DOUBLESTRING, SCE_H_SINGLESTRING, 0, 0), 63166, L"HTML String", L"fore:#3A6EA5", L"" },
	{ SCE_H_OTHER, 63167, L"HTML Other Tag", L"fore:#3A6EA5", L"" },
	{ MULTI_STYLE(SCE_H_COMMENT, SCE_H_XCCOMMENT, 0, 0), 63168, L"HTML Comment", L"fore:#808080", L"" },
	{ SCE_H_ENTITY, 63169, L"HTML Entity", L"fore:#B000B0", L"" },
	{ SCE_H_DEFAULT, 63170, L"HTML Element Text", L"", L"" },
	{ MULTI_STYLE(SCE_H_XMLSTART, SCE_H_XMLEND, 0, 0), NP2STYLE_XMLIdentifier, L"XML Identifier", L"bold; fore:#881280", L"" },
	{ SCE_H_SGML_DEFAULT, NP2STYLE_XMLSGML, L"SGML", L"fore:#881280", L"" },
	{ SCE_H_CDATA, NP2STYLE_XMLCDATA, L"CDATA", L"fore:#646464", L"" },
	{ MULTI_STYLE(SCE_H_ASP, SCE_H_ASPAT, 0, 0), 63172, L"ASP Tag", L"bold; fore:#8B008B", L"" },
	//{ SCE_H_SCRIPT, L"Script", L"", L"" },
	{ SCE_H_QUESTION, 63173, L"PHP Tag", L"bold; fore:#8B008B", L"" },
	/////////////////////////////////// PHP
	{ SCE_HPHP_DEFAULT, 63181, L"PHP Default", L"", L"" },
	{ MULTI_STYLE(SCE_HPHP_COMMENT, SCE_HPHP_COMMENTLINE, 0, 0), 63182, L"PHP Comment", L"fore:#008000", L"" },
	{ SCE_HPHP_WORD, 63183, L"PHP Keyword", L"bold; fore:#FF8000", L"" },
	{ MULTI_STYLE(SCE_HPHP_HSTRING, SCE_HPHP_SIMPLESTRING, 0, 0), 63184, L"PHP String", L"fore:#008000", L"" },
	{ SCE_HPHP_HEREDOC, 63185, L"PHP Heredoc String", L"fore:#648000", L"" },
	{ SCE_HPHP_NOWDOC, 63186, L"PHP Nowdoc String", L"fore:#A46000", L"" },
	{ SCE_HPHP_NUMBER, 63187, L"PHP Number", L"fore:#FF0000", L"" },
	{ SCE_HPHP_OPERATOR, 63188, L"PHP Operator", L"fore:#B000B0", L"" },
	{ SCE_HPHP_VARIABLE, 63189, L"PHP Variable", L"italic; fore:#003CE6", L"" },
	{ SCE_HPHP_HSTRING_VARIABLE, 63190, L"PHP String Variable", L"fore:#000080", L"" },
	{ SCE_HPHP_COMPLEX_VARIABLE, 63191, L"PHP Complex Variable", L"fore:#000080", L"" },
	////////////////////////////////// JavaScript
	{ MULTI_STYLE(SCE_HJ_DEFAULT, SCE_HJ_START, 0, 0), 63201, L"JS Default", L"", L"" },
	{ MULTI_STYLE(SCE_HJ_COMMENT, SCE_HJ_COMMENTLINE, SCE_HJ_COMMENTDOC, 0), 63202, L"JS Comment", L"fore:#008000", L"" },
	{ SCE_HJ_KEYWORD, 63203, L"JS Keyword", L"bold; fore:#FF8000", L"" },
	//{ SCE_HJ_WORD, 63204, L"JS Identifier", L"", L"" },
	{ MULTI_STYLE(SCE_HJ_DOUBLESTRING, SCE_HJ_SINGLESTRING, SCE_HJ_STRINGEOL, 0), 63205, L"JS String", L"fore:#008000", L"" },
	{ SCE_HJ_REGEX, 63206, L"JS Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_HJ_NUMBER, 63207, L"JS Number", L"fore:#FF0000", L"" },
	{ SCE_HJ_SYMBOLS, 63208, L"JS Symbols", L"fore:#B000B0", L"" },
	/////////////////////////////////// ASP Javascript
	{ MULTI_STYLE(SCE_HJA_DEFAULT, SCE_HJA_START, 0, 0), 63211, L"ASP JS Default", L"", L"" },
	{ MULTI_STYLE(SCE_HJA_COMMENT, SCE_HJA_COMMENTLINE, SCE_HJA_COMMENTDOC, 0), 63212, L"ASP JS Comment", L"fore:#008000", L"" },
	{ SCE_HJA_KEYWORD, 63213, L"ASP JS Keyword", L"bold; fore:#FF8000", L"" },
	//{ SCE_HJA_WORD, 63214, L"ASP JS Identifier", L"", L"" },
	{ MULTI_STYLE(SCE_HJA_DOUBLESTRING, SCE_HJA_SINGLESTRING, SCE_HJA_STRINGEOL, 0), 63215, L"ASP JS String", L"fore:#008000", L"" },
	{ SCE_HJA_REGEX, 63216, L"ASP JS Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_HJA_NUMBER, 63217, L"ASP JS Number", L"fore:#FF0000", L"" },
	{ SCE_HJA_SYMBOLS, 63218, L"ASP JS Symbols", L"fore:#B000B0", L"" },
	////////////////////////////////////// VBS
	{ MULTI_STYLE(SCE_HB_DEFAULT, SCE_HB_START, 0, 0), 63221, L"VBS Default", L"", L"" },
	{ SCE_HB_COMMENTLINE, 63222, L"VBS Comment", L"fore:#008000", L"" },
	{ SCE_HB_WORD, 63223, L"VBS Keyword", L"bold; fore:#FF8000", L"" },
	//{ SCE_HB_IDENTIFIER, 63224, L"VBS Identifier", L"", L"" },
	{ MULTI_STYLE(SCE_HB_STRING, SCE_HB_STRINGEOL, 0, 0), 63225, L"VBS String", L"fore:#008000", L"" },
	{ SCE_HB_NUMBER, 63226, L"VBS Number", L"fore:#FF0000", L"" },
	///////////////////////////////////// ASP VBS
	{ MULTI_STYLE(SCE_HBA_DEFAULT, SCE_HBA_START, 0, 0), 63231, L"ASP VBS Default", L"", L"" },
	{ SCE_HBA_COMMENTLINE, 63232, L"ASP VBS Comment", L"fore:#008000", L"" },
	{ SCE_HBA_WORD, 63233, L"ASP VBS Keyword", L"bold; fore:#FF8000", L"" },
	//{ SCE_HBA_IDENTIFIER, 63234, L"ASP VBS Identifier", L"", L"" },
	{ MULTI_STYLE(SCE_HBA_STRING, SCE_HBA_STRINGEOL, 0, 0), 63235, L"ASP VBS String", L"fore:#008000", L"" },
	{ SCE_HBA_NUMBER, 63236, L"ASP VBS Number", L"fore:#FF0000", L"" },
	//{ SCE_HP_START, L"Phyton Start", L"", L"" },
	//{ SCE_HP_DEFAULT, L"Phyton Default", L"", L"" },
	//{ SCE_HP_COMMENTLINE, L"Phyton Comment Line", L"", L"" },
	//{ SCE_HP_NUMBER, L"Phyton Number", L"", L"" },
	//{ SCE_HP_STRING, L"Phyton String", L"", L"" },
	//{ SCE_HP_CHARACTER, L"Phyton Character", L"", L"" },
	//{ SCE_HP_WORD, L"Phyton Keyword", L"", L"" },
	//{ SCE_HP_TRIPLE, L"Phyton Triple", L"", L"" },
	//{ SCE_HP_TRIPLEDOUBLE, L"Phyton Triple Double", L"", L"" },
	//{ SCE_HP_CLASSNAME, L"Phyton Class Name", L"", L"" },
	//{ SCE_HP_DEFNAME, L"Phyton Def Name", L"", L"" },
	//{ SCE_HP_OPERATOR, L"Phyton Operator", L"", L"" },
	//{ SCE_HP_IDENTIFIER, L"Phyton Identifier", L"", L"" },
	//{ SCE_HPA_START, L"ASP Phyton Start", L"", L"" },
	//{ SCE_HPA_DEFAULT, L"ASP Phyton Default", L"", L"" },
	//{ SCE_HPA_COMMENTLINE, L"ASP Phyton Comment Line", L"", L"" },
	//{ SCE_HPA_NUMBER, L"ASP Phyton Number", L"", L"" },
	//{ SCE_HPA_STRING, L"ASP Phyton String", L"", L"" },
	//{ SCE_HPA_CHARACTER, L"ASP Phyton Character", L"", L"" },
	//{ SCE_HPA_WORD, L"ASP Phyton Keyword", L"", L"" },
	//{ SCE_HPA_TRIPLE, L"ASP Phyton Triple", L"", L"" },
	//{ SCE_HPA_TRIPLEDOUBLE, L"ASP Phyton Triple Double", L"", L"" },
	//{ SCE_HPA_CLASSNAME, L"ASP Phyton Class Name", L"", L"" },
	//{ SCE_HPA_DEFNAME, L"ASP Phyton Def Name", L"", L"" },
	//{ SCE_HPA_OPERATOR, L"ASP Phyton Operator", L"", L"" },
	//{ SCE_HPA_IDENTIFIER, L"ASP Phyton Identifier", L"", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
