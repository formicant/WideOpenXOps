//! @file objectmanager.cpp
//! @brief ObjectManager�N���X�̒�`

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

#include "objectmanager.h"

//! @brief �R���X�g���N�^
ObjectManager::ObjectManager()
{
	HumanIndex = new human[MAX_HUMAN];
	WeaponIndex = new weapon[MAX_WEAPON];
	SmallObjectIndex = new smallobject[MAX_SMALLOBJECT];
	BulletIndex = new bullet[MAX_BULLET];
	GrenadeIndex = new grenade[MAX_GRENADE];
	EffectIndex = new effect[MAX_EFFECT];

	framecnt = 0;
	Human_ontarget = new int[MAX_HUMAN];
	Human_kill = new int[MAX_HUMAN];
	Human_headshot = new int[MAX_HUMAN];
	Human_ShotFlag = new bool[MAX_HUMAN];
	BulletObj_HumanIndex = new BulletObjectHumanIndex[MAX_BULLET];
	FriendlyFire = false;
	Player_HumanID = 0;
	AddHumanIndex_TextureID = -1;

	GameParamInfo = NULL;
	d3dg = NULL;
	Resource = NULL;
	BlockData = NULL;
	PointData = NULL;
	CollD = NULL;
}

//! @brief �f�B�X�g���N�^
ObjectManager::~ObjectManager()
{
	Cleanup();

	if( HumanIndex != NULL ){ delete [] HumanIndex; }
	if( WeaponIndex != NULL ){ delete [] WeaponIndex; }
	if( SmallObjectIndex != NULL ){ delete [] SmallObjectIndex; }
	if( BulletIndex != NULL ){ delete [] BulletIndex; }
	if( GrenadeIndex != NULL ){ delete [] GrenadeIndex; }
	if( EffectIndex != NULL ){ delete [] EffectIndex; }

	if( Human_ontarget != NULL ){ delete [] Human_ontarget; }
	if( Human_kill != NULL ){ delete [] Human_kill; }
	if( Human_headshot != NULL ){ delete [] Human_headshot; }
	if( Human_ShotFlag != NULL ){ delete [] Human_ShotFlag; }
	if( BulletObj_HumanIndex != NULL ){ delete [] BulletObj_HumanIndex; }
}

//! @brief �Q�Ƃ���N���X��ݒ�
//! @param in_GameParamInfo �Q�[���ݒ�f�[�^�Ǘ��N���X
//! @param in_d3dg �`�揈���N���X
//! @param in_Resource ���\�[�X�Ǘ��N���X
//! @param in_BlockData �u���b�N�f�[�^�Ǘ��N���X
//! @param in_PointData �|�C���g�f�[�^�Ǘ��N���X
//! @param in_CollD �����蔻�菈���N���X
//! @param in_GameSound ���ʉ��Đ��N���X
//! @param in_MIFdata MIF�R���g���[���N���X
//! @attention ���̊֐��Őݒ���s��Ȃ��ƁA�N���X���̂��������@�\���܂���B
void ObjectManager::SetClass(ParameterInfo *in_GameParamInfo, D3DGraphics *in_d3dg, ResourceManager *in_Resource, BlockDataInterface *in_BlockData, PointDataInterface *in_PointData, Collision *in_CollD, SoundManager *in_GameSound, MIFInterface *in_MIFdata)
{
	GameParamInfo = in_GameParamInfo;
	d3dg = in_d3dg;
	Resource = in_Resource;
	BlockData = in_BlockData;
	PointData = in_PointData;
	CollD = in_CollD;
	GameSound = in_GameSound;
	MIFdata = in_MIFdata;

	for(int i=0; i<MAX_HUMAN; i++){
		HumanIndex[i].SetParameterInfoClass(GameParamInfo);
	}
	for(int i=0; i<MAX_WEAPON; i++){
		WeaponIndex[i].SetParameterInfoClass(GameParamInfo);
	}
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		SmallObjectIndex[i].SetParameterInfoClass(GameParamInfo);
		SmallObjectIndex[i].SetMIFInterfaceClass(MIFdata);
	}
	for(int i=0; i<MAX_GRENADE; i++){
		GrenadeIndex[i].SetParameterInfoClass(GameParamInfo);
	}

	int bulletmodel, bullettexture;
	BulletParameter data;

	if( Resource->GetBulletModelTexture(0, &bulletmodel, &bullettexture) == 0 ){
		if( GameParamInfo->GetBullet(0, &data) == 0 ){
			for(int i=0; i<MAX_BULLET; i++){
				BulletIndex[i].SetModel(bulletmodel, data.size);
				BulletIndex[i].SetTexture(bullettexture);
			}
		}
	}

	if( Resource->GetBulletModelTexture(1, &bulletmodel, &bullettexture) == 0 ){
		if( GameParamInfo->GetBullet(1, &data) == 0 ){
			for(int i=0; i<MAX_GRENADE; i++){
				GrenadeIndex[i].SetModel(bulletmodel, data.size);
				GrenadeIndex[i].SetTexture(bullettexture);
			}
		}
	}
}

//! @brief �l�ǉ�
//! @param data �l�̃|�C���g�f�[�^�@�ipointdata�\���́j
//! @param infodata �Q�Ƃ���l���̃|�C���g�f�[�^�@�i�V�j
//! @return �����F�f�[�^�ԍ��i0�ȏ�j�@���s�F-1
//! @attention �����Ȑl�̎�ޔԍ����w�肳�ꂽ�ꍇ�� �ʏ́F��l�� ���o�ꂵ�܂��B�e�N�X�`���̓}�b�v�e�N�X�`��0�Ԃ��g�p����AHP�� 0 ���w��i�������j����܂��B
int ObjectManager::AddHumanIndex(pointdata data, pointdata infodata)
{
	int GetHumanFlag;
	HumanParameter HumanParam;
	GetHumanFlag = GameParamInfo->GetHuman(infodata.p2, &HumanParam);
	int Humanindexid = -1;
	int Weaponindexid = -1;
	class weapon *Weapon[TOTAL_HAVEWEAPON];
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		Weapon[i] = NULL;
	}

	//�l�̃��f���ԍ����擾
	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);

	for(int j=0; j<MAX_HUMAN; j++){
		if( HumanIndex[j].GetEnableFlag() == false ){
			//����������
			HumanIndex[j].SetPosData(data.x, data.y, data.z, data.r);
			HumanIndex[j].SetParamData(infodata.p2, data.id, data.p4, infodata.p3, true);
			if( GetHumanFlag == 0 ){
				int id = Resource->GetHumanTexture(infodata.p2);
				if( id == -1 ){
					id = AddHumanIndex_TextureID;
				}
				else{
					AddHumanIndex_TextureID = id;
				}

				HumanIndex[j].SetTexture(id);
				HumanIndex[j].SetModel(upmodel[ HumanParam.model ], armmodel, legmodel, walkmodel, runmodel);
			}
			else{
				AddHumanIndex_TextureID = d3dg->GetMapTextureID(0);

				HumanIndex[j].SetTexture(AddHumanIndex_TextureID);
				HumanIndex[j].SetModel(upmodel[0], armmodel, legmodel, walkmodel, runmodel);
			}
			HumanIndex[j].SetEnableFlag(true);
			Humanindexid = j;
			break;
		}
	}

	//�������ł��Ȃ�������G���[�Ƃ��ĕԂ�
	if( Humanindexid == -1 ){
		return -1;
	}

	//�l�̐ݒ�f�[�^���������ǂ߂Ă���΁`
	if( GetHumanFlag == 0 ){
		//����B�����z����Ƃ��Ēǉ�
		Weaponindexid = AddVisualWeaponIndex(HumanParam.Weapon[0], true);
		if( Weaponindexid != -1 ){
			//��������Δz��ɋL�^
			Weapon[0] = &WeaponIndex[Weaponindexid];
		}
		if( data.p1 == 1 ){
			//����A�̉��z����Ƃ��Ēǉ�
			Weaponindexid = AddVisualWeaponIndex(HumanParam.Weapon[1], true);
			if( Weaponindexid != -1 ){
				//��������Δz��ɋL�^
				Weapon[1] = &WeaponIndex[Weaponindexid];
			}
		}

		//�l�Ɏ�������
		HumanIndex[Humanindexid].SetWeapon(Weapon);
	}

	//���C�t���O��������
	Human_ShotFlag[Humanindexid] = false;

	//�v���C���[�Ȃ�΁A�ԍ����L�^
	if( ( (data.p1 == 1)||(data.p1 == 6) )&&(data.p4 == 0) ){
		Player_HumanID = Humanindexid;
	}

	return Humanindexid;
}

//! @brief �l�ǉ��i�Q�[�����p�j
//! @param px X���W
//! @param py Y���W
//! @param pz Z���W
//! @param rx X������
//! @param paramID ��ޔԍ�
//! @param TeamID �`�[���ԍ�
//! @param WeaponID �����ޔԍ��̔z��i�v�f���FTOTAL_HAVEWEAPON�j
//! @return �����F�f�[�^�ԍ��i0�ȏ�j�@���s�F-1
//! @attention �����Ȑl�̎�ޔԍ����w�肳�ꂽ�ꍇ�� �ʏ́F��l�� ���o�ꂵ�܂��B�e�N�X�`���̓}�b�v�e�N�X�`��0�Ԃ��g�p����AHP�� 0 ���w��i�������j����܂��B
int ObjectManager::AddHumanIndex(float px, float py, float pz, float rx, int paramID, int TeamID, int WeaponID[])
{
	int GetHumanFlag;
	HumanParameter HumanParam;
	GetHumanFlag = GameParamInfo->GetHuman(paramID, &HumanParam);
	int Humanindexid = -1;
	int Weaponindexid = -1;
	class weapon *Weapon[TOTAL_HAVEWEAPON];
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		Weapon[i] = NULL;
	}

	//�l�̃��f���ԍ����擾
	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);

	for(int i=0; i<MAX_HUMAN; i++){
		if( HumanIndex[i].GetEnableFlag() == false ){
			//����������
			HumanIndex[i].SetPosData(px, py, pz, rx);
			HumanIndex[i].SetParamData(paramID, -1, 0, TeamID, true);
			if( GetHumanFlag == 0 ){
				//�ǂݍ��߂Ȃ���΁A�O��ǂݍ��񂾃e�N�X�`���ԍ��𗘗p
				//�ǂݍ��߂�΁A����ǂݍ��ރe�N�X�`���ԍ����㏑��
				int id = Resource->GetHumanTexture(paramID);
				if( id == -1 ){
					id = AddHumanIndex_TextureID;
				}
				else{
					AddHumanIndex_TextureID = id;
				}

				HumanIndex[i].SetTexture(id);
				HumanIndex[i].SetModel(upmodel[ HumanParam.model ], armmodel, legmodel, walkmodel, runmodel);
			}
			else{
				//����ǂݍ��ރe�N�X�`���ԍ����㏑��
				AddHumanIndex_TextureID = d3dg->GetMapTextureID(0);

				HumanIndex[i].SetTexture(AddHumanIndex_TextureID);
				HumanIndex[i].SetModel(upmodel[0], armmodel, legmodel, walkmodel, runmodel);
			}
			HumanIndex[i].SetEnableFlag(true);
			Humanindexid = i;
			break;
		}
	}

	//�������ł��Ȃ�������G���[�Ƃ��ĕԂ�
	if( Humanindexid == -1 ){
		return -1;
	}

	if( GetHumanFlag == 0 ){
		//���z�����ǉ�
		for(int i=0; i<TOTAL_HAVEWEAPON; i++){
			Weaponindexid = AddVisualWeaponIndex(WeaponID[i], true);
			if( Weaponindexid != -1 ){
				//��������Δz��ɋL�^
				Weapon[i] = &WeaponIndex[Weaponindexid];
			}
		}

		//�l�Ɏ�������
		HumanIndex[Humanindexid].SetWeapon(Weapon);
	}

	return Humanindexid;
}

//! @brief ����ǉ�
//! @param data ����̃|�C���g�f�[�^�@�ipointdata�\���́j
//! @return �����F�f�[�^�ԍ��i0�ȏ�j�@���s�F-1
int ObjectManager::AddWeaponIndex(pointdata data)
{
	//����ԍ��ƒe�������o�^
	int WeaponID = data.p2;
	int nbs = (unsigned char)data.p3;

	//�����_���ȕ���Ȃ��
	if( data.p1 == 7 ){
		WeaponParameter WeaponParam;

		if( GetRand(2) == 0 ){
			if( GameParamInfo->GetWeapon(data.p2, &WeaponParam) == 1 ){ return -1; }
			nbs = (int)WeaponParam.nbsmax * TOTAL_WEAPON_AUTOBULLET;		//�e���㏑��
		}
		else{
			if( GameParamInfo->GetWeapon(data.p3, &WeaponParam) == 1 ){ return -1; }
			WeaponID = data.p3;						//����ԍ��㏑��
			nbs = (int)WeaponParam.nbsmax * TOTAL_WEAPON_AUTOBULLET;		//�e���㏑��
		}
	}

	//NONE�Ȃ�Ύ��s
	if( WeaponID == ID_WEAPON_NONE ){ return -1; }

	//���f���ƃe�N�X�`�����擾
	int model, texture;
	if( Resource->GetWeaponModelTexture(WeaponID, &model, &texture) == 1 ){
		return -1;
	}

	//�ݒ�l���擾
	WeaponParameter WeaponParam;
	if( GameParamInfo->GetWeapon(data.p2, &WeaponParam) == 1 ){ return -1; }

	for(int i=0; i<MAX_WEAPON; i++){
		if( WeaponIndex[i].GetEnableFlag() == false ){
			//����������
			WeaponIndex[i].SetPosData(data.x, data.y, data.z, data.r);
			WeaponIndex[i].SetParamData(WeaponID, 0, nbs, true);
			WeaponIndex[i].SetModel(model, WeaponParam.size);
			WeaponIndex[i].SetTexture(texture);
			WeaponIndex[i].SetEnableFlag(true);
			WeaponIndex[i].RunReload();
			return i;
		}
	}
	return -1;
}

