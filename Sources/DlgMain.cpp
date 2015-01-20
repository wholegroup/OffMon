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
// Конструктор по-умолчанию
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
	// открытие настроек
#ifdef _DEBUG
	OpenSettings(COMPANY_NAME, PROGRAM_NAME L" (debug)");
#else
	OpenSettings(COMPANY_NAME, PROGRAM_NAME);
#endif

	// Поиск всех возможных языков программы
	m_mapLanguages.clear();

	SearchLanguages();

	// установка языка программы
	m_wLanguageID = (WORD)GetSettingsInteger(L"m_wLanguageID", GetUserDefaultLangID());

	CMapWordStr::const_iterator item = m_mapLanguages.find(m_wLanguageID);
	if (m_mapLanguages.end() == item)
	{
		m_wLanguageID = DEFAULT_LANGUAGE;
	}

	// цвет текста
	m_crColor = static_cast<DWORD>(GetSettingsInteger(L"m_crColor", DEFAULT_COLOR));

	// задержка
	m_iSeconds = GetSettingsInteger(L"m_iSeconds", DEFAULT_SECONDS);

	// hotkey
	m_dwHotkeyOff = static_cast<DWORD>(GetSettingsInteger(L"m_dwHotkeyOff", 0));

	// высота окна
	m_fHeight = GetSettingsFloat(L"m_fHeight", DEFAULT_HEIGHT);

	// прозрачность
	m_bTransparency = static_cast<BYTE>(GetSettingsInteger(L"m_bTransparency", DEFAULT_TRANSPARENCY));

	// флаг блокировки рабочей станции
	m_bLock = GetSettingsBoolean(L"m_bLock", DEFAULT_LOCK);

	// флаг выхода из системы
	m_bLogoff = GetSettingsBoolean(L"m_bLogoff", DEFAULT_LOGOFF);

	// флаг выключения монитора при запуске программы
	m_bOffOnRun = GetSettingsBoolean(L"m_bOffOnRun", DEFAULT_OFFONRUN);

	// проверка первого запуска и установка автостарта
	if (GetSettingsBoolean(L"First", TRUE))
	{
		SetAutostart(TRUE);
	}

	SetSettingsBoolean(L"First", FALSE);

	// выключение монитора после блокировки станции
	m_bOffAfterLock = GetSettingsBoolean(L"m_bOffAfterLock", DEFAULT_OFFAFTERLOCK);

	// выключение монитора после запуска скринсейвера
	m_bOffAfterSS = GetSettingsBoolean(L"m_bOffAfterSS", DEFAULT_OFFAFTERSS);

	// выключение монитора при наведении мыши в верхний левый угол
	m_bOffLeftUp = GetSettingsBoolean(L"m_bOffLeftUp", DEFAULT_OFFLEFTUP);
}


//////////////////////////////////////////////////////////////////////////
// Деструктор по-умолчанию
//
CMainDlg::~CMainDlg()
{
}


//////////////////////////////////////////////////////////////////////////
// Фильтрация сообщений
//
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return ::IsDialogMessage(m_hWnd, pMsg);
}


//////////////////////////////////////////////////////////////////////////
// Инициализация диалога
//
LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// регистрация фильтра сообщений (нужно для работы ESC = IDCANCEL)
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	// установка иконки приложения
	HICON hIconSmall = reinterpret_cast<HICON>(::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_OFFMON), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));

	SetIcon(hIconSmall, FALSE);

	// создание иконки в трее
	CreateNotifyIcon();
	SetNotifyIconTip(PROGRAM_NAME L" v" PROGRAM_VERSION);

	// регистрация hotkey
	SetHotkeyOff(m_dwHotkeyOff);

	// создание объекта для получения событий
	m_pNotifyLock = new CNotifyLock(this);

	if (m_bOffAfterLock)
	{
		m_bOffAfterLock = FALSE;
		SetOffAfterLock(TRUE);
	}

	// создание прозрачного окна
	ModifyStyleEx(0, WS_EX_LAYERED);

	// установка высоты окна
	ATLVERIFY(SetSize());

	// прозрачность окна
	SetTransparency(m_bTransparency);

	// сокрытие окна
	ShowWindow(SW_HIDE);

	// выключения монитора при запуске программы если установлен флаг
	if (!m_bOnlyOption && m_bOffOnRun)
	{
		PostMessage(WM_COMMAND, IDB_OFFMON);
	}

	// вывод диалога настроек, если при запуске был соответствующий ключ
	if (m_bOnlyOption)
	{
		PostMessage(WM_COMMAND, IDB_OPTIONS);
	}

	// запуск таймера для отслеживания позиции мыши
	SetOffLeftUp(m_bOffLeftUp);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Уничтожение диалога
