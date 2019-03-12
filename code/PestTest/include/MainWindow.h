////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////

#include "ImageCtrl.h"

#include "alg/PestCount.h"

////////////////////////////////////////////////////////////////////////////////

class MainWindow : public CWindowImpl<MainWindow, CWindow, CFrameWinTraits>
{
public:
	DECLARE_WND_CLASS(_T("Pest Window Class"))

	//--------------------------------------------------------------------------
	enum { IDC_BTN_LOAD = 10, IDC_BTN_COUNT, IDC_TEXT_COUNT, IDC_PIC };

	CButton    m_btnLoad;
	CButton    m_btnCount;
	CStatic    m_txtCount;
	ImageCtrl  m_imageCtrl;
	//--------------------------------------------------------------------------


public:
//message handler
	BEGIN_MSG_MAP(MainWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_HANDLER(IDC_BTN_LOAD, BN_CLICKED, OnBtnLoadClicked)
		COMMAND_HANDLER(IDC_BTN_COUNT, BN_CLICKED, OnBtnCountClicked)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_btnLoad.Create(m_hWnd, rcDefault, _T("Load"),
						WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0,
						IDC_BTN_LOAD);
		m_btnCount.Create(m_hWnd, rcDefault, _T("Count"),
						WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0,
						IDC_BTN_COUNT);
		m_txtCount.Create(m_hWnd, rcDefault, _T("0"),
						WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0,
						IDC_TEXT_COUNT);
		m_imageCtrl.Create(m_hWnd, rcDefault, NULL,
						WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0,
						IDC_PIC);
		m_imageCtrl.UpdateScroll();
		return 0;
	}
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 1;
	}
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0);
		return 0;
	}
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CDCHandle dc((HDC)wParam);
		RECT rcClient;
		GetClientRect(&rcClient);
		CBrush bsh;
		bsh.CreateSolidBrush(RGB(64, 224, 205));
		dc.FillRect(&rcClient, bsh);
		return 1;
	}
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( wParam != SIZE_MINIMIZED ) {
			int w = LOWORD(lParam);
			int h = HIWORD(lParam);
			int x = 10;
			int y = 10;
			m_btnLoad.SetWindowPos(NULL, x, y, 60, 40, SWP_NOACTIVATE | SWP_NOZORDER);
			x += (60 + 10);
			m_btnCount.SetWindowPos(NULL, x, y, 60, 40, SWP_NOACTIVATE | SWP_NOZORDER);
			x += (60 + 10);
			m_txtCount.SetWindowPos(NULL, x, y, 60, 40, SWP_NOACTIVATE | SWP_NOZORDER);
			x = 10;
			y += (40 + 10);
			m_imageCtrl.SetWindowPos(NULL, x, y, w - 10 - 10, h - y - 10, SWP_NOACTIVATE | SWP_NOZORDER);
			m_imageCtrl.UpdateScroll();
		}
		bHandled = FALSE;
		return 1;
	}

	//--------------------------------------------------------------------------
	LRESULT OnBtnLoadClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFileDialog dlg(TRUE);
		if( dlg.DoModal() == IDOK ) {//弹出对话框
			CImage& image = m_imageCtrl.GetImage();
			image.Destroy();
			m_imageCtrl.UpdateScroll();
			if( FAILED(image.Load(dlg.m_szFileName)) ) {
				AtlMessageBox(NULL, _T("error load image"), _T("error"), MB_OK);
				return 0;
			}
			m_imageCtrl.UpdateScroll();
		}
		return 0;
	}
	LRESULT OnBtnCountClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//count
		ColorData colorData;
		GrayData grayData;
		CImage& image = m_imageCtrl.GetImage();

		int iCount = 0;

		CWaitCursor wac;

		//to data
		ImageDataHelper::ImageToColorData(image, colorData);
		//alogrithm
		iCount = PestCountHelper::CalcPestCount(colorData, grayData);
		//update UI
		ImageDataHelper::GrayDataToImage(grayData, image);
		m_imageCtrl.UpdateScroll();
		ATL::CString str;
		str.Format(_T("%d"), iCount);
		m_txtCount.SetWindowText(str);

		return 0;
	}
};

////////////////////////////////////////////////////////////////////////////////

