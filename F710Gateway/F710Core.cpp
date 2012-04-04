#include "StdAfx.h"
#include "F710Accessor.h"
#include "F710AnalyzeXML.h"
#include "F710Core.h"
#include "F710AbstractControl.h"
#include "F710Control.h"
#include "F710TCPControl.h"
#include "F710ModulesDefs.h"
#include "Misc.h"
#include "MemoryLeak.h"
#include "SharedConstants.h"
#include <dinput.h>
#include <vector>
#include <map>
using namespace std;

#if UNICODE || _UNICODE
static LPCTSTR g_FILE = __FILEW__;
#else
static LPCTSTR g_FILE = __FILE__;
#endif

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
static DIDEVCAPS g_diDevCaps			= {0};	// ジョイスティックの能力
static F710CommandSet g_CommandSet		= {0};

static map<tstring, pControlFunc> g_FunctionMap; // 実行すべき処理とその関数を保存します(例：CAMERA_UP:CameraUp関数へのポインタ)

const int STICK_THRESHOLD	= 550;
static PCTSTR g_szSoftwareName = _T("F710Gateway");
};

extern int F710ExitCode;

static BOOL CALLBACK F710EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* /*pContext*/ );
static BOOL CALLBACK F710EnumAxesCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID /*pvRef*/ );

F710Core::F710Core(void)
{
}


F710Core::~F710Core(void)
{
}

BOOL F710Core::Start(F710Context* pContext)
{
	// FunctionMapを初期化
	InitializeFunctionMap();
	// 設定ファイルを読む
	ReadConfigurationFile(pContext);
	// DirectInputの初期化

	if (!InitializeDirectInput(pContext)) {
		return FALSE;
	}

	pContext->pCommandSet = &g_CommandSet;

	USHORT uI4C3DModulePort = 0;
	_stscanf_s(pContext->pAnalyzer->GetGlobalValue(_T("port")), _T("%hu"), &uI4C3DModulePort);

	F710Accessor accessor;

	// 接続の前に２秒程度待つ(I4C3DModulesが先に起動するのを確実に待つため)
	Sleep(2000);

	pContext->sender = accessor.InitializeTCPSocket((sockaddr_in*)&pContext->address, "127.0.0.1", TRUE, uI4C3DModulePort);
	if (pContext->sender == INVALID_SOCKET) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_SOCKET_INVALID), GetLastError(), g_FILE, __LINE__);
		F710ExitCode = EXIT_SOCKET_ERROR;
		return FALSE;
	}
	if (!accessor.SetConnectingSocket(pContext->sender, &pContext->address)) {
		F710ExitCode = EXIT_CORE_CONNECT_ERROR;
		return FALSE;
	}
	return TRUE;
}

void F710Core::Stop(F710Context* pContext)
{
	UnInitialize(pContext);
	UnInitializeDirectInput(pContext);
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
void F710Core::UnInitialize(F710Context* pContext)
{
	// ソケットクローズ
	if (pContext->sender != INVALID_SOCKET) {
		closesocket(pContext->sender);
		pContext->sender = INVALID_SOCKET;
	}
	g_FunctionMap.clear();
}

BOOL F710Core::InitializeDirectInput(F710Context* pContext)
{
	HRESULT hr;

	// DirectInputの作成
	hr = DirectInput8Create( pContext->hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDInput, NULL );
	
	if ( FAILED( hr ) ) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_DI8_INVALID), GetLastError(), g_FILE, __LINE__);
		UnInitializeDirectInput(pContext);
		F710ExitCode = EXIT_GAMEPAD_SETUP_ERROR;
		return FALSE;
	}

	// デバイスを列挙して作成
	hr = g_pDInput->EnumDevices( DI8DEVCLASS_GAMECTRL, F710EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY );
	if ( FAILED( hr ) || g_pDIDev == NULL ) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_DI8_INVALID), GetLastError(), g_FILE, __LINE__);
		UnInitializeDirectInput(pContext);
		F710ExitCode = EXIT_GAMEPAD_NOT_FOUND;
		return FALSE;
	}

	// データ形式を設定
	hr = g_pDIDev->SetDataFormat( &c_dfDIJoystick2 );
	if ( FAILED( hr ) ) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_DI8_INVALID), GetLastError(), g_FILE, __LINE__);
		UnInitializeDirectInput(pContext);
		F710ExitCode = EXIT_GAMEPAD_SETUP_ERROR;
		return FALSE;
	}

	// 協調モードを設定（バックグラウンド＆非排他モード）
	hr = g_pDIDev->SetCooperativeLevel( pContext->hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND );
	if ( FAILED( hr ) ) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_DI8_INVALID), GetLastError(), g_FILE, __LINE__);
		UnInitializeDirectInput(pContext);
		F710ExitCode = EXIT_GAMEPAD_SETUP_ERROR;
		return FALSE;
	}

	// コールバック関数を使って各軸のモードを設定
	hr = g_pDIDev->EnumObjects( F710EnumAxesCallback, NULL, DIDFT_AXIS );
	if ( FAILED( hr ) ) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_DI8_INVALID), GetLastError(), g_FILE, __LINE__);
		UnInitializeDirectInput(pContext);
		F710ExitCode = EXIT_GAMEPAD_SETUP_ERROR;
		return FALSE;
	}
	// 入力制御開始
	g_pDIDev->Acquire();

	return TRUE;
}

