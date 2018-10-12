﻿//! @file window.cpp
//! @brief メインウィンドウの作成・制御

//--------------------------------------------------------------------------------
// 
// OpenXOPS
// Copyright (c) 2014-2018, OpenXOPS Project / [-_-;](mikan) All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// * Neither the name of the OpenXOPS Project nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL OpenXOPS Project BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//--------------------------------------------------------------------------------

#include "window.h"

//! @brief コンストラクタ
WindowControl::WindowControl()
{
	hInstance = 0;
	nCmdShow = 0;
	hWnd = NULL;
}

//! @brief ディストラクタ
WindowControl::~WindowControl()
{
	//
}

//! @brief アプリケーションの情報を設定
//! @param in_hInstance インスタンス ハンドル
//! @param in_nCmdShow ウィンドウの表示状態
void WindowControl::SetParam(HINSTANCE in_hInstance, int in_nCmdShow)
{
	hInstance = in_hInstance;
	nCmdShow = in_nCmdShow;
}

//! @brief メインウィンドウ作成
//! @param title ウィンドウタイトル
//! @param width ウィンドウの幅
//! @param height ウィンドウの高さ
//! @param fullscreen false：ウィンドウ表示　true：フルスクリーン用表示
//! @return 成功：true　失敗：false
//! @warning 先にSetParam()関数で設定しておく必要があります。
bool WindowControl::InitWindow(char* title, int width, int height, bool fullscreen)
{
	WNDCLASS wc;
	int x, y;
	RECT Rect;
	DWORD dwStyle;

	//ウィンドウクラスの登録
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	= WindowProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= hInstance;
	wc.hIcon		= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(MAINICON));
	wc.hCursor		= NULL;
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName	= NULL;
	wc.lpszClassName	= "MainWindow";
	if( RegisterClass(&wc) == 0 ){
		return false;
	}

	if( fullscreen == false ){
		dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	}
	else{
		dwStyle = WS_POPUP;
	}

	//ウィンドウサイズを計算
	Rect.left = 0;
	Rect.top = 0;
	Rect.right = width;
	Rect.bottom = height;
	AdjustWindowRect(&Rect, dwStyle, FALSE);
	width = Rect.right - Rect.left;
	height = Rect.bottom - Rect.top;

	//表示位置
	x = 0;
	y = 0;

	/*
	//ウィンドウモードなら、ウィンドウを画面の中心に配置する
	if( fullscreen == false ){
		SystemParametersInfo(SPI_GETWORKAREA, 0, &Rect, 0);
		x = (Rect.right - width)/2;
		y = (Rect.bottom - height)/2;
	}
	*/

	//ウィンドウ作成
	hWnd = CreateWindow( "MainWindow", title, dwStyle, x, y, width, height, NULL, NULL, hInstance, NULL );
	if( hWnd == NULL ){
		return false;
	}

	//表示
	ShowWindow( hWnd, nCmdShow );

	return true;
}

//! @brief メインウィンドウのウィンドウプロシージャ
LRESULT WINAPI WindowControl::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg ){
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//! @brief ウィンドウの表示モード切り替え
//! @param fullscreen false：ウィンドウ表示　true：フルスクリーン用表示
//! @return 成功：true　失敗：false
bool WindowControl::ChangeWindowMode(bool fullscreen)
{
	if( hWnd == NULL ){ return false; }

	DWORD dwStyle;
	RECT Rect;
	int width, height;

	//ウィンドウサイズを取得
	GetClientRect(hWnd, &Rect);

	if( fullscreen == false ){
		ChangeDisplaySettings(NULL, 0);		//ディスプレイ解像度を戻す

		dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	}
	else{
		dwStyle = WS_POPUP;
	}

	//ウィンドウサイズを計算
	AdjustWindowRect(&Rect, dwStyle, FALSE);
	width = Rect.right - Rect.left;
	height = Rect.bottom - Rect.top;

	//反映
	SetWindowLong(hWnd, GWL_STYLE, dwStyle);
	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	return true;
}

//! @brief ウィンドウハンドルを取得
//! @return ウィンドウハンドル
HWND WindowControl::GethWnd()
{
	return hWnd;
}

//! @brief ウィンドウメッセージを処理（メインループ判定）
//! @return アイドル状態：0　ゲームのメイン処理実行：1　ウィンドウが閉じた：-1
//! @attention 本関数から「1」が返された場合に限り、ゲームのメイン処理を実行してください。
//! @attention 本関数から「-1」が返された場合、各リソースやインターフェースを解放し、ゲームを終了してください。
int WindowControl::CheckMainLoop()
{
	MSG msg = {0};

	if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ){
		//ウィンドウメッセージが来ている

		TranslateMessage( &msg );
		DispatchMessage( &msg );

		if( msg.message == WM_QUIT ){
			return -1;
		}
		return 0;
	}
	else if( GetActiveWindow() == hWnd ){
		//ゲームのフレーム処理
		return 1;
	}
	//else{
		//ウィンドウメッセージが来るまで待つ
		WaitMessage();
		return 0;
	//}
}

