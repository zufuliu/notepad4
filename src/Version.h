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

#ifndef NOTEPAD2_VERSION_H
#define NOTEPAD2_VERSION_H

#include "VersionRev.h"

#ifndef _T
#if !defined(ISPP_INVOKED) && (defined(UNICODE) || defined(_UNICODE))
#define _T(text)	L##text
#else
#define _T(text)	text
#endif
#endif

#define DO_STRINGIFY(x)		_T(#x)
#define STRINGIFY(x)		DO_STRINGIFY(x)

#define VERSION_MAJOR		4
#define VERSION_MINOR		2
#define VERSION_BUILD		25

#define MY_APPNAME					L"Notepad2"
#define VERSION_FILEVERSION_NUM		VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,VERSION_REV
#define VERSION_FILEVERSION			STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." \
									STRINGIFY(VERSION_BUILD) "." STRINGIFY(VERSION_REV)
#define VERSION_LEGALCOPYRIGHT_SHORT	L"Copyright \xA9 2004-2018"
#define VERSION_LEGALCOPYRIGHT_LONG		L"\xA9 Florian Balmer 2004-2018"
#define VERSION_AUTHORNAME			L"Florian Balmer et al."
#define VERSION_COMPANYNAME			L"Florian Balmer et al."
#define VERSION_WEBPAGE_DISPLAY		L"flo's freeware - http://www.flos-freeware.ch"
#define VERSION_EMAIL_DISPLAY		L"florian.balmer@gmail.com"
#define VERSION_MODPAGE_DISPLAY		L"https://xhmikosr.github.io/notepad2-mod/"
#define VERSION_NEWPAGE_DISPLAY		L"https://github.com/zufuliu/notepad2"
#define VERSION_SCIPAGE_DISPLAY		L"http://www.scintilla.org/"

#define HELP_LINK_LATEST_RELEASE	L"https://github.com/zufuliu/notepad2/releases"
#define HELP_LINK_REPORT_ISSUE		L"https://github.com/zufuliu/notepad2/issues"
#define HELP_LINK_FEATURE_REQUEST	L"https://github.com/zufuliu/notepad2/issues"
#define HELP_LINK_ONLINE_WIKI		L"https://github.com/zufuliu/notepad2/wiki"

#if defined(__clang__)
#define VERSION_BUILD_TOOL			L"Clang " __clang_version__
#elif defined(__GNUC__)
#define VERSION_BUILD_TOOL			L"GCC " STRINGIFY(__GNUC__) L"." STRINGIFY(__GNUC_MINOR__) L"." STRINGIFY(__GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#define VERSION_BUILD_TOOL			L"MSVC " STRINGIFY(_MSC_FULL_VER)
#else
#define VERSION_BUILD_TOOL			L"Unknown Tool"
#endif
#define VERSION_BUILD_INFO			L"Compiled on " __DATE__ L" with " VERSION_BUILD_TOOL L",\n" \
									L"Scintilla 4.1.2" L"."

#if defined(_WIN64)
#define VERSION_FILEVERSION_LONG	L"Notepad2 (64-bit) " STRINGIFY(VERSION_MAJOR) L"." \
									STRINGIFY(VERSION_MINOR) L"." STRINGIFY(VERSION_BUILD)  \
									L" " VERSION_REV_FULL
#else
#define VERSION_FILEVERSION_LONG	L"Notepad2 " STRINGIFY(VERSION_MAJOR) L"."         \
									STRINGIFY(VERSION_MINOR) L"." STRINGIFY(VERSION_BUILD) \
									L" " VERSION_REV_FULL
#endif

#endif // NOTEPAD2_VERSION_H

// End of Version.h
