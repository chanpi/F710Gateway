#pragma once

#include "I4C3DDIModulesDefs.h"

/* DirectInputÇÃèâä˙âª */

class I4C3DDICore
{
public:
	I4C3DDICore(void);
	~I4C3DDICore(void);

	BOOL Start(I4C3DDIContext* pContext);
	void Stop(I4C3DDIContext* pContext);
	void CheckInput(I4C3DDIContext* pContext);

private:
	HRESULT InitializeDirectInput(I4C3DDIContext* pContext);
	void InitializeFunctionMap(I4C3DDIContext* pContext);
	LPVOID SearchFunctionByKey(LPCTSTR key);
	void ReadConfigurationFile(I4C3DDIContext* pContext);

	void UnInitialize(I4C3DDIContext* pContext);
};

