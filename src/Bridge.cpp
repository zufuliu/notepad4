/******************************************************************************
*
*
* Notepad4
*
* Bridge.cpp
*   Functionalities implemented in C++:
*   1. Printing
*   Mostly taken from SciTE, (c) Neil Hodgson, https://www.scintilla.org
*
* See Readme.txt for more information about this source code.
* Please send me your comments to this work.
*
* See License.txt for details about distribution and modification.
*
*                                              (c) Florian Balmer 1996-2011
*                                                  florian.balmer@gmail.com
*                                              https://www.flos-freeware.ch
*
*
******************************************************************************/

struct IUnknown;
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <string>
#include <string_view>
#include <memory>

#include "SciCall.h"
#include "VectorISA.h"
#include "Helpers.h"
#include "Dialogs.h"
#include "Notepad4.h"
#include "Edit.h"
#include "Styles.h"
#include "resource.h"

// Global settings...
extern HWND hwndMain;
extern PrintHeaderOption iPrintHeader;
extern PrintFooterOption iPrintFooter;
extern int iPrintColor;
extern int iPrintZoom;
extern RECT pageSetupMargin;
extern HWND hwndStatus;
extern WCHAR defaultTextFontName[LF_FACESIZE];

namespace {

// Stored objects...
HGLOBAL hDevMode {};
HGLOBAL hDevNames {};

// https://docs.microsoft.com/en-us/windows/win32/intl/locale-imeasure
// This value is 0 if the metric system (Systéme International d'Units,
// or S.I.) is used, and 1 if the United States system is used.
#define MeasurementInternational	0
#define MeasurementUnitedStates		1
inline UINT GetLocaleMeasurement() noexcept {
	UINT measurement = MeasurementInternational;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IMEASURE | LOCALE_RETURN_NUMBER,
		reinterpret_cast<LPWSTR>(&measurement), sizeof(UINT) / sizeof(WCHAR));
#else
	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IMEASURE | LOCALE_RETURN_NUMBER,
		reinterpret_cast<LPWSTR>(&measurement), sizeof(UINT) / sizeof(WCHAR));
#endif
	return measurement;
}

//=============================================================================
//
// EditPrintInit() - Setup default page margin if no values from registry
//
void EditPrintInit() noexcept {
	if ((pageSetupMargin.left | pageSetupMargin.top | pageSetupMargin.right | pageSetupMargin.bottom) < 0) {
		const UINT measurement = GetLocaleMeasurement();
		if (measurement == MeasurementInternational) {
			pageSetupMargin.left = 2000;
			pageSetupMargin.top = 2000;
			pageSetupMargin.right = 2000;
			pageSetupMargin.bottom = 2000;
		} else {
			pageSetupMargin.left = 1000;
			pageSetupMargin.top = 1000;
			pageSetupMargin.right = 1000;
			pageSetupMargin.bottom = 1000;
		}
	}
}

}

//=============================================================================
//
// EditPrint() - Code from SciTEWin::Print()
//
bool EditPrint(HWND hwnd, LPCWSTR pszDocTitle, BOOL bDefault) noexcept {
	PRINTDLG pdlg;
	memset(&pdlg, 0, sizeof(PRINTDLG));
	pdlg.lStructSize = sizeof(PRINTDLG);
	pdlg.hwndOwner = GetParent(hwnd);
	pdlg.hInstance = g_hInstance;
	pdlg.Flags = PD_USEDEVMODECOPIES | PD_ALLPAGES | PD_RETURNDC;
	pdlg.nFromPage = 1;
	pdlg.nToPage = 1;
	pdlg.nMinPage = 1;
	pdlg.nMaxPage = 0xffffU;
	pdlg.nCopies = 1;
	pdlg.hDC = nullptr;
	pdlg.hDevMode = hDevMode;
	pdlg.hDevNames = hDevNames;

	const Sci_Position startPos = SciCall_GetSelectionStart();
	const Sci_Position endPos = SciCall_GetSelectionEnd();

	if (startPos == endPos) {
		pdlg.Flags |= PD_NOSELECTION;
	} else {
		pdlg.Flags |= PD_SELECTION;
	}
	if (bDefault) {
		// Don't display dialog box, just use the default printer and options
		pdlg.Flags |= PD_RETURNDEFAULT;
	}
	if (!PrintDlg(&pdlg)) {
		return true; // False means error...
	}

	hDevMode = pdlg.hDevMode;
	hDevNames = pdlg.hDevNames;

	HDC hdc = pdlg.hDC;
	POINT ptDpi;
	POINT ptPage;

	// Get printer resolution
	ptDpi.x = GetDeviceCaps(hdc, LOGPIXELSX);		 // dpi in X direction
	ptDpi.y = GetDeviceCaps(hdc, LOGPIXELSY);		 // dpi in Y direction

	// Start by getting the physical page size (in device units).
	ptPage.x = GetDeviceCaps(hdc, PHYSICALWIDTH);		// device units
	ptPage.y = GetDeviceCaps(hdc, PHYSICALHEIGHT);	// device units

	RECT rectPhysMargins;
	// Get the dimensions of the unprintable
	// part of the page (in device units).
	rectPhysMargins.left = GetDeviceCaps(hdc, PHYSICALOFFSETX);
	rectPhysMargins.top = GetDeviceCaps(hdc, PHYSICALOFFSETY);

	// To get the right and lower unprintable area,
	// we take the entire width and height of the paper and
	// subtract everything else.
	rectPhysMargins.right = ptPage.x		// total paper width
							- GetDeviceCaps(hdc, HORZRES)	// printable width
							- rectPhysMargins.left;			// left unprintable margin

	rectPhysMargins.bottom = ptPage.y		// total paper height
							 - GetDeviceCaps(hdc, VERTRES)	// printable height
							 - rectPhysMargins.top;			// right unprintable margin

	// At this point, rectPhysMargins contains the widths of the
	// unprintable regions on all four sides of the page in device units.
	RECT rectMargins;
	RECT rectSetup;

	EditPrintInit();
	// Take in account the page setup given by the user (if one value is not null)
	if (pageSetupMargin.left != 0 || pageSetupMargin.right != 0 ||
			pageSetupMargin.top != 0 || pageSetupMargin.bottom != 0) {

		// Convert the hundredths of millimeters (HiMetric) or
		// thousandths of inches (HiEnglish) margin values
		// from the Page Setup dialog to device units.
		// (There are 2540 hundredths of a mm in an inch.)

		const UINT measurement = GetLocaleMeasurement();
		if (measurement == MeasurementInternational) {
			rectSetup.left		= MulDiv(pageSetupMargin.left, ptDpi.x, 2540);
			rectSetup.top		= MulDiv(pageSetupMargin.top, ptDpi.y, 2540);
			rectSetup.right		= MulDiv(pageSetupMargin.right, ptDpi.x, 2540);
			rectSetup.bottom	= MulDiv(pageSetupMargin.bottom, ptDpi.y, 2540);
		} else {
			rectSetup.left		= MulDiv(pageSetupMargin.left, ptDpi.x, 1000);
			rectSetup.top		= MulDiv(pageSetupMargin.top, ptDpi.y, 1000);
			rectSetup.right		= MulDiv(pageSetupMargin.right, ptDpi.x, 1000);
			rectSetup.bottom	= MulDiv(pageSetupMargin.bottom, ptDpi.y, 1000);
		}

		// Dont reduce margins below the minimum printable area
		rectMargins.left	= sci::max(rectPhysMargins.left, rectSetup.left);
		rectMargins.top		= sci::max(rectPhysMargins.top, rectSetup.top);
		rectMargins.right	= sci::max(rectPhysMargins.right, rectSetup.right);
		rectMargins.bottom	= sci::max(rectPhysMargins.bottom, rectSetup.bottom);
	} else {
		rectMargins.left	= rectPhysMargins.left;
		rectMargins.top		= rectPhysMargins.top;
		rectMargins.right	= rectPhysMargins.right;
		rectMargins.bottom	= rectPhysMargins.bottom;
	}

	// rectMargins now contains the values used to shrink the printable
	// area of the page.

	// Convert device coordinates into logical coordinates
	DPtoLP(hdc, reinterpret_cast<LPPOINT>(&rectMargins), 2);
	DPtoLP(hdc, reinterpret_cast<LPPOINT>(&rectPhysMargins), 2);

	// Convert page size to logical units and we're done!
	DPtoLP(hdc, reinterpret_cast<LPPOINT>(&ptPage), 1);

	const int fontSize = SciCall_StyleGetSizeFractional(STYLE_DEFAULT);

	TEXTMETRIC tm;
	int headerLineHeight = MulDiv(fontSize - SC_FONT_SIZE_MULTIPLIER/2, ptDpi.y, 72*SC_FONT_SIZE_MULTIPLIER);
	HFONT fontHeader = CreateFont(headerLineHeight,
							0, 0, 0,
							FW_NORMAL,
							FALSE, FALSE, FALSE,
							DEFAULT_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY,
							DEFAULT_PITCH,
							defaultTextFontName);
	SelectFont(hdc, fontHeader);
	GetTextMetrics(hdc, &tm);
	headerLineHeight = tm.tmHeight + tm.tmExternalLeading;

	if (iPrintHeader == PrintHeaderOption_LeaveBlank) {
		headerLineHeight = 0;
	}

	int footerLineHeight = MulDiv(fontSize - 2*SC_FONT_SIZE_MULTIPLIER, ptDpi.y, 72*SC_FONT_SIZE_MULTIPLIER);
	HFONT fontFooter = CreateFont(footerLineHeight,
							0, 0, 0,
							FW_NORMAL,
							FALSE, FALSE, FALSE,
							DEFAULT_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY,
							DEFAULT_PITCH,
							defaultTextFontName);
	SelectFont(hdc, fontFooter);
	GetTextMetrics(hdc, &tm);
	footerLineHeight = tm.tmHeight + tm.tmExternalLeading;

	if (iPrintFooter == PrintFooterOption_LeaveBlank) {
		footerLineHeight = 0;
	}

	const DOCINFO di = {sizeof(DOCINFO), pszDocTitle, nullptr, nullptr, 0};
	if (StartDoc(hdc, &di) < 0) {
		DeleteDC(hdc);
		if (fontHeader) {
			DeleteObject(fontHeader);
		}
		if (fontFooter) {
			DeleteObject(fontFooter);
		}
		return false;
	}

	// Get current date...
	SYSTEMTIME st;
	WCHAR dateString[256];
	GetLocalTime(&st);
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, dateString, COUNTOF(dateString), nullptr);
#else
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, dateString, COUNTOF(dateString));
#endif

	// Get current time...
	if (iPrintHeader == PrintHeaderOption_FilenameAndDateTime) {
		WCHAR timeString[128];
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, timeString, COUNTOF(timeString));
#else
		GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, timeString, COUNTOF(timeString));
