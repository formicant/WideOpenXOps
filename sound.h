//! @file sound.h
//! @brief SoundControlクラスの宣言

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

#ifndef SOUND2_H
#define SOUND2_H

#define MAX_LOADSOUND 24		//!< サウンドを読み込める最大数（ezds.dll）
#define MAX_SOUNDLISTS 100		//!< サウンドを再生する最大数（DirectSound）
#define MAX_SOUNDDIST 335		//!< サウンドを再生する最大距離

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"
#include <windows.h>

//#define SOUND_DIRECTSOUND	//!< @brief サウンドの再生ライブラリを選択 @details 定数宣言有効：DirectSound　定数宣言無効（コメント化）：ezds.dll

#ifdef SOUND_DIRECTSOUND
 #include <dsound.h>
 #include <mmsystem.h>
 #pragma comment(lib, "dsound.lib")
 #pragma comment(lib, "dxguid.lib")
 #pragma comment(lib, "winmm.lib")
#else	//#ifdef SOUND_DIRECTSOUND
 typedef int (*FARPROCH)(HWND);			//!< DLL Parameter
 typedef int (*FARPROCCI)(char*, int);	//!< DLL Parameter
 typedef int (*FARPROCIII)(int, int, int);		//!< DLL Parameter
 typedef int (*FARPROCI)(int);			//!< DLL Parameter
#endif	//#ifdef SOUND_DIRECTSOUND

//! @brief サウンドを再生するクラス
//! @details サウンドの読み込みから再生までを管理します。
//! @details 内部では ezds.dll を呼び出して使用しています。
//! @details 参考資料：「みかん箱」http://mikan.the-ninja.jp/　⇒　技術資料　⇒　ezds.dllファイル解析資料
class SoundControl
{
#ifdef SOUND_DIRECTSOUND
	LPDIRECTSOUND8 pDSound;					//!< DIRECTSOUND8のポインタ
	LPDIRECTSOUNDBUFFER pDSBuffer[MAX_LOADSOUND][MAX_SOUNDLISTS];	//!< セカンダリーバッファー
	LPDIRECTSOUND3DLISTENER8 p3DListener;	//!< リスナー

	bool CheckSoundFile(char* filename, int *filesize, int *fileoffset, WAVEFORMATEX** pwfex);
	int GetDSVolume(int volume);

public:
	SoundControl();
	~SoundControl();
	int InitSound(WindowControl *WindowCtrl);
	void SetVolume(float volume);
	void SetCamera(float x, float y, float z, float rx);
	int LoadSound(char* filename);
	int PlaySound(int id, int volume, int pan);
	int Play3DSound(int id, float x, float y, float z, int volume);
	void CleanupSound(int id);
#else	//#ifdef SOUND_DIRECTSOUND
	HINSTANCE lib;		//!< DLLファイルのインスタンス
	FARPROC DSver;		//!< DSver()
	FARPROCH DSinit;	//!< DSinit()
	FARPROC DSend;		//!< DSend()
	FARPROCCI DSload;	//!< DSload()
	FARPROCIII DSplay;	//!< DSplay()
	FARPROCI DSrelease;	//!< DSrelease()
	bool useflag[MAX_LOADSOUND];	//!< 使用中のサウンド番号を管理する配列

	float camera_x;			//!< カメラ座標
	float camera_y;			//!< カメラ座標
	float camera_z;			//!< カメラ座標
	float camera_rx;		//!< カメラX軸角度
	float mastervolume;		//!< 音量


	bool CheckSourceDist(float x, float y, float z, bool near, float *out_dist);
	int CalculationVolume(int MaxVolume, float dist, bool near);

public:
	SoundControl();
	~SoundControl();
	int InitSound(WindowControl *WindowCtrl);
	void SetVolume(float volume);
	void SetCamera(float x, float y, float z, float rx);
	int LoadSound(char* filename);
	int PlaySound(int id, int volume, int pan);
	int Play3DSound(int id, float x, float y, float z, int volume);
	void CleanupSound(int id);
#endif	//#ifdef SOUND_DIRECTSOUND
};

#endif