#include "StdAfx.h"
#include "F710Accessor.h"
#include "F710ModulesDefs.h"
#include "Misc.h"
#include "SharedConstants.h"
#include "MemoryLeak.h"

#if UNICODE || _UNICODE
static LPCTSTR g_FILE = __FILEW__;
#else
static LPCTSTR g_FILE = __FILE__;
#endif

F710Accessor::F710Accessor(void)
{
}


F710Accessor::~F710Accessor(void)
{
}

/**
 * @brief
 * ��M�^�C���A�E�g�w��t��TCP�\�P�b�g���쐬���܂��B
 * 
 * @param szAddress
 * �Ώۂ�IP�A�h���X�B
 * 
 * @param uPort
 * �Ώۂ�TCP�|�[�g�B
 * 
 * @param nTimeoutMilisec
 * ��M�^�C���A�E�g[msec]�B
 * 
 * @param bSend
 * ���M�\�P�b�g�̏ꍇTRUE�A��M�\�P�b�g�̏ꍇFALSE�B
 * 
 * @param backlog
 * listen�֐��ő҂��󂯂�N���C�A���g���B
 * 
 * @returns
 * ��������[���M/��M]�ҋ@��Ԃ̃\�P�b�g�B���s�����ꍇ�ɂ�INVALID_SOCKET��Ԃ��܂��B
 * 
 * ��M�^�C���A�E�g�w��t��TCP�\�P�b�g���쐬���܂��B
 */
SOCKET F710Accessor::InitializeTCPSocket(struct sockaddr_in* pAddress, LPCSTR szAddress, BOOL bSend, USHORT uPort)
{
	SOCKET socketHandler = INVALID_SOCKET;

	socketHandler = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketHandler == INVALID_SOCKET) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_SOCKET_INVALID), GetLastError(), g_FILE, __LINE__);
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

BOOL F710Accessor::SetConnectingSocket(const SOCKET& socketHandler, const struct sockaddr_in* pAddress) {
	BOOL bUse = TRUE;
	setsockopt(socketHandler, SOL_SOCKET, SO_REUSEADDR, (const char*)&bUse, sizeof(bUse));
	int nResult = 0;

	nResult = connect(socketHandler, (const sockaddr*)pAddress, sizeof(*pAddress));
	if (nResult == SOCKET_ERROR) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_SOCKET_CONNECT), GetLastError(), g_FILE, __LINE__);
		closesocket(socketHandler);
		return FALSE;
	}
	return TRUE;
}
