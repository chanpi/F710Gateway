#include "StdAfx.h"
#include "I4C3DDIAccessor.h"
#include "I4C3DDIAnalyzeXML.h"
#include "I4C3DDICore.h"
#include "I4C3DDIControl.h"
#include "I4C3DDIModulesDefs.h"
#include "Miscellaneous.h"
#include <dinput.h>
#include <map>

using namespace std;

#define SAFE_RELEASE(p)		{ if (p) { (p)->Release(); (p)=NULL; } }

static BOOL CALLBACK I4C3DDIEnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* /*pContext*/ );
static BOOL CALLBACK I4C3DDIEnumAxesCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID /*pvRef*/ );
static void SearchAnimationByKey(LPSTR destination, LPCTSTR key);

static LPDIRECTINPUT8 g_pDInput			= NULL;	// DirectInput
static LPDIRECTINPUTDEVICE8 g_pDIDev	= NULL;	// DirectInput
static DIDEVCAPS g_diDevCaps;					// ジョイスティックの能力
static I4C3DDICommandSet g_CommandSet	= {0};

static map<LPCTSTR, LPVOID> g_FunctionMap;		// 実行すべき処理とその関数を保存します(例：CAMERA_UP:CameraUp関数へのポインタ)
//static map<wstring, LPVOID> g_FunctionMap;		// 実行すべき処理とその関数を保存します(例：CAMERA_UP:CameraUp関数へのポインタ)

static const PCTSTR TAG_BUTTON_X		= _T("BUTTON_X");
static const PCTSTR TAG_BUTTON_A		= _T("BUTTON_A");
static const PCTSTR TAG_BUTTON_B		= _T("BUTTON_B");
static const PCTSTR TAG_BUTTON_Y		= _T("BUTTON_Y");
static const PCTSTR TAG_BUTTON_LB		= _T("BUTTON_LB");
static const PCTSTR TAG_BUTTON_LT		= _T("BUTTON_LT");
static const PCTSTR TAG_BUTTON_RB		= _T("BUTTON_RB");
static const PCTSTR TAG_BUTTON_RT		= _T("BUTTON_RT");
static const PCTSTR TAG_BUTTON_UP		= _T("BUTTON_UP");
static const PCTSTR TAG_BUTTON_DOWN		= _T("BUTTON_DOWN");
static const PCTSTR TAG_BUTTON_LEFT		= _T("BUTTON_LEFT");
static const PCTSTR TAG_BUTTON_RIGHT	= _T("BUTTON_RIGHT");
static const PCTSTR TAG_BUTTON_BACK		= _T("BUTTON_BACK");
static const PCTSTR TAG_BUTTON_START	= _T("BUTTON_START");
static const PCTSTR TAG_STICK_L_UP		= _T("STICK_L_UP");
static const PCTSTR TAG_STICK_L_DOWN	= _T("STICK_L_DOWN");
static const PCTSTR TAG_STICK_L_LEFT	= _T("STICK_L_LEFT");
static const PCTSTR TAG_STICK_L_RIGHT	= _T("STICK_L_RIGHT");
static const PCTSTR TAG_STICK_R_UP		= _T("STICK_R_UP");
static const PCTSTR TAG_STICK_R_DOWN	= _T("STICK_R_DOWN");
static const PCTSTR TAG_STICK_R_LEFT	= _T("STICK_R_LEFT");
static const PCTSTR TAG_STICK_R_RIGHT	= _T("STICK_R_RIGHT");
static const PCTSTR TAG_moveDelta		= _T("moveDelta");
static const PCTSTR TAG_angleDelta		= _T("angleDelta");
static const PCTSTR TAG_pitchDelta		= _T("pitchDelta");
static const PCTSTR TAG_heightDelta		= _T("heightDelta");
static const PCTSTR TAG_animation01Title	= _T("animation01Title");
static const PCTSTR TAG_animation02Title	= _T("animation02Title");
static const PCTSTR TAG_animation03Title	= _T("animation03Title");
static const PCTSTR TAG_animation04Title	= _T("animation04Title");

