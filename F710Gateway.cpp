// I4C3DDriver.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "F710Gateway.h"
#include "F710Modules.h"
#include "MemoryLeak.h"
#include "Miscellaneous.h"
#include "ErrorCodeList.h"
#include <ShellAPI.h>

#define MAX_LOADSTRING	100
#define MY_NOTIFYICON	(WM_APP+1)
#define TIMER_ID		1

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);

namespace {
	static int g_timerInterval = 30;
}

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

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_F710Gateway, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	int argc = 0;
	LPTSTR *argv = NULL;
	argv = CommandLineToArgvW(GetCommandLine(), &argc);
	if (argc < 3) {	// 最後の引数はランチャーからもらう"-run"
		// [例: F710Gateway.exe [LogicoolF710の入力チェックインターバル(msec)]]
		MessageBox(NULL, _T("[ERROR] 引数が足りません[例: F710Gateway.exe 50]。<F710Gateway>"), szTitle, MB_OK | MB_ICONERROR);
		LocalFree(argv);
		return EXIT_NO_ARGUMENTS;
	}
	if (0 != _tcsicmp(argv[2], _T("-run"))) {
		//LogDebugMessage(Log_Error, _T("起動オプションがありません。このアプリケーションはランチャーから起動される必要があります。"));
		LocalFree(argv);
		return EXIT_NOT_EXECUTABLE;
	}

	g_timerInterval = static_cast<int>(_wtoi(argv[1]));
	LocalFree(argv);

	static WSAData wsaData;
	WORD wVersion;
	int nResult;

	wVersion = MAKEWORD(2,2);
	nResult = WSAStartup(wVersion, &wsaData);
	if (nResult != 0) {
		MessageBox(NULL, _T("[ERROR] Initialize Winsock."), szTitle, MB_OK | MB_ICONERROR);
		return EXIT_SOCKET_ERROR;
	}
	if (wsaData.wVersion != wVersion) {
		MessageBox(NULL, _T("[ERROR] Winsock バージョン."), szTitle, MB_OK | MB_ICONERROR);
		WSACleanup();
		return EXIT_SOCKET_ERROR;
	}

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		WSACleanup();
		return EXIT_SYSTEM_ERROR;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_F710Gateway));

	// メイン メッセージ ループ:
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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
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
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int /*nCmdShow*/)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

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
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	TCHAR szFileName[MAX_PATH] = {0};
	static NOTIFYICONDATA nIcon = {0};
	static int sw = 1;

	switch (message)
	{
	case WM_CREATE:
		if (!F710Start(_T("I4C3D.xml"), hInst, hWnd)) {
			LogDebugMessage(Log_Error, _T("[ERROR] F710Gatewayの初期化に失敗しました。終了します。"));
			PostQuitMessage(EXIT_FAILURE);
			return 0;
		}

		if (!GetModuleFileName(NULL, szFileName, _countof(szFileName))) {
			LogDebugMessage(Log_Error,_T("[ERROR] 実行モジュール名の取得に失敗しました。終了します。"));
			PostQuitMessage(EXIT_SYSTEM_ERROR);
			return 0;
		}

		// DirectInputのために追記
		SetTimer(hWnd, TIMER_ID, g_timerInterval, NULL);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
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
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		F710Stop();
		PostQuitMessage(EXIT_SUCCESS);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
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
