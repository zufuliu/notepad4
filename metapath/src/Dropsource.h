/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* Dropsource.h
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

class FAR CDropSource : public IDropSource {
public:
	CDropSource() noexcept;
	virtual ~CDropSource() = default;

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(REFIID riid, PVOID *ppvObj) noexcept override;
	STDMETHOD_(ULONG, AddRef)() noexcept override;
	STDMETHOD_(ULONG, Release)() noexcept override;

	/* IDropSource methods */
	STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState) noexcept override;
	STDMETHOD(GiveFeedback)(DWORD dwEffect) noexcept override;

private:
	ULONG m_refs;
};

#endif // METAPATH_DROPSOURCE_H_

// End of Dropsource.h
