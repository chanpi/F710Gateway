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

	// szXMLUri�ɂ͐ݒ�t�@�C���̃p�X���w�肵�Ă��������B
	DLL_EXPORT BOOL WINAPI F710Start(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd);
	// �v���O�����̏I���ɕK���Ă�ł��������i���������[�N�ɂȂ���܂��j�B
	DLL_EXPORT void WINAPI F710Stop(void);

	// Gamepad�̓��͂��`�F�b�N���܂��B
	DLL_EXPORT void WINAPI F710CheckInput(void);

#ifdef __cplusplus
}
#endif

#endif /* _F710Modules_ */