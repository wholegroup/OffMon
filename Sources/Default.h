#pragma once

// ��������
#define COMPANY_NAME L"www.wholegroup.com"

// �������� ���������
#define PROGRAM_NAME L"OffMon"

// ���� ��-���������
#define DEFAULT_LANGUAGE MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL)

// ��� ������ ��� ����� �����
#define CHANGE_LANGUAGE 123

// ��������� ��� ����� ����� ���������
#define WM_USER_CHANGE_LANGUAGE WM_USER + 0x801

// ���� ����������� ����
#define RGB_TRANSPARENT RGB(89, 117, 26)

// ������ ����� ������������ ������ ����
#define DEFAULT_HEIGHT_NUMBER 0.67

// ������� ����� ������������ ������ ����
#define DEFAULT_WIDTH_CIRCLE 0.08

// ������ ���� ��-���������
#define DEFAULT_HEIGHT     1
#define DEFAULT_HEIGHT_MIN 0.1
#define DEFAULT_HEIGHT_MAX 1

// ���������� ������ �� ����������
#define DEFAULT_SECONDS     3
#define DEFAULT_SECONDS_MIN 1
#define DEFAULT_SECONDS_MAX 9

// ���� ��-���������
#define DEFAULT_COLOR RGB(57, 128, 124)

// ������������ ��-���������
#define DEFAULT_TRANSPARENCY     100
#define DEFAULT_TRANSPARENCY_MIN 25
#define DEFAULT_TRANSPARENCY_MAX 255

// ����� ��������� ��� ���������
#define DEFAULT_NUMBER_SETTINGS 4

// ������� ������� � �� ��� ������� ����������
#define DEFAULT_TIMER_COUNTDOWN 1000

// ������� ������� � �� ��� �������� ������� ������� ���� � ����� ������� ����
#define DEFAULT_TIMER_LEFTUP 1000

// ����� ����������� ��� ���������� ��������, ����� ���� � ����� ������� ����
#define DEFAULT_TIME_LEFTUP 3000

// ���� ���������� ������� ������� ��-���������
#define DEFAULT_LOCK FALSE

// ���� ������ �� ������� ��-���������
#define DEFAULT_LOGOFF FALSE

// ���� ��-��������� ���������� �������� ��� ������� ���������
#define DEFAULT_OFFONRUN FALSE

// ���� ��-��������� ���������� �������� ����� ����� ���������� �������
#define DEFAULT_OFFAFTERLOCK TRUE

// ���� ��-��������� ���������� �������� ����� ����� ������� ������������
#define DEFAULT_OFFAFTERSS FALSE

// ���� ��-��������� ���������� �������� ��� ��������� ���� � ����� ������� ����
#define DEFAULT_OFFLEFTUP FALSE

// �������������� ��������
enum OFFMON_TIMERS
{
	OFFMON_TIMER_COUNTDOWN = 0, // ������ ��� ������� ����������
	OFFMON_TIMER_LEFTUP         // ������ ��� ������������ ���� � ����� ������� ����
};

// ������� ��� �������
#ifdef _DEBUG
#define _RPT_API_TRACE(name)                                                     \
do                                                                               \
{                                                                                \
	unsigned __err = GetLastError();                                              \
	WCHAR    wcMsg[512];                                                          \
	                                                                              \
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, __err, 0, wcMsg, 512, NULL); \
	                                                                              \
	_RPTFW3(_CRT_WARN, L"%S() failed; ERR=%d %s\n", #name, __err, wcMsg);         \
	                                                                              \
	SetLastError(__err);                                                          \
} while(0, 0)
#else
#define _RPT_API_TRACE(name)	__noop
#endif

#ifdef _DEBUG
#define _RPT_API_ERROR(name)	                                                   \
do                                                                               \
{                                                                                \
	unsigned __err = GetLastError();                                              \
	WCHAR     wcMsg[512];                                                         \
	                                                                              \
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, __err, 0, wcMsg, 512, NULL); \
	                                                                              \
	_RPTFW3(_CRT_ERROR, L"%S() failed; ERR=%d %s\n", #name, __err, wcMsg);        \
	                                                                              \
	SetLastError(__err);                                                          \
	} while(0, 0)
#else
#define _RPT_API_ERROR(name)	__noop
#endif
