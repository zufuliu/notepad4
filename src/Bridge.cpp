/******************************************************************************
*
*
* Notepad2
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

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include "SciCall.h"
#if !NP2_FORCE_COMPILE_C_AS_CPP
extern "C" {
#endif

#include "Helpers.h"
#include "Dialogs.h"
#include "Notepad2.h"

#if !NP2_FORCE_COMPILE_C_AS_CPP
}
#endif
#include "resource.h"

// Global settings...
#if NP2_FORCE_COMPILE_C_AS_CPP
extern PrintHeaderOption iPrintHeader;
extern PrintFooterOption iPrintFooter;
extern int iPrintColor;
extern int iPrintZoom;
extern RECT pageSetupMargin;
extern HWND hwndStatus;
extern WCHAR defaultTextFontName[LF_FACESIZE];
#else
extern "C" PrintHeaderOption iPrintHeader;
extern "C" PrintFooterOption iPrintFooter;
extern "C" int iPrintColor;
extern "C" int iPrintZoom;
extern "C" RECT pageSetupMargin;
extern "C" HWND hwndStatus;
extern "C" WCHAR defaultTextFontName[LF_FACESIZE];
#endif

// Stored objects...
static HGLOBAL hDevMode {};
static HGLOBAL hDevNames {};

static void EditPrintInit() noexcept;

// https://docs.microsoft.com/en-us/windows/win32/intl/locale-imeasure
// This value is 0 if the metric system (Systéme International d'Units,
// or S.I.) is used, and 1 if the United States system is used.
#define MeasurementInternational	0
#define MeasurementUnitedStates		1
static inline UINT GetLocaleMeasurement() noexcept {
	UINT measurement = MeasurementInternational;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IMEASURE | LOCALE_RETURN_NUMBER,
		(LPWSTR)(&measurement), sizeof(UINT) / sizeof(WCHAR));
#else
	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IMEASURE | LOCALE_RETURN_NUMBER,
		(LPWSTR)(&measurement), sizeof(UINT) / sizeof(WCHAR));
#endif
	return measurement;
}

//=============================================================================
//
// EditPrint() - Code from SciTEWin::Print()
//
extern "C" bool EditPrint(HWND hwnd, LPCWSTR pszDocTitle) {
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
#if 0
	if (0) {
		// Don't display dialog box, just use the default printer and options
		pdlg.Flags |= PD_RETURNDEFAULT;
	}
#endif
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
	DPtoLP(hdc, (LPPOINT)&rectMargins, 2);
	DPtoLP(hdc, (LPPOINT)&rectPhysMargins, 2);

	// Convert page size to logical units and we're done!
	DPtoLP(hdc, (LPPOINT) &ptPage, 1);

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
	SelectObject(hdc, fontHeader);
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
	SelectObject(hdc, fontFooter);
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
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, dateString, 256);

	// Get current time...
	if (iPrintHeader == PrintHeaderOption_FilenameAndDateTime) {
		WCHAR timeString[128];
		GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, timeString, 128);
		lstrcat(dateString, L" ");
		lstrcat(dateString, timeString);
	}

	// Set print color mode
	SciCall_SetPrintColorMode(iPrintColor);

	// Set print zoom...
	SciCall_SetPrintMagnification(iPrintZoom);

	Sci_Position lengthDoc = SciCall_GetLength();
	const Sci_Position lengthDocMax = lengthDoc;
	Sci_Position lengthPrinted = 0;

	// Requested to print selection
	if (pdlg.Flags & PD_SELECTION) {
		if (startPos > endPos) {
			lengthPrinted = endPos;
			lengthDoc = startPos;
		} else {
			lengthPrinted = startPos;
			lengthDoc = endPos;
		}

		lengthPrinted = sci::max<Sci_Position>(lengthPrinted, 0);
		lengthDoc = sci::min(lengthDoc, lengthDocMax);
	}

	// We must substract the physical margins from the printable area
	struct Sci_RangeToFormatFull frPrint;
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
	int pageNum = 1;
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
			SelectObject(hdc, fontHeader);
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
				SelectObject(hdc, fontFooter);
				GetTextExtentPoint32(hdc, dateString, len, &sizeInfo);
				rcw.left = frPrint.rc.right - 10 - sizeInfo.cx;
				ExtTextOut(hdc, rcw.left + 5, rcw.bottom,
						   ETO_OPAQUE, &rcw, dateString,
						   len, nullptr);
			}

			SetTextAlign(hdc, ta);
			if (iPrintHeader != PrintHeaderOption_LeaveBlank) {
				HPEN pen = CreatePen(0, 1, RGB(0, 0, 0));
				HPEN penOld = (HPEN)SelectObject(hdc, pen);
				MoveToEx(hdc, frPrint.rc.left, frPrint.rc.top - headerLineHeight / 4, nullptr);
				LineTo(hdc, frPrint.rc.right, frPrint.rc.top - headerLineHeight / 4);
				SelectObject(hdc, penOld);
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
				SelectObject(hdc, fontFooter);
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
				HPEN penOld = (HPEN)SelectObject(hdc, pen);
				SetBkColor(hdc, RGB(0, 0, 0));
				MoveToEx(hdc, frPrint.rc.left, frPrint.rc.bottom + footerLineHeight / 4, nullptr);
				LineTo(hdc, frPrint.rc.right, frPrint.rc.bottom + footerLineHeight / 4);
				SelectObject(hdc, penOld);
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

		ComboBox_SetCurSel(hwndCtl, (int)iPrintHeader);
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

		ComboBox_SetCurSel(hwndCtl, (int)iPrintFooter);
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

			iPrintHeader = (PrintHeaderOption)SendDlgItemMessage(hwnd, IDC_PAGESETUP_HEADER_LIST, CB_GETCURSEL, 0, 0);
			iPrintFooter = (PrintFooterOption)SendDlgItemMessage(hwnd, IDC_PAGESETUP_FOOTER_LIST, CB_GETCURSEL, 0, 0);
			iPrintColor	 = (int)SendDlgItemMessage(hwnd, IDC_PAGESETUP_COLOR_MODE_LIST, CB_GETCURSEL, 0, 0);
		}
		break;

	default:
		break;
	}

	return 0;
}

extern "C" void EditPrintSetup(HWND hwnd) {
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

//=============================================================================
//
// EditPrintInit() - Setup default page margin if no values from registry
//
static void EditPrintInit() noexcept {
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
