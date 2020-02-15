#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_XML = {{
// Tag
NULL

, // 1 JavaScript
NULL

, // 2 VBScript
NULL

, // 3 Python
NULL

, // 4 PHP
NULL

, // 5 SGML/DTD
"doctype DOCTYPE PUBLIC SYSTEM "
"ELEMENT ATTLIST ENTITY "
"ANY CDATA EMPT ENTITIES ID IDREF IDREFS NDATA NOTATION NMTOKEN NMTOKENS "
"IMPLIED FIXED PCDATA REQUIRED "

"SGML "

, // 6 Attribute
"xml xmlns xsi "
"version encoding standalone noNamespaceSchemaLocation schemaLocation "
"lang id name type value "

, // 7 Value
"utf-8 UTF-8 ISO-8859-1 "
"true false yes no on off "

,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_XML[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_H_TAG, SCE_H_TAGUNKNOWN, SCE_H_TAGEND, 0), NP2StyleX_XMLTag, L"fore:#8B008B" },
	{ MULTI_STYLE(SCE_H_ATTRIBUTE, SCE_H_ATTRIBUTEUNKNOWN, 0, 0), NP2StyleX_XMLAttribute, L"fore:#FF0000" },
	{ MULTI_STYLE(SCE_H_VALUE, SCE_H_SGML_SPECIAL, 0, 0), NP2StyleX_XMLValue, L"fore:#1A1AA6" },
	{ MULTI_STYLE(SCE_H_DOUBLESTRING, SCE_H_SINGLESTRING, SCE_H_SGML_DOUBLESTRING, SCE_H_SGML_SIMPLESTRING), NP2StyleX_XMLString, L"fore:#008000" },
	{ MULTI_STYLE(SCE_H_OTHER, SCE_H_SGML_1ST_PARAM, 0, 0), NP2StyleX_XMLOtherTag, L"fore:#648000" },
	{ MULTI_STYLE(SCE_H_COMMENT, SCE_H_XCCOMMENT, SCE_H_SGML_COMMENT, SCE_H_SGML_1ST_PARAM_COMMENT), NP2StyleX_XMLComment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_H_ENTITY, SCE_H_SGML_ENTITY, SCE_H_SGML_ERROR, 0), NP2StyleX_XMLEntity, L"fore:#A46000" },
	{ SCE_H_DEFAULT, NP2StyleX_XMLElementText, L"" },
	{ MULTI_STYLE(SCE_H_XMLSTART, SCE_H_XMLEND, SCE_H_SGML_COMMAND, 0), NP2StyleX_XMLIdentifier, L"bold; fore:#881280" },
	{ MULTI_STYLE(SCE_H_SGML_DEFAULT, SCE_H_SGML_BLOCK_DEFAULT, 0, 0), NP2StyleX_XMLSGML, L"fore:#881280" },
	{ SCE_H_CDATA, NP2StyleX_XMLCDATA, L"fore:#646464" },
};

EDITLEXER lexXML = {
	SCLEX_XML, NP2LEX_XML,
	EDITLEXER_HOLE(L"XML Document", Styles_XML),
	L"xml; xsl; xslt; xsd; dtd; rss; svg; xul; axl; rdf; xaml; resx; plist; pom; mm; xrc; fbp",
	&Keywords_XML,
	Styles_XML
};
