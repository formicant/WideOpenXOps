//! @file config.cpp
//! @brief config�N���X�̒�` 

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
// * Neither the name of the OpenXOPS Project nor the�@names of its contributors 
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

//! �R���X�g���N�^
Config::Config()
{
	//�e�탁���o�[�ϐ�������
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

//! �f�B�X�g���N�^
Config::~Config()
{}

//! �ݒ�t�@�C����ǂݍ���
//! @param fname �t�@�C����
//! @return �����F0�@���s�F1
int Config::LoadFile(char *fname)
{
	FILE *fp;
	char buf;

	//�t�@�C�����J��
	fp = fopen(fname, "rb");
	if( fp == NULL ){
		return 1;
	}

	//�L�[�R�[�h���擾
	for(int i=0; i<TOTAL_ControlKey; i++){
		fread(&buf, 1, 1, fp);
		Keycode[i] = buf;
	}

	//�}�E�X���x
	fread(&buf, 1, 1, fp);
	MouseSensitivity = buf;

	//�t���X�N���[���L��
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ FullscreenFlag = false; }
	else{ FullscreenFlag = true; }

	//���ʉ��L��
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ SoundFlag = false; }
	else{ SoundFlag = true; }

	//�o���L��
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ BloodFlag = false; }
	else{ BloodFlag = true; }

	//��ʂ̖��邳
	fread(&buf, 1, 1, fp);
	Brightness = buf;

	//�}�E�X���]
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ InvertMouseFlag = false; }
	else{ InvertMouseFlag = true; }

	//�t���[���X�L�b�v
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ FrameskipFlag = false; }
	else{ FrameskipFlag = true; }

	//�ʂ̕W�����g�p
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ AnotherGunsightFlag = false; }
	else{ AnotherGunsightFlag = true; }

	//�v���C���[��
	fread(PlayerName, 1, MAX_PLAYERNAME, fp);

	//�t�@�C���n���h�������
	fclose(fp);
	return 0;
}

//! �I���W�i���L�[�R�[�h���擾
//! @param id �萔
//! @return �I���W�i���L�[�R�[�h
int Config::GetKeycode(int id)
{
	if( (id < 0)||((TOTAL_ControlKey -1) < id) ){ return 0; }

	return Keycode[id];
}

//! �}�E�X���x�擾
//! @return ���̒l
int Config::GetMouseSensitivity()
{
	return MouseSensitivity;
}

//! ��ʕ\�����[�h�擾
//! @return �E�B���h�E�Ffalse�@�t���X�N���[���Ftrue
bool Config::GetFullscreenFlag()
{
	return FullscreenFlag;
}

//! ���ʉ��ݒ�擾
//! @return �����Ffalse�@�L���Ftrue
bool Config::GetSoundFlag()
{
	return SoundFlag;
}

//! �o���ݒ�擾
//! @return �����Ffalse�@�L���Ftrue
bool Config::GetBloodFlag()
{
	return BloodFlag;
}

//! ��ʂ̖��邳�ݒ�擾
//! @return ���̒l
int Config::GetBrightness()
{
	return Brightness;
}

//! �}�E�X���]�ݒ�擾
//! @return �����Ffalse�@�L���Ftrue
bool Config::GetInvertMouseFlag()
{
	return InvertMouseFlag;
}

//! �t���[���X�L�b�v�ݒ�擾
//! @return �����Ffalse�@�L���Ftrue
bool Config::GetFrameskipFlag()
{
	return FrameskipFlag;
}

//! �ʂ̏Ə����g�p�ݒ�擾
//! @return �����Ffalse�@�L���Ftrue
bool Config::GetAnotherGunsightFlag()
{
	return AnotherGunsightFlag;
}

//! �v���C���[���擾
//! @param out_str �󂯎�镶����^�|�C���^
//! @return �v���C���[��������
int Config::GetPlayerName(char *out_str)
{
	if( out_str == NULL ){ return 0; }

	//�|�C���^�ɃR�s�[����
	strcpy(out_str, PlayerName);

	//��������Ԃ�
	return strlen(PlayerName);
}
