#ifndef _F710Modules_
#define _F710Modules_

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

	// szXMLUriには設定ファイルのパスを指定してください。
	BOOL F710Start(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd);
	// プログラムの終わりに必ず呼んでください（メモリリークにつながります）。
	void F710Stop(void);

	// Gamepadの入力をチェックします。
	void F710CheckInput(void);

#ifdef __cplusplus
}
#endif

#endif /* _F710Modules_ */