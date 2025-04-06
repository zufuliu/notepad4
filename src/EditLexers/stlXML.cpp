#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_XML = {{
//++Autogenerated -- start of section automatically generated
nullptr

, // 1 void tag
nullptr

, // 2 JavaScript
nullptr

, // 3 VBScript
nullptr

, // 4 SGML
"ANY ATTLIST CDATA DOCTYPE ELEMENT EMPTY ENTITIES ENTITY FIXED ID IDREF IDREFS IGNORE IMPLIED INCLUDE "
"NDATA NMTOKEN NMTOKENS NOTATION PCDATA PUBLIC REQUIRED SGML SYSTEM doctype "

, // 5 attribute
"alternate charset encoding href id media name standalone title type value version "
"xml xml-stylesheet xml:lang xml:space xmlns xmlns:xsi xsi:noNamespaceSchemaLocation xsi:schemaLocation "

, // 6 value
"ISO-8859-1 UTF-8 default false no off on preserve true utf-8 yes "

, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
//--Autogenerated -- end of section automatically generated
}};

static EDITSTYLE Styles_XML[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_H_TAG, SCE_H_TAGUNKNOWN, SCE_H_TAGEND, SCE_H_SCRIPT), NP2StyleX_XMLTag, L"fore:#8B008B" },
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
//Settings++Autogenerated -- start of section automatically generated
		LexerAttr_Default,
		TAB_WIDTH_4, INDENT_WIDTH_4,
		(1 << 0) | (1 << 1) | (1 << 2) | (1 << 3), // level1, level2, level13, level4
		0,
		'\0', 0, 0,
		0,
		0, 0,
		0, 0
		, KeywordAttr32(4, KeywordAttr_PreSorted) // SGML
		| KeywordAttr32(5, KeywordAttr_NoLexer) // attribute
		| KeywordAttr32(6, KeywordAttr_NoLexer) // value
		, 0,
		0, 0,
//Settings--Autogenerated -- end of section automatically generated
	EDITLEXER_HOLE(L"XML Document", Styles_XML),
	L"xml; xsl; xslt; xsd; dtd; rss; svg; xul; axl; rdf; xaml; resx; plist; pom; mm; xrc; fbp; wxml",
	&Keywords_XML,
	Styles_XML
};