#endif
		lstrcat(dateString, L" ");
		lstrcat(dateString, timeString);
	}

	// Set print color mode
	SciCall_SetPrintColorMode(iPrintColor);

	// Set print zoom...
	SciCall_SetPrintMagnification(iPrintZoom);

	Sci_Position lengthDoc = endPos;
	Sci_Position lengthPrinted = startPos;

	// Requested to print selection
	if (pdlg.Flags & PD_NOSELECTION) {
		lengthPrinted = 0;
		lengthDoc = SciCall_GetLength();
	}

	// We must substract the physical margins from the printable area
	Sci_RangeToFormatFull frPrint;
	frPrint.hdc = hdc;
	frPrint.hdcTarget = hdc;
	frPrint.rc.left		= rectMargins.left - rectPhysMargins.left;
	frPrint.rc.top		= rectMargins.top - rectPhysMargins.top;
	frPrint.rc.right	= ptPage.x - rectMargins.right - rectPhysMargins.left;
	frPrint.rc.bottom	= ptPage.y - rectMargins.bottom - rectPhysMargins.top;
	frPrint.rcPage.left		= 0;
	frPrint.rcPage.top		= 0;
	frPrint.rcPage.right	= ptPage.x - rectPhysMargins.left - rectPhysMargins.right - 1;
	frPrint.rcPage.bottom	= ptPage.y - rectPhysMargins.top - rectPhysMargins.bottom - 1;
	frPrint.rc.top		+= headerLineHeight + headerLineHeight / 2;
	frPrint.rc.bottom	-= footerLineHeight + footerLineHeight / 2;

	// Print each page
	UINT pageNum = 1;
	WCHAR tchPageFormat[128];
	WCHAR tchPageStatus[128];
	GetString(IDS_PRINT_PAGENUM, tchPageFormat, COUNTOF(tchPageFormat));
	GetString(IDS_PRINTFILE, tchPageStatus, COUNTOF(tchPageStatus));

	// Show wait cursor...
	BeginWaitCursor();

	bool printEmpty = lengthPrinted == lengthDoc;
	while (lengthPrinted < lengthDoc || printEmpty) {
		printEmpty = false;
		const bool printPage = !(pdlg.Flags & PD_PAGENUMS) || (pageNum >= pdlg.nFromPage && pageNum <= pdlg.nToPage);
		WCHAR tchNum[32];
		FormatNumber(tchNum, pageNum);
		WCHAR pageString[128];
		wsprintf(pageString, tchPageFormat, tchNum);

		if (printPage) {
			// Display current page number in Statusbar
			WCHAR statusString[128];
			wsprintf(statusString, tchPageStatus, tchNum);

			StatusSetText(hwndStatus, STATUS_HELP, statusString);
			StatusSetSimple(hwndStatus, TRUE);

			InvalidateRect(hwndStatus, nullptr, TRUE);
			UpdateWindow(hwndStatus);

			StartPage(hdc);

			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkColor(hdc, RGB(255, 255, 255));
			SelectFont(hdc, fontHeader);
			const UINT ta = SetTextAlign(hdc, TA_BOTTOM);
			RECT rcw = {
				frPrint.rc.left, frPrint.rc.top - headerLineHeight - headerLineHeight / 2,
				frPrint.rc.right, frPrint.rc.top - headerLineHeight / 2
			};

			if (iPrintHeader != PrintHeaderOption_LeaveBlank) {
				ExtTextOut(hdc, rcw.left + 5, rcw.bottom,
						   ETO_OPAQUE, &rcw, pszDocTitle,
						   lstrlen(pszDocTitle), nullptr);
			}

			// Print date in header
			if (iPrintHeader == PrintHeaderOption_FilenameAndDateTime || iPrintHeader == PrintHeaderOption_FilenameAndDate) {
				SIZE sizeInfo;
				const int len = lstrlen(dateString);
				SelectFont(hdc, fontFooter);
				GetTextExtentPoint32(hdc, dateString, len, &sizeInfo);
				rcw.left = frPrint.rc.right - 10 - sizeInfo.cx;
				ExtTextOut(hdc, rcw.left + 5, rcw.bottom,
						   ETO_OPAQUE, &rcw, dateString,
						   len, nullptr);
			}

			SetTextAlign(hdc, ta);
			if (iPrintHeader != PrintHeaderOption_LeaveBlank) {
				HPEN pen = CreatePen(0, 1, RGB(0, 0, 0));
				HPEN penOld = SelectPen(hdc, pen);
				MoveToEx(hdc, frPrint.rc.left, frPrint.rc.top - headerLineHeight / 4, nullptr);
				LineTo(hdc, frPrint.rc.right, frPrint.rc.top - headerLineHeight / 4);
				SelectPen(hdc, penOld);
				DeleteObject(pen);
			}
		}

		frPrint.chrg.cpMin = lengthPrinted;
		frPrint.chrg.cpMax = lengthDoc;

		lengthPrinted = SciCall_FormatRangeFull(printPage, &frPrint);

		if (printPage) {
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkColor(hdc, RGB(255, 255, 255));

			if (iPrintFooter == PrintFooterOption_PageNumber) {
				SelectFont(hdc, fontFooter);
				const UINT ta = SetTextAlign(hdc, TA_TOP);
				const RECT rcw = {
					frPrint.rc.left, frPrint.rc.bottom + footerLineHeight / 2,
					frPrint.rc.right, frPrint.rc.bottom + footerLineHeight + footerLineHeight / 2
				};

				SIZE sizeFooter;
				const int len = lstrlen(pageString);
				GetTextExtentPoint32(hdc, pageString, len, &sizeFooter);
				ExtTextOut(hdc, rcw.right - 5 - sizeFooter.cx, rcw.top,
						   ETO_OPAQUE, &rcw, pageString,
						   len, nullptr);

				SetTextAlign(hdc, ta);
				HPEN pen = ::CreatePen(0, 1, RGB(0, 0, 0));
				HPEN penOld = SelectPen(hdc, pen);
				SetBkColor(hdc, RGB(0, 0, 0));
				MoveToEx(hdc, frPrint.rc.left, frPrint.rc.bottom + footerLineHeight / 4, nullptr);
				LineTo(hdc, frPrint.rc.right, frPrint.rc.bottom + footerLineHeight / 4);
				SelectPen(hdc, penOld);
				DeleteObject(pen);
			}

			EndPage(hdc);
		}
		pageNum++;

		if ((pdlg.Flags & PD_PAGENUMS) && (pageNum > pdlg.nToPage)) {
			break;
		}
	}

	SciCall_FormatRangeFull(false, nullptr);

	EndDoc(hdc);
	DeleteDC(hdc);
	if (fontHeader) {
		DeleteObject(fontHeader);
	}
	if (fontFooter) {
		DeleteObject(fontFooter);
	}

	// Reset Statusbar to default mode
	StatusSetSimple(hwndStatus, FALSE);

	// Remove wait cursor...
	EndWaitCursor();

	return true;
}

