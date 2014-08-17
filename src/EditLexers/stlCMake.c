#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_CMake = {
"break else elseif return "
"foreach function if macro while endforeach endfunction endif endmacro endwhile"
, // command
"add_custom_command add_custom_target add_definitions add_dependencies add_executable "
"add_library add_subdirectory add_test aux_source_directory build_command cmake_minimum_required "
"cmake_policy configure_file create_test_sourcelist define_property enable_language "
"enable_testing execute_process export file find_file find_library find_package "
"find_path find_program fltk_wrap_ui get_cmake_property get_directory_property "
"get_filename_component get_property get_source_file_property get_target_property "
"get_test_property include include_directories include_external_msproject "
"include_regular_expression install link_directories list load_cache load_command "
"mark_as_advanced math message option project qt_wrap_cpp qt_wrap_ui remove_definitions "
"separate_arguments set set_directory_properties set_property set_source_files_properties "
"set_target_properties set_tests_properties site_name source_group string target_link_libraries "
"try_compile try_run unset variable_watch"
, // Variables that Provide Information
""
, // Variables That Change Behavior
""
, // Variables That Describe the System
""
, "", "", "", ""

#if NUMKEYWORD == 16
,"","","","","","",""
#endif
};

EDITLEXER lexCMake = { SCLEX_CMAKE, NP2LEX_CMAKE, L"CMake Script", L"cmake; ctest", L"", &Keywords_CMake,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_CMAKE_DEFAULT, L"Default", L"", L"" },
	{ SCE_CMAKE_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_CMAKE_COMMANDS, NP2STYLE_Command, L"Command", L"fore:#FF8000", L"" },
	{ SCE_CMAKE_COMMENT, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_CMAKE_STRINGDQ,SCE_CMAKE_STRINGSQ,SCE_CMAKE_STRINGBT,0), NP2STYLE_String, L"String", L"fore:#008080", L"" },
	//{ SCE_CMAKE_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_CMAKE_VARIABLE, NP2STYLE_Variable, L"Variable", L"fore:#CC3300", L"" },
	{ SCE_CMAKE_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_CMAKE_FUNCATION, NP2STYLE_Function, L"Function", L"fore:#808000", L"" },
	{ SCE_CMAKE_MACRO, NP2STYLE_Macro, L"Macro", L"fore:#808000", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
