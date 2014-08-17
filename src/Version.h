/******************************************************************************
*
* Notepad2-mod
*
* Version.h
*
* See License.txt for details about distribution and modification.
*
*                                       (c) XhmikosR 2010-2012
*                                       https://github.com/XhmikosR/notepad2-mod
*
*
******************************************************************************/


#ifndef NOTEPAD2_VERSION_H
#define NOTEPAD2_VERSION_H

#include "VersionRev.h"

#ifndef _T
#if !defined(ISPP_INVOKED) && (defined(UNICODE) || defined(_UNICODE))
#define _T(text) L##text
#else
#define _T(text) text
#endif
#endif

#define DO_STRINGIFY(x) _T(#x)
#define STRINGIFY(x)		DO_STRINGIFY(x)

#define VERSION_MAJOR		4
#define VERSION_MINOR		2
#define VERSION_BUILD		25

#define MY_APPNAME						L"Notepad2"
#define VERSION_FILEVERSION_NUM			VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,VERSION_REV
#define VERSION_FILEVERSION				STRINGIFY(VERSION_MAJOR) ", " STRINGIFY(VERSION_MINOR) ", " STRINGIFY(VERSION_BUILD) ", " STRINGIFY(VERSION_REV)
#define VERSION_LEGALCOPYRIGHT_SHORT	L"Copyright 2004-2013"
#define VERSION_LEGALCOPYRIGHT_LONG		L"?Florian Balmer 2004-2013"
#define VERSION_AUTHORNAME				L"Florian Balmer"
#define VERSION_WEBPAGEDISPLAY			L"flo's freeware - http://www.flos-freeware.ch"
//#define VERSION_EMAILDISPLAY			L"florian.balmer@gmail.com"
#define VERSION_EMAILDISPLAY			L"http://www.scintilla.org/"
#define VERSION_COMPANYNAME				L"Florian Balmer et all"
#define VERSION_MODPAGEDISPLAY			L"http://xhmikosr.github.com/notepad2-mod/"

#if defined(_WIN64)
   #define VERSION_FILEVERSION_LONG  L"Notepad2-mod (64-bit) " STRINGIFY(VERSION_MAJOR) L"." \
                                     STRINGIFY(VERSION_MINOR) L"." STRINGIFY(VERSION_BUILD)  \
                                     L" r" STRINGIFY(VERSION_REV) L" (" VERSION_HASH L")"
#else
   #define VERSION_FILEVERSION_LONG  L"Notepad2-mod " STRINGIFY(VERSION_MAJOR) L"."         \
                                     STRINGIFY(VERSION_MINOR) L"." STRINGIFY(VERSION_BUILD) \
                                     L" r" STRINGIFY(VERSION_REV) L" (" VERSION_HASH L")"
#endif

#endif // NOTEPAD2_VERSION_H

// End of Version.h
