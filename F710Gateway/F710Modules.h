#ifndef _F710Modules_
#define _F710Modules_

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

	// szXMLUri�ɂ͐ݒ�t�@�C���̃p�X���w�肵�Ă��������B
	BOOL F710Start(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd);
	// �v���O�����̏I���ɕK���Ă�ł��������i���������[�N�ɂȂ���܂��j�B
	void F710Stop(void);

	// Gamepad�̓��͂��`�F�b�N���܂��B
	void F710CheckInput(void);

#ifdef __cplusplus
}
#endif

#endif /* _F710Modules_ */