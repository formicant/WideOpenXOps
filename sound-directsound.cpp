//! @file sound-directsound.cpp
//! @brief SoundControl�N���X�̒�`�iDirectSound�Łj

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

//! @brief �R���X�g���N�^
SoundControl::SoundControl()
{
	pDSound = NULL;
}

//! @brief �f�B�X�g���N�^
SoundControl::~SoundControl()
{
	DestroySound();
}

//! @brief ������
//! @param WindowCtrl WindowControl�N���X�̃|�C���^
//! @return �����F0�@���s�F1
int SoundControl::InitSound(WindowControl *WindowCtrl)
{
#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_INIT, "�T�E���h", "DirectSound");
#endif

	//DirectSound�I�u�W�F�N�g�𐶐�
	if( FAILED( DirectSoundCreate8(NULL, &pDSound, NULL) ) ){
		return 1;
	}

	//�������x���̐ݒ�
	if( FAILED( pDSound->SetCooperativeLevel(WindowCtrl->GethWnd(), DSSCL_EXCLUSIVE) ) ){
		return 1;
	}

	//�v���C�}�� �o�b�t�@���쐬���A���X�i�[�C���^�[�t�F�C�X���擾����
	LPDIRECTSOUNDBUFFER pPrimary;
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	if( SUCCEEDED(pDSound->CreateSoundBuffer(&dsbd, &pPrimary, NULL)) ){
		pPrimary->QueryInterface(IID_IDirectSound3DListener8, (LPVOID *)&p3DListener);
		pPrimary->Release();
	}

	//�h�b�v���[���ʂ𖳌���
	p3DListener->SetDopplerFactor(DS3D_MINDOPPLERFACTOR, DS3D_IMMEDIATE);

	//���[���E�I�t�i�����x�����j�ݒ�
	p3DListener->SetRolloffFactor(0.05f, DS3D_IMMEDIATE);

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief ���
//! @attention �{�֐��͎����I�ɌĂяo����܂����A�����I�ɌĂяo�����Ƃ��\�ł��B
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
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "�T�E���h", "DirectSound");
#endif
}

//! @brief �Đ����ʂ�ݒ�
//! @param volume �Đ����ʁ@�i0.0�������@1.0��100%�j
void SoundControl::SetVolume(float volume)
{
	//
}

//! @brief �J�����̍��W�Ɗp�x��ݒ�
//! @param x �J������X���W
//! @param y �J������Y���W
//! @param z �J������Z���W
//! @param rx �J������X���p�x�@�i�\��j
//! @warning ���t���[���Ăяo���āA�ŐV�̃J�������W��ݒ�i�K�p�j���Ă��������B
void SoundControl::SetCamera(float x, float y, float z, float rx)
{
	p3DListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
	p3DListener->SetOrientation(cos(rx), 0.0f, sin(rx), 0.0f, 1.0f, 0.0f, DS3D_IMMEDIATE);
}

//! @brief �T�E���h��ǂݍ���
//! @param filename �t�@�C����
//! @return �����F0�ȏ�̔F���ԍ��@���s�F-1
//! @attention 2�`�����l���i�X�e���I�j�f�[�^���w�肳�ꂽ�ꍇ�A�E���̃f�[�^�����擾����A�����̃f�[�^�͖�������܂��B
int SoundControl::LoadSound(char* filename)
{
	if( pDSound == NULL ){ return -1; }

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "�T�E���h", filename);
#endif

	//�J���Ă���ԍ���T��
	int id=0;
	for(id=0; id<MAX_LOADSOUND; id++){
		if( pDSBuffer[id][0] == NULL ){ break; }
	}
	if( id == MAX_LOADSOUND ){ return -1; }

	WAVEFORMATEX* pwfex;
	int WavSize = 0;
	int Wavoffset = 0;
	bool d2channels = false;

	//Wave�t�@�C���̏����擾
	if( CheckSoundFile(filename, &WavSize, &Wavoffset, &pwfex) == true ){
		return -1;
	}
	pwfex->cbSize = 0;

	//�����X�e���I�f�[�^�Ȃ�
	if( pwfex->nChannels == 2 ){
		d2channels = true;		//�t���O��ݒ�
		WavSize /= 2;			//�T�C�Y�𔼕���

		//���m�����Ƃ��ċ����I�ɍČv�Z
		pwfex->nChannels = 1;
		pwfex->nAvgBytesPerSec /= 2;
		pwfex->nBlockAlign /= 2;
	}

	// DirectSound�Z�J���_���[�o�b�t�@�[�쐬
	DSBUFFERDESC dsbd;  
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = WavSize;
	dsbd.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	dsbd.lpwfxFormat = pwfex;
	if( FAILED( pDSound->CreateSoundBuffer(&dsbd, &pDSBuffer[id][0], NULL) ) ){
		return -1;	//�Z�J���_���o�b�t�@�[�쐬���s
	}

	//���b�N����
	VOID* pBuffer = NULL;
	DWORD dwBufferSize = 0; 
	if( FAILED(pDSBuffer[id][0]->Lock(0, WavSize, &pBuffer, &dwBufferSize, NULL, NULL, 0) ) ){
		return -1;	//�o�b�t�@�̃��b�N���s
	}

	//�ꎞ�̈���m��
	BYTE* pWavData;
	int dwSize = dwBufferSize;
	if( dwSize > WavSize ){ dwSize = WavSize; }
	if( d2channels == true ){
		dwSize = dwSize * 2;
	}
	pWavData = new BYTE[dwSize];
	if( pWavData == NULL ){
		//WAVE�t�@�C����ǂݍ��ރ������[���m�ۂł��Ȃ�
		return -1;
	}

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//�p�X��؂蕶����ϊ�
	filename = ChangePathDelimiter(filename);
