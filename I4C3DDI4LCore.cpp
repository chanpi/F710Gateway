#include "StdAfx.h"
#include "I4C3DDI4LAccessor.h"
#include "I4C3DDI4LAnalyzeXML.h"
#include "I4C3DDI4LCore.h"
#include "I4C3DDI4LControl.h"
#include "I4C3DDI4LModulesDefs.h"
#include "Miscellaneous.h"
#include <dinput.h>
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace {

#define CAMERA_UP_ON			1
#define CAMERA_DOWN_ON			2
#define CAMERA_LEFT_ON			4
#define CAMERA_RIGHT_ON			8
#define GO_UP_ON				16
#define GO_DOWN_ON				32
#define GO_LEFT_ON				64
#define GO_RIGHT_ON				128

#define CAMERA_MASK				15		// 00001111
#define GO_MASK					240		// 11110000

#define CAMERA_UP_LEFT_ON		5
#define CAMERA_UP_RIGHT_ON		9
#define CAMERA_DOWN_LEFT_ON		6
#define CAMERA_DOWN_RIGHT_ON	10
#define GO_UP_LEFT_ON			80
#define GO_UP_RIGHT_ON			144
#define GO_DOWN_LEFT_ON			96
#define GO_DOWN_RIGHT_ON		160

#define SAFE_RELEASE(p)		{ if (p) { (p)->Release(); (p)=NULL; } }

static void SearchAnimationByKey(LPSTR destination, LPCTSTR key);

static LPDIRECTINPUT8 g_pDInput			= NULL;	// DirectInput
static LPDIRECTINPUTDEVICE8 g_pDIDev	= NULL;	// DirectInput
static DIDEVCAPS g_diDevCaps;					// ジョイスティックの能力
static I4C3DDI4LCommandSet g_CommandSet	= {0};

static bool success = false;
const int STICK_THRESHOLD	= 550;

static map<wstring, LPVOID> g_FunctionMap;		// 実行すべき処理とその関数を保存します(例：CAMERA_UP:CameraUp関数へのポインタ)

};

static BOOL CALLBACK I4C3DDI4LEnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* /*pContext*/ );
static BOOL CALLBACK I4C3DDI4LEnumAxesCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID /*pvRef*/ );

I4C3DDI4LCore::I4C3DDI4LCore(void)
{
}


I4C3DDI4LCore::~I4C3DDI4LCore(void)
{
}

BOOL I4C3DDI4LCore::Start(I4C3DDI4LContext* pContext)
{
	// FunctionMapを初期化
	InitializeFunctionMap(pContext);
	// 設定ファイルを読む
	ReadConfigurationFile(pContext);
	// DirectInputの初期化

	if (!InitializeDirectInput(pContext)) {
		return FALSE;
	}

	pContext->pCommandSet = &g_CommandSet;

	USHORT uI4C3DModulePort = 0;
	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(_T("port")), _T("%hu"), &uI4C3DModulePort);

	I4C3DDI4LAccessor accessor;
	pContext->sender = accessor.InitializeTCPSocket((sockaddr_in*)&pContext->address, "127.0.0.1", TRUE, uI4C3DModulePort);
	if (pContext->sender == INVALID_SOCKET) {
		LogDebugMessage(Log_Error, _T("InitializeSocket <I4C3DDI4LModules::PrepareTargetController>"));
		return FALSE;
	}
	if (!accessor.SetConnectingSocket(pContext->sender, &pContext->address)) {
		return FALSE;
	}
	return TRUE;
}

void I4C3DDI4LCore::Stop(I4C3DDI4LContext* pContext)
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
void I4C3DDI4LCore::UnInitialize(I4C3DDI4LContext* pContext)
{
	// ソケットクローズ
	if (pContext->sender != INVALID_SOCKET) {
		closesocket(pContext->sender);
		pContext->sender = INVALID_SOCKET;
	}
}

