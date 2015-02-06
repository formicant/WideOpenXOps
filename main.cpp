//! @file main.cpp
//! @brief WinMain()関数の定義およびテストプログラム 

//--------------------------------------------------------------------------------
// 
// OpenXOPS
// Copyright (c) 2014-2015, OpenXOPS Project / [-_-;](mikan) All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// * Neither the name of the OpenXOPS Project nor the　names of its contributors 
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

//Doxygen設定ファイル
#include "doxygen.h"

#include "main.h"

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

//! ステートマシン
StateMachine GameState;

//! ゲーム設定データ
Config GameConfig;

//! @brief WinMain()関数
int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//乱数初期化
	InitRand();

#ifdef _DEBUG
	//メモリリークの検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//実行ファイルのある場所を、カレントディレクトリにする。
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	SetCurrentDirectory(path);
#endif

	//設定ファイル読み込み
	if( GameConfig.LoadFile("config.dat") == 1 ){
		MessageBox(NULL, "config data open failed", "error", MB_OK);
		return 1;
	}

#ifdef _DEBUG
	char str[24];
	GameConfig.GetPlayerName(str);
	MessageBox(NULL, str, "プレイヤー名", MB_OK);
#endif

	//ウィンドウ初期化
	HWND hWnd;
	hWnd = InitWindow(hPrevInstance, GAMENAME, SCREEN_WIDTH, SCREEN_HEIGHT, nCmdShow, GameConfig.GetFullscreenFlag());

	//基本的な初期化処理
	if( InitGame(hWnd) ){
		return 1;
	}

	opening Opening;
	mainmenu MainMenu;
	briefing Briefing;
	maingame MainGame;
	result Result;
	InitScreen(&Opening, &MainMenu, &Briefing, &MainGame, &Result);


	unsigned int framecnt = 0;

	MSG msg = {0};

	//[WM_QUIT]が来るまで回る
	while( msg.message != WM_QUIT ){
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ){		//ウインドウメッセージが来ている
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if( GetActiveWindow() == hWnd ){					//ウインドウがアクティブならば
			//メイン処理
			ProcessScreen(hWnd, &Opening, &MainMenu, &Briefing, &MainGame, &Result, framecnt);

			//FPS調整
			ControlFps();

			framecnt++;
		}
		else{													//ウインドウが非アクティブで、ウインドウメッセージも来ない
			//ウインドウメッセージが来るまで待つ
			WaitMessage();
		}
	}

	return 0;
}