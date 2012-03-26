#pragma once

#include "F710ModulesDefs.h"

/* DirectInputÇÃèâä˙âª */

class F710Core
{
public:
	F710Core(void);
	~F710Core(void);

	BOOL Start(F710Context* pContext);
	void Stop(F710Context* pContext);
	void CheckInput(F710Context* pContext);

private:
	BOOL InitializeDirectInput(F710Context* pContext);
	void InitializeFunctionMap(const F710Context* pContext);
	LPVOID SearchFunctionByKey(LPCTSTR tempKey);
	void ReadConfigurationFile(F710Context* pContext);
	BOOL CheckFunctionType(void (*func)(F710Context*), F710Context* pContext, BYTE* flag);
	void KickFunctionByType(F710Context* pContext, BYTE flag);

	void UnInitialize(F710Context* pContext);
};

