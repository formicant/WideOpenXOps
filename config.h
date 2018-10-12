//! @file config.h
//! @brief configクラスの宣言

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

#ifndef CONFING_H
#define CONFING_H

#define TOTAL_ControlKey 18		//!< 合計操作キー数
#define MAX_PLAYERNAME 21		//!< プレイヤー名の最大文字数

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

//! GetKeycode()用 定数
enum ControlKey {
	KEY_TURNUP = 0,
	KEY_TURNDOWN,
	KEY_TURNLEFT,
	KEY_TURNRIGHT,
	KEY_MOVEFORWARD,
	KEY_MOVEBACKWARD,
	KEY_MOVELEFT,
	KEY_MOVERIGHT,
	KEY_WALK,
	KEY_JUMP,
	KEY_RELOAD,
	KEY_DROPWEAPON,
	KEY_ZOOMSCOPE,
	KEY_ShotMODE,
	KEY_SWITCHWEAPON,
	KEY_WEAPON1,
	KEY_WEAPON2,
	KEY_Shot
};

//! @brief 設定ファイルを読み込むクラス
//! @details ファイルの参照と値の管理を行います。
//! @details 参考資料：「みかん箱」http://mikan.the-ninja.jp/　⇒　技術資料　⇒　config.datファイル解析資料
class Config
{
	int Keycode[TOTAL_ControlKey];		//!< オリジナルキーコード
	int MouseSensitivity;				//!< マウス感度
	bool FullscreenFlag;				//!< フルスクリーン有効
	bool SoundFlag;						//!< 効果音有効
	bool BloodFlag;						//!< 出血有効
	int Brightness;						//!< 画面の明るさ
	bool InvertMouseFlag;				//!< マウス反転
	bool FrameskipFlag;					//!< フレームスキップ
	bool AnotherGunsightFlag;			//!< 別の照準を使用
	char PlayerName[MAX_PLAYERNAME];	//!< プレイヤー名

public:
	Config();
	~Config();
	int LoadFile(char *fname);
	int GetKeycode(int id);
	int GetMouseSensitivity();
	bool GetFullscreenFlag();
	bool GetSoundFlag();
	bool GetBloodFlag();
	int GetBrightness();
	bool GetInvertMouseFlag();
	bool GetFrameskipFlag();
	bool GetAnotherGunsightFlag();
	int GetPlayerName(char *out_str);
};

#endif