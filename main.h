//! @file main.h
//! @brief 最上位のヘッダーファイル

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

#ifndef MAIN_H
#define MAIN_H

#define MAINICON 101		//!< Icon

#ifdef _DEBUG
 #include <crtdbg.h>
#endif

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif

#pragma warning(disable:4996)		//VC++警告防止

//定数
#define GAMENAME "OpenXOPS"		//!< ゲーム名
#define GAMEVERSION "1.011"		//!< ゲームのバージョン
#define GAMEFRAMEMS 30			//!< フレームあたりの処理時間（ms）
#define GAMEFPS (1000.0f/GAMEFRAMEMS)	//!< FPS（フレームレート）　1000 / 30 = 33.333[FPS]
#define SCREEN_WIDTH 640		//!< スクリーンの幅
#define SCREEN_HEIGHT 480		//!< スクリーンの高さ

#define MAX_ADDONLIST 128	//!< ADDONを読み込む最大数

#define TOTAL_HAVEWEAPON 2		//!< 持てる武器の数

//システムのインクルードファイル
#define _USE_MATH_DEFINES	 //!< math.h 'M_PI' enable
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//windows.hを使用しないならば
/*
#define HWND int
#define _MAX_PATH 260
#define MAX_PATH 260
*/

//#define PATH_DELIMITER_SLASH	//!< パス区切り文字を、'\'から‘/’へ変換する。

//BorlandC++ Compiler用の処理
#ifdef __BORLANDC__
 #define sqrtf(df) sqrt(df)
 int _matherr(struct _exception  *__e) { return 1; }
#endif

#define SetFlag(value, bit) value = value | bit		//!< ビットによるフラグ 設定
#define DelFlag(value, bit) value = value & (~bit)	//!< ビットによるフラグ 解除
#define GetFlag(value, bit) (value & bit)			//!< ビットによるフラグ 取得

//低レイヤー
#if H_LAYERLEVEL >= 1
 #include "window.h"
 #include "config.h"
 #include "datafile.h"
 #include "d3dgraphics.h"
 #include "input.h"
 #include "sound.h"
 #include "parameter.h"
#endif

//中間レイヤー
#if H_LAYERLEVEL >= 2
 #include "resource.h"
 #include "collision.h"
 #include "object.h"
 #include "soundmanager.h"
#endif

//高レイヤー
#if H_LAYERLEVEL >= 3
 #include "objectmanager.h"
 #include "scene.h"
 #include "gamemain.h"
 #include "ai.h"
 #include "statemachine.h"
 #include "event.h"
#endif

//追加ライブラリ
// not .lib

extern class StateMachine GameState;
extern class Config GameConfig;

#endif