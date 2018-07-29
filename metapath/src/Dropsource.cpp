/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* Dropsource.cpp
*   OLE drop source functionality
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

#include <windows.h>
#include "Dropsource.h"

/******************************************************************************
*
* IUnknown Implementation
*
******************************************************************************/
STDMETHODIMP CDropSource::QueryInterface(REFIID iid, void FAR *FAR *ppv) {
	if (iid == IID_IUnknown || iid == IID_IDropSource) {
		*ppv = this;
		InterlockedIncrement(&m_refs);
		return NOERROR;
	}
	*ppv = nullptr;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDropSource::AddRef() {
	return InterlockedIncrement(&m_refs);
}

STDMETHODIMP_(ULONG) CDropSource::Release() {
	ULONG refs = InterlockedDecrement(&m_refs);
	if (refs == 0) {
		delete this;
	}
	return refs;
}

/******************************************************************************
*
* CDropSource Constructor
*
******************************************************************************/
CDropSource::CDropSource() {
	m_refs = 1;
}

/******************************************************************************
*
* IDropSource Implementation
*
******************************************************************************/
STDMETHODIMP CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) {
	if (fEscapePressed) {
		return DRAGDROP_S_CANCEL;
	}
	if (!(grfKeyState & MK_LBUTTON) && !(grfKeyState & MK_RBUTTON)) {
		return DRAGDROP_S_DROP;
	}
	return NOERROR;
}

STDMETHODIMP CDropSource::GiveFeedback(DWORD dwEffect) {
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

extern "C" {

LPDROPSOURCE CreateDropSource(void) {
	return ((LPDROPSOURCE) new CDropSource);
}

}

// End of Dropsource.cpp
