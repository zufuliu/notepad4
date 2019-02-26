#include "EditLexer.h"
#include "EditStyle.h"

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
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, EDITSTYLE_HOLE(L"Type Keyword"), L"fore:#0000FF" },
	{ SCE_C_DIRECTIVE, NP2STYLE_Annotation, EDITSTYLE_HOLE(L"Annotation"), L"fore:#FF8000" },
	{ SCE_C_CLASS, NP2STYLE_Class, EDITSTYLE_HOLE(L"Class"), L"fore:#0080FF" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, EDITSTYLE_HOLE(L"Interface"), L"bold; fore:#1E90FF" },
	{ SCE_C_FUNCTION, NP2STYLE_Method, EDITSTYLE_HOLE(L"Method"), L"fore:#A46000" },
	{ SCE_C_ENUMERATION, NP2STYLE_Enumeration, EDITSTYLE_HOLE(L"Action"), L"fore:#FF8000" },
	{ SCE_C_CONSTANT, NP2STYLE_Constant, EDITSTYLE_HOLE(L"Property"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#008000" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, EDITSTYLE_HOLE(L"Doc Comment Tag"), L"bold; fore:#008000F" },
	{ SCE_C_COMMENTDOC_TAG_XML, NP2STYLE_DocCommentTagHTML, EDITSTYLE_HOLE(L"Doc Comment HTML Tag"), L"bold; fore:#008000F" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, 0, 0), NP2STYLE_DocComment, EDITSTYLE_HOLE(L"Doc Comment"), L"fore:#008000" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_TRIPLEVERBATIM, NP2STYLE_TripleString, EDITSTYLE_HOLE(L"Triple Quoted String"), L"fore:#F08000" },
	{ SCE_C_REGEX, NP2STYLE_Regex, EDITSTYLE_HOLE(L"Regex"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"fore:#000000; back:#FFC040" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexGradle = {
	SCLEX_CPP, NP2LEX_GRADLE,
	EDITLEXER_HOLE(L"Gradle Build Script", Styles_Gradle),
	L"gradle",
	&Keywords_Gradle,
	Styles_Gradle
};

