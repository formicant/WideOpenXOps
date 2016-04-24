//! @file objectmanager.h
//! @brief ObjectManager�N���X�̐錾

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

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#define MAX_HUMAN 96			//!< �ő�l���@�y�x���z�FAI���x�������̊֌W��e24�̔{���f�ɂ��邱�ƁI
#define MAX_WEAPON 200			//!< ����̍ő吔
#define MAX_SMALLOBJECT 40		//!< �����̍ő吔

#define MAX_BULLET 128			//!< �e�e�̍ő吔
#define MAX_GRENADE 32			//!< ��֒e�̍ő吔
#define MAX_EFFECT 256			//!< �G�t�F�N�g�̍ő吔

#define WEAPONSHOT_HEIGHT (VIEW_HEIGHT)	//!< �e�𔭎˂��鍂��

#define TOTAL_WEAPON_AUTOBULLET 3	//!< ���������Ɏ����I�ɕ�Ă񂳂��e���i���e���̉��{���j

#define HUMAN_BULLETCOLLISION_HEAD_H 2.8f	//!< ���̓����蔻��̍���
#define HUMAN_BULLETCOLLISION_HEAD_R 1.3f	//!< ���̓����蔻��̔��a
#define HUMAN_BULLETCOLLISION_UP_H 7.5f		//!< �㔼�g�̓����蔻��̍���
#define HUMAN_BULLETCOLLISION_UP_R 2.5f		//!< �㔼�g�̓����蔻��̔��a
#define HUMAN_BULLETCOLLISION_LEG_H 10.0f	//!< ���̓����蔻��̍���
#define HUMAN_BULLETCOLLISION_LEG_R 2.5f	//!< ���̓����蔻��̔��a

#define SMALLOBJECT_COLLISIONSCALE 0.13f	//!< ���������蔻��̔{��

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! �G�t�F�N�g�`��v�Z�p�\����
struct effectdata{
	int id;		//!< �f�[�^�ԍ�
	float dist;	//!< ����
};

//! @brief �I�u�W�F�N�g�Ǘ��N���X
//! @details �e�I�u�W�F�N�g�̏������E�v�Z�E�`��Ȃǂ��s���Ǘ����܂��B
class ObjectManager
{
	class human *HumanIndex;					//!< �l�I�u�W�F�N�g
	class weapon *WeaponIndex;					//!< ����I�u�W�F�N�g
	class smallobject *SmallObjectIndex;		//!< �����I�u�W�F�N�g
	class bullet *BulletIndex;					//!< �e�I�u�W�F�N�g
	class grenade *GrenadeIndex;				//!< ��֒e�I�u�W�F�N�g
	class effect *EffectIndex;				//!< ��֒e�I�u�W�F�N�g

	unsigned int framecnt;		//!< �t���[�����̃J�E���g

	int *Human_ontarget;	//!< ������
	int *Human_kill;		//!< �|�����G�̐�
	int *Human_headshot;	//!< �G�̓����ɖ���������
	bool *Human_ShotFlag;	//!< ���C�t���O�i�}�Y���t���b�V���p�j

	bool FriendlyFire;		//!< FF�i���m�����j�L����

	int Player_HumanID;		//!< �v���C���[�����삷��l�I�u�W�F�N�g��ID

	int AddHumanIndex_TextureID;		//!< �O��ǂݍ��񂾃e�N�X�`��ID

	ParameterInfo *GameParamInfo;		//!< �Q�[���̐ݒ�l
	D3DGraphics *d3dg;					//!< �`��N���X
	ResourceManager *Resource;			//!< ���\�[�X�Ǘ�
	BlockDataInterface *BlockData;		//!< �u���b�N�f�[�^�Ǘ��N���X
	PointDataInterface *PointData;		//!< �|�C���g�f�[�^�Ǘ��N���X
	Collision *CollD;					//!< �����蔻��Ǘ��N���X
	SoundManager *GameSound;				//!< �Q�[�����ʉ��Ǘ��N���X
	MIFInterface *MIFdata;				//!< MIF�R���g���[���N���X

