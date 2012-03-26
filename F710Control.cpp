#include "StdAfx.h"
#include "F710Control.h"
#include "F710ModulesDefs.h"
#include <vector>

using namespace std;

static void CreateCommand(char* buffer, int bufferLen, F710Context* pContext);

F710Control::F710Control(char cTermination)
	:F710AbstractControl(cTermination)
{
}


F710Control::~F710Control(void)
{
}

///////////////// スピード（移動量）の変更 /////////////////
void F710Control::ChangeSpeed(F710Context* pContext)
{
	pContext->pCommandSet->speed = 2;
}

void F710Control::NormalSpeed(F710Context* pContext)
{
	pContext->pCommandSet->speed = 1;
}

///////////////// MACRO /////////////////

void F710Control::PlayMacro1(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO1");
}

void F710Control::PlayMacro2(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO2");
}

void F710Control::PlayMacro3(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO3");
}

void F710Control::PlayMacro4(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO4");
}

void F710Control::PlayMacro5(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO5");
}

void F710Control::PlayMacro6(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO6");
}

void F710Control::PlayMacro7(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO7");
}

void F710Control::PlayMacro8(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO8");
}

void F710Control::PlayMacro9(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO9");
}

void F710Control::PlayMacro10(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO10");
}

void F710Control::PlayMacro11(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO11");
}

void F710Control::PlayMacro12(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO12");
}

void F710Control::PlayMacro13(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO13");
}

void F710Control::PlayMacro14(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO14");
}

void F710Control::PlayMacro15(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO15");
}

void F710Control::PlayMacro16(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO16");
}

void F710Control::PlayMacro17(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO17");
}

void F710Control::PlayMacro18(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO18");
}

void F710Control::PlayMacro19(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO19");
}

void F710Control::PlayMacro20(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO20");
}

void F710Control::PlayMacro21(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO21");
}

void F710Control::PlayMacro22(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO22");
}

void F710Control::PlayMacro23(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO23");
}

void F710Control::PlayMacro24(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO24");
}

void F710Control::PlayMacro25(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO25");
}

void F710Control::PlayMacro26(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO26");
}

void F710Control::PlayMacro27(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO27");
}

void F710Control::PlayMacro28(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO28");
}

void F710Control::PlayMacro29(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO29");
}

void F710Control::PlayMacro30(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO30");
}

void F710Control::PlayMacro31(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO31");
}

void F710Control::PlayMacro32(F710Context* pContext)
{
	PlayMacro(pContext, "MACRO32");
}

///////////////// DOLLY(だめならPOSORIENTで) /////////////////
// 前進（カメラxy）
void F710Control::GoForward(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d %c", 0, pContext->pCommandSet->DOLLY_DELTA * pContext->pCommandSet->speed, m_cTermination);
	Execute(pContext, message);
}

// 後退（カメラxy）
void F710Control::GoBackward(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d %c", 0, -pContext->pCommandSet->DOLLY_DELTA * pContext->pCommandSet->speed, m_cTermination);
	Execute(pContext, message);
}


///////////////// TRACK(だめならPOSORIENTで) /////////////////
void F710Control::GoUp(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", 0, pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, m_cTermination);
	Execute(pContext, message);
}

void F710Control::GoDown(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", 0, -pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, m_cTermination);
	Execute(pContext, message);
}

void F710Control::GoLeft(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, 0, m_cTermination);
	Execute(pContext, message);
}

void F710Control::GoRight(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c", -pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed, 0, m_cTermination);
	Execute(pContext, message);
}

void F710Control::GoUpRight(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		m_cTermination);
	Execute(pContext, message);
}

void F710Control::GoUpLeft(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		m_cTermination);
	Execute(pContext, message);
}

void F710Control::GoDownRight(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		m_cTermination);
	Execute(pContext, message);
}

void F710Control::GoDownLeft(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d %c",
		pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TRACK_DELTA * pContext->pCommandSet->speed,
		m_cTermination);
	Execute(pContext, message);
}


///////////////// TUMBLE /////////////////
void F710Control::CameraUp(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", 0, -pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, m_cTermination);
	Execute(pContext, message);
}

void F710Control::CameraDown(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", 0, pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, m_cTermination);
	Execute(pContext, message);
}

void F710Control::CameraLeft(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", -pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, 0, m_cTermination);
	Execute(pContext, message);
}

void F710Control::CameraRight(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c", pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed, 0, m_cTermination);
	Execute(pContext, message);
}

void F710Control::CameraUpRight(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		m_cTermination);
	Execute(pContext, message);
}

void F710Control::CameraUpLeft(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		m_cTermination);
	Execute(pContext, message);
}

void F710Control::CameraDownRight(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		m_cTermination);
	Execute(pContext, message);
}

void F710Control::CameraDownLeft(F710Context* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d %c",
		-pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		pContext->pCommandSet->TUMBLE_DELTA * pContext->pCommandSet->speed,
		m_cTermination);
	Execute(pContext, message);
}
