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
#include <memory>

#include <windows.h>
#include <ole2.h>

#include "Debugging.h"

#include "WinTypes.h"
#include "HanjaDic.h"

// see https://sourceforge.net/p/scintilla/bugs/2295/
// interface definitions based on generated header with following MSVC statement:
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

#if 0
class ScopedBSTR {
	BSTR bstr = nullptr;
public:
	ScopedBSTR(BSTR value = nullptr) noexcept : bstr{value} {}
	// Deleted so ScopedBSTR objects can not be copied. Moves are OK.
	ScopedBSTR(const ScopedBSTR &) = delete;
	ScopedBSTR &operator=(const ScopedBSTR &) = delete;
	// Moves are OK.
	ScopedBSTR(ScopedBSTR &&) = default;
	ScopedBSTR &operator=(ScopedBSTR &&) = default;
	~ScopedBSTR() {
		SysFreeString(bstr);
	}

	BSTR get() const noexcept {
		return bstr;
	}
	void reset(BSTR value = nullptr) noexcept {
		// https://en.cppreference.com/w/cpp/memory/unique_ptr/reset
		BSTR old = bstr;
		bstr = value;
		SysFreeString(old);
	}
};
#endif

class HanjaDic {
	std::unique_ptr<IHanjaDic, UnknownReleaser> HJinterface;

	bool OpenHanjaDic(LPCOLESTR lpszProgID) noexcept {
		CLSID CLSID_HanjaDic;
		HRESULT hr = CLSIDFromProgID(lpszProgID, &CLSID_HanjaDic);
		if (SUCCEEDED(hr)) {
			IHanjaDic *instance = nullptr;
			hr = CoCreateInstance(CLSID_HanjaDic, nullptr,
				//CLSCTX_INPROC_SERVER, __uuidof(IHanjaDic),
				CLSCTX_INPROC_SERVER, IID_IHanjaDic,
				reinterpret_cast<LPVOID *>(&instance));
			if (SUCCEEDED(hr)) {
				HJinterface.reset(instance);
				hr = instance->OpenMainDic();
				return SUCCEEDED(hr);
			}
		}
		return false;
	}

public:
	IHanjaDic *GetHanjaDic() const noexcept {
		return HJinterface.get();
	}

	bool Open() noexcept {
		return OpenHanjaDic(OLESTR("imkrhjd.hanjadic"))
			|| OpenHanjaDic(OLESTR("mshjdic.hanjadic"));
	}

	void Close() const noexcept {
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
#if 0
	bool HanjaToHangul(BSTR bstrHanja, ScopedBSTR &bstrHangul) const noexcept {
		BSTR result = nullptr;
		const HRESULT hr = HJinterface->HanjaToHangul(bstrHanja, &result);
		bstrHangul.reset(result);
		return SUCCEEDED(hr);
	}
#endif
};

struct HanjaDicCloser {
	IHanjaDic * const HJinterface;
	~HanjaDicCloser() {
		HJinterface->CloseMainDic();
	}
};

bool GetHangulOfHanja(std::wstring &inout) noexcept {
	// Convert every Hanja to Hangul.
	// Return whether any character been converted.
	// Hanja linked to different notes in Hangul have different codes,
	// so current character based conversion is enough.
	// great thanks for BLUEnLIVE.
	bool changed = false;
	HanjaDic dict;
	if (dict.Open()) {
		for (wchar_t &character : inout) {
			if (dict.IsHanja(character)) { // Pass hanja only!
#if 0
				const ScopedBSTR bstrHanja{SysAllocStringLen(&character, 1)};
				ScopedBSTR bstrHangul;
				if (dict.HanjaToHangul(bstrHanja.get(), bstrHangul)) {
					changed = true;
					character = bstrHangul.get()[0];
				}
#else
				BSTR bstrHanja = SysAllocStringLen(&character, 1);
				BSTR bstrHangul = nullptr;
				if (dict.HanjaToHangul(bstrHanja, &bstrHangul)) {
					changed = true;
					character = bstrHangul[0];
				}
				SysFreeString(bstrHangul);
				SysFreeString(bstrHanja);
#endif
			}
		}
		dict.Close();
	}
	return changed;
}

}

#if 0
// cl /utf-8 /W4 /EHsc /std:c++20 /GS- /GR- /Gv /Ox /FAcs /DNDEBUG /DUNICODE /I../src HanjaDic.cxx
// g++ -Wall -Wextra -std=gnu++20 -O2 -fno-rtti -DNDEBUG -DUNICODE -I../src HanjaDic.cxx -lole32 -loleaut32
#ifdef _MSC_VER
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#endif

using namespace Scintilla::Internal;
using namespace Scintilla::Internal::HanjaDict;

bool GetHangulOfHanja2(std::wstring &inout) {
	bool changed = false;
	HanjaDic dict;
	if (dict.Open()) {
		const HanjaDicCloser dictCloser { dict.GetHanjaDic() };
		std::wstring result;
		for (auto it = inout.cbegin(); it != inout.cend(); ) {
			bool converted = false;
			if (dict.IsHanja(*it)) {
				// scan consecutive hanja.
				auto next = it + 1;
				while (next != inout.cend() && dict.IsHanja(*next)) {
					++next;
				}
				const ScopedBSTR bstrHanja{SysAllocStringLen(&*it, static_cast<UINT>(next - it))};
				ScopedBSTR bstrHangul;
				converted = dict.HanjaToHangul(bstrHanja.get(), bstrHangul);
				if (converted) {
					changed = true;
					it = next;
					const UINT len = SysStringLen(bstrHangul.get());
					//printf("converted %u => %u\n", SysStringLen(bstrHanja.get()), len);
					for (UINT i = 0; i < len; i++) {
						result.push_back(bstrHangul.get()[i]);
					}
				}
			}
			if (!converted) {
				result.push_back(*it++);
			}
		}
		if (changed) {
			inout = result;
		}
	}
	return changed;
}

void ShowString(const char *name, std::wstring_view sv) noexcept {
	printf("%s[%zu]:", name, sv.length());
	for (const wchar_t character : sv) {
		if (character != ' ') {
			printf(" U+%04X", character);
		}
	}
	printf("\n");
}

int __cdecl main() {
	OleInitialize(nullptr);
	// https://www.koreanwikiproject.com/wiki/Hanja#Does_every_Hanja_character_have_only_one_sound_representation.3F
	constexpr std::wstring_view origin = L"年初 年末 昨年 來年 立式 獨立 組立 女子 女學生 少女 美女 不正 不得已 不便 不可能";
	ShowString(" origin", origin);

	std::wstring output1 { origin };
	GetHangulOfHanja(output1);
	ShowString("output1", output1);

	std::wstring output2 { origin };
	GetHangulOfHanja2(output2);
	ShowString("output2", output2);

	const bool same = output1 == output2;
	printf("%s %s %s\n", "output1", (same ? "==" : "!="), "output2");

	OleUninitialize();
	return 0;
}
#endif
