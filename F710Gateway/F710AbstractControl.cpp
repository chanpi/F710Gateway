#include "StdAfx.h"
#include "F710AbstractControl.h"
#include "MemoryLeak.h"

F710AbstractControl::F710AbstractControl(void)
{
}


F710AbstractControl::~F710AbstractControl(void)
{
}

BOOL F710AbstractControl::Initialize(F710Context* /*pContext*/, char cTermination)
{
	m_cTermination = cTermination;
	return TRUE;
}

void F710AbstractControl::Execute(F710Context* pContext, const char* message)
{
	send(pContext->sender, message, strlen(message), 0);
}

void F710AbstractControl::PlayMacro(F710Context* pContext, LPCSTR macroName)
{
	if (macroName != NULL && strcmp(macroName, "") != 0) {
		char message[BUFFER_SIZE] = {0};
		sprintf_s(message, _countof(message), "%s %c", macroName, m_cTermination);
		Execute(pContext, message);
	}
}