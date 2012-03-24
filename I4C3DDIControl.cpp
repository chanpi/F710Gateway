#include "StdAfx.h"
#include "I4C3DDIControl.h"
#include "I4C3DDIModulesDefs.h"
#include <vector>
#include <math.h>

using namespace std;

const double M_PI = asin(1.0) * 4.0;
//static int speed = 1;

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
 * ipod����̓d����TOPMOST��3D�\�t�g�ɓ]�����܂��B
 * 
 * @param szCommand
 * ipod�����M�����d���B
 * 
 * @param commandLen
 * �d�����B
 * 
 * ipod����̓d����TOPMOST��3D�\�t�g�ɓ]�����܂��B�]����UDP�ōs���܂��B
 */
void I4C3DDIControl::Execute(I4C3DDIContext* pContext, const char* message)
{
	EnterCriticalSection(&g_lock);
	send(pContext->sender, message, strlen(message), 0);
	LeaveCriticalSection(&g_lock);
}

///////////////// �X�s�[�h�i�ړ��ʁj�̕ύX /////////////////
void I4C3DDIControl::ChangeSpeed(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->speed = 2;
}

void I4C3DDIControl::NormalSpeed(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->speed = 1;
}

///////////////// MACRO /////////////////
void I4C3DDIControl::PlayMacro1(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "MACRO1 %c", g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::PlayMacro2(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "MACRO2 %c", g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::PlayMacro3(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "MACRO3 %c", g_cTermination);
	Execute(pContext, message);
}

void I4C3DDIControl::PlayMacro4(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "MACRO4 %c", g_cTermination);
	Execute(pContext, message);
}

///////////////// DOLLY(���߂Ȃ�POSORIENT��) /////////////////
// �O�i�i�J����xy�j
void I4C3DDIControl::GoForward(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d %c", 0, pContext->pCommandSet->DOLLY_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}

// ��ށi�J����xy�j
void I4C3DDIControl::GoBackward(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d %c", 0, -pContext->pCommandSet->DOLLY_DELTA * pContext->pCommandSet->speed, g_cTermination);
	Execute(pContext, message);
}


///////////////// TRACK(���߂Ȃ�POSORIENT��) /////////////////
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
