// https://docs.microsoft.com/en-us/windows/desktop/Intl/security-considerations--international-features
// https://docs.microsoft.com/en-us/windows/desktop/Intl/handling-sorting-in-your-applications

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcmpa
int lstrcmpA(LPCSTR lpString1, LPCSTR lpString2);
int StrCmpA(LPCSTR lpString1, LPCSTR lpString2);
	int strcmp(const char *s1, const char *s2);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcmpia
int lstrcmpiA(LPCSTR lpString1, LPCSTR lpString2);
int StrCmpIA(LPCSTR lpString1, LPCSTR lpString2);
	int strcasecmp(const char *s1, const char *s2);
	int _stricmp(const char *s1, const char *s2, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcmpna
int StrCmpNA(PCSTR psz1, PCSTR psz2, int nChar);
	int strncmp(const char *s1, const char *s2, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcmpnia
int StrCmpNIA(PCSTR psz1, PCSTR psz2, int nChar);
	int strncasecmp(const char *s1, const char *s2, size_t n);
	int _strnicmp(const char *s1, const char *s2, size_t n);


// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-wsprintfa
int wsprintfA(LPSTR buf, LPCSTR fmt, ...);
	int sprintf(char *str, const char *format, ...);

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-wvsprintfa
int wvsprintfA(LPSTR buf, LPCSTR fmt, va_list arglist)
	int vsprintf(char *str, const char *format, va_list ap);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-wnsprintfa
int wnsprintfA( PSTR  pszDest, int   cchDest, PCSTR pszFmt, ...);
	int snprintf(char *str, size_t size, const char *format, ...);


// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrlena
int lstrlenA(LPCSTR lpString);
	size_t strlen(const char *s);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcpya
LPSTR lstrcpyA(LPSTR  lpString1, LPCSTR lpString2);
LPSTR StrCpyA(LPSTR  lpString1, LPCSTR lpString2);
	char* strcpy(char *dest, const char *src);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcpyna
LPSTR lstrcpynA(LPSTR lpString1, LPCSTR lpString2, int iMaxLength);
	char* strncpy(char *dest, const char *src, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcata
LPSTR lstrcatA(LPSTR lpString1, LPCSTR lpString2);
	char* strcat(char *dest, const char *src);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcatbuffa
PSTR StrCatBuffA(PSTR  pszDest, PCSTR pszSrc, int cchDestBuffSize);
	char* strncat(char *dest, const char *src, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strdupa
PSTR StrDupA(PCSTR pszSrch);
	char* strdup(const char *s);


// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strchra
PCSTR StrChrA(PCSTR pszStart, WORD wMatch);
	char* strchr(const char *s, int c);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strchria
PCSTR StrChrIA(PCSTR pszStart, WORD wMatch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strpbrka
PCSTR StrPBrkA(PCSTR psz, PCSTR pszSet);
	char* strpbrk(const char *s, const char *accept);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strstra
PCSTR StrStrA(PCSTR pszFirst, PCSTR pszSrch);
	char* strstr(const char *haystack, const char *needle);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strstria
PCSTR StrStrIA(PCSTR pszFirst, PCSTR pszSrch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strtrima
BOOL StrTrimA(PSTR psz, PCSTR pszTrimChars);
