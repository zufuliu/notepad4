#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_HTML = {{
// Tag
"a abbr address area article aside audio b base bdi bdo "
"blockquote body br button canvas caption cite code col colgroup command datalist "
"data dd del details dfn dialog div dl dt em embed fieldset figcaption figure footer "
"form h1 h2 h3 h4 h5 h6 head header hgroup hr html i iframe img input ins kbd "
"keygen label legend li link main map mark meta meter nav noscript object ol "
"optgroup option output p param picture pre progress q rp rt ruby s samp script section "
"select slot small source span strong style sub summary sup table tbody td template textarea "
"tfoot th thead time title tr track u ul var video wbr "
"math svg "

// Other Tag
"datagrid datatemplate eventsource "

// Obsolete Elements
"applet acronym bgsound basefont big blink center dir font frame frameset isindex listing "
"marquee menu menuitem multicol nextid nobr noembed noframes plaintext rb rtc spacer strike tt xmp "

, // 1 JavaScript
"Infinity NaN arguments async await break case catch class const continue debugger default delete do else extends "
"false finally for function globalThis if in instanceof let new null return static super switch "
"this throw true try typeof undefined var void while with yield "
"enum implements interface package private protected public "
"__dirname __filename as export exports from import module require "

, // 2 VBScript
"Alias And As Attribute Begin Boolean ByRef Byte ByVal Call Case Class Compare Const Continue "
"Currency Date Declare Dim Do Double Each Else ElseIf Empty End Enum Eqv Erase Error Event Exit "
"Explicit False For Friend Function Get Global GoSub Goto If Imp Implement In Integer Is Let Lib "
"Load Long Loop LSet Me Mid Mod Module New Next Not Nothing Null Object On Option Optional Or "
"Preserve Private Property Public RaiseEvent ReDim Rem Resume Return RSet Select Set Single "
"Static Stop String Sub Then To True Type Unload Until Variant WEnd While With WithEvents Xor"

, // 3 Python
NULL

, // 4 PHP
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

, // 5 SGML/DTD
"doctype DOCTYPE PUBLIC SYSTEM "
"ELEMENT ATTLIST ENTITY "
"ANY CDATA EMPT ENTITIES ID IDREF IDREFS NDATA NOTATION NMTOKEN NMTOKENS "
"IMPLIED FIXED PCDATA REQUIRED "

"SGML "

, // 6 Attribute
// Attribute
"abbr accept accept-charset accesskey action allow allowfullscreen allowpaymentrequest alt as async autocapitalize autocomplete autofocus autoplay "
"border challenge charset checked cite class cols colspan command content "
"contenteditable contextmenu controls coords crossorigin data datetime decoding default "
"defer dir dirname disabled download draggable dropzone enctype enterkeyhint for form formaction "
"formenctype formmethod formnovalidate formtarget headers height hidden inputmode high href hreflang "
"http-equiv icon id imagesrcset imagesizes integrity is ismap itemid itemprop itemref itemscope itemtype "
"keytype kind label lang list loading longdesc loop low nonce minlength manifest max "
"maxlength media mediagroup method min multiple muted name nomodule novalidate open "
"optimum pattern placeholder playsinline poster preload radiogroup readonly referrerpolicy rel required "
"rev reversed rows rowspan sandbox sizes spellcheck scope scoped seamless selected shape size span "
"src srcdoc srclang srcset start step style tabindex target title translate type "
"typemustmatch usemap value width wrap "

// Other Attribute
"and bb face file password ping prompt pubdate public strike tt "
"xml xmlns "
"role ^aria- ^data- "
// RDFa
"property vocab resource typeof datatype "

// Obsolete Attributes
"align alink allowtransparency archive axis background bgcolor border bottommargin bordercolor cellpadding cellspacing "
"char charoff classid clear code codebase codetype color "
"compact datafld dataformatas datapagesize datasrc declare event "
"frame frameborder framespacing hspace image isindex language leftmargin link lowsrc "
"marginheight marginwidth methods noframes nohref noresize noshade nowrap "
"profile rightmargin rules scheme summary "
"scrolling standby text topmargin "
"urn valign valuetype version vlink vspace "

