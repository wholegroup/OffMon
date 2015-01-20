#pragma once 

#include "resource.h"

class CDlgOptions :
	public CIndirectDialogImpl<CDlgOptions, CResDlgTemplate>
{
	public:

		enum {IDD = IDD_OPTIONS};

		BEGIN_MSG_MAP(CDlgOptions)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			COMMAND_ID_HANDLER(IDOK, OnOK)
			COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
			COMMAND_ID_HANDLER(IDC_BUTTON_COLOR, OnColor)
			COMMAND_ID_HANDLER(IDC_BUTTON_LOCK, OnDisableCheckbox)
			COMMAND_ID_HANDLER(IDC_BUTTON_LOGOFF, OnDisableCheckbox)
			COMMAND_ID_HANDLER(IDC_BUTTON_OFFONRUN, OnDisableCheckbox)
			COMMAND_ID_HANDLER(IDC_BUTTON_AUTOSTART, OnDisableCheckbox)
			COMMAND_HANDLER(IDC_COMBO_LANGUAGES, CBN_SELCHANGE, OnChangeLanguage)
			NOTIFY_ID_HANDLER(IDC_SLIDER_DELAY, OnChangeDelay)
			NOTIFY_ID_HANDLER(IDC_SLIDER_SIZE, OnChangeSize)
			NOTIFY_ID_HANDLER(IDC_SLIDER_TRANSPARENCY, OnChangeTransparent)
		END_MSG_MAP()

		// ����������� ��-���������
		CDlgOptions(CMainDlg *pMainDlg);

		// ������������� ������� �������
		VOID DoInitTemplate();

		// ������������� ��������� �������
		VOID DoInitControls();

	private:

		CMainDlg*     m_pMainDlg;       // ��������� �� ������� ������
		CMapWordStr*  m_pMapLanguages;  // ������ ��������� ������
		CComboBox     m_cbLanguages;    // ���������� ������ ������
		WORD          m_wLanguageID;    // �� ���������� �����
		CHyperLink    m_hlEmail;        // ����������� �� e-mail
		CHyperLink    m_hlWWW;          // ����������� �� WWW
		CTrackBarCtrl m_tbDelay;        // �������� ��������� ��������
		CStatic       m_sDelay;         // ��� ������ �������� �������� 
		INT           m_iDelay;         // �������� � ���
		COLORREF      m_crColor;        // ����
		COLORREF      m_crColorOld;     // ���� ��� ��������������
		CStatic       m_sColor;         // ������������� ��� ������ �������� �����
		CRect         m_rColor;         // ��������� � ������� ��������������
		CHotKeyCtrl   m_hkOff;          // ������� ��� ��������� hotkey
		CTrackBarCtrl m_tbHeight;       // �������� ��������� ������ ����
		CStatic       m_sHeight;        // ��� ������ �������� ������ ����
		FLOAT         m_fHeight;        // ������ ����
		CTrackBarCtrl m_tbTransparency; // �������� ��������� ������������
		CStatic       m_sTransparency;  // static ��� ������ �������� ������������
		BYTE          m_bTransparency;  // �������� ������������
		CButton       m_btnLock;        // ������ ��������� ���������� ������� �������
		CButton       m_btnLogoff;      // ������ ��������� ������ �� �������
		CButton       m_btnOffOnRun;    // ������ ��������� ���������� �������� ��� �������
		CButton       m_btnAutostart;   // ������ ��������� �����������
		CButton       m_btnOffAfterLock;// ������ ��������� ���������� ����� ������������

	protected:

		// ������������� �������
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ��������� WM_DESTROY
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ��������� WM_PAINT
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ��������� IDOK
		LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ��������� IDCANCEL
		LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ����� �����
		LRESULT OnColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// �������� �������
		VOID CloseDialog(int nVal);

		// ����� �����
		LRESULT OnChangeLanguage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// �������������� ���������� ��������
		VOID RestoreSettings();

		// ��������� ��������
		LRESULT OnChangeDelay(WORD wID, LPNMHDR pnmh, BOOL& bHandled);

		// ��������� ������ ����
		LRESULT OnChangeSize(WORD wID, LPNMHDR pnmh, BOOL& bHandled);

		// ��������� ������������
		LRESULT OnChangeTransparent(WORD wID, LPNMHDR pnmh, BOOL& bHandled);

		// ��������� ��� ��������� checkbox'� 
		LRESULT OnDisableCheckbox(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};
