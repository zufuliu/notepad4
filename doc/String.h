// https://docs.microsoft.com/en-us/windows/desktop/Intl/security-considerations--international-features
// https://docs.microsoft.com/en-us/windows/desktop/Intl/handling-sorting-in-your-applications
// https://en.wikipedia.org/wiki/C_string_handling

// https://docs.microsoft.com/en-us/windows/desktop/api/stringapiset/nf-stringapiset-comparestringordinal
int CompareStringOrdinal(LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, BOOL bIgnoreCase);
// https://docs.microsoft.com/en-us/windows/desktop/api/stringapiset/nf-stringapiset-comparestringw
int CompareStringW(LCID Locale, DWORD dwCmpFlags, PCNZWCH lpString1, int cchCount1, PCNZWCH lpString2, int cchCount2);
// https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-findstringordinal
int FindStringOrdinal(DWORD dwFindStringOrdinalFlags, LPCWSTR lpStringSource, int cchSource, LPCWSTR lpStringValue, int cchValue, BOOL bIgnoreCase);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcmpa
int lstrcmpA(LPCSTR lpString1, LPCSTR lpString2);
#define StrCmpA		lstrcmpA
	int strcmp(const char *s1, const char *s2);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcmpw
int lstrcmpW(LPCWSTR lpString1, LPCWSTR lpString2);
// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcmpw
int StrCmpW(PCWSTR psz1, PCWSTR psz2);
	int wcscmp(const wchar_t *s1, const wchar_t *s2);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcmpia
int lstrcmpiA(LPCSTR lpString1, LPCSTR lpString2);
#define StrCmpIA	lstrcmpiA
	int strcasecmp(const char *s1, const char *s2);
	int _stricmp(const char *s1, const char *s2);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcmpiw
int lstrcmpiW(LPCWSTR lpString1, LPCWSTR lpString2);
// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcmpiw
int StrCmpIW(LPCWSTR lpString1, LPCWSTR lpString2);
	int wcscasecmp(const wchar_t *s1, const wchar_t *s2);
	int _wcsicmp(const wchar_t *s1, const wchar_t *s2);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcmpna
int StrCmpNA(PCSTR psz1, PCSTR psz2, int nChar);
#define StrNCmp		StrCmpNA
	int strncmp(const char *s1, const char *s2, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcmpnw
int StrCmpNW(PCWSTR psz1, PCWSTR psz2, int nChar);
#define StrNCmp		StrCmpNW
	int wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcmpnia
int StrCmpNIA(PCSTR psz1, PCSTR psz2, int nChar);
#define StrNCmpI	StrCmpNIA
	int strncasecmp(const char *s1, const char *s2, size_t n);
	int _strnicmp(const char *s1, const char *s2, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcmpniw
int StrCmpNIW(PCWSTR psz1, PCWSTR psz2, int nChar);
#define StrNCmpI	StrCmpNIW
	int wcsncasecmp(const wchar_t *s1, const wchar_t *s2, size_t n);
	int _wcsnicmp(const wchar_t *s1, const wchar_t *s2, size_t n);



// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-wsprintfa
int wsprintfA(LPSTR buf, LPCSTR fmt, ...);
	int sprintf(char *str, const char *format, ...);

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-wsprintfw
int wsprintfw(LPWSTR buf, LPCWSTR fmt, ...);

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-wvsprintfa
int wvsprintfA(LPSTR buf, LPCSTR fmt, va_list arglist)
	int vsprintf(char *str, const char *format, va_list ap);

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-wvsprintfw
int wvsprintfW(LPWSTR buf, LPCWSTR fmt, va_list arglist)

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-wnsprintfa
int wnsprintfA( PSTR pszDest, int cchDest, PCSTR pszFmt, ...);
	int snprintf(char *str, size_t size, const char *format, ...);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-wnsprintfw
int wnsprintfW(PWSTR pszDest, int cchDest, PCWSTR pszFmt, ...);
	int swprintf(wchar_t *wcs, size_t maxlen, const wchar_t *format, ...);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-wvnsprintfa
int wvnsprintfA( PSTR pszDest, int cchDest, PCSTR pszFmt, va_list arglist);
	int vsnprintf(char *str, size_t size, const char *format, va_list ap);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-wvnsprintfw
int wvnsprintfW(PWSTR pszDest, int cchDest, PCWSTR pszFmt, va_list arglist);
	int vswprintf(wchar_t *wcs, size_t maxlen, const wchar_t *format, va_list args);



// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrlena
int lstrlenA(LPCSTR lpString);
	size_t strlen(const char *s);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrlenw
int lstrlenW(LPCWSTR lpString);
	size_t wcslen(const wchar_t *s);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcpya
LPSTR lstrcpyA(LPSTR lpString1, LPCSTR lpString2);
#define StrCpyA		lstrcpyA
	char* strcpy(char *dest, const char *src);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcpyw
LPWSTR lstrcpyW(LPWSTR lpString1, LPCWSTR lpString2);
// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcpyw
PWSTR StrCpyW(PWSTR lpString1, PCWSTR lpString2);
	wchar_t* wcscpy(wchar_t *dest, const wchar_t *src);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcpyna
LPSTR lstrcpynA(LPSTR lpString1, LPCSTR lpString2, int iMaxLength);
#define StrCpyNA	lstrcpynA
#define StrNCpy		StrCpyNA
	char* strncpy(char *dest, const char *src, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcpynw
LPWSTR lstrcpynW(LPWSTR lpString1, LPCWSTR lpString2, int iMaxLength);
// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcpynw
PWSTR StrCpyNW(PWSTR pszDst, PCWSTR pszSrc, int cchMax);
#define StrNCpy		StrCpyNW
	wchar_t* wcsncpy(wchar_t *dest, const wchar_t *src, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcata
LPSTR lstrcatA(LPSTR lpString1, LPCSTR lpString2);
#define StrCatA		lstrcatA
	char* strcat(char *dest, const char *src);

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-lstrcatw
LPWSTR lstrcatW(LPWSTR lpString1, LPCWSTR lpString2);
// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcatw
PWSTR StrCatW(PWSTR psz1, PCWSTR psz2);
	wchar_t* wcscat(wchar_t *dest, const wchar_t *src);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcatbuffa
PSTR StrCatBuffA(PSTR pszDest, PCSTR pszSrc, int cchDestBuffSize);
// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strncata
PSTR StrNCatA(PSTR psz1, PCSTR psz2, int cchMax);
#define StrCatN		StrNCatA
	char* strncat(char *dest, const char *src, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcatbuffw
PWSTR StrCatBuffW(PWSTR pszDest, PCWSTR pszSrc, int cchDestBuffSize);
// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strncatw
PWSTR StrNCatW(PWSTR psz1, PCWSTR psz2, int cchMax);
#define StrCatN		StrNCatW
	wchar_t* wcsncat(wchar_t *dest, const wchar_t *src, size_t n);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strdupa
PSTR StrDupA(PCSTR pszSrch); // LocalFree()
	char* strdup(const char *s);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strdupw
PWSTR StrDupW(PCWSTR pszSrch); // LocalFree()
	wchar_t* wcsdup(const wchar_t *s);



// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strchra
PCSTR StrChrA(PCSTR pszStart, WORD wMatch);
	char* strchr(const char *s, int c);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strchrw
PCWSTR StrChrW(PCWSTR pszStart, WCHAR wMatch);
	wchar_t* wcschr(const wchar_t *wcs, wchar_t wc);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strpbrka
PCSTR StrPBrkA(PCSTR psz, PCSTR pszSet);
	char* strpbrk(const char *s, const char *accept);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strpbrkw
PCWSTR StrPBrkW(PCWSTR psz, PCWSTR pszSet);
	wchar_t* wcspbrk(const wchar_t *wcs, const wchar_t *accept);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strspna
int StrSpnA(PCSTR psz, PCSTR pszSet);
	size_t strspn(const char *s, const char *accept);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strspnw
int StrSpnW(PCWSTR psz, PCWSTR pszSet);
	size_t wcsspn(const wchar_t *wcs, const wchar_t *accept);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcspna
int StrCSpnA(PCSTR pszStr, PCSTR pszSet);
	size_t strcspn(const char *s, const char *reject);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strcspnw
int StrCSpnW(PCWSTR pszStr, PCWSTR pszSet);
	size_t wcscspn(const wchar_t *wcs, const wchar_t *reject);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strstra
PCSTR StrStrA(PCSTR pszFirst, PCSTR pszSrch);
	char* strstr(const char *haystack, const char *needle);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strstrw
PCWSTR StrStrW(PCWSTR pszFirst, PCWSTR pszSrch);
	wchar_t* wcsstr(const wchar_t *haystack, const wchar_t *needle);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strrchra
PCSTR StrRChrA(PCSTR pszStart, PCSTR pszEnd, WORD wMatch);
	char* strrchr(const char *s, int c);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strrchrw
PCWSTR StrRChrW(PCWSTR pszStart, PCWSTR pszEnd, WCHAR wMatch);
	wchar_t* wcsrchr(const wchar_t *wcs, wchar_t wc);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strchria
PCSTR StrChrIA(PCSTR pszStart, WORD wMatch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strchriw
PCWSTR StrChrIW(PCWSTR pszStart, WCHAR wMatch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strstria
PCSTR StrStrIA(PCSTR pszFirst, PCSTR pszSrch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strstriw
PCWSTR StrStrIW(PCWSTR pszFirst, PCWSTR pszSrch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strrchria
PCSTR StrRChrIA(PCSTR pszStart, PCSTR pszEnd, WORD wMatch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strrchriw
PCWSTR StrRChrIW(PCWSTR pszStart, PCWSTR pszEnd, WCHAR wMatch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strrstria
PCSTR StrRStrIA(PCSTR pszSource, PCSTR pszLast, PCSTR pszSrch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strrstriw
PCWSTR StrRStrIW(PCWSTR pszSource, PCWSTR pszLast, PCWSTR pszSrch);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strtrima
BOOL StrTrimA(PSTR psz, PCSTR pszTrimChars);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strtrimw
BOOL StrTrimW(PWSTR psz, PCWSTR pszTrimChars);

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-charpreva
LPSTR CharPrevA(LPCSTR lpszStart, LPCSTR lpszCurrent);

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-charprevw
LPWSTR CharPrevW(LPCWSTR lpszStart, LPCWSTR lpszCurrent);

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-charnexta
LPSTR CharNextA(LPCSTR lpsz);

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-charnextw
LPWSTR CharNextW(LPCWSTR lpsz);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strtointexa
BOOL StrToIntExA(PCSTR pszString, STIF_FLAGS dwFlags, int *piRet);

// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-strtointexw
BOOL StrToIntExW(PCWSTR pszString, STIF_FLAGS dwFlags, int *piRet);

int swscanf(const wchar_t *ws, const wchar_t *format, ...);
	double wcstod(const wchar_t *nptr, wchar_t **endptr);
	float wcstof(const wchar_t *nptr, wchar_t **endptr);
	long double wcstold(const wchar_t *nptr, wchar_t **endptr);
	long wcstol(const wchar_t *nptr, wchar_t **endptr, int base);
	long long wcstoll(const wchar_t *nptr, wchar_t **endptr, int base);
	unsigned long wcstoul(const wchar_t *nptr, wchar_t **endptr, int base);
	unsigned long long wcstoull(const wchar_t *nptr, wchar_t **endptr, int base);