void F710Core::UnInitializeDirectInput(F710Context* /*pContext*/)
{
	if (g_pDIDev) {
		g_pDIDev->Unacquire();
	}
	// デバイスを解放する
	SAFE_RELEASE(g_pDIDev);
	// DirectInputの解放
	SAFE_RELEASE(g_pDInput);
}

// コマンドとその関数とをマップに格納
void F710Core::InitializeFunctionMap(void)
{
	// mapは同一キーの場合上書きされる
	g_FunctionMap[COMMAND_GO_FORWARD] = &F710AbstractControl::GoForward;
	g_FunctionMap[COMMAND_GO_BACKWARD] = &F710AbstractControl::GoBackward;
	g_FunctionMap[COMMAND_GO_UP] = &F710AbstractControl::GoUp;
	g_FunctionMap[COMMAND_GO_DOWN] = &F710AbstractControl::GoDown;
	g_FunctionMap[COMMAND_GO_LEFT] = &F710AbstractControl::GoLeft;
	g_FunctionMap[COMMAND_GO_RIGHT] = &F710AbstractControl::GoRight;
	g_FunctionMap[COMMAND_CAMERA_UP] = &F710AbstractControl::CameraUp;
	g_FunctionMap[COMMAND_CAMERA_DOWN] = &F710AbstractControl::CameraDown;
	g_FunctionMap[COMMAND_CAMERA_LEFT] = &F710AbstractControl::CameraLeft;
	g_FunctionMap[COMMAND_CAMERA_RIGHT] = &F710AbstractControl::CameraRight;
	g_FunctionMap[COMMAND_SPEED_UP] = &F710AbstractControl::ChangeSpeed;
	g_FunctionMap[COMMAND_MACRO1] = &F710AbstractControl::PlayMacro1;
	g_FunctionMap[COMMAND_MACRO2] = &F710AbstractControl::PlayMacro2;
	g_FunctionMap[COMMAND_MACRO3] = &F710AbstractControl::PlayMacro3;
	g_FunctionMap[COMMAND_MACRO4] = &F710AbstractControl::PlayMacro4;
	g_FunctionMap[COMMAND_MACRO5] = &F710AbstractControl::PlayMacro5;
	g_FunctionMap[COMMAND_MACRO6] = &F710AbstractControl::PlayMacro6;
	g_FunctionMap[COMMAND_MACRO7] = &F710AbstractControl::PlayMacro7;
	g_FunctionMap[COMMAND_MACRO8] = &F710AbstractControl::PlayMacro8;
	g_FunctionMap[COMMAND_MACRO9] = &F710AbstractControl::PlayMacro9;
	g_FunctionMap[COMMAND_MACRO10] = &F710AbstractControl::PlayMacro10;
	g_FunctionMap[COMMAND_MACRO11] = &F710AbstractControl::PlayMacro11;
	g_FunctionMap[COMMAND_MACRO12] = &F710AbstractControl::PlayMacro12;
	g_FunctionMap[COMMAND_MACRO13] = &F710AbstractControl::PlayMacro13;
	g_FunctionMap[COMMAND_MACRO14] = &F710AbstractControl::PlayMacro14;
	g_FunctionMap[COMMAND_MACRO15] = &F710AbstractControl::PlayMacro15;
	g_FunctionMap[COMMAND_MACRO16] = &F710AbstractControl::PlayMacro16;
	g_FunctionMap[COMMAND_MACRO17] = &F710AbstractControl::PlayMacro17;
	g_FunctionMap[COMMAND_MACRO18] = &F710AbstractControl::PlayMacro18;
	g_FunctionMap[COMMAND_MACRO19] = &F710AbstractControl::PlayMacro19;
	g_FunctionMap[COMMAND_MACRO20] = &F710AbstractControl::PlayMacro20;
	g_FunctionMap[COMMAND_MACRO21] = &F710AbstractControl::PlayMacro21;
	g_FunctionMap[COMMAND_MACRO22] = &F710AbstractControl::PlayMacro22;
	g_FunctionMap[COMMAND_MACRO23] = &F710AbstractControl::PlayMacro23;
	g_FunctionMap[COMMAND_MACRO24] = &F710AbstractControl::PlayMacro24;
	g_FunctionMap[COMMAND_MACRO25] = &F710AbstractControl::PlayMacro25;
	g_FunctionMap[COMMAND_MACRO26] = &F710AbstractControl::PlayMacro26;
	g_FunctionMap[COMMAND_MACRO27] = &F710AbstractControl::PlayMacro27;
	g_FunctionMap[COMMAND_MACRO28] = &F710AbstractControl::PlayMacro28;
	g_FunctionMap[COMMAND_MACRO29] = &F710AbstractControl::PlayMacro29;
	g_FunctionMap[COMMAND_MACRO30] = &F710AbstractControl::PlayMacro30;
	g_FunctionMap[COMMAND_MACRO31] = &F710AbstractControl::PlayMacro31;
	g_FunctionMap[COMMAND_MACRO32] = &F710AbstractControl::PlayMacro32;
}

