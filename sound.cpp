//! @file sound.cpp
//! @brief SoundControlクラスの定義

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

#include "sound.h"

#ifdef SOUND_DIRECTSOUND

//! コンストラクタ
SoundControl::SoundControl()
{
	pDSound = NULL;
}

//! ディストラクタ
SoundControl::~SoundControl()
{
	for(int i=0;i<MAX_LOADSOUND; i++){
		for(int j=0; j<MAX_SOUNDLISTS; j++){
			if( pDSBuffer[i][j] != NULL ){ pDSBuffer[i][j]->Release(); }
		}
	}
	if( pDSound != NULL ){ pDSound->Release(); }
}

//! 初期化
//! @param hWnd ウィンドウハンドル
//! @return 成功：0　失敗：1
int SoundControl::InitSound(HWND hWnd)
{
	//DirectSoundオブジェクトを生成
	if( FAILED( DirectSoundCreate8(NULL, &pDSound, NULL) ) ){
		return 1;
	}

	//協調レベルの設定
	if( FAILED( pDSound->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE) ) ){
		return 1;
	}

	//プライマリ バッファを作成し、リスナーインターフェイスを取得する
	LPDIRECTSOUNDBUFFER pPrimary;
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	if( SUCCEEDED(pDSound->CreateSoundBuffer(&dsbd, &pPrimary, NULL)) ){
		pPrimary->QueryInterface(IID_IDirectSound3DListener8,(LPVOID *)&p3DListener);
		pPrimary->Release();
	}

	//ドップラー効果を無効に
	p3DListener->SetDopplerFactor(DS3D_MINDOPPLERFACTOR, DS3D_IMMEDIATE);

	//ロール・オフ（減衰度合い）設定
	p3DListener->SetRolloffFactor(0.05f, DS3D_IMMEDIATE);

	return 0;
}

//! 再生音量を設定
//! @param volume 再生音量　（0.0＝無音　1.0＝100%）
void SoundControl::SetVolume(float volume)
{
	//
}

//! カメラの座標と角度を設定
//! @param x カメラのX座標
//! @param y カメラのY座標
//! @param z カメラのZ座標
//! @param rx カメラのX軸角度　（予約）
//! @warning 毎フレーム呼び出して、最新のカメラ座標を設定（適用）してください。
void SoundControl::SetCamera(float x, float y, float z, float rx)
{
	p3DListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
	p3DListener->SetOrientation(cos(rx), 0.0f, sin(rx), 0.0f, 1.0f, 0.0f, DS3D_IMMEDIATE);
}

//! サウンドを読み込む
//! @param filename ファイル名
//! @return 成功：0以上の認識番号　失敗：-1
int SoundControl::LoadSound(char* filename)
{
	if( pDSound == NULL ){ return -1; }

	//開いている番号を探す
	int id=0;
	for(id=0; id<MAX_LOADSOUND; id++){
		if( pDSBuffer[id][0] == NULL ){ break; }
	}
	if( id == MAX_LOADSOUND ){ return -1; }

	WAVEFORMATEX* pwfex;
	int WavSize = 0;
	int Wavoffset = 0;

	//Waveファイルの情報を取得
	if( CheckSoundFile(filename, &WavSize, &Wavoffset, &pwfex) == true ){
		return -1;
	}
	pwfex->cbSize = 0;

	// DirectSoundセカンダリーバッファー作成
	DSBUFFERDESC dsbd;  
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = WavSize;
	dsbd.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	dsbd.lpwfxFormat = pwfex;
	if( FAILED( pDSound->CreateSoundBuffer(&dsbd, &pDSBuffer[id][0], NULL) ) ){
		return -1;	//セカンダリバッファー作成失敗
	}

	//ロックする
	VOID* pBuffer = NULL;
	DWORD dwBufferSize = 0; 
	if( FAILED(pDSBuffer[id][0]->Lock(0, WavSize, &pBuffer, &dwBufferSize, NULL, NULL, 0) ) ){
		return -1;	//バッファのロック失敗
	}

	//一時領域を確保
	BYTE* pWavData;
	int dwSize = dwBufferSize;
	if( dwSize > WavSize ){ dwSize = WavSize; }
	pWavData = new BYTE[dwSize];
	if( pWavData == NULL ){
		//WAVEファイルを読み込むメモリーが確保できない
		return -1;
	}

	//波形データを取り込む
	FILE* fp;
	fp = fopen(filename,"rb");
	fseek(fp, Wavoffset, SEEK_SET);
	fread(pWavData, 1, dwSize, fp);
	fclose(fp);

	//一時領域からセカンダリバッファーへコピー
	for(int i=0; i<dwSize; i++){
		*((BYTE*)pBuffer+i) = *((BYTE*) pWavData+i);
	}

	//ロック解除
	pDSBuffer[id][0]->Unlock(pBuffer, dwBufferSize, NULL, 0);

	//一時領域を解放
	delete pWavData;

	//セカンダリバッファーのコピーを作成
	for(int i=1; i<MAX_SOUNDLISTS; i++){
		if( pDSound->DuplicateSoundBuffer(pDSBuffer[id][0], &(pDSBuffer[id][i])) != DS_OK ){
			CleanupSound(id);
			return -1;
		}
	}

	//音量最小で一度再生しておく
	pDSBuffer[id][0]->SetVolume(DSBVOLUME_MIN);
	pDSBuffer[id][0]->Play(NULL, 0, NULL);

	return id;
}