//=============================================================================
//
// EditPrintSetup() - Code from SciTEWin::PrintSetup()
//
//
static UINT_PTR CALLBACK PageSetupHook(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM /*lParam*/) noexcept {
	switch (uiMsg) {
	case WM_INITDIALOG: {
		InitZoomLevelComboBox(hwnd, IDC_PAGESETUP_ZOOMLEVEL, iPrintZoom);

		// Set header options
		HWND hwndCtl = GetDlgItem(hwnd, IDC_PAGESETUP_HEADER_LIST);
		WCHAR tch[512];
		GetString(IDS_PRINT_HEADER, tch, COUNTOF(tch));
		lstrcat(tch, L"|");
		LPWSTR p1 = tch;
		LPWSTR p2;
		while ((p2 = StrChr(p1, L'|')) != nullptr) {
			*p2++ = L'\0';
			if (*p1) {
				ComboBox_AddString(hwndCtl, p1);
			}
			p1 = p2;
		}

		ComboBox_SetCurSel(hwndCtl, static_cast<int>(iPrintHeader));
		ComboBox_SetExtendedUI(hwndCtl, TRUE);

		// Set footer options
		hwndCtl = GetDlgItem(hwnd, IDC_PAGESETUP_FOOTER_LIST);
		GetString(IDS_PRINT_FOOTER, tch, COUNTOF(tch));
		lstrcat(tch, L"|");
		p1 = tch;
		while ((p2 = StrChr(p1, L'|')) != nullptr) {
			*p2++ = L'\0';
			if (*p1) {
				ComboBox_AddString(hwndCtl, p1);
			}
			p1 = p2;
		}

		ComboBox_SetCurSel(hwndCtl, static_cast<int>(iPrintFooter));
		ComboBox_SetExtendedUI(hwndCtl, TRUE);

		// Set color options
		hwndCtl = GetDlgItem(hwnd, IDC_PAGESETUP_COLOR_MODE_LIST);
		GetString(IDS_PRINT_COLOR, tch, COUNTOF(tch));
		lstrcat(tch, L"|");
		p1 = tch;
		while ((p2 = StrChr(p1, L'|')) != nullptr) {
			*p2++ = L'\0';
			if (*p1) {
				ComboBox_AddString(hwndCtl, p1);
			}
			p1 = p2;
		}

		ComboBox_SetCurSel(hwndCtl, iPrintColor);
		ComboBox_SetExtendedUI(hwndCtl, TRUE);

		// Make combox handier
		SendDlgItemMessage(hwnd, IDC_PAGESETUP_SOURCE_LIST, CB_SETEXTENDEDUI, TRUE, 0);
		SendDlgItemMessage(hwnd, IDC_PAGESETUP_ORIENTATION_LIST, CB_SETEXTENDEDUI, TRUE, 0);
	}
	break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			int zoomLevel;
			if (GetZoomLevelComboBoxValue(hwnd, IDC_PAGESETUP_ZOOMLEVEL, &zoomLevel)) {
				iPrintZoom = zoomLevel;
			} else {
				iPrintZoom = 100;
			}

			iPrintHeader = static_cast<PrintHeaderOption>(SendDlgItemMessage(hwnd, IDC_PAGESETUP_HEADER_LIST, CB_GETCURSEL, 0, 0));
			iPrintFooter = static_cast<PrintFooterOption>(SendDlgItemMessage(hwnd, IDC_PAGESETUP_FOOTER_LIST, CB_GETCURSEL, 0, 0));
			iPrintColor	 = static_cast<int>(SendDlgItemMessage(hwnd, IDC_PAGESETUP_COLOR_MODE_LIST, CB_GETCURSEL, 0, 0));
		}
		break;

	default:
		break;
	}

	return 0;
}