BOOL I4C3DDI4LCore::InitializeDirectInput(I4C3DDI4LContext* pContext)
{
	HRESULT hr;

	if (success) {
		return TRUE;
	}

	// DirectInputの作成
	hr = DirectInput8Create( pContext->hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDInput, NULL );
	
	if ( FAILED( hr ) ) {
		ReportError(_T("[ERROR] DirectInput8Create()"));
		LogDebugMessage(Log_Error, _T("[ERROR] DirectInput8Create()"));
		return FALSE;
	}

	// デバイスを列挙して作成
	hr = g_pDInput->EnumDevices( DI8DEVCLASS_GAMECTRL, I4C3DDI4LEnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY );
	if ( FAILED( hr ) || g_pDIDev == NULL ) {
		ReportError(_T("[ERROR] EnumDevices()"));
		LogDebugMessage(Log_Error, _T("[ERROR] EnumDevices()"));
		return FALSE;
	}

	// データ形式を設定
	hr = g_pDIDev->SetDataFormat( &c_dfDIJoystick2 );
	if ( FAILED( hr ) ) {
		ReportError(_T("[ERROR] SetDataFormat()"));
		LogDebugMessage(Log_Error, _T("[ERROR] SetDataFormat()"));
		return FALSE;
	}

	// 協調モードを設定（バックグラウンド＆非排他モード）
	hr = g_pDIDev->SetCooperativeLevel( pContext->hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND );
	if ( FAILED( hr ) ) {
		ReportError(_T("[ERROR] SetCooperativeLevel()"));
		LogDebugMessage(Log_Error, _T("[ERROR] SetCooperativeLevel()"));
		return FALSE;
	}

	// コールバック関数を使って各軸のモードを設定
	hr = g_pDIDev->EnumObjects( I4C3DDI4LEnumAxesCallback, NULL, DIDFT_AXIS );
	if ( FAILED( hr ) ) {
		ReportError(_T("[ERROR] EnumObjects()"));
		LogDebugMessage(Log_Error, _T("[ERROR] EnumObjects()"));
		return FALSE;
	}
	// 入力制御開始
	g_pDIDev->Acquire();

	success = true;
	return TRUE;
}

// コマンドとその関数とをマップに格納
void I4C3DDI4LCore::InitializeFunctionMap(const I4C3DDI4LContext* pContext)
{
	static bool success = false;

	if (success) {
		return;
	}

	// mapは同一キーの場合上書きされる
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_GO_FORWARD, &pContext->pController->GoForward) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_GO_BACKWARD, &pContext->pController->GoBackward) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_GO_UP, &pContext->pController->GoUp) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_GO_DOWN, &pContext->pController->GoDown) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_GO_LEFT, &pContext->pController->GoLeft) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_GO_RIGHT, &pContext->pController->GoRight) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_CAMERA_UP, &pContext->pController->CameraUp) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_CAMERA_DOWN, &pContext->pController->CameraDown) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_CAMERA_LEFT, &pContext->pController->CameraLeft) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_CAMERA_RIGHT, &pContext->pController->CameraRight) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_SPEED_UP, &pContext->pController->ChangeSpeed) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO1, &pContext->pController->PlayMacro1) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO2, &pContext->pController->PlayMacro2) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO3, &pContext->pController->PlayMacro3) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO4, &pContext->pController->PlayMacro4) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO5, &pContext->pController->PlayMacro5) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO6, &pContext->pController->PlayMacro6) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO7, &pContext->pController->PlayMacro7) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO8, &pContext->pController->PlayMacro8) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO9, &pContext->pController->PlayMacro9) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO10, &pContext->pController->PlayMacro10) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO11, &pContext->pController->PlayMacro11) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO12, &pContext->pController->PlayMacro12) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO13, &pContext->pController->PlayMacro13) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO14, &pContext->pController->PlayMacro14) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO15, &pContext->pController->PlayMacro15) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO16, &pContext->pController->PlayMacro16) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO17, &pContext->pController->PlayMacro17) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO18, &pContext->pController->PlayMacro18) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO19, &pContext->pController->PlayMacro19) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO20, &pContext->pController->PlayMacro20) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO21, &pContext->pController->PlayMacro21) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO22, &pContext->pController->PlayMacro22) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO23, &pContext->pController->PlayMacro23) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO24, &pContext->pController->PlayMacro24) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO25, &pContext->pController->PlayMacro25) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO26, &pContext->pController->PlayMacro26) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO27, &pContext->pController->PlayMacro27) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO28, &pContext->pController->PlayMacro28) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO29, &pContext->pController->PlayMacro29) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO30, &pContext->pController->PlayMacro30) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO31, &pContext->pController->PlayMacro31) );
	g_FunctionMap.insert( map<wstring, LPVOID>::value_type(COMMAND_MACRO32, &pContext->pController->PlayMacro32) );

	success = true;
}

LPVOID I4C3DDI4LCore::SearchFunctionByKey(LPCTSTR tempKey)
{
	wstring key = (tempKey == NULL) ? _T("") : tempKey;
	if (!key.empty()) {
		map<wstring, LPVOID>::iterator it = g_FunctionMap.find(key);
		if (it != g_FunctionMap.end()) {
			return it->second;
		}
	}
	return NULL;
}

