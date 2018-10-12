/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* version.h
*   metapath version information
*
* See Readme.txt for more information about this source code.
* Please send me your comments to this work.
*
* See License.txt for details about distribution and modification.
*
*                                              (c) Florian Balmer 1996-2011
*                                                  florian.balmer@gmail.com
*                                               http://www.flos-freeware.ch
*
*
******************************************************************************/

#ifndef METAPATH_VERSION_H_
#define METAPATH_VERSION_H_

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
#define VERSION_MINOR		0
#define VERSION_BUILD		13

#define MY_APPNAME					L"metapath"
#define VERSION_FILEVERSION_NUM		VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,VERSION_REV
#define VERSION_FILEVERSION			STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." \
									STRINGIFY(VERSION_BUILD) "." STRINGIFY(VERSION_REV)
#define VERSION_LEGALCOPYRIGHT_SHORT	L"Copyright \xA9 1996-2018"
#define VERSION_LEGALCOPYRIGHT_LONG		L"\xA9 1996-2018 Florian Balmer"
#define VERSION_AUTHORNAME			L"Florian Balmer et al."
#define VERSION_COMPANYNAME			L"Florian Balmer et al."
#define VERSION_WEBPAGE_DISPLAY		L"flo's freeware - http://www.flos-freeware.ch"
#define VERSION_EMAIL_DISPLAY		L"florian.balmer@gmail.com"
#define VERSION_NEWPAGE_DISPLAY		L"https://github.com/zufuliu/notepad2"

#if defined(__clang__)
#define VERSION_BUILD_TOOL			L"Clang " __clang_version__
#elif defined(__GNUC__)
#define VERSION_BUILD_TOOL			L"GCC " STRINGIFY(__GNUC__) L"." STRINGIFY(__GNUC_MINOR__) L"." STRINGIFY(__GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#define VERSION_BUILD_TOOL			L"MSVC " STRINGIFY(_MSC_FULL_VER)
#else
#define VERSION_BUILD_TOOL			L"Unknown Tool"
#endif
#define VERSION_BUILD_INFO			L"Compiled on " __DATE__ L" with " VERSION_BUILD_TOOL L"."

#if defined(_WIN64)
#define VERSION_FILEVERSION_LONG	L"metapath (64-bit) " STRINGIFY(VERSION_MAJOR) L"." \
									STRINGIFY(VERSION_MINOR) L"." STRINGIFY(VERSION_BUILD)  \
									L" " VERSION_REV_FULL
#else
#define VERSION_FILEVERSION_LONG	L"metapath " STRINGIFY(VERSION_MAJOR) L"."         \
									STRINGIFY(VERSION_MINOR) L"." STRINGIFY(VERSION_BUILD) \
									L" " VERSION_REV_FULL
#endif

#endif // METAPATH_VERSION_H_
