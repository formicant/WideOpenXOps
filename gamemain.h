//! @file gamemain.h
//! @brief ゲームメイン処理のサンプルコードのヘッダー

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

#ifndef GAMEMAIN_H
#define GAMEMAIN_H

#define ENABLE_DEBUGCONSOLE	//!< @brief デバック用コンソールの有効化（コメント化で機能無効）

#define MAINMENU_X 280		//!< メニューの表示 X座標（左上基準）
#define MAINMENU_Y 140		//!< メニューの表示 Y座標（〃）
#define TOTAL_MENUITEMS 8	//!< メニュー1画面に表示するミッション数
#define MAINMENU_H (TOTAL_MENUITEMS+2)*30 + 25	//!< メニューの表示サイズ・高さ

#define HUDA_WEAPON_POSX (SCREEN_WIDTH - 255)	//!< 武器情報を表示する領域・X座標
#define HUDA_WEAPON_POSY (SCREEN_HEIGHT - 98)	//!< 武器情報を表示する領域・Y座標
#define HUDA_WEAPON_SIZEW 8		//!< 武器情報を表示する領域・横サイズ（32ピクセルの配置個数）
#define HUDA_WEAPON_SIZEH 3		//!< 武器情報を表示する領域・縦サイズ（32ピクセルの配置個数）

#define VIEW_HEIGHT 19.0f							//!< 視点の高さ
#define VIEWANGLE_NORMAL ((float)M_PI/180*65)		//!< 視野角　標準
#define VIEWANGLE_SCOPE_1 ((float)M_PI/180*30)		//!< 視野角　スコープ1
#define VIEWANGLE_SCOPE_2 ((float)M_PI/180*15)		//!< 視野角　スコープ2

#define TOTAL_EVENTLINE 3				//!< イベントのライン数
#define TOTAL_EVENTENTRYPOINT_0 -100	//!< ライン 0 の開始認識番号
#define TOTAL_EVENTENTRYPOINT_1 -110	//!< ライン 1 の開始認識番号
#define TOTAL_EVENTENTRYPOINT_2 -120	//!< ライン 2 の開始認識番号

#define TOTAL_EVENTENT_SHOWMESSEC 5.0f		//!< イベントメッセージを表示する秒数

#ifdef ENABLE_DEBUGCONSOLE
 #define MAX_CONSOLELEN 45		//!< デバック用コンソールの文字数（行）
 #define MAX_CONSOLELINES 9		//!< デバック用コンソールの行数
 #define CONSOLE_PROMPT ">"		//!< デバック用コンソールのプロンプト
#endif

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

int InitGame(HWND hWnd);

//! ゲームの状態を受け渡しする構造体
struct GameInfo{
	bool selectaddon;		//!< addonを選択
	int selectmission_id;	//!< 選択されたミッション
	bool missioncomplete;	//!< ミッション完了
	unsigned int framecnt;	//!< フレーム数
	int fire;		//!< 射撃回数
	int ontarget;	//!< 命中数
	int kill;		//!< 倒した敵の数	
	int headshot;	//!< 敵の頭部に命中した数
};

#ifdef ENABLE_DEBUGCONSOLE
//! コンソールを管理する構造体
struct ConsoleData{
	int color;						//!< 色
	char textdata[MAX_CONSOLELEN];	//!< 文字列
};
#endif

//! @brief オープニング画面管理クラス
//! @details オープニング画面を管理します。
class opening : public D3Dscene
{
	//int opening_banner;		//!< オープニングで表示するテクスチャID
	void Render3D();
	void Render2D();

public:
	opening();
	~opening();
	int Create();
	void Input();
	void Process();
	void Destroy();
};

//! @brief メニュー画面管理クラス
//! @details メニュー画面を管理します。
class mainmenu : public D3Dscene
{
	int mainmenu_mouseX;		//!< メニュー画面マウスX座標
	int mainmenu_mouseY;		//!< メニュー画面マウスY座標
	int mainmenu_scrollitems_official;	//!< メニュー画面のスクロールしたアイテム数
	int mainmenu_scrollitems_addon;		//!< メニュー画面のスクロールしたアイテム数
	float mainmenu_scrollbar_official_height;	//!< メニュー画面のスクロールバーの高さ
	float mainmenu_scrollbar_official_scale;	//!< メニュー画面のスクロールバーの目盛
	int mainmenu_scrollbar_official_y;			//!< メニュー画面のスクロールバーのY座標
	float mainmenu_scrollbar_addon_height;		//!< メニュー画面のスクロールバーの高さ
	float mainmenu_scrollbar_addon_scale;		//!< メニュー画面のスクロールバーの目盛
	int mainmenu_scrollbar_addon_y;				//!< メニュー画面のスクロールバーのY座標
	bool mainmenu_scrollbar_flag;		//!< メニュー画面のスクロールバーを操作中を示すフラグ
	int gametitle;				//!< ゲームタイトル画像
	void Render3D();
	void Render2D();

public:
	mainmenu();
	~mainmenu();
	int Create();
	void Input();
	void Process();
	void Destroy();
};

