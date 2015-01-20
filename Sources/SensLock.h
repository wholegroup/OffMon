// Класс для подписки на событие DisplayLock

#pragma once

#include <sensevts.h>
#include <eventsys.h>
#include <Sens.h>
#include "DlgMain.h"

class CNotifyLock
	: private ISensLogon
{
	public:
		CNotifyLock(CMainDlg* pMainDlg)
			:ref(1), m_pMainDlg(pMainDlg)
		{}

	private:
		
		LONG      ref;        // счетчик ссылок на класс
		CMainDlg* m_pMainDlg; // указатель на главный диалог

		STDMETHOD_(ULONG, AddRef)()
		{
			return InterlockedIncrement(&ref);
		}

		STDMETHOD_(ULONG, Release)()
		{
			int tmp; 
			tmp = InterlockedDecrement(&ref); 
			return tmp; 
		}

		STDMETHOD(GetTypeInfoCount)(UINT*) 
		{ 
			return E_NOTIMPL; 
		}

		STDMETHOD(GetTypeInfo)(UINT, LCID, ITypeInfo**)
		{ 
			return E_NOTIMPL; 
		}

		STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, UINT, LCID, DISPID*)
		{ 
			return E_NOTIMPL; 
		}

		STDMETHOD(Invoke)(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*)
		{ 
			return E_NOTIMPL; 
		}

		STDMETHOD(QueryInterface)(REFIID riid, VOID** ppvObject)
		{
			*ppvObject = NULL;
	
			if ((IID_IUnknown == riid)  || (IID_IDispatch == riid) || (IID_ISensLogon == riid))
			{
				*ppvObject = this;
			}

			return (*ppvObject != NULL) ? S_OK : E_NOINTERFACE;
		}

		STDMETHOD(Logon)(BSTR bstrUserName)
		{
			UNREFERENCED_PARAMETER(bstrUserName);

			return S_OK;
		}

		STDMETHOD(Logoff)(BSTR bstrUserName)
		{
			UNREFERENCED_PARAMETER(bstrUserName);

			return S_OK;
		}

		STDMETHOD(StartShell)(BSTR bstrUserName)
		{
			UNREFERENCED_PARAMETER(bstrUserName);

			return S_OK;
		}

		STDMETHOD(DisplayLock)(BSTR bstrUserName)
		{
			UNREFERENCED_PARAMETER(bstrUserName);

			// установка флага - станция заблокирована
			m_pMainDlg->SetIsLocked(TRUE);

			// выключение монитора при блокировке станции
			// ожидаем в течении 5 секунд, отсутствие активности более 1 секунды, иначе монитор не выключаем
			Sleep(1000);

			LASTINPUTINFO liInfo;
			DWORD         dwTick;

			for (unsigned i = 0; i < 4; i++)
			{
				Sleep(1000);

				liInfo.cbSize = sizeof(liInfo);
				GetLastInputInfo(&liInfo);

				dwTick = GetTickCount();

				if ((dwTick - liInfo.dwTime) >= 1000)
				{
					::PostMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);

					return S_OK;
				}
			}

			return S_OK;
		}

		STDMETHOD(DisplayUnlock)(BSTR bstrUserName)
		{
			UNREFERENCED_PARAMETER(bstrUserName);

			// установка флага - станция разблокирована
			m_pMainDlg->SetIsLocked(FALSE);

			return S_OK;
		}

		STDMETHOD(StartScreenSaver)(BSTR bstrUserName)
		{
			UNREFERENCED_PARAMETER(bstrUserName);

			if (TRUE == m_pMainDlg->GetOffAfterSS())
			{
				::PostMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
			}

			return S_OK;
		}

		STDMETHOD(StopScreenSaver)(BSTR bstrUserName)
		{
			UNREFERENCED_PARAMETER(bstrUserName);

			return S_OK;
		}

	public:

		// Подписка на событие
		BOOL Init()
		{
			HRESULT                     hr;
			CComPtr<IEventSubscription> ess;

			hr = ess.CoCreateInstance(PROGID_EventSubscription);
			if (S_OK != hr)
			{
				_RPT_API_ERROR(ess.CoCreateInstance);

				return FALSE;
			}

			hr = ess->put_SubscriptionID(OLESTR("{E3604574-F092-419a-854A-87F9C16D9BCD}"));
			if (S_OK != hr)
			{
				_RPT_API_ERROR(put_SubscriptionID);

				return FALSE;
			}

			hr = ess->put_SubscriptionName(CComBSTR(PROGRAM_NAME));
			if (S_OK != hr)
			{
				_RPT_API_ERROR(put_SubscriptionName);

				return FALSE;
			}
			
			hr = ess->put_SubscriberInterface(this);
			if (S_OK != hr)
			{
				_RPT_API_ERROR(put_SubscriberInterface);

				return FALSE;
			}
			
			hr = ess->put_EventClassID(OLESTR("{D5978630-5B9F-11D1-8DD2-00AA004ABD5E}")); // SENSGUID_EVENTCLASS_LOGON
			if (S_OK != hr)
			{
				_RPT_API_ERROR(put_EventClassID);

				return FALSE;
			}

			CComPtr<IEventSystem> es;

			hr = es.CoCreateInstance(PROGID_EventSystem);
			if (S_OK != hr)
			{
				_RPT_API_ERROR(es.CoCreateInstance);

				return FALSE;
			}

			hr = es->Store(PROGID_EventSubscription, ess);
			if (S_OK != hr)
			{
				_RPT_API_ERROR(es->Store);

				return FALSE;
			}

			return TRUE;
		}

		BOOL Deinit()
		{
			HRESULT               hr;
			CComPtr<IEventSystem> es;
			int                   iError;

			hr = es.CoCreateInstance(PROGID_EventSystem);
			if (S_OK != hr)
			{
				_RPT_API_ERROR(es.CoCreateInstance);

				return FALSE;
			}

			es->Remove(PROGID_EventSubscription, L"SubscriptionID == {E3604574-F092-419a-854A-87F9C16D9BCD}", &iError);

			return TRUE;
		}
};