// Event Handle
"onaddtrack onabort onafterprint onauxclick onbeforeprint onbeforeunload onblur oncancel oncanplay oncanplaythrough onchange onclick onclose "
"oncontextmenu oncopy oncuechange oncut ondblclick ondrag ondragend ondragenter ondragexit ondragleave "
"ondragover ondragstart ondrop ondurationchange onemptied onended onerror onenter onexit "
"onformchange onforminput onmessage onmessageerror onpopstate onreadystatechange onredo onresize onundo "
"onfocus onformdata onhashchange oninput oninvalid onkeydown onkeypress onkeyup onlanguagechange onload onloadeddata "
"onloadedmetadata onloadend onloadstart onmousedown onmouseenter onmouseleave onmousemove onmouseout onmouseover "
"onmouseup onmousewheel onwheel onoffline ononline onpagehide onpageshow onpause onplay onplaying onprogress onpaste onratechange onrejectionhandled onremovetrack onreset "
"onscroll onsecuritypolicyviolation onseeked onseeking onselect onslotchange onshow onstalled onstorage onsubmit onsuspend "
"ontimeupdate ontoggle onvolumechange onunhandledrejection onunload onwaiting "

, // 7 Value
// metadata names
"application-name author description generator keywords referrer theme-color "
// Link types
"alternate canonical author bookmark dns-prefetch external help icon modulepreload license next nofollow noopener noreferrer opener "
"pingback preconnect prefetch preload prerender prev search stylesheet tag "
"about blob data http https mailto sms urn tel "
"_blank _self _parent _top "
// referrerpolicy, crossorigin
"never always origin-when-crossorigin "
"no-referrer no-referrer-when-downgrade unsafe-url origin-when-cross-origin " "anonymous use-credentials "
// http-equiv
"content-language content-type default-style refresh set-cookie content-security-policy "
// ol
"decimal lower-alpha upper-alpha lower-roman upper-roman "
// media preload, kind
"metadata auto " "subtitles captions descriptions chapters metadata "
// area shape
"circle circ default poly polygon rect rectangle "
// input type
"hidden text search url email password date month week time datetime-local number range color checkbox radio file submit image reset button "
// inputmode
"numeric "
// autocapitalize
"sentences words characters "
// Other Value, dir
"auto none on off true false yes no " "ltr rtl auto "
"application/x-www-form-urlencoded multipart/form-data text/plain GET POST "
"text/javascript module text/xml text/css application/octet-stream application/xml audio video image "
"allow-forms allow-popups allow-same-origin allow-scripts allow-top-navigation "
"row col rowgroup colgroup any "
"command context toolbar soft hard "

