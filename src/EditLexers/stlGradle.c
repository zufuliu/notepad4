#include "EditLexer.h"
#include "EditStyleX.h"

// https://gradle.org/

static KEYWORDLIST Keywords_Gradle = {{
"as assert break case catch class continue const def default do else enum extends "
"finally for goto if in implements import instanceof interface native new package "
"return switch threadsafe throw throws try while abstract final private protected "
"public static strictfp synchronized transient "
"volatile false null super this true it"

, // 1 Type Keyword
"boolean byte char class double float int long short void "

, // 2 Preprocessor
NULL

, // 3 Annotation
"Managed Model "

, // 4 Attribute
// Task types
NULL

, // 5 Class
"String Object "
"Project Task Gradle Settings Script SourceSet "
"Copy "

, // 6 Interface
"List Map Set "

, // 7 Action
"apply task "

, // 8 Property
// Project
"allprojects ant artifacts buildDir buildFile buildscript childProjects configurations convention defaultTasks dependencies description extensions gradle group logger logging name parent path pluginManager plugins project projectDir properties repositories resources rootDir rootProject state status subprojects tasks version "
// java
"archivesBaseName distsDir distsDirName docsDir docsDirName libsDir libsDirName reporting sourceCompatibility sourceSets targetCompatibility testReportDir testReportDirName testResultsDir testResultsDirName "
// publishing
"publishing "
// signing
"signing "

// Task
"actions dependsOn didWork enabled inputs outputs taskDependencies temporaryDir "
// Settings
"settings settingsDir startParameter "
// SourceSet
"allJava allSource compileClasspath java output runtimeClasspath "
// Copy
"caseSensitive destinationDir dirMode duplicatesStrategy excludes fileMode includeEmptyDirs includes source "
// JavaCompile
"classpath options "

, // 9
NULL
, // 10
NULL
, // 11 Method
// Project
"absoluteProjectPath afterEvaluate allprojects beforeEvaluate configure container copy copySpec delete evaluationDependsOn exec file fileTree files findProject getAllTasks getTasksByName hasProperty javaexec mkdir project property relativePath relativeProjectPath setProperty subprojects tarTree task uri zipTree "
// java
"manifest "
// Task
"deleteAllActions dependsOn doFirst doLast leftShift onlyIf "
// Settings
"include includeFlat "
// SourceSet
"compiledBy getCompileTaskName getTaskName "
// Copy
"eachFile exclude expand filesMatching filesNotMatching filter from into rename with "
// JavaCompile
"source "

, // 12
NULL
, // 13
NULL
, // 14
NULL

, // 15 Code Snippet
"for^() if^() switch^() while^() catch^() else^if^() else^{} "
"def^() "

// Script block
"allprojects^{} ant^{} artifacts^{} buildscript^{} configurations^{} dependencies^{} repositories^{} subprojects^{} "
// java
"reporting^{} sourceSets^{} "
// publishing
"publishing^{} "
// signing
"signing^{} "

// SourceSet
"java&{} resources^{}"
}};

static EDITSTYLE Styles_Gradle[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_C_DIRECTIVE, NP2StyleX_Annotation, L"fore:#FF8000" },
	{ SCE_C_CLASS, NP2StyleX_Class, L"fore:#0080FF" },
	{ SCE_C_INTERFACE, NP2StyleX_Interface, L"bold; fore:#1E90FF" },
	{ SCE_C_FUNCTION, NP2StyleX_Method, L"fore:#A46000" },
	{ SCE_C_ENUMERATION, NP2StyleX_Action, L"fore:#FF8000" },
	{ SCE_C_CONSTANT, NP2StyleX_Property, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_C_COMMENTDOC_TAG, NP2StyleX_DocCommentTag, L"bold; fore:#408080" },
	{ SCE_C_COMMENTDOC_TAG_XML, NP2StyleX_DocCommentTagHTML, L"bold; fore:#808080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, 0, 0), NP2StyleX_DocComment, L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_TRIPLEVERBATIM, NP2StyleX_TripleQuotedString, L"fore:#F08000" },
	{ SCE_C_REGEX, NP2StyleX_Regex, L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_LABEL, NP2StyleX_Label, L"back:#FFC040" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexGradle = {
	SCLEX_CPP, NP2LEX_GRADLE,
	EDITLEXER_HOLE(L"Gradle Build Script", Styles_Gradle),
	L"gradle",
	&Keywords_Gradle,
	Styles_Gradle
};
