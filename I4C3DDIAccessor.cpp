#include "StdAfx.h"
#include "I4C3DDIAccessor.h"
#include "I4C3DDIModulesDefs.h"
#include "Miscellaneous.h"

I4C3DDIAccessor::I4C3DDIAccessor(void)
{
}


I4C3DDIAccessor::~I4C3DDIAccessor(void)
{
}

/**
 * @brief
 * 受信タイムアウト指定付のTCPソケットを作成します。
 * 
 * @param szAddress
 * 対象のIPアドレス。
 * 
 * @param uPort
 * 対象のTCPポート。
 * 
 * @param nTimeoutMilisec
 * 受信タイムアウト[msec]。
 * 
 * @param bSend
 * 送信ソケットの場合TRUE、受信ソケットの場合FALSE。
 * 
 * @param backlog
 * listen関数で待ち受けるクライアント数。
 * 
 * @returns
 * 生成した[送信/受信]待機状態のソケット。失敗した場合にはINVALID_SOCKETを返します。
 * 
 * 受信タイムアウト指定付のTCPソケットを作成します。
 */
SOCKET I4C3DDIAccessor::InitializeTCPSocket(struct sockaddr_in* pAddress, LPCSTR szAddress, BOOL bSend, USHORT uPort)
{
	SOCKET socketHandler;
	TCHAR szError[I4C3D_BUFFER_SIZE];

	socketHandler = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketHandler == INVALID_SOCKET) {
		_stprintf_s(szError, _countof(szError), _T("[ERROR] socket() : %d"), WSAGetLastError());
		ReportError(szError);
		LogDebugMessage(Log_Error, szError);
		return socketHandler;
	}

	pAddress->sin_family = AF_INET;
	pAddress->sin_port = htons(uPort);
	if (bSend) {
		pAddress->sin_addr.S_un.S_addr = inet_addr(szAddress);
	} else {
		pAddress->sin_addr.S_un.S_addr = INADDR_ANY;
	}
	return socketHandler;
}

BOOL I4C3DDIAccessor::SetConnectingSocket(const SOCKET& socketHandler, const struct sockaddr_in* pAddress) {
	BOOL bUse = TRUE;
	setsockopt(socketHandler, SOL_SOCKET, SO_REUSEADDR, (const char*)&bUse, sizeof(bUse));
	int nResult = 0;

	TCHAR szError[I4C3D_BUFFER_SIZE] = {0};

	nResult = connect(socketHandler, (const sockaddr*)pAddress, sizeof(*pAddress));
	if (nResult == SOCKET_ERROR) {
		_stprintf_s(szError, _countof(szError), _T("[ERROR] connect() : %d"), WSAGetLastError());
		ReportError(szError);
		LogDebugMessage(Log_Error, szError);
		closesocket(socketHandler);
		return FALSE;
	}
	return TRUE;
}