pControlFunc F710Core::SearchFunctionByKey(LPCTSTR tempKey)
{
	tstring key = (tempKey == NULL) ? _T("") : tempKey;
	if (!key.empty()) {
		map<tstring, pControlFunc>::iterator it = g_FunctionMap.find(key);
		if (it != g_FunctionMap.end()) {
			return it->second;
		}
	}
	return NULL;
}

void F710Core::ReadConfigurationFile(F710Context* pContext)
{
	g_CommandSet.BUTTON_X = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_X));
	g_CommandSet.BUTTON_A = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_A));
	g_CommandSet.BUTTON_B = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_B));
	g_CommandSet.BUTTON_Y = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_Y));
	g_CommandSet.BUTTON_LB = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_LB));
	g_CommandSet.BUTTON_LT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_LT));
	g_CommandSet.BUTTON_RB = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_RB));
	g_CommandSet.BUTTON_RT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_RT));
	g_CommandSet.BUTTON_BACK = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_BACK));
	g_CommandSet.BUTTON_START = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_START));
	g_CommandSet.BUTTON_LEFTSTICK = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_LEFTSTICK));
	g_CommandSet.BUTTON_RIGHTSTICK = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_RIGHTSTICK));

	// 十字キー
	g_CommandSet.BUTTON_UP = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_UP));
	g_CommandSet.BUTTON_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_DOWN));
	g_CommandSet.BUTTON_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_LEFT));
	g_CommandSet.BUTTON_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_RIGHT));
	g_CommandSet.BUTTON_UPRIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_UPRIGHT));
	g_CommandSet.BUTTON_UPLEFT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_UPLEFT));
	g_CommandSet.BUTTON_DOWNRIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_DOWNRIGHT));
	g_CommandSet.BUTTON_DOWNLEFT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, BUTTON_DOWNLEFT));

	g_CommandSet.STICK_L_UP = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, STICK_L_UP));
	g_CommandSet.STICK_L_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, STICK_L_DOWN));
	g_CommandSet.STICK_L_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, STICK_L_LEFT));
	g_CommandSet.STICK_L_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, STICK_L_RIGHT));
	g_CommandSet.STICK_R_UP = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, STICK_R_UP));
	g_CommandSet.STICK_R_DOWN = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, STICK_R_DOWN));
	g_CommandSet.STICK_R_LEFT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, STICK_R_LEFT));
	g_CommandSet.STICK_R_RIGHT = SearchFunctionByKey(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, STICK_R_RIGHT));

	_stscanf_s(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, TUMBLE_DELTA), _T("%d"), &g_CommandSet.TUMBLE_DELTA);
	_stscanf_s(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, TRACK_DELTA), _T("%d"), &g_CommandSet.TRACK_DELTA);
	_stscanf_s(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, DOLLY_DELTA), _T("%d"), &g_CommandSet.DOLLY_DELTA);
	
	_stscanf_s(pContext->pAnalyzer->GetSoftValue(g_szSoftwareName, SHIFT_TRANSMISSION), _T("%f"), &g_CommandSet.SHIFT_TRANSMISSION);
}

