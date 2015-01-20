#pragma once

#include "resource.h"

class CNotifyLock;

//	����� ��������� ���� ���������
class CMainDlg : 
	public CIndirectDialogImpl<CMainDlg, CResDlgTemplate>,
	public CTrayIconImpl<CMainDlg>,
	public CSettingsImpl,
	public CMessageFilter
{
	public:
		enum { IDD = IDD_MAINDLG };

		BEGIN_MSG_MAP(CMainDlg)
			CHAIN_MSG_MAP(CTrayIconImpl<CMainDlg>)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_CLOSE, OnClose)
			MESSAGE_HANDLER(WM_QUERYENDSESSION, OnClose)
			MESSAGE_HANDLER(WM_TIMER, OnTimer)
			MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
			MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
			MESSAGE_HANDLER(WM_USER_CHANGE_LANGUAGE, OnChangeLanguage)
			COMMAND_ID_HANDLER(IDOK, OnOK)
			COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
			COMMAND_ID_HANDLER(IDB_QUIT, OnQuit)
			COMMAND_ID_HANDLER(IDB_OPTIONS, OnOptions)
			COMMAND_ID_HANDLER(IDB_OFFMON, OnOffMon)
			REFLECT_NOTIFICATIONS()
		END_MSG_MAP()

	public:

		// ����������� ��-���������
		CMainDlg(BOOL bOnlyOptions);

		// ���������� ��-���������
		~CMainDlg();

		// ������������� ������� �������
		VOID DoInitTemplate();

		// ������������� ��������� �������
		VOID DoInitControls();

	private:

		BOOL         m_bOnlyOption;   // ����� ������ �������� ���������
		CMapWordStr  m_mapLanguages;  // ������ ������ ���������(ID, ����)
		WORD         m_wLanguageID;   // �� ���������� ����� ���������
		INT          m_wSecond;       // ����� ��� ������ �������
		INT          m_iWidthArc;     // ������� �����
		INT          m_iSeconds;      // ���������� ������ �� ����������
		INT          m_iCurSecond;    // ������� �������
		BOOL         m_bTimerOn;      // TRUE, ���� ������ �������
		COLORREF     m_crColor;       // ���� ������
		BOOL         m_bLock;         // ����������� ������� �������
		BOOL         m_bLogoff;       // ���������� ������
		FLOAT        m_fHeight;       // ������ ���� � % (�� 0 �� 1)
		DWORD        m_dwHotkeyOff;   // ������� ������� ��� ����������
		BOOL         m_bHotkeyOn;     // TRUE, ���� ������ ��������
		BYTE         m_bTransparency; // ������������
		BOOL         m_bOffOnRun;     // ���������� ��� ������ ���������
		CNotifyLock* m_pNotifyLock;   // ����� �������� �� ������� ���������� ������� �������
		BOOL         m_bOffAfterLock; // TRUE, ���� ����� ��������� ������� ����� ���������� �������
		BOOL         m_bOffAfterSS;   // TRUE, ���� ����� ��������� ������� ��� ������� ������������
		BOOL         m_bOffLeftUp;    // TRUE, ���� ��������� ������� ��� ��������� ����� � ����� ������� ����
		DWORD        m_dwOffTime;     // ����� ���������� ��������
		BOOL         m_bIsLocked;     // TRUE, ���� ������� �������������

	protected:

		// ���������� ���������
		virtual BOOL PreTranslateMessage(MSG* pMsg);

		// ������������� �������
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// ����������� �������
		LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// ��������� IDOK
		LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// ��������� IDCANCEL
		LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// �������� �������
		VOID CloseDialog(int nVal);

		// ����� ���� ��������� ������ ���������
		VOID SearchLanguages();

		static BOOL CALLBACK SearchLanguageCallback(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, 
			WORD wIDLanguage, LONG_PTR lParam);

		// ����� �� ����������
		LRESULT OnQuit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// �������� ������� �� WM_CLOSE
		LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// ����� ������� ��������
		LRESULT OnOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// ���������� ��������
		LRESULT OnOffMon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// ��������� �������
		LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// ���������������� ��������������� ����������� ������
		UINT TransModifiers(WORD wMod);

		// ��������� ������� ������� �������
		LRESULT OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// ��������� ������/�������� ����
		LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// ��������� ������ ����
		BOOL SetSize();

		// ��������� �����������
		VOID RenderImage();

		// ����� �����
		LRESULT OnChangeLanguage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	public:

		// ����� ���� �� ������� ����� ������ ���� �� ������ � ����
		VOID OnNotifyIconLButtonUp(const POINT& rpt);

		// ����� ���� �� ������� ������ ������ ���� �� ������ � ����
		VOID OnNotifyIconRButtonUp(const POINT& rpt);

		// ��������� ��������� ������
		HGLOBAL GetResourceLang(LPWSTR lpResType, DWORD dwResID);

		// ��������� ������ �� ���������� �������������� � ������������ � ������������� ������
		std::wstring GetStringLang(DWORD dwResID);

		// ���������� ������ ��������� ������
		inline CMapWordStr* GetMapLanguages(){return &m_mapLanguages;}

		// ���������� �� ���������� ����� ���������
		inline WORD GetLanguageID() {return m_wLanguageID;}

		// ���������� ��������
		inline INT GetDelay() {return m_iSeconds;}

		// ������������� ��������
		inline VOID SetDelay(INT iSeconds) {m_iSeconds = iSeconds;}

		// ���������� ����
		inline COLORREF GetColor() {return m_crColor;}

		// ������������� ����
		inline VOID SetColor(COLORREF crColor) {m_crColor = crColor; RenderImage();}

		// ��������� ������� ������� ����������
		inline DWORD GetHotkeyOff(){return m_dwHotkeyOff;}

		// ��������� ������� ������� ����������
		VOID SetHotkeyOff(DWORD dwHotkeyShow);

		// ������������� ����� ������ �������
		VOID SetHeight(FLOAT fHeight);

		// ���������� ����������� ������ �������
		inline FLOAT GetHeight(){return m_fHeight;}

		// ��������� ������������
		VOID SetTransparency(BYTE bTransparency);

		// ���������� ������� ������������
		inline BYTE GetTransparency(){return m_bTransparency;}

		// ���������� TRUE, ���� ����� ����������� ������� ����� ���������� ��������
		inline BOOL GetLock() {return m_bLock;}

		// ������������� ���� ���������� ������� ����� ���������� ��������
		inline VOID SetLock(BOOL bLock) {m_bLock = bLock;}

		// ������������ TRUE, ���� ����� ��������� ����� �� ������� ����� ���������� ��������
		inline BOOL GetLogoff() {return m_bLogoff;}

		// ������������� ���� ������ �� ������� ����� ���������� ��������
		inline VOID SetLogoff(BOOL bLogoff) {m_bLogoff = bLogoff;}

		// ���������� ���� ���������� �������� ��� ������� ���������
		inline BOOL GetOffOnRun() {return m_bOffOnRun;}

		// ������������� ���� ���������� �������� ��� ������� ���������
		inline VOID SetOffOnRun(BOOL bOffOnRun) {m_bOffOnRun = bOffOnRun;}

		// ���������� TRUE, ���� ��������� ����������� ������ � Windows
		BOOL GetAutostart();

		// ������������� ����� ���������� ��� ������� Windows
		VOID SetAutostart(BOOL bAutostart);

		// ���������� TRUE, ���� ������� ����������� ����� ���������� �������
		inline BOOL GetOffAfterLock() {return m_bOffAfterLock;}

		// ��������� ���������� �������� ����� ���������� �������
		VOID SetOffAfterLock(BOOL bOffAfterLock);

		// ���������� TRUE, ���� ������� ������ ����������� ��� ������� ������������
		inline BOOL GetOffAfterSS() {return m_bOffAfterSS;}

		// ��������� ���������� �������� ��� ������� ������������	
		VOID SetOffAfterSS(BOOL bOffAfterSS);

		// ���������� TRUE, ���� ������� ������ ����������� ��� ��������� ������� � ����� ������� ����
		inline BOOL GetOffLeftUp() {return m_bOffLeftUp;}

		// ��������� ���������� �������� ��� ��������� ������� � ����� ������� ����
		VOID SetOffLeftUp(BOOL bOffLeftUp);

		// ������ � ������ m_bIsLocked - ������� �������������
		inline BOOL GetIsLocked() {return m_bIsLocked;}
		inline VOID SetIsLocked(BOOL bIsLocked) {m_bIsLocked = bIsLocked;}
};


