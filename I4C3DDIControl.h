#pragma once
#include "I4C3DDIModulesDefs.h"

class I4C3DDIControl
{
public:
	I4C3DDIControl(char cTermination);
	~I4C3DDIControl(void);

	static void ChangeSpeed(I4C3DDIContext* pContext);
	static void NormalSpeed(I4C3DDIContext* pContext);
	static void StartAnimation1(I4C3DDIContext* pContext);
	static void StartAnimation2(I4C3DDIContext* pContext);
	static void StartAnimation3(I4C3DDIContext* pContext);
	static void StartAnimation4(I4C3DDIContext* pContext);
	
	// DOLLY
	static void GoForward(I4C3DDIContext* pContext);	// 前進（カメラxy）
	static void GoBackward(I4C3DDIContext* pContext);	// 後退（カメラxy）
	
	// TRACK
	static void GoUp(I4C3DDIContext* pContext);
	static void GoDown(I4C3DDIContext* pContext);
	static void GoLeft(I4C3DDIContext* pContext);
	static void GoRight(I4C3DDIContext* pContext);

	// TUMBLE
	static void CameraUp(I4C3DDIContext* pContext);
	static void CameraDown(I4C3DDIContext* pContext);
	static void CameraLeft(I4C3DDIContext* pContext);
	static void CameraRight(I4C3DDIContext* pContext);

private:
	static void Execute(I4C3DDIContext* pContext, const char* message);
};

