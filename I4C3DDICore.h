#pragma once

#include "I4C3DDIModulesDefs.h"

/* DirectInput�̏����� */

class I4C3DDICore
{
public:
	I4C3DDICore(void);
	~I4C3DDICore(void);

	BOOL Start(I4C3DDIContext* pContext);
	void Stop(I4C3DDIContext* pContext);
	void CheckInput(I4C3DDIContext* pContext);

private:
	BOOL InitializeDirectInput(I4C3DDIContext* pContext);
	void InitializeFunctionMap(const I4C3DDIContext* pContext);
	LPVOID SearchFunctionByKey(LPCTSTR tempKey);
	void ReadConfigurationFile(I4C3DDIContext* pContext);
	BOOL CheckFunctionType(void (*func)(I4C3DDIContext*), I4C3DDIContext* pContext, BYTE* flag);
	void KickFunctionByType(I4C3DDIContext* pContext, BYTE flag);

	void UnInitialize(I4C3DDIContext* pContext);
};

