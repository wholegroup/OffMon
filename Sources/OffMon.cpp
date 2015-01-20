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

CAppModule _Module;

int Run(LPTSTR lpstrCmdLine = NULL, BOOL bSettings = FALSE)
{
	// ������ ��������� ������ �� ������� ����� ������� ������ ��������
	std::wstring wsCmdLine    = lpstrCmdLine;
	BOOL         bOnlyOptions = FALSE;

	if (L"-o" == wsCmdLine)
	{
		bOnlyOptions = TRUE;
	}

	CMessageLoop theLoop;

	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain(bOnlyOptions);

	if (NULL == dlgMain.Create(NULL))
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));

		return 0;
	}

	if (!bOnlyOptions && bSettings)
	{
		dlgMain.PostMessage(WM_COMMAND, IDB_OPTIONS);
	}

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

	return nRet;
}

INT WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	// ��������� ������ � ������� ������
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// �������� ���������� ������� �������������� ������ ��������� � �������� ������������ ������
	HANDLE  hMutexRun   = NULL;
	wstring wsMutexName = L"Local\\{7E9CA7A6-70EB-43bb-8961-A04C14E3FD1B}";

	// ���� ������ ����, ������ ����� ��������� ��� ��������
	hMutexRun = OpenMutex(0, FALSE, wsMutexName.c_str());
	if (NULL != hMutexRun)
	{
		ATLVERIFY(CloseHandle(hMutexRun));
		_RPT_API_ERROR(OpenMutex);
		return 0;
	}

	if (ERROR_ACCESS_DENIED == GetLastError())
	{
		return 0;
	}

	// ������� ����� ������
	hMutexRun = CreateMutex(NULL, TRUE, wsMutexName.c_str());
	if (NULL == hMutexRun)
	{
		_RPT_API_ERROR(CreateMutex);
		return 0;
	}

	// �������������
	::DefWindowProc(NULL, 0, 0, 0L);

	// ������������� ���������
	AtlInitCommonControls(ICC_BAR_CLASSES);

	// ������������� COM
	HRESULT hRes = S_OK;

	hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	// ������������� ������������ ������
	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	// ������ �������� �����
	INT iRet = 0;

	do 
	{
		iRet = Run(lpstrCmdLine, (CHANGE_LANGUAGE == iRet) ? TRUE : FALSE);
	} while (CHANGE_LANGUAGE == iRet);

	// ���������� �������� ������
	_Module.Term();

	// ������� �������� COM 
	::CoUninitialize();

	// �������� �������
	ATLVERIFY(CloseHandle(hMutexRun));

	return iRet;
}