//! @brief エラーメッセージ表示
//! @param *str メッセージ
void WindowControl::ErrorInfo(char *str)
{
	MessageBox(hWnd, str, "error", MB_OK);
}

//! @brief ウィンドウを閉じるように要求
void WindowControl::CloseWindow()
{
	PostMessage(hWnd, WM_CLOSE, 0L, 0L);
}

#ifdef ENABLE_DEBUGLOG
//! @brief デバック用ログにユーザー環境を出力
//! @note 現時点ではOS情報のみ
void GetOperatingEnvironment()
{
	char str[128];
	char str2[128];

	//OSのバージョン取得
	OSVERSIONINFO OSver;
	OSver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
	GetVersionEx(&OSver);

	//OSビット数取得
	SYSTEM_INFO SystemInfo = {0};
	GetNativeSystemInfo(&SystemInfo);
	switch(SystemInfo.wProcessorArchitecture){
		case PROCESSOR_ARCHITECTURE_AMD64: strcpy(str2, "64bit"); break;
		case PROCESSOR_ARCHITECTURE_IA64:  strcpy(str2, "64bit"); break;
		case PROCESSOR_ARCHITECTURE_INTEL: strcpy(str2, "32bit"); break;
		default: strcpy(str2, "unknown bit");
	}

	//ログに出力
	sprintf(str, "OS：Windows Version %d.%d.%d (%s)", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, str2);
	OutputLog.WriteLog(LOG_CHECK, "環境", str);
}
#endif

//! @brief fps（Frames Per Second：フレームレート）計算
//! @param getcnt fpsを取得する周期（フレーム単位）
//! @return fps数
float GetFps(int getcnt)
{
	static unsigned int ptimeg = 0;
	unsigned int nowtime;
	static float pfps = 0.0f;
	static int cnt = 0;

	if( cnt == 0 ){
		ptimeg = GetTimeMS();
	}
	if( cnt == getcnt ){
		nowtime = GetTimeMS();
		pfps = 1000.0f / ((nowtime - ptimeg)/getcnt);
		cnt = 0;
	}
	else{
		cnt += 1;
	}

	return pfps;
}

//! @brief fps（Frames Per Second：フレームレート）調整
//! @return 調整を実施：true　　調整を実施せず：false
bool ControlFps()
{
	static unsigned int ptimec = 0;
	unsigned int nowtime;
	unsigned int waittime;

	nowtime = GetTimeMS();
	waittime = GAMEFRAMEMS - (nowtime - ptimec);
	if( (0 < waittime)&&(waittime <= GAMEFRAMEMS) ){
		Sleep(waittime);
		ptimec = GetTimeMS();
		return true;
	}

	ptimec = nowtime;
	return false;
}

//! @brief ミリ秒単位を返す
//! @return ミリ秒
unsigned int GetTimeMS()
{
	unsigned int time;

	timeBeginPeriod(1);
	time = timeGetTime();
	timeEndPeriod(1);

	return time;
}

//! @brief 日時による文字列を取得
//! @param str 文字列を受け取るポインタ
void GetTimeName(char *str)
{
	time_t timer;
	struct tm *local;

	timer = time(NULL);
	local = localtime(&timer);

	sprintf(str, "%04d%02d%02d%02d%02d%02d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
}

//! @brief 乱数を初期化
void InitRand()
{
	srand(GetTimeMS());
}

//! @brief ランダムな整数値を返す
//! @param num 範囲
//! @return 0～num-1
int GetRand(int num)
{
	return rand()%num;

	//return rand() / (RAND_MAX/num);

	//本家XOPSのアルゴリズム？
	//static int memory = GetTimeMS();
	//int x;
	//memory = memory * 214013 + 2745024;
	//x = memory >> 16;
	//x = x & 0x00007FFF;
	//return x%num;
}

#ifdef ENABLE_PATH_DELIMITER_SLASH
//! @brief パス区切り文字を変換
//! @param *str ファイルパスのポインタ
//! @return 新しいファイルパスのポインタ
//! @note '\'を'/'へ置き換えます。
char* ChangePathDelimiter(char *str)
{
	static char newstr[MAX_PATH];
	strcpy(newstr, str);

	for(int i=0; i<strlen(newstr); i++){
		if( newstr[i] == '\\' ){
			newstr[i] = '/';
		}
	}

	return newstr;
}
#endif