//! @brief ���z����ǉ�
//! @param WeaponID ����̎�ޔԍ�
//! @param loadbullet �e�����[�h����
//! @return �����F�f�[�^�ԍ��i0�ȏ�j�@���s�F-1
//! @attention �l���ŏ����珊�����Ă��镐����A����f�[�^�i�I�u�W�F�N�g�j�Ƃ��Ēǉ��������邽�߂̊֐��ł��B
//! @attention ����P�̂Ƃ��Ĕz�u����ꍇ�́AAddWeaponIndex()�֐����g�p���Ă��������B
int ObjectManager::AddVisualWeaponIndex(int WeaponID, bool loadbullet)
{
	//NONE�Ȃ�Ύ��s
	if( WeaponID == ID_WEAPON_NONE ){ return -1; }

	//�����擾
	WeaponParameter WeaponParam;
	if( GameParamInfo->GetWeapon(WeaponID, &WeaponParam) == 1 ){ return -1; }

	//���f���ƃe�N�X�`�����擾
	int model, texture;
	if( Resource->GetWeaponModelTexture(WeaponID, &model, &texture) == 1 ){
		return -1;
	}

	for(int i=0; i<MAX_WEAPON; i++){
		if( WeaponIndex[i].GetEnableFlag() == false ){
			//������
			WeaponIndex[i].SetPosData(0.0f, 0.0f, 0.0f, 0.0f);
			if( loadbullet == false ){
				WeaponIndex[i].SetParamData(WeaponID, 0, 0, true);
			}
			else{
				WeaponIndex[i].SetParamData(WeaponID, WeaponParam.nbsmax, WeaponParam.nbsmax * TOTAL_WEAPON_AUTOBULLET, true);
			}
			WeaponIndex[i].SetModel(model, WeaponParam.size);
			WeaponIndex[i].SetTexture(texture);
			WeaponIndex[i].SetEnableFlag(true);
			return i;
		}
	}
	return -1;
}

//! @brief �����ǉ�
//! @param data �����̃|�C���g�f�[�^�@�ipointdata�\���́j
//! @return �����F�f�[�^�ԍ��i0�ȏ�j�@���s�F-1
int ObjectManager::AddSmallObjectIndex(pointdata data)
{
	//���f���ƃe�N�X�`�����擾
	int model, texture;
	if( Resource->GetSmallObjectModelTexture(data.p2, &model, &texture) == 1 ){
		return -1;
	}

	for(int i=0; i<MAX_SMALLOBJECT; i++){
		if( SmallObjectIndex[i].GetEnableFlag() == false ){
			//������
			SmallObjectIndex[i].SetPosData(data.x, data.y, data.z, data.r);
			SmallObjectIndex[i].SetParamData(data.p2, data.p4, true);
			SmallObjectIndex[i].SetModel(model, SMALLOBJECT_SCALE);
			SmallObjectIndex[i].SetTexture(texture);
			SmallObjectIndex[i].SetEnableFlag(true);

			//�ʒu�C���t���O���L���Ȃ�΁A�}�b�v�Ɣ���
			if( data.p3 !=0 ){
				SmallObjectIndex[i].CollisionMap(CollD);
			}
			return i;
		}
	}
	return -1;
}

//! @brief �����ǉ��i�Q�[�����p�j
//! @param px X���W
//! @param py Y���W
//! @param pz Z���W
//! @param rx X������
//! @param paramID ��ޔԍ�
//! @param MapColl �ʒu�C���t���O
//! @return �����F�f�[�^�ԍ��i0�ȏ�j�@���s�F-1
int ObjectManager::AddSmallObjectIndex(float px, float py, float pz, float rx, int paramID, bool MapColl)
{
	//���f���ƃe�N�X�`�����擾
	int model, texture;
	if( Resource->GetSmallObjectModelTexture(paramID, &model, &texture) == 1 ){
		return -1;
	}

	for(int j=0; j<MAX_SMALLOBJECT; j++){
		if( SmallObjectIndex[j].GetEnableFlag() == false ){
			//������
			SmallObjectIndex[j].SetPosData(px, py, pz, rx);
			SmallObjectIndex[j].SetParamData(paramID, 0, true);
			SmallObjectIndex[j].SetModel(model, SMALLOBJECT_SCALE);
			SmallObjectIndex[j].SetTexture(texture);
			SmallObjectIndex[j].SetEnableFlag(true);

			//�ʒu�C���t���O���L���Ȃ�΁A�}�b�v�Ɣ���
			if( MapColl == true ){
				SmallObjectIndex[j].CollisionMap(CollD);
			}
			return j;
		}
	}
	return -1;
}

//! @brief �G�t�F�N�g�ǉ�
//! @param pos_x X���W
//! @param pos_y Y���W
//! @param pos_z Z���W
//! @param move_x X���ړ���
//! @param move_y Y���ړ���
//! @param move_z Z���ړ���
//! @param size �\���{��
//! @param rotation ��]�p�x
//! @param count �\���t���[����
//! @param texture �e�N�X�`���̔F���ԍ�
//! @param settype �G�t�F�N�g�̎�ށ@�iEffect_Type ��g�ݍ�����j
//! @return �����F�f�[�^�ԍ��i0�ȏ�j�@���s�F-1
int ObjectManager::AddEffect(float pos_x, float pos_y, float pos_z, float move_x, float move_y, float move_z, float size, float rotation, int count, int texture, int settype)
{
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetEnableFlag() == false ){
			EffectIndex[i].SetPosData(pos_x, pos_y, pos_z, 0.0f);
			EffectIndex[i].SetParamData(move_x, move_y, move_z, size, rotation, count, texture, settype, true);
			EffectIndex[i].SetEnableFlag(true);
			return i;
		}
	}

	return -1;
}

//! @brief �i�r���{�[�h�łȂ��j�G�t�F�N�g�ǉ�
//! @param id �u���b�N�ԍ�
//! @param face �ʔԍ�
//! @param pos_x X���W
//! @param pos_y Y���W
//! @param pos_z Z���W
//! @param size �\���{��
//! @param rotation ��]�p�x
//! @param count �\���t���[����
//! @param texture �e�N�X�`���̔F���ԍ�
//! @return �����F�f�[�^�ԍ��i0�ȏ�j�@���s�F-1
int ObjectManager::AddMapEffect(int id, int face, float pos_x, float pos_y, float pos_z, float size, float rotation, int count, int texture)
{
	blockdata data;
	float vx, vy, vz;
	float rx, ry;

	//�u���b�N�̖ʂ̏����擾
	BlockData->Getdata(&data, id);
	vx = data.material[face].vx;
	vy = data.material[face].vy;
	vz = data.material[face].vz;

	//�p�x�����߂�
	rx = atan2(vz, vx)*-1 + (float)M_PI;
	ry = atan2(vy, sqrt(vx*vx + vz*vz))*-1;

	//�G�t�F�N�g�쐬
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetEnableFlag() == false ){
			EffectIndex[i].SetPosData(pos_x, pos_y, pos_z, 0.0f);
			EffectIndex[i].SetParamData(0.0f, 0.0f, 0.0f, size, rotation, count, texture, EFFECT_DISAPPEARHALF | EFFECT_NOBILLBOARD, true);
			EffectIndex[i].SetRxRy(rx, ry);
			EffectIndex[i].SetEnableFlag(true);
			return i;
		}
	}

	return -1;
}

