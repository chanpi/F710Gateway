#include "StdAfx.h"
#include "I4C3DDIControl.h"
#include "I4C3DDIModulesDefs.h"
#include <vector>
#include <math.h>

using namespace std;

const double M_PI = asin(1.0) * 4.0;

static void CreateCommand(char* buffer, int bufferLen, I4C3DDIContext* pContext);
static CRITICAL_SECTION g_lock;

I4C3DDIControl::I4C3DDIControl(void)
{
}


I4C3DDIControl::~I4C3DDIControl(void)
{
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

void CreateCommand(char* buffer, int bufferLen, I4C3DDIContext* pContext)
{
	sprintf_s(buffer, bufferLen, "POSORIENT PHR CAMERA %d %d %d %d %d %d?",
		pContext->pCommandSet->x, pContext->pCommandSet->y, pContext->pCommandSet->z,
		pContext->pCommandSet->p, pContext->pCommandSet->h, pContext->pCommandSet->r);
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

///////////////// ANIMATION /////////////////
void I4C3DDIControl::StartAnimation1(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "ANIM %s START?", pContext->pCommandSet->animation01Title);
	Execute(pContext, message);
}

void I4C3DDIControl::StartAnimation2(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "ANIM %s START?", pContext->pCommandSet->animation02Title);
	Execute(pContext, message);
}

void I4C3DDIControl::StartAnimation3(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "ANIM %s START?", pContext->pCommandSet->animation03Title);
	Execute(pContext, message);
}

void I4C3DDIControl::StartAnimation4(I4C3DDIContext* pContext)
{
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "ANIM %s START?", pContext->pCommandSet->animation04Title);
	Execute(pContext, message);
}

///////////////// DOLLY(だめならPOSORIENTで) /////////////////
// 前進（カメラxy）
void I4C3DDIControl::GoForward(I4C3DDIContext* pContext)
{
	
	pContext->pCommandSet->x += (int)(pContext->pCommandSet->move * pContext->pCommandSet->speed * sin(M_PI / 180 * pContext->pCommandSet->h));
	pContext->pCommandSet->y -= (int)(pContext->pCommandSet->move * pContext->pCommandSet->speed * cos(M_PI / 180 * pContext->pCommandSet->h));

	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d?", pContext->pCommandSet->x, pContext->pCommandSet->y);
	Execute(pContext, message);
}

// 後退（カメラxy）
void I4C3DDIControl::GoBackward(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->x -= (int)(pContext->pCommandSet->move * pContext->pCommandSet->speed * sin(M_PI / 180 * pContext->pCommandSet->h));
	pContext->pCommandSet->y += (int)(pContext->pCommandSet->move * pContext->pCommandSet->speed * cos(M_PI / 180 * pContext->pCommandSet->h));

	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "DOLLY %d %d?", pContext->pCommandSet->x, pContext->pCommandSet->y);
	Execute(pContext, message);
}


///////////////// TRACK(だめならPOSORIENTで) /////////////////
void I4C3DDIControl::GoUp(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->z = pContext->pCommandSet->z + pContext->pCommandSet->height * pContext->pCommandSet->speed;
    //root.heightText = string.Format("{0}", root.z);
    //form1.HeightText = root.heightText;
	
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d?", pContext->pCommandSet->x, pContext->pCommandSet->y);
	Execute(pContext, message);
}

void I4C3DDIControl::GoDown(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->z = pContext->pCommandSet->z - pContext->pCommandSet->height * pContext->pCommandSet->speed;
    //root.heightText = string.Format("{0}", root.z);
    //form1.HeightText = root.heightText;
	
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d?", pContext->pCommandSet->x, pContext->pCommandSet->y);
	Execute(pContext, message);
}

void I4C3DDIControl::GoLeft(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->z = pContext->pCommandSet->z + pContext->pCommandSet->height * pContext->pCommandSet->speed;
    //root.heightText = string.Format("{0}", root.z);
    //form1.HeightText = root.heightText;
	
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d?", pContext->pCommandSet->x, pContext->pCommandSet->y);
	Execute(pContext, message);
}

void I4C3DDIControl::GoRight(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->z = pContext->pCommandSet->z - pContext->pCommandSet->height * pContext->pCommandSet->speed;
    //root.heightText = string.Format("{0}", root.z);
    //form1.HeightText = root.heightText;
	
	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TRACK %d %d?", pContext->pCommandSet->x, pContext->pCommandSet->y);
	Execute(pContext, message);
}


///////////////// TUMBLE /////////////////
void I4C3DDIControl::CameraUp(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->p -= pContext->pCommandSet->pitch * pContext->pCommandSet->speed;
    //if (pContext->pCommandSet->p < -88)
    //{
    //    pContext->pCommandSet->p = -88;
    //    pContext->pCommandSet->r = 0;
    //}

	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d?", 0, pContext->pCommandSet->p);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraDown(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->p += pContext->pCommandSet->pitch * pContext->pCommandSet->speed;
    //if (pContext->pCommandSet->p < -88)
    //{
    //    pContext->pCommandSet->p = -88;
    //    pContext->pCommandSet->r = 0;
    //}

	char message[I4C3D_BUFFER_SIZE] = {0};
	sprintf_s(message, _countof(message), "TUMBLE %d %d?", 0, pContext->pCommandSet->p);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraLeft(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->h += pContext->pCommandSet->angle * pContext->pCommandSet->speed;

	char message[I4C3D_BUFFER_SIZE] = {0};
	CreateCommand(message, _countof(message), pContext);
	sprintf_s(message, _countof(message), "TUMBLE %d %d?", pContext->pCommandSet->h, 0);
	Execute(pContext, message);
}

void I4C3DDIControl::CameraRight(I4C3DDIContext* pContext)
{
	pContext->pCommandSet->h -= pContext->pCommandSet->angle * pContext->pCommandSet->speed;

	char message[I4C3D_BUFFER_SIZE] = {0};
	CreateCommand(message, _countof(message), pContext);
	sprintf_s(message, _countof(message), "TUMBLE %d %d?", pContext->pCommandSet->h, 0);
	Execute(pContext, message);
}