I4C3DDICore::I4C3DDICore(void)
{
}


I4C3DDICore::~I4C3DDICore(void)
{
}

BOOL I4C3DDICore::Start(I4C3DDIContext* pContext)
{
	// FunctionMapを初期化
	InitializeFunctionMap(pContext);
	// 設定ファイルを読む
	ReadConfigurationFile(pContext);
	// DirectInputの初期化
	InitializeDirectInput(pContext);

	pContext->pCommandSet = &g_CommandSet;

	USHORT uI4C3DModulePort = 0;
	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(_T("port")), _T("%u"), &uI4C3DModulePort);

	I4C3DDIAccessor accessor;
	pContext->sender = accessor.InitializeTCPSocket((sockaddr_in*)&pContext->address, "127.0.0.1", TRUE, uI4C3DModulePort);
	if (pContext->sender == INVALID_SOCKET) {
		LogDebugMessage(Log_Error, _T("InitializeSocket <I4C3DDIModules::PrepareTargetController>"));
		return FALSE;
	}
	return TRUE;
}

void I4C3DDICore::Stop(I4C3DDIContext* pContext)
{
	UnInitialize(pContext);
}

/**
 * @brief
 * I4C3Dモジュールの終了処理を行います。
 * 
 * @param pContext
 * I4C3Dモジュールのコンテキストのポインタ。
 * 
 * I4C3Dモジュールの終了処理を行います。
 * 各イベントオブジェクトの停止、解放、各スレッドの終了を待ち、解放します。
 */
void I4C3DDICore::UnInitialize(I4C3DDIContext* pContext)
{
	// ソケットクローズ
	if (pContext->sender != INVALID_SOCKET) {
		closesocket(pContext->sender);
		pContext->sender = INVALID_SOCKET;
	}

	if (g_CommandSet.animation01Title) {
		delete g_CommandSet.animation01Title;
		g_CommandSet.animation01Title = NULL;
	}
	if (g_CommandSet.animation02Title) {
		delete g_CommandSet.animation02Title;
		g_CommandSet.animation02Title = NULL;
	}
	if (g_CommandSet.animation03Title) {
		delete g_CommandSet.animation03Title;
		g_CommandSet.animation03Title = NULL;
	}
	if (g_CommandSet.animation04Title) {
		delete g_CommandSet.animation04Title;
		g_CommandSet.animation04Title = NULL;
	}
}

