//! @file datafile.h
//! @brief �f�[�^�Ǘ��N���X�̐錾

//--------------------------------------------------------------------------------
// 
// OpenXOPS
// Copyright (c) 2014-2016, OpenXOPS Project / [-_-;](mikan) All rights reserved.
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

#ifndef DATAFILE_H
#define DATAFILE_H

#define MAX_BLOCKS 160			//!< �ő�u���b�N��
#define TOTAL_BLOCKTEXTURE 10	//!< ���v�e�N�X�`����

#define MAX_POINTS 200			//!< �ő�|�C���g��

#define MAX_POINTMESSAGES 16	//!< .msg�t�@�C���̍ő僁�b�Z�[�W��
#define MAX_POINTMESSAGEBYTE (71+2)	//!< .msg�t�@�C�� 1�s������̍ő�o�C�g��

#define LIGHT_RX DegreeToRadian(190)	//!< ���C�g�̉����p�x�i�u���b�N�̉A�e�v�Z�p�j
#define LIGHT_RY DegreeToRadian(120)	//!< ���C�g�̏c���p�x�i�u���b�N�̉A�e�v�Z�p�j

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

#include <ctype.h>

//! �u���b�N�f�[�^�p�\���́@�iblockdata�\���̂Ŏg�p�j
struct b_material{
	int textureID;				//!< textureID
	float u[4];					//!< textureUV-U
	float v[4];					//!< textureUV-V
	float center_x;				//!< center
	float center_y;				//!< center
	float center_z;				//!< center
	float vx;					//!< normal vector 
	float vy;					//!< normal vector 
	float vz;					//!< normal vector 
	float shadow;				//!< shadow 
};
//! �u���b�N�f�[�^�p�\����
struct blockdata{
	int id;						//!< DataID
	float x[8];					//!< Position
	float y[8];					//!< Position
	float z[8];					//!< Position
	b_material material[6];		//!< Material data
};

//! �|�C���g�f�[�^�p�\����
struct pointdata{
	int id;						//!< DataID
	float x;					//!< Position
	float y;					//!< Position
	float z;					//!< Position
	float r;					//!< Rotation
	signed char p1;				//!< Param
	signed char p2;				//!< Param
	signed char p3;				//!< Param
	signed char p4;				//!< Param
};

//! @brief �f�[�^���Ǘ�����N���X�i���N���X�j
//! @details �Q�[���̃f�[�^���A�t�@�C������ǂݍ��ݏ�������N���X�Q�̊��N���X�ł��B
class DataInterface
{
protected:
	int datas;		//!< �f�[�^��

public:
	DataInterface();
	~DataInterface();
	virtual int LoadFiledata(char *fname);
	virtual int GetTotaldatas();
	virtual int Getdata(void *out_data, int id);
};

//! @brief �u���b�N�f�[�^���Ǘ�����N���X
//! @details �}�b�v�f�[�^�Ƃ��Ďg���A�u���b�N�f�[�^�t�@�C�����Ǘ����܂��B
//! @details �t�@�C���̓ǂݍ��݂̂ق��A�}�b�v�ւ̉e�i�e�ʂ̖��邳�\���j���v�Z���܂��B
class BlockDataInterface : public DataInterface
{
	blockdata *data;								//!< �u���b�N�f�[�^��\���\����
	char texture[TOTAL_BLOCKTEXTURE][_MAX_PATH];		//!< �e�N�X�`����

public:
	BlockDataInterface();
	~BlockDataInterface();
	int LoadFiledata(char *fname);
	void CalculationBlockdata(bool screen);
	int GetTexture(char *fname, int id);
	int Getdata(blockdata *out_data, int id);
};

bool blockdataface(int faceID, int* vID, int* uvID);