//! サウンドを再生（非3D再生）
//! @param id 認識番号
//! @param volume 再生ボリューム
//! @param pan ＜無効＞
//! @return 成功：1　失敗：0
//! @note 用途：プレイヤー自身が発生する音・ゲーム空間全体で均一に鳴らす音・BGM
int SoundControl::PlaySound(int id, int volume, int pan)
{
	if( pDSound == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return 0; }
	if( pDSBuffer[id][0] == NULL ){ return 0; }

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//再生状況を取得
		pDSBuffer[id][i]->GetStatus(&status);

		//サウンドが停止中ならば
		if( (status & DSBSTATUS_PLAYING) == 0x00 ){
			LPDIRECTSOUND3DBUFFER pDS3DBuffer;
			DWORD status = 0;
			if( FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8,(VOID**)&pDS3DBuffer)) ){
				//IDirectSound3DBuffer8を取得できない
				return 0;
			}

			//3D再生を無効
			pDS3DBuffer->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);

			//ボリュームを設定し再生
			pDSBuffer[id][i]->SetVolume( GetDSVolume(volume) );
			pDSBuffer[id][i]->Play(NULL, 0, NULL);
			return 1;
		}
	}

	return 0;
}

//! サウンドを再生（3D再生）
//! @param id 認識番号
//! @param x 音源のX座標
//! @param y 音源のY座標
//! @param z 音源のZ座標
//! @param volume 再生ボリューム
//! @return 成功：1　失敗：0
//! @note 用途：絶対的な位置を持ち距離により減衰する、一般的な効果音。
int SoundControl::Play3DSound(int id, float x, float y, float z, int volume)
{
	if( pDSound == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return 0; }
	if( pDSBuffer[id][0] == NULL ){ return 0; }

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//再生状況を取得
		pDSBuffer[id][i]->GetStatus(&status);

		//サウンドが停止中ならば
		if( (status & DSBSTATUS_PLAYING) == 0x00 ){

			LPDIRECTSOUND3DBUFFER pDS3DBuffer;
			if( FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8,(VOID**)&pDS3DBuffer)) ){
				//IDirectSound3DBuffer8を取得できない
				return 0;
			}

			//最小距離と最大距離の設定
			pDS3DBuffer->SetMinDistance(1, DS3D_IMMEDIATE);
			pDS3DBuffer->SetMaxDistance(MAX_SOUNDDIST, DS3D_IMMEDIATE);

			//音源の座標を設定
			pDS3DBuffer->SetPosition(x, y, z, DS3D_IMMEDIATE);

			//3D再生を有効
			pDS3DBuffer->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);

			//ボリュームを設定し再生
			pDSBuffer[id][i]->SetVolume( GetDSVolume(volume) );
			pDSBuffer[id][i]->Play(NULL, 0, NULL);
			return 1;
		}
	}

	return 0;
}

//! サウンドを解放
//! @param id 認識番号
void SoundControl::CleanupSound(int id)
{
	if( pDSound == NULL ){ return; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return; }
	if( pDSBuffer[id][0] == NULL ){ return; }

	//対象の全セカンダリバッファーを解放
	for(int i=0; i<MAX_SOUNDLISTS; i++){
		if( pDSBuffer[id][i] != NULL ){ pDSBuffer[id][i]->Release(); }
		pDSBuffer[id][i] = NULL;
	}
}