//
LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// завершение ослеживания блокировки компьютера
	if (NULL != m_pNotifyLock)
	{
		if (TRUE == m_bOffAfterLock)
		{
			m_pNotifyLock->Deinit();
		}
		delete m_pNotifyLock;
		m_pNotifyLock = NULL;
	}

	// удаление таймера
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

	// удаление хоткея
	if (m_bHotkeyOn)
	{
		ATLVERIFY(UnregisterHotKey(*this, 0));
		m_bHotkeyOn = FALSE;
	}

	// удаление фильтра сообщений
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);

	pLoop->RemoveMessageFilter(this);

	// сохраняем настройки программы
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
// Обработка IDOK
//
LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка IDCANCEL
//
LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_bTimerOn)
	{
		ATLVERIFY(KillTimer(OFFMON_TIMER_COUNTDOWN));
		m_bTimerOn = FALSE;
		
		ShowWindow(SW_HIDE);

		// выход, если задан флаг выключение при запуске программы
		if (m_bOffOnRun)
		{
			PostMessage(WM_CLOSE);
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Выход из приложения
//
LRESULT CMainDlg::OnQuit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Закрытие диалога
//
VOID CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();

	::PostQuitMessage(nVal);
}


//////////////////////////////////////////////////////////////////////////
// Поиск всех возможных языков программы
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
	// неиспользуемые параметры
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpszType);
	UNREFERENCED_PARAMETER(lpszName);

	// проверка входных значений
	ATLASSERT(GetModuleHandle(NULL) == hModule);
	ATLASSERT(RT_DIALOG == lpszType);
	ATLASSERT(MAKEINTRESOURCE(IDD_OPTIONS) == lpszName);

	// получения указателя на массив языков
	CMapWordStr *pMap = (CMapWordStr*)lParam;

	// получение наименования языка по его идентификатору wIDLanguage
	std::vector<WCHAR> wcBuf(128);

	GetLocaleInfo(MAKELCID(wIDLanguage, 0), LOCALE_SNATIVELANGNAME, &wcBuf.front(), (INT)wcBuf.size());

	// перевод первой буквы в верхний регистр
	CharUpperBuff(&wcBuf.front(), 1);

	// добавление языка в массив
	pMap->insert(make_pair(wIDLanguage, &wcBuf.front()));

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Загружает строку по указанному идентификатору в соответствии с 
// установленным языком
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
// Загружает указанный ресурс
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
// Инициализация шаблона диалога
//
VOID CMainDlg::DoInitTemplate() 
{
	m_Template.Create((LPDLGTEMPLATE)GetResourceLang(RT_DIALOG, this->IDD));
}


//////////////////////////////////////////////////////////////////////////
// Инициализация контролов диалога
//
VOID CMainDlg::DoInitControls() 
{
	ATLASSERT(TRUE);
}


//////////////////////////////////////////////////////////////////////////
// Вывод меню в трее по нажатию левой кнопки мыши
//
VOID CMainDlg::OnNotifyIconLButtonUp(const POINT& rpt)
{
	OnNotifyIconRButtonUp(rpt);
}


//////////////////////////////////////////////////////////////////////////
// Вывод меню в трее по нажатию правой кнопки мыши
//
VOID CMainDlg::OnNotifyIconRButtonUp(const POINT& rpt)
{
	CMenu       menuPopup;
	menuPopup.LoadMenuIndirect(GetResourceLang(RT_MENU, IDR_MENU_POPUP));

	CMenuHandle menuVisible;
	menuVisible = menuPopup.GetSubMenu(0);

	// установка пункта ВЫКЛЮЧИТЬ жирным
	menuVisible.SetMenuDefaultItem(IDB_OFFMON);

	// вывод меню
	SetForegroundWindow(*this); // для того, чтобы меню исчезало при потере фокуса
	menuVisible.TrackPopupMenuEx(TPM_RIGHTALIGN | TPM_BOTTOMALIGN, rpt.x, rpt.y, *this); 
	PostMessage(WM_NULL);
}


