#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_XML = {{
NULL, NULL, NULL

, // 4 Tag
"xml xmlns xsi DOCTYPE doctype PUBLIC "

, // 5 Attribute
"version encoding standalone noNamespaceSchemaLocation schemaLocation CDATA "
"lang id name type value "

, // 6 Value
"utf-8 UTF-8 ISO-8859-1 "
"true false yes no on off "

, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_XML[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_H_TAG, SCE_H_TAGUNKNOWN, SCE_H_TAGEND, 0), NP2StyleX_XMLTag, L"fore:#8B008B" },
	{ MULTI_STYLE(SCE_H_ATTRIBUTE, SCE_H_ATTRIBUTEUNKNOWN, 0, 0), NP2StyleX_XMLAttribute, L"fore:#FF0000" },
	{ SCE_H_VALUE, NP2StyleX_XMLValue, L"fore:#1A1AA6" },
	{ MULTI_STYLE(SCE_H_DOUBLESTRING, SCE_H_SINGLESTRING, SCE_H_SGML_DOUBLESTRING, SCE_H_SGML_SIMPLESTRING), NP2StyleX_XMLString, L"fore:#008000" },
	{ SCE_H_OTHER, NP2StyleX_XMLOtherTag, L"fore:#1A1AA6" },
	{ MULTI_STYLE(SCE_H_COMMENT, SCE_H_XCCOMMENT, SCE_H_SGML_COMMENT, SCE_H_SGML_1ST_PARAM_COMMENT), NP2StyleX_XMLComment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_H_ENTITY, SCE_H_SGML_ERROR, 0, 0), NP2StyleX_XMLEntity, L"fore:#B000B0" },
	{ SCE_H_DEFAULT, NP2StyleX_XMLElementText, L"" },
	{ MULTI_STYLE(SCE_H_XMLSTART, SCE_H_XMLEND, 0, 0), NP2StyleX_XMLIdentifier, L"bold; fore:#881280" },
	{ MULTI_STYLE(SCE_H_SGML_DEFAULT, SCE_H_SGML_BLOCK_DEFAULT, SCE_H_SGML_COMMAND, SCE_H_SGML_ENTITY), NP2StyleX_XMLSGML, L"fore:#881280" },
	{ SCE_H_CDATA, NP2StyleX_XMLCDATA, L"fore:#646464" },
};

EDITLEXER lexXML = {
	SCLEX_XML, NP2LEX_XML,
	EDITLEXER_HOLE(L"XML Document", Styles_XML),
	L"xml; xsl; xslt; xsd; dtd; rss; svg; xul; axl; rdf; xaml; resx; plist; pom; mm; xrc; fbp",
	&Keywords_XML,
	Styles_XML
};
