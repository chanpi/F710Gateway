#include "stdafx.h"
#include "I4C3DCommon.h"
#include "F710Modules.h"
#include "F710AnalyzeXML.h"
#include "F710Core.h"
#include "F710Control.h"
#include "F710TCPControl.h"
#include "Misc.h"
#include "MemoryLeak.h"
#include "SharedConstants.h"

#if UNICODE || _UNICODE
static LPCTSTR g_FILE = __FILEW__;
#else
static LPCTSTR g_FILE = __FILE__;
#endif

static BOOL PrepareTargetController(char cTermination);
static void DestroyTargetController();

static const PCTSTR TAG_LOG			= _T("log");
static const PCTSTR TAG_OFF			= _T("off");
static const PCTSTR TAG_DEBUG		= _T("debug");
static const PCTSTR TAG_INFO		= _T("info");
static const PCTSTR TAG_TERMINATION	= _T("termination");
static const PCTSTR TAG_RTTECMODE	= _T("rttec_mode");

static F710Core g_Core;
static F710Context g_Context;
static BOOL g_bStarted = FALSE;

extern int F710ExitCode;

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
BOOL F710Start(PCTSTR szXMLUri, HINSTANCE hInst, HWND hWnd)
{
	if (g_bStarted) {
		return TRUE;
	}
	// 必要な初期化
	ZeroMemory(&g_Core, sizeof(g_Core));
	ZeroMemory(&g_Context, sizeof(g_Context));
	g_Context.hInst = hInst;
	g_Context.hWnd = hWnd;
	g_Context.pAnalyzer = new F710AnalyzeXML;
	if (g_Context.pAnalyzer == NULL) {
		ReportError(_T(MESSAGE_ERROR_MEMORY_INVALID));
		F710Stop();
		F710ExitCode = EXIT_SYSTEM_ERROR;
		return FALSE;
	}
	if (!g_Context.pAnalyzer->LoadXML(szXMLUri)) {
		ReportError(_T(MESSAGE_ERROR_XML_LOAD));
		F710Stop();
		F710ExitCode = EXIT_INVALID_FILE_CONFIGURATION;
		return FALSE;
	}

	// 設定ファイルからログレベルを取得
	LOG_LEVEL logLevel = Log_Error;
	PCTSTR szLogLevel = g_Context.pAnalyzer->GetGlobalValue(TAG_LOG);
	if (szLogLevel != NULL) {
		if (!_tcsicmp(szLogLevel, TAG_OFF)) {
			logLevel = Log_Off;
		} else if (!_tcsicmp(szLogLevel, TAG_DEBUG)) {
			logLevel = Log_Debug;
		} else if (!_tcsicmp(szLogLevel, TAG_INFO)) {
			logLevel = Log_Info;
		}
		// 指定なし、上記以外ならLog_Error
	}
	LogFileOpenW(SHARED_LOG_FILE_NAME, logLevel);
	if (logLevel <= Log_Info) {
		LogFileOpenA(SHARED_LOGINFO_FILE_NAME, logLevel);	// プロファイル情報書き出しのため
	}

	// 設定ファイルから終端文字を取得
	char cTermination = '?';
	PCTSTR szTermination = g_Context.pAnalyzer->GetGlobalValue(TAG_TERMINATION);
	if (szTermination != NULL) {
		char cszTermination[5];
		if (_tcslen(szTermination) != 1) {
			LoggingMessage(Log_Error, _T(MESSAGE_ERROR_CFG_TERMCHAR), GetLastError(), g_FILE, __LINE__);
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

	g_Context.bRTTECMode = (_tcsicmp(g_Context.pAnalyzer->GetGlobalValue(TAG_RTTECMODE), _T("on")) == 0);
	if (!PrepareTargetController(cTermination)) {
		F710Stop();
		F710ExitCode = EXIT_SYSTEM_ERROR;
	}

	g_bStarted = g_Core.Start(&g_Context);
	if (!g_bStarted) {
		F710Stop();
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
void F710Stop(void)
{
	g_Core.Stop(&g_Context);
	if (g_Context.pAnalyzer != NULL) {
		delete g_Context.pAnalyzer;
		g_Context.pAnalyzer = NULL;
	}
	DestroyTargetController();

	//g_Context.bIsAlive = FALSE;
	g_bStarted = FALSE;

	LogFileCloseW();
	LogFileCloseA();	// Openしていないときは何もせずTRUEが返る
}

// 入力チェック
void F710CheckInput(void)
{
	g_Core.CheckInput(&g_Context);
}

BOOL PrepareTargetController(char cTermination)
{
	if (g_Context.pController != NULL) {
		DestroyTargetController();
	}

	if (g_Context.bRTTECMode) {
		g_Context.pController = new F710TCPControl;
	} else {
		g_Context.pController = new F710Control;
	}

	// 初期化
	if (g_Context.pController == NULL || !g_Context.pController->Initialize(&g_Context, cTermination)) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_SYSTEM_INIT), GetLastError(), g_FILE, __LINE__);
		return FALSE;
	}

	return TRUE;
}

void DestroyTargetController()
{
	if (g_Context.pController != NULL) {
		g_Context.pController->UnInitialize();
		delete g_Context.pController;
		g_Context.pController = NULL;
	}
}