//! @brief ブリーフィング画面管理クラス
//! @details ブリーフィング画面を管理します。
class briefing : public D2Dscene
{
	bool TwoTexture;	//!< ブリーフィング画像を2枚使用
	int TextureA;	//!< ブリーフィング画像A
	int TextureB;	//!< ブリーフィング画像B
	void Render2D();

public:
	briefing();
	~briefing();
	int Create();
	void Destroy();
};

//! @brief メインゲーム画面管理クラス
//! @details メインゲーム画面を管理します。
class maingame : public D3Dscene
{
	//class EventControl Event[TOTAL_EVENTLINE];	//!< イベント制御クラス
	float mouse_rx;		//!< マウスによる水平軸角度
	float mouse_ry;		//!< マウスによる垂直軸角度
	float view_rx;		//!< マウス角度とカメラ角度の差（水平軸）
	float view_ry;		//!< マウス角度とカメラ角度の差（垂直軸） 
	bool ShowInfo_Debugmode;	//!< 座標などを表示するデバックモード
	bool Camera_Debugmode;		//!< カメラデバックモード
	bool tag;					//!< オブジェクトのタグを表示
	bool radar;					//!< 簡易レーダー表示
	bool wireframe;				//!< マップをワイヤーフレーム表示
	bool CenterLine;			//!< 3D空間に中心線を表示
	bool Camera_Blind;			//!< 目隠し表示
	bool Camera_F1mode;			//!< カメラF1モード
	int Camera_F2mode;			//!< カメラF2モード
	bool Camera_HOMEmode;		//!< カメラHOMEモード
	bool Cmd_F5;				//!< 裏技F5モード
	int InvincibleID;			//!< 無敵な人の判定
	bool PlayerAI;				//!< プレイヤー操作をAIに委ねる
	bool AIstop;				//!< AI処理を停止する
	bool AINoFight;				//!< AIが非戦闘化する（戦わない）
	int start_framecnt;			//!< メインゲーム開始時のカウント
	int end_framecnt;			//!< メインゲーム終了のカウント
	bool EventStop;				//!< イベント処理を停止する
	int message_id;				//!< 表示中のイベントメッセージ番号
	int message_cnt;			//!< 表示中のイベントメッセージカウント
	bool redflash_flag;		//!< レッドフラッシュ描画フラグ
	int time;		//!< timer
	int time_input;					//!< 入力取得の処理時間
	int time_process_object;		//!< 基本オブジェクトの処理時間
	int time_process_ai;			//!< AIの処理時間
	int time_process_event;			//!< イベントの処理時間
	int time_sound;				//!< サウンドの処理時間
	int time_render;				//!< 描画の処理時間
	GameInfo MainGameInfo;			//!< リザルト用管理クラス
	bool CheckInputControl(int CheckKey, int mode);
	void Render3D();
	void Render2D();
	bool GetRadarPos(float in_x, float in_y, float in_z, int RadarPosX, int RadarPosY, int RadarSize, float RadarWorldR, int *out_x, int *out_y, float *local_y, bool check);
	void RenderRadar();

#ifdef ENABLE_DEBUGCONSOLE
	bool Show_Console;			//!< デバック用コンソールを表示
	ConsoleData *InfoConsoleData;			//!< デバック用コンソールデータ（表示済み）
	ConsoleData *InputConsoleData;			//!< デバック用コンソールデータ（入力中）
	char NewCommand[MAX_CONSOLELEN];	//!< 新たに入力された未処理のコマンド
	bool ScreenShot;		//!< SSを撮影する
	void AddInfoConsole(int color, char *str);
	void ConsoleInputText(char inchar);
	void ConsoleDeleteText();
	bool GetCommandNum(char *cmd, int *num);
	void InputConsole();
	void ProcessConsole();
	void RenderConsole();
#endif

public:
	maingame();
	~maingame();
	int Create();
	void Input();
	void Process();
	void Sound();
	void Destroy();
};

//! @brief リザルト画面管理クラス
//! @details リザルト（結果表示）画面を管理します。
class result : public D2Dscene
{
	void Render2D();

public:
	result();
	~result();
};

void InitScreen(opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result);
void ProcessScreen(HWND hWnd, opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result, unsigned int framecnt);

#endif