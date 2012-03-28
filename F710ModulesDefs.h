#pragma once

#include "stdafx.h"

class F710AnalyzeXML;
class F710AbstractControl;

#define MAX_LOADSTRING			100
#define I4C3D_BUFFER_SIZE		256

#define _STR(x)     #x
#define _STR2(x)    _STR(x)
#define __SLINE__   _STR2(__LINE__)
#define HERE        __FILE__ "("__SLINE__")"

#if DEBUG || _DEBUG
#define DEBUG_PROFILE_MONITOR	DebugProfileMonitor(HERE);
#else
#define DEBUG_PROFILE_MONITOR
#endif

const PCTSTR COMMAND_LIST			= _T("COMMAND_LIST");
const PCTSTR COMMAND_GO_FORWARD		= _T("GO_FORWARD");
const PCTSTR COMMAND_GO_BACKWARD	= _T("GO_BACKWARD");
const PCTSTR COMMAND_GO_UP			= _T("GO_UP");
const PCTSTR COMMAND_GO_DOWN		= _T("GO_DOWN");
const PCTSTR COMMAND_GO_LEFT		= _T("GO_LEFT");
const PCTSTR COMMAND_GO_RIGHT		= _T("GO_RIGHT");
const PCTSTR COMMAND_CAMERA_UP		= _T("CAMERA_UP");
const PCTSTR COMMAND_CAMERA_DOWN	= _T("CAMERA_DOWN");
const PCTSTR COMMAND_CAMERA_LEFT	= _T("CAMERA_LEFT");
const PCTSTR COMMAND_CAMERA_RIGHT	= _T("CAMERA_RIGHT");
const PCTSTR COMMAND_SPEED_UP		= _T("SPEED_UP");
const PCTSTR COMMAND_MACRO1			= _T("MACRO1");
const PCTSTR COMMAND_MACRO2			= _T("MACRO2");
const PCTSTR COMMAND_MACRO3			= _T("MACRO3");
const PCTSTR COMMAND_MACRO4			= _T("MACRO4");
const PCTSTR COMMAND_MACRO5			= _T("MACRO5");
const PCTSTR COMMAND_MACRO6			= _T("MACRO6");
const PCTSTR COMMAND_MACRO7			= _T("MACRO7");
const PCTSTR COMMAND_MACRO8			= _T("MACRO8");
const PCTSTR COMMAND_MACRO9			= _T("MACRO9");
const PCTSTR COMMAND_MACRO10		= _T("MACRO10");
const PCTSTR COMMAND_MACRO11		= _T("MACRO11");
const PCTSTR COMMAND_MACRO12		= _T("MACRO12");
const PCTSTR COMMAND_MACRO13		= _T("MACRO13");
const PCTSTR COMMAND_MACRO14		= _T("MACRO14");
const PCTSTR COMMAND_MACRO15		= _T("MACRO15");
const PCTSTR COMMAND_MACRO16		= _T("MACRO16");
const PCTSTR COMMAND_MACRO17		= _T("MACRO17");
const PCTSTR COMMAND_MACRO18		= _T("MACRO18");
const PCTSTR COMMAND_MACRO19		= _T("MACRO19");
const PCTSTR COMMAND_MACRO20		= _T("MACRO20");
const PCTSTR COMMAND_MACRO21		= _T("MACRO21");
const PCTSTR COMMAND_MACRO22		= _T("MACRO22");
const PCTSTR COMMAND_MACRO23		= _T("MACRO23");
const PCTSTR COMMAND_MACRO24		= _T("MACRO24");
const PCTSTR COMMAND_MACRO25		= _T("MACRO25");
const PCTSTR COMMAND_MACRO26		= _T("MACRO26");
const PCTSTR COMMAND_MACRO27		= _T("MACRO27");
const PCTSTR COMMAND_MACRO28		= _T("MACRO28");
const PCTSTR COMMAND_MACRO29		= _T("MACRO29");
const PCTSTR COMMAND_MACRO30		= _T("MACRO30");
const PCTSTR COMMAND_MACRO31		= _T("MACRO31");
const PCTSTR COMMAND_MACRO32		= _T("MACRO32");

