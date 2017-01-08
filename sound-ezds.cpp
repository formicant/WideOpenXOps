//! @file sound-directsound.cpp
//! @brief SoundControl�N���X�̒�`�iezds.dll�Łj

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

//! @brief �R���X�g���N�^
SoundControl::SoundControl()
{
	lib = NULL;

	//�g�p�ς݃t���O��������
	for(int i=0; i<MAX_LOADSOUND; i++){
		useflag[i] = false;
	}
}

//! @brief �f�B�X�g���N�^
SoundControl::~SoundControl()
{
	DestroySound();
}

//! @brief ������@n
//! �iDLL�̃��[�h�A�������֐��̎��s�j
//! @param WindowCtrl WindowControl�N���X�̃|�C���^
//! @return �����F0�@���s�F1
int SoundControl::InitSound(WindowControl *WindowCtrl)
{
#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_INIT, "�T�E���h", "ezds.dll");
#endif

	if( lib != NULL ){
		return 1;
	}

	//DLL��ǂݍ���
	lib = LoadLibrary("ezds.dll");
	if( lib == NULL ){
		return 1;
	}

	//�֐������蓖��
	DSver = GetProcAddress(lib, "DSver");
	DSinit = (FARPROCH)GetProcAddress(lib, "DSinit");
	DSend = GetProcAddress(lib, "DSend");
	DSload = (FARPROCCI)GetProcAddress(lib, "DSload");
	DSplay = (FARPROCIII)GetProcAddress(lib, "DSplay");
	DSrelease = (FARPROCI)GetProcAddress(lib, "DSrelease");

	//DLL�����������s
	if( DSinit == NULL ){
		//DLL���J��
		FreeLibrary(lib);
		lib = NULL;
		//return 1;
	}
	if( DSinit(WindowCtrl->GethWnd()) == 0 ){
		//DLL���J��
		FreeLibrary(lib);
		lib = NULL;
		//return 1;
	}

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
	if( lib == NULL ){ return; }

	//�g�p���̃T�E���h�f�[�^���𐔂���
	int total = 0;
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == true ){ total += 1; }
	}

	//�T�E���h�f�[�^���J�����ADLL���I��
	if( DSrelease != NULL ){ DSrelease(total); }
	if( DSend != NULL ){ DSend(); }

	//DLL���J��
	FreeLibrary(lib);
	lib = NULL;

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "�T�E���h", "ezds.dll");
#endif
}

//! @brief �Đ����ʂ�ݒ�
//! @param volume �Đ����ʁ@�i0.0�������@1.0��100%�j
void SoundControl::SetVolume(float volume)
{
	mastervolume = volume;
}

//! @brief �J�����̍��W�Ɗp�x��ݒ�
//! @param x �J������X���W
//! @param y �J������Y���W
//! @param z �J������Z���W
//! @param rx �J������X���p�x�@�i�\��j
//! @warning ���t���[���Ăяo���āA�ŐV�̃J�������W��ݒ�i�K�p�j���Ă��������B
void SoundControl::SetCamera(float x, float y, float z, float rx)
{
	camera_x = x;
	camera_y = y;
	camera_z = z;
	camera_rx = rx;
}

//! @brief �T�E���h��ǂݍ���
//! @param filename �t�@�C����
//! @return �����F0�ȏ�̔F���ԍ��@���s�F-1
int SoundControl::LoadSound(char* filename)
{
	if( lib == NULL ){ return -1; }

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "�T�E���h", filename);
#endif

	//�g�p���Ă��Ȃ��f�[�^�ԍ���T��
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == false ){

#ifdef PATH_DELIMITER_SLASH
			//�p�X��؂蕶����ϊ�
			filename = ChangePathDelimiter(filename);
#endif

			//�ǂݍ��݂����݂�
			if( DSload == NULL ){ return -1; }
			if( DSload(filename, i) == 0 ){ return -1; }

			//���������ʂň�x�Đ����Ă���
			if( DSplay == NULL ){ return -1; }
			DSplay(i, -99, 0);

			//�g�p����\���t���O���Z�b�g
			useflag[i] = true;

#ifdef ENABLE_DEBUGLOG
			//���O�ɏo��
			OutputLog.WriteLog(LOG_COMPLETE, "", i);
#endif
			return i;
		}
	}

	return -1;
}

//! @brief �T�E���h���Đ��i��3D�Đ�DLL�Ăяo���j
//! @param id �F���ԍ�
//! @param volume �Đ��{�����[��
//! @param pan �p���i���E�o�����X�j
//! @return �����F1�@���s�F0
//! @note �p�r�F�v���C���[���g���������鉹�E�Q�[����ԑS�̂ŋψ�ɖ炷���EBGM
int SoundControl::PlaySound(int id, int volume, int pan)
{
	if( lib == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return 0; }
	if( useflag[id] == false ){ return 0; }

	//�T�E���h���Đ�
	if( DSplay == NULL ){ return 0; }
	return DSplay(id, (int)(mastervolume * volume), pan);
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
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return 0; }
	if( useflag[id] == false ){ return 0; }

	float dist;
	int playvolume;
	int pan = 0;
	
	//�����ɂ��Đ����ʌ���
	if( CheckSourceDist(x, y, z, false, &dist) == false ){
		return 0;
	}
	playvolume = CalculationVolume(volume, dist, false);

	/*
	//���E�̃p���i�Đ��o�����X�j�̌���
	float vx = x - camera_x;
	float vz = z - camera_z;
	float rx = (atan2(vz, vx) - camera_rx) * -1;
	for(; rx > (float)M_PI; rx -= (float)M_PI*2){}
	for(; rx < (float)M_PI*-1; rx += (float)M_PI*2){}
	pan = (int)((float)10 / M_PI * rx);
	*/

	//DLL�Ăяo��
	return PlaySound(id, playvolume, pan);
}

//! @brief �T�E���h�����
//! @param id �F���ԍ�
void SoundControl::CleanupSound(int id)
{
	if( lib == NULL ){ return; }
	if( (id < 0)||(MAX_LOADSOUND -1 < id) ){ return; }
	if( useflag[id] == false ){ return; }

	//�ǂݍ��݂��Ӑ}�I�Ɏ��s�����A�����I�ɏ�����
	if( DSload != NULL ){ DSload("", id); }

	//�g�p���t���O������
	useflag[id] = false;

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "�T�E���h", id);
#endif
}

//! @brief �����Ƃ̋����𒲂ׂ�
//! @param x ������X���W
//! @param y ������Y���W
//! @param z ������Z���W
//! @param snear �ߋ�������
//! @param out_dist ����
//! @return �L���i���j�Ftrue�@�����i�O�j�Ffalse
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

//! @brief ���ʂ��v�Z
//! @param MaxVolume �����̍ő剹��
//! @param dist �����Ƃ̋���
//! @param snear �ߋ�������
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