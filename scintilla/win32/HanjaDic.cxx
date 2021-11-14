// Scintilla source code edit control
/** @file HanjaDic.cxx
 ** Korean Hanja Dictionary
 ** Convert between Korean Hanja and Hangul by COM interface.
 **/
// Copyright 2015 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

//#include <cstdio>
#include <string>
#include <string_view>

#include <windows.h>
#include <ole2.h>

#include "Debugging.h"

#include "HanjaDic.h"

// see https://sourceforge.net/p/scintilla/bugs/2295/
//#import "file:C:\Windows\System32\IME\IMEKR\DICTS\imkrhjd.dll" raw_interfaces_only
//#include "imkrhjd.tlh"
//using namespace IMKRHJDLib;

namespace Scintilla::Internal::HanjaDict {

// copied from PlatWin.h
template <class T>
inline void ReleaseUnknown(T *&ppUnknown) noexcept {
	if (ppUnknown) {
#if 0
		ppUnknown->Release();
#else
		try {
			ppUnknown->Release();
		} catch (...) {
			// Never occurs
			NP2_unreachable();
		}
#endif
		ppUnknown = nullptr;
	}
}

#if 1
interface IRadical;
interface IHanja;
interface IStrokes;

enum HANJA_TYPE { HANJA_UNKNOWN = 0, HANJA_K0 = 1, HANJA_K1 = 2, HANJA_OTHER = 3 };

interface IHanjaDic : IUnknown {
	STDMETHOD(OpenMainDic)();
	STDMETHOD(CloseMainDic)();
	STDMETHOD(GetHanjaWords)(BSTR bstrHangul, SAFEARRAY* ppsaHanja, VARIANT_BOOL* pfFound);
	STDMETHOD(GetHanjaChars)(unsigned short wchHangul, BSTR* pbstrHanjaChars, VARIANT_BOOL* pfFound);
	STDMETHOD(HanjaToHangul)(BSTR bstrHanja, BSTR* pbstrHangul);
	STDMETHOD(GetHanjaType)(unsigned short wchHanja, HANJA_TYPE* pHanjaType);
	STDMETHOD(GetHanjaSense)(unsigned short wchHanja, BSTR* pbstrSense);
	STDMETHOD(GetRadicalID)(short SeqNumOfRadical, short* pRadicalID, unsigned short* pwchRadical);
	STDMETHOD(GetRadical)(short nRadicalID, IRadical** ppIRadical);
	STDMETHOD(RadicalIDToHanja)(short nRadicalID, unsigned short* pwchRadical);
	STDMETHOD(GetHanja)(unsigned short wchHanja, IHanja** ppIHanja);
	STDMETHOD(GetStrokes)(short nStrokes, IStrokes** ppIStrokes);
	STDMETHOD(OpenDefaultCustomDic)();
	STDMETHOD(OpenCustomDic)(BSTR bstrPath, long* plUdr);
	STDMETHOD(CloseDefaultCustomDic)();
	STDMETHOD(CloseCustomDic)(long lUdr);
	STDMETHOD(CloseAllCustomDics)();
	STDMETHOD(GetDefaultCustomHanjaWords)(BSTR bstrHangul, SAFEARRAY** ppsaHanja, VARIANT_BOOL* pfFound);
	STDMETHOD(GetCustomHanjaWords)(long lUdr, BSTR bstrHangul, SAFEARRAY** ppsaHanja, VARIANT_BOOL* pfFound);
	STDMETHOD(PutDefaultCustomHanjaWord)(BSTR bstrHangul, BSTR bstrHanja);
	STDMETHOD(PutCustomHanjaWord)(long lUdr, BSTR bstrHangul, BSTR bstrHanja);
	STDMETHOD(MaxNumOfRadicals)(short* pVal);
	STDMETHOD(MaxNumOfStrokes)(short* pVal);
	STDMETHOD(DefaultCustomDic)(long* pVal);
	STDMETHOD(DefaultCustomDic)(long pVal);
	STDMETHOD(MaxHanjaType)(HANJA_TYPE* pHanjaType);
	STDMETHOD(MaxHanjaType)(HANJA_TYPE pHanjaType);
};

extern "C" const GUID __declspec(selectany) IID_IHanjaDic =
{ 0xad75f3ac, 0x18cd, 0x48c6, { 0xa2, 0x7d, 0xf1, 0xe9, 0xa7, 0xdc, 0xe4, 0x32 } };
#endif

class HanjaDic {
	IHanjaDic *HJinterface = nullptr;

public:
	HanjaDic() noexcept {
		if (!OpenHanjaDic(OLESTR("imkrhjd.hanjadic"))) {
			OpenHanjaDic(OLESTR("mshjdic.hanjadic"));
		}
	}

