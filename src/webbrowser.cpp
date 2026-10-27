// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <math.h>
#include "dbgprint.h"
#include "webbrowser.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

bool __fastcall FUN_00551630(const char *url);

// SYNTHETIC: GAMEMD 0x00773240 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x00773260 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00773290 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x007732B0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x007732D0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x007732F0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00773310 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00773330 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00773370 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x007733A0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x007733D0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00773400 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00773410 SYMBOL
// _$E35

extern int DAT_00B72CF0;
extern int DAT_00B72CF4;
extern int DAT_00B72CF8;

// FUNCTION: GAMEMD 0x00773430
void FUN_00773430(void)
{
	DAT_00B72CF8 = 0;
	DAT_00B72CF0 = 128;
	DAT_00B72CF4 = 128;
}

// FUNCTION: GAMEMD 0x00773450
WebBrowser * __fastcall Create_Web_Browser(HWND window, RECT *rect)
{
	WebBrowser *browser = new WebBrowser;
	if (browser) {
		browser->Window = window;
		DebugString("Creating WOLBrowser component.\n");
		if (SUCCEEDED(CoCreateInstance(CLSID_WOLBrowser, NULL, CLSCTX_INPROC_SERVER, IID_IWOLBrowser, (void **)&browser->Browser))) {
			DebugString("Starting up WOLBrowser\n");
			browser->Browser->AddObserver(browser);
			browser->Browser->Startup(window, rect);
			browser->Browser->AllowPageCertification(TRUE);
		}
	}
	return browser;
}

// FUNCTION: GAMEMD 0x00773500
WebBrowser::WebBrowser(void) :
	RefCount(1),
	Visible(false),
	Window(NULL)
{
	DebugString("Instantiating embedded WebBrowser\n");
}

// FUNCTION: GAMEMD 0x00773540
WebBrowser::~WebBrowser(void)
{
	DebugString("Destructing embedded WebBrowser\n");
	if (Browser) {
		DebugString("Releasing WOLBrowser\n");
		Browser->RemoveObserver(this);
		Browser->Shutdown();
	}
}

// FUNCTION: GAMEMD 0x00773590
bool WebBrowser::Init(HWND window, RECT *rect)
{
	Window = window;
	DebugString("Creating WOLBrowser component.\n");
	if (SUCCEEDED(CoCreateInstance(CLSID_WOLBrowser, NULL, CLSCTX_INPROC_SERVER, IID_IWOLBrowser, (void **)&Browser))) {
		DebugString("Starting up WOLBrowser\n");
		Browser->AddObserver(this);
		Browser->Startup(window, rect);
		Browser->AllowPageCertification(TRUE);
	}
	return true;
}

