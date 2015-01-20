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
#include "DlgMain.h"
#include "DlgOptions.h"

//////////////////////////////////////////////////////////////////////////
// Конструктор по-умолчанию
//
CDlgOptions::CDlgOptions(CMainDlg *pMainDlg)
{
	ATLASSERT(NULL != pMainDlg);
	m_pMainDlg = pMainDlg;
}


//////////////////////////////////////////////////////////////////////////
// Инициализация диалога
//
LRESULT CDlgOptions::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// выравниваем по центру
	CenterWindow();

	// вставка в заголовок диалога версии программы
	std::vector<WCHAR> wcTitle(GetWindowTextLength() + 1);

	::GetWindowText(m_hWnd, &wcTitle.front(), static_cast<int>(wcTitle.size()));

	std::wstring wsTitle = &wcTitle.front();

	wsTitle += L" v";
	wsTitle += PROGRAM_VERSION;
	SetWindowText(wsTitle.c_str());

	// заполнение списка языков
	m_cbLanguages = static_cast<CComboBox>(GetDlgItem(IDC_COMBO_LANGUAGES));

	m_wLanguageID   = m_pMainDlg->GetLanguageID();
	m_pMapLanguages = m_pMainDlg->GetMapLanguages();
	ATLASSERT(NULL != m_pMapLanguages);

	INT iSelect = 0;
	INT i       = 0;

	for (CMapWordStr::const_iterator lang = m_pMapLanguages->begin(); lang != m_pMapLanguages->end(); lang++, i++)
	{
		m_cbLanguages.AddString(lang->second.c_str());

		if (lang->first == m_wLanguageID)
		{
			iSelect = i;
		}
	}

	m_cbLanguages.SetCurSel(iSelect);

	// установка ползунка задержки
	m_iDelay = m_pMainDlg->GetDelay();

	m_tbDelay = static_cast<CTrackBarCtrl>(GetDlgItem(IDC_SLIDER_DELAY));
	m_tbDelay.SetRange(DEFAULT_SECONDS_MIN, DEFAULT_SECONDS_MAX);
	m_tbDelay.SetPos(m_iDelay);

	std::wostringstream streamDelay;
	streamDelay << m_iDelay << L" " << m_pMainDlg->GetStringLang(IDS_SECOND);

	m_sDelay = static_cast<CStatic>(GetDlgItem(IDC_STATIC_DELAY_SECONDS));
	m_sDelay.SetWindowText(streamDelay.str().c_str());

	// установка ползунка прозрачности
	m_bTransparency = m_pMainDlg->GetTransparency();

	m_tbTransparency = static_cast<CTrackBarCtrl>(GetDlgItem(IDC_SLIDER_TRANSPARENCY));
	m_tbTransparency.SetRange(DEFAULT_TRANSPARENCY_MIN, 255);
	m_tbTransparency.SetPos(m_bTransparency);

	std::wostringstream streamTransparency;
	streamTransparency << static_cast<INT>((100. - m_bTransparency * 100. / m_tbTransparency.GetRangeMax())) << "%";

	m_sTransparency = static_cast<CStatic>(GetDlgItem(IDC_STATIC_PERCENT_TRANSPARENCY));
	m_sTransparency.SetWindowText(streamTransparency.str().c_str());

	// установка ползунка высоты окна
	m_fHeight = m_pMainDlg->GetHeight();

	m_tbHeight = static_cast<CTrackBarCtrl>(GetDlgItem(IDC_SLIDER_SIZE));
	m_tbHeight.SetRange(static_cast<INT>(DEFAULT_HEIGHT_MIN * 1000), static_cast<INT>(DEFAULT_HEIGHT_MAX * 1000));
	m_tbHeight.SetPos(static_cast<int>(m_fHeight * 1000));

	std::wostringstream streamHeight;
	streamHeight << (m_tbHeight.GetPos() * 100 / m_tbHeight.GetRangeMax()) << "%";

	m_sHeight = static_cast<CStatic>(GetDlgItem(IDC_STATIC_PERCENT_SIZE));
	m_sHeight.SetWindowText(streamHeight.str().c_str());

	// вывод цвета
	m_crColorOld = m_crColor = m_pMainDlg->GetColor();
	m_sColor = static_cast<CStatic>(GetDlgItem(IDC_STATIC_VIEWCOLOR));
	m_sColor.GetWindowRect(m_rColor);
	ScreenToClient(m_rColor);

	// установка горячей клавиши
	DWORD dwHotkeyShow = m_pMainDlg->GetHotkeyOff();

	m_hkOff = static_cast<CHotKeyCtrl>(GetDlgItem(IDC_HOTKEY_OFF));
	m_hkOff.SetHotKey(LOBYTE(LOWORD(dwHotkeyShow)), HIBYTE(LOWORD(dwHotkeyShow)));

	// установка кнопки блокировки рабочей станции
	m_btnLock = static_cast<CButton>(GetDlgItem(IDC_BUTTON_LOCK));
	m_btnLock.SetCheck(m_pMainDlg->GetLock());

	// установка кнопки выхода из системы
	m_btnLogoff = static_cast<CButton>(GetDlgItem(IDC_BUTTON_LOGOFF));
	m_btnLogoff.SetCheck(m_pMainDlg->GetLogoff());

	// установки кнопки выключения монитора при запуске
	m_btnOffOnRun = static_cast<CButton>(GetDlgItem(IDC_BUTTON_OFFONRUN));
	m_btnOffOnRun.SetCheck(m_pMainDlg->GetOffOnRun());

	// установки кнопки автозапуска программы при старте Windows
	m_btnAutostart = static_cast<CButton>(GetDlgItem(IDC_BUTTON_AUTOSTART));
	m_btnAutostart.SetCheck(m_pMainDlg->GetAutostart());

	PostMessage(WM_COMMAND, IDC_BUTTON_AUTOSTART);

	// установка кнопки выключения монитора после блокирования станции
	m_btnOffAfterLock = static_cast<CButton>(GetDlgItem(IDC_BUTTON_OFFAFTERLOCK));
	m_btnOffAfterLock.SetCheck(m_pMainDlg->GetOffAfterLock());
	
	// установка ссылок
	m_hlEmail.SetHyperLink(m_pMainDlg->GetStringLang(IDS_EMAIL).c_str());
	m_hlEmail.SubclassWindow(GetDlgItem(IDC_HYPERLINK_EMAIL));

	m_hlWWW.SetHyperLink(m_pMainDlg->GetStringLang(IDS_WWW).c_str());
	m_hlWWW.SubclassWindow(GetDlgItem(IDC_HYPERLINK_WWW));

	// изменение шрифтов
	CLogFont lfBold(static_cast<CStatic>(GetDlgItem(IDC_STATIC_WWW)).GetFont());
	lfBold.SetBold();

	static_cast<CStatic>(GetDlgItem(IDC_STATIC_WWW)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_EMAIL)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_LANG)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_DELAY)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_COLOR)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_SIZE)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_HOTKEY)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_TRANSPARENCY)).SetFont(lfBold.CreateFontIndirect());

	// вывод окна
	m_pMainDlg->ShowWindow(SW_SHOW);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка WM_DESTROY
