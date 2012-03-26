#pragma once
#include "F710ModulesDefs.h"

class F710Accessor
{
public:
	F710Accessor(void);
	~F710Accessor(void);

	SOCKET InitializeTCPSocket(struct sockaddr_in* pAddress, LPCSTR szAddress, BOOL bSend, USHORT uPort);
	BOOL SetConnectingSocket(const SOCKET& socketHandler, const struct sockaddr_in* pAddress);
};