//! @brief �o��������
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param damage �_���[�W
//! @param CollideMap �}�b�v�ւ̕t��
void ObjectManager::SetHumanBlood(float x, float y, float z, int damage, bool CollideMap)
{
	int addtype;

	if( CollideMap == false ){
		addtype = EFFECT_NORMAL;
	}
	else{
		addtype = EFFECT_COLLIDEMAP;
	}

	if( GameConfig.GetBloodFlag() == true ){
		AddEffect(x, y, z, 0.0f, 0.0f, 0.0f, 10.0f, DegreeToRadian(10)*GetRand(18), (int)(GAMEFPS * 0.4f), Resource->GetEffectBloodTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_TRANSLUCENT | addtype);
		for(int i=0; i<(damage/10); i++){
			float rx = DegreeToRadian(10)*GetRand(36);
			AddEffect(x + cos(rx)*1.0f, y + (float)(GetRand(20)-10)/10, z + sin(rx)*1.0f, cos(rx)*0.5f, GetRand(5)*0.1f + 0.5f, sin(rx)*0.5f, 5.0f, DegreeToRadian(10)*GetRand(18), (int)(GAMEFPS * 0.5f), Resource->GetEffectBloodTexture(), EFFECT_FALL | EFFECT_TRANSLUCENT | addtype);
		}
	}
}

//! @brief �l���m�̓����蔻��
//! @param in_humanA �Ώۂ̐l�I�u�W�F�N�gA
//! @param in_humanB �Ώۂ̐l�I�u�W�F�N�gB
//! @return �������Ă���Ftrue�@�������Ă��Ȃ��Ffalse
//! @warning in_humanA��in_humanB�ŋ�ʂ͂���܂���B
//! @warning in_humanA��in_humanB�ł̑g�ݍ����́A1�t���[���Ԃ�1�x�������s���Ă��������B
//! @attention ���N���X�͎����I��AddPosOrder()��p���āA���݂������������܂��B
bool ObjectManager::CollideHuman(human *in_humanA, human *in_humanB)
{
	float h1_x, h1_y, h1_z;
	float h2_x, h2_y, h2_z;
	float angle, length;

	//����������Ă��Ȃ����A���S���ē���Δ��肵�Ȃ�
	if( in_humanA->GetEnableFlag() == false ){ return false; }
	if( in_humanB->GetEnableFlag() == false ){ return false; }
	if( in_humanA->GetHP() <= 0 ){ return false; }
	if( in_humanB->GetHP() <= 0 ){ return false; }

	//���݂��̍��W���擾
	in_humanA->GetPosData(&h1_x, &h1_y, &h1_z, NULL);
	in_humanB->GetPosData(&h2_x, &h2_y, &h2_z, NULL);

	//�~���̓����蔻��
	if( CollideCylinder(h1_x, h1_y, h1_z, 3.0f, HUMAN_HEIGTH-0.5f, h2_x, h2_y, h2_z, 3.0f, HUMAN_HEIGTH-0.5f, &angle, &length) == true ){
		//�߂荞�񂾕����������o��
		in_humanA->AddPosOrder(angle, 0.0f, length/2);
		in_humanB->AddPosOrder(angle + (float)M_PI, 0.0f, length/2);
		return true;
	}

	return false;
}

//! @brief �e�̓����蔻��Ə���
//! @param in_bullet �Ώۂ̒e�I�u�W�F�N�g
//! @return ���������Ftrue�@�������Ă��Ȃ��Ffalse
//! @attention ������s���Ώۂ́u�}�b�v�v�u�l�i���E�㔼�g�E�����g�j�v�u�����v�ł��B
//! @attention ����Ɍ��炸�A�_���[�W�v�Z����ʉ��Đ��܂ň�т��čs���܂��B
bool ObjectManager::CollideBullet(bullet *in_bullet)
{
	//�g�p����Ă��Ȃ��e�ۂȂ�΁A���������ɕԂ��B
	if( in_bullet->GetEnableFlag() == false ){ return false; }

	int objectID;
	float bx, by, bz;
	float brx, bry;
	int attacks;
	int penetration;
	int speed;
	int teamid;
	int humanid;
	float vx, vy, vz;
	int HumanHead_id;
	int HumanUp_id;
	int HumanLeg_id;
	int SmallObject_id;
	float map_Dist;
	float HumanHead_Dist;
	float HumanUp_Dist;
	float HumanLeg_Dist;
	float SmallObject_Dist;
	float CheckDist;
	bool CollideFlag;

	int id, face;
	float Dist;

	//�e�I�u�W�F�N�g�̃f�[�^�ԍ����擾
	objectID = GetBulletObjectID(in_bullet);

	//�e�ۂ̍��W���擾���A�x�N�g�����Z�o�B
	in_bullet->GetPosData(&bx, &by, &bz, &brx, &bry);
	in_bullet->GetParamData(&attacks, &penetration, &speed, &teamid, &humanid);
	vx = cos(brx)*cos(bry);
	vy = sin(bry);
	vz = sin(brx)*cos(bry);

	CheckDist = 0;
	CollideFlag = false;

	for(float TotalDist=0.0f; TotalDist<speed; TotalDist+=CheckDist){
		CheckDist = speed - TotalDist;

		//�ђʗ͂��c���Ă��Ȃ����
		if( penetration < 0 ){
			//�e�͖����ɂ���
			in_bullet->SetEnableFlag(false);
			break;
		}

		float bvx, bvy, bvz;
		bvx = bx + vx*TotalDist;
		bvy = by + vy*TotalDist;
		bvz = bz + vz*TotalDist;

		HumanHead_id = -1;
		HumanUp_id = -1;
		HumanLeg_id = -1;
		SmallObject_id = -1;
		map_Dist = (float)speed - TotalDist + 1;
		HumanHead_Dist = (float)speed - TotalDist + 1;
		HumanUp_Dist = (float)speed - TotalDist + 1;
		HumanLeg_Dist = (float)speed - TotalDist + 1;
		SmallObject_Dist = (float)speed - TotalDist + 1;

		//�}�b�v�Ƃ̓����蔻��
		if( CollD->CheckALLBlockIntersectRay(bvx, bvy, bvz, vx, vy, vz, &id, &face, &Dist, (float)speed - TotalDist) == true ){
			map_Dist = Dist;
		}

		//�l�Ƃ̓����蔻��
		for(int i=0; i<MAX_HUMAN; i++){
			//���̐l���g�����C�����e�Ȃ珈�����Ȃ��B
			if( i == humanid ){ continue; }

			//�g�p����Ă��Ȃ����A���S���Ă���Ώ������Ȃ��B
			if( HumanIndex[i].GetEnableFlag() == false ){ continue; }
			if( HumanIndex[i].GetHP() <= 0 ){ continue; }

			//���ɓ��������l�Ȃ�A�������Ȃ��B
			if( BulletObj_HumanIndex[objectID].GetIndexFlag(i) == true ){ continue; }

			//���W���擾
			float ox, oy, oz;
			int h_teamid;
			HumanIndex[i].GetPosData(&ox, &oy, &oz, NULL);
			HumanIndex[i].GetParamData(NULL, NULL, NULL, &h_teamid);

			if( FriendlyFire == false ){
				//�����`�[���ԍ��i�����j�Ȃ珈�����Ȃ�
				if( h_teamid == teamid ){ continue; }
			}

			//���̓����蔻��
			if( CollideCylinderRay(ox, oy + HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, oz, HUMAN_BULLETCOLLISION_HEAD_R, HUMAN_BULLETCOLLISION_HEAD_H, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){
				if( Dist < HumanHead_Dist ){
					HumanHead_id = i;
					HumanHead_Dist = Dist;
				}
			}

			//�㔼�g�Ɠ����蔻��
			if( CollideCylinderRay(ox, oy + HUMAN_BULLETCOLLISION_LEG_H, oz, HUMAN_BULLETCOLLISION_UP_R, HUMAN_BULLETCOLLISION_UP_H, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){
				if( Dist < HumanUp_Dist ){
					HumanUp_id = i;
					HumanUp_Dist = Dist;
				}
			}

			//���Ɠ����蔻��
			if( CollideCylinderRay(ox, oy, oz, HUMAN_BULLETCOLLISION_LEG_R, HUMAN_BULLETCOLLISION_LEG_H, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){
				if( Dist < HumanLeg_Dist ){
					HumanLeg_id = i;
					HumanLeg_Dist = Dist;
				}
			}
		}

		//�����Ƃ̓����蔻��
		for(int i=0; i<MAX_SMALLOBJECT; i++){
			//�g�p����Ă��Ȃ���Ώ������Ȃ�
			if( SmallObjectIndex[i].GetEnableFlag() == false ){ continue; }

			//���W���擾
			float ox, oy, oz;
			int id;
			float decide;
			SmallObjectIndex[i].GetPosData(&ox, &oy, &oz, NULL);
			SmallObjectIndex[i].GetParamData(&id, NULL);

			//�����蔻��̑傫�����擾
			if( id == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
				decide = (float)MIFdata->GetAddSmallobjectDecide()*SMALLOBJECT_COLLISIONSCALE;
			}
			else{
				SmallObjectParameter Param;
				GameParamInfo->GetSmallObject(id, &Param);
				decide = (float)Param.decide*SMALLOBJECT_COLLISIONSCALE;
			}

			//�����蔻��
			if( CollideSphereRay(ox, oy, oz, decide, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){
				if( Dist < SmallObject_Dist ){
					SmallObject_id = i;
					SmallObject_Dist = Dist;
				}
			}
		}

		//�}�b�v�Ƃ̏Փˋ������ŒZ�Ȃ�΁`
		if( (map_Dist <= speed)&&(map_Dist < HumanHead_Dist)&&(map_Dist < HumanUp_Dist)&&(map_Dist < HumanLeg_Dist)&&(map_Dist < SmallObject_Dist) ){
			//�e���}�b�v�ɓ�����������
			HitBulletMap(bx + vx*(map_Dist+TotalDist), by + vy*(map_Dist+TotalDist), bz + vz*(map_Dist+TotalDist));

			int Penetration_Dist;

			//�}�b�v�ɂ߂荞��ł���ʂ𒲂ׂ�
			//�@�@�ŏI�n�_���}�b�v�ɂ߂荞��ł���Ȃ��
			if( CollD->CheckALLBlockIntersectRay(bx + vx*speed, by + vy*speed, bz + vz*speed, vx*-1, vy*-1, vz*-1, &id, &face, &Dist, (float)speed - map_Dist+TotalDist) == true ){
				//���˒n�_�ƍŏI�n�_�A���ꂼ�ꂩ��̌��˓_����ђʋ��������߂�
				Penetration_Dist = (int)( ((float)speed - map_Dist+TotalDist - Dist) / 2 );
			}
			else{
				//�Փ˓_����ŏI�n�_�܂ňړ�����
				Penetration_Dist = (int)( ((float)speed - map_Dist+TotalDist) / 2 );
			}

			//�U���͂Ɗђʗ͂��v�Z
			for(int i=0; i<Penetration_Dist; i++){
				attacks = (int)((float)attacks * 0.6f);
			}
			penetration -= Penetration_Dist;

			CheckDist = map_Dist + 1.0f;
		}

		//�l�̓��Ƃ̏Փˋ������ŒZ�Ȃ�΁`
		if( (HumanHead_Dist <= speed)&&(HumanHead_Dist < map_Dist)&&(HumanHead_Dist < HumanUp_Dist)&&(HumanHead_Dist < HumanLeg_Dist)&&(HumanHead_Dist < SmallObject_Dist) ){
			//�l�ɓ�����������
			HitBulletHuman(HumanHead_id, 0, bx + vx*(HumanHead_Dist+TotalDist), by + vy*(HumanHead_Dist+TotalDist), bz + vz*(HumanHead_Dist+TotalDist), brx, attacks, humanid);

			//�ΐl����p���X�g�ݒ�
			BulletObj_HumanIndex[objectID].SetIndexFlag(HumanHead_id);

			//�U���͂Ɗђʗ͂��v�Z
			attacks = (int)((float)attacks * 0.6f);
			penetration -= 1;

			CheckDist = HumanHead_Dist + 1.0f;
		}

		//�l�̏㔼�g�Ƃ̏Փˋ������ŒZ�Ȃ�΁`
		if( (HumanUp_Dist <= speed)&&(HumanUp_Dist < map_Dist)&&(HumanUp_Dist < HumanHead_Dist)&&(HumanUp_Dist < HumanLeg_Dist)&&(HumanUp_Dist < SmallObject_Dist) ){
			//�l�ɓ�����������
			HitBulletHuman(HumanUp_id, 1, bx + vx*(HumanUp_Dist+TotalDist), by + vy*(HumanUp_Dist+TotalDist), bz + vz*(HumanUp_Dist+TotalDist), brx, attacks, humanid);

			//�ΐl����p���X�g�ݒ�
			BulletObj_HumanIndex[objectID].SetIndexFlag(HumanUp_id);

			//�U���͂Ɗђʗ͂��v�Z
			attacks = (int)((float)attacks * 0.6f);
			penetration -= 1;

			CheckDist = HumanUp_Dist + 1.0f;
		}

		//�l�̑��Ƃ̏Փˋ������ŒZ�Ȃ�΁`
		if( (HumanLeg_Dist <= speed)&&(HumanLeg_Dist < map_Dist)&&(HumanLeg_Dist < HumanHead_Dist)&&(HumanLeg_Dist < HumanUp_Dist)&&(HumanLeg_Dist < SmallObject_Dist) ){
			//�l�ɓ�����������
			HitBulletHuman(HumanLeg_id, 2, bx + vx*(HumanLeg_Dist+TotalDist), by + vy*(HumanLeg_Dist+TotalDist), bz + vz*(HumanLeg_Dist+TotalDist), brx, attacks, humanid);

			//�ΐl����p���X�g�ݒ�
			BulletObj_HumanIndex[objectID].SetIndexFlag(HumanLeg_id);

			//�U���͂Ɗђʗ͂��v�Z
			attacks = (int)((float)attacks * 0.7f);
			penetration -= 1;

			CheckDist = HumanLeg_Dist + 1.0f;
		}

		//�����Ƃ̏Փˋ������ŒZ�Ȃ�΁`
		if( (SmallObject_Dist <= speed)&&(SmallObject_Dist < map_Dist)&&(SmallObject_Dist < HumanHead_Dist)&&(SmallObject_Dist < HumanUp_Dist)&&(SmallObject_Dist < HumanLeg_Dist) ){
			//�����ɓ�����������
			HitBulletSmallObject(SmallObject_id, bx + vx*(SmallObject_Dist+TotalDist), by + vy*(SmallObject_Dist+TotalDist), bz + vz*(SmallObject_Dist+TotalDist), attacks);

			//�����̎�ޔԍ����擾
			int id;
			int decide;
			SmallObjectIndex[SmallObject_id].GetParamData(&id, NULL);

			//�����蔻��̑傫�����擾
			if( id == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
				decide = (int)( (float)MIFdata->GetAddSmallobjectDecide()*SMALLOBJECT_COLLISIONSCALE );
			}
			else{
				SmallObjectParameter Param;
				GameParamInfo->GetSmallObject(id, &Param);
				decide = (int)( (float)Param.decide*SMALLOBJECT_COLLISIONSCALE );
			}	

			//�ђʗ͂��v�Z
			for(int i=0; i<decide; i++){
				attacks = (int)((float)attacks * 0.7f);
			}

			CheckDist = SmallObject_Dist + 1.0f;
		}

		//�ݒ��K�p�i���ɍU���́E�ђʗ́j
		in_bullet->SetParamData(attacks, penetration, speed, teamid, humanid, false);
	}

	return CollideFlag;
}

//! @brief �e���}�b�v�ɓ�����������
//! @param x ���eX���W
//! @param y ���eY���W
//! @param z ���eZ���W
void ObjectManager::HitBulletMap(float x, float y, float z)
{
	//�G�t�F�N�g�i���j��\��
	AddEffect(x, y, z, 0.0f, 0.05f, 0.0f, 5.0f, DegreeToRadian(10)*GetRand(18), (int)(GAMEFPS * 0.5f), Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY);

	//���ʉ����Đ�
	GameSound->HitMap(x, y, z);
}

//! @brief �e���l�ɓ�����������
//! @param HitHuman_id �l�̔ԍ�
//! @param Hit_id ��e�ӏ��i���F0�@���́F1�@���F2�j
//! @param x ��eX���W
//! @param y ��eY���W
//! @param z ��eZ���W
//! @param brx �����p�x
//! @param attacks �U����
//! @param Shothuman_id ���˂����l�̔ԍ�
void ObjectManager::HitBulletHuman(int HitHuman_id, int Hit_id, float x, float y, float z, float brx, int attacks, int Shothuman_id)
{
	int damage = 0;
	int paramid;
	HumanParameter Paraminfo;
	bool NotRobot;

	//�g�p����Ă��Ȃ����A���S���Ă���Ώ������Ȃ��B
	if( HumanIndex[HitHuman_id].GetEnableFlag() == false ){ return; }
	if( HumanIndex[HitHuman_id].GetHP() <= 0 ){ return; }

	//�l�Ƀ_���[�W�ƏՌ���^����
	if( Hit_id == 0 ){ HumanIndex[HitHuman_id].HitBulletHead(attacks); }
	if( Hit_id == 1 ){ HumanIndex[HitHuman_id].HitBulletUp(attacks); }
	if( Hit_id == 2 ){ HumanIndex[HitHuman_id].HitBulletLeg(attacks); }
	HumanIndex[HitHuman_id].AddPosOrder(brx, 0.0f, 1.0f);

	//���{�b�g���ǂ�������
	HumanIndex[HitHuman_id].GetParamData(&paramid, NULL, NULL, NULL);
	GameParamInfo->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 1 ){
		NotRobot = false;
	}
	else{
		NotRobot = true;
	}

	//�G�t�F�N�g�i���j��\��
	if( Hit_id == 0 ){ damage = (int)((float)attacks * HUMAN_DAMAGE_HEAD); }
	if( Hit_id == 1 ){ damage = (int)((float)attacks * HUMAN_DAMAGE_UP); }
	if( Hit_id == 2 ){ damage = (int)((float)attacks * HUMAN_DAMAGE_LEG); }
	SetHumanBlood(x, y, z, damage, NotRobot);

	//���ʉ����Đ�
	GameSound->HitHuman(x, y, z);

	//�e�𔭎˂����l�̐��ʂɉ��Z
	Human_ontarget[Shothuman_id] += 1;
	if( Hit_id == 0 ){ Human_headshot[Shothuman_id] += 1; }
	if( HumanIndex[HitHuman_id].GetHP() <= 0 ){
		Human_kill[Shothuman_id] += 1;
	}
}

//! @brief �e�������ɓ�����������
//! @param HitSmallObject_id �����̔ԍ�
//! @param x ���eX���W
//! @param y ���eY���W
//! @param z ���eZ���W
//! @param attacks �U����
void ObjectManager::HitBulletSmallObject(int HitSmallObject_id, float x, float y, float z, int attacks)
{
	int hp;

	//�g�p����Ă��Ȃ���Ώ������Ȃ��B
	if( SmallObjectIndex[HitSmallObject_id].GetEnableFlag() == false ){ return; }

	//�̗͂��Ȃ���Ώ������Ȃ�
	hp = SmallObjectIndex[HitSmallObject_id].GetHP();
	if( hp <= 0 ){ return; }

	//�����Ƀ_���[�W��^����
	SmallObjectIndex[HitSmallObject_id].HitBullet(attacks);

	//�G�t�F�N�g�i���j��\��
	AddEffect(x, y, z, 0.0f, 0.05f, 0.0f, 5.0f, DegreeToRadian(10)*GetRand(18), (int)(GAMEFPS * 0.5f), Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY);

	//���ʉ����Đ�
	int id;
	SmallObjectIndex[HitSmallObject_id].GetParamData(&id, NULL);
	GameSound->HitSmallObject(x, y, z, id);
}

//! @brief ��֒e�̃_���[�W����Ə���
//! @param in_grenade �Ώۂ̎�֒e�I�u�W�F�N�g
//! @return ���������Ftrue�@�������Ă��Ȃ��Ffalse
//! @attention ������s���Ώۂ́u�l�v�Ɓu�����v�ł��B
//! @attention �_���[�W����Ɍ��炸�A�_���[�W�v�Z����ʉ��Đ��܂ň�т��čs���܂��B
bool ObjectManager::GrenadeExplosion(grenade *in_grenade)
{
	bool returnflag = false;

	//���W���擾
	float gx, gy, gz;
	int humanid;
	in_grenade->GetPosData(&gx, &gy, &gz, NULL, NULL);
	in_grenade->GetParamData(NULL, NULL, NULL, NULL, &humanid);

	//�l�ɔ����̓����蔻��
	for(int i=0; i<MAX_HUMAN; i++){
		//����������Ă��Ȃ����A���S���Ă���Ώ������Ȃ��B
		if( HumanIndex[i].GetEnableFlag() == false ){ continue; }
		if( HumanIndex[i].GetHP() <= 0 ){ continue; }

		float hx, hy, hz;
		float x, y, z, r;

		//�l�̍��W���擾���A�������v�Z
		HumanIndex[i].GetPosData(&hx, &hy, &hz, NULL);
		x = hx - gx;
		y = hy - gy;
		z = hz - gz;
		r = sqrt(x*x + y*y + z*z);

		//100.0��艓����Όv�Z���Ȃ�
		if( r > MAX_DAMAGE_GRENADE_DISTANCE + HUMAN_HEIGTH ){ continue; }

		float dummy = 0.0f;
		int total_damage = 0;
		int damage;

		//�����ɓ����蔻��
		y = hy + 2.0f - gy;
		r = sqrt(x*x + y*y + z*z);
		//�u���b�N���Ղ��Ă��Ȃ���΁@�i���C�œ����蔻����s���A�������Ă��Ȃ���΁j
		if( CollD->CheckALLBlockIntersectRay(gx, gy, gz, x/r, y/r, z/r, NULL, NULL, &dummy, r) == false ){
			//�_���[�W�ʂ��v�Z
			damage = HUMAN_DAMAGE_GRENADE_LEG - (int)((float)HUMAN_DAMAGE_GRENADE_LEG/MAX_DAMAGE_GRENADE_DISTANCE * r);
			if( damage > 0 ){
				total_damage += damage;
			}
		}

		//���ɓ����蔻��
		y = hy + 18.0f - gy;
		r = sqrt(x*x + y*y + z*z);
		//�u���b�N���Ղ��Ă��Ȃ���΁@�i���C�œ����蔻����s���A�������Ă��Ȃ���΁j
		if( CollD->CheckALLBlockIntersectRay(gx, gy, gz, x/r, y/r, z/r, NULL, NULL, &dummy, r) == false ){
			//�_���[�W�ʂ��v�Z
			damage = HUMAN_DAMAGE_GRENADE_HEAD - (int)((float)HUMAN_DAMAGE_GRENADE_HEAD/MAX_DAMAGE_GRENADE_DISTANCE * r);
			if( damage > 0 ){
				total_damage += damage;
			}
		}

		if( total_damage > 0 ){
			//�_���[�W�𔽉f
			HumanIndex[i].HitGrenadeExplosion(total_damage);

			float y2;
			float arx, ary;

			//�|���Ă���΁A���˂����l�̐��ʂɉ��Z
			if( HumanIndex[i].GetHP() <= 0 ){
				Human_kill[humanid] += 1;
			}

			//�G�t�F�N�g�i���j��\��
			SetHumanBlood(hx, hy+15.0f, hz, total_damage, false);

			//�l�Ǝ�֒e�̋������Z�o
			x = gx - hx;
			y = gy - (hy + 1.0f);
			z = gz - hz;

			//�p�x�����߂�
			arx = atan2(z, x);

			if( sin(atan2(y, sqrt(x*x + z*z))) < 0.0f ){		//������ɔ�ԂȂ�A�p�x���v�Z
				y2 = gy - (hy + HUMAN_HEIGTH);
				ary = atan2(y2, sqrt(x*x + z*z)) + (float)M_PI;
			}
			else{		//�������ɔ�ԂȂ�A�����p�x�͖����B�i�����Œn�ʂɂ߂荞�ނ̂�h�~�j
				ary = 0.0f;
			}

			//�����ɂ�镗��
			HumanIndex[i].AddPosOrder(arx, ary, 2.2f/MAX_DAMAGE_GRENADE_DISTANCE * (MAX_DAMAGE_GRENADE_DISTANCE - sqrt(x*x + y*y + z*z)));

			returnflag = true;
		}
	}

	//�����ɔ����̓����蔻��
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		//�g�p����Ă���Ώ������Ȃ�
		if( SmallObjectIndex[i].GetEnableFlag() == false ){ continue; }

		float sx, sy, sz;
		float x, y, z, r;

		//�����̍��W���擾���A�������v�Z
		SmallObjectIndex[i].GetPosData(&sx, &sy, &sz, NULL);
		x = sx - gx;
		y = sy - gy;
		z = sz - gz;
		r = sqrt(x*x + y*y + z*z);

		//100.0��艓����Όv�Z���Ȃ�
		if( r > MAX_DAMAGE_GRENADE_DISTANCE ){ continue; }

		float dummy = 0.0f;

		//�u���b�N���Ղ��Ă��Ȃ���΁@�i���C�œ����蔻����s���A�������Ă��Ȃ���΁j
		if( CollD->CheckALLBlockIntersectRay(gx, gy, gz, x/r, y/r, z/r, NULL, NULL, &dummy, r) == false ){
			int id, damage;
			SmallObjectIndex[i].GetParamData(&id, NULL);

			//�_���[�W�ʂ��v�Z���A���f
			damage = SMALLOBJECT_DAMAGE_GRENADE - (int)((float)SMALLOBJECT_DAMAGE_GRENADE/MAX_DAMAGE_GRENADE_DISTANCE * r);
			SmallObjectIndex[i].HitGrenadeExplosion(damage);

			//����������ʉ��𔭂���
			GameSound->HitSmallObject(sx, sy, sz, id);

			returnflag = true;
		}
	}

	//�G�t�F�N�g�i�t���b�V���j�̕\��
	AddEffect(gx, gy, gz, 0.0f, 0.0f, 0.0f, 30.0f, 0.0f, 2, Resource->GetEffectMflashTexture(), EFFECT_NORMAL);

	//�G�t�F�N�g�i���j�̕\��
	float rnd = DegreeToRadian(10)*GetRand(18);
	AddEffect(gx+1.0f, gy+1.0f, gz+1.0f, 0.1f, 0.2f, 0.1f, 50.0f, rnd, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_ROTATION | EFFECT_TRANSLUCENT);
	AddEffect(gx-1.0f, gy-1.0f, gz-1.0f, -0.1f, 0.2f, -0.1f, 50.0f, rnd*-1, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_ROTATION | EFFECT_TRANSLUCENT);
	AddEffect(gx-1.0f, gy-1.0f, gz+1.0f, -0.1f, 0.2f, 0.1f, 50.0f, rnd, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_ROTATION | EFFECT_TRANSLUCENT);
	AddEffect(gx+1.0f, gy+1.0f, gz-1.0f, 0.1f, 0.2f, -0.1f, 50.0f, rnd*-1, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_ROTATION | EFFECT_TRANSLUCENT);

	//���ʉ����Đ�
	GameSound->GrenadeExplosion(gx, gy, gz);

	return returnflag;
}

//! @brief �|�ꂽ�ۂ̃G�t�F�N�g�ݒ�
//! @param in_human �Ώۂ̐l�I�u�W�F�N�g
void ObjectManager::DeadEffect(human *in_human)
{
	if( in_human == NULL ){ return; }

	int paramid;
	float hx, hy, hz, hrx, hry;
	HumanParameter data;

	//�e��p�����[�^�[���擾
	in_human->GetParamData(&paramid, NULL, NULL, NULL);
	in_human->GetPosData(&hx, &hy, &hz, &hrx);
	hry = in_human->GetDeadRy();

	//�ݒ�l���擾
	if( GameParamInfo->GetHuman(paramid, &data) != 0 ){ return; }

	//�������{�b�g�Ȃ��
	if( data.type == 1 ){

		//���ӂ�̍��W���Z�o
		hx += cos(hrx*-1 - (float)M_PI/2) * sin(hry) * HUMAN_HEIGTH/2;
		hz += sin(hrx*-1 - (float)M_PI/2) * sin(hry) * HUMAN_HEIGTH/2;

		//�G�t�F�N�g�i���j�̕\��
		float rnd = DegreeToRadian(10)*GetRand(18);
		AddEffect(hx+1.0f, hy+1.0f, hz+1.0f, 0.0f, 0.05f, 0.0f, 10.0f, rnd, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION);
		AddEffect(hx-1.0f, hy-1.0f, hz-1.0f, 0.0f, 0.05f, 0.0f, 10.0f, rnd*-1, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION);
		AddEffect(hx-1.0f, hy-1.0f, hz+1.0f, 0.0f, 0.05f, 0.0f, 10.0f, rnd, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION);
		AddEffect(hx+1.0f, hy+1.0f, hz-1.0f, 0.0f, 0.05f, 0.0f, 10.0f, rnd*-1, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION);
	}
}

//! �}�b�v�Ɍ����t�����邩����
//! @param in_effect �Ώۂ̃G�t�F�N�g�I�u�W�F�N�g
//! @param id �t������u���b�N�ԍ����󂯎��|�C���^
//! @param face �t������ʔԍ����󂯎��|�C���^
//! @param pos_x X���W���󂯎��|�C���^
//! @param pos_y Y���W���󂯎��|�C���^
//! @param pos_z Z���W���󂯎��|�C���^
//! @return �t������Ftrue�@�t�����Ȃ��Ffalse
bool ObjectManager::CollideBlood(effect *in_effect, int *id, int *face, float *pos_x, float *pos_y, float *pos_z)
{
	//�����ȃG�t�F�N�g�Ȃ�Ώ������Ȃ�
	if( in_effect->GetEnableFlag() == false ){ return false; }
	if( in_effect->GetCollideMapFlag() == false ){ return false; }
	if( in_effect->GetTextureID() != Resource->GetEffectBloodTexture() ){ return false; }

	//�����o�Ȃ��ݒ�Ȃ珈�����Ȃ�
	if( GameConfig.GetBloodFlag() == false ){ return false; }

	float x, y, z;
	float move_x, move_y, move_z;
	float vx, vy, vz, dist, dist2;

	//�G�t�F�N�g�̃p�����[�^�[�擾
	in_effect->GetPosData(&x, &y, &z, NULL);
	in_effect->GetMove(&move_x, &move_y, &move_z);

	//�G�t�F�N�g���ړ����Ȃ��Ȃ珈�����Ȃ�
	if( (move_x == 0.0f)&&(move_y == 0.0f)&&(move_z == 0.0f) ){ return false; }

	//�x�N�g�������߂�
	dist = sqrt(move_x*move_x + move_y*move_y + move_z*move_z);
	vx = move_x / dist;
	vy = move_y / dist;
	vz = move_z / dist;

	//���C�̓����蔻��
	if( CollD->CheckALLBlockIntersectRay(x, y, z, vx, vy, vz, id, face, &dist2, dist) == false ){
		return false;
	}

	//�}�b�v���킸���ɕ�������
	dist2 -= 1.0f;

	//�t��������W�����߂�
	*pos_x = x + vx*dist2;
	*pos_y = y + vy*dist2;
	*pos_z = z + vz*dist2;

	return true;
}

//! @brief ������E��
//! @param in_human �Ώۂ̐l�I�u�W�F�N�g
//! @param in_weapon �Ώۂ̕���I�u�W�F�N�g
void ObjectManager::PickupWeapon(human *in_human, weapon *in_weapon)
{
	//�����Ȑl�Ȃ�Ώ������Ȃ�
	if( in_human->GetEnableFlag() == false ){ return; }
	if( in_human->GetHP() <= 0 ){ return; }

	//����������Ă��镐��ŁA���N���g���Ă��Ȃ�����Ȃ�΁`
	if( (in_weapon->GetEnableFlag() == true)&&(in_weapon->GetUsingFlag() == false) ){
		float human_x, human_y, human_z;
		float weapon_x, weapon_y, weapon_z;
		float x, z;
		float r;

		//�l�ƕ���̍��W���擾
		in_human->GetPosData(&human_x, &human_y, &human_z, NULL);
		in_weapon->GetPosData(&weapon_x, &weapon_y, &weapon_z, NULL);

		//�������͈͓��Ȃ��
		if( (human_y-2.0f <= weapon_y)&&(human_y+16.0f > weapon_y) ){
			//�������v�Z
			x = human_x - weapon_x;
			z = human_z - weapon_z;
			r = x*x + z*z;
			//�������͈͓��Ȃ�΁A�E�킹��B
			if( r < (5.0f * 5.0f) ){
				in_human->PickupWeapon(in_weapon);
			}
		}
	}
}

//! @brief �I�u�W�F�N�g�����
void ObjectManager::CleanupPointDataToObject()
{
	for(int i=0; i<MAX_HUMAN; i++){
		HumanIndex[i].SetEnableFlag(false);
	}

	for(int i=0; i<MAX_WEAPON; i++){
		WeaponIndex[i].SetEnableFlag(false);
	}

	for(int i=0; i<MAX_SMALLOBJECT; i++){
		SmallObjectIndex[i].SetEnableFlag(false);
	}

	for(int i=0; i<MAX_BULLET; i++){
		BulletIndex[i].SetEnableFlag(false);
	}

	for(int i=0; i<MAX_GRENADE; i++){
		GrenadeIndex[i].SetEnableFlag(false);
	}

	for(int i=0; i<MAX_EFFECT; i++){
		EffectIndex[i].SetEnableFlag(false);
	}


	if( Resource != NULL ){
		Resource->CleanupHumanTexture();
	}
}

//! @brief �|�C���g�f�[�^�����ɃI�u�W�F�N�g��z�u
void ObjectManager::LoadPointData()
{
	FriendlyFire = false;
	Player_HumanID = 0;

	/*
	//�l���|�C���g��T��
	for(int i=0; i<PointData->GetTotaldatas(); i++){
		pointdata data;
		PointData->Getdata(&data, i);

		if( data.p1 == 4 ){
			//�l�̃e�N�X�`����o�^
			Resource->AddHumanTexture(data.p2);
		}
	}
	*/

	//�l�E����E������T��
	for(int i=0; i<PointData->GetTotaldatas(); i++){
		pointdata data;
		PointData->Getdata(&data, i);

		//�l�Ȃ��
		if( (data.p1 == 1)||(data.p1 == 6) ){
			pointdata humaninfodata;

			//�l���|�C���g��T��
			if( PointData->SearchPointdata(&humaninfodata, 0x01 + 0x08, 4, 0, 0, data.p2, 0) == 0 ){
				//continue;

				//�l���|�C���g��������Ȃ�������A�Ƃ肠�����u���� �� A�v�Ƃ��Ēǉ��B�i�o�O�̍Č��j

				HumanParameter HumanParam;
				int Weapon[TOTAL_HAVEWEAPON];
				GameParamInfo->GetHuman(0, &HumanParam);
				for(int j=0; j<TOTAL_HAVEWEAPON; j++){
					Weapon[j] = HumanParam.Weapon[j];
				}

				//�l�̃e�N�X�`����o�^
				Resource->AddHumanTexture(0);

				//�l��ǉ�
				if( data.p1 == 6 ){
					Weapon[1] = ID_WEAPON_NONE;
				}

				//�v���C���[�Ȃ�΁A�ԍ����L�^
				if( (data.p4 == 0)&&(Player_HumanID == 0) ){
					Player_HumanID = AddHumanIndex(data.x, data.y, data.z, data.r, 0, i, Weapon);
				}
				else{
					AddHumanIndex(data.x, data.y, data.z, data.r, 0, i, Weapon);
				}
			}
			else{
				//�l�̃e�N�X�`����o�^
				Resource->AddHumanTexture(humaninfodata.p2);

				//�l�Ƃ��Ēǉ�
				AddHumanIndex(data, humaninfodata);
			}
		}

		//����Ȃ��
		if( (data.p1 == 2)||(data.p1 == 7) ){
			AddWeaponIndex(data);
		}

		//�����Ȃ��
		if( data.p1 == 5 ){
			AddSmallObjectIndex(data);
		}
	}
}

//! @brief ���\�[�X�̉�
//! @todo �S�ẴG�t�F�N�g������������B
void ObjectManager::Recovery()
{
	//�l�̃��f���ԍ����擾
	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);

	//�l
	int HumanID;
	int GetHumanFlag;
	HumanParameter HumanParam;
	for(int i=0; i<MAX_HUMAN; i++){
		if( HumanIndex[i].GetEnableFlag() == true ){
			HumanIndex[i].GetParamData(&HumanID, NULL, NULL, NULL);

			GetHumanFlag = GameParamInfo->GetHuman(HumanID, &HumanParam);

			if( GetHumanFlag == 0 ){
				//�l�̃e�N�X�`����o�^
				Resource->AddHumanTexture(HumanID);

				//�ǂݍ��߂Ȃ���΁A�O��ǂݍ��񂾃e�N�X�`���ԍ��𗘗p
				//�ǂݍ��߂�΁A����ǂݍ��ރe�N�X�`���ԍ����㏑��
				int id = Resource->GetHumanTexture(HumanID);
				if( id == -1 ){
					id = AddHumanIndex_TextureID;
				}
				else{
					AddHumanIndex_TextureID = id;
				}

				HumanIndex[i].SetTexture(id);
				HumanIndex[i].SetModel(upmodel[ HumanParam.model ], armmodel, legmodel, walkmodel, runmodel);
			}
			else{
				//����ǂݍ��ރe�N�X�`���ԍ����㏑��
				AddHumanIndex_TextureID = d3dg->GetMapTextureID(0);

				HumanIndex[i].SetTexture(AddHumanIndex_TextureID);
				HumanIndex[i].SetModel(upmodel[0], armmodel, legmodel, walkmodel, runmodel);
			}
		}
	}

	//����
	int WeaponID;
	int Weaponmodel, Weapontexture;
	for(int i=0; i<MAX_WEAPON; i++){
		if( WeaponIndex[i].GetEnableFlag() == true ){
			//�ݒ�l���擾
			WeaponParameter WeaponParam;
			WeaponIndex[i].GetParamData(&WeaponID, NULL, NULL);
			if( Resource->GetWeaponModelTexture(WeaponID, &Weaponmodel, &Weapontexture) == 1 ){ continue; }
			if( GameParamInfo->GetWeapon(WeaponID, &WeaponParam) == 1 ){ continue; }

			//�K�p
			WeaponIndex[i].SetModel(Weaponmodel, WeaponParam.size);
			WeaponIndex[i].SetTexture(Weapontexture);
		}
	}

	//����
	int SmallObjectID;
	int SmallObjectmodel, SmallObjecttexture;
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		if( SmallObjectIndex[i].GetEnableFlag() == true ){
			//�ݒ�l���擾
			SmallObjectIndex[i].GetParamData(&SmallObjectID, NULL);
			if( Resource->GetSmallObjectModelTexture(SmallObjectID, &SmallObjectmodel, &SmallObjecttexture) == 1 ){ continue; }

			//�K�p
			SmallObjectIndex[i].SetModel(SmallObjectmodel, SMALLOBJECT_SCALE);
			SmallObjectIndex[i].SetTexture(SmallObjecttexture);
		}
	}

	int bulletmodel, bullettexture;
	BulletParameter data;

	//�e�e�K�p
	if( Resource->GetBulletModelTexture(0, &bulletmodel, &bullettexture) == 0 ){
		if( GameParamInfo->GetBullet(0, &data) == 0 ){
			for(int i=0; i<MAX_BULLET; i++){
				BulletIndex[i].SetModel(bulletmodel, data.size);
				BulletIndex[i].SetTexture(bullettexture);
			}
		}
	}

	//��֒e�K�p
	if( Resource->GetBulletModelTexture(1, &bulletmodel, &bullettexture) == 0 ){
		if( GameParamInfo->GetBullet(1, &data) == 0 ){
			for(int i=0; i<MAX_GRENADE; i++){
				GrenadeIndex[i].SetModel(bulletmodel, data.size);
				GrenadeIndex[i].SetTexture(bullettexture);
			}
		}
	}

	//�S�ẴG�t�F�N�g�𖳌���
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetEnableFlag() == true ){
			EffectIndex[i].SetEnableFlag(false);
		}
	}
}

