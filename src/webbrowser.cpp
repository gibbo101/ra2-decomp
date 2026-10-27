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
#include "wstring.h"
#include "rcptr.h"
#include "playerprofile.h"
#include "wolopt.h"
#include "session.h"

bool __fastcall FUN_00551630(const char *url);
rc_ptr<PlayerProfile> __fastcall FUN_00778A20(Wstring const * name);
int __fastcall FUN_00789500(char const * login, char const * password, int plain, int locale);
void __fastcall FUN_00606740(HWND dialog, bool value);
extern Wstring DAT_00B73E40;
extern Wstring DAT_00B7778C;
extern WebBrowser *DAT_00B72D0C;
namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
	bool Updated(void);
}
namespace OwnerDraw { void __fastcall UpdateControlPosition(HWND dialog, LPARAM lparam); }

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

// FUNCTION: GAMEMD 0x00773BC0
HRESULT STDMETHODCALLTYPE WebBrowser::OnFailedPageCertification(void)
{
	DebugString("!!!!! PAGE FAILED CERTIFICATION !!!!!\n");

	char path[MAX_PATH];
	if (Get_HTML_Path(path, MAX_PATH)) {
		mbstowcs(PendingURL, path, MAX_PATH);
		wcscat(PendingURL, L"BadPage.htm");

		wchar_t framePage[MAX_PATH];
		mbstowcs(framePage, path, MAX_PATH);
		wcscat(framePage, L"frame.htm");
		return Browser->Navigate(framePage, 0, NULL);
	}
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773D20
HRESULT STDMETHODCALLTYPE WebBrowser::OnErrorMsg(const wchar_t *error)
{
	DebugString("OnErrorMsg- %S\n", error);
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773D40
HRESULT STDMETHODCALLTYPE WebBrowser::OnRegisterLogin(const wchar_t *nick, const wchar_t *pass)
{
	char nickName[64];
	char password[64];
	wcstombs(nickName, nick, 64);
	wcstombs(password, pass, 64);

	Wstring name(nickName);
	DebugString("Register WWOnline login: %s - %s\n", nickName, password);
	rc_ptr<PlayerProfile> profile = FUN_00778A20(&name);

	WOLOptionsClass options;
	if (options.Is_Bit1_Clear()) {
		int nicknum = FUN_00789500(nickName, password, 1, profile->Field10);
		Session.Write_MultiPlayer_Settings();
	}
	DAT_00B73E40 = nickName;
	DAT_00B7778C = password;
	return S_OK;
}

// FUNCTION: GAMEMD 0x00773E30
BOOL CALLBACK Web_Page_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	BOOL rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	if (message == WM_INITDIALOG) {
		POINT size = { 800, 600 };
		OwnerDraw::UpdateControlPosition(window, (LPARAM)&size);
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x00773E90
void __fastcall Do_Web_Page_Dialog(const char *page)
{
	if (page == NULL || strlen(page) == 0) {
		DebugString("DoWebPageDialog: Invalid parameter.\n");
		return;
	}

	DebugString("Creating WebBrowser Dialog for %s.\n", page);
	HWND dialog = UI::BeginDialog(0x131, (DLGPROC)Web_Page_Dialog_Proc, 0);
	if (dialog == NULL) {
		return;
	}

	int quit = 0;
	SetWindowLong(dialog, 8, (LONG)&quit);
	FUN_00606740(dialog, true);
	UI::FocusOnWindow(dialog);

	RECT client;
	GetClientRect(dialog, &client);
	RECT rect;
	rect.left = ((client.right - client.left) - 800) / 2 + client.left;
	rect.right = rect.left + 800;
	rect.top = ((client.bottom - client.top) - 600) / 2 + client.top;
	rect.bottom = rect.top + 600;

	WebBrowser *browser = new WebBrowser;
	if (browser) {
		browser->Window = dialog;
		DebugString("Creating WOLBrowser component.\n");
		if (SUCCEEDED(CoCreateInstance(CLSID_WOLBrowser, NULL, CLSCTX_INPROC_SERVER, IID_IWOLBrowser, (void **)&browser->Browser))) {
			DebugString("Starting up WOLBrowser\n");
			browser->Browser->AddObserver(browser);
			browser->Browser->Startup(dialog, &rect);
			browser->Browser->AllowPageCertification(TRUE);
		}
	}
	DAT_00B72D0C = browser;
	if (browser) {
		if (browser->Show_Page((char *)page)) {
			while (!quit && !UI::Updated()) {
			}
		}
		DAT_00B72D0C->Release();
		DAT_00B72D0C = NULL;
	}
	UI::EndDialog(dialog);
}

// FUNCTION: GAMEMD 0x00774070
int __fastcall Is_Web_Browser_Visible(void)
{
	WebBrowser *browser = DAT_00B72D0C;
	if (browser && browser->Browser && browser->Visible) {
		return 1;
	}
	return 0;
}

// SYNTHETIC: GAMEMD 0x007740A0
// ??_GWebBrowser@@UAEPAXI@Z