//
LRESULT CDlgOptions::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка IDOK
//
LRESULT CDlgOptions::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// установка задержки
	m_pMainDlg->SetDelay(m_tbDelay.GetPos());

	// установка горячей клавиши
	m_pMainDlg->SetHotkeyOff(m_hkOff.GetHotKey());

	// установка блокировки рабочей станции
	m_pMainDlg->SetLock(m_btnLock.GetCheck());

	// установка выхода из системы
	m_pMainDlg->SetLogoff(m_btnLogoff.GetCheck());

	// установка флага выключения монитора при запуске программы
	m_pMainDlg->SetOffOnRun(m_btnOffOnRun.GetCheck());

	// установка флага автозапуска с Windows
	m_pMainDlg->SetAutostart(m_btnAutostart.GetCheck());

	// установка выключения монитора после блокировки станции
	m_pMainDlg->SetOffAfterLock(m_btnOffAfterLock.GetCheck());

	CloseDialog(IDOK);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка IDCANCEL
//
LRESULT CDlgOptions::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	RestoreSettings();

	CloseDialog(IDCANCEL);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Инициализация шаблона диалога
//
VOID CDlgOptions::DoInitTemplate() 
{
	m_Template.Create((LPDLGTEMPLATE)m_pMainDlg->GetResourceLang(RT_DIALOG, this->IDD));
}


//////////////////////////////////////////////////////////////////////////
// Инициализация контролов диалога
//
VOID CDlgOptions::DoInitControls() 
{
	ATLASSERT(TRUE);
}


//////////////////////////////////////////////////////////////////////////
// Закрытие диалога
//
VOID CDlgOptions::CloseDialog(int nVal)
{
	m_pMainDlg->ShowWindow(SW_HIDE);

	EndDialog(nVal);
}


