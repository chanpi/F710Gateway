#include "StdAfx.h"
#include "F710TCPControl.h"
#include "RTT4ECCommon.h"
#include "I4C3DCommon.h"
#include "F710AnalyzeXML.h"
#include <math.h>

namespace {
	const float M_PI = 3.14159;
	const LPCTSTR TAG_RTT4EC		= _T("RTT4EC");
	const LPCTSTR TAG_MOVE_DELTA	= _T("move_delta");
	const LPCTSTR TAG_ANGLE_DELTA	= _T("angle_delta");
	const LPCTSTR TAG_PITCH_DELTA	= _T("pitch_delta");
	const LPCTSTR TAG_HEIGHT_DELTA	= _T("height_delta");
	static RTT4ECContext g_rtt4ecContext = {0};
};

F710TCPControl::F710TCPControl(void)
{
}


F710TCPControl::~F710TCPControl(void)
{
}

BOOL F710TCPControl::Initialize(F710Context* pContext, char cTermination)
{
	F710AbstractControl::Initialize(pContext, cTermination);

	// deltaを読み込み
	g_rtt4ecContext.move = 7;
	g_rtt4ecContext.angle = 1;
	g_rtt4ecContext.pitch = 1;
	g_rtt4ecContext.height = 2;
	_stscanf_s(pContext->pAnalyzer->GetSoftValue(TAG_RTT4EC, TAG_MOVE_DELTA), _T("%d"), &g_rtt4ecContext.move);
	_stscanf_s(pContext->pAnalyzer->GetSoftValue(TAG_RTT4EC, TAG_ANGLE_DELTA), _T("%d"), &g_rtt4ecContext.angle);
	_stscanf_s(pContext->pAnalyzer->GetSoftValue(TAG_RTT4EC, TAG_PITCH_DELTA), _T("%d"), &g_rtt4ecContext.pitch);
	_stscanf_s(pContext->pAnalyzer->GetSoftValue(TAG_RTT4EC, TAG_HEIGHT_DELTA), _T("%d"), &g_rtt4ecContext.height);

	// マクロの読み込み
	TCHAR szMacroName[16] = {0};
	PCTSTR szMacroValue = NULL;
	for (int j = 1; j <= MAX_MACROS; j++) {
		_stprintf_s(szMacroName, _countof(szMacroName), _T("MACRO%d"), j);
		szMacroValue = pContext->pAnalyzer->GetSoftValue(TAG_RTT4EC, szMacroName);
		if (szMacroValue == NULL) {
			//break;
			continue;
		}

#if UNICODE || _UNICODE
		char cszMacroValue[BUFFER_SIZE] = {0};
		WideCharToMultiByte(CP_ACP, 0, szMacroValue, _tcslen(szMacroValue), cszMacroValue, sizeof(cszMacroValue), NULL, NULL);
		g_rtt4ecContext.macroMap[szMacroName] = cszMacroValue;
#else
		g_rtt4ecContext.macroMap[szMacroName] = szMacroValue;
#endif
	}
	return TRUE;
}

void F710TCPControl::UnInitialize(void)
{
	g_rtt4ecContext.macroMap.clear();
}


void F710TCPControl::ChangeSpeed(F710Context* /*pContext*/)
{
	g_rtt4ecContext.speed = 2;
}

void F710TCPControl::NormalSpeed(F710Context* /*pContext*/)
{
	g_rtt4ecContext.speed = 1;
}

void F710TCPControl::PlayMacro1(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO1"))].c_str());
}

void F710TCPControl::PlayMacro2(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO2"))].c_str());
}

void F710TCPControl::PlayMacro3(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO3"))].c_str());
}

void F710TCPControl::PlayMacro4(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO4"))].c_str());
}

void F710TCPControl::PlayMacro5(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO5"))].c_str());
}

void F710TCPControl::PlayMacro6(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO6"))].c_str());
}

void F710TCPControl::PlayMacro7(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO7"))].c_str());
}

void F710TCPControl::PlayMacro8(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO8"))].c_str());
}

void F710TCPControl::PlayMacro9(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO9"))].c_str());
}

void F710TCPControl::PlayMacro10(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO10"))].c_str());
}

void F710TCPControl::PlayMacro11(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO11"))].c_str());
}

void F710TCPControl::PlayMacro12(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO12"))].c_str());
}

void F710TCPControl::PlayMacro13(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO13"))].c_str());
}

void F710TCPControl::PlayMacro14(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO14"))].c_str());
}

void F710TCPControl::PlayMacro15(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO15"))].c_str());
}

void F710TCPControl::PlayMacro16(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO16"))].c_str());
}

void F710TCPControl::PlayMacro17(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO17"))].c_str());
}

void F710TCPControl::PlayMacro18(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO18"))].c_str());
}

void F710TCPControl::PlayMacro19(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO19"))].c_str());
}

void F710TCPControl::PlayMacro20(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO20"))].c_str());
}

void F710TCPControl::PlayMacro21(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO21"))].c_str());
}

void F710TCPControl::PlayMacro22(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO22"))].c_str());
}

void F710TCPControl::PlayMacro23(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO23"))].c_str());
}

void F710TCPControl::PlayMacro24(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO24"))].c_str());
}

void F710TCPControl::PlayMacro25(F710Context* pContext) 
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO25"))].c_str());
}

void F710TCPControl::PlayMacro26(F710Context* pContext) 
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO26"))].c_str());
}

void F710TCPControl::PlayMacro27(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO27"))].c_str());
}

void F710TCPControl::PlayMacro28(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO28"))].c_str());
}

