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

#include "HanjaDic.h"

// see https://sourceforge.net/p/scintilla/bugs/2295/
//#import "file:C:\Windows\System32\IME\IMEKR\DICTS\imkrhjd.dll" raw_interfaces_only
//#include "imkrhjd.tlh"
//using namespace IMKRHJDLib;

namespace Scintilla::Internal::HanjaDict {

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
		CLSID CLSID_HanjaDic;
		HRESULT hr = CLSIDFromProgID(OLESTR("mshjdic.hanjadic"), &CLSID_HanjaDic);
		if (!SUCCEEDED(hr)) {
			hr = CLSIDFromProgID(OLESTR("imkrhjd.hanjadic"), &CLSID_HanjaDic);
		}
		if (SUCCEEDED(hr)) {
			hr = CoCreateInstance(CLSID_HanjaDic, nullptr,
				//CLSCTX_INPROC_SERVER, __uuidof(IHanjaDic),
				CLSCTX_INPROC_SERVER, IID_IHanjaDic,
				(LPVOID *)&HJinterface);
			if (!SUCCEEDED(hr)) {
			}
		}
	}

	// Deleted so HanjaDic objects can not be copied.
	HanjaDic(const HanjaDic &) = delete;
	HanjaDic(HanjaDic &&) = delete;
	HanjaDic &operator=(const HanjaDic &) = delete;
	HanjaDic &operator=(HanjaDic &&) = delete;

	~HanjaDic() {
		if (HJinterface) {
			try {
				// This can never fail but IUnknown::Release is not marked noexcept.
				HJinterface->Release();
			} catch (...) {
				// Ignore any exception
			}
		}
	}

	bool HJdictAvailable() const noexcept {
		return HJinterface != nullptr;
	}

	bool OpenMainDic() const noexcept {
		const HRESULT hr = HJinterface->OpenMainDic();
		return SUCCEEDED(hr);
	}

	void CloseMainDic() const noexcept {
		HJinterface->CloseMainDic();
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

int GetHangulOfHanja(wchar_t *inout) noexcept {
	// Convert every hanja to hangul.
	// Return the number of characters converted.
	int changed = 0;
	const HanjaDic dict;
	if (dict.HJdictAvailable() && dict.OpenMainDic()) {
		const size_t len = lstrlenW(inout);
		BSTR bstrHangul = nullptr;
		for (size_t i = 0; i < len; i++) {
			if (dict.IsHanja(inout[i])) { // Pass hanja only!
				const wchar_t conv[2] = { inout[i], L'\0' };
				BSTR bstrHanja = SysAllocString(conv);
				if (dict.HanjaToHangul(bstrHanja, &bstrHangul)) {
					inout[i] = bstrHangul[0];
					changed += 1;
				}
				SysFreeString(bstrHanja);
			}
		}
		SysFreeString(bstrHangul);
		dict.CloseMainDic();
	}
	return changed;
}

}

#if 0
// cl /W4 /EHsc /std:c++17 /Ox /DNDEBUG /DUNICODE HanjaDic.cxx
using namespace Scintilla::Internal::HanjaDict;
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

int main() {
	OleInitialize(nullptr);
	// Hanja U+5357, Hangul U+B0A8
	wchar_t inout[2] = { L'\u5357', L'\0' };
	const int changed = GetHangulOfHanja(inout);
	printf("changed=%d, %04X\n", changed, inout[0]);
	OleUninitialize();
	return 0;
}
#endif
