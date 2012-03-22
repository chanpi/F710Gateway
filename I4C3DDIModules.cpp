#include "stdafx.h"
#include "I4C3DDIModules.h"
#include "I4C3DDIAnalyzeXML.h"
#include "I4C3DDICore.h"
#include "I4C3DDIControl.h"
#include "Miscellaneous.h"

static BOOL PrepareTargetController(char cTermination);
static const PCTSTR TAG_TERMINATION	= _T("termination");

static I4C3DDICore g_Core;
static I4C3DDIContext g_Context;
static BOOL g_bStarted = FALSE;

/**
 * @brief
 * I4C3D���W���[�����J�n���܂��B
 * �O���̃A�v���P�[�V�����͂��̊֐����ĂԂ��ƂŃ��W���[�����J�n�ł��܂��B
 * 
 * @param szXMLUri
 * �ݒ�t�@�C����XML�ւ̃p�X�B
 * 
 * @param szTargetWindowTitle
 * Description of parameter szTargetWindowTitle.
 * 
 * @returns
 * Write description of return value here.
 * 
 * Write detailed description for I4C3DStart here.
 * 
 * @remarks
 * �v���O�����I�����ɂ͕K��I4C3DStop()���Ă�ł��������B
 * 
 * @see
 * I4C3DStop()
 */
BOOL WINAPI I4C3DDIStart(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd)
{
	if (g_bStarted) {
		return TRUE;
	}
	// �K�v�ȏ�����
	ZeroMemory(&g_Core, sizeof(g_Core));
	ZeroMemory(&g_Context, sizeof(g_Context));
	g_Context.hInst = hInst;
	g_Context.hWnd = hWnd;
	g_Context.pAnalyzer = new I4C3DDIAnalyzeXML();
	if (g_Context.pAnalyzer == NULL) {
		ReportError(_T("[ERROR] �������̊m�ۂɎ��s���Ă��܂��B�������͍s���܂���B"));
		I4C3DDIStop();
		return FALSE;
	}
	if (!g_Context.pAnalyzer->LoadXML(szXMLUri)) {
		ReportError(_T("[ERROR] XML�̃��[�h�Ɏ��s���Ă��܂��B�������͍s���܂���B"));
		I4C3DDIStop();
		return FALSE;
	}

	//// �ݒ�t�@�C�����烍�O���x�����擾
	//LOG_LEVEL logLevel = Log_Error;
	//PCTSTR szLogLevel = g_Context.pAnalyzer->GetGlobalValue(TAG_LOG);
	//if (szLogLevel != NULL) {
	//	if (!_tcsicmp(szLogLevel, TAG_OFF)) {
	//		logLevel = Log_Off;
	//	} else if (!_tcsicmp(szLogLevel, TAG_DEBUG)) {
	//		logLevel = Log_Debug;
	//	} else if (!_tcsicmp(szLogLevel, TAG_INFO)) {
	//		logLevel = Log_Info;
	//	}
	//	// �w��Ȃ��A��L�ȊO�Ȃ�Log_Error
	//}
	//LogFileOpenW("mainmodule", logLevel);
	//if (logLevel <= Log_Info) {
	//	LogFileOpenA("mainmoduleinfo", logLevel);	// �v���t�@�C����񏑂��o���̂���
	//}

	// �ݒ�t�@�C������I�[�������擾
	char cTermination = '?';
	PCTSTR szTermination = g_Context.pAnalyzer->GetGlobalValue(TAG_TERMINATION);
	if (szTermination != NULL) {
		char cszTermination[5];
		if (_tcslen(szTermination) != 1) {
			LogDebugMessage(Log_Error, _T("�ݒ�t�@�C���̏I�[�����̎w��Ɍ�肪����܂��B1�����Ŏw�肵�Ă��������B'?'�ɉ��w�肳��܂�"));
			szTermination = _T("?");
		}
#if UNICODE || _UNICODE
		WideCharToMultiByte(CP_ACP, 0, szTermination, _tcslen(szTermination), cszTermination, sizeof(cszTermination), NULL, NULL);
#else
		strcpy_s(cszTermination, sizeof(cszTermination), szTermination);
#endif
		RemoveWhiteSpaceA(cszTermination);
		cTermination = cszTermination[0];
	}

	if (!PrepareTargetController(cTermination)) {
		I4C3DDIStop();
		return FALSE;
	}

	g_bStarted = g_Core.Start(&g_Context);
	if (!g_bStarted) {
		I4C3DDIStop();
	}
	return g_bStarted;
}

/**
 * @brief
 * I4C3D���W���[�����I�����܂��B
 * 
 * I4C3D���W���[�����I�����܂��B
 * �v���O�����̏I���ɂ͕K���Ăяo���Ă��������i���������[�N�ɂȂ���܂��j�B
 * 
 * @remarks
 * I4C3DStart()�ƑΉ����Ă��܂��B
 * 
 * @see
 * I4C3DStart()
 */
void WINAPI I4C3DDIStop(void)
{
	g_Core.Stop(&g_Context);
	if (g_Context.pAnalyzer != NULL) {
		delete g_Context.pAnalyzer;
		g_Context.pAnalyzer = NULL;
	}
	if (g_Context.pController != NULL) {
		delete g_Context.pController;
		g_Context.pController = NULL;
	}
	//DestroyTargetController();
	//g_Context.bIsAlive = FALSE;
	g_bStarted = FALSE;

	LogFileCloseW();
	LogFileCloseA();	// Open���Ă��Ȃ��Ƃ��͉�������TRUE���Ԃ�
}

// ���̓`�F�b�N
void WINAPI I4C3DDICheckInput(void)
{
	g_Core.CheckInput(&g_Context);
}

BOOL PrepareTargetController(char cTermination)
{
	if (g_Context.pController != NULL) {
		delete g_Context.pController;
		g_Context.pController = NULL;
	}
	g_Context.pController = new I4C3DDIControl;

	//// ������
	//if (g_Context.pController == NULL || !g_Context.pController->Initialize(&g_Context, cTermination)) {
	//	LogDebugMessage(Log_Error, _T("�R���g���[���̏������Ɏ��s���Ă��܂��B<I4C3DModules::SelectTargetController>"));
	//	return FALSE;
	//}

	return TRUE;
}

//void DestroyTargetController()
//{
//	if (g_Context.pController != NULL) {
//		g_Context.pController->UnInitialize();
//	}
//}