//////////////////////////////////////////////////////////////////////////
// Закрытие диалога по WM_CLOSE
//
LRESULT CMainDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CloseDialog(0);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Вызов диалога настроек
//
LRESULT CMainDlg::OnOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDlgOptions dlg(this);

	dlg.DoModal(*this);

	// выход, если задан флаг выключение при запуске программы
	if (m_bOnlyOption || m_bOffOnRun)
	{
		PostMessage(WM_CLOSE);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Запуска таймера на выключение монитора
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
// Обработка таймера
//
LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// ИД таймера
	UINT_PTR uiTimerID = static_cast<UINT_PTR>(wParam);

	// таймер отсчета до выключения
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

			// выход, если задан флаг выключение при запуске программы
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

	// таймер для проверки мыши в левом верхнем углу
	// если он там находится заданное время, и не было активности, то выключаем монитор
	if ((OFFMON_TIMER_LEFTUP == uiTimerID) && (FALSE == GetIsLocked()))
	{
		// координаты курсора мыши
		CPoint ptCursor;
		GetCursorPos(&ptCursor);

		// получение последнего ввода данных
		LASTINPUTINFO lii;
		lii.cbSize = sizeof(lii);
		lii.dwTime = 0;

		GetLastInputInfo(&lii);

		if ((m_dwOffTime != lii.dwTime) && (0 == ptCursor.x) && (0 == ptCursor.y))
		{
			// выключение монитора
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
// Смена языка
//
LRESULT CMainDlg::OnChangeLanguage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// ИД языка
	WORD wLanguageId = static_cast<WORD>(wParam);

	// поиск языка
	CMapWordStr::iterator find = m_mapLanguages.find(wLanguageId);

	// выходим, если указанный язык не поддерживаем
	if (find == m_mapLanguages.end())
	{
		return static_cast<LRESULT>(0);
	}

	// смена языка
	m_wLanguageID = wLanguageId;

	// перезапуск программы
	CloseDialog(CHANGE_LANGUAGE);	

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// Установка горячей клавиши выключения
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
// устанавливает новую ширину линейки
//
VOID CMainDlg::SetHeight(FLOAT fHeight)
{
	m_fHeight = fHeight;

	ATLVERIFY(SetSize());
}


//////////////////////////////////////////////////////////////////////////
// Транформирование идентификаторов управляющих клавиш
// (из-за несовместимости GetHotkey and RegisterHotkey)
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
// Обработка нажатия горячей клавиши
//
LRESULT CMainDlg::OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_COMMAND, IDB_OFFMON);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка показа/сокрытия окна
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
// Изменение высоты окна
//
BOOL CMainDlg::SetSize()
{
	// получение размеров десктопа
	CRect rectDesktop;
	::GetClientRect(GetDesktopWindow(), &rectDesktop);

	// минимум 
	INT lMin = rectDesktop.Height() > rectDesktop.Width() ? rectDesktop.Width() : rectDesktop.Height();

	// вычисление толщины круга
	m_iWidthArc = static_cast<INT>(lMin * m_fHeight * DEFAULT_WIDTH_CIRCLE);

	// изменение размеров окна
	ResizeClient(static_cast<INT>(lMin * m_fHeight), static_cast<INT>(lMin * m_fHeight));

	// рендеринг изображения
	RenderImage();

	// выравнивание окна по центру
	CenterWindow();

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Установка прозрачности
//
VOID CMainDlg::SetTransparency(BYTE bTransparency)
{
	m_bTransparency = bTransparency;

	RenderImage();
}


//////////////////////////////////////////////////////////////////////////
// Возвращает TRUE, если программа запускается вместе с Windows
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
// Устанавливает режим автостарта при запуске Windows
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
// Установка выключения монитора после блокировки станции
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
// Рендеринг изображения
//
VOID CMainDlg::RenderImage()
{
	// получение оконных координат
	CRect rectClient;
	CRect rectWindow;

	GetClientRect(&rectClient);
	GetWindowRect(&rectWindow);

	// создание битмапа
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

	// инициализация AGG
	agg::rendering_buffer aggRendBuf;
	aggRendBuf.attach((PUCHAR)pvBuf, rectClient.Width(), rectClient.Height(), rectClient.Width() * 4);

	agg::pixfmt_bgra32                     aggPixF(aggRendBuf);
	agg::renderer_base<agg::pixfmt_bgra32> aggRenB(aggPixF);

	// очистка фона
	aggRenB.clear(agg::rgba8(0, 0, 0, 0));

	agg::renderer_scanline_aa_solid<agg::renderer_base<agg::pixfmt_bgra32>> aggRen(aggRenB);
	agg::rasterizer_scanline_aa<>                                           aggRas;
	agg::scanline_p8                                                        aggSl;

	// окружность
	double dRadius       = (rectClient.Width() < rectClient.Height()) ? rectClient.Width() / 2.L : rectClient.Height() / 2.L;
	double dEllipseWidth = static_cast<double>(m_iWidthArc);

	agg::ellipse                   aggEllipse(dRadius, dRadius, dRadius - dEllipseWidth / 2.L, dRadius - dEllipseWidth / 2.L);
	agg::conv_stroke<agg::ellipse> aggEStr(aggEllipse);

	aggEStr.width(dEllipseWidth);

	aggRas.add_path(aggEStr);
	aggRen.color(agg::rgba8(GetRValue(m_crColor), GetGValue(m_crColor), GetBValue(m_crColor), m_bTransparency));

	agg::render_scanlines(aggRas, aggSl, aggRen);

	// очистка растеризатора
	aggRas.reset();

	// инициализация шрифта
	font_engine_type     aggFontEng(dcMem);
	font_manager_type    aggFontMan(aggFontEng);
	conv_font_curve_type aggFontCurves(aggFontMan.path_adaptor());

	aggFontEng.height(300);
	aggFontEng.weight(FW_BOLD);
	aggFontEng.create_font("Arial", agg::glyph_ren_outline);

	// получение вектора цифры
	std::wostringstream ssNumber;
	ssNumber << m_iCurSecond;

	const agg::glyph_cache* aggGlyph = aggFontMan.glyph(static_cast<WCHAR>(*(ssNumber.str().c_str())));

	// вычисление коэффициента увеличения шрифта
	double dFontWidth  = aggGlyph->bounds.x2 - aggGlyph->bounds.x1;
	double dFontHeight = aggGlyph->bounds.y2 - aggGlyph->bounds.y1;
	double dScale      = rectClient.Height() / dFontHeight * DEFAULT_HEIGHT_NUMBER;

	// координаты проекции шрифта
	double dFontX = dRadius - (dFontWidth * dScale) / 2 - aggGlyph->bounds.x1 * dScale;
	double dFontY = dRadius - (dFontHeight * dScale) / 2 - aggGlyph->bounds.y1 * dScale;

	aggFontMan.add_kerning(&dFontX, &dFontY);
	aggFontMan.init_embedded_adaptors(aggGlyph, dFontX, dFontY, dScale);

	// растеризация
	aggRas.add_path(aggFontCurves);
	aggRen.color(agg::rgba8(GetRValue(m_crColor), GetGValue(m_crColor), GetBValue(m_crColor), m_bTransparency));

	agg::render_scanlines(aggRas, aggSl, aggRen);

	// обновление окна
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

	// удаление созданных ресурсов
	::SelectObject(dcMem, hBmpOld);
	::DeleteObject(hBmp);
	::DeleteDC(dcMem);
}


//////////////////////////////////////////////////////////////////////////
// Установка выключения монитора при запуске скринсейвера	
//
VOID CMainDlg::SetOffAfterSS(BOOL bOffAfterSS)
{
	m_bOffAfterSS = bOffAfterSS;
}


//////////////////////////////////////////////////////////////////////////
// Установка выключения монитора при наведении курсора в левый верхний угол
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
