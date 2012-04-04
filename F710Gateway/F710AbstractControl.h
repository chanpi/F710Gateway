#pragma once

#include "F710ModulesDefs.h"

const int BUFFER_SIZE = 256;

class F710AbstractControl
{
public:
	F710AbstractControl();
	virtual ~F710AbstractControl(void);

	virtual BOOL Initialize(F710Context* pContext, char cTermination);
	virtual void UnInitialize(void) = 0;

	virtual void ChangeSpeed(F710Context* pContext) = 0;
	virtual void NormalSpeed(F710Context* pContext) = 0;
	virtual void PlayMacro1(F710Context* pContext) = 0;
	virtual void PlayMacro2(F710Context* pContext) = 0;
	virtual void PlayMacro3(F710Context* pContext) = 0;
	virtual void PlayMacro4(F710Context* pContext) = 0;
	virtual void PlayMacro5(F710Context* pContext) = 0;
	virtual void PlayMacro6(F710Context* pContext) = 0;
	virtual void PlayMacro7(F710Context* pContext) = 0;
	virtual void PlayMacro8(F710Context* pContext) = 0;
	virtual void PlayMacro9(F710Context* pContext) = 0;
	virtual void PlayMacro10(F710Context* pContext) = 0;
	virtual void PlayMacro11(F710Context* pContext) = 0;
	virtual void PlayMacro12(F710Context* pContext) = 0;
	virtual void PlayMacro13(F710Context* pContext) = 0;
	virtual void PlayMacro14(F710Context* pContext) = 0;
	virtual void PlayMacro15(F710Context* pContext) = 0;
	virtual void PlayMacro16(F710Context* pContext) = 0;
	virtual void PlayMacro17(F710Context* pContext) = 0;
	virtual void PlayMacro18(F710Context* pContext) = 0;
	virtual void PlayMacro19(F710Context* pContext) = 0;
	virtual void PlayMacro20(F710Context* pContext) = 0;
	virtual void PlayMacro21(F710Context* pContext) = 0;
	virtual void PlayMacro22(F710Context* pContext) = 0;
	virtual void PlayMacro23(F710Context* pContext) = 0;
	virtual void PlayMacro24(F710Context* pContext) = 0;
	virtual void PlayMacro25(F710Context* pContext) = 0;
	virtual void PlayMacro26(F710Context* pContext) = 0;
	virtual void PlayMacro27(F710Context* pContext) = 0;
	virtual void PlayMacro28(F710Context* pContext) = 0;
	virtual void PlayMacro29(F710Context* pContext) = 0;
	virtual void PlayMacro30(F710Context* pContext) = 0;
	virtual void PlayMacro31(F710Context* pContext) = 0;
	virtual void PlayMacro32(F710Context* pContext) = 0;

	// DOLLY
	virtual void GoForward(F710Context* pContext) = 0;	// 前進（カメラxy）
	virtual void GoBackward(F710Context* pContext) = 0;	// 後退（カメラxy）
	
	// TRACK
	virtual void GoUp(F710Context* pContext) = 0;
	virtual void GoDown(F710Context* pContext) = 0;
	virtual void GoLeft(F710Context* pContext) = 0;
	virtual void GoRight(F710Context* pContext) = 0;
	virtual void GoUpRight(F710Context* pContext) = 0;
	virtual void GoUpLeft(F710Context* pContext) = 0;
	virtual void GoDownRight(F710Context* pContext) = 0;
	virtual void GoDownLeft(F710Context* pContext) = 0;

	// TUMBLE
	virtual void CameraUp(F710Context* pContext) = 0;
	virtual void CameraDown(F710Context* pContext) = 0;
	virtual void CameraLeft(F710Context* pContext) = 0;
	virtual void CameraRight(F710Context* pContext) = 0;
	virtual void CameraUpRight(F710Context* pContext) = 0;
	virtual void CameraUpLeft(F710Context* pContext) = 0;
	virtual void CameraDownRight(F710Context* pContext) = 0;
	virtual void CameraDownLeft(F710Context* pContext) = 0;

protected:
	char m_cTermination;

	virtual void Execute(F710Context* pContext, const char* message);
	virtual void PlayMacro(F710Context* pContext, LPCSTR macroName);
};