,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_HTML[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_H_TAG, SCE_H_TAGEND, 0, 0), NP2StyleX_HTMLTag, L"fore:#648000" },
	{ SCE_H_TAGUNKNOWN, NP2StyleX_HTMLUnknownTag, L"fore:#C80000" },
	{ SCE_H_ATTRIBUTE, NP2StyleX_HTMLAttribute, L"fore:#FF4000" },
	{ SCE_H_ATTRIBUTEUNKNOWN, NP2StyleX_HTMLUnknownAttribute, L"fore:#C80000" },
	{ MULTI_STYLE(SCE_H_VALUE, SCE_H_SGML_SPECIAL, 0, 0), NP2StyleX_HTMLValue, L"fore:#3A6EA5" },
	{ MULTI_STYLE(SCE_H_DOUBLESTRING, SCE_H_SINGLESTRING, SCE_H_SGML_DOUBLESTRING, SCE_H_SGML_SIMPLESTRING), NP2StyleX_HTMLString, L"fore:#3A6EA5" },
	{ MULTI_STYLE(SCE_H_OTHER, SCE_H_SGML_1ST_PARAM, 0, 0), NP2StyleX_HTMLOtherTag, L"fore:#0080C0" },
	{ MULTI_STYLE(SCE_H_COMMENT, SCE_H_XCCOMMENT, SCE_H_SGML_COMMENT, SCE_H_SGML_1ST_PARAM_COMMENT), NP2StyleX_HTMLComment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_H_ENTITY, SCE_H_SGML_ENTITY, SCE_H_SGML_ERROR, 0), NP2StyleX_HTMLEntity, L"fore:#A46000" },
	{ SCE_H_DEFAULT, NP2StyleX_HTMLElementText, L"" },
	{ MULTI_STYLE(SCE_H_XMLSTART, SCE_H_XMLEND, SCE_H_SGML_COMMAND, 0), NP2StyleX_XMLIdentifier, L"bold; fore:#881280" },
	{ MULTI_STYLE(SCE_H_SGML_DEFAULT, SCE_H_SGML_BLOCK_DEFAULT, 0, 0), NP2StyleX_XMLSGML, L"fore:#881280" },
	{ SCE_H_CDATA, NP2StyleX_XMLCDATA, L"fore:#646464" },
	{ MULTI_STYLE(SCE_H_ASP, SCE_H_ASPAT, 0, 0), NP2StyleX_ASPTag, L"bold; fore:#8B008B" },
	//{ SCE_H_SCRIPT, EDITSTYLE_HOLE(L"Script"), L"" },
	{ SCE_H_QUESTION, NP2StyleX_PHPTag, L"bold; fore:#8B008B" },
	/////////////////////////////////// PHP
	{ SCE_HPHP_DEFAULT, NP2StyleX_PHPDefault, L"" },
	{ MULTI_STYLE(SCE_HPHP_COMMENT, SCE_HPHP_COMMENTLINE, 0, 0), NP2StyleX_PHPComment, L"fore:#608060" },
	{ SCE_HPHP_WORD, NP2StyleX_PHPKeyword, L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_HPHP_HSTRING, SCE_HPHP_SIMPLESTRING, 0, 0), NP2StyleX_PHPString, L"fore:#008000" },
	{ SCE_HPHP_HEREDOC, NP2StyleX_PHPHeredocString, L"fore:#648000" },
	{ SCE_HPHP_NOWDOC, NP2StyleX_PHPNowdocString, L"fore:#A46000" },
	{ SCE_HPHP_NUMBER, NP2StyleX_PHPNumber, L"fore:#FF0000" },
	{ SCE_HPHP_OPERATOR, NP2StyleX_PHPOperator, L"fore:#B000B0" },
	{ SCE_HPHP_VARIABLE, NP2StyleX_PHPVariable, L"italic; fore:#003CE6" },
	{ SCE_HPHP_HSTRING_VARIABLE, NP2StyleX_PHPStringVariable, L"fore:#000080" },
	{ SCE_HPHP_COMPLEX_VARIABLE, NP2StyleX_PHPComplexVariable, L"fore:#000080" },
	////////////////////////////////// JavaScript
	{ MULTI_STYLE(SCE_HJ_DEFAULT, SCE_HJ_START, 0, 0), NP2StyleX_JSDefault, L"" },
	{ MULTI_STYLE(SCE_HJ_COMMENT, SCE_HJ_COMMENTLINE, SCE_HJ_COMMENTDOC, 0), NP2StyleX_JSComment, L"fore:#608060" },
	{ SCE_HJ_KEYWORD, NP2StyleX_JSKeyword, L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_HJ_DOUBLESTRING, SCE_HJ_SINGLESTRING, SCE_HJ_STRINGEOL, 0), NP2StyleX_JSString, L"fore:#008000" },
	{ SCE_HJ_REGEX, NP2StyleX_JSRegex, L"fore:#006633; back:#FFF1A8" },
	{ SCE_HJ_NUMBER, NP2StyleX_JSNumber, L"fore:#FF0000" },
	{ SCE_HJ_SYMBOLS, NP2StyleX_JSSymbol, L"fore:#B000B0" },
	/////////////////////////////////// ASP Javascript
	{ MULTI_STYLE(SCE_HJA_DEFAULT, SCE_HJA_START, 0, 0), NP2StyleX_ASPJSDefault, L"" },
	{ MULTI_STYLE(SCE_HJA_COMMENT, SCE_HJA_COMMENTLINE, SCE_HJA_COMMENTDOC, 0), NP2StyleX_ASPJSComment, L"fore:#608060" },
	{ SCE_HJA_KEYWORD, NP2StyleX_ASPJSKeyword, L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_HJA_DOUBLESTRING, SCE_HJA_SINGLESTRING, SCE_HJA_STRINGEOL, 0), NP2StyleX_ASPJSString, L"fore:#008000" },
	{ SCE_HJA_REGEX, NP2StyleX_ASPJSRegex, L"fore:#006633; back:#FFF1A8" },
	{ SCE_HJA_NUMBER, NP2StyleX_ASPJSNumber, L"fore:#FF0000" },
	{ SCE_HJA_SYMBOLS, NP2StyleX_ASPJSSymbol, L"fore:#B000B0" },
	////////////////////////////////////// VBS
	{ MULTI_STYLE(SCE_HB_DEFAULT, SCE_HB_START, 0, 0), NP2StyleX_VBSDefault, L"" },
	{ SCE_HB_COMMENTLINE, NP2StyleX_VBSComment, L"fore:#608060" },
	{ SCE_HB_WORD, NP2StyleX_VBSKeyword, L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_HB_STRING, SCE_HB_STRINGEOL, 0, 0), NP2StyleX_VBSString, L"fore:#008000" },
	{ SCE_HB_NUMBER, NP2StyleX_VBSNumber, L"fore:#FF0000" },
	///////////////////////////////////// ASP VBS
	{ MULTI_STYLE(SCE_HBA_DEFAULT, SCE_HBA_START, 0, 0), NP2StyleX_ASPVBSDefault, L"" },
	{ SCE_HBA_COMMENTLINE, NP2StyleX_ASPVBSComment, L"fore:#608060" },
	{ SCE_HBA_WORD, NP2StyleX_ASPVBSKeyword, L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_HBA_STRING, SCE_HBA_STRINGEOL, 0, 0), NP2StyleX_ASPVBSString, L"fore:#008000" },
	{ SCE_HBA_NUMBER, NP2StyleX_ASPVBSNumber, L"fore:#FF0000" },
	//{ SCE_HP_DEFAULT, EDITSTYLE_HOLE(L"Phyton Default"), L"" },
	//{ SCE_HP_COMMENTLINE, EDITSTYLE_HOLE(L"Phyton Comment Line"), L"" },
	//{ SCE_HP_NUMBER, EDITSTYLE_HOLE(L"Phyton Number"), L"" },
	//{ SCE_HP_STRING, EDITSTYLE_HOLE(L"Phyton String"), L"" },
	//{ SCE_HP_CHARACTER, EDITSTYLE_HOLE(L"Phyton Character"), L"" },
	//{ SCE_HP_WORD, EDITSTYLE_HOLE(L"Phyton Keyword"), L"" },
	//{ SCE_HP_TRIPLE, EDITSTYLE_HOLE(L"Phyton Triple"), L"" },
	//{ SCE_HP_TRIPLEDOUBLE, EDITSTYLE_HOLE(L"Phyton Triple Double"), L"" },
	//{ SCE_HP_CLASSNAME, EDITSTYLE_HOLE(L"Phyton Class Name"), L"" },
	//{ SCE_HP_DEFNAME, EDITSTYLE_HOLE(L"Phyton Def Name"), L"" },
	//{ SCE_HP_OPERATOR, EDITSTYLE_HOLE(L"Phyton Operator"), L"" },
	//{ SCE_HPA_DEFAULT, EDITSTYLE_HOLE(L"ASP Phyton Default"), L"" },
	//{ SCE_HPA_COMMENTLINE, EDITSTYLE_HOLE(L"ASP Phyton Comment Line"), L"" },
	//{ SCE_HPA_NUMBER, EDITSTYLE_HOLE(L"ASP Phyton Number"), L"" },
	//{ SCE_HPA_STRING, EDITSTYLE_HOLE(L"ASP Phyton String"), L"" },
	//{ SCE_HPA_CHARACTER, EDITSTYLE_HOLE(L"ASP Phyton Character"), L"" },
	//{ SCE_HPA_WORD, EDITSTYLE_HOLE(L"ASP Phyton Keyword"), L"" },
	//{ SCE_HPA_TRIPLE, EDITSTYLE_HOLE(L"ASP Phyton Triple"), L"" },
	//{ SCE_HPA_TRIPLEDOUBLE, EDITSTYLE_HOLE(L"ASP Phyton Triple Double"), L"" },
	//{ SCE_HPA_CLASSNAME, EDITSTYLE_HOLE(L"ASP Phyton Class Name"), L"" },
	//{ SCE_HPA_DEFNAME, EDITSTYLE_HOLE(L"ASP Phyton Def Name"), L"" },
	//{ SCE_HPA_OPERATOR, EDITSTYLE_HOLE(L"ASP Phyton Operator"), L"" },
};

EDITLEXER lexHTML = {
	SCLEX_HTML, NP2LEX_HTML,
	EDITLEXER_HOLE(L"Web Source Code", Styles_HTML),
	L"html; htm; shtml; xhtml; asp; aspx; jsp; mht; htd; htt; hta; htc; cfm; tpl; jd",
	&Keywords_HTML,
	Styles_HTML
};