// FUNCTION: GAMEMD 0x00773600
bool WebBrowser::Show_Page(char *page)
{
	char url[512];

	if (Get_Page_URL(page, url, sizeof(url))) {
		DebugString("Showing webpage '%s' - %s\n", page, url);
		if (Browser) {
			mbstowcs(PendingURL, url, 512);

			char path[MAX_PATH];
			if (Get_HTML_Path(path, MAX_PATH)) {
				wchar_t framePage[MAX_PATH];
				mbstowcs(framePage, path, MAX_PATH);
				wcscat(framePage, L"frame.htm");

				if (SUCCEEDED(Browser->Navigate(framePage, 0, NULL))) {
					Show();
					return true;
				}
			}
		} else {
			return FUN_00551630(url);
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x00773860
void WebBrowser::Show(void)
{
	if (Browser) {
		Browser->Show();
		Visible = true;
	}
}

// FUNCTION: GAMEMD 0x00773880
void WebBrowser::Hide(void)
{
	if (Browser) {
		Browser->Hide();
		Visible = false;
	}
}

// FUNCTION: GAMEMD 0x007738A0
bool WebBrowser::Get_Page_URL(const char *page, char *url, int size)
{
	HKEY key;
	LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Westwood\\Yuri's Revenge\\URL", 0, KEY_READ, &key);

	if (result == ERROR_SUCCESS) {
		char valueName[64];
		strcpy(valueName, page);
		if (Browser == NULL) {
			strcat(valueName, "X");
		}

		DWORD type;
		DWORD sizeOfBuffer = size;
		result = RegQueryValueEx(key, valueName, NULL, &type, (unsigned char *)url, &sizeOfBuffer);
		RegCloseKey(key);
	}
	return result == ERROR_SUCCESS;
}

// FUNCTION: GAMEMD 0x00773970
bool WebBrowser::Get_HTML_Path(char *path, int size)
{
	HKEY key;
	LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Westwood\\Yuri's Revenge", 0, KEY_READ, &key);

	if (result == ERROR_SUCCESS) {
		DWORD type;
		DWORD sizeOfBuffer = size;
		result = RegQueryValueEx(key, "HTMLPath", NULL, &type, (unsigned char *)path, &sizeOfBuffer);
		RegCloseKey(key);

		if (result == ERROR_SUCCESS) {
			char *slash = strrchr(path, '\\');
			if (slash == NULL || strlen(path) > (size_t)(slash - path) + 1) {
				strcat(path, "\\");
			}
		}
	}
	return result == ERROR_SUCCESS;
}

// FUNCTION: GAMEMD 0x00773A40
HRESULT STDMETHODCALLTYPE WebBrowser::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;
	if (riid == IID_IUnknown || riid == IID_IWOLBrowserEvent) {
		*ppvObject = static_cast<IWOLBrowserEvent *>(this);
	} else {
		return E_NOINTERFACE;
	}
	static_cast<IUnknown *>(*ppvObject)->AddRef();
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773AA0
ULONG STDMETHODCALLTYPE WebBrowser::AddRef(void)
{
	return ++RefCount;
}

// FUNCTION: GAMEMD 0x00773AB0
ULONG STDMETHODCALLTYPE WebBrowser::Release(void)
{
	if (--RefCount == 0) {
		delete this;
		return 0;
	}
	return RefCount;
}

// FUNCTION: GAMEMD 0x00773AD0
HRESULT STDMETHODCALLTYPE WebBrowser::OnScriptQuit(void)
{
	if (Window) {
		*(int *)GetWindowLong(Window, 8) = 1;
	}
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773B00
HRESULT STDMETHODCALLTYPE WebBrowser::OnBeforeNavigate(const wchar_t *, const wchar_t *)
{
	return S_OK;
}

extern wchar_t DAT_008496B8[];

// FUNCTION: GAMEMD 0x00773B10
HRESULT STDMETHODCALLTYPE WebBrowser::OnDocumentComplete(const wchar_t *, BOOL isTopFrame)
{
	if (isTopFrame) {
		Browser->Navigate(PendingURL, 0, DAT_008496B8);
	}
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773B40
HRESULT STDMETHODCALLTYPE WebBrowser::OnDownloadBegin(void)
{
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773B50
HRESULT STDMETHODCALLTYPE WebBrowser::OnProgressChange(LONG, LONG)
{
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773B60
HRESULT STDMETHODCALLTYPE WebBrowser::OnDownloadComplete(void)
{
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773B70
HRESULT STDMETHODCALLTYPE WebBrowser::OnNavigateComplete(const wchar_t *)
{
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773B80
HRESULT STDMETHODCALLTYPE WebBrowser::OnStatusTextChange(const wchar_t *)
{
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773B90
HRESULT STDMETHODCALLTYPE WebBrowser::OnTitleChange(const wchar_t *)
{
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773BA0
HRESULT STDMETHODCALLTYPE WebBrowser::OnNewWindow(void)
{
	return E_FAIL;
}

// FUNCTION: GAMEMD 0x00773BB0
HRESULT STDMETHODCALLTYPE WebBrowser::OnShowMessage(const wchar_t *, const wchar_t *, ULONG, LONG *)
{
	return S_OK;
}
