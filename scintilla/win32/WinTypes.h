// Scintilla source code edit control
/** @file WinTypes.h
 ** Implement safe release of COM objects and access to functions in DLLs.
 ** Header contains all implementation - there is no .cxx file.
 **/
// Copyright 2020-2021 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

// Release an IUnknown* and set to nullptr.
// While IUnknown::Release must be noexcept, it isn't marked as such so produces
// warnings which are avoided by the catch.
template <class T>
inline void ReleaseUnknown(T *&ppUnknown) noexcept {
	if (ppUnknown) {
		try {
			ppUnknown->Release();
		} catch (...) {
			// Never occurs
			NP2_unreachable();
		}
		ppUnknown = nullptr;
	}
}

struct UnknownReleaser {
	// Called by unique_ptr to destroy/free the resource
	template <class T>
	void operator()(T *pUnknown) const noexcept {
		try {
			pUnknown->Release();
		} catch (...) {
			// IUnknown::Release must not throw, ignore if it does.
			NP2_unreachable();
		}
	}
};

/// Find a function in a DLL and convert to a function pointer.
/// This avoids undefined and conditionally defined behaviour.
template<typename T>
inline T DLLFunction(HMODULE hModule, LPCSTR lpProcName) noexcept {
	FARPROC function = ::GetProcAddress(hModule, lpProcName);
#if defined(__clang__) || defined(__GNUC__) || (_MSC_VER >= 1926)
	return __builtin_bit_cast(T, function);
#else
	static_assert(sizeof(T) == sizeof(function));
	T fp {};
	memcpy(&fp, &function, sizeof(T));
	return fp;
#endif
}

template<typename T>
inline T DLLFunctionEx(LPCWSTR lpDllName, LPCSTR lpProcName) noexcept {
	return DLLFunction<T>(::GetModuleHandleW(lpDllName), lpProcName);
}

// similar to IID_PPV_ARGS() but without __uuidof() check
template <class T>
inline void** AsPPVArgs(T** pp) noexcept {
	static_assert(__is_base_of(IUnknown, T));
	return reinterpret_cast<void **>(pp);
}

}