void I4C3DDI4LCore::ReadConfigurationFile(I4C3DDI4LContext* pContext)
{
	g_CommandSet.BUTTON_X = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_X));
	g_CommandSet.BUTTON_A = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_A));
	g_CommandSet.BUTTON_B = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_B));
	g_CommandSet.BUTTON_Y = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_Y));
	g_CommandSet.BUTTON_LB = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_LB));
	g_CommandSet.BUTTON_LT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_LT));
	g_CommandSet.BUTTON_RB = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_RB));
	g_CommandSet.BUTTON_RT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_RT));
	g_CommandSet.BUTTON_BACK = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_BACK));
	g_CommandSet.BUTTON_START = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_START));
	g_CommandSet.BUTTON_LEFTSTICK = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_LEFTSTICK));
	g_CommandSet.BUTTON_RIGHTSTICK = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_RIGHTSTICK));

	// 十字キー
	g_CommandSet.BUTTON_UP = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_UP));
	g_CommandSet.BUTTON_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_DOWN));
	g_CommandSet.BUTTON_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_LEFT));
	g_CommandSet.BUTTON_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_RIGHT));
	g_CommandSet.BUTTON_UPRIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_UPRIGHT));
	g_CommandSet.BUTTON_UPLEFT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_UPLEFT));
	g_CommandSet.BUTTON_DOWNRIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_DOWNRIGHT));
	g_CommandSet.BUTTON_DOWNLEFT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(BUTTON_DOWNLEFT));

	g_CommandSet.STICK_L_UP = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(STICK_L_UP));
	g_CommandSet.STICK_L_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(STICK_L_DOWN));
	g_CommandSet.STICK_L_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(STICK_L_LEFT));
	g_CommandSet.STICK_L_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(STICK_L_RIGHT));
	g_CommandSet.STICK_R_UP = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(STICK_R_UP));
	g_CommandSet.STICK_R_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(STICK_R_DOWN));
	g_CommandSet.STICK_R_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(STICK_R_LEFT));
	g_CommandSet.STICK_R_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetGlobalValue(STICK_R_RIGHT));

	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(TUMBLE_DELTA), _T("%d"), &g_CommandSet.TUMBLE_DELTA);
	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(TRACK_DELTA), _T("%d"), &g_CommandSet.TRACK_DELTA);
	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(DOLLY_DELTA), _T("%d"), &g_CommandSet.DOLLY_DELTA);
}

