#ifndef _F710Modules_
#define _F710Modules_

#pragma comment(linker, "/export:F710Start=_F710Start@12")
#pragma comment(linker, "/export:F710Stop=_F710Stop@0")
#pragma comment(linker, "/export:F710CheckInput=_F710CheckInput@0")

#include <Windows.h>

#ifdef LOGICOOLF710REPEATER_EXPORTS
#define DLL_EXPORT		__declspec(dllexport)
#else
#define DLL_EXPORT		__declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// szXMLUriには設定ファイルのパスを指定してください。
	DLL_EXPORT BOOL WINAPI F710Start(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd);
	// プログラムの終わりに必ず呼んでください（メモリリークにつながります）。
	DLL_EXPORT void WINAPI F710Stop(void);

	// Gamepadの入力をチェックします。
	DLL_EXPORT void WINAPI F710CheckInput(void);

#ifdef __cplusplus
}
#endif

#endif /* _F710Modules_ */