void F710TCPControl::PlayMacro29(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO29"))].c_str());
}

void F710TCPControl::PlayMacro30(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO30"))].c_str());
}

void F710TCPControl::PlayMacro31(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO31"))].c_str());
}

void F710TCPControl::PlayMacro32(F710Context* pContext)
{
	PlayMacro(pContext, g_rtt4ecContext.macroMap[wstring(_T("MACRO32"))].c_str());
}

// DOLLY
// 前進（カメラxy）
void F710TCPControl::GoForward(F710Context* pContext)
{
	g_rtt4ecContext.x += g_rtt4ecContext.move * g_rtt4ecContext.speed * sin(M_PI * g_rtt4ecContext.h / 180);
	g_rtt4ecContext.y -= g_rtt4ecContext.move * g_rtt4ecContext.speed * cos(M_PI * g_rtt4ecContext.h / 180);
	ExecuteCameraCommand(pContext);
}

// 後退（カメラxy）
void F710TCPControl::GoBackward(F710Context* pContext)
{
	g_rtt4ecContext.x -= g_rtt4ecContext.move * g_rtt4ecContext.speed * sin(M_PI * g_rtt4ecContext.h / 180);
	g_rtt4ecContext.y += g_rtt4ecContext.move * g_rtt4ecContext.speed * cos(M_PI * g_rtt4ecContext.h / 180);
	ExecuteCameraCommand(pContext);
}
	
// TRACK
void F710TCPControl::GoUp(F710Context* pContext)
{
	g_rtt4ecContext.z += g_rtt4ecContext.height * g_rtt4ecContext.speed;
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::GoDown(F710Context* pContext)
{
	g_rtt4ecContext.z -= g_rtt4ecContext.height * g_rtt4ecContext.speed;
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::GoLeft(F710Context* /*pContext*/)
{
}

void F710TCPControl::GoRight(F710Context* /*pContext*/)
{
}

void F710TCPControl::GoUpRight(F710Context* /*pContext*/)
{
}

void F710TCPControl::GoUpLeft(F710Context* /*pContext*/)
{
}

void F710TCPControl::GoDownRight(F710Context* /*pContext*/)
{
}

void F710TCPControl::GoDownLeft(F710Context* /*pContext*/)
{
}

// TUMBLE
void F710TCPControl::CameraUp(F710Context* pContext)
{
	g_rtt4ecContext.p -= g_rtt4ecContext.pitch * g_rtt4ecContext.speed;
    //if (g_rtt4ecContext.p < -88)
    //{
    //    g_rtt4ecContext.p = -88;
    //    g_rtt4ecContext.r = 0;
    //}
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::CameraDown(F710Context* pContext)
{
	g_rtt4ecContext.p += g_rtt4ecContext.pitch * g_rtt4ecContext.speed;
    //if (g_rtt4ecContext.p > 88)
    //{
    //    g_rtt4ecContext.p = 88;
    //    g_rtt4ecContext.r = 0;
    //}
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::CameraLeft(F710Context* pContext)
{
	g_rtt4ecContext.h += g_rtt4ecContext.angle * g_rtt4ecContext.speed;
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::CameraRight(F710Context* pContext)
{
	g_rtt4ecContext.h -= g_rtt4ecContext.angle * g_rtt4ecContext.speed;
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::CameraUpRight(F710Context* pContext)
{
	g_rtt4ecContext.p -= g_rtt4ecContext.pitch * g_rtt4ecContext.speed;
    //if (g_rtt4ecContext.p < -88)
    //{
    //    g_rtt4ecContext.p = -88;
    //    g_rtt4ecContext.r = 0;
    //}
	g_rtt4ecContext.h -= g_rtt4ecContext.angle * g_rtt4ecContext.speed;
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::CameraUpLeft(F710Context* pContext)
{
	g_rtt4ecContext.p -= g_rtt4ecContext.pitch * g_rtt4ecContext.speed;
    //if (g_rtt4ecContext.p < -88)
    //{
    //    g_rtt4ecContext.p = -88;
    //    g_rtt4ecContext.r = 0;
    //}
	g_rtt4ecContext.h += g_rtt4ecContext.angle * g_rtt4ecContext.speed;
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::CameraDownRight(F710Context* pContext)
{
	g_rtt4ecContext.p += g_rtt4ecContext.pitch * g_rtt4ecContext.speed;
    //if (g_rtt4ecContext.p > 88)
    //{
    //    g_rtt4ecContext.p = 88;
    //    g_rtt4ecContext.r = 0;
    //}
	g_rtt4ecContext.h -= g_rtt4ecContext.angle * g_rtt4ecContext.speed;
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::CameraDownLeft(F710Context* pContext)
{
	g_rtt4ecContext.p += g_rtt4ecContext.pitch * g_rtt4ecContext.speed;
    //if (g_rtt4ecContext.p > 88)
    //{
    //    g_rtt4ecContext.p = 88;
    //    g_rtt4ecContext.r = 0;
    //}
	g_rtt4ecContext.h += g_rtt4ecContext.angle * g_rtt4ecContext.speed;
	ExecuteCameraCommand(pContext);
}

void F710TCPControl::ExecuteCameraCommand(F710Context* pContext)
{
	char message[BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), g_cameraCommandFormat,
		g_rtt4ecContext.x, g_rtt4ecContext.y, g_rtt4ecContext.z,
		g_rtt4ecContext.p, g_rtt4ecContext.h, g_rtt4ecContext.r, m_cTermination);

	Execute(pContext, message);
}