// ジョイスティックを列挙する関数
BOOL CALLBACK I4C3DDI4LEnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* /*pContext*/ )
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
BOOL CALLBACK I4C3DDI4LEnumAxesCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID /*pvRef*/ )
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
void I4C3DDI4LCore::CheckInput(I4C3DDI4LContext* pContext)
{
	DIJOYSTATE2 dijs = {0};
	HRESULT hr;
	vector<LPVOID> functionList;

	BYTE directionFlag = 0;

	if (!success) {
		return;
	}

	hr = g_pDIDev->GetDeviceState( sizeof( DIJOYSTATE2 ), &dijs );
	if ( SUCCEEDED( hr ) ) {
		// Xボタン
		if (dijs.rgbButtons[0] & 0x80) {
			OutputDebugString(_T("Key>> X\n"));
			if (g_CommandSet.buttons[0] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_X);
			}
		}
		// Aボタン
		if (dijs.rgbButtons[1] & 0x80) {
			OutputDebugString(_T("Key>> A\n"));
			if (g_CommandSet.buttons[1] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_A);
			}
		}
		// Bボタン
		if (dijs.rgbButtons[2] & 0x80) {
			OutputDebugString(_T("Key>> B\n"));
			if (g_CommandSet.buttons[2] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_B);
			}
		}
		// Yボタン
		if (dijs.rgbButtons[3] & 0x80) {
			OutputDebugString(_T("Key>> Y\n"));
			if (g_CommandSet.buttons[3] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_Y);
			}
		}
		// LBボタン
		if (dijs.rgbButtons[4] & 0x80) {
			OutputDebugString(_T("Key>> LB\n"));
			if (g_CommandSet.buttons[4] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_LB);
			}
		}
		// RBボタン
		if (dijs.rgbButtons[5] & 0x80) {
			OutputDebugString(_T("Key>> RB\n"));
			if (g_CommandSet.buttons[5] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_RB);
			}
		}
		// LTボタン
		if (dijs.rgbButtons[6] & 0x80) {
			OutputDebugString(_T("Key>> LT\n"));
			if (g_CommandSet.buttons[6] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_LT);
			}
		}
		// RTボタン
		if (dijs.rgbButtons[7] & 0x80) {
			OutputDebugString(_T("Key>> RT\n"));
			if (g_CommandSet.buttons[7] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_RT);
			}
		}
		// BACKボタン
		if (dijs.rgbButtons[8] & 0x80) {
			OutputDebugString(_T("Key>> BACK\n"));
			if (g_CommandSet.buttons[8] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_BACK);
			}
		}
		// STARTボタン
		if (dijs.rgbButtons[9] & 0x80) {
			OutputDebugString(_T("Key>> START\n"));
			if (g_CommandSet.buttons[9] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_START);
			}
		}

		// 左スティックをボタンとして押す
		if (dijs.rgbButtons[10] & 0x80) {
			OutputDebugString(_T("Key>> LeftStickButton\n"));
			if (g_CommandSet.buttons[10] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_LEFTSTICK);
			}
		}
		// 右スティックをボタンとして押す
		if (dijs.rgbButtons[11] & 0x80) {
			OutputDebugString(_T("Key>> RightStickButton\n"));
			if (g_CommandSet.buttons[11] & 0x80) {
				functionList.push_back(g_CommandSet.BUTTON_RIGHTSTICK);
			}
		}


		// 方向キー上
		if (dijs.rgdwPOV[0] == 0) {
			OutputDebugString(_T("Key>> UP\n"));
			functionList.push_back(g_CommandSet.BUTTON_UP);
		// 方向キー右上
		} else if (dijs.rgdwPOV[0] == 4500) {
			OutputDebugString(_T("Key>> UP-RIGHT\n"));
			functionList.push_back(g_CommandSet.BUTTON_UPRIGHT);
		// 方向キー右
		} else if (dijs.rgdwPOV[0] == 9000) {
			OutputDebugString(_T("Key>> RIGHT\n"));
			functionList.push_back(g_CommandSet.BUTTON_RIGHT);
		// 方向キー右下
		} else if (dijs.rgdwPOV[0] == 13500) {
			OutputDebugString(_T("Key>> DOWN-RIGHT\n"));
			functionList.push_back(g_CommandSet.BUTTON_DOWNRIGHT);
		// 方向キー下
		} else if (dijs.rgdwPOV[0] == 18000) {
			OutputDebugString(_T("Key>> DOWN\n"));
			functionList.push_back(g_CommandSet.BUTTON_DOWN);
		// 方向キー左下
		} else if (dijs.rgdwPOV[0] == 22500) {
			OutputDebugString(_T("Key>> DOWN-LEFT\n"));
			functionList.push_back(g_CommandSet.BUTTON_DOWNLEFT);
		// 方向キー左
		} else if (dijs.rgdwPOV[0] == 27000) {
			OutputDebugString(_T("Key>> LEFT\n"));
			functionList.push_back(g_CommandSet.BUTTON_LEFT);
		// 方向キー左上
		} else if (dijs.rgdwPOV[0] == 31500) {
			OutputDebugString(_T("Key>> UP-LEFT\n"));
			functionList.push_back(g_CommandSet.BUTTON_UPLEFT);
		}

		// 左スティック
		if (dijs.lY <= -STICK_THRESHOLD) {
			OutputDebugString(_T("Stick L>> 0\n"));
			functionList.push_back(g_CommandSet.STICK_L_UP);
		} else if (dijs.lY >= STICK_THRESHOLD) {
			OutputDebugString(_T("Stick L>> 180\n"));
			functionList.push_back(g_CommandSet.STICK_L_DOWN);
		}
		
		if (dijs.lX <= -STICK_THRESHOLD) {
			OutputDebugString(_T("Stick L>> 270\n"));
			functionList.push_back(g_CommandSet.STICK_L_LEFT);

		} else if (dijs.lX >= STICK_THRESHOLD) {
			OutputDebugString(_T("Stick L>> 90\n"));
			functionList.push_back(g_CommandSet.STICK_L_RIGHT);
		}

		// 右スティック
		if (dijs.lZ <= -STICK_THRESHOLD) {
			OutputDebugString(_T("Stick R>> -1000(left)\n"));
			functionList.push_back(g_CommandSet.STICK_R_LEFT);

		} else if (dijs.lZ >= STICK_THRESHOLD) {
			OutputDebugString(_T("Stick R>> 1000(right)\n"));
			functionList.push_back(g_CommandSet.STICK_R_RIGHT);

		}
		
		if (dijs.lRz <= -STICK_THRESHOLD) {
			OutputDebugString(_T("Stick R>> -1000(up)\n"));
			functionList.push_back(g_CommandSet.STICK_R_UP);

		} else if (dijs.lRz >= STICK_THRESHOLD) {
			OutputDebugString(_T("Stick R>> 1000(down)\n"));
			functionList.push_back(g_CommandSet.STICK_R_DOWN);

		}

		if (!functionList.empty()) {
			vector<LPVOID>::iterator it = functionList.begin();
			void (*func)(I4C3DDI4LContext*) = NULL;
			for (; it != functionList.end(); it++) {
				func = (void (*)(I4C3DDI4LContext*))*it;

				// 斜め向き対応
				if (CheckFunctionType(func, pContext, &directionFlag)) {
					continue;
				}

				if (func != NULL) {
					(*func)(pContext);
				}
			}

			// directionFlagをチェックし、カメラコマンドと並行移動コマンドを実行する
			KickFunctionByType(pContext, directionFlag);
			pContext->pController->NormalSpeed(pContext);
		}
		// 今回のボタンの状態を保存（チャタリング防止のための措置）
		memcpy_s(g_CommandSet.buttons, sizeof(g_CommandSet.buttons), dijs.rgbButtons, _countof(dijs.rgbButtons));

	} else if ( hr == DIERR_INPUTLOST || hr == DIERR_UNPLUGGED ) {
		g_pDIDev->Acquire();
		LogDebugMessage(Log_Debug, _T("g_pDIDev->Acquire()\n"));
	}
}

