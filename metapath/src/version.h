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

#define MY_APPNAME                   L"metapath"
#define VERSION_FILEVERSION_NUM      4,0,13,0
#define VERSION_FILEVERSION          L"metapath 4.0.13"
#define VERSION_LEGALCOPYRIGHT_SHORT L"Copyright \xA9 1996-2018"
#define VERSION_LEGALCOPYRIGHT_LONG  L"\xA9 1996-2018 Florian Balmer"
#define VERSION_AUTHORNAME           L"Florian Balmer"
#define VERSION_COMPANYNAME          L"Florian Balmer et al."
#define VERSION_WEBPAGE_DISPLAY      L"flo's freeware - http://www.flos-freeware.ch"
#define VERSION_EMAIL_DISPLAY        L"florian.balmer@gmail.com"
#define VERSION_NEWPAGE_DISPLAY      L"https://github.com/zufuliu/notepad2"

#if defined(_WIN64)
#define VERSION_FILEVERSION_LONG     L"metapath (64-bit) 4.0.13"
#else
#define VERSION_FILEVERSION_LONG     L"metapath 4.0.13"
#endif

#endif // METAPATH_VERSION_H_
