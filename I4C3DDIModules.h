#ifndef _I4C3DDIModules_
#define _I4C3DDIModules_

#pragma comment(linker, "/export:I4C3DDIStart=_I4C3DDIStart@12")
#pragma comment(linker, "/export:I4C3DDIStop=_I4C3DDIStop@0")
#pragma comment(linker, "/export:I4C3DDICheckInput=_I4C3DDICheckInput@0")

#include <Windows.h>

#ifdef I4C3DDIRECTINPUTMODULES_EXPORTS
#define DLL_EXPORT		__declspec(dllexport)
#else
#define DLL_EXPORT		__declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// szXMLUri�ɂ͐ݒ�t�@�C���̃p�X���w�肵�Ă��������B
	DLL_EXPORT BOOL WINAPI I4C3DDIStart(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd);
	// �v���O�����̏I���ɕK���Ă�ł��������i���������[�N�ɂȂ���܂��j�B
	DLL_EXPORT void WINAPI I4C3DDIStop(void);

	// Gamepad�̓��͂��`�F�b�N���܂��B
	DLL_EXPORT void WINAPI I4C3DDICheckInput(void);

#ifdef __cplusplus
}
#endif

#endif /* _I4C3DDIModules_ */