//! Waveファイルの情報を調べる
//! @param filename ファイル名
//! @param filesize ファイルサイズを受け取るポインタ
//! @param fileoffset データオフセットを受け取るポインタ
//! @param pwfex WAVEFORMATEX型を受け取る二重ポインタ
//! @return 成功：false　失敗：true
bool SoundControl::CheckSoundFile(char* filename, int *filesize, int *fileoffset, WAVEFORMATEX** pwfex)
{
	HMMIO hMmio = NULL;
	MMCKINFO ckInfo;
	MMCKINFO riffckInfo;
	PCMWAVEFORMAT pcmWaveFormat; 
	bool errorflag;

	hMmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if( hMmio == NULL ){
		//ファイルがない
		return true;
	}

	//WAVEファイルか確認
	errorflag = true;
	if( mmioDescend( hMmio, &riffckInfo, NULL, 0 ) == MMSYSERR_NOERROR ){
		if( (mmioFOURCC('R','I','F','F') == riffckInfo.ckid) && (mmioFOURCC('W','A','V','E') == riffckInfo.fccType) ){
			errorflag = false;
		}
	}
	if( errorflag == true ){
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//WAVEファイルでない
	}

	//フォーマット情報取得
	*pwfex = NULL;
	ckInfo.ckid = mmioFOURCC('f','m','t',' ');
	if( mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK) == MMSYSERR_NOERROR ){
		if( mmioRead(hMmio, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat)) == sizeof(pcmWaveFormat) ){
			if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM ){
				if( pcmWaveFormat.wf.nChannels == 1 ){	//3D
					*pwfex = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
					if( *pwfex != NULL ){
						memcpy( *pwfex, &pcmWaveFormat, sizeof(pcmWaveFormat) );
						//pwfex->cbSize = 0;
					}
				}
			}
		}
	}
	if( *pwfex == NULL ){
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//ファイルフォーマットが対応してない
	}
	if( mmioAscend(hMmio, &ckInfo, 0) != MMSYSERR_NOERROR ){
		delete pwfex;
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//チャンク制御失敗
	}

	//データ領域を調べる
	ckInfo.ckid = mmioFOURCC('d','a','t','a');
	if( mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK) != MMSYSERR_NOERROR ){
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//ファイルフォーマットが対応してない
	}

	mmioClose(hMmio, MMIO_FHOPEN);

	*filesize = ckInfo.cksize;
	*fileoffset = riffckInfo.dwDataOffset + sizeof(FOURCC);
	return false;
}

//! SetVolume()用　1/100 dB (デシベル) を計算
//! @param volume 音量（0～155）
//! @return 1/100 dB (デシベル)
int SoundControl::GetDSVolume(int volume)
{
	if( volume <= 0 ){ return DSBVOLUME_MIN; }
	if( volume >= 155 ){ return DSBVOLUME_MAX; }

	float volume2 = 1.0f/155 * volume;
	return (int)(10.0f * (log10f(volume2) / log10f(2)));
}

#else

//! コンストラクタ
SoundControl::SoundControl()
{
	lib = NULL;

	//使用済みフラグを初期化
	for(int i=0; i<MAX_LOADSOUND; i++){
		useflag[i] = false;
	}
}

//! ディストラクタ
SoundControl::~SoundControl()
{
	if( lib == NULL ){ return; }

	//使用中のサウンドデータ数を数える
	int total = 0;
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == true ){ total += 1; }
	}

	//サウンドデータを開放し、DLLを終了
	DSrelease(total);
	DSend();

	//DLLを開放
	FreeLibrary(lib);
}

//! 初期化@n
//! （DLLのロード、初期化関数の実行）
//! @param hWnd ウィンドウハンドル
//! @return 成功：0　失敗：1
int SoundControl::InitSound(HWND hWnd)
{
	if( lib != NULL ){
		return 1;
	}

	//DLLを読み込む
	lib = LoadLibrary("ezds.dll");
	if (lib == NULL){
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
	if( DSinit(hWnd) == 0 ){
		//DLLを開放
		FreeLibrary(lib);
		lib = NULL;
		//return 1;
	}

	return 0;
}

//! 再生音量を設定
//! @param volume 再生音量　（0.0＝無音　1.0＝100%）
void SoundControl::SetVolume(float volume)
{
	mastervolume = volume;
}

//! カメラの座標と角度を設定
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

//! サウンドを読み込む
//! @param filename ファイル名
//! @return 成功：0以上の認識番号　失敗：-1
int SoundControl::LoadSound(char* filename)
{
	if( lib == NULL ){ return -1; }

	//使用していないデータ番号を探す
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == false ){

			//読み込みを試みる
			if( DSload(filename, i) == 0 ){ return -1; }

			//使用中を新たすフラグをセット
			useflag[i] = true;
			return i;
		}
	}

	return -1;
}

//! サウンドを再生（非3D再生DLL呼び出し）
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
	return DSplay(id, (int)(mastervolume * volume), pan);
}

//! サウンドを再生（3D再生）
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

//! サウンドを解放
//! @param id 認識番号
void SoundControl::CleanupSound(int id)
{
	if( lib == NULL ){ return; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return; }
	if( useflag[id] == false ){ return; }

	//読み込みを意図的に失敗させ、強制的に初期化
	DSload("", id);

	//使用中フラグを解除
	useflag[id] = false;
}

//! 音源との距離を調べる
//! @param x 音源のX座標
//! @param y 音源のY座標
//! @param z 音源のZ座標
//! @param snear 近距離音源
//! @param out_dist 距離
//! @return 有効（内）：true　無効（外）：false
bool SoundControl::CheckSourceDist(float x, float y, float z, bool snear, float *out_dist)
{
	float dx, dy ,dz, dist;
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

//! 音量を計算
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

#endif