//////////////////////////////////////////////////////////////////////////
// Смена языка
//
LRESULT CDlgOptions::OnChangeLanguage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	RestoreSettings();

	ATLASSERT(CBN_SELCHANGE == wNotifyCode);

	INT iSelect = m_cbLanguages.GetCurSel();
	ATLASSERT(iSelect < static_cast<INT>(m_pMapLanguages->size()));

	CMapWordStr::const_iterator lang = m_pMapLanguages->begin();
	advance(lang, iSelect);

	m_pMainDlg->PostMessage(WM_USER_CHANGE_LANGUAGE, static_cast<WPARAM>(lang->first));

	CloseDialog(0);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Восстановление измененных настроек
//
VOID CDlgOptions::RestoreSettings()
{
	m_pMainDlg->SetColor(m_crColorOld);
	m_pMainDlg->SetHeight(m_fHeight);
	m_pMainDlg->SetTransparency(m_bTransparency);
}


//////////////////////////////////////////////////////////////////////////
// Выбор цвета
//
LRESULT CDlgOptions::OnColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CColorDialog dlgColor(m_crColor, CC_FULLOPEN | CC_RGBINIT);

	if (IDOK == dlgColor.DoModal())
	{
		m_crColor = dlgColor.GetColor();
		m_pMainDlg->SetColor(m_crColor);
		m_pMainDlg->Invalidate(TRUE);
		
		InvalidateRect(m_rColor);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Изменение задержки
//
LRESULT CDlgOptions::OnChangeDelay(WORD wID, LPNMHDR pnmh, BOOL& bHandled)
{
	ATLASSERT(IDC_SLIDER_DELAY == wID);

	std::wostringstream streamDelay;
	streamDelay << m_tbDelay.GetPos() << L" " << m_pMainDlg->GetStringLang(IDS_SECOND);

	m_sDelay.SetWindowText(streamDelay.str().c_str());

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка WM_PAINT
//
LRESULT CDlgOptions::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC dc(*this);

	dc.FillSolidRect(m_rColor, m_crColor);
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Изменение размера окна
//
LRESULT CDlgOptions::OnChangeSize(WORD wID, LPNMHDR pnmh, BOOL& bHandled)
{
	ATLASSERT(IDC_SLIDER_SIZE == wID);

	if (0 == m_tbHeight.GetPos())
	{
		m_pMainDlg->SetHeight(0);
	}
	else
	{
		m_pMainDlg->SetHeight(m_tbHeight.GetPos() / static_cast<FLOAT>(m_tbHeight.GetRangeMax()));
	}

	std::wostringstream streamHeight;
	streamHeight << (m_tbHeight.GetPos() * 100 / m_tbHeight.GetRangeMax()) << "%";

	m_sHeight.SetWindowText(streamHeight.str().c_str());

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Изменение прозрачности
//
LRESULT CDlgOptions::OnChangeTransparent(WORD wID, LPNMHDR pnmh, BOOL& bHandled)
{
	ATLASSERT(IDC_SLIDER_TRANSPARENCY == wID);

	m_pMainDlg->SetTransparency(m_tbTransparency.GetPos());

	std::wostringstream streamTransparency;
	streamTransparency << static_cast<INT>((100. - m_tbTransparency.GetPos() * 
		100. / m_tbTransparency.GetRangeMax())) << L"%";

	m_sTransparency.SetWindowText(streamTransparency.str().c_str());

	return 0;
}	


//////////////////////////////////////////////////////////////////////////
// Выключает или выключает checkbox'ы 
//
LRESULT CDlgOptions::OnDisableCheckbox(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	//
	if (m_btnLock.GetCheck())
	{
		m_btnLogoff.EnableWindow(FALSE);
	}
	else
	{
		m_btnLogoff.EnableWindow(TRUE);
	}

	//
	if (m_btnLogoff.GetCheck())
	{
		m_btnLock.EnableWindow(FALSE);
	}
	else
	{
		m_btnLock.EnableWindow(TRUE);
	}

	//
	if (m_btnOffOnRun.GetCheck())
	{
		m_btnAutostart.EnableWindow(FALSE);
	}
	else
	{
		m_btnAutostart.EnableWindow(TRUE);
	}

	//
	if (m_btnAutostart.GetCheck())
	{
		m_btnOffOnRun.EnableWindow(FALSE);
	}
	else
	{
		m_btnOffOnRun.EnableWindow(TRUE);
	}

	return 0;
}
