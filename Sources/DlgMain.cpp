/*
 * Copyright (C) 2015 Andrey Rychkov <wholegroup@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "SensLock.h"
#include "DlgMain.h"
#include "DlgOptions.h"

//////////////////////////////////////////////////////////////////////////
// ����������� ��-���������
//
CMainDlg::CMainDlg(BOOL bOnlyOption) :
	m_bOnlyOption(bOnlyOption),
	m_iSeconds(DEFAULT_SECONDS),
	m_iCurSecond(DEFAULT_NUMBER_SETTINGS),
	m_bTimerOn(FALSE),
	m_bLock(FALSE),
	m_bLogoff(FALSE),
	m_bHotkeyOn(FALSE),
	m_pNotifyLock(NULL),
	m_bOffAfterLock(FALSE),
	m_dwOffTime(0),
	m_bIsLocked(FALSE)
{
	// �������� ��������
#ifdef _DEBUG
	OpenSettings(COMPANY_NAME, PROGRAM_NAME L" (debug)");
#else
	OpenSettings(COMPANY_NAME, PROGRAM_NAME);
#endif

	// ����� ���� ��������� ������ ���������
	m_mapLanguages.clear();

	SearchLanguages();

	// ��������� ����� ���������
	m_wLanguageID = (WORD)GetSettingsInteger(L"m_wLanguageID", GetUserDefaultLangID());

	CMapWordStr::const_iterator item = m_mapLanguages.find(m_wLanguageID);
	if (m_mapLanguages.end() == item)
	{
		m_wLanguageID = DEFAULT_LANGUAGE;
	}

	// ���� ������
	m_crColor = static_cast<DWORD>(GetSettingsInteger(L"m_crColor", DEFAULT_COLOR));

	// ��������
	m_iSeconds = GetSettingsInteger(L"m_iSeconds", DEFAULT_SECONDS);

	// hotkey
	m_dwHotkeyOff = static_cast<DWORD>(GetSettingsInteger(L"m_dwHotkeyOff", 0));

	// ������ ����
	m_fHeight = GetSettingsFloat(L"m_fHeight", DEFAULT_HEIGHT);

	// ������������
	m_bTransparency = static_cast<BYTE>(GetSettingsInteger(L"m_bTransparency", DEFAULT_TRANSPARENCY));

	// ���� ���������� ������� �������
	m_bLock = GetSettingsBoolean(L"m_bLock", DEFAULT_LOCK);

	// ���� ������ �� �������
	m_bLogoff = GetSettingsBoolean(L"m_bLogoff", DEFAULT_LOGOFF);

	// ���� ���������� �������� ��� ������� ���������
	m_bOffOnRun = GetSettingsBoolean(L"m_bOffOnRun", DEFAULT_OFFONRUN);

	// �������� ������� ������� � ��������� ����������
	if (GetSettingsBoolean(L"First", TRUE))
	{
		SetAutostart(TRUE);
	}

	SetSettingsBoolean(L"First", FALSE);

	// ���������� �������� ����� ���������� �������
	m_bOffAfterLock = GetSettingsBoolean(L"m_bOffAfterLock", DEFAULT_OFFAFTERLOCK);

	// ���������� �������� ����� ������� ������������
	m_bOffAfterSS = GetSettingsBoolean(L"m_bOffAfterSS", DEFAULT_OFFAFTERSS);

	// ���������� �������� ��� ��������� ���� � ������� ����� ����
	m_bOffLeftUp = GetSettingsBoolean(L"m_bOffLeftUp", DEFAULT_OFFLEFTUP);
}


//////////////////////////////////////////////////////////////////////////
// ���������� ��-���������
//
CMainDlg::~CMainDlg()
{
}


//////////////////////////////////////////////////////////////////////////
// ���������� ���������
//
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return ::IsDialogMessage(m_hWnd, pMsg);
}


//////////////////////////////////////////////////////////////////////////
// ������������� �������
//
LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// ����������� ������� ��������� (����� ��� ������ ESC = IDCANCEL)
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	// ��������� ������ ����������
	HICON hIconSmall = reinterpret_cast<HICON>(::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_OFFMON), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));

	SetIcon(hIconSmall, FALSE);

	// �������� ������ � ����
	CreateNotifyIcon();
	SetNotifyIconTip(PROGRAM_NAME L" v" PROGRAM_VERSION);

	// ����������� hotkey
	SetHotkeyOff(m_dwHotkeyOff);

	// �������� ������� ��� ��������� �������
	m_pNotifyLock = new CNotifyLock(this);

	if (m_bOffAfterLock)
	{
		m_bOffAfterLock = FALSE;
		SetOffAfterLock(TRUE);
	}

	// �������� ����������� ����
	ModifyStyleEx(0, WS_EX_LAYERED);

	// ��������� ������ ����
	ATLVERIFY(SetSize());

	// ������������ ����
	SetTransparency(m_bTransparency);

	// �������� ����
	ShowWindow(SW_HIDE);

	// ���������� �������� ��� ������� ��������� ���� ���������� ����
	if (!m_bOnlyOption && m_bOffOnRun)
	{
		PostMessage(WM_COMMAND, IDB_OFFMON);
	}

	// ����� ������� ��������, ���� ��� ������� ��� ��������������� ����
	if (m_bOnlyOption)
	{
		PostMessage(WM_COMMAND, IDB_OPTIONS);
	}

	// ������ ������� ��� ������������ ������� ����
	SetOffLeftUp(m_bOffLeftUp);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// ����������� �������
//
LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// ���������� ����������� ���������� ����������
	if (NULL != m_pNotifyLock)
	{
		if (TRUE == m_bOffAfterLock)
		{
			m_pNotifyLock->Deinit();
		}
		delete m_pNotifyLock;
		m_pNotifyLock = NULL;
	}

	// �������� �������
	if (m_bTimerOn)
	{
		ATLVERIFY(KillTimer(OFFMON_TIMER_COUNTDOWN));
		m_bTimerOn = FALSE;
	}

	if (m_bOffLeftUp)
	{
		ATLVERIFY(KillTimer(OFFMON_TIMER_LEFTUP));
		m_bOffLeftUp = FALSE;
	}

	// �������� ������
	if (m_bHotkeyOn)
	{
		ATLVERIFY(UnregisterHotKey(*this, 0));
		m_bHotkeyOn = FALSE;
	}

	// �������� ������� ���������
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);

	pLoop->RemoveMessageFilter(this);

	// ��������� ��������� ���������
	SetSettingsInteger(L"m_wLanguageID",   m_wLanguageID);
	SetSettingsInteger(L"m_iSeconds",      m_iSeconds);
	SetSettingsInteger(L"m_crColor",       static_cast<INT>(m_crColor));
	SetSettingsInteger(L"m_dwHotkeyOff",   m_dwHotkeyOff);
	SetSettingsFloat(L"m_fHeight",         m_fHeight);
	SetSettingsInteger(L"m_bTransparency", m_bTransparency);
	SetSettingsBoolean(L"m_bLock",         m_bLock);
	SetSettingsBoolean(L"m_bLogoff",       m_bLogoff);
	SetSettingsBoolean(L"m_bOffOnRun",     m_bOffOnRun);
	SetSettingsBoolean(L"m_bOffAfterLock", m_bOffAfterLock);
	SetSettingsBoolean(L"m_bOffAfterSS",   m_bOffAfterSS);
	SetSettingsBoolean(L"m_bOffLeftUp",    m_bOffLeftUp);

	CloseSettings(TRUE);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ��������� IDOK
//
LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ��������� IDCANCEL
//
LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_bTimerOn)
	{
		ATLVERIFY(KillTimer(OFFMON_TIMER_COUNTDOWN));
		m_bTimerOn = FALSE;
		
		ShowWindow(SW_HIDE);

		// �����, ���� ����� ���� ���������� ��� ������� ���������
		if (m_bOffOnRun)
		{
			PostMessage(WM_CLOSE);
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ����� �� ����������
//
LRESULT CMainDlg::OnQuit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// �������� �������
//
VOID CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();

	::PostQuitMessage(nVal);
}


//////////////////////////////////////////////////////////////////////////
// ����� ���� ��������� ������ ���������
//
VOID CMainDlg::SearchLanguages()
{
	if (FALSE == ::EnumResourceLanguages(GetModuleHandle(NULL), RT_DIALOG, 
		MAKEINTRESOURCE(IDD_OPTIONS), (ENUMRESLANGPROC)CMainDlg::SearchLanguageCallback, 
		(LONG_PTR)&m_mapLanguages))
	{
		MessageBox(L"Error::EnumResourceLanguages");
	}
	ATLASSERT(0 < m_mapLanguages.size());
}


BOOL CALLBACK CMainDlg::SearchLanguageCallback(HMODULE hModule, LPCTSTR lpszType, 
	LPTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam)
{
	// �������������� ���������
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpszType);
	UNREFERENCED_PARAMETER(lpszName);

	// �������� ������� ��������
	ATLASSERT(GetModuleHandle(NULL) == hModule);
	ATLASSERT(RT_DIALOG == lpszType);
	ATLASSERT(MAKEINTRESOURCE(IDD_OPTIONS) == lpszName);

	// ��������� ��������� �� ������ ������
	CMapWordStr *pMap = (CMapWordStr*)lParam;

	// ��������� ������������ ����� �� ��� �������������� wIDLanguage
	std::vector<WCHAR> wcBuf(128);

	GetLocaleInfo(MAKELCID(wIDLanguage, 0), LOCALE_SNATIVELANGNAME, &wcBuf.front(), (INT)wcBuf.size());

	// ������� ������ ����� � ������� �������
	CharUpperBuff(&wcBuf.front(), 1);

	// ���������� ����� � ������
	pMap->insert(make_pair(wIDLanguage, &wcBuf.front()));

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// ��������� ������ �� ���������� �������������� � ������������ � 
// ������������� ������
//
std::wstring CMainDlg::GetStringLang(DWORD dwResID)
{
	LPCWSTR lpStr;

	lpStr = (LPCWSTR)GetResourceLang(RT_STRING, 1 + (dwResID >> 4));
	if (NULL == lpStr)
	{
		return L"";
	}

	for (DWORD dwStrPos = 0; dwStrPos < (dwResID & 0x000F); dwStrPos++)
	{
		lpStr += *lpStr + 1;
	}

	std::wstring wsStr = (lpStr + 1);

	wsStr.erase(*lpStr);

	return wsStr;
}


//////////////////////////////////////////////////////////////////////////
// ��������� ��������� ������
//
HGLOBAL CMainDlg::GetResourceLang(LPWSTR lpResType, DWORD dwResID)
{
	HINSTANCE hAppInstance = GetModuleHandle(NULL);
	HRSRC     hrRes;

	hrRes = FindResourceEx(hAppInstance, lpResType, MAKEINTRESOURCE(dwResID), m_wLanguageID);
	if (NULL == hrRes)
	{
		hrRes = FindResourceEx(hAppInstance, lpResType, MAKEINTRESOURCE(dwResID), DEFAULT_LANGUAGE);
	}

	return LoadResource(hAppInstance, hrRes);
}


//////////////////////////////////////////////////////////////////////////
// ������������� ������� �������
//
VOID CMainDlg::DoInitTemplate() 
{
	m_Template.Create((LPDLGTEMPLATE)GetResourceLang(RT_DIALOG, this->IDD));
}


//////////////////////////////////////////////////////////////////////////
// ������������� ��������� �������
//
VOID CMainDlg::DoInitControls() 
{
	ATLASSERT(TRUE);
}


//////////////////////////////////////////////////////////////////////////
// ����� ���� � ���� �� ������� ����� ������ ����
//
VOID CMainDlg::OnNotifyIconLButtonUp(const POINT& rpt)
{
	OnNotifyIconRButtonUp(rpt);
}


//////////////////////////////////////////////////////////////////////////
// ����� ���� � ���� �� ������� ������ ������ ����
//
VOID CMainDlg::OnNotifyIconRButtonUp(const POINT& rpt)
{
	CMenu       menuPopup;
	menuPopup.LoadMenuIndirect(GetResourceLang(RT_MENU, IDR_MENU_POPUP));

	CMenuHandle menuVisible;
	menuVisible = menuPopup.GetSubMenu(0);

	// ��������� ������ ��������� ������
	menuVisible.SetMenuDefaultItem(IDB_OFFMON);

	// ����� ����
	SetForegroundWindow(*this); // ��� ����, ����� ���� �������� ��� ������ ������
	menuVisible.TrackPopupMenuEx(TPM_RIGHTALIGN | TPM_BOTTOMALIGN, rpt.x, rpt.y, *this); 
	PostMessage(WM_NULL);
}


//////////////////////////////////////////////////////////////////////////
// �������� ������� �� WM_CLOSE
//
LRESULT CMainDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CloseDialog(0);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ����� ������� ��������
//
LRESULT CMainDlg::OnOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDlgOptions dlg(this);

	dlg.DoModal(*this);

	// �����, ���� ����� ���� ���������� ��� ������� ���������
	if (m_bOnlyOption || m_bOffOnRun)
	{
		PostMessage(WM_CLOSE);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ������� ������� �� ���������� ��������
//
LRESULT CMainDlg::OnOffMon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_iCurSecond = m_iSeconds;

	if (SetTimer(OFFMON_TIMER_COUNTDOWN, DEFAULT_TIMER_COUNTDOWN, NULL))
	{
		m_bTimerOn = TRUE;

		RenderImage();

		SetForegroundWindow(*this);
		ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		_RPT_API_ERROR(SetTimer);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ��������� �������
//
LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// �� �������
	UINT_PTR uiTimerID = static_cast<UINT_PTR>(wParam);

	// ������ ������� �� ����������
	if (OFFMON_TIMER_COUNTDOWN == uiTimerID)
	{
		m_iCurSecond--;

		if (0 == m_iCurSecond)
		{
			if (m_bTimerOn)
			{
				ATLVERIFY(KillTimer(OFFMON_TIMER_COUNTDOWN));
				m_bTimerOn = FALSE;
			}

			ShowWindow(SW_HIDE);

			#ifndef _DEBUG
			::PostMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);

			if (m_bLogoff)
			{
				ExitWindows(0, 0);
			}
			else

			if (m_bLock)
			{
				LockWorkStation();
			}
			#endif

			// �����, ���� ����� ���� ���������� ��� ������� ���������
			if (m_bOffOnRun)
			{
				PostMessage(WM_CLOSE);
			}
		}
		else
		{
			RenderImage();
		}
	}
	else

	// ������ ��� �������� ���� � ����� ������� ����
	// ���� �� ��� ��������� �������� �����, � �� ���� ����������, �� ��������� �������
	if ((OFFMON_TIMER_LEFTUP == uiTimerID) && (FALSE == GetIsLocked()))
	{
		// ���������� ������� ����
		CPoint ptCursor;
		GetCursorPos(&ptCursor);

		// ��������� ���������� ����� ������
		LASTINPUTINFO lii;
		lii.cbSize = sizeof(lii);
		lii.dwTime = 0;

		GetLastInputInfo(&lii);

		if ((m_dwOffTime != lii.dwTime) && (0 == ptCursor.x) && (0 == ptCursor.y))
		{
			// ���������� ��������
			if ((GetTickCount() - lii.dwTime) >= DEFAULT_TIME_LEFTUP)
			{
				m_dwOffTime = lii.dwTime;

				::PostMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
			}
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ����� �����
//
LRESULT CMainDlg::OnChangeLanguage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// �� �����
	WORD wLanguageId = static_cast<WORD>(wParam);

	// ����� �����
	CMapWordStr::iterator find = m_mapLanguages.find(wLanguageId);

	// �������, ���� ��������� ���� �� ������������
	if (find == m_mapLanguages.end())
	{
		return static_cast<LRESULT>(0);
	}

	// ����� �����
	m_wLanguageID = wLanguageId;

	// ���������� ���������
	CloseDialog(CHANGE_LANGUAGE);	

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// ��������� ������� ������� ����������
//
VOID CMainDlg::SetHotkeyOff(DWORD dwHotkeyShow)
{
	m_dwHotkeyOff = dwHotkeyShow;

	if (m_bHotkeyOn)
	{
		ATLVERIFY(UnregisterHotKey(*this, 0));

		m_bHotkeyOn = FALSE;
	}

	if (RegisterHotKey(*this, 0, TransModifiers(HIBYTE(LOWORD(m_dwHotkeyOff))), 
		LOBYTE(LOWORD(m_dwHotkeyOff))))
	{
		m_bHotkeyOn = TRUE;
	}
}


//////////////////////////////////////////////////////////////////////////
// ������������� ����� ������ �������
//
VOID CMainDlg::SetHeight(FLOAT fHeight)
{
	m_fHeight = fHeight;

	ATLVERIFY(SetSize());
}


//////////////////////////////////////////////////////////////////////////
// ���������������� ��������������� ����������� ������
// (��-�� ��������������� GetHotkey and RegisterHotkey)
//
UINT CMainDlg::TransModifiers(WORD wMod)
{
	UINT uMod = 0;

	if (HOTKEYF_ALT & wMod)
	{
		uMod |= MOD_ALT;
	}

	if (HOTKEYF_SHIFT & wMod)
	{
		uMod |= MOD_SHIFT;
	}

	if (HOTKEYF_CONTROL & wMod)
	{
		uMod |= MOD_CONTROL;
	}

	return uMod;
}


//////////////////////////////////////////////////////////////////////////
// ��������� ������� ������� �������
//
LRESULT CMainDlg::OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_COMMAND, IDB_OFFMON);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ��������� ������/�������� ����
//
LRESULT CMainDlg::OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!wParam)
	{
		m_iCurSecond = DEFAULT_NUMBER_SETTINGS;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ��������� ������ ����
//
BOOL CMainDlg::SetSize()
{
	// ��������� �������� ��������
	CRect rectDesktop;
	::GetClientRect(GetDesktopWindow(), &rectDesktop);

	// ������� 
	INT lMin = rectDesktop.Height() > rectDesktop.Width() ? rectDesktop.Width() : rectDesktop.Height();

	// ���������� ������� �����
	m_iWidthArc = static_cast<INT>(lMin * m_fHeight * DEFAULT_WIDTH_CIRCLE);

	// ��������� �������� ����
	ResizeClient(static_cast<INT>(lMin * m_fHeight), static_cast<INT>(lMin * m_fHeight));

	// ��������� �����������
	RenderImage();

	// ������������ ���� �� ������
	CenterWindow();

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// ��������� ������������
//
VOID CMainDlg::SetTransparency(BYTE bTransparency)
{
	m_bTransparency = bTransparency;

	RenderImage();
}


//////////////////////////////////////////////////////////////////////////
// ���������� TRUE, ���� ��������� ����������� ������ � Windows
//
BOOL CMainDlg::GetAutostart()
{
	BOOL bAutostart = FALSE;
	HKEY hKey;

	if (ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey))
	{
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, PROGRAM_NAME, NULL, NULL, NULL, NULL))
		{
			bAutostart = TRUE;
		}
		else
		{
			bAutostart = FALSE;
		}

		RegCloseKey(hKey);
	}

	return bAutostart;
}


//////////////////////////////////////////////////////////////////////////
// ������������� ����� ���������� ��� ������� Windows
//
VOID CMainDlg::SetAutostart(BOOL bAutostart)
{
	HKEY  hKey;
	WCHAR ptcFileName[MAX_PATH];

	if (ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &hKey))
	{
		if (bAutostart)
		{
			ZeroMemory(ptcFileName, sizeof(WCHAR) * MAX_PATH);
			GetModuleFileName(NULL, ptcFileName, sizeof(WCHAR) * (MAX_PATH - 1));
			RegSetValueEx(hKey, PROGRAM_NAME, 0, REG_SZ, (BYTE*)ptcFileName, 
				static_cast<DWORD>(wcslen(ptcFileName) * sizeof(WCHAR)));
		}
		else
		{
			RegDeleteValue(hKey, PROGRAM_NAME);
		}

		RegCloseKey(hKey);
	}
}


//////////////////////////////////////////////////////////////////////////
// ��������� ���������� �������� ����� ���������� �������
//
VOID CMainDlg::SetOffAfterLock(BOOL bOffAfterLock)
{
	if ((TRUE == m_bOffAfterLock) && (FALSE == bOffAfterLock))
	{
		m_pNotifyLock->Deinit();
	}

	if ((TRUE == bOffAfterLock) && (FALSE == m_bOffAfterLock))
	{
		m_pNotifyLock->Init();
	}

	m_bOffAfterLock = bOffAfterLock;
}


//////////////////////////////////////////////////////////////////////////
// ��������� �����������
//
VOID CMainDlg::RenderImage()
{
	// ��������� ������� ���������
	CRect rectClient;
	CRect rectWindow;

	GetClientRect(&rectClient);
	GetWindowRect(&rectWindow);

	// �������� �������
	BITMAPINFO bmpInfo;

	bmpInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER); 
	bmpInfo.bmiHeader.biWidth         = rectClient.Width(); 
	bmpInfo.bmiHeader.biHeight        = rectClient.Height(); 
	bmpInfo.bmiHeader.biPlanes        = 1; 
	bmpInfo.bmiHeader.biBitCount      = 32; 
	bmpInfo.bmiHeader.biCompression   = BI_RGB; 
	bmpInfo.bmiHeader.biSizeImage     = 0; 
	bmpInfo.bmiHeader.biXPelsPerMeter = 0; 
	bmpInfo.bmiHeader.biYPelsPerMeter = 0; 
	bmpInfo.bmiHeader.biClrUsed       = 0; 
	bmpInfo.bmiHeader.biClrImportant  = 0; 

	HDC   dcMem = ::CreateCompatibleDC(GetDC()); 
	PVOID pvBuf = NULL;

	HBITMAP hBmp    = ::CreateDIBSection(dcMem, &bmpInfo, DIB_RGB_COLORS, &pvBuf, 0, 0);
	HBITMAP hBmpOld = static_cast<HBITMAP>(::SelectObject(dcMem, hBmp));

	// ������������� AGG
	agg::rendering_buffer aggRendBuf;
	aggRendBuf.attach((PUCHAR)pvBuf, rectClient.Width(), rectClient.Height(), rectClient.Width() * 4);

	agg::pixfmt_bgra32                     aggPixF(aggRendBuf);
	agg::renderer_base<agg::pixfmt_bgra32> aggRenB(aggPixF);

	// ������� ����
	aggRenB.clear(agg::rgba8(0, 0, 0, 0));

	agg::renderer_scanline_aa_solid<agg::renderer_base<agg::pixfmt_bgra32>> aggRen(aggRenB);
	agg::rasterizer_scanline_aa<>                                           aggRas;
	agg::scanline_p8                                                        aggSl;

	// ����������
	double dRadius       = (rectClient.Width() < rectClient.Height()) ? rectClient.Width() / 2.L : rectClient.Height() / 2.L;
	double dEllipseWidth = static_cast<double>(m_iWidthArc);

	agg::ellipse                   aggEllipse(dRadius, dRadius, dRadius - dEllipseWidth / 2.L, dRadius - dEllipseWidth / 2.L);
	agg::conv_stroke<agg::ellipse> aggEStr(aggEllipse);

	aggEStr.width(dEllipseWidth);

	aggRas.add_path(aggEStr);
	aggRen.color(agg::rgba8(GetRValue(m_crColor), GetGValue(m_crColor), GetBValue(m_crColor), m_bTransparency));

	agg::render_scanlines(aggRas, aggSl, aggRen);

	// ������� �������������
	aggRas.reset();

	// ������������� ������
	font_engine_type     aggFontEng(dcMem);
	font_manager_type    aggFontMan(aggFontEng);
	conv_font_curve_type aggFontCurves(aggFontMan.path_adaptor());

	aggFontEng.height(300);
	aggFontEng.weight(FW_BOLD);
	aggFontEng.create_font("Arial", agg::glyph_ren_outline);

	// ��������� ������� �����
	std::wostringstream ssNumber;
	ssNumber << m_iCurSecond;

	const agg::glyph_cache* aggGlyph = aggFontMan.glyph(static_cast<WCHAR>(*(ssNumber.str().c_str())));

	// ���������� ������������ ���������� ������
	double dFontWidth  = aggGlyph->bounds.x2 - aggGlyph->bounds.x1;
	double dFontHeight = aggGlyph->bounds.y2 - aggGlyph->bounds.y1;
	double dScale      = rectClient.Height() / dFontHeight * DEFAULT_HEIGHT_NUMBER;

	// ���������� �������� ������
	double dFontX = dRadius - (dFontWidth * dScale) / 2 - aggGlyph->bounds.x1 * dScale;
	double dFontY = dRadius - (dFontHeight * dScale) / 2 - aggGlyph->bounds.y1 * dScale;

	aggFontMan.add_kerning(&dFontX, &dFontY);
	aggFontMan.init_embedded_adaptors(aggGlyph, dFontX, dFontY, dScale);

	// ������������
	aggRas.add_path(aggFontCurves);
	aggRen.color(agg::rgba8(GetRValue(m_crColor), GetGValue(m_crColor), GetBValue(m_crColor), m_bTransparency));

	agg::render_scanlines(aggRas, aggSl, aggRen);

	// ���������� ����
	BLENDFUNCTION blend;

	blend.BlendOp             = AC_SRC_OVER;
	blend.BlendFlags          = 0;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat         = AC_SRC_ALPHA;

	if (FALSE == UpdateLayeredWindow(m_hWnd, NULL, &rectWindow.TopLeft(), &rectClient.Size(), 
		dcMem, &rectClient.TopLeft(), NULL, &blend, ULW_ALPHA))
	{
		MessageBox(L"Error in UpdateLayeredWindow", L"Update failed", MB_OK | MB_ICONERROR);
	}

	// �������� ��������� ��������
	::SelectObject(dcMem, hBmpOld);
	::DeleteObject(hBmp);
	::DeleteDC(dcMem);
}


//////////////////////////////////////////////////////////////////////////
// ��������� ���������� �������� ��� ������� ������������	
//
VOID CMainDlg::SetOffAfterSS(BOOL bOffAfterSS)
{
	m_bOffAfterSS = bOffAfterSS;
}


//////////////////////////////////////////////////////////////////////////
// ��������� ���������� �������� ��� ��������� ������� � ����� ������� ����
//
VOID CMainDlg::SetOffLeftUp(BOOL bOffLeftUp)
{
	if (TRUE == bOffLeftUp)
	{
		SetTimer(OFFMON_TIMER_LEFTUP, DEFAULT_TIMER_LEFTUP, NULL);
	}
	else
	{
		KillTimer(OFFMON_TIMER_LEFTUP);
	}

	m_bOffLeftUp = bOffLeftUp;
}