void EditPrintSetup(HWND hwnd) noexcept {
	DLGTEMPLATE *pDlgTemplate = LoadThemedDialogTemplate(MAKEINTRESOURCE(IDD_PAGESETUP), g_hInstance);

	PAGESETUPDLG pdlg;
	memset(&pdlg, 0, sizeof(PAGESETUPDLG));
	pdlg.lStructSize = sizeof(PAGESETUPDLG);
	pdlg.Flags = PSD_ENABLEPAGESETUPHOOK | PSD_ENABLEPAGESETUPTEMPLATEHANDLE;
	pdlg.lpfnPageSetupHook = PageSetupHook;
	pdlg.hPageSetupTemplate = pDlgTemplate;
	pdlg.hwndOwner = GetParent(hwnd);
	pdlg.hInstance = g_hInstance;

	EditPrintInit();
	if (pageSetupMargin.left != 0 || pageSetupMargin.right != 0 || pageSetupMargin.top != 0 || pageSetupMargin.bottom != 0) {
		pdlg.Flags |= PSD_MARGINS;
		pdlg.rtMargin.left		= pageSetupMargin.left;
		pdlg.rtMargin.top		= pageSetupMargin.top;
		pdlg.rtMargin.right		= pageSetupMargin.right;
		pdlg.rtMargin.bottom	= pageSetupMargin.bottom;
	}

	pdlg.hDevMode = hDevMode;
	pdlg.hDevNames = hDevNames;

	if (PageSetupDlg(&pdlg)) {
		pageSetupMargin.left	= pdlg.rtMargin.left;
		pageSetupMargin.top		= pdlg.rtMargin.top;
		pageSetupMargin.right	= pdlg.rtMargin.right;
		pageSetupMargin.bottom	= pdlg.rtMargin.bottom;

		hDevMode = pdlg.hDevMode;
		hDevNames = pdlg.hDevNames;
	}

	NP2HeapFree(pDlgTemplate);
}

namespace { // copy as RTF

#if (__cplusplus > 201703L || (defined(_MSVC_LANG) && _MSVC_LANG > 201703L)) && ( \
	(defined(_MSC_VER) && _MSC_VER >= 1928 && (defined(_WIN64) || !defined(__clang__))) || \
	(defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 16000) || \
	(!defined(_LIBCPP_VERSION) && defined(__GNUC__) && __GNUC__ >= 11) )
using std::make_unique_for_overwrite; // requires C++20 library support
#else
// https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique
template<class T>
std::enable_if_t<std::is_array_v<T>, std::unique_ptr<T>>
make_unique_for_overwrite(std::size_t n) {
	return std::unique_ptr<T>(new std::remove_extent_t<T>[n]);
}
#endif

struct DocumentStyledText {
	std::unique_ptr<StyleDefinition[]> styleList;
	unsigned styleCount;
	UINT cpEdit;
};

void GetStyleDefinitionFor(int style, StyleDefinition &definition) noexcept {
	definition.fontSize = SciCall_StyleGetSizeFractional(style);
	definition.foreColor = SciCall_StyleGetFore(style);
	definition.backColor = SciCall_StyleGetBack(style);
	definition.weight = SciCall_StyleGetWeight(style);
	definition.italic = SciCall_StyleGetItalic(style);
	definition.underline = SciCall_StyleGetUnderline(style);
	definition.strike = SciCall_StyleGetStrike(style);
	definition.eolFilled = SciCall_StyleGetEOLFilled(style);
	definition.charset = SciCall_StyleGetCharacterSet(style);
	SciCall_StyleGetFont(style, definition.fontFace);
}

DocumentStyledText GetDocumentStyledText(uint8_t (&styleMap)[STYLE_MAX + 1], const char *styledText, size_t textLength) noexcept {
	uint32_t styleUsed[8]{}; // bitmap for styles used in the range
	styleUsed[STYLE_DEFAULT >> 5] |= (1U << (STYLE_DEFAULT & 31));
	unsigned maxStyle = STYLE_DEFAULT;

	for (size_t offset = 0; offset < textLength; offset++) {
		const uint8_t style = styledText[offset];
		styleUsed[style >> 5] |= (1U << (style & 31));
		maxStyle = max<unsigned>(style, maxStyle);
	}

	++maxStyle;
	static_assert(__is_standard_layout(StyleDefinition));
	//static_assert(STYLE_DEFAULT == 0); //! STYLE_DEFAULT should be put at styleList[0]
	memset(styleMap, 0, STYLE_MAX + 1);

	unsigned styleCount = 0;
	std::unique_ptr<StyleDefinition[]> styleList = std::make_unique<StyleDefinition[]>(maxStyle);
	if constexpr (STYLE_DEFAULT != 0) {
		styleCount = 1;
		styleUsed[STYLE_DEFAULT >> 5] &= ~(1U << (STYLE_DEFAULT & 31));
		GetStyleDefinitionFor(STYLE_DEFAULT, styleList[0]);
	}

	for (unsigned style = 0; style < maxStyle; style++) {
		if (!BitTestEx(styleUsed, style)) {
			continue;
		}

		StyleDefinition &definition = styleList[styleCount];
		GetStyleDefinitionFor(style, definition);
		unsigned index = 0;
		for (; index < styleCount; index++) {
			// NOLINTNEXTLINE(bugprone-suspicious-memory-comparison)
			if (memcmp(&definition, &styleList[index], sizeof(StyleDefinition)) == 0) {
				memset(definition.fontFace, 0, sizeof(definition.fontFace));
				break;
			}
		}
		styleMap[style] = static_cast<uint8_t>(index);
		if (index == styleCount) {
			styleCount++;
		}
	}

	const UINT cpEdit = SciCall_GetCodePage();
	return { std::move(styleList), styleCount, cpEdit };
}

// code based SciTE's ExportRTF.cxx
// Rich Text Format (RTF) Specification Version 1.9.1
// https://www.loc.gov/preservation/digital/formats/fdd/fdd000473.shtml
// https://latex2rtf.sourceforge.net/RTF-Spec-1.2.pdf

// RTF version, character set and ANSI code page, default font, default tab width
#define RTF_HEADEROPEN "{\\rtf1\\ansi\\ansicpg%u\\deff0\\deftab420"
#define RTF_FONTDEFOPEN "{\\fonttbl"
#define RTF_FONTDEFCLOSE "}"
// omitted definition for first color, which is used to turn off background highlighting
#define RTF_COLORDEFOPEN "{\\colortbl;"
#define RTF_COLORDEFCLOSE "}"
#define RTF_HEADERCLOSE "\n"
#define RTF_BODYOPEN ""
#define RTF_BODYCLOSE "}"
// box paragraph with background color
#define RTF_PARAGRAPH_BEGIN "\\pard\\box\\cbpat%u"
#define RTF_PARAGRAPH_END "\\par\n"

#define RTF_SETFONTFACE "\\f"
#define RTF_SETFONTSIZE "\\fs"
#define RTF_SETCOLOR "\\cf"
#define RTF_SETBACKGROUND "\\highlight"
#define RTF_BOLD_ON "\\b"
#define RTF_BOLD_OFF "\\b0"
#define RTF_ITALIC_ON "\\i"
#define RTF_ITALIC_OFF "\\i0"
#define RTF_UNDERLINE_ON "\\ul"
#define RTF_UNDERLINE_OFF "\\ulnone"
#define RTF_STRIKE_ON "\\strike"
#define RTF_STRIKE_OFF "\\strike0"

#define RTF_EOL "\\line\n"
#define RTF_TAB "\\tab "

// font face, size, color, bold, italic, underline, strike
#define RTF_MAX_STYLEPROP 7
#define RTF_MAX_STYLEDEF 128
#ifndef CF_RTF
#define CF_RTF TEXT("Rich Text Format")
#endif

// extract the next RTF control word from *style
void GetRTFNextControl(const char **style, char *control) noexcept {
	const char *pos = *style;
	*control = '\0';
	if ('\0' == *pos) {
		return;
	}
	pos++; // implicit skip over leading '\'
	while ('\0' != *pos && '\\' != *pos) {
		pos++;
	}
	const size_t len = pos - *style;
	memcpy(control, *style, len);
	control[len] = '\0';
	*style = pos;
}

