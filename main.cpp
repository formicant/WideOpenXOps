//! @file main.cpp
//! @brief WinMain()関数の定義およびテストプログラム 

//--------------------------------------------------------------------------------
// 
// OpenXOPS
// Copyright (c) 2014-2017, OpenXOPS Project / [-_-;](mikan) All rights reserved.
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

//Doxygen設定ファイル
#include "doxygen.h"

#include "main.h"

#ifdef _DEBUG
 #include <shlwapi.h>
 #pragma comment(lib, "Shlwapi.lib")
#endif

#ifdef ENABLE_DEBUGLOG
 //! ログ出力
 DebugLog OutputLog;
#endif

//! ステートマシン
StateMachine GameState;

//! ゲーム設定データ
Config GameConfig;

//! メインウィンドウ
WindowControl MainWindow;

//! @brief WinMain()関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//未使用引数対策
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

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

	//char str[24];
	//GameConfig.GetPlayerName(str);
	//MessageBox(NULL, str, "プレイヤー名", MB_OK);
	char str[255];
	strcpy(str, "[Information]\nThe compiler is the Debug mode.\nIf release the software, Switch compiler to Release mode.");
	MessageBox(NULL, str, GAMENAME, MB_OK);
#endif

#ifdef ENABLE_DEBUGLOG
	//メモ：Windows環境なら、lpCmdLine引数とかGetCommandLine()関数でも取れますけどね・・。

	//引数を分解
	for(int i=0; i<__argc; i++){

		//"log"が与えられていたら、ログ出力を有効化
		if( strcmp(__argv[i], "log") == 0 ){
			//ファイル作成
			OutputLog.MakeLog();

			MainWindow.ErrorInfo("Enable Debug Log...");
		}
	}

	//ユーザー環境を出力
	GetOperatingEnvironment();

	char infostr[64];

	/*
	//ログに出力
	sprintf(infostr, "%d個", __argc);
	OutputLog.WriteLog(LOG_CHECK, "引数", infostr);
	for(int i=0; i<__argc; i++){
		sprintf(infostr, "引数[%d]", i);
		OutputLog.WriteLog(LOG_CHECK, infostr, __argv[i]);
	}
	*/

	//ログに出力
	OutputLog.WriteLog(LOG_CHECK, "起動", "エントリーポイント開始");
#endif

#ifdef _DEBUG
	//ログに出力
	OutputLog.WriteLog(LOG_CHECK, "起動", "Visual C++ デバッグモード");
#endif

	//設定ファイル読み込み
	if( GameConfig.LoadFile("config.dat") == 1 ){
		MainWindow.ErrorInfo("config data open failed");
		return 1;
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	sprintf(infostr, "解像度：%d x %d", SCREEN_WIDTH, SCREEN_HEIGHT);
	OutputLog.WriteLog(LOG_CHECK, "環境", infostr);
	if( GameConfig.GetFullscreenFlag() == false ){
		OutputLog.WriteLog(LOG_CHECK, "環境", "ウィンドウモード：ウィンドウ");
	}
	else{
		OutputLog.WriteLog(LOG_CHECK, "環境", "ウィンドウモード：フルスクリーン");
	}
#endif

	//ウィンドウ初期化
	MainWindow.SetParam(hPrevInstance, nCmdShow);
	MainWindow.InitWindow(GAMENAME, SCREEN_WIDTH, SCREEN_HEIGHT, GameConfig.GetFullscreenFlag());

	//基本的な初期化処理
	if( InitGame(&MainWindow) ){
		return 1;
	}

	opening Opening;
	mainmenu MainMenu;
	briefing Briefing;
	maingame MainGame;
	result Result;
	InitScreen(&MainWindow, &Opening, &MainMenu, &Briefing, &MainGame, &Result);


#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CHECK, "起動", "メインループ突入");
#endif

	unsigned int framecnt = 0;

	for(int flag = 0; flag != -1; flag = MainWindow.CheckMainLoop()){
		if( flag == 1 ){
			//メイン処理
			ProcessScreen(&MainWindow, &Opening, &MainMenu, &Briefing, &MainGame, &Result, framecnt);

			//FPS調整
			ControlFps();

			framecnt++;
		}
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CHECK, "終了", "メインループ脱出");
#endif


	//基本的な解放処理
	CleanupGame();

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CHECK, "終了", "エントリーポイント終了");
#endif
	return 0;
}