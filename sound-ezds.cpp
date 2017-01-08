//! @file sound-directsound.cpp
//! @brief SoundControlクラスの定義（ezds.dll版）

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

#include "sound.h"

#ifndef SOUND_DIRECTSOUND

//! @brief コンストラクタ
SoundControl::SoundControl()
{
	lib = NULL;

	//使用済みフラグを初期化
	for(int i=0; i<MAX_LOADSOUND; i++){
		useflag[i] = false;
	}
}

//! @brief ディストラクタ
SoundControl::~SoundControl()
{
	DestroySound();
}

//! @brief 初期化@n
//! （DLLのロード、初期化関数の実行）
//! @param WindowCtrl WindowControlクラスのポインタ
//! @return 成功：0　失敗：1
int SoundControl::InitSound(WindowControl *WindowCtrl)
{
#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_INIT, "サウンド", "ezds.dll");
#endif

	if( lib != NULL ){
		return 1;
	}

	//DLLを読み込む
	lib = LoadLibrary("ezds.dll");
	if( lib == NULL ){
		return 1;
	}

	//関数を割り当て
	DSver = GetProcAddress(lib, "DSver");
	DSinit = (FARPROCH)GetProcAddress(lib, "DSinit");
	DSend = GetProcAddress(lib, "DSend");
	DSload = (FARPROCCI)GetProcAddress(lib, "DSload");
	DSplay = (FARPROCIII)GetProcAddress(lib, "DSplay");
	DSrelease = (FARPROCI)GetProcAddress(lib, "DSrelease");

	//DLL初期化を実行
	if( DSinit == NULL ){
		//DLLを開放
		FreeLibrary(lib);
		lib = NULL;
		//return 1;
	}
	if( DSinit(WindowCtrl->GethWnd()) == 0 ){
		//DLLを開放
		FreeLibrary(lib);
		lib = NULL;
		//return 1;
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief 解放
//! @attention 本関数は自動的に呼び出されますが、明示的に呼び出すことも可能です。
void SoundControl::DestroySound()
{
	if( lib == NULL ){ return; }

	//使用中のサウンドデータ数を数える
	int total = 0;
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == true ){ total += 1; }
	}

	//サウンドデータを開放し、DLLを終了
	if( DSrelease != NULL ){ DSrelease(total); }
	if( DSend != NULL ){ DSend(); }

	//DLLを開放
	FreeLibrary(lib);
	lib = NULL;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "サウンド", "ezds.dll");
#endif
}

//! @brief 再生音量を設定
//! @param volume 再生音量　（0.0＝無音　1.0＝100%）
void SoundControl::SetVolume(float volume)
{
	mastervolume = volume;
}

//! @brief カメラの座標と角度を設定
//! @param x カメラのX座標
//! @param y カメラのY座標
//! @param z カメラのZ座標
//! @param rx カメラのX軸角度　（予約）
//! @warning 毎フレーム呼び出して、最新のカメラ座標を設定（適用）してください。
void SoundControl::SetCamera(float x, float y, float z, float rx)
{
	camera_x = x;
	camera_y = y;
	camera_z = z;
	camera_rx = rx;
}

//! @brief サウンドを読み込む
//! @param filename ファイル名
//! @return 成功：0以上の認識番号　失敗：-1
int SoundControl::LoadSound(char* filename)
{
	if( lib == NULL ){ return -1; }

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "サウンド", filename);
#endif

	//使用していないデータ番号を探す
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == false ){

#ifdef PATH_DELIMITER_SLASH
			//パス区切り文字を変換
			filename = ChangePathDelimiter(filename);
#endif

			//読み込みを試みる
			if( DSload == NULL ){ return -1; }
			if( DSload(filename, i) == 0 ){ return -1; }

			//小さい音量で一度再生しておく
			if( DSplay == NULL ){ return -1; }
			DSplay(i, -99, 0);

			//使用中を表すフラグをセット
			useflag[i] = true;

#ifdef ENABLE_DEBUGLOG
			//ログに出力
			OutputLog.WriteLog(LOG_COMPLETE, "", i);
#endif
			return i;
		}
	}

	return -1;
}