	bool OpenHanjaDic(LPCOLESTR lpszProgID) noexcept {
		CLSID CLSID_HanjaDic;
		HRESULT hr = CLSIDFromProgID(lpszProgID, &CLSID_HanjaDic);
		if (SUCCEEDED(hr)) {
			hr = CoCreateInstance(CLSID_HanjaDic, nullptr,
				//CLSCTX_INPROC_SERVER, __uuidof(IHanjaDic),
				CLSCTX_INPROC_SERVER, IID_IHanjaDic,
				(LPVOID *)&HJinterface);
			if (SUCCEEDED(hr)) {
				hr = HJinterface->OpenMainDic();
				if (SUCCEEDED(hr)) {
					return true;
				}
			}
			ReleaseUnknown(HJinterface);
		}
		return false;
	}

	// Deleted so HanjaDic objects can not be copied.
	HanjaDic(const HanjaDic &) = delete;
	HanjaDic(HanjaDic &&) = delete;
	HanjaDic &operator=(const HanjaDic &) = delete;
	HanjaDic &operator=(HanjaDic &&) = delete;

	~HanjaDic() {
		if (HJinterface) {
			HJinterface->CloseMainDic();
			ReleaseUnknown(HJinterface);
		}
	}

	bool HJdictAvailable() const noexcept {
		return HJinterface != nullptr;
	}

	bool IsHanja(wchar_t hanja) const noexcept {
		HANJA_TYPE hanjaType = HANJA_UNKNOWN;
		const HRESULT hr = HJinterface->GetHanjaType(hanja, &hanjaType);
		return SUCCEEDED(hr) && hanjaType > HANJA_UNKNOWN;
	}

	bool HanjaToHangul(BSTR bstrHanja, BSTR* pbstrHangul) const noexcept {
		const HRESULT hr = HJinterface->HanjaToHangul(bstrHanja, pbstrHangul);
		return SUCCEEDED(hr);
	}
};

bool GetHangulOfHanja(std::wstring &inout) noexcept {
	// Convert every hanja to hangul.
	// Return whether any character been converted.
	bool changed = false;
	const HanjaDic dict;
	if (dict.HJdictAvailable()) {
		for (wchar_t &character : inout) {
			if (dict.IsHanja(character)) { // Pass hanja only!
				const wchar_t hanja[2] = { character, L'\0' };
				BSTR bstrHanja = SysAllocString(hanja);
				BSTR bstrHangul = nullptr;
				if (dict.HanjaToHangul(bstrHanja, &bstrHangul)) {
					changed = true;
					character = bstrHangul[0];
				}
				SysFreeString(bstrHangul);
				SysFreeString(bstrHanja);
			}
		}
	}
	return changed;
}

}

#if 0
// cl /W4 /EHsc /std:c++17 /Ox /DNDEBUG /DUNICODE /I../src HanjaDic.cxx
using namespace Scintilla::Internal::HanjaDict;
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

int main() {
	OleInitialize(nullptr);
	// Hanja U+5357, Hangul U+B0A8
	std::wstring inout = L"\u5357";
	const bool changed = GetHangulOfHanja(inout);
	printf("changed=%d, %04X\n", changed, inout[0]);
	OleUninitialize();
	return 0;
}
#endif