// extracts control words that are different between two styles
// \f0\fs20\cf0\b0\i0
void GetRTFStyleChange(std::string &delta, const char *last, const char *current) {
	char lastControl[RTF_MAX_STYLEDEF];
	char currentControl[RTF_MAX_STYLEDEF];
	lastControl[0] = '\0';
	currentControl[0] = '\0';
	const char *lastPos = last;
	const char *currentPos = current;
	const size_t len = delta.length();
	for (int i = 0; i < RTF_MAX_STYLEPROP; i++) {
		GetRTFNextControl(&lastPos, lastControl);
		GetRTFNextControl(&currentPos, currentControl);
		if (strcmp(lastControl, currentControl) != 0) {	// changed
			delta += currentControl;
		}
	}
	if (len != delta.length()) {
		delta += ' ';
	}
}

constexpr int GetRTFFontSize(int size) noexcept {
	return size / (SC_FONT_SIZE_MULTIPLIER / 2);
}

void SaveToStreamRTF(std::string &os, const char *styledText, size_t textLength, Sci_Position startPos, Sci_Position endPos) {
	uint8_t styleMap[STYLE_MAX + 1];
	const auto [styleList, styleCount, cpEdit] = GetDocumentStyledText(styleMap, styledText, textLength);
	const std::unique_ptr<std::string[]> styles = make_unique_for_overwrite<std::string[]>(styleCount);
	const std::unique_ptr<LPCSTR[]> fontList = make_unique_for_overwrite<LPCSTR[]>(styleCount);
	const std::unique_ptr<COLORREF[]> colorList = make_unique_for_overwrite<COLORREF[]>(2*styleCount);

	UINT legacyACP = cpEdit;
	if (legacyACP == SC_CP_UTF8 || legacyACP == 0) {
		legacyACP = mEncoding[CPI_DEFAULT].uCodePage;
	}

	char fmtbuf[RTF_MAX_STYLEDEF];
	memset(fmtbuf, 0, 4);
	unsigned fmtlen = sprintf(fmtbuf, RTF_HEADEROPEN RTF_FONTDEFOPEN, legacyACP);
	os += std::string_view{fmtbuf, fmtlen};

	int fontCount = 0;
	int colorCount = 0;
	for (unsigned styleIndex = 0; styleIndex < styleCount; styleIndex++) {
		StyleDefinition &definition = styleList[styleIndex];

		int iFont = 0;
		for (; iFont < fontCount; iFont++) {
			if (_stricmp(fontList[iFont], definition.fontFace) == 0) {
				break;
			}
		}
		if (iFont == fontCount) {
			fontList[fontCount++] = definition.fontFace;
			// convert fontFace to ANSI code page
			char fontFace[LF_FACESIZE]{};
			MultiByteToWideChar(CP_UTF8, 0, definition.fontFace, -1, definition.fontWide, COUNTOF(definition.fontWide));
			WideCharToMultiByte(legacyACP, 0, definition.fontWide, -1, fontFace, COUNTOF(fontFace), nullptr, nullptr);
			const int charset = definition.charset;
			if (charset == DEFAULT_CHARSET || charset == ANSI_CHARSET) {
				fmtlen = sprintf(fmtbuf, "{\\f%d\\fnil %s;}", iFont, fontFace);
			} else {
				fmtlen = sprintf(fmtbuf, "{\\f%d\\fnil\\fcharset%d %s;}", iFont, charset, fontFace);
			}
			os += std::string_view{fmtbuf, fmtlen};
		}

		int iFore = 0;
		for (; iFore < colorCount; iFore++) {
			if (colorList[iFore] == definition.foreColor) {
				break;
			}
		}
		if (iFore == colorCount) {
			colorList[colorCount++] = definition.foreColor;
		}

		// PL: highlights seems doesn't follow a distinct table, at least with WordPad and Word 97
		// Perhaps it is different for Word 6?
		int iBack = 1;
		for (; iBack < colorCount; iBack++) {
			if (colorList[iBack] == definition.backColor) {
				break;
			}
		}
		if (iBack == colorCount) {
			colorList[colorCount++] = definition.backColor;
		}

		definition.backIndex = static_cast<uint16_t>(iBack + 1);
		fmtlen = sprintf(fmtbuf, RTF_SETFONTFACE "%d" RTF_SETFONTSIZE "%d" RTF_SETCOLOR "%d",
			iFont, GetRTFFontSize(definition.fontSize), iFore + 1);

		std::string osStyle(fmtbuf, fmtlen);
		osStyle += ((definition.weight >= FW_SEMIBOLD) ? RTF_BOLD_ON : RTF_BOLD_OFF);
		osStyle += (definition.italic ? RTF_ITALIC_ON : RTF_ITALIC_OFF);
		osStyle += (definition.underline ? RTF_UNDERLINE_ON : RTF_UNDERLINE_OFF);
		osStyle += (definition.strike ? RTF_STRIKE_ON : RTF_STRIKE_OFF);
		styles[styleIndex] = std::move(osStyle);
	}

	os += RTF_FONTDEFCLOSE RTF_COLORDEFOPEN;
	for (int i = 0; i < colorCount; i++) {
		const COLORREF color = colorList[i];
		fmtlen = sprintf(fmtbuf, "\\red%d\\green%d\\blue%d;",
			static_cast<int>(color & 0xff), static_cast<int>((color >> 8) & 0xff), static_cast<int>((color >> 16) & 0xff));
		os += std::string_view{fmtbuf, fmtlen};
	}
	os += RTF_COLORDEFCLOSE RTF_HEADERCLOSE RTF_BODYOPEN;

	const char *lastStyle = "";
	unsigned styleCurrent = STYLE_MAX + 1;
	unsigned column = 0;
	// check eolFilled on first line
	constexpr uint8_t defaultBackground = 2; // omitted default, STYLE_DEFAULT foreColor, STYLE_DEFAULT backColor
	bool eolFilled = false;
	unsigned background = defaultBackground;
	unsigned highlight = 0;
	{
		const Sci_Line line = SciCall_LineFromPosition(startPos);
		const Sci_Position pos = SciCall_PositionFromLine(line + 1);
		if (pos < endPos) {
			uint8_t eolStyle = styledText[2*(pos - startPos) - 1];
			eolStyle = styleMap[eolStyle];
			eolFilled = styleList[eolStyle].eolFilled;
			if (eolFilled) {
				background = styleList[eolStyle].backIndex;
			}
		}
		fmtlen = sprintf(fmtbuf, RTF_PARAGRAPH_BEGIN, background);
		os += std::string_view{fmtbuf, fmtlen};
	}

	const char * const textBuffer = styledText + textLength + 1;
	for (size_t offset = 0; offset < textLength; offset++) {
		uint8_t style = styledText[offset];
		style = styleMap[style];
		if (style != styleCurrent) {
			styleCurrent = style;
			const char * const currentStyle = styles[style].c_str();
			GetRTFStyleChange(os, lastStyle, currentStyle);
			lastStyle = currentStyle;
			// detect background color change
			unsigned backIndex = styleList[style].backIndex;
			backIndex = (backIndex == background)? 0 : backIndex;
			if (backIndex != highlight) {
				highlight = backIndex;
				fmtlen = sprintf(fmtbuf, RTF_SETBACKGROUND "%u ", backIndex);
				os += std::string_view{fmtbuf, fmtlen};
			}
		}

		const char ch = textBuffer[offset];
		std::string_view sv;
		column++;
		if (ch == '{') {
			sv = "\\{";
		} else if (ch == '}') {
			sv = "\\}";
		} else if (ch == '\\') {
			sv = "\\\\";
		} else if (ch == '\t') {
			if (!fvCurFile.bTabsAsSpaces) {
				sv = RTF_TAB;
			} else {
				const unsigned tabWidth = fvCurFile.iTabWidth;
				const unsigned padding = tabWidth - ((column - 1) % tabWidth);
				column += padding;
				for (unsigned itab = 0; itab < padding; itab++) {
					os += ' ';
				}
			}
		} else if (ch == '\r' || ch == '\n') {
			sv = RTF_EOL;
			column = 0;
			if (ch == '\r' && textBuffer[offset + 1] == '\n') {
				offset += 1;
			}
			// check eolFilled on next line
			const Sci_Line line = SciCall_LineFromPosition(startPos + offset);
			const Sci_Position pos = SciCall_PositionFromLine(line + 2);
			if (pos < endPos) {
				uint8_t eolStyle = styledText[2*(pos - startPos) - 1];
				eolStyle = styleMap[eolStyle];
				bool changed = styleList[eolStyle].eolFilled;
				if (changed) {
					eolFilled = true;
					const unsigned backIndex = styleList[eolStyle].backIndex;
					changed = backIndex != background;
					background = backIndex;
				} else if (eolFilled) {
					changed = true;
					eolFilled = false;
					background = defaultBackground;
				}
				if (changed) {
					lastStyle = "";
					styleCurrent = STYLE_MAX + 1;
					highlight = 0;
					fmtlen = sprintf(fmtbuf, RTF_PARAGRAPH_END RTF_PARAGRAPH_BEGIN, background);
					sv = {fmtbuf, fmtlen};
				}
			}
		} else if (static_cast<signed char>(ch) < 0 && cpEdit == SC_CP_UTF8) {
			const Sci_Position pos = startPos + offset;
			Sci_Position width = 0;
			const unsigned int u32 = SciCall_GetCharacterAndWidth(pos, &width);
			offset += width - 1;
			if (u32 < 0x10000) {
				fmtlen = sprintf(fmtbuf, "\\u%d?", static_cast<short>(u32));
			} else {
				fmtlen = sprintf(fmtbuf, "\\u%d?\\u%d?",
					static_cast<short>(((u32 - 0x10000) >> 10) + 0xD800),
					static_cast<short>((u32 & 0x3ff) + 0xDC00));
			}
			sv = {fmtbuf, fmtlen};
		}

		if (sv.empty()) {
			if (ch != '\t') {
				os += ch;
			}
		} else {
			os += sv;
		}
	}

	os += RTF_PARAGRAPH_END RTF_BODYCLOSE;
}

}

