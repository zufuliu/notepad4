#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_Gradle = {
"as assert break case catch class continue const def default do else enum extends "
"finally for goto if in implements import instanceof interface native new package "
"return switch threadsafe throw throws try while abstract final private protected "
"public static strictfp synchronized transient "
"volatile false null super this true it"

, // type keyword
"boolean byte char class double float int long short void "
, // preprocessor
""
, // annotation
"Managed Model  "
, // attribute
// Task types
""
, // class
"String Object "
"Project Task Gradle Settings Script SourceSet "
"Copy "
, // interface
"List Map Set "
, // Action
"apply task "
, // Properties
/// Project
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

#if NUMKEYWORD == 16
,
""
,
""
, // Methods
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

,
//
""
,
""
,
""
, // auto complete
"for^() if^() switch^() while^() catch^() else^if^()  "
"def^() "

// Script blocks
"allprojects^{} ant^{} artifacts^{} buildscript^{} configurations^{} dependencies^{} repositories^{} subprojects^{} "
// java
"reporting^{} sourceSets^{} "
// publishing
"publishing^{} "
// signing
"signing^{} "

// SourceSet
"java&{} resources^{}"

#endif
};

EDITLEXER lexGradle = { SCLEX_CPP, NP2LEX_GRADLE, L"Gradle Build Script", L"gradle", L"", &Keywords_Gradle,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_DIRECTIVE, NP2STYLE_Annotation, L"Annotation", L"fore:#FF8000", L""},
	{ SCE_C_CLASS, NP2STYLE_Class, L"Class", L"fore:#0080FF", L"" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, L"Interface", L"bold; fore:#1E90FF", L""},
	{ SCE_C_FUNCTION, NP2STYLE_Method, L"Method", L"fore:#A46000", L"" },
	{ SCE_C_ENUMERATION, NP2STYLE_Enumeration, L"Action", L"fore:#FF8000", L""},
	{ SCE_C_CONSTANT, NP2STYLE_Constant, L"Properties", L"fore:#B000B0", L""},
	{ MULTI_STYLE(SCE_C_COMMENT,SCE_C_COMMENTLINE,0,0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"bold; fore:#008000F", L"" },
	{ SCE_C_COMMENTDOC_TAG_XML, NP2STYLE_DocCommentTagHTML, L"Doc Comment HTML Tag", L"bold; fore:#008000F", L"" },
	{ MULTI_STYLE(SCE_C_STRING,SCE_C_CHARACTER,SCE_C_STRINGEOL,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_TRIPLEVERBATIM, NP2STYLE_TripleString, L"Triple Quoted String", L"fore:#F08000", L"" },
	{ SCE_C_REGEX, NP2STYLE_Regex, L"Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_C_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L""},
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
