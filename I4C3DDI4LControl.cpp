#include "StdAfx.h"
#include "I4C3DDI4LControl.h"
#include "I4C3DDI4LModulesDefs.h"
#include <vector>
#include <math.h>

using namespace std;

const double M_PI = asin(1.0) * 4.0;

static void CreateCommand(char* buffer, int bufferLen, I4C3DDI4LContext* pContext);
static CRITICAL_SECTION g_lock;
static char g_cTermination = '?';

I4C3DDI4LControl::I4C3DDI4LControl(char cTermination)
{
	g_cTermination = cTermination;
	InitializeCriticalSection(&g_lock);
}


I4C3DDI4LControl::~I4C3DDI4LControl(void)
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
void I4C3DDI4LControl::Execute(I4C3DDI4LContext* pContext, const char* message)
{
	EnterCriticalSection(&g_lock);
	send(pContext->sender, message, strlen(message), 0);
	LeaveCriticalSection(&g_lock);
}

///////////////// スピード（移動量）の変更 /////////////////
void I4C3DDI4LControl::ChangeSpeed(I4C3DDI4LContext* pContext)
{
	pContext->pCommandSet->speed = 2;
}

void I4C3DDI4LControl::NormalSpeed(I4C3DDI4LContext* pContext)
{
	pContext->pCommandSet->speed = 1;
}

///////////////// MACRO /////////////////
void I4C3DDI4LControl::PlayMacro(I4C3DDI4LContext* pContext, LPCSTR macroName)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "%s %c", macroName, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::PlayMacro1(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO1");
}

void I4C3DDI4LControl::PlayMacro2(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO2");
}

void I4C3DDI4LControl::PlayMacro3(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO3");
}

void I4C3DDI4LControl::PlayMacro4(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO4");
}

void I4C3DDI4LControl::PlayMacro5(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO5");
}

void I4C3DDI4LControl::PlayMacro6(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO6");
}

void I4C3DDI4LControl::PlayMacro7(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO7");
}

void I4C3DDI4LControl::PlayMacro8(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO8");
}

void I4C3DDI4LControl::PlayMacro9(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO9");
}

void I4C3DDI4LControl::PlayMacro10(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO10");
}

void I4C3DDI4LControl::PlayMacro11(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO11");
}

void I4C3DDI4LControl::PlayMacro12(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO12");
}

void I4C3DDI4LControl::PlayMacro13(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO13");
}

void I4C3DDI4LControl::PlayMacro14(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO14");
}

void I4C3DDI4LControl::PlayMacro15(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO15");
}

void I4C3DDI4LControl::PlayMacro16(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO16");
}

void I4C3DDI4LControl::PlayMacro17(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO17");
}

void I4C3DDI4LControl::PlayMacro18(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO18");
}

void I4C3DDI4LControl::PlayMacro19(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO19");
}

void I4C3DDI4LControl::PlayMacro20(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO20");
}

void I4C3DDI4LControl::PlayMacro21(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO21");
}

void I4C3DDI4LControl::PlayMacro22(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO22");
}

void I4C3DDI4LControl::PlayMacro23(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO23");
}

void I4C3DDI4LControl::PlayMacro24(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO24");
}

void I4C3DDI4LControl::PlayMacro25(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO25");
}

void I4C3DDI4LControl::PlayMacro26(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO26");
}

void I4C3DDI4LControl::PlayMacro27(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO27");
}

void I4C3DDI4LControl::PlayMacro28(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO28");
}

void I4C3DDI4LControl::PlayMacro29(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO29");
}

void I4C3DDI4LControl::PlayMacro30(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO30");
}

void I4C3DDI4LControl::PlayMacro31(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO31");
}

void I4C3DDI4LControl::PlayMacro32(I4C3DDI4LContext* pContext)
{
	PlayMacro(pContext, "MACRO32");
}

///////////////// DOLLY(だめならPOSORIENTで) /////////////////
// 前進（カメラxy）
void I4C3DDI4LControl::GoForward(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d %c", 0, pContext->pCommandSet->DOLLY_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

// 後退（カメラxy）
void I4C3DDI4LControl::GoBackward(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d %c", 0, -pContext->pCommandSet->DOLLY_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}


///////////////// TRACK(だめならPOSORIENTで) /////////////////
void I4C3DDI4LControl::GoUp(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", 0, pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::GoDown(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", 0, -pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::GoLeft(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, 0, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::GoRight(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", -pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, 0, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::GoUpRight(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::GoUpLeft(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::GoDownRight(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::GoDownLeft(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}


///////////////// TUMBLE /////////////////
void I4C3DDI4LControl::CameraUp(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", 0, -pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::CameraDown(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", 0, pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::CameraLeft(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", -pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, 0, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::CameraRight(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, 0, g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::CameraUpRight(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::CameraUpLeft(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::CameraDownRight(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}

void I4C3DDI4LControl::CameraDownLeft(I4C3DDI4LContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		g_cTermination);
	Execute(pContext, message);
}