namespace { // code pretty

enum {
	SpaceOption_None = 0,
	SpaceOption_IndentAfter = 1,
	SpaceOption_SpaceBefore = 2,
	SpaceOption_SpaceAfter = 4,
	SpaceOption_NewLineBefore = 8,
	SpaceOption_NewLineAfter = 16,
	SpaceOption_DanglingStmt = 32,
	SpaceOption_PushBrace = 64,
	SpaceOption_PopBrace = 128,
};

int AddStyleSeparator(LPCEDITLEXER pLex, int ch, int chPrev, int style) noexcept {
	if ((style >= pLex->stringStyleFirst && style <= pLex->stringStyleLast)
		|| (pLex->iLexer == SCLEX_JSON && (style == SCE_JSON_PROPERTYNAME))
		|| (pLex->iLexer == SCLEX_JAVASCRIPT && (style == SCE_JS_KEY || style == SCE_JS_OPERATOR_PF))) {
		return SpaceOption_None;
	}
	// a++ + ++b, a + +1, a-- - --b, a - -1
	if (ch == '+' || ch == '-') {
		if (style == SCE_CSS_OPERATOR2 && pLex->iLexer == SCLEX_CSS) {
			// '+' and '-' inside math function requires space on both side
			return SpaceOption_SpaceBefore | SpaceOption_SpaceAfter;
		}
		if (ch == chPrev) {
			return SpaceOption_SpaceBefore;
		}
	}
	// var name; return .5; CSS property: 1 #1 .5 --name;
	if (BitTestEx(DefaultWordCharSet, chPrev)) {
		if (BitTestEx(DefaultWordCharSet, ch) || ch == '$' || ch == '#' || (ch == '-' && pLex->iLexer == SCLEX_CSS)) {
			// TODO: improve CSS An+B when B is negative, https://www.w3.org/TR/css-syntax-3/#anb-microsyntax
			return SpaceOption_SpaceBefore;
		}
		if (ch == '.' && style != pLex->operatorStyle && style != pLex->operatorStyle2) {
			return SpaceOption_SpaceBefore;
		}
	}
	return SpaceOption_None;
}

void CodePretty(std::string &output, LPCEDITLEXER pLex, const char *styledText, size_t textLength) {
	char fmtbuf[128];
	std::string braceStack(1, '\0'); // sentinel
	memset(fmtbuf, 0, 4);

	unsigned fmtlen = 0;
	uint32_t blockLevel = 0;
	uint32_t indentPrev = 0;
	uint8_t chPrev = 0;
	uint8_t chPrevNonWhite = 0;
	bool commentEndEOL = false;
	bool defaultCase = false;

	unsigned eol = '\r' | ('\n' << 8);
	unsigned eolWidth = SciCall_GetEOLMode();
	eol >>= 8*(eolWidth >> 1);
	eolWidth = (eolWidth == SC_EOL_CRLF) ? 2 : 1;

	constexpr unsigned maxFmtLen = sizeof(fmtbuf) - 4 - 4; // \r\n + 4 + \r\n
	constexpr uint8_t braceObject = '{' + 1;
	constexpr uint8_t braceTemplate = '{' + 2;
	constexpr uint8_t bracketArray = '[' + 1;

	uint8_t braceTop = '\0';
	uint8_t stylePrev = styledText[0];
	uint8_t styleBefore = stylePrev;
	const char * const textBuffer = styledText + textLength + 1;
	for (size_t offset = 0; offset < textLength; offset++) {
		if (fmtlen >= maxFmtLen) {
			// keep last character and eol in the buffer
			output += std::string_view{fmtbuf, fmtlen - 4};
			memcpy(fmtbuf, &fmtbuf[fmtlen - 4], 4);
			fmtlen = 4;
		}

		const uint8_t style = styledText[offset];
		if (style == 0) {
			styleBefore = 0;
			continue;
		}

		int spaceOption = SpaceOption_None;
		unsigned operatorLen = 0;
		const uint8_t ch = textBuffer[offset];
		if (style <= pLex->commentStyleMarker) {
			if (ch == '/' && (chPrev == '\n' || styleBefore == 0 || styleBefore > pLex->commentStyleMarker) && textBuffer[offset + 1] == '/') {
				commentEndEOL = true; // fix indentation after comment line
			} else if (styledText[offset + 1] == 0 && (textBuffer[offset + 1] == '\n' || textBuffer[offset + 1] == '\r')) {
				commentEndEOL = true; // keep new line after block comment
				spaceOption = SpaceOption_NewLineAfter;
			}
		} else if (style != styleBefore) {
			spaceOption = AddStyleSeparator(pLex, ch, chPrev, style);
			if (style == SCE_JS_WORD && pLex->iLexer == SCLEX_JAVASCRIPT && (ch == 'c' || ch == 'd')) {
				if ((textBuffer[offset + 1] == 'a' || textBuffer[offset + 1] == 'e')
					&& (textBuffer[offset + 2] == 's' || textBuffer[offset + 2] == 'f')) {
					defaultCase = true;
					spaceOption |= SpaceOption_NewLineBefore;
				}
			}
			if (chPrev == ')') {
				if (pLex->iLexer != SCLEX_JSON && (stylePrev == pLex->operatorStyle || stylePrev == pLex->operatorStyle2)) {
					if (pLex->iLexer == SCLEX_CSS) {
						if ((ch == '+' || ch == '-') || (ch != ':' && style != SCE_CSS_OPERATOR)) { // :not([class]):hover
							spaceOption |= SpaceOption_SpaceBefore; // CSS property: function() value
						}
					} else if (style != SCE_JS_OPERATOR && style != SCE_JS_OPERATOR2) {
						spaceOption |= SpaceOption_NewLineBefore | SpaceOption_DanglingStmt;
						indentPrev++; // if (), for (), while () statement
					}
				}
			} else if ((stylePrev == SCE_CSS_AT_RULE && pLex->iLexer == SCLEX_CSS)
				|| (stylePrev == SCE_JS_WORD && pLex->iLexer == SCLEX_JAVASCRIPT
					&& (style <= SCE_JS_OPERATOR_PF || ch == '{' || ch == '[' || ch == '!' || ch == '~' || (ch == '(' && chPrev != 't')))) {
				// not set(), get();
				spaceOption |= SpaceOption_SpaceBefore;
			}
		}
		if (style == pLex->operatorStyle || style == pLex->operatorStyle2) {
			if (ch == ':') {
				spaceOption |= SpaceOption_SpaceAfter; // property: value
				if (pLex->iLexer == SCLEX_JAVASCRIPT) {
					if (defaultCase && braceTop == '{') {
						defaultCase = false;
						spaceOption |= SpaceOption_NewLineAfter;
					} else if (stylePrev != SCE_JS_KEY && stylePrev != SCE_JS_LABEL && (stylePrev != SCE_JS_WORD || chPrev != 't')) {
						spaceOption |= SpaceOption_SpaceBefore; // ternary operator, not set: / get:
					}
				}
			} else if (ch == ',') {
				if (pLex->iLexer == SCLEX_JSON || braceTop == braceObject || braceTop == bracketArray) {
					spaceOption |= SpaceOption_NewLineAfter;
				} else {
					spaceOption |= SpaceOption_SpaceAfter;
				}
			} else if (ch == ';') {
				if (braceTop == '(') {
					spaceOption |= SpaceOption_SpaceAfter; // for (;;)
				} else {
					spaceOption |= SpaceOption_NewLineAfter;
				}
			} else if (ch == '{' || ch == '[') {
				if (ch == '{' && (chPrev == ')' || pLex->iLexer == SCLEX_CSS)) {
					spaceOption |= SpaceOption_SpaceBefore; // if (...){}, CSS: selector {rule}
				}
				if (pLex->iLexer == SCLEX_JAVASCRIPT) {
					spaceOption |= SpaceOption_PushBrace;
					if (chPrev == '$' && style == styleBefore) {
						braceTop = braceTemplate; // ${}
					} else if (chPrevNonWhite == '=' || chPrevNonWhite == '(' || chPrevNonWhite == ',' || chPrevNonWhite == '['
						|| (chPrevNonWhite == ':' && (braceTop == braceObject || braceTop == bracketArray))) {
						braceTop = ch + 1;
						spaceOption |= SpaceOption_NewLineAfter | SpaceOption_IndentAfter;
					} else {
						braceTop = ch;
						if (ch == '{') {
							spaceOption |= SpaceOption_NewLineAfter | SpaceOption_IndentAfter;
							if (stylePrev >= SCE_JS_IDENTIFIER) {
								spaceOption |= SpaceOption_SpaceBefore;
							}
						}
					}
				} else if (ch == '{' || pLex->iLexer == SCLEX_JSON) {
					spaceOption |= SpaceOption_NewLineAfter | SpaceOption_IndentAfter;
				}
			} else if (ch == '}' || ch == ']') {
				if ((ch == '}' && braceTop != braceTemplate) || (ch == ']' && (braceTop == bracketArray || pLex->iLexer == SCLEX_JSON))) {
					spaceOption |= SpaceOption_NewLineBefore | SpaceOption_NewLineAfter;
					if (blockLevel > 0) {
						--blockLevel;
					}
				}
				if (pLex->iLexer == SCLEX_JAVASCRIPT && static_cast<uint8_t>(ch - braceTop) < 3) {
					spaceOption |= SpaceOption_PopBrace;
				}
			} else if (pLex->iLexer != SCLEX_JSON) {
				if (pLex->iLexer == SCLEX_CSS) {
					const uint8_t chNext = textBuffer[offset + 1];
					if (style == SCE_CSS_OPERATOR2
						|| ch == '>' // child combinator
						|| (ch == '&' && chNext != ':' && chNext != '.' && chNext != ')') // nesting selector, &:hover, &.class, :not(&)
						|| (ch == '~' && chNext != '=') // subsequent-sibling combinator
						|| (ch == '|' && chNext == '|') // column combinator
						// next-sibling combinator
						|| (ch == '+' && styledText[offset + 1] != SCE_CSS_NUMBER && styledText[offset + 1] != SCE_CSS_DIMENSION)
						) {
						spaceOption |= SpaceOption_SpaceAfter;
						if (ch == '|') {
							operatorLen = 2;
						}
						if (chPrev != '(') { // :has(> img)
							spaceOption |= SpaceOption_SpaceBefore;
						}
					} else if (ch == '!') {
						spaceOption |= SpaceOption_SpaceBefore; // !important
					}
				} else {
					if (ch == '(') {
						braceTop = '(';
						spaceOption |= SpaceOption_PushBrace;
					} else if (ch == ')') {
						if (braceTop == '(') {
							spaceOption |= SpaceOption_PopBrace;
						}
					} else if (ch != '.' && ch != '~' && ch != '$') {
						// https://tc39.es/ecma262/#sec-punctuators
						// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators
						// !		!=		!==
						// %		%=
						// &	&&	&=		&&=
						// *	**	*=		**=
						// +	++	+=
						// -	--	-=
						// /		/=
						// <	<<	<=		<<=
						// =	=>	==		===
						// >	>>	>=		>>=	>>>		>>>=
						// ?	?.	??		??=
						// ^		^=
						// |	||	|=		||=
						uint8_t chNext = textBuffer[offset + 1];
						if (chNext == '=' || (ch != '!' && ch == chNext)) {
							operatorLen = 2;
							spaceOption |= SpaceOption_SpaceBefore | SpaceOption_SpaceAfter;
							chNext = textBuffer[offset + 2];
							if (chNext == '=' || chNext == '>') {
								operatorLen = 3;
								if (chNext == '>' && textBuffer[offset + 3] == '=') {
									operatorLen = 4;
								}
							}
						} else if (ch == '+' || ch == '-') {
							// detect unary / binary operator, similar to FollowExpression()
							if (stylePrev == SCE_JS_WORD) {
								spaceOption |= SpaceOption_SpaceBefore;
							} else if (chPrevNonWhite == ')' || chPrevNonWhite == ']'
								|| (stylePrev >= SCE_JS_NUMBER && stylePrev <= SCE_JS_OPERATOR_PF)
								|| (stylePrev >= SCE_JS_IDENTIFIER && stylePrev <= SCE_JS_CONSTANT)) {
								spaceOption |= SpaceOption_SpaceBefore | SpaceOption_SpaceAfter;
							}
						} else if (ch == '*' && stylePrev == SCE_JS_WORD) {
							// yield*, function*
							spaceOption |= SpaceOption_SpaceAfter;
						} else if (ch != '!' && (ch != '?' || chNext != '.')) {
							spaceOption |= SpaceOption_SpaceBefore | SpaceOption_SpaceAfter;
							if (ch == '=' && chNext == '>') {
								operatorLen = 2;
							}
						}
					}
				}
			}
			if (chPrev == '\n' && (ch == ','
				|| ch == ')' // JavaScript: function({})
				|| ch == ';' // JavaScript: {};
				|| ch == '=' // JavaScript: let {} = value, const {} = value
				|| ch == ':' // JavaScript: ternary operator
				|| (ch == '.' && pLex->iLexer != SCLEX_CSS) // JavaScript: {}.property, [].property; CSS: .class {rule}
				|| ((ch == ']' || ch == '}') && chPrevNonWhite == ch - 2))) { // empty [], {}
				chPrev = '\0';
				fmtlen -= eolWidth;
				if ((spaceOption & SpaceOption_NewLineBefore) == 0) {
					chPrev = fmtbuf[fmtlen - 1];
				}
			}
			if (spaceOption & SpaceOption_PushBrace) {
				braceStack.push_back(static_cast<char>(braceTop));
			} else if (spaceOption & SpaceOption_PopBrace) {
				braceStack.pop_back();
				braceTop = braceStack.back();
			}
		}
		if (chPrev > ' ') {
			if (spaceOption & SpaceOption_NewLineBefore) {
				chPrev = '\n';
				memcpy(&fmtbuf[fmtlen], &eol, 2);
				fmtlen += eolWidth;
			} else if (spaceOption & SpaceOption_SpaceBefore) {
				fmtbuf[fmtlen++] = ' ';
			}
		}
		if (chPrev == '\n') {
			uint32_t count = (spaceOption & SpaceOption_DanglingStmt) ? indentPrev : blockLevel;
			indentPrev = count;
			char indent = '\t';
			if (fvCurFile.bTabsAsSpaces) {
				indent = ' ';
				count *= fvCurFile.iTabWidth;
			}
			if (count != 0) {
				output += std::string_view{fmtbuf, fmtlen};
				fmtlen = 0;
				output.append(count, indent);
			}
		}
		if (ch == '\r' || ch == '\n') {
			spaceOption |= SpaceOption_NewLineAfter;
			if (ch == '\r' && textBuffer[offset + 1] == '\n') {
				offset += 1;
			}
		} else {
			if (ch > ' ' && style > pLex->commentStyleMarker) {
				chPrevNonWhite = ch;
			}
			if (operatorLen > 1) {
				memcpy(&fmtbuf[fmtlen], textBuffer + offset, operatorLen);
				fmtlen += operatorLen;
				offset += operatorLen - 1;
				chPrevNonWhite = chPrev = fmtbuf[fmtlen - 1];
			} else {
				chPrev = ch;
				fmtbuf[fmtlen++] = static_cast<char>(ch);
			}
		}
		if (spaceOption & SpaceOption_NewLineAfter) {
			blockLevel += spaceOption & SpaceOption_IndentAfter;
			chPrev = '\n';
			// don't add indentation inside comment and string
			if ((!commentEndEOL && style <= pLex->commentStyleMarker)
				|| (style >= pLex->stringStyleFirst && style <= pLex->stringStyleLast)) {
				chPrev = '\r';
			}
			commentEndEOL = false;
			defaultCase = false;
			memcpy(&fmtbuf[fmtlen], &eol, 2);
			fmtlen += eolWidth;
		} else if (spaceOption & SpaceOption_SpaceAfter) {
			chPrev = ' ';
			fmtbuf[fmtlen++] = ' ';
		}
		stylePrev = style;
		styleBefore = style;
	}
	if (fmtlen != 0) {
		output += std::string_view{fmtbuf, fmtlen};
	}
}

}

