#pragma once
#include "I4C3DDI4LModulesDefs.h"

class I4C3DDI4LControl
{
public:
	I4C3DDI4LControl(char cTermination);
	~I4C3DDI4LControl(void);

	static void ChangeSpeed(I4C3DDI4LContext* pContext);
	static void NormalSpeed(I4C3DDI4LContext* pContext);
	static void PlayMacro1(I4C3DDI4LContext* pContext);
	static void PlayMacro2(I4C3DDI4LContext* pContext);
	static void PlayMacro3(I4C3DDI4LContext* pContext);
	static void PlayMacro4(I4C3DDI4LContext* pContext);
	static void PlayMacro5(I4C3DDI4LContext* pContext);
	static void PlayMacro6(I4C3DDI4LContext* pContext);
	static void PlayMacro7(I4C3DDI4LContext* pContext);
	static void PlayMacro8(I4C3DDI4LContext* pContext);
	static void PlayMacro9(I4C3DDI4LContext* pContext);
	static void PlayMacro10(I4C3DDI4LContext* pContext);
	static void PlayMacro11(I4C3DDI4LContext* pContext);
	static void PlayMacro12(I4C3DDI4LContext* pContext);
	static void PlayMacro13(I4C3DDI4LContext* pContext);
	static void PlayMacro14(I4C3DDI4LContext* pContext);
	static void PlayMacro15(I4C3DDI4LContext* pContext);
	static void PlayMacro16(I4C3DDI4LContext* pContext);
	static void PlayMacro17(I4C3DDI4LContext* pContext);
	static void PlayMacro18(I4C3DDI4LContext* pContext);
	static void PlayMacro19(I4C3DDI4LContext* pContext);
	static void PlayMacro20(I4C3DDI4LContext* pContext);
	static void PlayMacro21(I4C3DDI4LContext* pContext);
	static void PlayMacro22(I4C3DDI4LContext* pContext);
	static void PlayMacro23(I4C3DDI4LContext* pContext);
	static void PlayMacro24(I4C3DDI4LContext* pContext);
	static void PlayMacro25(I4C3DDI4LContext* pContext);
	static void PlayMacro26(I4C3DDI4LContext* pContext);
	static void PlayMacro27(I4C3DDI4LContext* pContext);
	static void PlayMacro28(I4C3DDI4LContext* pContext);
	static void PlayMacro29(I4C3DDI4LContext* pContext);
	static void PlayMacro30(I4C3DDI4LContext* pContext);
	static void PlayMacro31(I4C3DDI4LContext* pContext);
	static void PlayMacro32(I4C3DDI4LContext* pContext);

	// DOLLY
	static void GoForward(I4C3DDI4LContext* pContext);	// �O�i�i�J����xy�j
	static void GoBackward(I4C3DDI4LContext* pContext);	// ��ށi�J����xy�j
	
	// TRACK
	static void GoUp(I4C3DDI4LContext* pContext);
	static void GoDown(I4C3DDI4LContext* pContext);
	static void GoLeft(I4C3DDI4LContext* pContext);
	static void GoRight(I4C3DDI4LContext* pContext);
	static void GoUpRight(I4C3DDI4LContext* pContext);
	static void GoUpLeft(I4C3DDI4LContext* pContext);
	static void GoDownRight(I4C3DDI4LContext* pContext);
	static void GoDownLeft(I4C3DDI4LContext* pContext);

	// TUMBLE
	static void CameraUp(I4C3DDI4LContext* pContext);
	static void CameraDown(I4C3DDI4LContext* pContext);
	static void CameraLeft(I4C3DDI4LContext* pContext);
	static void CameraRight(I4C3DDI4LContext* pContext);
	static void CameraUpRight(I4C3DDI4LContext* pContext);
	static void CameraUpLeft(I4C3DDI4LContext* pContext);
	static void CameraDownRight(I4C3DDI4LContext* pContext);
	static void CameraDownLeft(I4C3DDI4LContext* pContext);

private:
	static void Execute(I4C3DDI4LContext* pContext, const char* message);
	static void PlayMacro(I4C3DDI4LContext* pContext, LPCSTR macroName);
};