#endif

	//�g�`�f�[�^����荞��
	FILE* fp;
	fp = fopen(filename, "rb");
	fseek(fp, Wavoffset, SEEK_SET);
	fread(pWavData, 1, dwSize, fp);
	fclose(fp);

	//�ꎞ�̈悩��Z�J���_���o�b�t�@�[�փR�s�[
	if( d2channels == false ){
		//���m�����f�[�^�Ȃ�1�o�C�g�ÂR�s�[����
		for(int i=0; i<dwSize; i++){
			*((BYTE*)pBuffer+i) = *((BYTE*) pWavData+i);
		}
	}
	else{
		//�T���v�����O�o�C�g�����擾
		int samplingbytes = pwfex->wBitsPerSample/2;
		
		//�X�e���I�f�[�^�Ȃ�A�E���̃f�[�^�����i�[
		int byte = 0;
		for(int i=0; i<dwSize; i++){
			*((BYTE*)pBuffer+byte) = *((BYTE*) pWavData+i);
			byte += 1;
			if( i%samplingbytes == samplingbytes-1 ){ i += samplingbytes; }		//�����̃f�[�^�̓X�L�b�v
		}
	}

	//���b�N����
	pDSBuffer[id][0]->Unlock(pBuffer, dwBufferSize, NULL, 0);

	//�ꎞ�̈�����
	delete pWavData;

	//�t�H�[�}�b�g�����
	delete pwfex;

	//�Z�J���_���o�b�t�@�[�̃R�s�[���쐬
	for(int i=1; i<MAX_SOUNDLISTS; i++){
		if( pDSound->DuplicateSoundBuffer(pDSBuffer[id][0], &(pDSBuffer[id][i])) != DS_OK ){
			CleanupSound(id);
			return -1;
		}
	}

	//���ʍŏ��ň�x�Đ����Ă���
	pDSBuffer[id][0]->SetVolume(DSBVOLUME_MIN);
	pDSBuffer[id][0]->Play(NULL, 0, NULL);

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif

	return id;
}

//! @brief �T�E���h���Đ��i��3D�Đ��j
//! @param id �F���ԍ�
//! @param volume �Đ��{�����[��
//! @param pan ��������
//! @return �����F1�@���s�F0
//! @note �p�r�F�v���C���[���g���������鉹�E�Q�[����ԑS�̂ŋψ�ɖ炷���EBGM
int SoundControl::PlaySound(int id, int volume, int pan)
{
	if( pDSound == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return 0; }
	if( pDSBuffer[id][0] == NULL ){ return 0; }

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//�Đ��󋵂��擾
		pDSBuffer[id][i]->GetStatus(&status);

		//�T�E���h����~���Ȃ��
		if( (status & DSBSTATUS_PLAYING) == 0x00 ){
			LPDIRECTSOUND3DBUFFER pDS3DBuffer;
			if( FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pDS3DBuffer)) ){
				//IDirectSound3DBuffer8���擾�ł��Ȃ�
				return 0;
			}

			//3D�Đ��𖳌�
			pDS3DBuffer->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);

			//�{�����[����ݒ肵�Đ�
			pDSBuffer[id][i]->SetVolume( GetDSVolume(volume) );
			pDSBuffer[id][i]->Play(NULL, 0, NULL);
			return 1;
		}
	}

	return 0;
}