//! @brief FF�i���m�����j�L�����t���O���擾
//! @return �t���O
bool ObjectManager::GetFriendlyFireFlag()
{
	return FriendlyFire;
}

//! @brief FF�i���m�����j�L�����t���O��ݒ�
//! @param flag �t���O
void ObjectManager::SetFriendlyFireFlag(bool flag)
{
	FriendlyFire = flag;
}

//! @brief �v���C���[�ԍ����擾
//! @return �v���C���[�̃f�[�^�ԍ�
int ObjectManager::GetPlayerID()
{
	return Player_HumanID;
}

//! @brief �v���C���[�ԍ���ݒ�
//! @param id �v���C���[�̃f�[�^�ԍ�
void ObjectManager::SetPlayerID(int id)
{
	Player_HumanID = id;
}

//! @brief �w�肵���f�[�^�ԍ���human�N���X���擾
//! @param id �f�[�^�ԍ�
//! @return �l�I�u�W�F�N�g�̃|�C���^�@�i�����ȃf�[�^�ԍ��� NULL�j
human* ObjectManager::GeHumanObject(int id)
{
	if( (id < 0)||(MAX_HUMAN-1 < id) ){ return NULL; }
	return &(HumanIndex[id]);
}

//! @brief �v���C���[��human�N���X���擾
//! @return �l�I�u�W�F�N�g�i�v���C���[�j�̃|�C���^
human* ObjectManager::GetPlayerHumanObject()
{
	return GeHumanObject(Player_HumanID);
}