// ジョイスティックを列挙する関数
BOOL CALLBACK F710EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* /*pContext*/ )
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
BOOL CALLBACK F710EnumAxesCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID /*pvRef*/ )
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
void F710Core::CheckInput(F710Context* pContext)
{
	DIJOYSTATE2 dijs = {0};
	HRESULT hr;
	vector<pControlFunc> functionList;

	BYTE directionFlag = 0;

	if (g_pDIDev == NULL) {
		return;
	}

	hr = g_pDIDev->GetDeviceState( sizeof( DIJOYSTATE2 ), &dijs );
	if ( SUCCEEDED( hr ) ) {
		pContext->pController->NormalSpeed(pContext);

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
			vector<pControlFunc>::iterator it = functionList.begin();
			void (F710AbstractControl::*pFunc)(F710Context*) = NULL;
			for (; it != functionList.end(); it++) {
				pFunc = *it;

				// 斜め向き対応
				if (CheckFunctionType(pFunc, &directionFlag)) {
					continue;
				}

				if (pFunc != NULL) {
					(pContext->pController->*pFunc)(pContext);
				}
			}

			// directionFlagをチェックし、カメラコマンドと並行移動コマンドを実行する
			KickFunctionByType(pContext, directionFlag);
		}
		// 今回のボタンの状態を保存（チャタリング防止のための措置）
		memcpy_s(g_CommandSet.buttons, sizeof(g_CommandSet.buttons), dijs.rgbButtons, _countof(dijs.rgbButtons));

	} else if ( hr == DIERR_INPUTLOST ) {
		g_pDIDev->Acquire();
		LoggingMessage(Log_Debug, _T(MESSAGE_ERROR_DI8_UNKNOWN), GetLastError(), g_FILE, __LINE__);
	} else if ( hr == DIERR_UNPLUGGED ) {
		F710ExitCode = EXIT_GAMEPAD_NOT_FOUND;
		return;
	}
}

BOOL F710Core::CheckFunctionType(pControlFunc pFunc, BYTE* flag)
{
	BOOL ret = TRUE;
	if (pFunc == &F710AbstractControl::CameraUp) {
		*flag |= CAMERA_UP_ON;
	} else if (pFunc == &F710AbstractControl::CameraDown) {
		*flag |= CAMERA_DOWN_ON;
	} else if (pFunc == &F710AbstractControl::CameraLeft) {
		*flag |= CAMERA_LEFT_ON;
	} else if (pFunc == &F710AbstractControl::CameraRight) {
		*flag |= CAMERA_RIGHT_ON;
	} else if (pFunc == &F710AbstractControl::GoUp) {
		*flag |= GO_UP_ON;
	} else if (pFunc == &F710AbstractControl::GoDown) {
		*flag |= GO_DOWN_ON;
	} else if (pFunc == &F710AbstractControl::GoLeft) {
		*flag |= GO_LEFT_ON;
	} else if (pFunc == &F710AbstractControl::GoRight) {
		*flag |= GO_RIGHT_ON;
	} else {
		ret = FALSE;
	}
	return ret;
}

void F710Core::KickFunctionByType(F710Context* pContext, BYTE flag)
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