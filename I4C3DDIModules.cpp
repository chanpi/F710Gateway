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
 * I4C3Dモジュールを開始します。
 * 外部のアプリケーションはこの関数を呼ぶことでモジュールを開始できます。
 * 
 * @param szXMLUri
 * 設定ファイルのXMLへのパス。
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
 * プログラム終了時には必ずI4C3DStop()を呼んでください。
 * 
 * @see
 * I4C3DStop()
 */
BOOL WINAPI I4C3DDIStart(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd)
{
	if (g_bStarted) {
		return TRUE;
	}
	// 必要な初期化
	ZeroMemory(&g_Core, sizeof(g_Core));
	ZeroMemory(&g_Context, sizeof(g_Context));
	g_Context.hInst = hInst;
	g_Context.hWnd = hWnd;
	g_Context.pAnalyzer = new I4C3DDIAnalyzeXML();
	if (g_Context.pAnalyzer == NULL) {
		ReportError(_T("[ERROR] メモリの確保に失敗しています。初期化は行われません。"));
		I4C3DDIStop();
		return FALSE;
	}
	if (!g_Context.pAnalyzer->LoadXML(szXMLUri)) {
		ReportError(_T("[ERROR] XMLのロードに失敗しています。初期化は行われません。"));
		I4C3DDIStop();
		return FALSE;
	}

	//// 設定ファイルからログレベルを取得
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
	//	// 指定なし、上記以外ならLog_Error
	//}
	//LogFileOpenW("mainmodule", logLevel);
	//if (logLevel <= Log_Info) {
	//	LogFileOpenA("mainmoduleinfo", logLevel);	// プロファイル情報書き出しのため
	//}

	// 設定ファイルから終端文字を取得
	char cTermination = '?';
	PCTSTR szTermination = g_Context.pAnalyzer->GetGlobalValue(TAG_TERMINATION);
	if (szTermination != NULL) {
		char cszTermination[5];
		if (_tcslen(szTermination) != 1) {
			LogDebugMessage(Log_Error, _T("設定ファイルの終端文字の指定に誤りがあります。1文字で指定してください。'?'に仮指定されます"));
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
 * I4C3Dモジュールを終了します。
 * 
 * I4C3Dモジュールを終了します。
 * プログラムの終わりには必ず呼び出してください（メモリリークにつながります）。
 * 
 * @remarks
 * I4C3DStart()と対応しています。
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
	LogFileCloseA();	// Openしていないときは何もせずTRUEが返る
}

// 入力チェック
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

	//// 初期化
	//if (g_Context.pController == NULL || !g_Context.pController->Initialize(&g_Context, cTermination)) {
	//	LogDebugMessage(Log_Error, _T("コントローラの初期化に失敗しています。<I4C3DModules::SelectTargetController>"));
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