HRESULT I4C3DDICore::InitializeDirectInput(I4C3DDIContext* pContext)
{
	HRESULT hr;
	static bool success = false;

	if (success) {
		return S_OK;
	}

	// DirectInputの作成
	hr = DirectInput8Create( pContext->hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDInput, NULL );
	
	if ( FAILED( hr ) ) {
		ReportError(_T("[ERROR] DirectInput8Create()"));
		LogDebugMessage(Log_Error, _T("[ERROR] DirectInput8Create()"));
		return hr;
	}

	// デバイスを列挙して作成
	hr = g_pDInput->EnumDevices( DI8DEVCLASS_GAMECTRL, I4C3DDIEnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY );
	if ( FAILED( hr ) || g_pDIDev == NULL ) {
		ReportError(_T("[ERROR] EnumDevices()"));
		LogDebugMessage(Log_Error, _T("[ERROR] EnumDevices()"));
		return hr;
	}

	// データ形式を設定
	hr = g_pDIDev->SetDataFormat( &c_dfDIJoystick2 );
	if ( FAILED( hr ) ) {
		ReportError(_T("[ERROR] SetDataFormat()"));
		LogDebugMessage(Log_Error, _T("[ERROR] SetDataFormat()"));
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDIDev->SetCooperativeLevel( pContext->hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
	if ( FAILED( hr ) ) {
		ReportError(_T("[ERROR] SetCooperativeLevel()"));
		LogDebugMessage(Log_Error, _T("[ERROR] SetCooperativeLevel()"));
		return hr;
	}

	// コールバック関数を使って各軸のモードを設定
	hr = g_pDIDev->EnumObjects( I4C3DDIEnumAxesCallback, NULL, DIDFT_AXIS );
	if ( FAILED( hr ) ) {
		ReportError(_T("[ERROR] EnumObjects()"));
		LogDebugMessage(Log_Error, _T("[ERROR] EnumObjects()"));
		return hr;
	}
	// 入力制御開始
	g_pDIDev->Acquire();

	success = true;
	return S_OK;
}

// コマンドとその関数とをマップに格納
void I4C3DDICore::InitializeFunctionMap(I4C3DDIContext* pContext)
{
	static bool success = false;

	if (success) {
		return;
	}

	// mapは同一キーの場合上書きされる
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("GO_FORWARD"), pContext->pController->GoForward) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("GO_BACKWARD"), pContext->pController->GoBackward) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("GO_UP"), pContext->pController->GoUp) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("GO_DOWN"), pContext->pController->GoDown) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("GO_LEFT"), pContext->pController->GoLeft) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("GO_RIGHT"), pContext->pController->GoRight) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("CAMERA_UP"), pContext->pController->CameraUp) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("CAMERA_DOWN"), pContext->pController->CameraDown) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("CAMERA_LEFT"), pContext->pController->CameraLeft) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("CAMERA_RIGHT"), pContext->pController->CameraRight) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("SPEED_UP"), pContext->pController->ChangeSpeed) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("ANIM01_START"), pContext->pController->StartAnimation1) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("ANIM02_START"), pContext->pController->StartAnimation2) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("ANIM03_START"), pContext->pController->StartAnimation3) );
	g_FunctionMap.insert( map<LPCTSTR, LPVOID>::value_type(_T("ANIM04_START"), pContext->pController->StartAnimation4) );

	success = true;
}

LPVOID I4C3DDICore::SearchFunctionByKey(LPCTSTR key)
{
	if (key != NULL) {
		map<LPCTSTR, LPVOID>::iterator it = g_FunctionMap.find(key);
		if (it != g_FunctionMap.end()) {
			return it->second;
		}
	}
	return NULL;
}

void SearchAnimationByKey(LPSTR destination, LPCTSTR key)
{
	if (destination != NULL) {
		delete destination;
	}
	g_CommandSet.animation01Title = new CHAR[_tcslen(key)];

#if UNICODE || _UNICODE
	WideCharToMultiByte(CP_ACP, 0, key, _tcslen(key), destination, sizeof(destination), NULL, NULL);
#else
	strcpy_s(destination, sizeof(destination), key);
#endif
}

void I4C3DDICore::ReadConfigurationFile(I4C3DDIContext* pContext)
{
	LPCSTR key = NULL;

	g_CommandSet.BUTTON_X = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_X));
	g_CommandSet.BUTTON_A = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_A));
	g_CommandSet.BUTTON_B = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_B));
	g_CommandSet.BUTTON_Y = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_Y));
	g_CommandSet.BUTTON_LB = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_LB));
	g_CommandSet.BUTTON_LT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_LT));
	g_CommandSet.BUTTON_RB = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_RB));
	g_CommandSet.BUTTON_RT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_RT));
	g_CommandSet.BUTTON_UP = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_UP));
	g_CommandSet.BUTTON_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_DOWN));
	g_CommandSet.BUTTON_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_LEFT));
	g_CommandSet.BUTTON_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_RIGHT));
	g_CommandSet.BUTTON_BACK = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_BACK));
	g_CommandSet.BUTTON_START = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_BUTTON_START));
	g_CommandSet.STICK_L_UP = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_STICK_L_UP));
	g_CommandSet.STICK_L_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_STICK_L_DOWN));
	g_CommandSet.STICK_L_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_STICK_L_LEFT));
	g_CommandSet.STICK_L_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_STICK_L_RIGHT));
	g_CommandSet.STICK_R_UP = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_STICK_R_UP));
	g_CommandSet.STICK_R_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_STICK_R_DOWN));
	g_CommandSet.STICK_R_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_STICK_R_LEFT));
	g_CommandSet.STICK_R_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(TAG_STICK_R_RIGHT));

	// アニメーションタイトル
	SearchAnimationByKey(g_CommandSet.animation01Title, pContext->pAnalyzer->GetGlobalValue(TAG_animation01Title));
	SearchAnimationByKey(g_CommandSet.animation02Title, pContext->pAnalyzer->GetGlobalValue(TAG_animation02Title));
	SearchAnimationByKey(g_CommandSet.animation03Title, pContext->pAnalyzer->GetGlobalValue(TAG_animation03Title));
	SearchAnimationByKey(g_CommandSet.animation04Title, pContext->pAnalyzer->GetGlobalValue(TAG_animation04Title));

	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(TAG_moveDelta), _T("%d"), &g_CommandSet.moveDelta);
	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(TAG_angleDelta), _T("%d"), &g_CommandSet.angleDelta);
	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(TAG_pitchDelta), _T("%d"), &g_CommandSet.pitchDelta);
	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(TAG_heightDelta), _T("%d"), &g_CommandSet.heightDelta);
}

