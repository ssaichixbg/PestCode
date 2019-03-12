////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////

class ImageCtrl : public ATL::CWindowImpl<ImageCtrl, ATL::CWindow, ATL::CControlWinTraits>,
				public CScrollImpl<ImageCtrl>
{
public:
	DECLARE_WND_CLASS(NULL)

//------------------------------------------------------------------------------
	CImage m_image;

	CImage& GetImage() throw()
	{
		return m_image;
	}

	void UpdateScroll() throw()
	{
		int cx = 1;
		int cy = 1;
		if( !m_image.IsNull() ) {
			cx = m_image.GetWidth();
			cy = m_image.GetHeight();
		}
		SetScrollSize(cx, cy, TRUE, false);//设置滚动视图的大小
	}

//------------------------------------------------------------------------------
//message handler
	BEGIN_MSG_MAP(ImageCtrl)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		CHAIN_MSG_MAP(CScrollImpl<ImageCtrl>)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CDCHandle dc((HDC)wParam);
		RECT rcClient;
		GetClientRect(&rcClient);
		CBrush bsh;
		bsh.CreateSolidBrush(RGB(64, 64, 64));
		dc.FillRect(&rcClient, bsh);
		return 1;
	}
//------------------------------------------------------------------------------
// Overrideables
	void DoPaint(CDCHandle dc)
	{
		if( !m_image.IsNull() ) {
			int nOldMode = dc.SetStretchBltMode(COLORONCOLOR);
			m_image.Draw(dc, _WTYPES_NS::CRect(0, 0, m_image.GetWidth(), m_image.GetHeight()));
			dc.SetStretchBltMode(nOldMode);
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