//! @brief �T�E���h���Đ��i3D�Đ��j
//! @param id �F���ԍ�
//! @param x ������X���W
//! @param y ������Y���W
//! @param z ������Z���W
//! @param volume �Đ��{�����[��
//! @return �����F1�@���s�F0
//! @note �p�r�F��ΓI�Ȉʒu�����������ɂ�茸������A��ʓI�Ȍ��ʉ��B
int SoundControl::Play3DSound(int id, float x, float y, float z, int volume)
{
	if( pDSound == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return 0; }
	if( pDSBuffer[id][0] == NULL ){ return 0; }

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//�Đ��󋵂��擾
		pDSBuffer[id][i]->GetStatus(&status);

		//�T�E���h����~���Ȃ��
		if( (status & DSBSTATUS_PLAYING) == 0x00 ){

			LPDIRECTSOUND3DBUFFER pDS3DBuffer;
			if( FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pDS3DBuffer)) ){
				//IDirectSound3DBuffer8���擾�ł��Ȃ�
				return 0;
			}

			//�ŏ������ƍő勗���̐ݒ�
			pDS3DBuffer->SetMinDistance(1, DS3D_IMMEDIATE);
			pDS3DBuffer->SetMaxDistance(MAX_SOUNDDIST, DS3D_IMMEDIATE);

			//�����̍��W��ݒ�
			pDS3DBuffer->SetPosition(x, y, z, DS3D_IMMEDIATE);

			//3D�Đ���L��
			pDS3DBuffer->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);

			//�{�����[����ݒ肵�Đ�
			pDSBuffer[id][i]->SetVolume( GetDSVolume(volume) );
			pDSBuffer[id][i]->Play(NULL, 0, NULL);
			return 1;
		}
	}

	return 0;
}

//! @brief �T�E���h�����
//! @param id �F���ԍ�
void SoundControl::CleanupSound(int id)
{
	if( pDSound == NULL ){ return; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return; }
	if( pDSBuffer[id][0] == NULL ){ return; }

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//�Đ��󋵂��擾
		pDSBuffer[id][i]->GetStatus(&status);

		//�T�E���h���Đ����Ȃ�Β�~����
		if( (status & DSBSTATUS_PLAYING) != 0x00 ){
			pDSBuffer[id][i]->Stop();
		}

		//�Ώۂ̃Z�J���_���o�b�t�@�[�����
		if( pDSBuffer[id][i] != NULL ){ pDSBuffer[id][i]->Release(); }
		pDSBuffer[id][i] = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "�T�E���h", id);
#endif
}

//! @brief Wave�t�@�C���̏��𒲂ׂ�
//! @param filename �t�@�C����
//! @param filesize �t�@�C���T�C�Y���󂯎��|�C���^
//! @param fileoffset �f�[�^�I�t�Z�b�g���󂯎��|�C���^
//! @param pwfex WAVEFORMATEX�^���󂯎���d�|�C���^
//! @return �����Ffalse�@���s�Ftrue
bool SoundControl::CheckSoundFile(char* filename, int *filesize, int *fileoffset, WAVEFORMATEX** pwfex)
{
	HMMIO hMmio = NULL;
	MMCKINFO ckInfo;
	MMCKINFO riffckInfo;
	PCMWAVEFORMAT pcmWaveFormat; 
	bool errorflag;

	hMmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if( hMmio == NULL ){
		//�t�@�C�����Ȃ�
		return true;
	}

	//WAVE�t�@�C�����m�F
	errorflag = true;
	if( mmioDescend( hMmio, &riffckInfo, NULL, 0 ) == MMSYSERR_NOERROR ){
		if( (mmioFOURCC('R','I','F','F') == riffckInfo.ckid) && (mmioFOURCC('W','A','V','E') == riffckInfo.fccType) ){
			errorflag = false;
		}
	}
	if( errorflag == true ){
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//WAVE�t�@�C���łȂ�
	}

	//�t�H�[�}�b�g���擾
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
		return true;	//�t�@�C���t�H�[�}�b�g���Ή����ĂȂ�
	}
	if( mmioAscend(hMmio, &ckInfo, 0) != MMSYSERR_NOERROR ){
		delete pwfex;
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//�`�����N���䎸�s
	}

	//�f�[�^�̈�𒲂ׂ�
	ckInfo.ckid = mmioFOURCC('d','a','t','a');
	if( mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK) != MMSYSERR_NOERROR ){
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//�t�@�C���t�H�[�}�b�g���Ή����ĂȂ�
	}

	mmioClose(hMmio, MMIO_FHOPEN);

	*filesize = ckInfo.cksize;
	*fileoffset = ckInfo.dwDataOffset;
	return false;
}

//! @brief SetVolume()�p�@1/100 dB (�f�V�x��) ���v�Z
//! @param volume ���ʁi-50�`100�j
//! @return 1/100 dB (�f�V�x��).
//! @attention ������ 156 �ȏオ�n���ꂽ�ꍇ�͖����ɂȂ�܂��B
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