//! @brief �|�C���g�f�[�^���Ǘ�����N���X
//! @details �~�b�V�����f�[�^�Ƃ��Ďg���A�|�C���g�f�[�^�t�@�C�����Ǘ����܂��B
//! @details �t�@�C���̓ǂݍ��݂̂ق��A�|�C���g�̌����@�\������܂��B
class PointDataInterface : public DataInterface
{
	pointdata *data;							//!< �u���b�N�f�[�^��\���\����
	char *text[MAX_POINTMESSAGES];				//!< �C�x���g���b�Z�[�W

public:
	PointDataInterface();
	~PointDataInterface();
	int LoadFiledata(char *fname);
	int Getdata(pointdata *out_data, int id);
	int SetParam(int id, signed char p1, signed char p2, signed char p3, signed char p4);
	int GetMessageText(char *str, int id);
	int SearchPointdata(int* id, unsigned char pmask, signed char p1, signed char p2, signed char p3, signed char p4, int offset = 0);
	int SearchPointdata(pointdata *out_data, unsigned char pmask, signed char p1, signed char p2, signed char p3, signed char p4, int offset = 0);
};

//! @brief MIF���Ǘ�����N���X
//! @details MIF�iMISSION INFORMATION FILE�j���Ǘ����܂��B
//! @details �W���~�b�V�����̃u���[�t�B���O�t�@�C���i.txt�j�ɂ��Ή����Ă��܂��B
class MIFInterface : public DataInterface
{
	bool mif;								//!< �t�@�C���`���� .mif
	char mission_name[24];					//!< �~�b�V�������ʖ�
	char mission_fullname[64];				//!< �~�b�V������������
	char blockfile_path[_MAX_PATH];			//!< �u���b�N�f�[�^�t�@�C��
	char pointfile_path[_MAX_PATH];			//!< �|�C���g�f�[�^�t�@�C��
	int skynumber;							//!< �w�i���ID�@�i�Ȃ��F0�j
	char picturefileA_path[_MAX_PATH];		//!< �u���[�t�B���O�摜A
	char picturefileB_path[_MAX_PATH];		//!< �u���[�t�B���O�摜B�i�ǉ����j
	char addsmallobject_path[_MAX_PATH];	//!< �ǉ��������t�@�C���ւ̃p�X
	char briefingtext[816];					//!< �u���[�t�B���O���́E�{��
	bool collision;					//!< �����蔻��𑽂߂ɍs��
	bool screen;					//!< ��ʂ��Â߂ɂ���
	char addsmallobject_modelpath[_MAX_PATH];	//!< �ǉ������̃��f���f�[�^�p�X
	char addsmallobject_texturepath[_MAX_PATH];	//!< �ǉ������̃e�N�X�`���p�X
	int addsmallobject_decide;					//!< �ǉ������̓����蔻��̑傫��
	int addsmallobject_hp;						//!< �ǉ������̑ϋv��
	char addsmallobject_soundpath[_MAX_PATH];	//!< �ǉ������̃T�E���h�f�[�^�p�X
	int addsmallobject_jump;					//!< �ǉ������̔�ы

public:
	MIFInterface();
	~MIFInterface();
	int LoadFiledata(char *fname);
	bool GetFiletype();
	char* GetMissionName();
	char* GetMissionFullname();
	void GetDatafilePath(char *blockfile, char *pointfile);
	int GetSkynumber();
	void GetPicturefilePath(char *picturefileA, char *picturefileB);
	char* GetBriefingText();
	bool GetCollisionFlag();
	bool GetScreenFlag();
	char* GetAddSmallobjectModelPath();
	char* GetAddSmallobjectTexturePath();
	int GetAddSmallobjectDecide();
	int GetAddSmallobjectHP();
	char* GetAddSmallobjectSoundPath();
	int GetAddSmallobjectJump();
};

//! @brief ADDON���X�g���Ǘ�����N���X
//! @details ����̃f�B���N�g���ɓ�����.mif��ADDON���X�g�Ƃ��ĊǗ����܂��B
class AddonList : public DataInterface
{
	char filename[MAX_ADDONLIST][_MAX_PATH];	//!< .mif�t�@�C����
	char mission_name[MAX_ADDONLIST][24];		//!< �~�b�V�������ʖ�

	void GetMIFlist(char *dir);
	void GetMissionName(char *dir);
	void Sort();

public:
	AddonList();
	~AddonList();
	int LoadFiledata(char *dir);
	char *GetMissionName(int id);
	char *GetFileName(int id);
};

int DeleteLinefeed(char str[]);

#endif