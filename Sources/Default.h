#pragma once

// компания
#define COMPANY_NAME L"www.wholegroup.com"

// название программы
#define PROGRAM_NAME L"OffMon"

// язык по-умолчанию
#define DEFAULT_LANGUAGE MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL)

// код выхода для смены языка
#define CHANGE_LANGUAGE 123

// сообщение для смены языка программы
#define WM_USER_CHANGE_LANGUAGE WM_USER + 0x801

// цвет прозрачного фона
#define RGB_TRANSPARENT RGB(89, 117, 26)

// высота цифры относительно высоты окна
#define DEFAULT_HEIGHT_NUMBER 0.67

// толщина круга относительно высоты окна
#define DEFAULT_WIDTH_CIRCLE 0.08

// высота окна по-умолчанию
#define DEFAULT_HEIGHT     1
#define DEFAULT_HEIGHT_MIN 0.1
#define DEFAULT_HEIGHT_MAX 1

// количество секунд до выключения
#define DEFAULT_SECONDS     3
#define DEFAULT_SECONDS_MIN 1
#define DEFAULT_SECONDS_MAX 9

// цвет по-умолчанию
#define DEFAULT_COLOR RGB(57, 128, 124)

// прозрачность по-умолчанию
#define DEFAULT_TRANSPARENCY     100
#define DEFAULT_TRANSPARENCY_MIN 25
#define DEFAULT_TRANSPARENCY_MAX 255

// цифра выводимая при настройке
#define DEFAULT_NUMBER_SETTINGS 4

// частота таймера в мс для отсчета выключения
#define DEFAULT_TIMER_COUNTDOWN 1000

// частота таймера в мс для проверки наличия курсора мыши в левом верхнем углу
#define DEFAULT_TIMER_LEFTUP 1000

// время бездействия для выключения монитора, когда мышь в левом верхнем углу
#define DEFAULT_TIME_LEFTUP 3000

// флаг блокировки рабочей станции по-умолчанию
#define DEFAULT_LOCK FALSE

// флаг выхода из системы по-умолчанию
#define DEFAULT_LOGOFF FALSE

// флаг по-умолчанию выключения монитора при запуске программы
#define DEFAULT_OFFONRUN FALSE

// флаг по-умолчанию выключения монитора сразу после блокировки станции
#define DEFAULT_OFFAFTERLOCK TRUE

// флаг по-умолчанию выключения монитора сразу после запуска скринсейвера
#define DEFAULT_OFFAFTERSS FALSE

// флаг по-умолчанию выключения монитора при наведении мыши в левый верхний угол
#define DEFAULT_OFFLEFTUP FALSE

// идентификаторы таймеров
enum OFFMON_TIMERS
{
	OFFMON_TIMER_COUNTDOWN = 0, // таймер для отсчета выключения
	OFFMON_TIMER_LEFTUP         // таймер для отслеживания мыши в левом верхнем углу
};

// макросы для отладки
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
