#pragma once

#include "resource.h"

class CNotifyLock;

//	класс основного окна программы
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

		// Конструктор по-умолчанию
		CMainDlg(BOOL bOnlyOptions);

		// Деструктор по-умолчанию
		~CMainDlg();

		// Инициализация шаблона диалога
		VOID DoInitTemplate();

		// Инициализация контролов диалога
		VOID DoInitControls();

	private:

		BOOL         m_bOnlyOption;   // вывод только настроек программы
		CMapWordStr  m_mapLanguages;  // список языков программы(ID, язык)
		WORD         m_wLanguageID;   // ИД выбранного языка программы
		INT          m_wSecond;       // номер для вывода отсчета
		INT          m_iWidthArc;     // толщина круга
		INT          m_iSeconds;      // количество секунд до выключения
		INT          m_iCurSecond;    // текущая секунда
		BOOL         m_bTimerOn;      // TRUE, если таймер включен
		COLORREF     m_crColor;       // цвет текста
		BOOL         m_bLock;         // блокировать рабочую станцию
		BOOL         m_bLogoff;       // завершение работы
		FLOAT        m_fHeight;       // высота окна в % (от 0 до 1)
		DWORD        m_dwHotkeyOff;   // горячая клавиша для выключения
		BOOL         m_bHotkeyOn;     // TRUE, если хоткей работает
		BYTE         m_bTransparency; // прозрачность
		BOOL         m_bOffOnRun;     // выключение при старте программы
		CNotifyLock* m_pNotifyLock;   // класс подписки на событие блокировки рабочей станции
		BOOL         m_bOffAfterLock; // TRUE, если нужно выключить монитор после блокировки станции
		BOOL         m_bOffAfterSS;   // TRUE, если нужно выключить монитор при запуске скринсейвера
		BOOL         m_bOffLeftUp;    // TRUE, если выключать монитор при наведении мышью в левый верхний угол
		DWORD        m_dwOffTime;     // время выключения монитора
		BOOL         m_bIsLocked;     // TRUE, если станция заблокирована

	protected:

		// Фильтрация сообщений
		virtual BOOL PreTranslateMessage(MSG* pMsg);

		// Инициализация диалога
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Уничтожение диалога
		LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Обработка IDOK
		LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// Обработка IDCANCEL
		LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// Закрытие диалога
		VOID CloseDialog(int nVal);

		// Поиск всех возможных языков программы
		VOID SearchLanguages();

		static BOOL CALLBACK SearchLanguageCallback(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, 
			WORD wIDLanguage, LONG_PTR lParam);

		// Выход из приложения
		LRESULT OnQuit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// Закрытие диалога по WM_CLOSE
		LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Вызов диалога настроек
		LRESULT OnOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// Выключение монитора
		LRESULT OnOffMon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// Обработка таймера
		LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Транформирование идентификаторов управляющих клавиш
		UINT TransModifiers(WORD wMod);

		// Обработка нажатия горячей клавиши
		LRESULT OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Обработка показа/сокрытия окна
		LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Изменение высоты окна
		BOOL SetSize();

		// Рендеринг изображения
		VOID RenderImage();

		// Смена языка
		LRESULT OnChangeLanguage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	public:

		// Вывод меню по нажатию левой кнопки мыши на значке в трее
		VOID OnNotifyIconLButtonUp(const POINT& rpt);

		// Вывод меню по нажатию правой кнопки мыши на значке в трее
		VOID OnNotifyIconRButtonUp(const POINT& rpt);

		// Загружает указанный ресурс
		HGLOBAL GetResourceLang(LPWSTR lpResType, DWORD dwResID);

		// Загружает строку по указанному идентификатору в соответствии с установленным языком
		std::wstring GetStringLang(DWORD dwResID);

		// Возвращает список доступных языков
		inline CMapWordStr* GetMapLanguages(){return &m_mapLanguages;}

		// Возвращает ИД выбранного языка программы
		inline WORD GetLanguageID() {return m_wLanguageID;}

		// возвращает задержку
		inline INT GetDelay() {return m_iSeconds;}

		// устанавливает задержку
		inline VOID SetDelay(INT iSeconds) {m_iSeconds = iSeconds;}

		// возвращает цвет
		inline COLORREF GetColor() {return m_crColor;}

		// устанавливает цвет
		inline VOID SetColor(COLORREF crColor) {m_crColor = crColor; RenderImage();}

		// Установка горячей клавиши выключения
		inline DWORD GetHotkeyOff(){return m_dwHotkeyOff;}

		// Установка горячей клавиши выключения
		VOID SetHotkeyOff(DWORD dwHotkeyShow);

		// устанавливает новую ширину линейки
		VOID SetHeight(FLOAT fHeight);

		// возвращает коэффициент ширины линейки
		inline FLOAT GetHeight(){return m_fHeight;}

		// Установка прозрачности
		VOID SetTransparency(BYTE bTransparency);

		// Возвращает текущую прозрачность
		inline BYTE GetTransparency(){return m_bTransparency;}

		// Возвращает TRUE, если нужно блокировать станцию после выключения монитора
		inline BOOL GetLock() {return m_bLock;}

		// Устанавливает флаг блокировки станции после выключения монитора
		inline VOID SetLock(BOOL bLock) {m_bLock = bLock;}

		// Возвращается TRUE, если нужно выполнять выход из системы после выключения монитора
		inline BOOL GetLogoff() {return m_bLogoff;}

		// Устанавливает флаг выхода из системы после блокировки монитора
		inline VOID SetLogoff(BOOL bLogoff) {m_bLogoff = bLogoff;}

		// Возвращает флаг выключения монитора при запуске программы
		inline BOOL GetOffOnRun() {return m_bOffOnRun;}

		// Устанавливает флаг выключения монитора при запуске программы
		inline VOID SetOffOnRun(BOOL bOffOnRun) {m_bOffOnRun = bOffOnRun;}

		// Возвращает TRUE, если программа запускается вместе с Windows
		BOOL GetAutostart();

		// Устанавливает режим автостарта при запуске Windows
		VOID SetAutostart(BOOL bAutostart);

		// Возвращает TRUE, если монитор выключается после блокировки станции
		inline BOOL GetOffAfterLock() {return m_bOffAfterLock;}

		// Установка выключения монитора после блокировки станции
		VOID SetOffAfterLock(BOOL bOffAfterLock);

		// Возвращает TRUE, если монитор должен выключаться при запуске скринсейвера
		inline BOOL GetOffAfterSS() {return m_bOffAfterSS;}

		// Установка выключения монитора при запуске скринсейвера	
		VOID SetOffAfterSS(BOOL bOffAfterSS);

		// Возвращает TRUE, если монитор должен выключаться при наведении курсора в левый верхний угол
		inline BOOL GetOffLeftUp() {return m_bOffLeftUp;}

		// Установка выключения монитора при наведении курсора в левый верхний угол
		VOID SetOffLeftUp(BOOL bOffLeftUp);

		// Работа с флагом m_bIsLocked - станция заблокирована
		inline BOOL GetIsLocked() {return m_bIsLocked;}
		inline VOID SetIsLocked(BOOL bIsLocked) {m_bIsLocked = bIsLocked;}
};