//! @brief �w�肵���f�[�^�ԍ���weapon�N���X���擾
//! @param id �f�[�^�ԍ�
//! @return ����I�u�W�F�N�g�̃|�C���^�@�i�����ȃf�[�^�ԍ��� NULL�j
weapon* ObjectManager::GetWeaponObject(int id)
{
	if( (id < 0)||(MAX_WEAPON-1 < id) ){ return NULL; }
	return &(WeaponIndex[id]);
}

//! @brief �w�肵���f�[�^�ԍ���smallobject�N���X���擾
//! @param id �f�[�^�ԍ�
//! @return �����I�u�W�F�N�g�̃|�C���^�@�i�����ȃf�[�^�ԍ��� NULL�j
smallobject* ObjectManager::GetSmallObject(int id)
{
	if( (id < 0)||(MAX_SMALLOBJECT-1 < id) ){ return NULL; }
	return &(SmallObjectIndex[id]);
}

//! @brief �w�肵���f�[�^�ԍ���bullet�N���X���擾
//! @param id �f�[�^�ԍ�
//! @return �e�I�u�W�F�N�g�̃|�C���^�@�i�����ȃf�[�^�ԍ��� NULL�j
bullet* ObjectManager::GetBulletObject(int id)
{
	if( (id < 0)||(MAX_BULLET-1 < id) ){ return NULL; }
	return &(BulletIndex[id]);
}

//! @brief �w�肵��bullet�|�C���^�̃f�[�^�ԍ����擾
//! @param object �e�I�u�W�F�N�g�̃|�C���^
//! @return �f�[�^�ԍ��@�i�G���[�F-1�j
int ObjectManager::GetBulletObjectID(bullet* object)
{
	for(int i=0; i<MAX_BULLET; i++){
		if( &(BulletIndex[i]) == object ){
			return i;
		}
	}

	return -1;
}

//! @brief �g�p����Ă��Ȃ�bullet�N���X���擾
//! @return ���ݖ��g�p�̒e�I�u�W�F�N�g�̃|�C���^�@�i���s����� NULL�j
bullet* ObjectManager::GetNewBulletObject()
{
	for(int i=0; i<MAX_BULLET; i++){
		if( BulletIndex[i].GetEnableFlag() == false ){
			return &(BulletIndex[i]);
		}
	}
	return NULL;
}

//! @brief �g�p����Ă��Ȃ�grenade�N���X���擾
//! @return ���ݖ��g�p�̎�֒e�I�u�W�F�N�g�̃|�C���^�@�i���s����� NULL�j
grenade* ObjectManager::GetNewGrenadeObject()
{
	for(int i=0; i<MAX_GRENADE; i++){
		if( GrenadeIndex[i].GetEnableFlag() == false ){
			return &(GrenadeIndex[i]);
		}
	}
	return NULL;
}

//! @brief �l������
//! @param p4 �����Ώۂ̔F���ԍ�
//! @return �Y������human�N���X�̃|�C���^�@�i������Ȃ��ꍇ��NULL�j
//! @attention �����Y������ꍇ�A�ŏ��ɊY�������f�[�^��Ԃ��܂��B
human* ObjectManager::SearchHuman(signed char p4)
{
	signed char humanp4;

	for(int i=0; i<MAX_HUMAN; i++){
		//�g���Ă��Ȃ��l�Ȃ�Ώ������Ȃ�
		if( HumanIndex[i].GetEnableFlag() == false ){ continue; }

		//��4�p�����[�^���擾
		HumanIndex[i].GetParamData(NULL, NULL, &humanp4, NULL);

		//�w�肳�ꂽp4�ƈ�v����ΕԂ�
		if( humanp4 == p4 ){
			return &(HumanIndex[i]);
		}
	}
	return NULL;
}

//! @brief ����������
//! @param p4 �����Ώۂ̔F���ԍ�
//! @return �Y������smallobject�N���X�̃|�C���^�@�i������Ȃ��ꍇ��NULL�j
//! @attention �����Y������ꍇ�A�ŏ��ɊY�������f�[�^��Ԃ��܂��B
smallobject* ObjectManager::SearchSmallobject(signed char p4)
{
	signed char smallobjectp4;

	for(int i=0; i<MAX_SMALLOBJECT; i++){
		//�g���Ă��Ȃ��l�Ȃ�Ώ������Ȃ�
		//�@�y�j��ς݂̃I�u�W�F�N�g�����肷�邽�߁A�����z
		//if( SmallObjectIndex[i].GetEnableFlag() == false ){ continue; }

		//��4�p�����[�^���擾
		SmallObjectIndex[i].GetParamData(NULL, &smallobjectp4);

		//�w�肳�ꂽp4�ƈ�v����ΕԂ�
		if( smallobjectp4 == p4 ){
			return &(SmallObjectIndex[i]);
		}
	}
	return NULL;
}

//! @brief �O�i�i����j�����s
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
void ObjectManager::MoveForward(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//�I�u�W�F�N�g�Ƀt���O��ݒ�
	HumanIndex[human_id].SetMoveForward();
}

