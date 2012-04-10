// I4C3DDriver.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "F710Gateway.h"
#include "F710Modules.h"
#include "MemoryLeak.h"
#include "Misc.h"
#include "SharedConstants.h"
#include "CertificateManager.h"
#include <ShellAPI.h>

#include <cstdlib>	// �K�v

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define new  ::new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#endif

#if UNICODE || _UNICODE
static LPCTSTR g_FILE = __FILEW__;
#else
static LPCTSTR g_FILE = __FILE__;
#endif

#define MAX_LOADSTRING	100
#define MY_NOTIFYICON	(WM_APP+1)
#define TIMER_ID		1

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);

namespace {
	static int g_timerInterval = 30;
	static TCHAR g_szConfigFile[MAX_PATH] = {0};
	static LPCTSTR g_szExecutableOption = _T("-run");
}

int F710ExitCode = EXIT_SUCCESS;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if DEBUG || _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

 	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_F710Gateway, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	int argc = 0;
	LPTSTR *argv = NULL;
	argv = CommandLineToArgvW(GetCommandLine(), &argc);
	if (argc < 5) {	// �Ō�̈����̓����`���[������炤"-run"
		// [��: F710Gateway.exe Hamster.lic [LogicoolF710�̓��̓`�F�b�N�C���^�[�o��(msec)] [�ݒ�t�@�C����] -run]
		MessageBox(NULL, _T(MESSAGE_ERROR_PLUGIN_ARGUMENT), szTitle, MB_OK | MB_ICONERROR);
		LocalFree(argv);
		return EXIT_NO_ARGUMENTS;
	}

	// ���C�Z���X�t�@�C�����擾
	int result = CheckLicense(argv[1]);
	if (result != EXIT_SUCCESS) {
		LocalFree(argv);
		return result;
	}

	if (0 != _tcsicmp(argv[4], g_szExecutableOption)) {
		//LoggingMessage(Log_Error, _T(MESSAGE_ERROR_PLUGIN_OPTION), GetLastError(), g_FILE, __LINE__);
		LocalFree(argv);
		return EXIT_NOT_EXECUTABLE;
	}

	g_timerInterval = static_cast<int>(_wtoi(argv[2]));
	_tcscpy_s(g_szConfigFile, _countof(g_szConfigFile), argv[3]);
	LocalFree(argv);

	static WSAData wsaData = {0};
	WORD wVersion = 0;
	int nResult = 0;

	wVersion = MAKEWORD(2,2);
	nResult = WSAStartup(wVersion, &wsaData);
	if (nResult != 0) {
		MessageBox(NULL, _T(MESSAGE_ERROR_SOCKET_INVALID), szTitle, MB_OK | MB_ICONERROR);
		return EXIT_SOCKET_ERROR;
	}
	if (wsaData.wVersion != wVersion) {
		MessageBox(NULL, _T(MESSAGE_ERROR_SOCKET_INVALID), szTitle, MB_OK | MB_ICONERROR);
		WSACleanup();
		return EXIT_SOCKET_ERROR;
	}

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		WSACleanup();
		return EXIT_SYSTEM_ERROR;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_F710Gateway));

	// ���C�� ���b�Z�[�W ���[�v:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	WSACleanup();
	return (int) msg.wParam;
}

//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_F710Gateway));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_F710Gateway);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int /*nCmdShow*/)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 300, 200, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	TCHAR szFileName[MAX_PATH] = {0};
	static NOTIFYICONDATA nIcon = {0};

	switch (message)
	{
	case WM_CREATE:
		if (!F710Start(g_szConfigFile, hInst, hWnd)) {
			LoggingMessage(Log_Error, _T(MESSAGE_ERROR_SYSTEM_INIT), GetLastError(), g_FILE, __LINE__);
			PostQuitMessage(F710ExitCode);
			return 0;
		}

		if (!GetModuleFileName(NULL, szFileName, _countof(szFileName))) {
			LoggingMessage(Log_Error,_T(MESSAGE_ERROR_SYSTEM_INIT), GetLastError(), g_FILE, __LINE__);
			PostQuitMessage(EXIT_SYSTEM_ERROR);
			return 0;
		}

		// DirectInput�̂��߂ɒǋL
		SetTimer(hWnd, TIMER_ID, g_timerInterval, NULL);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_TIMER:
		F710CheckInput();
		if (F710ExitCode != EXIT_SUCCESS) {
			PostQuitMessage(F710ExitCode);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �`��R�[�h�������ɒǉ����Ă�������...
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		F710Stop();		
		PostQuitMessage(F710ExitCode);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
