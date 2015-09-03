//! @file config.h
//! @brief config�N���X�̐錾

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

#define TOTAL_ControlKey 18		//!< ���v����L�[��
#define MAX_PLAYERNAME 21		//!< �v���C���[���̍ő啶����

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

//! GetKeycode()�p �萔
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

//! @brief �ݒ�t�@�C����ǂݍ��ރN���X
//! @details �t�@�C���̎Q�Ƃƒl�̊Ǘ����s���܂��B
//! @details �Q�l�����F�u�݂��񔠁vhttp://mikan.the-ninja.jp/�@�ˁ@�Z�p�����@�ˁ@config.dat�t�@�C����͎���
class Config
{
	int Keycode[TOTAL_ControlKey];		//!< �I���W�i���L�[�R�[�h
	int MouseSensitivity;				//!< �}�E�X���x
	bool FullscreenFlag;				//!< �t���X�N���[���L��
	bool SoundFlag;						//!< ���ʉ��L��
	bool BloodFlag;						//!< �o���L��
	int Brightness;						//!< ��ʂ̖��邳
	bool InvertMouseFlag;				//!< �}�E�X���]
	bool FrameskipFlag;					//!< �t���[���X�L�b�v
	bool AnotherGunsightFlag;			//!< �ʂ̕W�����g�p
	char PlayerName[MAX_PLAYERNAME];	//!< �v���C���[��

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