//! @brief ��ނ����s
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
void ObjectManager::MoveBack(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//�I�u�W�F�N�g�Ƀt���O��ݒ�
	HumanIndex[human_id].SetMoveBack();
}

//! @brief ����������s
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
void ObjectManager::MoveLeft(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//�I�u�W�F�N�g�Ƀt���O��ݒ�
	HumanIndex[human_id].SetMoveLeft();
}

//! @brief �E��������s
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
void ObjectManager::MoveRight(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//�I�u�W�F�N�g�Ƀt���O��ݒ�
	HumanIndex[human_id].SetMoveRight();
}

//! @brief ���������s
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
void ObjectManager::MoveWalk(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//�I�u�W�F�N�g�Ƀt���O��ݒ�
	HumanIndex[human_id].SetMoveWalk();
}

//! @brief �W�����v
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
void ObjectManager::MoveJump(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	HumanIndex[human_id].Jump();
}

//! @brief ���C
//! @param human_id ���C����l�ԍ�
//! @return �ʏ�e���ˁF1�@��֒e���ˁF2�@���s�F0
int ObjectManager::ShotWeapon(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return 0; }

	human *MyHuman = &(HumanIndex[human_id]);

	float pos_x, pos_y, pos_z;
	int teamid;
	float rotation_x, armrotation_y;
	int weapon_paramid;
	int GunsightErrorRange;
	WeaponParameter ParamData;
	bool playerflag;
	bool grenadeflag;

	//�l�̍��W�Ɗp�x���擾
	MyHuman->GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	MyHuman->GetParamData(NULL, NULL, NULL, &teamid);
	MyHuman->GetRxRy(&rotation_x, &armrotation_y);

	//�Ώێ҂��v���C���[���g������
	if( MyHuman == &(HumanIndex[Player_HumanID]) ){
		playerflag = true;
	}
	else{
		playerflag = false;
	}

	//�e�̔��˂�v��
	if( MyHuman->ShotWeapon(&weapon_paramid, &GunsightErrorRange) == false ){ return 0; }

	//����̏����擾
	if( GameParamInfo->GetWeapon(weapon_paramid, &ParamData) != 0 ){ return 0; }

	//��֒e������
	if( weapon_paramid == ID_WEAPON_GRENADE ){ grenadeflag = true; }
	else{ grenadeflag = false; }

	//�덷�͈̔͂��v�Z
	int ErrorRange;
	ErrorRange = GunsightErrorRange;
	if( ErrorRange < ParamData.ErrorRangeMIN ){ ErrorRange = ParamData.ErrorRangeMIN; }

	//���ˊp�x�i�덷�[���̃x�[�X�j������
	float rx, ry;
	rx = rotation_x*-1 + (float)M_PI/2;
	ry = armrotation_y;

	//�덷������
	int ErrorRange_x, ErrorRange_y;
	if( (ErrorRange - ParamData.ErrorRangeMIN) == 0 ){
		ErrorRange_x = ErrorRange;
		ErrorRange_y = ErrorRange;
	}
	else{
		ErrorRange_x = GetRand(ErrorRange - ParamData.ErrorRangeMIN) + ParamData.ErrorRangeMIN;
		ErrorRange_y = GetRand(ErrorRange - ParamData.ErrorRangeMIN) + ParamData.ErrorRangeMIN;
	}
	if( GetRand(2) == 0 ){ ErrorRange_x *= -1; }
	if( GetRand(2) == 0 ){ ErrorRange_y *= -1; }

	//���ˌ덷�̊p�x�����߂�
	float a, r;
	a = atan2((float)ErrorRange_y, (float)ErrorRange_x);
	r = sqrt((float)(ErrorRange_x*ErrorRange_x + ErrorRange_y*ErrorRange_y));

	//�덷���@�p�x�ɉ��Z
	rx += cos(a)*r * DegreeToRadian(0.15f);
	ry += sin(a)*r * DegreeToRadian(0.15f);

	//��֒e�łȂ����
	if( grenadeflag == false ){
		class bullet* newbullet;

		//�i�V���b�g�K���Ȃǁj���˂���e�̐����J��Ԃ�
		for(int i=0; i<ParamData.burst; i++){
			int attacks;
			float rx2, ry2;

			//�i�V���b�g�K���Ȃǁj���˂���e�����������
			if( ParamData.burst > 1 ){
				//1�̒e������̍U���͂��Z�o
				//�@�@�S�e���킹�āA�U���͂�2�{�ɂȂ�悤�ɂ���B
				attacks = (int)( (float)ParamData.attacks / ((float)ParamData.burst/2) );

				//����Ɍ덷�𑝂₵�Ċg�U������
				int len;
				a = DegreeToRadian(10) * GetRand(36);
				len = GetRand(5)*2+5;
				rx2 = rx + cos(a)*len * DegreeToRadian(0.15f);
				ry2 = ry + sin(a)*len * DegreeToRadian(0.15f);
			}
			else{
				//���̂܂܍U���͂֔��f
				attacks = ParamData.attacks;

				//���ˌ덷�͂��̂܂�
				rx2 = rx;
				ry2 = ry;
			}

			//���˂��関�g�p�̃I�u�W�F�N�g���擾
			newbullet = GetNewBulletObject();
			if( newbullet == NULL ){ return 0; }

			//�e�e�𔭎�
			newbullet->SetPosData(pos_x, pos_y + WEAPONSHOT_HEIGHT, pos_z, rx2, ry2);
			newbullet->SetParamData(attacks, ParamData.penetration, ParamData.speed * BULLET_SPEEDSCALE, teamid, human_id, true);
			newbullet->SetEnableFlag(true);

			//�ΐl����p���X�g������
			BulletObj_HumanIndex[ GetBulletObjectID(newbullet) ].Init();
		}

		//���C�t���O��ݒ�
		Human_ShotFlag[human_id] = true;
	}
	else{
		class grenade* newgrenade;

		//���˂��関�g�p�̃I�u�W�F�N�g���擾
		newgrenade = GetNewGrenadeObject();
		if( newgrenade == NULL ){ return 0; }

		//��֒e����
		newgrenade->SetPosData(pos_x, pos_y + WEAPONSHOT_HEIGHT, pos_z, rx, ry);
		newgrenade->SetParamData(8.0f, human_id, true);
		newgrenade->SetEnableFlag(true);
	}

	if( ParamData.soundvolume > 0 ){
		//�e�����Đ�
		GameSound->ShotWeapon(pos_x, pos_y + WEAPONSHOT_HEIGHT, pos_z, weapon_paramid, teamid, playerflag);
	}

	if( grenadeflag == true ){
		return 2;
	}
	return 1;
}

//! @brief �}�Y���t���b�V����ݒ�
//! @param humanid �l�̔ԍ�
//! @attention ���̊֐��̌Ăяo���^�C�~���O�����ƁA�e���ɑ΂��ă}�Y���t���b�V���������܂���B
void ObjectManager::ShotWeaponEffect(int humanid)
{
	float pos_x, pos_y, pos_z;
	float rotation_x, armrotation_y;
	int weapon_paramid;
	WeaponParameter ParamData;
	float x, y, z;
	float flashsize, smokesize, yakkyousize;
	float rx, mx, my, mz;

	//�l�̍��W�Ɗp�x���擾
	HumanIndex[humanid].GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	HumanIndex[humanid].GetRxRy(&rotation_x, &armrotation_y);

	//����̏����擾
	weapon_paramid = HumanIndex[humanid].GetMainWeaponTypeNO();
	if( GameParamInfo->GetWeapon(weapon_paramid, &ParamData) != 0 ){ return; }

	//�}�Y���t���b�V���Ɖ��̃T�C�Y������
	flashsize = 0.06f * ParamData.attacks;
	smokesize = flashsize;
	yakkyousize = 0.01f * ParamData.attacks;
	if( ParamData.silencer == true ){
		flashsize /= 2;
	}

	//��䰂̈ړ��ʂ�����
	rx = rotation_x*-1 + (float)M_PI/2;
	mx = cos(rx - (float)M_PI/2) * ParamData.yakkyou_sx /10;
	my = (ParamData.yakkyou_sy + (GetRand(3)-1)) /10;
	mz = sin(rx - (float)M_PI/2) * ParamData.yakkyou_sx /10;

	//�s��ŃG�t�F�N�g���W���v�Z
	d3dg->SetWorldTransformHumanWeapon(pos_x, pos_y + 16.0f, pos_z, ParamData.flashx/10, ParamData.flashy/10, ParamData.flashz/10, rotation_x, armrotation_y*-1, 1.0f);
	d3dg->GetWorldTransformPos(&x, &y, &z);
	d3dg->ResetWorldTransform();

	//�}�Y���t���b�V���`��
	AddEffect(x, y, z, 0.0f, 0.0f, 0.0f, flashsize, DegreeToRadian(10)*GetRand(18), 1, Resource->GetEffectMflashTexture(), EFFECT_NORMAL);

	//�s��ŃG�t�F�N�g���W���v�Z
	d3dg->SetWorldTransformHumanWeapon(pos_x, pos_y + 16.0f, pos_z, ParamData.flashx/10, ParamData.flashy/10, ParamData.flashz/10 - 0.1f, rotation_x, armrotation_y*-1, 1.0f);
	d3dg->GetWorldTransformPos(&x, &y, &z);
	d3dg->ResetWorldTransform();

	//�G�t�F�N�g�i���j�̕\��
	AddEffect(x, y, z, 0.0f, 0.05f, 0.0f, smokesize, DegreeToRadian(10)*GetRand(18), (int)(GAMEFPS/3), Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION);

	//�s��ŃG�t�F�N�g���W���v�Z
	d3dg->SetWorldTransformHumanWeapon(pos_x, pos_y + 16.0f, pos_z, ParamData.yakkyou_px/10, ParamData.yakkyou_py/10, ParamData.yakkyou_pz/10, rotation_x, armrotation_y*-1, 1.0f);
	d3dg->GetWorldTransformPos(&x, &y, &z);
	d3dg->ResetWorldTransform();

	//��䰕`��
	AddEffect(x, y, z, mx, my, mz, yakkyousize, DegreeToRadian(10)*GetRand(18), (int)(GAMEFPS/2), Resource->GetEffectYakkyouTexture(), EFFECT_ROTATION | EFFECT_FALL);
}

//! @brief ����������[�h
//! @param human_id �Ώۂ̐l�ԍ�
void ObjectManager::ReloadWeapon(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//�����Ȑl�Ȃ�Ώ������Ȃ�
	if( HumanIndex[human_id].GetEnableFlag() == false ){ return; }
	if( HumanIndex[human_id].GetHP() <= 0 ){ return; }

	//�����[�h�����s
	if( HumanIndex[human_id].ReloadWeapon() == true ){
		float x, y, z;
		int id;

		//�l�̍��W�ƃ`�[���ԍ����擾
		HumanIndex[human_id].GetPosData(&x, &y, &z, NULL);
		HumanIndex[human_id].GetParamData(NULL, NULL, NULL, &id);
		y += 16.0f;

		//������z�u
		GameSound->ReloadWeapon(x, y, z, id);
	}
}

//! @brief �����؂�ւ���i�����ւ���j
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
//! @param id �����ւ��镐��@�i-1 �Ŏ��̕���j
void ObjectManager::ChangeWeapon(int human_id, int id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	HumanIndex[human_id].ChangeWeapon(id);
}

//! @brief ������̂Ă�
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
//! @return �����Ftrue�@���s�Ffalse
bool ObjectManager::DumpWeapon(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return false; }

	return HumanIndex[human_id].DumpWeapon();
}

//! @brief �X�R�[�v���[�h��؂�ւ�
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
void ObjectManager::ChangeScopeMode(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	if( HumanIndex[human_id].GetScopeMode() == 0 ){	//�X�R�[�v���g�p���Ă��Ȃ���΁A�X�R�[�v��ݒ�
		HumanIndex[human_id].SetEnableScope();
	}
	else{											//�g�p���Ȃ�A����
		HumanIndex[human_id].SetDisableScope();
	}
}

//! @brief ����̃V���b�g���[�h�؂�ւ�
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
//! @return �����F0�@���s�F1
int ObjectManager::ChangeShotMode(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return 1; }

	return HumanIndex[human_id].ChangeShotMode();
}

//! @brief ���Z�E�������Ă��镐��̒e��ǉ�
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
//! @return �����Ftrue�@���s�Ffalse
bool ObjectManager::CheatAddBullet(int human_id)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return false; }

	int selectweapon;
	weapon *weapon[TOTAL_HAVEWEAPON];
	int id_param, lnbs, nbs;
	WeaponParameter ParamData;

	//�������Ă��镐����擾
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}
	HumanIndex[human_id].GetWeapon(&selectweapon, weapon);

	//��������̕���������Ă����
	if( weapon[selectweapon] != NULL ){
		//����̎�ނƒe���A����̐ݒ�l���擾
		weapon[selectweapon]->GetParamData(&id_param, &lnbs, &nbs);
		if( GameParamInfo->GetWeapon(id_param, &ParamData) == 0 ){
			//�ő�e�������Z���A�K�p
			nbs += ParamData.nbsmax;
			weapon[selectweapon]->ResetWeaponParam(Resource, id_param, lnbs, nbs);
			return true;
		}
	}

	return false;
}

