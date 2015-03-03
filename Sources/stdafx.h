#pragma once

// минимально требуемые версии
#define WINVER        0x0500
#define _WIN32_WINNT  0x0501
#define _WIN32_IE     0x0501
#define _RICHEDIT_VER 0x0200

// WinAPI
#include <Windows.h>

// GDI+
#include <GdiPlus.h>

// STL
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <ctime>

// тип списка языков (ИД, название)
typedef std::map<WORD, const std::wstring> CMapWordStr;

// WTL
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include <atldlgs.h>

// AGG
#pragma warning(push)
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4267) // conversion x64

#include "agg-2.4/include/agg_scanline_p.h"
#include "agg-2.4/include/agg_renderer_scanline.h"
#include "agg-2.4/include/agg_pixfmt_rgba.h"
#include "agg-2.4/include/agg_ellipse.h"
#include "agg-2.4/include/agg_conv_stroke.h"
#include "agg-2.4/font_win32_tt/agg_font_win32_tt.h"

#pragma warning(pop)

typedef agg::font_engine_win32_tt_int16                       font_engine_type;
typedef agg::font_cache_manager<font_engine_type>             font_manager_type;
typedef agg::conv_curve<font_manager_type::path_adaptor_type> conv_font_curve_type;

// для использования настроек программы
#include "WTLAddons/CSettingsImpl.h"

// для создания иконки в трее
#include "CTrayIconImpl.h"

// версия программы (генерируется при сборке MSBuild)
#include "Version.h"

// константы по-умолчанию
#include "Default.h"

// шаблон для создания мультиязычных диалогов
#include "DlgTemplate.h"

extern CAppModule _Module;

#if defined _M_IX86
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


