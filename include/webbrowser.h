// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <windows.h>
#include <ole2.h>

extern "C" const IID IID_IWOLBrowserEvent;
extern "C" const IID IID_IWOLBrowser;
extern "C" const CLSID CLSID_WOLBrowser;

interface IWOLBrowserEvent : public IUnknown
{
	public:
		virtual HRESULT STDMETHODCALLTYPE OnScriptQuit(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnBeforeNavigate(const wchar_t *url, const wchar_t *targetFrame) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnDocumentComplete(const wchar_t *url, BOOL isTopFrame) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnDownloadBegin(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnProgressChange(LONG progress, LONG progressMax) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnDownloadComplete(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnNavigateComplete(const wchar_t *url) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnStatusTextChange(const wchar_t *statusText) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnTitleChange(const wchar_t *title) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnNewWindow(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnShowMessage(const wchar_t *text, const wchar_t *caption, ULONG type, LONG *result) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnFailedPageCertification(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnErrorMsg(const wchar_t *error) = 0;
		virtual HRESULT STDMETHODCALLTYPE OnRegisterLogin(const wchar_t *nick, const wchar_t *pass) = 0;
};

interface IWOLBrowser : public IUnknown
{
	public:
		virtual HRESULT STDMETHODCALLTYPE Startup(HWND parent, LPRECT rect) = 0;
		virtual HRESULT STDMETHODCALLTYPE Shutdown(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE AddObserver(IWOLBrowserEvent *observer) = 0;
		virtual HRESULT STDMETHODCALLTYPE RemoveObserver(IWOLBrowserEvent *observer) = 0;
		virtual HRESULT STDMETHODCALLTYPE Show(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE Hide(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE Reposition(LPRECT rect) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetUIFlags(ULONG uiFlags) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetDownloadControl(ULONG dlFlags) = 0;
		virtual HRESULT STDMETHODCALLTYPE ExecuteScript(const wchar_t *script, DISPPARAMS *params, VARIANT *result) = 0;
		virtual HRESULT STDMETHODCALLTYPE ExecuteScriptInFrame(const wchar_t *frame, const wchar_t *script, DISPPARAMS *params, VARIANT *result) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPageCertificate(const wchar_t *certificate) = 0;
		virtual HRESULT STDMETHODCALLTYPE AllowPageCertification(BOOL allow) = 0;
		virtual HRESULT STDMETHODCALLTYPE Navigate(const wchar_t *url, long flags, const wchar_t *targetFrame) = 0;
		virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE Refresh(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE GoBack(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE GoForward(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE GoHome(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE GoSearch(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetLocationName(wchar_t *locationName, LONG size) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetLocationURL(wchar_t *locationURL, LONG size) = 0;
};

template <class T>
class CComPtr
{
	public:
		CComPtr(void) { p = NULL; }
		~CComPtr(void) { if (p) p->Release(); }
		operator T *(void) const { return p; }
		T & operator *(void) const { return *p; }
		T ** operator &(void) { return &p; }
		T * operator ->(void) const { return p; }

		T *p;
};

class WebBrowser : public IWOLBrowserEvent
{
	public:
		WebBrowser(void);
		virtual ~WebBrowser(void);

		bool Init(HWND window, RECT *rect);
		void Show(void);
		void Hide(void);
		bool Show_Page(char *page);
		bool Get_Page_URL(const char *page, char *url, int size);
		bool Get_HTML_Path(char *path, int size);

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef(void);
		virtual ULONG STDMETHODCALLTYPE Release(void);

		virtual HRESULT STDMETHODCALLTYPE OnScriptQuit(void);
		virtual HRESULT STDMETHODCALLTYPE OnBeforeNavigate(const wchar_t *url, const wchar_t *targetFrame);
		virtual HRESULT STDMETHODCALLTYPE OnDocumentComplete(const wchar_t *url, BOOL isTopFrame);
		virtual HRESULT STDMETHODCALLTYPE OnDownloadBegin(void);
		virtual HRESULT STDMETHODCALLTYPE OnProgressChange(LONG progress, LONG progressMax);
		virtual HRESULT STDMETHODCALLTYPE OnDownloadComplete(void);
		virtual HRESULT STDMETHODCALLTYPE OnNavigateComplete(const wchar_t *url);
		virtual HRESULT STDMETHODCALLTYPE OnStatusTextChange(const wchar_t *statusText);
		virtual HRESULT STDMETHODCALLTYPE OnTitleChange(const wchar_t *title);
		virtual HRESULT STDMETHODCALLTYPE OnNewWindow(void);
		virtual HRESULT STDMETHODCALLTYPE OnShowMessage(const wchar_t *text, const wchar_t *caption, ULONG type, LONG *result);
		virtual HRESULT STDMETHODCALLTYPE OnFailedPageCertification(void);
		virtual HRESULT STDMETHODCALLTYPE OnErrorMsg(const wchar_t *error);
		virtual HRESULT STDMETHODCALLTYPE OnRegisterLogin(const wchar_t *nick, const wchar_t *pass);

		ULONG RefCount;
		CComPtr<IWOLBrowser> Browser;
		wchar_t PendingURL[512];
		bool Visible;
		HWND Window;
};

WebBrowser * __fastcall Create_Web_Browser(HWND window, RECT *rect);
