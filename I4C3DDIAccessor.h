#pragma once
#include "I4C3DDIModulesDefs.h"

class I4C3DDIAccessor
{
public:
	I4C3DDIAccessor(void);
	~I4C3DDIAccessor(void);

	SOCKET InitializeTCPSocket(struct sockaddr_in* pAddress, LPCSTR szAddress, BOOL bSend, USHORT uPort);
};