//! @brief ���Z�E�������Ă��镐���ύX
//! @param human_id �l�̔ԍ��i0�`MAX_HUMAN-1�j
//! @param new_weaponID �V���ɐݒ肷�镐��̎�ޔԍ�
//! @return �����Ftrue�@���s�Ffalse
bool ObjectManager::CheatNewWeapon(int human_id, int new_weaponID)
{
	//�l�͈̔͂��`�F�b�N
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return false; }
	if( (new_weaponID < 0)||(TOTAL_PARAMETERINFO_WEAPON <= new_weaponID) ){ return false; }

	human *myHuman = &(HumanIndex[human_id]);

	int selectweapon;
	weapon *weapon[TOTAL_HAVEWEAPON];
	int lnbs, nbs;

	//�������Ă��镐����擾
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}
	myHuman->GetWeapon(&selectweapon, weapon);

	//���ݕ�������L���Ă��炸�A�V���Ɏ������镐����Ȃ��Ȃ�
	if( (weapon[selectweapon] == NULL)&&(new_weaponID == ID_WEAPON_NONE) ){
		return false;		//�������̂����Ӗ�
	}

	if( weapon[selectweapon] == NULL ){		//��������L���Ă��Ȃ����
		int dataid = -1;

		//�V���������z�u
		dataid = AddVisualWeaponIndex(new_weaponID, false);

		//���킪�z�u�ł���΁A�������������
		if( dataid != -1 ){
			weapon[selectweapon] = &(WeaponIndex[dataid]);
			myHuman->SetWeapon(weapon);
			return true;
		}
	}
	else{									//��������L���Ă����
		if( new_weaponID == ID_WEAPON_NONE ){			//����������Ȃ�
			//��x������̂Ă���ŁA���̕�����폜
			if( myHuman->DumpWeapon() == true ){
				weapon[selectweapon]->SetEnableFlag(false);
				return true;
			}
		}
		else{											//�����ύX����Ȃ�
			//����ݒ���擾�i�e���ݒ�������p�����߁j
			weapon[selectweapon]->GetParamData(NULL, &lnbs, &nbs);

			//����ݒ��K�p
			weapon[selectweapon]->ResetWeaponParam(Resource, new_weaponID, lnbs, nbs);

			return true;
		}
	}

	return false;
}

//! @brief �]���r�̍U�����󂯂邩����
//! @param MyHuman �U������l�I�u�W�F�N�g�i�]���r���j�̃|�C���^
//! @param EnemyHuman �U�����󂯂��l�I�u�W�F�N�g�̃|�C���^
//! @return �����Ftrue�@�s�����Ffalse
//! @warning MyHuman �̓]���r�ȊO���w��ł��܂��B<b>�]���r���ǂ����͔��肵�Ȃ�</b>���߁A�]���r�ł��邩�\�ߊm�F���Ă����K�v������܂��B
//! @attention ����݂̂����{���܂��B���̔��肪���������� HitZombieAttack()�֐� ���Ăяo���Ă��������B 
bool ObjectManager::CheckZombieAttack(human* MyHuman, human* EnemyHuman)
{
	if( MyHuman == NULL ){ return false; }
	if( EnemyHuman == NULL ){ return false; }

	//�g�p����Ă��Ȃ����A���S���Ă���Ώ������Ȃ��B
	if( MyHuman->GetEnableFlag() == false ){ return false; }
	if( MyHuman->GetHP() <= 0 ){ return false; }
	if( EnemyHuman->GetEnableFlag() == false ){ return false; }
	if( EnemyHuman->GetHP() <= 0 ){ return false; }

	float mx, my, mz, mrx, tx, ty, tz;
	int mteam, tteam;
	float AttackPoint_x, AttackPoint_y, AttackPoint_z;
	float ax, az;

	MyHuman->GetPosData(&mx, &my, &mz, &mrx);
	MyHuman->GetParamData(NULL, NULL, NULL, &mteam);
	my += VIEW_HEIGHT;
	EnemyHuman->GetPosData(&tx, &ty, &tz, NULL);
	EnemyHuman->GetParamData(NULL, NULL, NULL, &tteam);
	ty += VIEW_HEIGHT;

	//�����Ȃ�Ώ������Ȃ�
	if( mteam == tteam ){ return false; }

	//�U���|�C���g�i�r�̐�[�j�����߂�
	AttackPoint_x = mx + cos(mrx*-1 + (float)M_PI/2) * 2.0f;
	AttackPoint_y = my + VIEW_HEIGHT - 0.5f;
	AttackPoint_z = mz + sin(mrx*-1 + (float)M_PI/2) * 2.0f;
	ax = AttackPoint_x - tx;
	az = AttackPoint_z - tz;

	//�G�i�U���Ώہj���U���|�C���g�ɐG��Ă���΁A�������Ă���
	if( (ax*ax + az*az) < 3.3f*3.3f ){
		if( (AttackPoint_y >= ty)&&(AttackPoint_y <= (ty + HUMAN_HEIGTH)) ){
			return true;
		}
	}

	return false;
}

//! @brief �]���r�̍U�����󂯂�����
//! @param EnemyHuman �U�����󂯂��l�I�u�W�F�N�g�̃|�C���^
void ObjectManager::HitZombieAttack(human* EnemyHuman)
{
	if( EnemyHuman == NULL ){ return; }

	//�g�p����Ă��Ȃ����A���S���Ă���Ώ������Ȃ��B
	if( EnemyHuman->GetEnableFlag() == false ){ return; }
	if( EnemyHuman->GetHP() <= 0 ){ return; }

	float tx, ty, tz;
	int paramid;
	HumanParameter Paraminfo;
	bool NotRobot;

	EnemyHuman->GetPosData(&tx, &ty, &tz, NULL);
	ty += VIEW_HEIGHT;

	//���{�b�g���ǂ�������
	EnemyHuman->GetParamData(&paramid, NULL, NULL, NULL);
	GameParamInfo->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 1 ){
		NotRobot = false;
	}
	else{
		NotRobot = true;
	}

	//�_���[�W�Ȃǂ��v�Z
	EnemyHuman->HitZombieAttack();

	//�G�t�F�N�g�i���j��\��
	SetHumanBlood(tx, ty, tz, HUMAN_DAMAGE_ZOMBIEU, NotRobot);

	//���ʉ����Đ�
	GameSound->HitHuman(tx, ty, tz);
}

//! @brief ���҂�h������
//! @param id �l�̔ԍ��i0�`MAX_HUMAN-1�j
//! @return true�F�����@false�F���s
//! @warning ��Ԃ�̂܂ܑh�����܂�
//! @attention �����Ȑl�ԍ����w�肳�ꂽ�ꍇ��A�w�肵���l���������Ă����ꍇ�A���邢�͓�l�Ԃɑ΂��Ď��s�����ꍇ�A���̊֐��͎��s���܂��B
bool ObjectManager::HumanResuscitation(int id)
{
	if( (id < 0)||(MAX_HUMAN-1 < id) ){ return false; }

	//�g�p����Ă��Ȃ����A�������Ă���Ώ������Ȃ��B
	if( HumanIndex[id].GetEnableFlag() == false ){ return false; }
	if( HumanIndex[id].GetDeadFlag() == false ){ return false; }

	int id_param, dataid, team;
	signed char p4;
	HumanIndex[id].GetParamData(&id_param, &dataid, &p4, &team);
	if( (id_param < 0)||( TOTAL_PARAMETERINFO_HUMAN-1 < id_param) ){ return false; }	//��l�ԂȂ珈�����Ȃ�
	HumanIndex[id].SetParamData(id_param, dataid, p4, team, true);

	return true;
}

//! @brief �Q�[���N���A�[�E�Q�[���I�[�o�[�̔���
//! @return �Q�[���N���A�[�F1�@�Q�[���I�[�o�[�F2�@����Ȃ��F0
//! @attention �Q�[���N���A�\�ƃQ�[���I�[�o�[�������ɐ�����������ł́A�{��XOPS�Ɠ��l�Ɂu�Q�[���N���A�[�v�Ɣ��肳��܂��B
int ObjectManager::CheckGameOverorComplete()
{
	//�����F
	//
	//�@�{���́A�Q�[���I�[�o�[������ɍs���A���ɃQ�[���N���A�[����������������������I�ł��B
	//�@�v���C���[��HP�� 0 �Ȃ�Α��Q�[���I�[�o�[�Ƃ��Ĕ��肵�A�G�̒���HP���c���Ă���҂����������_�� ����Ȃ��A
	//�@2�̏����ɓ��Ă͂܂�Ȃ���΁A�����I�ɃQ�[���N���A�[�ƂȂ�܂��B
	//
	//�@�������A�{��XOPS�̓Q�[���N���A�[�̔����D�悷�邽�߁A��ɃQ�[���N���A�[�̔�����s���Ă��܂��B
	//�@�����Ă���G�̐����i��HP�Ƃ��āj�����A�G�̐��i��HP�j�� 0 �Ȃ�΁A�Q�[���N���A�[�Ɣ��肵�܂��B
	//�@�܂����̂悤��1�̊֐��ŏ������悤�Ƃ����ɁA�Q�[���N���A�[�ƃQ�[���I�[�o�[�Ŋ֐����̂𕪂��Ă��ǂ��̂ł����ˁE�E�B

	//�Q�[���N���A�[����
	int MyTeamid, teamid;
	int TotalEnemyHP = 0;
	HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &MyTeamid);	//�v���C���[�̃`�[���ԍ����擾
	for(int i=0; i<MAX_HUMAN; i++){
		//����������Ă��Ȃ���Ώ������Ȃ�
		if( HumanIndex[i].GetEnableFlag() == false ){ continue; }

		//���ׂ�Ώۂ̃`�[���ԍ����擾
		HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);

		//�قȂ�`�[���ԍ��i���G�j�Ȃ��
		if( teamid != MyTeamid ){
			//�����Ă���΁A�G��HP�Ƃ��ĉ��Z
			//if( HumanIndex[i].GetHP() > 0 ){
			if( HumanIndex[i].GetDeadFlag() == false ){
				TotalEnemyHP += HumanIndex[i].GetHP();
			}
		}
	}
	if( TotalEnemyHP == 0 ){	//�S�G��HP�� 0 �Ȃ��
		return 1;
	}


	//�Q�[���I�[�o�[����
	if( HumanIndex[Player_HumanID].GetEnableFlag() == true ){	//����Ώۂ��L���Ȃ�΁@�i���F���Z�ɂ��ύX�΍�j
		if( HumanIndex[Player_HumanID].GetDeadFlag() == true ){		//�v���C���[�����S���Ă����
			return 2;
		}
	}

	return 0;
}

//! @brief �I�u�W�F�N�g�̏����擾
//! @param camera_x �J������X���W
//! @param camera_y �J������Y���W
//! @param camera_z �J������Z���W
//! @param camera_rx �J�����̉����p�x
//! @param camera_ry �J�����̏c���p�x
//! @param color �F���擾����|�C���^
//! @param infostr �������擾����|�C���^
//! @return �\����񂠂�Ftrue�@�\�����Ȃ��Ffalse
bool ObjectManager::GetObjectInfoTag(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, int *color, char *infostr)
{
	float dist = 50.0f;
	float px, py, pz;
	float rx, ry;
	float r;
	int Player_teamID;

	//������������
	infostr[0] = '\0';

	//�v���C���[�̃`�[���ԍ����擾
	HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &Player_teamID);

	//�l
	for(int i=0; i<MAX_HUMAN; i++){
		//�v���C���[���g�Ȃ珈�����Ȃ�
		if( i == Player_HumanID ){ continue; }

		if( HumanIndex[i].GetEnableFlag() == true ){
			HumanIndex[i].GetPosData(&px, &py, &pz, NULL);

			//���_����ɑΏۂ܂ł̊p�x���Z�o
			if( CheckTargetAngle(camera_x, camera_y, camera_z, camera_rx, 0.0f, px, py, pz, dist, &rx, NULL, &r) == true ){
				int team;

				//�p�x��A���E�ɓ����Ă����
				if( fabs(rx) < DegreeToRadian(10) ){
					HumanIndex[i].GetParamData(NULL, NULL, NULL, &team);
					if( team == Player_teamID ){
						*color = d3dg->GetColorCode(0.0f,0.0f,1.0f,1.0f);
						sprintf(infostr, "Human[%d]  HP %d : Friend", i, HumanIndex[i].GetHP());
					}
					else{
						*color = d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f);
						sprintf(infostr, "Human[%d]  HP %d : Enemy", i, HumanIndex[i].GetHP());
					}
					dist = sqrt(r);
				}
			}
		}
	}

	//����
	for(int i=0; i<MAX_WEAPON; i++){
		if( (WeaponIndex[i].GetEnableFlag() == true)&&(WeaponIndex[i].GetUsingFlag() == false) ){
			int lnbs, nbs;

			WeaponIndex[i].GetPosData(&px, &py, &pz, NULL);
			WeaponIndex[i].GetParamData(NULL, &lnbs, &nbs);

			//���_����ɑΏۂ܂ł̊p�x���Z�o
			if( CheckTargetAngle(camera_x, camera_y, camera_z, camera_rx, camera_ry, px, py, pz, dist, &rx, &ry, &r) == true ){
				//�p�x��A���E�ɓ����Ă����
				if( (fabs(rx) < DegreeToRadian(10))&&(fabs(ry) < DegreeToRadian(10)) ){
					*color = d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f);
					sprintf(infostr, "Weapon[%d]  %d:%d", i, lnbs, (nbs - lnbs));
					dist = sqrt(r);
				}
			}
		}
	}

	//����
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		if( SmallObjectIndex[i].GetEnableFlag() == true ){
			SmallObjectIndex[i].GetPosData(&px, &py, &pz, NULL);
			
			//���_����ɑΏۂ܂ł̊p�x���Z�o
			if( CheckTargetAngle(camera_x, camera_y, camera_z, camera_rx, camera_ry, px, py, pz, dist, &rx, &ry, &r) == true ){
				//�p�x��A���E�ɓ����Ă����
				if( (fabs(rx) < DegreeToRadian(10))&&(fabs(ry) < DegreeToRadian(10)) ){
					*color = d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f);
					sprintf(infostr, "SmallObject[%d]  HP %d", i, SmallObjectIndex[i].GetHP());
					dist = sqrt(r);
				}
			}
		}
	}

	//�������ݒ肳��Ă��邩�ǂ����ŁA�L���ȏ��������ꂽ������B
	if( infostr[0] == NULL ){
		return false;
	}
	return true;
}

