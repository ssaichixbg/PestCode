////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////

#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#define _USE_MATH_DEFINES  1

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

//ATL
#include <atlimage.h>

//WTL
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#define _ATL_USE_CSTRING_FLOAT
#include <atlmisc.h>
#include <atlscrl.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atldlgs.h>

////////////////////////////////////////////////////////////////////////////////

#include "MainWindow.h"

////////////////////////////////////////////////////////////////////////////////

CAppModule _Module;

////////////////////////////////////////////////////////////////////////////////

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = 0;

	//--------------------------------------------------------------------------
	{
		MainWindow wndMain;
		MSG msg;
		// Create & show our main window
		if( NULL == wndMain.Create(NULL, CWindow::rcDefault, _T("Pest Test")) ) {  
			// Bad news, window creation failed
			AtlMessageBox(NULL, _T("error create main window"), _T("error"), MB_OK);
			nRet = 1;
			goto LABEL_EXIT;
		}
		wndMain.ShowWindow(nCmdShow);
		wndMain.UpdateWindow();
		// Run the message loop
		while( GetMessage(&msg, NULL, 0, 0) > 0 ) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		nRet = (int)msg.wParam;
	}
	//--------------------------------------------------------------------------

LABEL_EXIT:
	//--------------------------------------------------------------------------

	_Module.Term();
	::CoUninitialize();

	return nRet;
}

////////////////////////////////////////////////////////////////////////////////
