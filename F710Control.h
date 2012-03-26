#pragma once
#include "F710ModulesDefs.h"

class F710Control
{
public:
	F710Control(char cTermination);
	~F710Control(void);

	static void ChangeSpeed(F710Context* pContext);
	static void NormalSpeed(F710Context* pContext);
	static void PlayMacro1(F710Context* pContext);
	static void PlayMacro2(F710Context* pContext);
	static void PlayMacro3(F710Context* pContext);
	static void PlayMacro4(F710Context* pContext);
	static void PlayMacro5(F710Context* pContext);
	static void PlayMacro6(F710Context* pContext);
	static void PlayMacro7(F710Context* pContext);
	static void PlayMacro8(F710Context* pContext);
	static void PlayMacro9(F710Context* pContext);
	static void PlayMacro10(F710Context* pContext);
	static void PlayMacro11(F710Context* pContext);
	static void PlayMacro12(F710Context* pContext);
	static void PlayMacro13(F710Context* pContext);
	static void PlayMacro14(F710Context* pContext);
	static void PlayMacro15(F710Context* pContext);
	static void PlayMacro16(F710Context* pContext);
	static void PlayMacro17(F710Context* pContext);
	static void PlayMacro18(F710Context* pContext);
	static void PlayMacro19(F710Context* pContext);
	static void PlayMacro20(F710Context* pContext);
	static void PlayMacro21(F710Context* pContext);
	static void PlayMacro22(F710Context* pContext);
	static void PlayMacro23(F710Context* pContext);
	static void PlayMacro24(F710Context* pContext);
	static void PlayMacro25(F710Context* pContext);
	static void PlayMacro26(F710Context* pContext);
	static void PlayMacro27(F710Context* pContext);
	static void PlayMacro28(F710Context* pContext);
	static void PlayMacro29(F710Context* pContext);
	static void PlayMacro30(F710Context* pContext);
	static void PlayMacro31(F710Context* pContext);
	static void PlayMacro32(F710Context* pContext);

	// DOLLY
	static void GoForward(F710Context* pContext);	// 前進（カメラxy）
	static void GoBackward(F710Context* pContext);	// 後退（カメラxy）
	
	// TRACK
	static void GoUp(F710Context* pContext);
	static void GoDown(F710Context* pContext);
	static void GoLeft(F710Context* pContext);
	static void GoRight(F710Context* pContext);
	static void GoUpRight(F710Context* pContext);
	static void GoUpLeft(F710Context* pContext);
	static void GoDownRight(F710Context* pContext);
	static void GoDownLeft(F710Context* pContext);

	// TUMBLE
	static void CameraUp(F710Context* pContext);
	static void CameraDown(F710Context* pContext);
	static void CameraLeft(F710Context* pContext);
	static void CameraRight(F710Context* pContext);
	static void CameraUpRight(F710Context* pContext);
	static void CameraUpLeft(F710Context* pContext);
	static void CameraDownRight(F710Context* pContext);
	static void CameraDownLeft(F710Context* pContext);

private:
	static void Execute(F710Context* pContext, const char* message);
	static void PlayMacro(F710Context* pContext, LPCSTR macroName);
};

