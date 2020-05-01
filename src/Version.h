/******************************************************************************
*
* Notepad2-mod
*
* Version.h
*
* See License.txt for details about distribution and modification.
*
*                                       (c) XhmikosR 2010-2014
*                                       https://github.com/XhmikosR/notepad2-mod
*
*
******************************************************************************/
#pragma once

#include "VersionRev.h"

#define DO_STRINGIFY(x)		TEXT(#x)
#define STRINGIFY(x)		DO_STRINGIFY(x)

#define VERSION_MAJOR				4

#define MY_APPNAME					L"Notepad2"
#define MY_APPNAME_DESCRIPTION		L"Notepad2 Text Editor"
#define VERSION_FILEVERSION_NUM		VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,VERSION_REV
#define VERSION_FILEVERSION			STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." \
									STRINGIFY(VERSION_BUILD) "." STRINGIFY(VERSION_REV)
#define VERSION_LEGALCOPYRIGHT_SHORT	L"Copyright \xA9 2004-2020"
#define VERSION_LEGALCOPYRIGHT_LONG		L"\xA9 Florian Balmer 2004-2020"
#define VERSION_AUTHORNAME			L"Florian Balmer et al."
#define VERSION_COMPANYNAME			L"Florian Balmer et al."
#define VERSION_WEBPAGE_DISPLAY		L"flo's freeware - http://www.flos-freeware.ch"
#define VERSION_EMAIL_DISPLAY		L"florian.balmer@gmail.com"
#define VERSION_MODPAGE_DISPLAY		L"https://xhmikosr.github.io/notepad2-mod/"
#define VERSION_NEWPAGE_DISPLAY		L"https://github.com/zufuliu/notepad2"
#define VERSION_SCIPAGE_DISPLAY		L"https://www.scintilla.org/"

#define HELP_LINK_LATEST_RELEASE	L"https://github.com/zufuliu/notepad2/releases"
#define HELP_LINK_LATEST_BUILD		L"https://ci.appveyor.com/project/zufuliu/notepad2"
#define HELP_LINK_REPORT_ISSUE		L"https://github.com/zufuliu/notepad2/issues"
#define HELP_LINK_FEATURE_REQUEST	L"https://github.com/zufuliu/notepad2/issues"
#define HELP_LINK_ONLINE_WIKI		L"https://github.com/zufuliu/notepad2/wiki"

#define VERSION_BUILD_INFO_LIB		L",\nScintilla 4.3.3"
#define VERSION_BUILD_INFO_FORMAT	L"Compiled on " __DATE__ L" with %s %d.%d.%d" VERSION_BUILD_INFO_LIB
#if defined(__clang__)
#define VERSION_BUILD_TOOL_NAME		L"Clang"
#define VERSION_BUILD_TOOL_MAJOR	__clang_major__
#define VERSION_BUILD_TOOL_MINOR	__clang_minor__
#define VERSION_BUILD_TOOL_PATCH	__clang_patchlevel__
#elif defined(__GNUC__)
#define VERSION_BUILD_TOOL_NAME		L"GCC"
#define VERSION_BUILD_TOOL_MAJOR	__GNUC__
#define VERSION_BUILD_TOOL_MINOR	__GNUC_MINOR__
#define VERSION_BUILD_TOOL_PATCH	__GNUC_PATCHLEVEL__
#elif defined(__INTEL_COMPILER_BUILD_DATE)
#if defined(__ICL)
#define VERSION_BUILD_TOOL_NAME		L"Intel C++ " STRINGIFY(__ICL)
#else
#define VERSION_BUILD_TOOL_NAME		L"Intel C++"
#endif
#define VERSION_BUILD_TOOL_MAJOR	(__INTEL_COMPILER_BUILD_DATE / 10000)
#define VERSION_BUILD_TOOL_MINOR	((__INTEL_COMPILER_BUILD_DATE / 100) % 100)
#define VERSION_BUILD_TOOL_PATCH	(__INTEL_COMPILER_BUILD_DATE % 100)
#undef VERSION_BUILD_INFO_FORMAT
#define VERSION_BUILD_INFO_FORMAT	L"Compiled on " __DATE__ L" with %s %4d-%02d-%02d" VERSION_BUILD_INFO_LIB
#elif defined(_MSC_VER)
#define VERSION_BUILD_TOOL_NAME		L"Visual C++"
#define VERSION_BUILD_TOOL_MAJOR	(_MSC_VER / 100) // 2-digit
#define VERSION_BUILD_TOOL_MINOR	(_MSC_VER % 100) // 2-digit
#define VERSION_BUILD_TOOL_PATCH	(_MSC_FULL_VER % 100000) // 5-digit
#define VERSION_BUILD_TOOL_BUILD	_MSC_BUILD // 2?-digit
#undef VERSION_BUILD_INFO_FORMAT
#define VERSION_BUILD_INFO_FORMAT	L"Compiled on " __DATE__ L" with %s %d.%02d.%05d.%d" VERSION_BUILD_INFO_LIB
#endif

#if defined(_WIN64)
	#if defined(__aarch64__) || defined(_ARM64_) || defined(_M_ARM64)
		#define VERSION_FILEVERSION_ARCH	L" (arm64) "
	#elif defined(__ia64__) || defined(_IA64_) || defined(_M_IA64)
		#define VERSION_FILEVERSION_ARCH	L" (ia64) "
	#elif defined(__AVX2__)
		#define VERSION_FILEVERSION_ARCH	L" (avx2) "
	#else
		#define VERSION_FILEVERSION_ARCH	L" (64-bit) "
	#endif
#else
	#if defined(__arm__) || defined(_ARM_) || defined(_M_ARM)
		#define VERSION_FILEVERSION_ARCH	L" (arm32) "
	#else
		#define VERSION_FILEVERSION_ARCH	L" (32-bit) "
	#endif
#endif
#define VERSION_FILEVERSION_LONG	MY_APPNAME VERSION_FILEVERSION_ARCH STRINGIFY(VERSION_MAJOR) L"." \
									STRINGIFY(VERSION_MINOR) L"." STRINGIFY(VERSION_BUILD) \
									L" " VERSION_REV_FULL