//! @brief �I�u�W�F�N�g�̎�v�Z����
//! @param cmdF5id �㏸�@�\�iF5���Z�j������l�f�[�^�ԍ��i-1�ŋ@�\�����j
//! @param demomode �f�����[�h
//! @param camera_rx �J�����̉����p�x
//! @param camera_ry �J�����̏c���p�x
//! @return ��� 0
//! @attention ��ʓI�� cmdF5id �́AF5���Z�g�p���̓v���C���[�ԍ��iGetPlayerID()�֐��Ŏ擾�j�A���g�p���� -1 ���w�肵�܂��B
//! @attention demomode �͎�Ƀ��j���[��ʂŎg�p���܂��B�L���ɂ���ƁA�e�e�E��֒e���������܂���B
int ObjectManager::Process(int cmdF5id, bool demomode, float camera_rx, float camera_ry)
{
	//���̃t���[���̐����������
	for(int i=0; i<MAX_HUMAN; i++){
		Human_ontarget[i] = 0;
		Human_kill[i] = 0;
		Human_headshot[i] = 0;
	}

	//�l�I�u�W�F�N�g�̏���
	for(int i=0; i<MAX_HUMAN; i++){
		bool cmdF5;

		if( i == cmdF5id ){
			cmdF5 = true;
		}
		else{
			cmdF5 = false;
		}

		if( HumanIndex[i].RunFrame(CollD, BlockData, cmdF5) == 2 ){
			//���S���̃G�t�F�N�g
			DeadEffect(&(HumanIndex[i]));
		}

		//����
		if( HumanIndex[i].GetMovemode(false) == 2 ){
			//���鑫���ǉ�
			float posx, posy, posz;
			int teamid;
			HumanIndex[i].GetPosData(&posx, &posy, &posz, NULL);
			HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);
			GameSound->SetFootsteps(posx, posy, posz, teamid);
		}
	}

	//����I�u�W�F�N�g�̏���
	for(int i=0; i<MAX_WEAPON; i++){
		WeaponIndex[i].RunFrame(CollD);
	}

	//�����I�u�W�F�N�g�̏���
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		SmallObjectIndex[i].RunFrame();
	}

	if( demomode == false ){
		//�e�I�u�W�F�N�g�̏���
		for(int i=0; i<MAX_BULLET; i++){
			float bx, by, bz, brx, bry;
			int speed, teamid;
			float mx, my, mz;

			CollideBullet(&BulletIndex[i]);		//�����蔻������s
			BulletIndex[i].RunFrame();	//��v�Z

			if( BulletIndex[i].GetEnableFlag() == true ){
				//�e�̍��W�Ɗp�x���擾
				BulletIndex[i].GetParamData(NULL, NULL, &speed, &teamid, NULL);
				BulletIndex[i].GetPosData(&bx, &by, &bz, &brx, &bry);
				mx = cos(brx)*cos(bry)*speed;
				my = sin(bry)*speed;
				mz = sin(brx)*cos(bry)*speed;
				GameSound->PassingBullet(bx, by, bz, mx, my, mz, teamid);
			}
		}
	}
	else{
		//�S�Ă̒e�I�u�W�F�N�g������
		for(int i=0; i<MAX_BULLET; i++){
			if( BulletIndex[i].GetEnableFlag() == true ){
				BulletIndex[i].SetEnableFlag(false);
			}
		}
	}

	//����𔭖C�����ۂ̃G�t�F�N�g���o��
	for(int i=0; i<MAX_HUMAN; i++){
		if( Human_ShotFlag[i] == true ){
			ShotWeaponEffect(i);
			Human_ShotFlag[i] = false;
		}
	}

	//�G�t�F�N�g�I�u�W�F�N�g�̏���
	for(int i=0; i<MAX_EFFECT; i++){
		int id, face;
		float pos_x, pos_y, pos_z;
		if( CollideBlood(&(EffectIndex[i]), &id, &face, &pos_x, &pos_y, &pos_z) == true ){
			AddMapEffect(id, face, pos_x, pos_y, pos_z, 10.0f, DegreeToRadian(10)*GetRand(18), (int)GAMEFPS * 20, Resource->GetEffectBloodTexture());
			EffectIndex[i].SetEnableFlag(false);
		}
		else{
			EffectIndex[i].RunFrame(camera_rx, camera_ry);
		}
	}

	if( demomode == false ){
		//��֒e�̏���
		for(int i=0; i<MAX_GRENADE; i++){
			float speed = GrenadeIndex[i].GetSpeed();

			//��v�Z
			int rcr = GrenadeIndex[i].RunFrame(CollD);

			//�o�E���h�E���˕Ԃ����Ȃ��
			if( rcr == 1 ){
				if( speed > 3.4f ){
					//���W���擾���A���ʉ��Đ�
					float x, y, z;
					GrenadeIndex[i].GetPosData(&x, &y, &z, NULL, NULL);
					GameSound->GrenadeBound(x, y, z);
				}
			}

			//���������Ȃ�
			if( rcr == 2 ){
				GrenadeExplosion(&(GrenadeIndex[i]));
			}
		}
	}
	else{
		//�S�Ă̎�֒e������
		for(int i=0; i<MAX_GRENADE; i++){
			if( GrenadeIndex[i].GetEnableFlag() == true ){
				GrenadeIndex[i].SetEnableFlag(false);
			}
		}
	}


	//������E������
	for(int i=0; i<MAX_HUMAN; i++){
		//������E�������Ŗ��t���[����������ŏ�������Ӗ��͂Ȃ��̂ŁA�e����2�t���[����1�񏈗��ɃP�`���Ă܂��@�i�O�O�G
		//�t���[�����������ڂ̎��́A�f�[�^�ԍ��������̕���B��̎��́A��̕���������B
		for(int j=(framecnt%2); j<MAX_WEAPON; j+=2){
			PickupWeapon(&HumanIndex[i], &WeaponIndex[j]);
		}
	}

	for(int i=0; i<MAX_HUMAN; i++){
		for(int j=i+1; j<MAX_HUMAN; j++){
			//�l���m�̓����蔻��i���������j
			CollideHuman(&HumanIndex[i], &HumanIndex[j]);
		}
	}

	framecnt += 1;

	return 0;
}

//! @brief ���t���[���� �������E�|�����G�̐��E�w�b�g�V���b�g�� �̎擾
//! @param id �擾����l�̃f�[�^�ԍ�
//! @param ontarget ���������󂯎��|�C���^
//! @param kill �|�����G�̐����󂯎��|�C���^
//! @param headshot �G�̓����ɖ������������󂯎��|�C���^
//! @return �����Ftrue�@���s�Ffalse
bool ObjectManager::GetHumanShotInfo(int id, int *ontarget, int *kill, int *headshot)
{
	if( (id < 0)||(MAX_HUMAN-1 < id) ){ return false; }
	*ontarget = Human_ontarget[id];
	*kill = Human_kill[id];
	*headshot = Human_headshot[id];
	return true;
}

//! @brief �G�t�F�N�g���\�[�g����
//! @param camera_x �J������X���W
//! @param camera_y �J������Y���W
//! @param camera_z �J������Z���W
//! @param data ���ʂ��󂯎�� effectdata�\���́@�i�v�f���FMAX_EFFECT�j
//! @return �L���ȃG�t�F�N�g�̐�
int ObjectManager::SortEffect(float camera_x, float camera_y, float camera_z, effectdata data[])
{
	int cnt = 0;

	//�J��������̋��������߂A�����𐔂���B
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetEnableFlag() == true ){
			float ex, ey, ez;
			float x, y, z;

			//���W���擾���A�������v�Z�A�f�[�^�ɓo�^�B
			EffectIndex[i].GetPosData(&ex, &ey, &ez, NULL);
			x = ex - camera_x;
			y = ey - camera_y;
			z = ez - camera_z;
			data[cnt].id = i;
			data[cnt].dist = x*x + y*y + z*z;

			//�J�E���g�����Z
			cnt += 1;
		}
	}

	//�P���}���\�[�g
	effectdata temp;
	for(int i=1; i<cnt; i++) {
		int j;
		temp = data[i];
		for(j=i; j>0 && data[j-1].dist<temp.dist; j--){
			data[j] = data[j-1];
		}
		data[j] = temp;
	}

	return cnt;
}

//! @brief �I�u�W�F�N�g�̕`�揈��
//! @param camera_x �J������X���W
//! @param camera_y �J������Y���W
//! @param camera_z �J������Z���W
//! @param HidePlayer �v���C���[�̔�\���ݒ�@�i�\���F0�@��\���F1�@�r�ƕ���̂ݕ\���F2�j
void ObjectManager::Render(float camera_x, float camera_y, float camera_z, int HidePlayer)
{
	//���[���h���W�����_��
	d3dg->ResetWorldTransform();

	//�l�`��
	for(int i=0; i<MAX_HUMAN; i++){
		bool DrawArm, player;

		//�r�̕\��
		if( HidePlayer == 0 ){
			DrawArm = false;
		}
		else if( Player_HumanID != i ){	// HidePlayer != 0
			DrawArm = false;
		}
		else if( HidePlayer == 2 ){		// Player_HumanID == i
			DrawArm = true;
		}
		else{							// Player_HumanID == i && HidePlayer == 1
			continue;	//�v���C���[���g��\�����Ȃ��Ȃ�X�L�b�v
		}

		//�v���C���[���ǂ�������
		if( Player_HumanID == i ){
			player = true;
		}
		else{
			player = false;
		}

		HumanIndex[i].Render(d3dg, Resource, DrawArm, player);

		/*
		//�����蔻��̊ȈՕ\��
		float x, y, z;
		HumanIndex[i].GetPosData(&x, &y, &z, NULL);
		if( HumanIndex[i].GetHP() > 0 ){
			d3dg->SetWorldTransform(x, y, z, 0.0f, 0.0f, 1.0f);
			d3dg->Drawline(HUMAN_BULLETCOLLISION_HEAD_R, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H + HUMAN_BULLETCOLLISION_HEAD_H, 0.0f, HUMAN_BULLETCOLLISION_HEAD_R * -1, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H + HUMAN_BULLETCOLLISION_HEAD_H, 0.0f);
			d3dg->Drawline(0.0f, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H + HUMAN_BULLETCOLLISION_HEAD_H, HUMAN_BULLETCOLLISION_HEAD_R, 0.0f, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H + HUMAN_BULLETCOLLISION_HEAD_H, HUMAN_BULLETCOLLISION_HEAD_R * -1);
			d3dg->Drawline(HUMAN_BULLETCOLLISION_UP_R, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, 0.0f, HUMAN_BULLETCOLLISION_UP_R * -1, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, 0.0f);
			d3dg->Drawline(0.0f, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, HUMAN_BULLETCOLLISION_UP_R, 0.0f, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, HUMAN_BULLETCOLLISION_UP_R * -1);
			d3dg->Drawline(HUMAN_BULLETCOLLISION_LEG_R, HUMAN_BULLETCOLLISION_LEG_H, 0.0f, HUMAN_BULLETCOLLISION_LEG_R * -1, HUMAN_BULLETCOLLISION_LEG_H, 0.0f);
			d3dg->Drawline(0.0f, HUMAN_BULLETCOLLISION_LEG_H, HUMAN_BULLETCOLLISION_LEG_R, 0.0f, HUMAN_BULLETCOLLISION_LEG_H, HUMAN_BULLETCOLLISION_LEG_R * -1);
		}
		*/
	}

	//����`��
	for(int i=0; i<MAX_WEAPON; i++){
		WeaponIndex[i].Render(d3dg);
	}

	//�����`��
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		SmallObjectIndex[i].Render(d3dg);
	}

	//�e�`��
	for(int i=0; i<MAX_BULLET; i++){
		BulletIndex[i].Render(d3dg);
	}

	//��֒e�`��
	for(int i=0; i<MAX_GRENADE; i++){
		GrenadeIndex[i].Render(d3dg);
	}

	//�G�t�F�N�g�`��
	/*
	for(int i=0; i<MAX_EFFECT; i++){
		EffectIndex[i].Render(d3dg);
	}
	*/
	effectdata data[MAX_EFFECT];
	int cnt = SortEffect(camera_x, camera_y, camera_z, data);
	for(int i=0; i<cnt; i++) {
		EffectIndex[ data[i].id ].Render(d3dg);
	}
}

//! @brief �f�[�^�̉��
void ObjectManager::Cleanup()
{
	//�|�C���g�f�[�^���
	CleanupPointDataToObject();
}

//! @brief �R���X�g���N�^
BulletObjectHumanIndex::BulletObjectHumanIndex()
{
	HumanIndex = new bool[MAX_HUMAN];

	Init();
}

//! @brief �f�B�X�g���N�^
BulletObjectHumanIndex::~BulletObjectHumanIndex()
{
	if( HumanIndex == NULL ){ delete [] HumanIndex; }
}

//! @brief ������
void BulletObjectHumanIndex::Init()
{
	for(int i=0; i<MAX_HUMAN; i++){
		HumanIndex[i] = false;
	}
}

//! @brief �t���O���擾
//! @param id �l�̃f�[�^�ԍ�
//! @return �t���O�l
bool BulletObjectHumanIndex::GetIndexFlag(int id)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return false; }
	return HumanIndex[id];
}

//! @brief �t���O��L����
//! @param id �l�̃f�[�^�ԍ�
void BulletObjectHumanIndex::SetIndexFlag(int id)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return; }
	HumanIndex[id] = true;
}