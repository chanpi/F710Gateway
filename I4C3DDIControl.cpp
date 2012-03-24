#include "StdAfx.h"
#include "I4C3DDIControl.h"
#include "I4C3DDIModulesDefs.h"
#include <vector>
#include <math.h>

using namespace std;

const double M_PI = asin(1.0) * 4.0;

static void CreateCommand(char* buffer, int bufferLen, I4C3DDIContext* pContext);
static CRITICAL_SECTION g_lock;
static char g_cTermination = '?';

I4C3DDIControl::I4C3DDIControl(char cTermination)
{
	g_cTermination = cTermination;
	InitializeCriticalSection(&g_lock);
}


I4C3DDIControl::~I4C3DDIControl(void)
{
	DeleteCriticalSection(&g_lock);
}


/**
 * @brief
 * ipodからの電文をTOPMOSTの3Dソフトに転送します。
 * 
 * @param szCommand
 * ipodから受信した電文。
 * 
 * @param commandLen
 * 電文長。
 * 
 * ipodからの電文をTOPMOSTの3Dソフトに転送します。転送はUDPで行います。
 */
void I4C3DDIControl::Execute(I4C3DDIContext* pContext, const char* message)
{
	EnterCriticalSection(&g_lock);
	send(pContext->sender, message, strlen(message), 0);
	LeaveCriticalSection(&g_lock);
}

///////////////// スピード（移動量）の変更 /////////////////
void I4C3DDIControl::ChangeSpeed(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->speed = 2;
}

void I4C3DDIControl::NormalSpeed(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->speed = 1;
}

///////////////// MACRO /////////////////
void I4C3DDIControl::PlayMacro(I4C3DDIContext* pContext, LPCSTR macroName)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "%s %c", macroName, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::PlayMacro1(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO1");
}

void I4C3DDIControl::PlayMacro2(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO2");
}

void I4C3DDIControl::PlayMacro3(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO3");
}

void I4C3DDIControl::PlayMacro4(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO4");
}

void I4C3DDIControl::PlayMacro5(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO5");
}

void I4C3DDIControl::PlayMacro6(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO6");
}

void I4C3DDIControl::PlayMacro7(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO7");
}

void I4C3DDIControl::PlayMacro8(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO8");
}

void I4C3DDIControl::PlayMacro9(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO9");
}

void I4C3DDIControl::PlayMacro10(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO10");
}

void I4C3DDIControl::PlayMacro11(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO11");
}

void I4C3DDIControl::PlayMacro12(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO12");
}

void I4C3DDIControl::PlayMacro13(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO13");
}

void I4C3DDIControl::PlayMacro14(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO14");
}

void I4C3DDIControl::PlayMacro15(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO15");
}

void I4C3DDIControl::PlayMacro16(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO16");
}

void I4C3DDIControl::PlayMacro17(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO17");
}

void I4C3DDIControl::PlayMacro18(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO18");
}

void I4C3DDIControl::PlayMacro19(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO19");
}

void I4C3DDIControl::PlayMacro20(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO20");
}

void I4C3DDIControl::PlayMacro21(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO21");
}

void I4C3DDIControl::PlayMacro22(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO22");
}

void I4C3DDIControl::PlayMacro23(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO23");
}

void I4C3DDIControl::PlayMacro24(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO24");
}

void I4C3DDIControl::PlayMacro25(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO25");
}

void I4C3DDIControl::PlayMacro26(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO26");
}

void I4C3DDIControl::PlayMacro27(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO27");
}

void I4C3DDIControl::PlayMacro28(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO28");
}

void I4C3DDIControl::PlayMacro29(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO29");
}

void I4C3DDIControl::PlayMacro30(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO30");
}

void I4C3DDIControl::PlayMacro31(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO31");
}

void I4C3DDIControl::PlayMacro32(I4C3DDIContext* pContext)
{
	PlayMacro(pContext, "MACRO32");
}

///////////////// DOLLY(だめならPOSORIENTで) /////////////////
// 前進（カメラxy）
void I4C3DDIControl::GoForward(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d %c", 0, pContext->pCommandSet->DOLLY_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

// 後退（カメラxy）
void I4C3DDIControl::GoBackward(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d %c", 0, -pContext->pCommandSet->DOLLY_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}


///////////////// TRACK(だめならPOSORIENTで) /////////////////
void I4C3DDIControl::GoUp(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", 0, pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::GoDown(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", 0, -pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::GoLeft(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, 0, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::GoRight(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", -pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, 0, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::GoUpRight(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::GoUpLeft(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::GoDownRight(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::GoDownLeft(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}


///////////////// TUMBLE /////////////////
void I4C3DDIControl::CameraUp(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", 0, -pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraDown(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", 0, pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraLeft(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", -pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, 0, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraRight(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, 0, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraUpRight(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraUpLeft(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraDownRight(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraDownLeft(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}
