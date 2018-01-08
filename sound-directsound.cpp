//! @file sound-directsound.cpp
//! @brief SoundControlクラスの定義（DirectSound版）

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

#ifdef SOUND_DIRECTSOUND

//! @brief コンストラクタ
SoundControl::SoundControl()
{
	pDSound = NULL;
}

//! @brief ディストラクタ
SoundControl::~SoundControl()
{
	DestroySound();
}

//! @brief 初期化
//! @param WindowCtrl WindowControlクラスのポインタ
//! @return 成功：0　失敗：1
int SoundControl::InitSound(WindowControl *WindowCtrl)
{
#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_INIT, "サウンド", "DirectSound");
#endif

	//DirectSoundオブジェクトを生成
	if( FAILED( DirectSoundCreate8(NULL, &pDSound, NULL) ) ){
		return 1;
	}

	//協調レベルの設定
	if( FAILED( pDSound->SetCooperativeLevel(WindowCtrl->GethWnd(), DSSCL_EXCLUSIVE) ) ){
		return 1;
	}

	//プライマリ バッファを作成し、リスナーインターフェイスを取得する
	LPDIRECTSOUNDBUFFER pPrimary;
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	if( SUCCEEDED(pDSound->CreateSoundBuffer(&dsbd, &pPrimary, NULL)) ){
		pPrimary->QueryInterface(IID_IDirectSound3DListener8, (LPVOID *)&p3DListener);
		pPrimary->Release();
	}

	//ドップラー効果を無効に
	p3DListener->SetDopplerFactor(DS3D_MINDOPPLERFACTOR, DS3D_IMMEDIATE);

	//ロール・オフ（減衰度合い）設定
	p3DListener->SetRolloffFactor(0.05f, DS3D_IMMEDIATE);

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
	if( pDSound == NULL ){ return; }

	for(int i=0;i<MAX_LOADSOUND; i++){
		for(int j=0; j<MAX_SOUNDLISTS; j++){
			if( pDSBuffer[i][j] != NULL ){
				pDSBuffer[i][j]->Release();
				pDSBuffer[i][j] = NULL;
			}
		}
	}
	if( pDSound != NULL ){
		pDSound->Release();
		pDSound = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "サウンド", "DirectSound");
#endif
}

//! @brief 再生音量を設定
//! @param volume 再生音量　（0.0＝無音　1.0＝100%）
void SoundControl::SetVolume(float volume)
{
	//
}

//! @brief カメラの座標と角度を設定
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

//! @brief サウンドを読み込む
//! @param filename ファイル名
//! @return 成功：0以上の認識番号　失敗：-1
//! @attention 2チャンネル（ステレオ）データが指定された場合、右側のデータだけ取得され、左側のデータは無視されます。
int SoundControl::LoadSound(char* filename)
{
	if( pDSound == NULL ){ return -1; }

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "サウンド", filename);
#endif

	//開いている番号を探す
	int id=0;
	for(id=0; id<MAX_LOADSOUND; id++){
		if( pDSBuffer[id][0] == NULL ){ break; }
	}
	if( id == MAX_LOADSOUND ){ return -1; }

	WAVEFORMATEX* pwfex;
	int WavSize = 0;
	int Wavoffset = 0;
	bool d2channels = false;

	//Waveファイルの情報を取得
	if( CheckSoundFile(filename, &WavSize, &Wavoffset, &pwfex) == true ){
		return -1;
	}
	pwfex->cbSize = 0;

	//もしステレオデータなら
	if( pwfex->nChannels == 2 ){
		d2channels = true;		//フラグを設定
		WavSize /= 2;			//サイズを半分に

		//モノラルとして強制的に再計算
		pwfex->nChannels = 1;
		pwfex->nAvgBytesPerSec /= 2;
		pwfex->nBlockAlign /= 2;
	}

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
	if( d2channels == true ){
		dwSize = dwSize * 2;
	}
	pWavData = new BYTE[dwSize];
	if( pWavData == NULL ){
		//WAVEファイルを読み込むメモリーが確保できない
		return -1;
	}

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//パス区切り文字を変換
	filename = ChangePathDelimiter(filename);
#endif

	//波形データを取り込む
	FILE* fp;
	fp = fopen(filename, "rb");
	fseek(fp, Wavoffset, SEEK_SET);
	fread(pWavData, 1, dwSize, fp);
	fclose(fp);

	//一時領域からセカンダリバッファーへコピー
	if( d2channels == false ){
		//モノラルデータなら1バイトづつコピーする
		for(int i=0; i<dwSize; i++){
			*((BYTE*)pBuffer+i) = *((BYTE*) pWavData+i);
		}
	}
	else{
		//サンプリングバイト数を取得
		int samplingbytes = pwfex->wBitsPerSample/2;
		
		//ステレオデータなら、右側のデータだけ格納
		int byte = 0;
		for(int i=0; i<dwSize; i++){
			*((BYTE*)pBuffer+byte) = *((BYTE*) pWavData+i);
			byte += 1;
			if( i%samplingbytes == samplingbytes-1 ){ i += samplingbytes; }		//左側のデータはスキップ
		}
	}

	//ロック解除
	pDSBuffer[id][0]->Unlock(pBuffer, dwBufferSize, NULL, 0);

	//一時領域を解放
	delete pWavData;

	//フォーマット情報解放
	delete pwfex;

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

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif

	return id;
}

//! @brief サウンドを再生（非3D再生）
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
			if( FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pDS3DBuffer)) ){
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
			if( FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pDS3DBuffer)) ){
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

//! @brief サウンドを解放
//! @param id 認識番号
void SoundControl::CleanupSound(int id)
{
	if( pDSound == NULL ){ return; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return; }
	if( pDSBuffer[id][0] == NULL ){ return; }

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//再生状況を取得
		pDSBuffer[id][i]->GetStatus(&status);

		//サウンドが再生中ならば停止する
		if( (status & DSBSTATUS_PLAYING) != 0x00 ){
			pDSBuffer[id][i]->Stop();
		}

		//対象のセカンダリバッファーを解放
		if( pDSBuffer[id][i] != NULL ){ pDSBuffer[id][i]->Release(); }
		pDSBuffer[id][i] = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "サウンド", id);
#endif
}

//! @brief Waveファイルの情報を調べる
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
				if( (pcmWaveFormat.wf.nChannels == 1)||(pcmWaveFormat.wf.nChannels == 2) ){
					*pwfex = new WAVEFORMATEX;
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
	*fileoffset = ckInfo.dwDataOffset;
	return false;
}

//! @brief SetVolume()用　1/100 dB (デシベル) を計算
//! @param volume 音量（-50～100）
//! @return 1/100 dB (デシベル).
//! @attention 引数に 156 以上が渡された場合は無音になります。
int SoundControl::GetDSVolume(int volume)
{
	if( volume >= 156 ){ return DSBVOLUME_MIN; }
	if( volume <= -50 ){ return DSBVOLUME_MIN; }
	if( volume >= 100 ){ return DSBVOLUME_MAX; }

	float volume2 = 1.0f/150 * (volume + 50);
	int retn = (int)((DSBVOLUME_MIN-DSBVOLUME_MAX) * (1.0f - volume2));
	return retn;
}

#endif	//SOUND_DIRECTSOUND