// ジョイスティックを列挙する関数
BOOL CALLBACK I4C3DDIEnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* /*pContext*/ )
{
	HRESULT hr;

	// 列挙されたジョイスティックへのインターフェイスを取得する
	hr = g_pDInput->CreateDevice( pdidInstance->guidInstance, &g_pDIDev, NULL );
	if ( FAILED( hr ) ) {
		return DIENUM_CONTINUE;
	}
	// ジョイスティックの能力を調べる
	g_diDevCaps.dwSize = sizeof( DIDEVCAPS );
	hr = g_pDIDev->GetCapabilities( &g_diDevCaps );
	if ( FAILED( hr ) ) {
		// ジョイスティック能力の取得に失敗
		SAFE_RELEASE( g_pDIDev );
		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}

// ジョイスティックの軸を列挙する関数。
// 最小値を-1000に、最大値を+1000に設定する。
BOOL CALLBACK I4C3DDIEnumAxesCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID /*pvRef*/ )
{
	HRESULT hr;

	// 軸の値の範囲を設定（-1000 - 1000）
	DIPROPRANGE diprg;
	ZeroMemory( &diprg, sizeof( diprg ) );
	diprg.diph.dwSize = sizeof( diprg );
	diprg.diph.dwHeaderSize = sizeof( diprg.diph );
	diprg.diph.dwObj = lpddoi->dwType;
	diprg.diph.dwHow = DIPH_BYID;
	diprg.lMin = -1000;
	diprg.lMax = +1000;
	hr = g_pDIDev->SetProperty( DIPROP_RANGE, &diprg.diph );
	if ( FAILED( hr ) ) {
		return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

// Controller入力
void I4C3DDICore::CheckInput(I4C3DDIContext* pContext)
{
	DIJOYSTATE2 dijs = {0};
	HRESULT hr;
	void (*func)(I4C3DDIContext*) = NULL;

	hr = g_pDIDev->GetDeviceState( sizeof( DIJOYSTATE2 ), &dijs );
	if ( SUCCEEDED( hr ) ) {
		// Xボタン
		if (dijs.rgbButtons[0] & 0x80) {
			OutputDebugString(_T("Key>> X\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_X;

		// Aボタン
		} else if (dijs.rgbButtons[1] & 0x80) {
			OutputDebugString(_T("Key>> A\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_A;

		// Bボタン
		} else if (dijs.rgbButtons[2] & 0x80) {
			OutputDebugString(_T("Key>> B\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_B;

		// Yボタン
		} else if (dijs.rgbButtons[3] & 0x80) {
			OutputDebugString(_T("Key>> Y\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_Y;

		// LBボタン
		} else if (dijs.rgbButtons[4] & 0x80) {
			OutputDebugString(_T("Key>> LB\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_LB;

		// RBボタン
		} else if (dijs.rgbButtons[5] & 0x80) {
			OutputDebugString(_T("Key>> RB\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_RB;

		// LTボタン
		} else if (dijs.rgbButtons[6] & 0x80) {
			OutputDebugString(_T("Key>> LT\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_LT;

		// RTボタン
		} else if (dijs.rgbButtons[7] & 0x80) {
			OutputDebugString(_T("Key>> RT\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_RT;

		// BACKボタン
		} else if (dijs.rgbButtons[8] & 0x80) {
			OutputDebugString(_T("Key>> BACK\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_BACK;

		// STARTボタン
		} else if (dijs.rgbButtons[9] & 0x80) {
			OutputDebugString(_T("Key>> START\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_START;

		// 方向キー上
		} else if (dijs.lY == -1000) {
			OutputDebugString(_T("Key>> UP\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_UP;

		// 方向キー下
		} else if (dijs.lY == 1000) {
			OutputDebugString(_T("Key>> DOWN\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_DOWN;

		// 方向キー左
		} else if (dijs.lX == -1000) {
			OutputDebugString(_T("Key>> LEFT\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_LEFT;

		// 方向キー右
		} else if (dijs.lX == 1000) {
			OutputDebugString(_T("Key>> RIGHT\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.BUTTON_RIGHT;

		// 左スティック
		} else if (dijs.rgdwPOV[0] == 0) {
			OutputDebugString(_T("Stick L>> 0\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.STICK_L_UP;

		} else if (dijs.rgdwPOV[0] == 9000) {
			OutputDebugString(_T("Stick L>> 90\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.STICK_L_RIGHT;

		} else if (dijs.rgdwPOV[0] == 18000) {
			OutputDebugString(_T("Stick L>> 180\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.STICK_L_DOWN;

		} else if (dijs.rgdwPOV[0] == 27000) {
			OutputDebugString(_T("Stick L>> 270\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.STICK_L_LEFT;

		// 右スティック
		} else if (dijs.lZ == -1000) {
			OutputDebugString(_T("Stick R>> -1000(left)\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.STICK_R_LEFT;

		} else if (dijs.lZ == 1000) {
			OutputDebugString(_T("Stick R>> 1000(right)\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.STICK_R_RIGHT;

		} else if (dijs.lRz == -1000) {
			OutputDebugString(_T("Stick R>> -1000(up)\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.STICK_R_LEFT;

		} else if (dijs.lRz == 1000) {
			OutputDebugString(_T("Stick R>> 1000(down)\n"));
			func = (void (*)(I4C3DDIContext*))g_CommandSet.STICK_R_RIGHT;

		//} else {
		//	// dijs.lRz　1000と-1000で上下が見分けられそうだ
		//	TCHAR buffer[32];
		//	//_stprintf_s(buffer, _T("R>> %d %d %d : %d %d %d\n"), dijs.lARx, dijs.lARy, dijs.lARz, dijs.lAX, dijs.lAY, dijs.lAZ);
		//	//_stprintf_s(buffer, _T("R>> %d %d %d : %d %d %d\n"), dijs.lFRx, dijs.lFRy, dijs.lFRz, dijs.lFX, dijs.lFY, dijs.lFZ);
		//	//_stprintf_s(buffer, _T("R>> %d %d %d : %d %d %d\n"), dijs.lRx, dijs.lRy, dijs.lRz, dijs.lVRx, dijs.lVRy, dijs.lVRz);
		//	_stprintf_s(buffer, _T("R>> %d : %d\n"), dijs.lRz, dijs.lZ);	// dijs.lRz:1000->下 -1000->上	// dijs.lZ:1000->右 -1000->左
		//	OutputDebugString(buffer);
		}

		if (func != NULL) {
			(*func)(pContext);
		}

	} else if ( hr == DIERR_INPUTLOST ) {
		g_pDIDev->Acquire();
		OutputDebugString(_T("g_pDIDev->Acquire()\n"));
	}
}