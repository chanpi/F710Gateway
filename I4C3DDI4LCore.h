#pragma once

#include "I4C3DDI4LModulesDefs.h"

/* DirectInputÇÃèâä˙âª */

class I4C3DDI4LCore
{
public:
	I4C3DDI4LCore(void);
	~I4C3DDI4LCore(void);

	BOOL Start(I4C3DDI4LContext* pContext);
	void Stop(I4C3DDI4LContext* pContext);
	void CheckInput(I4C3DDI4LContext* pContext);

private:
	BOOL InitializeDirectInput(I4C3DDI4LContext* pContext);
	void InitializeFunctionMap(const I4C3DDI4LContext* pContext);
	LPVOID SearchFunctionByKey(LPCTSTR tempKey);
	void ReadConfigurationFile(I4C3DDI4LContext* pContext);
	BOOL CheckFunctionType(void (*func)(I4C3DDI4LContext*), I4C3DDI4LContext* pContext, BYTE* flag);
	void KickFunctionByType(I4C3DDI4LContext* pContext, BYTE flag);

	void UnInitialize(I4C3DDI4LContext* pContext);
};

