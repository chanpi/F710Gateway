#ifndef _I4C3DDI4LModules_
#define _I4C3DDI4LModules_

#pragma comment(linker, "/export:I4C3DDI4LStart=_I4C3DDI4LStart@12")
#pragma comment(linker, "/export:I4C3DDI4LStop=_I4C3DDI4LStop@0")
#pragma comment(linker, "/export:I4C3DDI4LCheckInput=_I4C3DDI4LCheckInput@0")

#include <Windows.h>

#ifdef I4C3DDINPUT4LOGICOOLMODULES_EXPORTS
#define DLL_EXPORT		__declspec(dllexport)
#else
#define DLL_EXPORT		__declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// szXMLUriには設定ファイルのパスを指定してください。
	DLL_EXPORT BOOL WINAPI I4C3DDI4LStart(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd);
	// プログラムの終わりに必ず呼んでください（メモリリークにつながります）。
	DLL_EXPORT void WINAPI I4C3DDI4LStop(void);

	// Gamepadの入力をチェックします。
	DLL_EXPORT void WINAPI I4C3DDI4LCheckInput(void);

#ifdef __cplusplus
}
#endif

#endif /* _I4C3DDI4LModules_ */