/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* DropSource.h
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

#ifndef METAPATH_DROPSOURCE_H_
#define METAPATH_DROPSOURCE_H_

class CDropSource : public IDropSource {
public:
	CDropSource() noexcept;
	// Defaulted so CDropSource objects can be copied.
	CDropSource(const CDropSource &) = default;
	CDropSource(CDropSource &&) = default;
	CDropSource &operator=(const CDropSource &) = default;
	CDropSource &operator=(CDropSource &&) = default;
	virtual ~CDropSource() = default;

	/* IUnknown methods */
	STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv) noexcept override;
	STDMETHODIMP_(ULONG)AddRef() noexcept override;
	STDMETHODIMP_(ULONG)Release() noexcept override;

	/* IDropSource methods */
	STDMETHODIMP QueryContinueDrag(BOOL fEsc, DWORD grfKeyState) noexcept override;
	STDMETHODIMP GiveFeedback(DWORD) noexcept override;

private:
	ULONG m_refs;
};

#endif // METAPATH_DROPSOURCE_H_

// End of DropSource.h
