/******************************************************************************
*
*
* matepath - The universal Explorer-like Plugin
*
* DropSource.cpp
*   OLE drop source functionality
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
#include <shlobj.h>
#include "DropSource.h"

/******************************************************************************
*
* IUnknown Implementation
*
******************************************************************************/
STDMETHODIMP CDropSource::QueryInterface(REFIID iid, PVOID *ppv) noexcept {
	if (iid == IID_IUnknown || iid == IID_IDropSource) {
		*ppv = this;
		AddRef();
		return S_OK;
	}
	*ppv = nullptr;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDropSource::AddRef() noexcept {
	return 1; // do nothing
}

STDMETHODIMP_(ULONG) CDropSource::Release() noexcept {
	return 1; // do nothing
}

/******************************************************************************
*
* IDropSource Implementation
*
******************************************************************************/
STDMETHODIMP CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) noexcept {
	if (fEscapePressed) {
		return DRAGDROP_S_CANCEL;
	}
	if (!(grfKeyState & MK_LBUTTON) && !(grfKeyState & MK_RBUTTON)) {
		return DRAGDROP_S_DROP;
	}
	return S_OK;
}

STDMETHODIMP CDropSource::GiveFeedback(DWORD /*dwEffect*/) noexcept {
	return DRAGDROP_S_USEDEFAULTCURSORS;
}