	int AddHumanIndex(pointdata data, pointdata infodata);
	int AddWeaponIndex(pointdata data);
	int AddSmallObjectIndex(pointdata data);
	void SetHumanBlood(float x, float y, float z, int damage, bool CollideMap);
	bool CollideHuman(human *in_humanA, human *in_humanB);
	bool CollideBullet(bullet *in_bullet);
	void HitBulletMap(float x, float y, float z);
	void HitBulletHuman(int HitHuman_id, int Hit_id, float x, float y, float z, float brx, int attacks, int humanid);
	void HitBulletSmallObject(int HitSmallObject_id, float x, float y, float z, int attacks);
	bool GrenadeExplosion(grenade *in_grenade);
	void DeadEffect(human *in_human);
	bool CollideBlood(effect *in_effect, int *id, int *face, float *pos_x, float *pos_y, float *pos_z);
	void PickupWeapon(human *in_human, weapon *in_weapon);
	void CleanupPointDataToObject();
	int SortEffect(float camera_x, float camera_y, float camera_z, effectdata data[]);
	void ShotWeaponEffect(int humanid);

public:
	ObjectManager();
	~ObjectManager();
	void SetClass(ParameterInfo *in_GameParamInfo, D3DGraphics *in_d3dg, ResourceManager *in_Resource, BlockDataInterface *in_BlockData, PointDataInterface *in_PointData, Collision *in_CollD, SoundManager *in_GameSound, MIFInterface *in_MIFdata);
	int AddHumanIndex(float px, float py, float pz, float rx, int CharacterID, int TeamID, int WeaponID[]);
	int AddVisualWeaponIndex(int WeaponID, bool loadbullet);
	int AddSmallObjectIndex(float px, float py, float pz, float rx, int paramID, bool MapColl);
	int AddEffect(float pos_x, float pos_y, float pos_z, float move_x, float move_y, float move_z, float size, float rotation, int count, int texture, int settype);
	int AddMapEffect(int id, int face, float pos_x, float pos_y, float pos_z, float size, float rotation, int count, int texture);
	void LoadPointData();
	void Recovery();
	bool GetFriendlyFireFlag();
	void SetFriendlyFireFlag(bool flag);
	int GetPlayerID();
	void SetPlayerID(int id);
	human* GeHumanObject(int id);
	human* GetPlayerHumanObject();
	weapon* GetWeaponObject(int id);
	smallobject* GetSmallObject(int id);
	bullet* GetBulletObject(int id);
	bullet* GetNewBulletObject();
	grenade* GetNewGrenadeObject();
	human* SearchHuman(signed char p4);
	smallobject* SearchSmallobject(signed char p4);
	void MoveForward(int human_id);
	void MoveBack(int human_id);
	void MoveLeft(int human_id);
	void MoveRight(int human_id);
	void MoveWalk(int human_id);
	void MoveJump(int human_id);
	int ShotWeapon(int human_id);
	void ReloadWeapon(int human_id);
	void ChangeWeapon(int human_id, int id = -1);
	bool DumpWeapon(int human_id);
	void ChangeScopeMode(int human_id);
	int ChangeShotMode(int human_id);
	bool CheatAddBullet(int human_id);
	bool CheatNewWeapon(int human_id, int new_weaponID);
	bool CheckZombieAttack(human* MyHuman, human* EnemyHuman);
	void HitZombieAttack(human* EnemyHuman);
	bool HumanResuscitation(int id);
	int CheckGameOverorComplete();
	bool GetObjectInfoTag(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, int *color, char *infostr);
	int Process(int cmdF5id, bool demomode, float camera_rx, float camera_ry);
	bool GetHumanShotInfo(int id, int *ontarget, int *kill, int *headshot);
	void Render(float camera_x, float camera_y, float camera_z, int HidePlayer);
	void Cleanup();
};

#endif