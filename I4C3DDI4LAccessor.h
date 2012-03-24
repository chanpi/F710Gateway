#pragma once
#include "I4C3DDI4LModulesDefs.h"

class I4C3DDI4LAccessor
{
public:
	I4C3DDI4LAccessor(void);
	~I4C3DDI4LAccessor(void);

	SOCKET InitializeTCPSocket(struct sockaddr_in* pAddress, LPCSTR szAddress, BOOL bSend, USHORT uPort);
	BOOL SetConnectingSocket(const SOCKET& socketHandler, const struct sockaddr_in* pAddress);
};