BOOL I4C3DDI4LCore::CheckFunctionType(void (*func)(I4C3DDI4LContext*), I4C3DDI4LContext* pContext, BYTE* flag)
{
	BOOL ret = TRUE;
	if (func == pContext->pController->CameraUp) {
		*flag |= CAMERA_UP_ON;
	} else if (func == pContext->pController->CameraDown) {
		*flag |= CAMERA_DOWN_ON;
	} else if (func == pContext->pController->CameraLeft) {
		*flag |= CAMERA_LEFT_ON;
	} else if (func == pContext->pController->CameraRight) {
		*flag |= CAMERA_RIGHT_ON;
	} else if (func == pContext->pController->GoUp) {
		*flag |= GO_UP_ON;
	} else if (func == pContext->pController->GoDown) {
		*flag |= GO_DOWN_ON;
	} else if (func == pContext->pController->GoLeft) {
		*flag |= GO_LEFT_ON;
	} else if (func == pContext->pController->GoRight) {
		*flag |= GO_RIGHT_ON;
	} else {
		ret = FALSE;
	}
	return ret;
}

void I4C3DDI4LCore::KickFunctionByType(I4C3DDI4LContext* pContext, BYTE flag)
{
	switch (flag & CAMERA_MASK) {
	case CAMERA_UP_ON:
		pContext->pController->CameraUp(pContext);
		break;
	case CAMERA_UP_LEFT_ON:
		pContext->pController->CameraUpLeft(pContext);
		break;
	case CAMERA_LEFT_ON:
		pContext->pController->CameraLeft(pContext);
		break;
	case CAMERA_DOWN_LEFT_ON:
		pContext->pController->CameraDownLeft(pContext);
		break;
	case CAMERA_DOWN_ON:
		pContext->pController->CameraDown(pContext);
		break;
	case CAMERA_DOWN_RIGHT_ON:
		pContext->pController->CameraDownRight(pContext);
		break;
	case CAMERA_RIGHT_ON:
		pContext->pController->CameraRight(pContext);
		break;
	case CAMERA_UP_RIGHT_ON:
		pContext->pController->CameraUpRight(pContext);
		break;
	}

	switch (flag & GO_MASK) {
	case GO_UP_ON:
		return pContext->pController->GoUp(pContext);
	case GO_UP_LEFT_ON:
		return pContext->pController->GoUpLeft(pContext);
	case GO_LEFT_ON:
		return pContext->pController->GoLeft(pContext);
	case GO_DOWN_LEFT_ON:
		return pContext->pController->GoDownLeft(pContext);
	case GO_DOWN_ON:
		return pContext->pController->GoDown(pContext);
	case GO_DOWN_RIGHT_ON:
		return pContext->pController->GoDownRight(pContext);
	case GO_RIGHT_ON:
		return pContext->pController->GoRight(pContext);
	case GO_UP_RIGHT_ON:
		return pContext->pController->GoUpRight(pContext);
	}
}