const PCTSTR BUTTON_X				= _T("BUTTON_X");
const PCTSTR BUTTON_A				= _T("BUTTON_A");
const PCTSTR BUTTON_B				= _T("BUTTON_B");
const PCTSTR BUTTON_Y				= _T("BUTTON_Y");
const PCTSTR BUTTON_LB				= _T("BUTTON_LB");
const PCTSTR BUTTON_LT				= _T("BUTTON_LT");
const PCTSTR BUTTON_RB				= _T("BUTTON_RB");
const PCTSTR BUTTON_RT				= _T("BUTTON_RT");
const PCTSTR BUTTON_BACK			= _T("BUTTON_BACK");
const PCTSTR BUTTON_START			= _T("BUTTON_START");
const PCTSTR BUTTON_LEFTSTICK		= _T("BUTTON_LEFTSTICK");
const PCTSTR BUTTON_RIGHTSTICK		= _T("BUTTON_RIGHTSTICK");
const PCTSTR BUTTON_UP				= _T("BUTTON_UP");
const PCTSTR BUTTON_DOWN			= _T("BUTTON_DOWN");
const PCTSTR BUTTON_LEFT			= _T("BUTTON_LEFT");
const PCTSTR BUTTON_RIGHT			= _T("BUTTON_RIGHT");
const PCTSTR BUTTON_UPRIGHT			= _T("BUTTON_UPRIGHT");
const PCTSTR BUTTON_UPLEFT			= _T("BUTTON_UPLEFT");
const PCTSTR BUTTON_DOWNRIGHT		= _T("BUTTON_DOWNRIGHT");
const PCTSTR BUTTON_DOWNLEFT		= _T("BUTTON_DOWNLEFT");

const PCTSTR STICK_L_UP				= _T("STICK_L_UP");
const PCTSTR STICK_L_DOWN			= _T("STICK_L_DOWN");
const PCTSTR STICK_L_LEFT			= _T("STICK_L_LEFT");
const PCTSTR STICK_L_RIGHT			= _T("STICK_L_RIGHT");
const PCTSTR STICK_R_UP				= _T("STICK_R_UP");
const PCTSTR STICK_R_DOWN			= _T("STICK_R_DOWN");
const PCTSTR STICK_R_LEFT			= _T("STICK_R_LEFT");
const PCTSTR STICK_R_RIGHT			= _T("STICK_R_RIGHT");

const PCTSTR TUMBLE_DELTA			= _T("TUMBLE_DELTA");
const PCTSTR TRACK_DELTA			= _T("TRACK_DELTA");
const PCTSTR DOLLY_DELTA			= _T("DOLLY_DELTA");
const PCTSTR CONTENTS				= _T("CONTENTS");
const PCTSTR SHIFT_TRANSMISSION		= _T("SHIFT_TRANSMISSION");

struct F710Context;
typedef void (F710AbstractControl::*pControlFunc)(struct F710Context*);

typedef struct F710CommandSet {
	pControlFunc BUTTON_X;
	pControlFunc BUTTON_A;
	pControlFunc BUTTON_B;
	pControlFunc BUTTON_Y;
	pControlFunc BUTTON_LB;
	pControlFunc BUTTON_LT;
	pControlFunc BUTTON_RB;
	pControlFunc BUTTON_RT;
	pControlFunc BUTTON_BACK;
	pControlFunc BUTTON_START;
	pControlFunc BUTTON_LEFTSTICK;
	pControlFunc BUTTON_RIGHTSTICK;

	// 十字キー
	pControlFunc BUTTON_UP;
	pControlFunc BUTTON_DOWN;
	pControlFunc BUTTON_LEFT;
	pControlFunc BUTTON_RIGHT;
	pControlFunc BUTTON_UPRIGHT;
	pControlFunc BUTTON_UPLEFT;
	pControlFunc BUTTON_DOWNRIGHT;
	pControlFunc BUTTON_DOWNLEFT;

	// 左アナログスティック
	pControlFunc STICK_L_UP;
	pControlFunc STICK_L_DOWN;
	pControlFunc STICK_L_LEFT;
	pControlFunc STICK_L_RIGHT;

	// 右アナログスティック
	pControlFunc STICK_R_UP;
	pControlFunc STICK_R_DOWN;
	pControlFunc STICK_R_LEFT;
	pControlFunc STICK_R_RIGHT;

	int TUMBLE_DELTA;
	int TRACK_DELTA;
	int DOLLY_DELTA;
	
	float SHIFT_TRANSMISSION;	// 変速
	float speed;
	BYTE buttons[128];
} F710CommandSet;

typedef struct F710Context {
	F710AnalyzeXML* pAnalyzer;
	BOOL bRTT4ECMode;
	F710AbstractControl* pController;
	HINSTANCE hInst;
	HWND hWnd;

	SOCKET sender;
	struct sockaddr_in address;

	F710CommandSet* pCommandSet;
} F710Context;