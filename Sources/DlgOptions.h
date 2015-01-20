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

		// Конструктор по-умолчанию
		CDlgOptions(CMainDlg *pMainDlg);

		// Инициализация шаблона диалога
		VOID DoInitTemplate();

		// Инициализация контролов диалога
		VOID DoInitControls();

	private:

		CMainDlg*     m_pMainDlg;       // указатель на главный диалог
		CMapWordStr*  m_pMapLanguages;  // массив доступных языков
		CComboBox     m_cbLanguages;    // выпадающий список языков
		WORD          m_wLanguageID;    // ИД выбранного языка
		CHyperLink    m_hlEmail;        // гиперссылка на e-mail
		CHyperLink    m_hlWWW;          // гиперссылка на WWW
		CTrackBarCtrl m_tbDelay;        // ползунок установки задержки
		CStatic       m_sDelay;         // для вывода значения задержки 
		INT           m_iDelay;         // задержка в сек
		COLORREF      m_crColor;        // цвет
		COLORREF      m_crColorOld;     // цвет для восстановления
		CStatic       m_sColor;         // прямоугольник для вывода текущего цвета
		CRect         m_rColor;         // положение и размеры прямоугольника
		CHotKeyCtrl   m_hkOff;          // контрол для установки hotkey
		CTrackBarCtrl m_tbHeight;       // ползунок установки высоты окна
		CStatic       m_sHeight;        // для вывода значения высоты окна
		FLOAT         m_fHeight;        // высота окна
		CTrackBarCtrl m_tbTransparency; // ползунок изменения прозрачности
		CStatic       m_sTransparency;  // static для вывода процента прозрачности
		BYTE          m_bTransparency;  // значение прозрачности
		CButton       m_btnLock;        // кнопка установки блокировки рабочей станции
		CButton       m_btnLogoff;      // кнопка установки выхода из системы
		CButton       m_btnOffOnRun;    // кнопка установки выключения монитора при запуске
		CButton       m_btnAutostart;   // кнопки установки автозапуска
		CButton       m_btnOffAfterLock;// кнопка установки выключения после блокирования

	protected:

		// Инициализация диалога
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Обработка WM_DESTROY
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Обработка WM_PAINT
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Обработка IDOK
		LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Обработка IDCANCEL
		LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Выбор цвета
		LRESULT OnColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Закрытие диалога
		VOID CloseDialog(int nVal);

		// Смены языка
		LRESULT OnChangeLanguage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Восстановление измененных настроек
		VOID RestoreSettings();

		// Изменение задержки
		LRESULT OnChangeDelay(WORD wID, LPNMHDR pnmh, BOOL& bHandled);

		// Изменение высоты окна
		LRESULT OnChangeSize(WORD wID, LPNMHDR pnmh, BOOL& bHandled);

		// Изменение прозрачности
		LRESULT OnChangeTransparent(WORD wID, LPNMHDR pnmh, BOOL& bHandled);

		// Выключает или выключает checkbox'ы 
		LRESULT OnDisableCheckbox(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};