//! @brief サウンドを再生（非3D再生DLL呼び出し）
//! @param id 認識番号
//! @param volume 再生ボリューム
//! @param pan パン（左右バランス）
//! @return 成功：1　失敗：0
//! @note 用途：プレイヤー自身が発生する音・ゲーム空間全体で均一に鳴らす音・BGM
int SoundControl::PlaySound(int id, int volume, int pan)
{
	if( lib == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return 0; }
	if( useflag[id] == false ){ return 0; }

	//サウンドを再生
	if( DSplay == NULL ){ return 0; }
	return DSplay(id, (int)(mastervolume * volume), pan);
}

//! @brief サウンドを再生（3D再生）
//! @param id 認識番号
//! @param x 音源のX座標
//! @param y 音源のY座標
//! @param z 音源のZ座標
//! @param volume 再生ボリューム
//! @return 成功：1　失敗：0
//! @note 用途：絶対的な位置を持ち距離により減衰する、一般的な効果音。
int SoundControl::Play3DSound(int id, float x, float y, float z, int volume)
{
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return 0; }
	if( useflag[id] == false ){ return 0; }

	float dist;
	int playvolume;
	int pan = 0;
	
	//距離による再生音量決定
	if( CheckSourceDist(x, y, z, false, &dist) == false ){
		return 0;
	}
	playvolume = CalculationVolume(volume, dist, false);

	/*
	//左右のパン（再生バランス）の決定
	float vx = x - camera_x;
	float vz = z - camera_z;
	float rx = (atan2(vz, vx) - camera_rx) * -1;
	for(; rx > (float)M_PI; rx -= (float)M_PI*2){}
	for(; rx < (float)M_PI*-1; rx += (float)M_PI*2){}
	pan = (int)((float)10 / M_PI * rx);
	*/

	//DLL呼び出し
	return PlaySound(id, playvolume, pan);
}

//! @brief サウンドを解放
//! @param id 認識番号
void SoundControl::CleanupSound(int id)
{
	if( lib == NULL ){ return; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return; }
	if( useflag[id] == false ){ return; }

	//読み込みを意図的に失敗させ、強制的に初期化
	if( DSload != NULL ){ DSload("", id); }

	//使用中フラグを解除
	useflag[id] = false;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "サウンド", id);
#endif
}

//! @brief 音源との距離を調べる
//! @param x 音源のX座標
//! @param y 音源のY座標
//! @param z 音源のZ座標
//! @param snear 近距離音源
//! @param out_dist 距離
//! @return 有効（内）：true　無効（外）：false
bool SoundControl::CheckSourceDist(float x, float y, float z, bool snear, float *out_dist)
{
	float dx, dy, dz, dist;
	int max_dist;

	if( snear == false ){
		max_dist = MAX_SOUNDDIST;
	}
	else{
		max_dist = 30;
	}

	dx = camera_x - x;
	dy = camera_y - y;
	dz = camera_z - z;
	dist = dx*dx + dy*dy + dz*dz;
	if( dist > max_dist * max_dist ){
		*out_dist = 0.0f;
		return false;
	}

	*out_dist = sqrt(dist);
	return true;
}

//! @brief 音量を計算
//! @param MaxVolume 音源の最大音量
//! @param dist 音源との距離
//! @param snear 近距離音源
int SoundControl::CalculationVolume(int MaxVolume, float dist, bool snear)
{
	int max_dist;
	if( snear == false ){
		max_dist = MAX_SOUNDDIST;
	}
	else{
		max_dist = 30;
	}
	return (int)( (float)MaxVolume/max_dist*dist*-1 + MaxVolume );
}

#endif	//SOUND_DIRECTSOUND