void EditFormatCode(int menu) noexcept {
	LPCEDITLEXER pLex = pLexCurrent;
	if (menu != IDM_EDIT_COPYRTF && pLex->iLexer != SCLEX_JSON && pLex->iLexer != SCLEX_CSS && pLex->iLexer != SCLEX_JAVASCRIPT) {
		return;
	}
	Sci_Position startPos = SciCall_GetSelectionStart();
	Sci_Position endPos = SciCall_GetSelectionEnd();
	bool wholeDoc = false;
	if (startPos == endPos) {
		wholeDoc = true;
		startPos = 0;
		endPos = SciCall_GetLength();
	}

	try {
		SciCall_EnsureStyledTo(endPos);
		const std::unique_ptr<char[]> styledText = make_unique_for_overwrite<char[]>(2*(endPos - startPos) + 2);
		const Sci_TextRangeFull tr { { startPos, endPos }, styledText.get() };
		const size_t textLength = SciCall_GetStyledTextFull(&tr);
		std::string output;
		std::string_view result;
		bool changed = false;

		if (menu == IDM_EDIT_COPYRTF) {
			// code from SciTEWin::CopyAsRTF()
			SaveToStreamRTF(output, styledText.get(), textLength, startPos, endPos);
			//printf("%s:\n%s\n", __func__, output.c_str());
			const size_t len = output.length() + 1; // +1 for NUL
			HGLOBAL handle = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, len);
			if (handle) {
				::OpenClipboard(hwndMain);
				::EmptyClipboard();
				char *ptr = static_cast<char *>(::GlobalLock(handle));
				if (ptr) {
					memcpy(ptr, output.c_str(), len);
					::GlobalUnlock(handle);
				}
				::SetClipboardData(::RegisterClipboardFormat(CF_RTF), handle);
				::CloseClipboard();
			}
		} else if (menu == IDM_EDIT_CODE_COMPRESS) {
			size_t index = 0;
			int chPrev = 0;
			int stylePrev = static_cast<uint8_t>(styledText[0]);
			const char * const textBuffer = styledText.get() + textLength + 1;
			for (size_t offset = 0; offset < textLength; offset++) {
				const uint8_t style = styledText[offset];
				if (style > pLex->commentStyleMarker) {
					const uint8_t ch = textBuffer[offset];
					int spaceOption = SpaceOption_None;
					if (style != stylePrev) {
						spaceOption = AddStyleSeparator(pLex, ch, chPrev, style);
						if (spaceOption & SpaceOption_SpaceBefore) {
							styledText[index++] = ' ';
						}
					}
					chPrev = ch;
					styledText[index++] = static_cast<char>(ch);
					if (spaceOption & SpaceOption_SpaceAfter) {
						chPrev = ' ';
						styledText[index++] = ' ';
					}
				}
				stylePrev = style;
			}
			styledText[index] = '\0';
			if (index < textLength) {
				changed = true;
				result = {styledText.get(), index};
			}
		} else {
			CodePretty(output, pLex, styledText.get(), textLength);
			if (output.length() != textLength) {
				changed = true;
				result = output;
			}
		}
		if (changed) {
			if (wholeDoc) {
				SciCall_TargetWholeDocument();
				SciCall_ReplaceTarget(result.length(), result.data());
			} else {
				EditReplaceMainSelection(result.length(), result.data());
			}
		}
	} catch (...) {
	}
}
