#pragma once
#include "F710ModulesDefs.h"
#include "F710AbstractControl.h"

class F710TCPControl : public F710AbstractControl
{
public:
	F710TCPControl(F710Context* pContext, char cTermination);
	~F710TCPControl(void);

	void ChangeSpeed(F710Context* pContext);
	void NormalSpeed(F710Context* pContext);
	void PlayMacro1(F710Context* pContext);
	void PlayMacro2(F710Context* pContext);
	void PlayMacro3(F710Context* pContext);
	void PlayMacro4(F710Context* pContext);
	void PlayMacro5(F710Context* pContext);
	void PlayMacro6(F710Context* pContext);
	void PlayMacro7(F710Context* pContext);
	void PlayMacro8(F710Context* pContext);
	void PlayMacro9(F710Context* pContext);
	void PlayMacro10(F710Context* pContext);
	void PlayMacro11(F710Context* pContext);
	void PlayMacro12(F710Context* pContext);
	void PlayMacro13(F710Context* pContext);
	void PlayMacro14(F710Context* pContext);
	void PlayMacro15(F710Context* pContext);
	void PlayMacro16(F710Context* pContext);
	void PlayMacro17(F710Context* pContext);
	void PlayMacro18(F710Context* pContext);
	void PlayMacro19(F710Context* pContext);
	void PlayMacro20(F710Context* pContext);
	void PlayMacro21(F710Context* pContext);
	void PlayMacro22(F710Context* pContext);
	void PlayMacro23(F710Context* pContext);
	void PlayMacro24(F710Context* pContext);
	void PlayMacro25(F710Context* pContext);
	void PlayMacro26(F710Context* pContext);
	void PlayMacro27(F710Context* pContext);
	void PlayMacro28(F710Context* pContext);
	void PlayMacro29(F710Context* pContext);
	void PlayMacro30(F710Context* pContext);
	void PlayMacro31(F710Context* pContext);
	void PlayMacro32(F710Context* pContext);

	// DOLLY
	void GoForward(F710Context* pContext);	// 前進（カメラxy）
	void GoBackward(F710Context* pContext);	// 後退（カメラxy）
	
	// TRACK
	void GoUp(F710Context* pContext);
	void GoDown(F710Context* pContext);
	void GoLeft(F710Context* pContext);
	void GoRight(F710Context* pContext);
	void GoUpRight(F710Context* pContext);
	void GoUpLeft(F710Context* pContext);
	void GoDownRight(F710Context* pContext);
	void GoDownLeft(F710Context* pContext);

	// TUMBLE
	void CameraUp(F710Context* pContext);
	void CameraDown(F710Context* pContext);
	void CameraLeft(F710Context* pContext);
	void CameraRight(F710Context* pContext);
	void CameraUpRight(F710Context* pContext);
	void CameraUpLeft(F710Context* pContext);
	void CameraDownRight(F710Context* pContext);
	void CameraDownLeft(F710Context* pContext);

private:
	void ExecuteCameraCommand(F710Context* pContext);
};

