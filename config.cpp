//! @file config.cpp
//! @brief configクラスの定義 

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

#include "config.h"

//! コンストラクタ
Config::Config()
{
	//各種メンバー変数初期化
	for(int i=0; i<TOTAL_ControlKey; i++){
		Keycode[i] = 0;
	}
	MouseSensitivity = 0;
	FullscreenFlag = false;
	SoundFlag = false;
	BloodFlag = false;
	Brightness = 0;
	InvertMouseFlag = false;
	FrameskipFlag = false;
	AnotherGunsightFlag = false;
	strcpy(PlayerName, "");
}

//! ディストラクタ
Config::~Config()
{}

//! 設定ファイルを読み込む
//! @param fname ファイル名
//! @return 成功：0　失敗：1
int Config::LoadFile(char *fname)
{
	FILE *fp;
	char buf;

	//ファイルを開く
	fp = fopen(fname, "rb");
	if( fp == NULL ){
		return 1;
	}

	//キーコードを取得
	for(int i=0; i<TOTAL_ControlKey; i++){
		fread(&buf, 1, 1, fp);
		Keycode[i] = buf;
	}

	//マウス感度
	fread(&buf, 1, 1, fp);
	MouseSensitivity = buf;

	//フルスクリーン有効
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ FullscreenFlag = false; }
	else{ FullscreenFlag = true; }

	//効果音有効
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ SoundFlag = false; }
	else{ SoundFlag = true; }

	//出血有効
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ BloodFlag = false; }
	else{ BloodFlag = true; }

	//画面の明るさ
	fread(&buf, 1, 1, fp);
	Brightness = buf;

	//マウス反転
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ InvertMouseFlag = false; }
	else{ InvertMouseFlag = true; }

	//フレームスキップ
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ FrameskipFlag = false; }
	else{ FrameskipFlag = true; }

	//別の標準を使用
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ AnotherGunsightFlag = false; }
	else{ AnotherGunsightFlag = true; }

	//プレイヤー名
	fread(PlayerName, 1, MAX_PLAYERNAME, fp);

	//ファイルハンドルを閉じる
	fclose(fp);
	return 0;
}

//! オリジナルキーコードを取得
//! @param id 定数
//! @return オリジナルキーコード
int Config::GetKeycode(int id)
{
	if( (id < 0)||((TOTAL_ControlKey -1) < id) ){ return 0; }

	return Keycode[id];
}

//! マウス感度取得
//! @return 生の値
int Config::GetMouseSensitivity()
{
	return MouseSensitivity;
}

//! 画面表示モード取得
//! @return ウィンドウ：false　フルスクリーン：true
bool Config::GetFullscreenFlag()
{
	return FullscreenFlag;
}

//! 効果音設定取得
//! @return 無効：false　有効：true
bool Config::GetSoundFlag()
{
	return SoundFlag;
}

//! 出血設定取得
//! @return 無効：false　有効：true
bool Config::GetBloodFlag()
{
	return BloodFlag;
}

//! 画面の明るさ設定取得
//! @return 生の値
int Config::GetBrightness()
{
	return Brightness;
}

//! マウス反転設定取得
//! @return 無効：false　有効：true
bool Config::GetInvertMouseFlag()
{
	return InvertMouseFlag;
}

//! フレームスキップ設定取得
//! @return 無効：false　有効：true
bool Config::GetFrameskipFlag()
{
	return FrameskipFlag;
}

//! 別の照準を使用設定取得
//! @return 無効：false　有効：true
bool Config::GetAnotherGunsightFlag()
{
	return AnotherGunsightFlag;
}

//! プレイヤー名取得
//! @param out_str 受け取る文字列型ポインタ
//! @return プレイヤー名文字数
int Config::GetPlayerName(char *out_str)
{
	if( out_str == NULL ){ return 0; }

	//ポインタにコピーする
	strcpy(out_str, PlayerName);

	//文字数を返す
	return strlen(PlayerName);
}
