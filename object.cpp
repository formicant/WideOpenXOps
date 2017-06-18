//! @file object.cpp
//! @brief object�N���X�̒�`

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

#include "object.h"

//! @brief �R���X�g���N�^
object::object(class ParameterInfo *in_Param, float x, float y, float z, float rx, float size, bool flag)
{
	Param = in_Param;
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;
	EnableFlag = flag;
	model_size = size;

	id_parameter = 0;
	id_model = -1;
	id_texture = -1;
	DarkModelFlag = false;
}

//! @brief �f�B�X�g���N�^
object::~object()
{}

//! @brief �ݒ�l���Ǘ�����N���X��o�^
//! @attention �e�֐����g�p����O�Ɏ��s���邱�ƁB
void object::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;
}

//! @brief ���W�Ɗp�x��ݒ�
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param rx ������]
void object::SetPosData(float x, float y, float z, float rx)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;
}

//! @brief ���W�Ɗp�x���擾
//! @param x X���W���󂯎��|�C���^�iNULL�j
//! @param y Y���W���󂯎��|�C���^�iNULL�j
//! @param z Z���W���󂯎��|�C���^�iNULL�j
//! @param rx ������]���󂯎��|�C���^�iNULL�j
void object::GetPosData(float *x, float *y, float *z, float *rx)
{
	if( x != NULL ){ *x = pos_x; }
	if( y != NULL ){ *y = pos_y; }
	if( z != NULL ){ *z = pos_z; }
	if( rx != NULL ){ *rx = rotation_x; }
}

//! @brief �L�����t���O��ݒ�
//! @param flag �ݒ肷��t���O
void object::SetEnableFlag(bool flag)
{
	EnableFlag = flag;
}

//! @brief �L�����t���O���擾
//! @return ���ݐݒ肳��Ă���t���O
bool object::GetEnableFlag()
{
	return EnableFlag;
}

//! @brief ���f���f�[�^��ݒ�
//! @param id ���f���F���ԍ�
//! @param size �`��{��
void object::SetModel(int id, float size)
{
	id_model = id;
	model_size = size;
}

//! @brief ���f���f�[�^���擾
//! @param id ���f���F���ԍ����󂯎��|�C���^�iNULL�j
//! @param size �`��{�����󂯎��|�C���^�iNULL�j
void object::GetModel(int *id, float *size)
{
	if( id != NULL ){ *id = id_model; }
	if( size != NULL ){ *size = model_size; }
}

//! @brief �e�N�X�`����ݒ�
//! @param id �e�N�X�`���F���ԍ�
void object::SetTexture(int id)
{
	id_texture = id;
}

//! @brief �e�N�X�`�����擾
//! @return �e�N�X�`���F���ԍ�
int object::GetTexture()
{
	return id_texture;
}

//! @brief ���f�����Â�����t���O��ݒ�
//! @param flag �t���O
void object::SetDarkModelFlag(bool flag)
{
	DarkModelFlag = flag;
}

//! @brief �v�Z�����s�i���R�����Ȃǁj
int object::RunFrame()
{
	return 0;
}

//! @brief �`��
//! @param d3dg D3DGraphics�̃|�C���^
void object::Render(D3DGraphics *d3dg)
{
	if( d3dg == NULL ){ return; }
	if( EnableFlag == false ){ return; }

	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, rotation_x, 0.0f, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag);
}

//! @brief �R���X�g���N�^
human::human(class ParameterInfo *in_Param, float x, float y, float z, float rx, int id_param, int dataid, signed char p4, int team, bool flag)
{
	//HumanParameter data;

	Param = in_Param;
	pos_x = x;
	pos_y = y;
	pos_z = z;
	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	move_y_flag = false;
	rotation_x = rx;
	id_parameter = id_param;
	upmodel_size = 9.4f;
	armmodel_size = 9.0f;
	legmodel_size = 9.0f;
	EnableFlag = flag;
	rotation_y = 0.0f;
	armrotation_y = 0.0f;
	point_dataid = dataid;
	point_p4 = p4;
	teamid = team;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}
	selectweapon = 1;
	//if( Param->GetHuman(id_param, &data) == 0 ){
	//	hp = data.hp;
	//}
	//else{
		hp = 0;
	//}
#ifdef HUMAN_DEADBODY_COLLISION
	deadstate = 0;
#endif
	add_ry = 0.0f;
	id_texture = -1;

	move_rx = 0.0f;
	MoveFlag = 0x00;
	MoveFlag_lt = MoveFlag;
	scopemode = 0;
	HitFlag = false;
	Hit_rx = 0.0f;
	totalmove = 0.0f;
	StateGunsightErrorRange = 0;
	ReactionGunsightErrorRange = 0;

	MotionCtrl = new HumanMotionControl;
}

//! @brief �f�B�X�g���N�^
human::~human()
{
	delete MotionCtrl;
}

//! @brief �ݒ�l���Ǘ�����N���X��o�^
//! @attention �e�֐����g�p����O�Ɏ��s���邱�ƁB
void human::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;

	MotionCtrl->SetParameterInfoClass(in_Param);
}

//! @brief �ݒ�l��ݒ�
//! @param id_param �l�̎�ޔԍ�
//! @param dataid �|�C���g�̃f�[�^�ԍ�
//! @param p4 ��4�p�����[�^
//! @param team �`�[���ԍ�
//! @param init �I�u�W�F�N�g��������
//! @warning init������true�ɂ��ď���������ꍇ�A���SetPosData()�֐��Ŋp�x��ݒ肵�Ă������ƁB
void human::SetParamData(int id_param, int dataid, signed char p4, int team, bool init)
{
	id_parameter = id_param;
	point_dataid = dataid;
	point_p4 = p4;
	teamid = team;

	if( init == true ){
		HumanParameter data;

		move_x = 0.0f;
		move_y = 0.0f;
		move_z = 0.0f;
		move_y_flag = false;
		rotation_y = 0.0f;
		armrotation_y = DegreeToRadian(-30);

		for(int i=0; i<TOTAL_HAVEWEAPON; i++){
			weapon[i] = NULL;
		}
		selectweapon = 1;
		if( Param->GetHuman(id_param, &data) == 0 ){
			hp = data.hp;
		}
		else{
			hp = 0;
		}
#ifdef HUMAN_DEADBODY_COLLISION
		deadstate = 0;
#endif
		add_ry = 0.0f;
		MoveFlag = 0x00;
		MoveFlag_lt = MoveFlag;
		scopemode = 0;
		HitFlag = false;
		Hit_rx = 0.0f;
		totalmove = 0.0f;
		Invincible = false;

		MotionCtrl->Init(rotation_x);
	}
}

//! @brief �ݒ�l���擾
//! @param id_param �l�̎�ޔԍ����󂯎��|�C���^�iNULL�j
//! @param dataid �|�C���g�̃f�[�^�ԍ����󂯎��|�C���^�iNULL�j
//! @param p4 ��4�p�����[�^���󂯎��|�C���^�iNULL�j
//! @param team �`�[���ԍ����󂯎��|�C���^�iNULL�j
void human::GetParamData(int *id_param, int *dataid, signed char *p4, int *team)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( dataid != NULL ){ *dataid = point_dataid; }
	if( p4 != NULL ){ *p4 = point_p4; }
	if( team != NULL ){ *team = teamid; }
}

//! @brief HP���擾
//! @return HP
int human::GetHP()
{
	return hp;
}

//! @brief HP��ݒ�
//! @param in_hp �V���ɐݒ肷��HP
//! @return �����Ftrue�@���s�Ffalse
//! @attention HP��1�ȏ�̐l�ɑ΂��Ď��s���Ȃ��Ǝ��s���܂��B
bool human::SetHP(int in_hp)
{
	if( hp > 0 ){
		hp = in_hp;
		return true;
	}
	return false;
}


//! @brief ���̂��ǂ�������
//! @return ���́Ftrue�@���̂łȂ��Ffalse
//! @warning ���S�ɐÎ~������Ԃ��u���́v�Ɣ��肵�܂��B�|��Ă���Œ��̐l�͑ΏۂɊ܂܂�Ȃ����߁Ahp <= 0 ���S�Ď��̂Ɣ��肳���Ƃ͌���܂���B
bool human::GetDeadFlag()
{
#ifdef HUMAN_DEADBODY_COLLISION
	if( deadstate == 5 ){ return true; }
	return false;
#else
	if( hp <= 0 ){ return true; }
	return false;
#endif
}

//! @brief �`�[���ԍ���ݒ�i�㏑���j
//! @param id �V�����`�[���ԍ�
void human::SetTeamID(int id)
{
	teamid = id;
}

//! @brief ���G�t���O���擾
//! @return true�F���G�@false�F�ʏ�
//! @attention ���G��Ԃ̏ꍇ�A�e�e�E��֒e�̔����E�����@�ɂ��_���[�W����؎󂯂܂���B
bool human::GetInvincibleFlag()
{
	return Invincible;
}

//! @brief ���G�t���O��ݒ�
//! @param flag true�F���G�@false�F�ʏ�
//! @attention ���G��Ԃ̏ꍇ�A�e�e�E��֒e�̔����E�����@�ɂ��_���[�W����؎󂯂܂���B
void human::SetInvincibleFlag(bool flag)
{
	Invincible = flag;
}

//! @brief �O�����̈ړ��ʂ��擾
//! @param *x X���ړ��ʂ��擾����|�C���^�iNULL�j
//! @param *y Y���ړ��ʂ��擾����|�C���^�iNULL�j
//! @param *z Z���ړ��ʂ��擾����|�C���^�iNULL�j
void human::GetMovePos(float *x, float *y, float *z)
{
	if( x != NULL ){ *x = move_x; }
	if( y != NULL ){ *y = move_y; }
	if( z != NULL ){ *z = move_z; }
}

//! @brief ���f���f�[�^��ݒ�
//! @param upmodel �㔼�g�̃��f��
//! @param armmodel[] �r�̃��f���̔z��i�z�񐔁FTOTAL_ARMMODE�j
//! @param legmodel ���i�Î~��ԁj�̃��f��
//! @param walkmodel[] �r�̃��f���̔z��i�z�񐔁FTOTAL_WALKMODE�j
//! @param runmodel[] �r�̃��f���̔z��i�z�񐔁FTOTAL_RUNMODE�j
void human::SetModel(int upmodel, int armmodel[], int legmodel, int walkmodel[], int runmodel[])
{
	MotionCtrl->SetModel(upmodel, armmodel, legmodel, walkmodel, runmodel);
}

//! @brief �����ݒ�
//! @param in_weapon[] �ݒ肷��weapon�N���X�̃|�C���^�z��
//! @warning �ʏ�� PickupWeapon()�֐� ���g�p���邱��
void human::SetWeapon(class weapon *in_weapon[])
{
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		if( in_weapon[i] == NULL ){
			weapon[i] = NULL;
		}
		else{
			//����𐳂����E����΁A��������Ƃ��ēo�^
			if( in_weapon[i]->Pickup() == 0 ){
				weapon[i] = in_weapon[i];
			}
		}
	}
}

//! @brief ������E��
//! @param in_weapon[] �ݒ肷��weapon�N���X�̃|�C���^
//! @return �����F1�@���s�F0
//! @attention �l�̎�ނ� �]���r �̏ꍇ�A���̊֐��͎��s���܂��B
int human::PickupWeapon(class weapon *in_weapon)
{
	if( in_weapon == NULL ){
		return 0;
	}

	//�l�̎�ނ� �]���r �Ȃ�΁A���s
	HumanParameter Paraminfo;
	Param->GetHuman(id_parameter, &Paraminfo);
	if( Paraminfo.type == 2 ){
		return 0;
	}

	if( weapon[selectweapon] == NULL ){
		//����𐳂����E����΁A��������Ƃ��ēo�^
		if( in_weapon->Pickup() == 0 ){
			weapon[selectweapon] = in_weapon;

			//�r�̊p�x�i�����j��ݒ�
			class weapon *nowweapon;
			int id_param = 0;
			nowweapon = weapon[selectweapon];
			nowweapon->GetParamData(&id_param, NULL, NULL);
			MotionCtrl->PickupWeapon(id_param);

			//�؂�ւ������̃J�E���g
			selectweaponcnt = 10;

			return 1;
		}
	}
	return 0;
}

//! @brief �����؂�ւ���i�����ւ���j
//! @param id �����ւ��镐��@�i-1 �Ŏ��̕���j
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
void human::ChangeWeapon(int id)
{
	//�̗͂��Ȃ���Ύ��s
	if( hp <= 0 ){ return; }

	//�����[�h���Ȃ玸�s
	if( weapon[selectweapon] != NULL ){
		if( weapon[selectweapon]->GetReloadCnt() > 0 ){ return; }
	}

	//��������ɐ؂�ւ��悤�Ƃ��Ă���Ȃ�A���s
	if( selectweapon == id ){ return; }

	//����؂�ւ����Ȃ玸�s
	if( selectweaponcnt > 0 ){ return; }

	if( id == -1 ){
		//���̕���ԍ���I��
		selectweapon += 1;
		if( selectweapon == TOTAL_HAVEWEAPON ){
			selectweapon = 0;
		}
	}
	else{
		//����ԍ����͈͓����m�F
		if( (id < 0)||((TOTAL_HAVEWEAPON -1) < id ) ){ return; }
		selectweapon = id;
	}

	//�X�R�[�v�\��������
	SetDisableScope();

	//���ݑ������镐��̃N���X���擾
	class weapon *nowweapon;
	nowweapon = weapon[selectweapon];

	//�r�̊p�x�i�����j��ݒ�
	if( nowweapon == NULL ){	//��Ԃ�
		MotionCtrl->ChangeWeapon(ID_WEAPON_NONE);
	}
	else{
		int id_param = 0;
		nowweapon->GetParamData(&id_param, NULL, NULL);
		MotionCtrl->ChangeWeapon(id_param);
	}

	//�؂�ւ������̃J�E���g
	selectweaponcnt = 10;
}

//! @brief ����̐؂�ւ��J�E���g���擾
//! @return �J�E���g���@�i1�ȏ�Ő؂�ւ����j
int human::GetChangeWeaponCnt()
{
	return selectweaponcnt;
}

//! @brief ������擾
//! @param out_selectweapon �I������Ă��镐��@�i0 �` [TOTAL_HAVEWEAPON]-1�j
//! @param out_weapon �󂯎��weapon�N���X�̃|�C���^�z��@�i�z�񐔁FTOTAL_HAVEWEAPON�j
void human::GetWeapon(int *out_selectweapon, class weapon *out_weapon[])
{
	*out_selectweapon = selectweapon;
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		out_weapon[i] = weapon[i];
	}
}

//! @brief ���ݑ������Ă��镐��̎�ޔԍ����擾
//! @return ����̎�ޔԍ��i0 �` TOTAL_PARAMETERINFO_WEAPON -1�j
//! @attention ���ݎ�Ɏ����Ă��镐��̎�ޔԍ��ł��B�@GetWeapon()�֐� ��p���Ē��ׂ�̂Ɠ����ł��B
int human::GetMainWeaponTypeNO()
{
	if( weapon[selectweapon] == NULL ){
		return ID_WEAPON_NONE;
	}

	int id_param;
	weapon[selectweapon]->GetParamData(&id_param, NULL, NULL);
	return id_param;
}

//! @brief �A�ːݒ���擾
//! @return �A�ˉ\�Ftrue�@�A�˕s�Ffalse
bool human::GetWeaponBlazingmode()
{
	int param_id;
	WeaponParameter data;

	//����𑕔����Ă��Ȃ���΁A���s
	if( weapon[selectweapon] == NULL ){ return false; }

	//�������Ă��镐��̎�ޔԍ����擾
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	//�A�ːݒ��Ԃ�
	Param->GetWeapon(param_id, &data);
	return data.blazingmode;
}

//! @brief ���C����
//! @param weapon_paramid ���C��������̔ԍ����󂯎��|�C���^
//! @param GunsightErrorRange ���C�����ۂ̏Ə��덷���󂯎��|�C���^
//! @return �����F1�@���s�F0
//! @attention �e�I�u�W�F�N�g�̏�����A���C���̍Đ��͕ʓr�s���K�v������܂��B
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
bool human::ShotWeapon(int *weapon_paramid, int *GunsightErrorRange)
{
	int param_id;

	//����؂�ւ����Ȃ玸�s
	if( selectweaponcnt > 0 ){ return false; }

	//����𑕔����Ă��Ȃ���΁A���s
	if( weapon[selectweapon] == NULL ){ return false; }

	//�e�̔��ˏ������s��
	if( weapon[selectweapon]->Shot() != 0 ){ return false; }

	//����̎�ޔԍ����擾
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);


	//����̐ݒ�l���擾
	WeaponParameter ParamData;
	if( Param->GetWeapon(param_id, &ParamData) != 0 ){ return false; }

	//�����X�R�[�v�̕���ŃX�R�[�v��`���Ă��Ȃ���΁A�덷 20�B
	if( (scopemode == 0)&&(ParamData.scopemode == 2) ){
		ReactionGunsightErrorRange = 20;
	}


	//����̎�ޔԍ��ƌ덷��Ԃ�
	*weapon_paramid = param_id;
	*GunsightErrorRange = StateGunsightErrorRange + ReactionGunsightErrorRange;


	//�����X�R�[�v�ȊO�̕�����A�X�R�[�v���g�킸�Ɏg���Ă�����A�ݒ肳�ꂽ�덷�����Z�B
	if( (ParamData.scopemode != 2)&&(scopemode == 0) ){
		ReactionGunsightErrorRange += ParamData.reaction;
	}

	//�X�R�[�v���g�p���Ă����Ԃ̔��������Z
	if( (ParamData.scopemode == 1)&&(scopemode != 0) ){
		armrotation_y += DegreeToRadian(1) * (WEAPONERRORRANGE_SCALE * ParamData.reaction);
	}
	if( ParamData.scopemode == 2 ){
		armrotation_y += DegreeToRadian(1) * (WEAPONERRORRANGE_SCALE * ParamData.reaction);
	}

	//�r�ɔ�����`����
	MotionCtrl->ShotWeapon(param_id);

	//���킪�����Ȃ��Ă���΁A��������O���������ɁB�@�i��֒e�p�j
	if( weapon[selectweapon]->GetEnableFlag() == false ){
		weapon[selectweapon] = NULL;
	}
	return true;
}

//! @brief �����[�h
//! @return �����Ftrue�@���s�Ffalse
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
bool human::ReloadWeapon()
{
	//����؂�ւ����Ȃ玸�s
	if( selectweaponcnt > 0 ){ return false; }

	//��������̕���𑕔����Ă���΁`
	if( weapon[selectweapon] != NULL ){
		//�����[�h���Ȃ玸�s
		if( weapon[selectweapon]->GetReloadCnt() > 0 ){ return false; }

		//�����[�h�������J�n
		if( weapon[selectweapon]->StartReload() != 0 ){ return false; }

		//�X�R�[�v���[�h������
		SetDisableScope();

		//���[�V�������s
		class weapon *nowweapon;
		int id_param = 0;
		nowweapon = weapon[selectweapon];
		nowweapon->GetParamData(&id_param, NULL, NULL);
		MotionCtrl->ReloadWeapon(id_param);

		return true;
	}
	return false;
}

//! @brief ������̂Ă�
//! @return �����Ftrue�@���s�Ffalse
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
bool human::DumpWeapon()
{
	//����؂�ւ����Ȃ玸�s
	if( selectweaponcnt > 0 ){ return false; }

	//��������̕���𑕔����Ă���΁`
	if( weapon[selectweapon] != NULL ){
		//�����[�h���Ȃ玸�s
		if( weapon[selectweapon]->GetReloadCnt() > 0 ){ return false; }

		//������̂āA����������
		weapon[selectweapon]->Dropoff(pos_x, pos_y, pos_z, rotation_x, 1.63f);
		weapon[selectweapon] = NULL;

		//�X�R�[�v���[�h������
		SetDisableScope();

		//���[�V�������s
		MotionCtrl->DumpWeapon();

		return true;
	}

	return false;
}

//! @brief ����̃V���b�g���[�h�؂�ւ�
//! @return �����F0�@���s�F1
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
int human::ChangeShotMode()
{
	//����𑕔����ĂȂ���Ύ��s
	if( weapon[selectweapon] == NULL ){ return 1; }

	//�������Ă��镐��̏����擾
	int param_id, lnbs, nbs;
	weapon[selectweapon]->GetParamData(&param_id, &lnbs, &nbs);

	//����̃V���b�g���[�h�؂�ւ���i�V��������ԍ��j�𒲂ׂ�
	WeaponParameter ParamData;
	int ChangeWeapon;
	if( Param->GetWeapon(param_id, &ParamData) != 0 ){ return 1; }
	ChangeWeapon = ParamData.ChangeWeapon;

	//�V��������ԍ������������m�F
	if( ChangeWeapon == param_id ){ return 1; }
	if( (ChangeWeapon < 0)||(TOTAL_PARAMETERINFO_WEAPON-1 < ChangeWeapon) ){ return 1; }

	//�ݒ��K�p
	weapon[selectweapon]->SetParamData(ChangeWeapon, lnbs, nbs, false);

	//���[�V�������s
	MotionCtrl->ChangeShotMode(ChangeWeapon);

	return 0;
}

//! @brief �O�i�i����j��ݒ�
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
void human::SetMoveForward()
{
	SetFlag(MoveFlag, MOVEFLAG_FORWARD);
}

//! @brief ��ނ�ݒ�
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
void human::SetMoveBack()
{
	SetFlag(MoveFlag, MOVEFLAG_BACK);
}

//! @brief �������ݒ�
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
void human::SetMoveLeft()
{
	SetFlag(MoveFlag, MOVEFLAG_LEFT);
}

//! @brief �E�����ݒ�
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
void human::SetMoveRight()
{
	SetFlag(MoveFlag, MOVEFLAG_RIGHT);
}

//! @brief ������ݒ�
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
void human::SetMoveWalk()
{
	SetFlag(MoveFlag, MOVEFLAG_WALK);
}

//! @brief �l�̈ړ����[�h���擾
//! @param nowdata ���݂̒l���擾�Ftrue�@�O�t���[���̒l���g�p�Ffalse
//! @return �����F1�@����F2�@�ړ����ĂȂ��F0
int human::GetMovemode(bool nowdata)
{
	//�g�p����Ă��Ȃ����A��������Ă��Ȃ���ΏI��
	if( EnableFlag == false ){ return 0; }
	if( hp <= 0 ){ return 0; }

	if( nowdata == false ){	//�O�̃f�[�^
		//�����Ȃ�� 1
		if( GetFlag(MoveFlag_lt, MOVEFLAG_WALK) ){
			return 1;
		}
		//����Ȃ�� 2
		if( GetFlag(MoveFlag_lt, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
			return 2;
		}
	}
	else{					//���݂̃f�[�^
		//�����Ȃ�� 1
		if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
			return 1;
		}
		//����Ȃ�� 2
		if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
			return 2;
		}
	}

	//�ړ����ĂȂ�
	return 0;
}

//! @brief �X�R�[�v��ݒ�
//! @return �����Ftrue�@���s�Ffalse
bool human::SetEnableScope()
{
	int param_id;
	WeaponParameter data;

	//��������𑕔����ĂȂ���Ύ��s
	if( weapon[selectweapon] == NULL ){ return false; }

	//����̎�ޔԍ����擾
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	//����̐ݒ�l���擾
	Param->GetWeapon(param_id, &data);

	//�X�R�[�v��ݒ�
	scopemode = data.scopemode;
	return true;
}

//! @brief �X�R�[�v������
void human::SetDisableScope()
{
	scopemode = 0;
}

//! @brief �X�R�[�v�ݒ���擾
int human::GetScopeMode()
{
	return scopemode;
}

//! @brief �����Əc���̌������擾
//! @param rx �������擾����|�C���^
//! @param ry �c�����擾����|�C���^
void human::GetRxRy(float *rx, float *ry)
{
	*rx = rotation_x;
	*ry = armrotation_y;
}

//! @brief �����Əc���̌�����ݒ�
//! @param rx �ݒ肷�鉡��
//! @param ry �ݒ肷��c��
void human::SetRxRy(float rx, float ry)
{
	rotation_x = rx;
	armrotation_y = ry;
}

//! @brief �S�̂̉�]�p�x�擾
//! @return �c�����擾����|�C���^
//! @warning ���S���ē|���ۂ̊p�x�ł��BGetRxRy()�֐��Ŏ󂯎��l�Ƃ͈قȂ�܂��B
float human::GetDeadRy()
{
	return rotation_y;
}

//! @brief �W�����v
//! @return �����F0�@���s�F1
//! @attention �Q�[���ォ�璼�ڌĂяo�����Ƃ͔����AObjectManager�N���X����Ăяo���Ă��������B
int human::Jump()
{
	//�n�ʂɐG��Ă���A�������x��0.0�Ȃ��
	if( move_y_flag == false ){
		if( move_y == 0.0f ){
			move_y = HUMAN_JUMP_SPEED;

			//���[�V�������s
			MotionCtrl->Jump();
			return 0;
		}
	}
	return 1;
}

//! @brief ���������E�͂�������
//! @param rx ����
//! @param ry �c��
//! @param speed ���x
void human::AddPosOrder(float rx, float ry, float speed)
{
	move_x += cos(rx) * cos(ry) * speed;
	move_y += sin(ry) * speed;
	move_z += sin(rx) * cos(ry) * speed;
}

//! @brief �e�� �� �Ƀq�b�g
//! @param attacks �e�̍U����
void human::HitBulletHead(int attacks)
{
	if( Invincible == false ){
		hp -= (int)((float)attacks * HUMAN_DAMAGE_HEAD) + GetRand(16);
	}
	ReactionGunsightErrorRange = 15;
}

//! @brief �e�� �㔼�g �Ƀq�b�g
//! @param attacks �e�̍U����
void human::HitBulletUp(int attacks)
{
	if( Invincible == false ){
		hp -= (int)((float)attacks * HUMAN_DAMAGE_UP) + GetRand(8);
	}
	ReactionGunsightErrorRange = 12;
}

//! @brief �e�� �����g �Ƀq�b�g
//! @param attacks �e�̍U����
void human::HitBulletLeg(int attacks)
{
	if( Invincible == false ){
		hp -= (int)((float)attacks * HUMAN_DAMAGE_LEG) + GetRand(6);
	}
	ReactionGunsightErrorRange = 8;
}

//! @brief �]���r�̍U�����q�b�g
void human::HitZombieAttack()
{
	if( Invincible == false ){
		hp -= HUMAN_DAMAGE_ZOMBIEU + GetRand(HUMAN_DAMAGE_ZOMBIEA);
	}
	ReactionGunsightErrorRange = 10;
}

//! @brief ��֒e�̔������q�b�g
//! @param attacks �����̍U����
//! @attention �����ɂ��v�Z�����O�ɍς܂��Ă��������B
void human::HitGrenadeExplosion(int attacks)
{
	if( Invincible == false ){
		hp -= attacks;
	}
	ReactionGunsightErrorRange = 10;
}

//! @brief ��e�t���O���Z�b�g
//! @param rx ��e��������
void human::SetHitFlag(float rx)
{
	HitFlag = true;
	Hit_rx = rx;
}

//! @brief ��e�������`�F�b�N����
//! @param rx ��e�����������󂯎��|�C���^�iNULL�j
//! @return ��e�����Ftrue�@��e���ĂȂ��Ffalse
//! @attention ���s����ƁA�t���O�� false �ɏ���������܂��B
bool human::CheckHit(float *rx)
{
	bool returnflag = HitFlag;
	if( rx != NULL ){ *rx = Hit_rx; }
	HitFlag = false;
	//Hit_rx = 0.0f;
	return returnflag;
}

//! @brief ���v�ړ��ʂ��擾
//! @return ���v�ړ���
float human::GetTotalMove()
{
	return totalmove;
}

//! @brief �Ə��̏�Ԍ덷�̏���
//!	@attention ControlProcess()���O�ŏ������邱��
void human::GunsightErrorRange()
{
	//������
	StateGunsightErrorRange = 0;

	//�e����ɂ��덷��ݒ�
	if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
		StateGunsightErrorRange = 4;
	}
	if( GetFlag(MoveFlag, MOVEFLAG_FORWARD) ){
		StateGunsightErrorRange = 8;
	}
	if( GetFlag(MoveFlag, MOVEFLAG_BACK) ){
		StateGunsightErrorRange = 6;
	}
	if( GetFlag(MoveFlag, (MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
		StateGunsightErrorRange = 7;
	}
	if( move_y_flag == true ){
		StateGunsightErrorRange = 22;
	}
	if( hp < 40 ){
		StateGunsightErrorRange += 3;
	}

	//��������𑕔����Ă����
	if( weapon[selectweapon] != NULL ){
		//����̐ݒ�f�[�^���擾
		int param;
		WeaponParameter data;
		weapon[selectweapon]->GetParamData(&param, NULL, NULL);
		Param->GetWeapon(param, &data);

		//�덷�� 1 ���炷
		ReactionGunsightErrorRange -= 1;

		//�덷�͈̔͂�␳
		if( ReactionGunsightErrorRange < 0 ){ ReactionGunsightErrorRange = 0; }
		if( ReactionGunsightErrorRange > data.ErrorRangeMAX ){ ReactionGunsightErrorRange = data.ErrorRangeMAX; }
	}
	else{
		ReactionGunsightErrorRange = 0;
	}
}

//! @brief ���S����Ɠ|��鏈��
//! @return �Î~�������́F4�@�|��I���������F3�@�|��Ă���Œ��F2�@�|��n�߂�F1�@�������Ȃ��F0
int human::CheckAndProcessDead(class Collision *CollD)
{
#ifdef HUMAN_DEADBODY_COLLISION

	//�����F
	//
	//��ԁF0
	//�m�������Ȃ��n
	//�@HP��0�ȉ��ŁA�����t���ʂȂ�@��ԁF1
	//�@HP��0�ȉ��ŁA�ǂɓ����t���Ȃ�@��ԁF4
	//
	//��ԁF1
	//�m�X���n�߂�n
	//�@135�x�s������@��ԁF2
	//�@�ǂɓ���ł�����@��ԁF3
	//
	//��ԁF2
	//�m�����n
	//�@�����n�ʂɕt������@��ԁF4
	//
	//��ԁF3
	//�m�������点��n
	//�@�@
	//��ԁF4
	//�m�Œ�n

	float check_posx, check_posy, check_posz;

	if( deadstate == 0 ){
		if( hp <= 0 ){		//HP�� 0 �ȉ��ɂȂ����i���S�����j�u�ԂȂ�A�|���n�߂�
			float tr;

			//�Ō�ɍU�����󂯂��������v�Z
			tr = Hit_rx - rotation_x;
			for(; tr > (float)M_PI; tr -= (float)M_PI*2){}
			for(; tr < (float)M_PI*-1; tr += (float)M_PI*2){}

			//�̂̊p�x
			if( ((float)M_PI/2*-1 < tr)&&(tr < (float)M_PI/2) ){
				add_ry = HUMAN_DEADADDRY;
			}
			else{
				add_ry = HUMAN_DEADADDRY * -1;
			}

			//�r�̊p�x
			if( armrotation_y < 0.0f ){
				armrotation_y = DegreeToRadian(-90);
			}
			else{
				armrotation_y = DegreeToRadian(90);
			}

			//���̂����܂�ʂ悤�A������ +1.0 ����
			pos_y += 1.0f;

			//�������Ă��镐���S�Ď̂Ă�
			for(int i=0; i<TOTAL_HAVEWEAPON; i++){
				if( weapon[i] != NULL ){
					weapon[i]->Dropoff(pos_x, pos_y, pos_z, DegreeToRadian(10)*GetRand(36), 1.5f);
					weapon[i] = NULL;
				}
			}

			//�X�R�[�v���[�h������
			SetDisableScope();

			//���̃t���[���̓��̍��W���擾
			check_posx = pos_x + cos(rotation_x*-1 - (float)M_PI/2) * sin(add_ry) * HUMAN_HEIGHT;
			check_posy = pos_y + cos(add_ry) * HUMAN_HEIGHT;
			check_posz = pos_z + sin(rotation_x*-1 - (float)M_PI/2) * sin(add_ry) * HUMAN_HEIGHT;

			if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
				deadstate = 2;
			}
			else{
				rotation_y += add_ry;
				deadstate = 1;
			}
			return 1;
		}
		return 0;
	}

	if( deadstate == 1 ){
		//135�x�ȏ�|��Ă����
		if( fabs(rotation_y) >= DegreeToRadian(135) ){
			deadstate = 2;
			return 2;
		}

		if( pos_y <= (HUMAN_DEADLINE + 10.0f) ){
			//90�x�ȏ�|��Ă����
			if( fabs(rotation_y) >= DegreeToRadian(90) ){
				deadstate = 4;
				return 2;
			}
		}

		//�O��ɓ|��
		if( rotation_y > 0.0f ){		//rotation_y < DegreeToRadian(135)
			add_ry += HUMAN_DEADADDRY;
		}
		else if( rotation_y < 0.0f ){	//rotation_y > DegreeToRadian(-135)
			add_ry -= HUMAN_DEADADDRY;
		}

		if( pos_y <= HUMAN_DEADLINE ){
			rotation_y += add_ry;
		}
		else{
			//���̃t���[���̓��̍��W���擾
			check_posx = pos_x + cos(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;
			check_posy = pos_y + cos(rotation_y + add_ry) * HUMAN_HEIGHT;
			check_posz = pos_z + sin(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;

			if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
				deadstate = 3;
			}
			else{
				rotation_y += add_ry;
			}
		}

		return 2;
	}

	if( deadstate == 2 ){
		if( pos_y <= HUMAN_DEADLINE ){
			deadstate = 4;
			return 2;
		}

		//���̃t���[���̑��̍��W
		check_posx = pos_x;
		check_posy = pos_y - 0.5f;
		check_posz = pos_z;

		if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
			deadstate = 4;
		}
		else{
			pos_y -= 0.5f;
		}

		return 2;
	}

	if( deadstate == 3 ){
		//deadstate = 4;

		//90�x�ȏ�|��Ă����
		if( fabs(rotation_y) >= DegreeToRadian(90) ){
			deadstate = 4;
			return 2;
		}

		//�O��ɓ|��
		if( rotation_y > 0.0f ){		//rotation_y < DegreeToRadian(90)
			add_ry += HUMAN_DEADADDRY;
		}
		else if( rotation_y < 0.0f ){	//rotation_y > DegreeToRadian(-90)
			add_ry -= HUMAN_DEADADDRY;
		}

		//���̃t���[���̑��̍��W���擾
		check_posx = pos_x - cos(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posy = pos_y + 0.1f;
		check_posz = pos_z - sin(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;

		if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
			deadstate = 4;
			return 2;
		}

		//���̃t���[���̓��̍��W���擾
		check_posx = pos_x - cos(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posy = pos_y + cos(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posz = pos_z - sin(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;

		if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
			deadstate = 4;
			return 2;
		}

		//���̍��W���ړ�
		pos_x -= cos(rotation_x*-1 - (float)M_PI/2) * sin(add_ry) * HUMAN_HEIGHT;
		pos_z -= sin(rotation_x*-1 - (float)M_PI/2) * sin(add_ry) * HUMAN_HEIGHT;

		rotation_y += add_ry;
		return 2;
	}

	if( deadstate == 4 ){
		//�������Ȃ��i�Œ�j
		deadstate = 5;
		return 3;
	}

	if( deadstate == 5 ){
		//�������Ȃ��i�Œ�j
		return 4;
	}

	return 0;
#else
	if( fabs(rotation_y) >= DegreeToRadian(90) ){
		return 4;
	}
	else if( rotation_y > 0.0f ){		//�|��n�߂Ă���΁A���̂܂ܓ|���B
		add_ry += HUMAN_DEADADDRY;
		rotation_y += add_ry;
		if( rotation_y >= DegreeToRadian(90) ){
			rotation_y = DegreeToRadian(90);
			return 3;
		}
		return 2;
	}
	else if( rotation_y < 0.0f ){	//�|��n�߂Ă���΁A���̂܂ܓ|���B
		add_ry -= HUMAN_DEADADDRY;
		rotation_y += add_ry;
		if( rotation_y <= DegreeToRadian(-90) ){
			rotation_y = DegreeToRadian(-90);
			return 3;
		}
		return 2;
	}
	else if( hp <= 0 ){		//HP�� 0 �ȉ��ɂȂ����i���S�����j�u�ԂȂ�A�|���n�߂�
		//�̂̊p�x�E�r�̊p�x
		switch( GetRand(4) ){
			case 0:
				add_ry = HUMAN_DEADADDRY;
				armrotation_y = DegreeToRadian(90);
				break;
			case 1:
				add_ry = HUMAN_DEADADDRY * -1;
				armrotation_y = DegreeToRadian(90);
				break;
			case 2:
				add_ry = HUMAN_DEADADDRY;
				armrotation_y = DegreeToRadian(-90);
				break;
			case 3:
				add_ry = HUMAN_DEADADDRY * -1;
				armrotation_y = DegreeToRadian(-90);
				break;
		}

		rotation_y += add_ry;

		//���̂����܂�ʂ悤�A������ +1.0 ����
		pos_y += 1.0f;

		//�������Ă��镐���S�Ď̂Ă�
		for(int i=0; i<TOTAL_HAVEWEAPON; i++){
			if( weapon[i] != NULL ){
				weapon[i]->Dropoff(pos_x, pos_y, pos_z, DegreeToRadian(10)*GetRand(36), 1.5f);
				weapon[i] = NULL;
			}
		}

		//�X�R�[�v���[�h������
		SetDisableScope();

		return 1;
	}

	return 0;
#endif
}

//! @brief ����ɂ��ړ��v�Z
//! @attention ���s����ƁA�e�L�[�t���O�� false �ɏ���������܂��B
void human::ControlProcess()
{
	//�i�s�����Ƒ��x������
	if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
		move_rx = DegreeToRadian(0);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_PROGRESSWALK_ACCELERATION);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_FORWARD ){
		move_rx = DegreeToRadian(0);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_PROGRESSRUN_ACCELERATION);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_BACK ){
		move_rx = DegreeToRadian(180);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_REGRESSRUN_ACCELERATION);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_LEFT ){
		move_rx = DegreeToRadian(90);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_SIDEWAYSRUN_ACCELERATION);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_RIGHT ){
		move_rx = DegreeToRadian(-90);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_SIDEWAYSRUN_ACCELERATION);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_FORWARD | MOVEFLAG_LEFT) ){
		move_rx = DegreeToRadian(45);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_PROGRESSRUN_SIDEWAYSRUN_ACCELERATION);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_BACK | MOVEFLAG_LEFT) ){
		move_rx = DegreeToRadian(135);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_REGRESSRUN_SIDEWAYSRUN_ACCELERATION);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_BACK | MOVEFLAG_RIGHT) ){
		move_rx = DegreeToRadian(-135);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_REGRESSRUN_SIDEWAYSRUN_ACCELERATION);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_FORWARD | MOVEFLAG_RIGHT) ){
		move_rx = DegreeToRadian(-45);
		AddPosOrder(rotation_x*-1 + move_rx + (float)M_PI/2, 0.0f, HUMAN_PROGRESSRUN_SIDEWAYSRUN_ACCELERATION);
	}
	else{
		move_rx = 0.0f;
	}

	//�t���O���o�b�N�A�b�v
	MoveFlag_lt = MoveFlag;

	//�L�[�t���O�����ɖ߂�
	MoveFlag = 0x00;
}

//! @brief �}�b�v�Ƃ̓����蔻��
//! @param CollD Collision�N���X�̃|�C���^
//! @param inblockdata BlockDataInterface�N���X�̃|�C���^
//! @param AddCollisionFlag �ǉ��̓����蔻��t���O
//! @param FallDist Y���̈ړ��ʂ��擾����|�C���^
//! @param nowmove_x X���̈ړ��ʂ��擾����|�C���^
//! @param nowmove_z Z���̈ړ��ʂ��擾����|�C���^
//! @return �u���b�N�ɖ��܂��Ă���Ftrue�@���܂��Ă��Ȃ��Ffalse
bool human::MapCollisionDetection(class Collision *CollD, class BlockDataInterface *inblockdata, bool AddCollisionFlag, float *FallDist, float *nowmove_x, float *nowmove_z)
{
	bool inside = false;
	int id;
	int face;
	float vx, vz;
	float vy = 0.1f;
	float speed;
	float Dist;
	float FallDistance;
	float offset;
	float move_x2 = move_x;
	float move_z2 = move_z;

	//�����M���M���͓����蔻�肩�珜�O����
	offset = 0.1f;

	//�@�㉺�����̓����蔻��i�W�����v�E���R�����j
	//--------------------------------------------------

	//�������u���b�N�ɖ��܂��Ă��Ȃ����
	if( CollD->CheckALLBlockInside(pos_x, pos_y + offset, pos_z) == false ){
		//�������x���v�Z
		move_y -= HUMAN_DAMAGE_SPEED;
		if( move_y < HUMAN_DAMAGE_MAXSPEED ){ move_y = HUMAN_DAMAGE_MAXSPEED; }

		if( move_y > 0.0f ){
			//������֓����蔻��
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + HUMAN_HEIGHT, pos_z, 0, 1, 0, NULL, NULL, &Dist, move_y) == true ){
				CollD->CheckALLBlockIntersectRay(pos_x, pos_y + HUMAN_HEIGHT, pos_z, 0, 1, 0, NULL, NULL, &Dist, move_y);

				FallDistance = Dist;
				move_y = 0.0f;
			}
			else{
				FallDistance = move_y;
			}
			move_y_flag = true;

			//DummyRay() �֐��̌��ʂɊւ�炸 FallDistance = move_y; �ł��ǂ��̂��낤���H
		}
		else{
			 int id, face;
			 struct blockdata bdata;

			//�������֓����蔻��
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + offset, pos_z, 0, -1, 0, NULL, NULL, &Dist, move_y*-1 + offset) == true ){
				if( CollD->CheckALLBlockIntersectRay(pos_x, pos_y + offset, pos_z, 0, -1, 0, &id, &face, &Dist, move_y*-1 + offset) == false ){
					//�����A-DummyRay()�֐��ł̓u���b�N��������̂� -Ray()�֐��Ō�����Ȃ��Ȃ�A�����炭�u���b�N���c��ł���B
					//�Ƃ肠���K���Ȓl�Ō떂�����A���U����h���B
					Dist = offset;
				}

				if( Invincible == false ){
					//�_���[�W�v�Z
					if( move_y > HUMAN_DAMAGE_MINSPEED ){ hp -= 0; }
					else{ hp -= (int)((float)HUMAN_DAMAGE_MAXFALL / fabs(HUMAN_DAMAGE_MAXSPEED - (HUMAN_DAMAGE_MINSPEED)) * fabs(move_y - (HUMAN_DAMAGE_MINSPEED))); }
				}

				FallDistance = (Dist - offset) * -1;
				move_y = 0.0f;
				
				inblockdata->Getdata(&bdata, id);

				//�Ζʂɗ����Ă���Ȃ�
				if( acos(bdata.material[face].vy) > HUMAN_MAPCOLLISION_SLOPEANGLE ){
					//�n�ʂƔF�߂Ȃ��@�i�W�����v�΍�j
					move_y_flag = true;

					float angvx = atan2(bdata.material[face].vz, bdata.material[face].vx);
					float angvy = acos(bdata.material[face].vy);

					//�����o���͂̌���
					//y = -0.6*x*x + 1.9*x - 0.2
					float force = -0.6f*angvy*angvy + 1.9f*angvy - 0.2f;

					//�����o��
					move_x = cos(angvx) * cos(HUMAN_MAPCOLLISION_SLOPEFORCEANGLE) * force;
					move_y = sin(HUMAN_MAPCOLLISION_SLOPEFORCEANGLE) * force;
					move_z = sin(angvx) * cos(HUMAN_MAPCOLLISION_SLOPEFORCEANGLE) * force;

					move_x2 = move_x;
					move_z2 = move_z;
				}
				else{
					move_y_flag = false;
				}
			}
			else{
				FallDistance = move_y;
				move_y_flag = true;
			}
		}
	}
	else{	//���܂��Ă���
		FallDistance = move_y;
		move_y = 0.0f;
		move_y_flag = false;
	}


	//�@���������̓����蔻��i�ړ��j
	//--------------------------------------------------

	if( (move_x2*move_x2 + move_z2*move_z2) ){
		int surface;
		float ang = atan2(move_z2, move_x2);
		float newpos_x, newpos_y, newpos_z;

		//���t�߂𓖂��蔻��
		for(int i=0; i<MAX_BLOCKS; i++){
			float min_x, min_y, min_z, max_x, max_y, max_z;
			CollD->GetBlockPosMINMAX(i, &min_x, &min_y, &min_z, &max_x, &max_y, &max_z);

			//���t�߂�AABB�ōr���
			if( CollideBoxAABB(min_x, min_y, min_z, max_x, max_y, max_z,
				pos_x - HUMAN_MAPCOLLISION_R - 1.0f, pos_y + HUMAN_MAPCOLLISION_HEIGHT - 1.0f, pos_z - HUMAN_MAPCOLLISION_R - 1.0f, pos_x + HUMAN_MAPCOLLISION_R + 1.0f, pos_y + HUMAN_MAPCOLLISION_HEIGHT + 1.0f, pos_z + HUMAN_MAPCOLLISION_R + 1.0f) == true
			){
				surface = -1;
				CollD->CheckBlockInside(i, pos_x, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z, false, &surface);

				if( surface != -1 ){
					//HUMAN_MAPCOLLISION_R ���̐�𒲂ׂ�
					if( CollD->CheckBlockInside(i, pos_x + cos(ang)*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z + sin(ang)*HUMAN_MAPCOLLISION_R, true, NULL) == true ){
						CollD->ScratchVector(i, surface, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
					}

					//���E90�x�Â𒲂ׂ�
					if( CollD->CheckBlockInside(i, pos_x + cos(ang + (float)M_PI/2)*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z + sin(ang + (float)M_PI/2)*HUMAN_MAPCOLLISION_R, true, NULL) == true ){
						if( CollD->CheckPolygonFrontRx(i, surface, ang) == true ){		//�i�s�����ɑ΂��ĕ\�����Ȃ�`
							CollD->ScratchVector(i, surface, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
						}
					}
					if( CollD->CheckBlockInside(i, pos_x + cos(ang - (float)M_PI/2)*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z + sin(ang - (float)M_PI/2)*HUMAN_MAPCOLLISION_R, true, NULL) == true ){
						if( CollD->CheckPolygonFrontRx(i, surface, ang) == true ){		//�i�s�����ɑ΂��ĕ\�����Ȃ�`
							CollD->ScratchVector(i, surface, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
						}
					}
				}
			}
		}

		//�i�s�����������x�N�g�����Z�o
		vx = move_x2;
		vz = move_z2;
		speed = sqrt(vx*vx + vz*vz);
		if( speed > 0.0f ){
			vx = vx / speed;
			vz = vz / speed;
		}

		//���𓖂��蔻��
		if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + HUMAN_HEIGHT, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == true ){
			CollD->CheckALLBlockIntersectRay(pos_x, pos_y + FallDistance + HUMAN_HEIGHT, pos_z, vx, 0, vz, &id, &face, &Dist, speed);
			CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
		}

		if( AddCollisionFlag == true ){
			//���t�߂̒ǉ������蔻��
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + HUMAN_MAPCOLLISION_ADD_HEIGHT_A, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == true ){
				CollD->CheckALLBlockIntersectRay(pos_x, pos_y + FallDistance + HUMAN_MAPCOLLISION_ADD_HEIGHT_A, pos_z, vx, 0, vz, &id, &face, &Dist, speed);
				CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
			}

			//���t�߂̒ǉ������蔻��
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + HUMAN_MAPCOLLISION_ADD_HEIGHT_B, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == true ){
				CollD->CheckALLBlockIntersectRay(pos_x, pos_y + FallDistance + HUMAN_MAPCOLLISION_ADD_HEIGHT_B, pos_z, vx, 0, vz, &id, &face, &Dist, speed);
				CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
			}
		}

		//�������u���b�N�ɖ��܂��Ă��Ȃ����
		if( CollD->CheckALLBlockInside(pos_x, pos_y + offset, pos_z) == false ){

			//�i�s�����������x�N�g�����Z�o
			vx = move_x2;
			vz = move_z2;
			speed = sqrt(vx*vx + vz*vz);
			if( speed > 0.0f ){
				vx = vx / speed;
				vz = vz / speed;
			}

			//�����𓖂��蔻��
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + offset, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == true ){
				CollD->CheckALLBlockIntersectRay(pos_x, pos_y + offset, pos_z, vx, 0, vz, &id, &face, &Dist, speed);

				struct blockdata bdata;
				inblockdata->Getdata(&bdata, id);
				float surface_ang = acos(bdata.material[face].vy);

				if( surface_ang > DegreeToRadian((90.0f-1.0f)) ){			//�ǂȂ�
					//���z�����鍂�������ׂ�
					if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + 3.5f + offset, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == false ){
						//�l����Ɏ����グ��
						FallDistance = 0.4f;
						move_y = 0.0f;
					}

					//�����𓖂��蔻��
					CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
				}
				else if( surface_ang > HUMAN_MAPCOLLISION_SLOPEANGLE ){		//�Ζʁ`�ǂȂ�
					//�����𓖂��蔻��
					CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
				}
				else{														//�����`�ΖʂȂ�
					//�n�ʂƔF�߂Ȃ��@�i�W�����v�΍�j
					move_y_flag = true;

					//�ړ���̈ʒu���v�Z
					newpos_x = pos_x + move_x2;
					newpos_y = pos_y + FallDistance;
					newpos_z = pos_z + move_z2;

					//�ړ���̍����𒲂ׂ�
					if( CollD->CheckALLBlockInside(newpos_x, newpos_y + HUMAN_HEIGHT, newpos_z) == false ){
						if( CollD->CheckALLBlockIntersectRay(newpos_x, newpos_y + HUMAN_HEIGHT, newpos_z, 0, -1, 0, NULL, NULL, &Dist, HUMAN_HEIGHT) == true ){
							float height = HUMAN_HEIGHT - Dist;

							//�l����Ɏ����グ��
							FallDistance = height;

							if( height > 0.4f ){
								move_x2 *= 0.75f;
								move_z2 *= 0.75f;
								move_x = move_x2;
								move_z = move_z2;
							}

							move_y = 0.0f;
						}
					}
				}
			}
		}

		//�ړ���̈ʒu���v�Z
		newpos_x = pos_x + move_x2;
		newpos_y = pos_y + FallDistance;
		newpos_z = pos_z + move_z2;

		//�S�g�����߂Ċm�F
		if(
			(CollD->CheckALLBlockInside(newpos_x, newpos_y + offset, newpos_z) == true)||
			(CollD->CheckALLBlockIntersectRay(newpos_x, newpos_y + offset, newpos_z, 0, 1, 0, NULL, NULL, &Dist, HUMAN_HEIGHT - offset - 1.0f) == true)
		){
			//�߂荞�ނȂ�ړ����Ȃ�
			move_x2 = 0.0f;
			move_z2 = 0.0f;
			inside = true;
		}
	}

	*FallDist = FallDistance;
	*nowmove_x = move_x2;
	*nowmove_z = move_z2;
	return inside;
}

//! @brief �v�Z�����s�i�����蔻��j
//! @param CollD Collision�̃|�C���^
//! @param inblockdata BlockDataInterface�̃|�C���^
//! @param AddCollisionFlag �ǉ��̓����蔻��t���O
//! @param player �Ώۂ̐l�����v���C���[���ǂ���
//! @param F5mode �㏸�@�\�iF5���Z�j�̃t���O�@�i�L���Ftrue�@�����Ffalse�j
//! @return �����Ȃ��F0�@�ʏ폈���F1�@���S���ē|��I���������F2�@�Î~�������́F3�@�n�`�ɂ�莀�S��������F4
int human::RunFrame(class Collision *CollD, class BlockDataInterface *inblockdata, bool AddCollisionFlag, bool player, bool F5mode)
{
	if( CollD == NULL ){ return 0; }
	if( EnableFlag == false ){ return 0; }

#ifdef HUMAN_DEADBODY_COLLISION
	if( deadstate == 5 ){ return 3; }
#else
	if( hp <= 0 ){
		if( fabs(rotation_y) >= DegreeToRadian(90) ){
			return 3;
		}
	}
#endif

	int WeaponReloadCnt;
	float FallDistance;
	float nowmove_x, nowmove_z;
	int CheckDead;
	int hp_old;

	//����؂�ւ��J�E���g
	if( selectweaponcnt > 0 ){
		selectweaponcnt -= 1;
	}

	//�����[�h�J�E���g�擾
	if( weapon[selectweapon] != NULL ){
		WeaponReloadCnt = weapon[selectweapon]->GetReloadCnt();
	}
	else{
		WeaponReloadCnt = 0;
	}

	//�Ə��̏�Ԍ덷�̏���
	GunsightErrorRange();

	//���S����Ɠ|��鏈��
	CheckDead = CheckAndProcessDead(CollD);
	if( CheckDead == 3 ){ return 2; }

	if( CheckDead == 0 ){
		//�i�s�����Ƒ��x������
		ControlProcess();

		//�}�b�v�Ƃ̓����蔻��
		hp_old = hp;
		MapCollisionDetection(CollD, inblockdata, AddCollisionFlag, &FallDistance, &nowmove_x, &nowmove_z);

		//�ړ�����Ȃ�
		if( (nowmove_x*nowmove_x + nowmove_z*nowmove_z) > 0.0f ){
			totalmove += sqrt(nowmove_x*nowmove_x + nowmove_z*nowmove_z);
		}

		//���W�ړ�
		pos_x += nowmove_x;
		pos_z += nowmove_z;

		//�ړ��ʂ�����
		move_x *= HUMAN_ATTENUATION;
		move_z *= HUMAN_ATTENUATION;

		//F5���g�p���Ă��Ȃ���΁A�v�Z���ʂ𔽉f
		if( F5mode == false ){
			pos_y += FallDistance;
		}
		else{
			move_y = 0.0f;
			pos_y += 5.0f;	//�g�p���Ă���΁A�����I�ɏ㏸
		}


		//-100.0��艺�ɗ�������A���S
		if( pos_y < HUMAN_DEADLINE ){
			pos_y = HUMAN_DEADLINE;
			hp = 0;
			return 4;
		}

		//����̃}�b�v�Ƃ̓����蔻���HP���[���ɂȂ����Ȃ�A�n�`�ɂ�鎀�S
		if( (hp_old >0)&&(hp <= 0) ){
			return 4;
		}
	}


	//���ݑ������镐��̃N���X���擾
	class weapon *nowweapon;
	int weapon_paramid;
	nowweapon = weapon[selectweapon];

	if( nowweapon == NULL ){		//��Ԃ�
		weapon_paramid = ID_WEAPON_NONE;
	}
	else{							//��������������Ă���
		nowweapon->GetParamData(&weapon_paramid, NULL, NULL);
	}

	//���[�V�����v�Z
	MotionCtrl->RunFrame(rotation_x, armrotation_y, weapon_paramid, WeaponReloadCnt, MoveFlag_lt, hp, player);

	if( CheckDead != 0 ){ return 3; }
	return 1;
}

//! @brief �W���덷���擾
int human::GetGunsightErrorRange()
{
	return StateGunsightErrorRange + ReactionGunsightErrorRange;
}

//! @brief �`��
//! @param d3dg D3DGraphics�̃|�C���^
//! @param Resource ResourceManager�̃|�C���^
//! @param DrawArm �r�ƕ���̂ݕ`�悷��
//! @param player �Ώۂ̐l�����v���C���[���ǂ���
//! @todo �r�̈ʒu���s��ŋ��߂�
//! @todo ���̂̕��ʂ̍����iY���j����������
void human::Render(class D3DGraphics *d3dg, class ResourceManager *Resource, bool DrawArm, bool player)
{
	//���g�p�����΍�
	UNREFERENCED_PARAMETER(player);

	//����������������Ă��Ȃ���΁A�������Ȃ�
	if( d3dg == NULL ){ return; }
	if( EnableFlag == false ){ return; }

	//���[�V�����擾
	int upmodel, armmodel, legmodel;
	float armry, legrx;
	MotionCtrl->GetRenderMotion(&armry, &legrx, &upmodel, &armmodel, &legmodel);

	if( DrawArm == false ){
		//�㔼�g��`��
		d3dg->SetWorldTransform(pos_x, pos_y - 1.0f, pos_z, rotation_x + (float)M_PI, rotation_y, upmodel_size);
		d3dg->RenderModel(upmodel, id_texture, DarkModelFlag);

		//����`��
		d3dg->SetWorldTransform(pos_x, pos_y, pos_z, legrx + (float)M_PI, rotation_y, legmodel_size);
		d3dg->RenderModel(legmodel, id_texture, DarkModelFlag);
	}

	//���ݑ������镐��̃N���X���擾
	class weapon *nowweapon;
	nowweapon = weapon[selectweapon];

	//�r��`��
	if( rotation_y != 0.0f ){		//���S���ē|��Ă��� or �|��n�߂�
		float x = pos_x + cos(rotation_x*-1 - (float)M_PI/2)*sin(rotation_y)*16.0f;
		float y = pos_y + cos(rotation_y)*16.0f;
		float z = pos_z + sin(rotation_x*-1 - (float)M_PI/2)*sin(rotation_y)*16.0f;
		d3dg->SetWorldTransform(x, y, z, rotation_x + (float)M_PI, armrotation_y + rotation_y, armmodel_size);
		d3dg->RenderModel(armmodel, id_texture, DarkModelFlag);
	}
	else if( nowweapon == NULL ){	//��Ԃ�
		d3dg->SetWorldTransform(pos_x, pos_y + 16.0f, pos_z, rotation_x + (float)M_PI, armry, armmodel_size);
		d3dg->RenderModel(armmodel, id_texture, DarkModelFlag);
	}
	else{							//��������������Ă���
		//����̃��f���ƃe�N�X�`�����擾
		int id_param;
		WeaponParameter paramdata;
		int model, texture;
		nowweapon->GetParamData(&id_param, NULL, NULL);
		Param->GetWeapon(id_param, &paramdata);
		Resource->GetWeaponModelTexture(id_param, &model, &texture);

		//�r��`��
		d3dg->SetWorldTransform(pos_x, pos_y + 16.0f, pos_z, rotation_x + (float)M_PI, armry, armmodel_size);
		d3dg->RenderModel(armmodel, id_texture, DarkModelFlag);

		//�����`��
		d3dg->SetWorldTransformHumanWeapon(pos_x, pos_y + 16.0f, pos_z, paramdata.mx/10*-1, paramdata.my/10, paramdata.mz/10*-1, rotation_x + (float)M_PI, armry, paramdata.size);
		d3dg->RenderModel(model, texture, DarkModelFlag);
	}
}

//! @brief �R���X�g���N�^
weapon::weapon(class ParameterInfo *in_Param, float x, float y, float z, float rx, int id_param, int nbs, bool flag)
{
	Param = in_Param;
	pos_x = x;
	pos_y = y;
	pos_z = z;
	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	rotation_x = rx;
	EnableFlag = flag;

	id_parameter = id_param;
	usingflag = false;
	bullets = nbs;
	Loadbullets = 0;
	shotcnt = 0;
	motionflag = true;

	if( Param != NULL ){
		WeaponParameter ParamData;
		if( Param->GetWeapon(id_param, &ParamData) == 0 ){
			model_size = ParamData.size;
			//id_model = ParamData.id_model;
			//id_texture = ParamData.id_texture;
		}
	}
	id_model = -1;
	id_texture = -1;
}

//! @brief �f�B�X�g���N�^
weapon::~weapon()
{}

//! @brief ���W�Ɗp�x��ݒ�
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param rx ������]
void weapon::SetPosData(float x, float y, float z, float rx)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	rotation_x = rx;
}

//! @brief �ݒ�l��ݒ�
//! @param id_param ����̎�ޔԍ�
//! @param lnbs ���e��
//! @param nbs ���v�e��
//! @param init �I�u�W�F�N�g��������
void weapon::SetParamData(int id_param, int lnbs, int nbs, bool init)
{
	id_parameter = id_param;
	bullets = nbs;
	Loadbullets = lnbs;

	if( init == true ){
		usingflag = false;
		shotcnt = 0;
		reloadcnt = 0;
		motionflag = true;
	}
}

//! @brief �ݒ�l���擾
//! @param id_param ����̎�ޔԍ����󂯎��|�C���^�iNULL�j
//! @param lnbs ���e�����󂯎��|�C���^�iNULL�j
//! @param nbs ���v�e�����󂯎��|�C���^�iNULL�j
void weapon::GetParamData(int *id_param, int *lnbs, int *nbs)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( lnbs != NULL ){ *lnbs = Loadbullets; }
	if( nbs != NULL ){ *nbs = bullets; }
}

//! @brief ����̎g�p�󋵂̎擾
//! @return �g�p���Ftrue�@���g�p�Ffalse
bool weapon::GetUsingFlag()
{
	return usingflag;
}

//! @brief ������E��
//! @return �����F0�@���s�F1
int weapon::Pickup()
{
	if( usingflag == true ){ return 1; }
	usingflag = true;
	return 0;
}

//! @brief ������̂Ă�
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param rx ������]
//! @param speed �̂Ă鏉��
void weapon::Dropoff(float x, float y, float z, float rx, float speed)
{
	//�`�悷����W�Ɗp�x��ݒ�
	move_x = cos(rx*-1 + (float)M_PI/2) * speed;
	move_y = 0.0f;
	move_z = sin(rx*-1 + (float)M_PI/2) * speed;
	pos_x = x + cos(rx*-1 + (float)M_PI/2) * 5.0f;
	pos_y = y + 16.0f + move_y;
	pos_z = z + sin(rx*-1 + (float)M_PI/2) * 5.0f;
	rotation_x = rx + (float)M_PI;

	//���g�p�i�������j�ɐݒ肵�A���W�ړ���L����
	usingflag = false;
	motionflag = true;
}

//! @brief ���C
//! @return �����F0�@���s�F1
//! @attention �A�ˊԊu���l������܂��B
//! @attention �֐������s����̂́A�����ꂩ�̏����ł��B�@�u�A�ˊԊu�ɖ����Ȃ��v�u�����[�h���s���v�u�e���Ȃ��v�u�����ȕ���̎�ނ��ݒ肳��Ă���v
int weapon::Shot()
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( Param == NULL ){ return 1; }

	//���ˊԊu�ɖ����Ȃ����A�����[�h�����A�e��������Ύ��s
	if( shotcnt > 0 ){ return 1; }
	if( reloadcnt > 0 ){ return 1; }
	if( Loadbullets == 0 ){ return 1; }

	//�ݒ�l���擾
	WeaponParameter ParamData;
	if( Param->GetWeapon(id_parameter, &ParamData) == 1 ){ return 1; }

	//���킪��֒e�Ȃ�΁`
	if( id_parameter == ID_WEAPON_GRENADE ){
		//�e�����炵�A�A�˃J�E���g��ݒ�
		bullets -= 1;
		Loadbullets -= 1;
		shotcnt = ParamData.blazings;

		if( (bullets - Loadbullets) <= 0 ){		//�i�����[�h���Ă��Ȃ��j�e�������Ȃ�΁A���킲�Ə��ł�����B
			EnableFlag = false;
			usingflag = false;
		}
		else if( Loadbullets <= 0 ){			//���������[�h
			Loadbullets = 1;
		}
		return 0;
	}

	//�e�����炵�A�A�˃J�E���g��ݒ�
	Loadbullets -= 1;
	bullets -= 1;
	shotcnt = ParamData.blazings;
	return 0;
}

//! @brief �����[�h���J�n
//! @return �����F0�@���s�F1
//! @attention �����[�h���Ԃ��l������܂��B
//! @attention �֐������s����̂́A�����ꂩ�̏����ł��B�@�u�����[�h���s���v�u�e���Ȃ��v�u�����ȕ���̎�ނ��ݒ肳��Ă���v
int weapon::StartReload()
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( Param == NULL ){ return 1; }

	//�����[�h�����A�e��������Ύ��s
	if( reloadcnt > 0 ){ return 1; }
	if( (bullets - Loadbullets) == 0 ){ return 1; }

	//����̐��\�l���擾
	WeaponParameter ParamData;
	if( Param->GetWeapon(id_parameter, &ParamData) != 0 ){ return 1; }

	//�����[�h�J�E���g��ݒ�
	reloadcnt = ParamData.reloads + 1;
	return 0;
}

//! @brief �����[�h�����s
//! @attention StartReload()�֐��ƈقȂ�A�u���ɒe���[���܂��B�����[�h���Ԃ͍l������܂���B
//! @attention �����[�h���Ԃ��l������ꍇ�AStartReload()�֐����Ăяo���Ă��������B���̊֐��͎����I�Ɏ��s����܂��B
int weapon::RunReload()
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( Param == NULL ){ return 1; }

	//�e��������Ύ��s
	if( (bullets - Loadbullets) == 0 ){ return 1; }

	//����̐��\�l����A���U����e�����擾
	WeaponParameter ParamData;
	int nbsmax = 0;
	if( Param->GetWeapon(id_parameter, &ParamData) == 0 ){
		nbsmax = ParamData.nbsmax;
	}

	if( (bullets - Loadbullets) < nbsmax ){	//�c��̒e����葕�U����e�����������
		bullets = (bullets - Loadbullets);
		Loadbullets = bullets;
	}
	else{									//�c��̒e���̕����������
		bullets -= Loadbullets;
		Loadbullets = nbsmax;
	}

	return 0;
}

//! @brief �����[�h�J�E���g���擾
//! @return �J�E���g���@�i�����[�h���F1�ȏ�j
int weapon::GetReloadCnt()
{
	return reloadcnt;
}

//! @brief ����̎�ށE���e���̕ύX
//! @param Resource ResourceManager�̃|�C���^
//! @param id_param ��ޔԍ�
//! @param lnbs ���e��
//! @param nbs ���v�e��
//! @return �����F1�@���s�F0
//! @attention �v���C���[�ɂ�闠�Z�iF6�EF7�j�p�ɗp�ӂ��ꂽ�֐��ł��B��֒e���I�����ꂽ�ꍇ�A�����I�ɒe���[���܂��B
//! @attention �g�p����Ă��Ȃ�����I�u�W�F�N�g�ɑ΂��Ď��s����ƁA���̊֐��͎��s���܂��B
bool weapon::ResetWeaponParam(class ResourceManager *Resource, int id_param, int lnbs, int nbs)
{
	//����������Ă��Ȃ���΁A���s
	if( EnableFlag == false ){ return 0; }

	//�w�肳�ꂽ�ݒ�l�֏㏑��
	id_parameter = id_param;
	bullets = nbs;
	Loadbullets = lnbs;

	//������֒e�Ȃ�΁A���������[�h
	if( id_param == ID_WEAPON_GRENADE ){
		if( (bullets > 0)&&(Loadbullets == 0) ){
			Loadbullets = 1;
		}
	}

	//���f���ƃe�N�X�`����ݒ�
	Resource->GetWeaponModelTexture(id_param, &id_model, &id_texture);
	WeaponParameter param;
	if( Param->GetWeapon(id_param, &param) == 0 ){
		model_size = param.size;
	}

	return 1;
}

//! @brief �v�Z�����s�i���R�����j
//! @param CollD Collision�̃|�C���^
int weapon::RunFrame(class Collision *CollD)
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( CollD == NULL ){ return 0; }

	//����������Ă��Ȃ���΁A���s
	if( EnableFlag == false ){ return 0; }

	//�A�˃J�E���g���c���Ă���΁A1 ���炷
	if( shotcnt > 0 ){
		shotcnt -= 1;
	}
	else if( reloadcnt > 0 ){
		//�����[�h�J�E���g���c���Ă���� 1 ���炵�A�J�E���g�� 0 �Ȃ�΃����[�h���������s
		reloadcnt -= 1;
		if( reloadcnt == 0 ){
			RunReload();
		}
	}

	//�N�ɂ��g���Ă��炸�A�ړ��t���O���L���Ȃ�΁`
	if( (usingflag == false)&&(motionflag == true) ){
		float Dist;
		float maxDist;

		//�ړ����x���X�V
		move_x *= 0.96f;
		move_z *= 0.96f;
		if( move_y > -1.8f ){
			move_y -= 0.3f;
		}

		//�u���b�N�ɖ��܂��Ă���Ώ������Ȃ�
		if( CollD->CheckALLBlockInside(pos_x, pos_y, pos_z) == true ){
			motionflag = false;
			return 0;
		}

		//�����̈ړ����x�����߂�
		maxDist = sqrt(move_x*move_x + move_z*move_z);
		if( maxDist < 0.1f ){
			maxDist = 0.0f;
			move_x = 0.0f;
			move_z = 0.0f;
		}

		//�u���b�N�ɐڂ��Ă���΁A����ȏ�͐����ړ����Ȃ��B
		if( maxDist > 0.0f ){
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y, pos_z, move_x/maxDist, 0, move_z/maxDist, NULL, NULL, &Dist, maxDist) == true ){
				//�u���b�N�ɖ��܂�Ȃ��悤�Ɏ�O��
				Dist -= 0.1f;

				//�ڂ��Ă�����W�܂ňړ�
				pos_x += move_x/maxDist * Dist;
				pos_z += move_z/maxDist * Dist;

				//�ړ��ʂ� 0 ��
				move_x = 0.0f;
				move_z = 0.0f;
			}
		}

		//�u���b�N�ɐڂ��Ă���΁A�����܂ŗ�������
		if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y, pos_z, 0, -1, 0, NULL, NULL, &Dist, fabs(move_y)) == true ){
			CollD->CheckALLBlockIntersectRay(pos_x, pos_y, pos_z, 0, -1, 0, NULL, NULL, &Dist, fabs(move_y));
			pos_y -= Dist - 0.2f;
			motionflag = false;
			return 0;
		}

		//���W�𔽉f
		pos_x += move_x;
		pos_y += move_y;
		pos_z += move_z;
	}

	return 0;
}

//! @brief �`��
//! @param d3dg D3DGraphics�̃|�C���^
void weapon::Render(class D3DGraphics *d3dg)
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( d3dg == NULL ){ return; }

	//����������ĂȂ����A�N���Ɏg���Ă���Ώ������Ȃ�
	if( EnableFlag == false ){ return; }
	if( usingflag == true ){ return; }

	//�����`��
	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, rotation_x, 0.0f, (float)M_PI/2, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag);
}

//! @brief �R���X�g���N�^
smallobject::smallobject(class ParameterInfo *in_Param, class MIFInterface *in_MIFdata, float x, float y, float z, float rx, int id_param, signed char p4, bool flag)
{
	Param = in_Param;
	MIFdata = in_MIFdata;
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;
	rotation_y = 0.0f;
	EnableFlag = flag;
	model_size = 5.0f;

	id_parameter = id_param;
	point_p4 = p4;

	hp = 0;		//�b��
	jump_rx = 0.0f;
	move_rx = 0.0f;
	add_rx = 0.0f;
	add_ry = 0.0f;
	jump_cnt = 0;

	if( Param != NULL ){
		SmallObjectParameter ParamData;
		if( Param->GetSmallObject(id_param, &ParamData) == 0 ){
			hp = ParamData.hp;
		}
	}
	id_model = -1;
	id_texture = -1;
}

//! @brief �f�B�X�g���N�^
smallobject::~smallobject()
{}

//! @brief MIF�f�[�^���Ǘ�����N���X��ݒ�
//! @param in_MIFdata MIFInterface�N���X�̃|�C���^
void smallobject::SetMIFInterfaceClass(class MIFInterface *in_MIFdata)
{
	MIFdata = in_MIFdata;
}

//! @brief �ݒ�l��ݒ�
//! @param id_param �����̎�ޔԍ�
//! @param p4 ��4�p�����[�^
//! @param init �I�u�W�F�N�g��������
void smallobject::SetParamData(int id_param, signed char p4, bool init)
{
	rotation_y = 0.0f;
	id_parameter = id_param;
	point_p4 = p4;

	if( init == true ){
		hp = 0;		//�b��
		jump_rx = 0.0f;
		move_rx = 0.0f;
		add_rx = 0.0f;
		add_ry = 0.0f;
		jump_cnt = 0;

		if( id_param == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
			if( MIFdata != NULL ){
				hp = MIFdata->GetAddSmallobjectHP();
			}
		}
		else{
			if( Param != NULL ){
				SmallObjectParameter ParamData;
				if( Param->GetSmallObject(id_param, &ParamData) == 0 ){
					hp = ParamData.hp;
				}
			}
		}
	}
}

//! @brief �ݒ�l���擾
//! @param id_param �����̎�ޔԍ����󂯎��|�C���^�iNULL�j
//! @param p4 ��4�p�����[�^���󂯎��|�C���^�iNULL�j
void smallobject::GetParamData(int *id_param, signed char *p4)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( p4 != NULL ){ *p4 = point_p4; }
}

//! @brief �̗͂��擾
//! @return �̗͒l
int smallobject::GetHP()
{
	return hp;
}

//! @brief �u���b�N�̏�Ɉړ�
//! @param CollD Collision�̃|�C���^
//! @return ���̍��W����̈ړ��ʁi0�ňړ��Ȃ��j
float smallobject::CollisionMap(class Collision *CollD)
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( CollD == NULL ){ return 0.0f; }

	float Dist;
	SmallObjectParameter ParamData;

	//�u���b�N�ɖ��܂��Ă���΁A���̂܂�
	if( CollD->CheckALLBlockInside(pos_x, pos_y, pos_z) == true ){ return 0.0f; }

	//�������ɓ����蔻��
	if( CollD->CheckALLBlockIntersectRay(pos_x, pos_y, pos_z, 0, -1, 0, NULL, NULL, &Dist, 1000.0f) == true ){
		//�����蔻��̑傫�����擾
		if( id_parameter == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
			Dist -= (float)MIFdata->GetAddSmallobjectDecide()/10.0f;
		}
		else{
			if( Param->GetSmallObject(id_parameter, &ParamData) == 0 ){
				Dist -= (float)ParamData.decide/10.0f;
			}
		}

		//���W���ړ�����
		pos_y -= Dist;
		return Dist;
	}

	return 0.0f;
}

//! @brief �e���q�b�g����
//! @param attacks �e�̍U����
void smallobject::HitBullet(int attacks)
{
	hp -= attacks;
	if( hp <= 0 ){
		Destruction();
	}
}

//! @brief ��֒e�̔������q�b�g����
//! @param attacks �����̍U����
//! @attention �����ɂ��v�Z�����O�ɍς܂��Ă��������B
void smallobject::HitGrenadeExplosion(int attacks)
{
	hp -= attacks;
	if( hp <= 0 ){
		Destruction();
	}
}

//! @brief ������j�󂷂�
//! @attention �ʏ�� HitBullet()�֐� ����� GrenadeExplosion()�֐� ���玩���I�Ɏ��s����邽�߁A���ڌĂяo���K�v�͂���܂���B
void smallobject::Destruction()
{
	//EnableFlag = false;
	//return;

	int jump;

	//��ы���擾
	if( id_parameter == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
		jump = MIFdata->GetAddSmallobjectJump();
	}
	else{
		SmallObjectParameter paramdata;
		Param->GetSmallObject(id_parameter, &paramdata);
		jump = paramdata.jump;
	}

	//��΂�
	hp = 0;
	jump_cnt = jump;

	//�p���ݒ�
	jump_rx = DegreeToRadian(10) * GetRand(36);
	move_rx = (float)jump * 0.04243f;
	add_rx = DegreeToRadian(1) * GetRand(20);
	add_ry = DegreeToRadian(1) * GetRand(20);
}

//! @brief �v�Z�����s�i�j�󎞂̈ړ��Ȃǁj
int smallobject::RunFrame()
{
	//�`�悳��Ă��Ȃ����A�̗͂��c���Ă��Ȃ���Ώ������Ȃ��B
	if( EnableFlag == false ){ return 0; }
	if( hp > 0 ){ return 0; }

	int cnt;
	int jump;

	//��ы���擾
	if( id_parameter == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
		jump = MIFdata->GetAddSmallobjectJump();
	}
	else{
		SmallObjectParameter paramdata;
		Param->GetSmallObject(id_parameter, &paramdata);
		jump = paramdata.jump;
	}
	
	//�������ł���J�E���g�����v�Z
	cnt = jump - jump_cnt;

	//�p��������W�E�p�x���v�Z
	pos_x += cos(jump_rx) * move_rx;
	pos_y += jump_cnt * 0.1f;
	pos_z += sin(jump_rx) * move_rx;
	rotation_x += add_rx;
	rotation_y += add_ry;

	jump_cnt -= 1;

	//1�b���ł�����`��I��
	if( cnt > (int)GAMEFPS ){
		EnableFlag = false;
		return 2;
	}

	return 1;
}

//! @brief �`��
//! @param d3dg D3DGraphics�̃|�C���^
void smallobject::Render(D3DGraphics *d3dg)
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( d3dg == NULL ){ return; }

	//����������Ă��Ȃ���Ώ������Ȃ��B
	if( EnableFlag == false ){ return; }

	//�`��
	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, rotation_x, rotation_y, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag);
}

//! @brief �R���X�g���N�^
bullet::bullet(int modelid, int textureid)
{
	model_size = 1.0f;
	id_model = modelid;
	id_texture = textureid;
	EnableFlag = false;
}

//! @brief �f�B�X�g���N�^
bullet::~bullet()
{}

//! @brief ���W�Ɗp�x��ݒ�
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param rx ������]
//! @param ry �c����]
void bullet::SetPosData(float x, float y, float z, float rx, float ry)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;
	rotation_y = ry;
}

//! @brief �ݒ�l��ݒ�
//! @param _attacks �U����
//! @param _penetration �ђʗ�
//! @param _speed �e��
//! @param _teamid �`�[���ԍ�
//! @param _humanid �l�̃f�[�^�ԍ�
//! @param init �I�u�W�F�N�g��������
void bullet::SetParamData(int _attacks, int _penetration, int _speed, int _teamid, int _humanid, bool init)
{
	attacks = _attacks;
	penetration = _penetration;
	speed = _speed;
	teamid = _teamid;
	humanid = _humanid;

	if( init == true ){
		cnt = 0;
	}
}

//! @brief ���W�Ɗp�x���擾
//! @param x X���W���󂯎��|�C���^�iNULL�j
//! @param y Y���W���󂯎��|�C���^�iNULL�j
//! @param z Z���W���󂯎��|�C���^�iNULL�j
//! @param rx ������]���󂯎��|�C���^�iNULL�j
//! @param ry �c����]���󂯎��|�C���^�iNULL�j
void bullet::GetPosData(float *x, float *y, float *z, float *rx, float *ry)
{
	if( x != NULL ){ *x = pos_x; }
	if( y != NULL ){ *y = pos_y; }
	if( z != NULL ){ *z = pos_z; }
	if( rx != NULL ){ *rx = rotation_x; }
	if( ry != NULL ){ *ry = rotation_y; }
}

//! @brief �ݒ�l���擾
//! @param _attacks �U���͂��󂯎��|�C���^�iNULL�j
//! @param _penetration �ђʗ͂��󂯎��|�C���^�iNULL�j
//! @param _speed �e�����󂯎��|�C���^�iNULL�j
//! @param _teamid �`�[���ԍ����󂯎��|�C���^�iNULL�j
//! @param _humanid �l�̃f�[�^�ԍ����󂯎��|�C���^�iNULL�j
void bullet::GetParamData(int *_attacks, int *_penetration, int *_speed, int *_teamid, int *_humanid)
{
	if( _attacks != NULL ){ *_attacks = attacks; }
	if( _penetration != NULL ){ *_penetration = penetration; }
	if( _speed != NULL ){ *_speed = speed; }
	if( _teamid != NULL ){ *_teamid = teamid; }
	if( _humanid != NULL ){ *_humanid = humanid; }
}

//! @brief �v�Z�����s�i�e�̐i�s�E���ԏ��Łj
int bullet::RunFrame()
{
	//����������Ă��Ȃ���Ώ������Ȃ�
	if( EnableFlag == false ){ return 0; }

	//���Ŏ��Ԃ��߂��Ă���΁A�I�u�W�F�N�g�𖳌���
	if( cnt > BULLET_DESTROYFRAME ){
		EnableFlag = false;
		return 0;
	}

	//�ړ�����
	pos_x += cos(rotation_x)*cos(rotation_y)*speed;
	pos_y += sin(rotation_y)*speed;
	pos_z += sin(rotation_x)*cos(rotation_y)*speed;
	cnt += 1;

	return 0;
}

//! @brief �`��
//! @param d3dg D3DGraphics�̃|�C���^
void bullet::Render(class D3DGraphics *d3dg)
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( d3dg == NULL ){ return; }

	//����������Ă��Ȃ���Ώ������Ȃ��B
	if( EnableFlag == false ){ return; }

	//�e���ړ��O��������`�悵�Ȃ�
	//�@�e��������˂������Č�����΍�
	if( cnt == 0 ){ return; }

	//�`��
	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, (rotation_x * -1 - (float)M_PI/2), rotation_y, model_size);
	d3dg->RenderModel(id_model, id_texture, false);
}

//! @brief �R���X�g���N�^
grenade::grenade(int modelid, int textureid) : bullet(modelid, textureid)
{
	if( Param != NULL ){
		WeaponParameter ParamData;
		if( Param->GetWeapon(ID_WEAPON_GRENADE, &ParamData) == 0 ){
			model_size = ParamData.size;
		}
	}
}

//! @brief �f�B�X�g���N�^
grenade::~grenade()
{}

//! @brief ���W�Ə���ݒ�
//! @param speed ����
//! @param _teamid �`�[���ԍ�
//! @param _humanid �l�̃f�[�^�ԍ�
//! @param init �I�u�W�F�N�g��������
//! @attention ��� SetPosData() �����s���Ă��������B
void grenade::SetParamData(float speed, int _teamid, int _humanid, bool init)
{
	move_x = cos(rotation_x) * cos(rotation_y) * speed;
	move_y = sin(rotation_y) * speed;
	move_z = sin(rotation_x) * cos(rotation_y) * speed;
	teamid = _teamid;
	humanid = _humanid;

	if( init == true ){
		cnt = 0;
	}
}

//! @brief �ݒ�l���擾
//! @param _speed ���x���󂯎��|�C���^�iNULL�j
//! @param _teamid �`�[���ԍ����󂯎��|�C���^�iNULL�j
//! @param _humanid �l�̃f�[�^�ԍ����󂯎��|�C���^�iNULL�j
void grenade::GetParamData(float *_speed, int *_teamid, int *_humanid)
{
	if( _speed != NULL ){ *_speed = GetSpeed(); }
	if( _teamid != NULL ){ *_teamid = teamid; }
	if( _humanid != NULL ){ *_humanid = humanid; }
}

//! @brief ���x���擾
//! @return ���x
float grenade::GetSpeed()
{
	return sqrt(move_x*move_x + move_y*move_y + move_z*move_z);
}

//! @brief �v�Z�����s�i��֒e�̐i�s�E�����j
//! @return �����F2�@�o�E���h�E���˕Ԃ�F1�@����ȊO�F0
int grenade::RunFrame(class Collision *CollD)
{
	//����������Ă��Ȃ���Ώ������Ȃ�
	if( EnableFlag == false ){ return 0; }

	//���Ԃ��߂��Ă���΁A�I�u�W�F�N�g�𖳌������A�u�����v�Ƃ��ĕԂ��B
	if( cnt > GRENADE_DESTROYFRAME ){
		EnableFlag = false;
		return 2;
	}

	//�Î~���Ă���Ώ������Ȃ�
	if( (move_x == 0.0f)&&(move_y == 0.0f)&&(move_z == 0.0f) ){
		cnt += 1;
		return 0;
	}

	/*
	//�Î~�ɋ߂���ԂȂ�΁A�ړ����������Ȃ��B
	if( (move_x*move_x + move_y*move_y + move_z*move_z) < 0.1f*0.1f ){
		cnt += 1;
		return 0;
	}
	*/

	//�ړ����x���v�Z
	move_x *= 0.98f;
	move_y = (move_y - 0.17f) * 0.98f;
	move_z *= 0.98f;

	int id, face;
	float Dist;
	float maxDist = sqrt(move_x*move_x + move_y*move_y + move_z*move_z);

	//�}�b�v�ɑ΂��ē����蔻������s
	if( CollD->CheckALLBlockIntersectRay(pos_x, pos_y, pos_z, move_x/maxDist, move_y/maxDist, move_z/maxDist, &id, &face, &Dist, maxDist) == true ){
		float vx, vy, vz;

		//�}�b�v�ƏՓ˂�����W�܂ňړ�
		pos_x += move_x/maxDist * (Dist - 0.1f);
		pos_y += move_y/maxDist * (Dist - 0.1f);
		pos_z += move_z/maxDist * (Dist - 0.1f);

		//���˂���x�N�g�������߂�
		CollD->ReflectVector(id, face, move_x, move_y, move_z, &vx, &vy, &vz);

		//����
		move_x = vx * GRENADE_BOUND_ACCELERATION;
		move_y = vy * GRENADE_BOUND_ACCELERATION;
		move_z = vz * GRENADE_BOUND_ACCELERATION;

		cnt += 1;
		return 1;
	}

	//���W���ړ�
	pos_x += move_x;
	pos_y += move_y;
	pos_z += move_z;

	cnt += 1;
	return 0;
}

//! @brief �`��
//! @param d3dg D3DGraphics�̃|�C���^
void grenade::Render(class D3DGraphics *d3dg)
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( d3dg == NULL ){ return; }

	//����������Ă��Ȃ���Ώ������Ȃ��B
	if( EnableFlag == false ){ return; }

	//�`��
	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, (rotation_x * -1 - (float)M_PI/2), 0.0f, (float)M_PI/2, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag);
}

//! @brief �R���X�g���N�^
effect::effect(float x, float y, float z, float size, float rotation, int count, int texture, int settype)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	model_size = size;
	rotation_x = 0.0f;
	rotation_y = 0.0f;
	rotation_texture = rotation;
	cnt = count;
	setcnt = count;
	id_texture = texture;
	type = settype;
	if( cnt > 0 ){
		EnableFlag = true;
	}
	else{
		EnableFlag = false;
	}
	alpha = 1.0f;
}

//! @brief �f�B�X�g���N�^
effect::~effect()
{}

//! @brief �ݒ�l��ݒ�
//! @param in_move_x X���ړ���
//! @param in_move_y Y���ړ���
//! @param in_move_z Z���ړ���
//! @param size �`��{��
//! @param rotation ��]�p�x
//! @param count �`��t���[����
//! @param texture �e�N�X�`���̔F���ԍ�
//! @param settype �G�t�F�N�g�̎�ށ@�iEffect_Type ��g�ݍ�����j
//! @param init �I�u�W�F�N�g��������
void effect::SetParamData(float in_move_x, float in_move_y, float in_move_z, float size, float rotation, int count, int texture, int settype, bool init)
{
	move_x = in_move_x;
	move_y = in_move_y;
	move_z = in_move_z;
	model_size = size;
	rotation_texture = rotation;
	cnt = count;
	setcnt = count;
	id_texture = texture;
	type = settype;

	if( type & EFFECT_TRANSLUCENT ){
		set_alpha = 0.5f;
	}
	else{
		set_alpha = 1.0f;
	}
	alpha = set_alpha;

	if( init == true ){
		rotation_x = 0.0f;
		rotation_y = 0.0f;
	}
}

//! @brief �����Əc���̌�����ݒ�
//! @param rx �ݒ肷�鉡��
//! @param ry �ݒ肷��c��
void effect::SetRxRy(float rx, float ry)
{
	rotation_x = rx;
	rotation_y = ry;
}

//! �e�N�X�`���F���ԍ����擾
//! @return �e�N�X�`���F���ԍ�
int effect::GetTextureID()
{
	return id_texture;
}

//! �G�t�F�N�g�̈ړ��ʂ��擾
//! @param mx X���ړ��ʂ��󂯎��|�C���^
//! @param my Y���ړ��ʂ��󂯎��|�C���^
//! @param mz Z���ړ��ʂ��󂯎��|�C���^
void effect::GetMove(float *mx, float *my, float *mz)
{
	*mx = move_x;
	*my = move_y;
	*mz = move_z;
}

//! �G�t�F�N�g�̃}�b�v�����蔻��t���O�擾
//! @return �L���Ftrue�@�����Ffalse
bool effect::GetCollideMapFlag()
{
	if( type&EFFECT_COLLIDEMAP ){
		return true;
	}
	//else{
		return false;
	//}
}

//! @brief �v�Z�����s�i�r���{�[�h���j
//! @param camera_rx �J�����̉����p�x
//! @param camera_ry �J�����̏c���p�x
//! @return �������s�F1�@�`��ŏI�t���[���F2�@�����Ȃ��F0
int effect::RunFrame(float camera_rx, float camera_ry)
{
	//����������Ă��Ȃ���Ώ������Ȃ�
	if( EnableFlag == false ){ return 0; }

	//�J�E���g���I��������A�������Ȃ��悤�ɐݒ�
	if( cnt <= 0 ){
		EnableFlag = false;
		return 2;
	}

	if( (type & EFFECT_NOBILLBOARD) == 0 ){
		//�J�������W��K�p���A�r���{�[�h������
		rotation_x = camera_rx*-1;
		rotation_y = camera_ry;
	}

	//���W�ړ�
	pos_x += move_x;
	pos_y += move_y;
	pos_z += move_z;

	//���ꏈ�������s
	if( type & EFFECT_DISAPPEAR ){	//����
		alpha -= set_alpha/setcnt;
	}
	if( type & EFFECT_DISAPPEARHALF ){	//�����̎��Ԃŏ���
		if( setcnt/2 > cnt ){
			alpha -= set_alpha/(setcnt/2);
		}
	}
	if( type & EFFECT_MAGNIFY ){	//�g��
		model_size += model_size/50;
	}
	if( type & EFFECT_ROTATION ){	//��]
		if( rotation_texture > 0.0f ){
			rotation_texture += DegreeToRadian(1);
		}
		else{
			rotation_texture -= DegreeToRadian(1);
		}
	}
	if( type & EFFECT_FALL ){		//����
		move_y = (move_y - 0.17f) * 0.98f;
	}

	//�J�E���g�� 1 ����
	cnt -= 1;
	return 1;
}

//! @brief �`��
//! @param d3dg D3DGraphics�̃|�C���^
void effect::Render(class D3DGraphics *d3dg)
{
	//�N���X���ݒ肳��Ă��Ȃ���Ύ��s
	if( d3dg == NULL ){ return; }

	//����������Ă��Ȃ���Ώ������Ȃ��B
	if( EnableFlag == false ){ return; }

	//�`��
	d3dg->SetWorldTransformEffect(pos_x, pos_y, pos_z, rotation_x, rotation_y, rotation_texture, model_size);
	d3dg->RenderBoard(id_texture, alpha);
}

//! @brief �R���X�g���N�^
HumanMotionControl::HumanMotionControl(class ParameterInfo *in_Param)
{
	Param = in_Param;

	id_upmodel = -1;
	for(int i=0; i<TOTAL_ARMMODE; i++){
		id_armmodel[i] = -1;
	}
	id_legmodel = -1;
	for(int i=0; i<TOTAL_WALKMODE; i++){
		id_walkmodel[i] = -1;
	}
	for(int i=0; i<TOTAL_RUNMODE; i++){
		id_runmodel[i] = -1;
	}

	reaction_y = 0.0f;
	armmodel_rotation_y = 0.0f;
	legrotation_x = 0.0f;
	walkcnt = 0;
	runcnt = 0;
}

//! @brief �f�B�X�g���N�^
HumanMotionControl::~HumanMotionControl()
{}

//! @brief �ݒ�l���Ǘ�����N���X��o�^
//! @attention �e�֐����g�p����O�Ɏ��s���邱�ƁB
void HumanMotionControl::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;
}

//! @brief ���f���f�[�^��ݒ�
//! @param upmodel �㔼�g�̃��f��
//! @param armmodel[] �r�̃��f���̔z��i�z�񐔁FTOTAL_ARMMODE�j
//! @param legmodel ���i�Î~��ԁj�̃��f��
//! @param walkmodel[] �r�̃��f���̔z��i�z�񐔁FTOTAL_WALKMODE�j
//! @param runmodel[] �r�̃��f���̔z��i�z�񐔁FTOTAL_RUNMODE�j
void HumanMotionControl::SetModel(int upmodel, int armmodel[], int legmodel, int walkmodel[], int runmodel[])
{
	id_upmodel = upmodel;
	for(int i=0; i<TOTAL_ARMMODE; i++){
		id_armmodel[i] = armmodel[i];
	}
	id_legmodel = legmodel;
	for(int i=0; i<TOTAL_WALKMODE; i++){
		id_walkmodel[i] = walkmodel[i];
	}
	for(int i=0; i<TOTAL_RUNMODE; i++){
		id_runmodel[i] = runmodel[i];
	}
}

//! @brief �������n�֐�
//! @param rx ��]�p�x
void HumanMotionControl::Init(float rx)
{
	reaction_y = 0.0f;
	armmodel_rotation_y = 0.0f;
	legrotation_x = rx;
	walkcnt = 0;
	runcnt = 0;
}

//! @brief ������E��
//! @param weapon_paramid ����̎�ޔԍ�
void HumanMotionControl::PickupWeapon(int weapon_paramid)
{
	//���g�p�����΍�
	UNREFERENCED_PARAMETER(weapon_paramid);

	//�r�̊p�x�i�����j��ݒ�
	reaction_y = DegreeToRadian(-20);
}

//! @brief �����؂�ւ���i�����ւ���j
//! @param weapon_paramid ����̎�ޔԍ�
void HumanMotionControl::ChangeWeapon(int weapon_paramid)
{
	//���g�p�����΍�
	UNREFERENCED_PARAMETER(weapon_paramid);

	//�r�̊p�x�i�����j��ݒ�
	reaction_y = DegreeToRadian(-20);
}

//! @brief ���C����
//! @param weapon_paramid ����̎�ޔԍ�
void HumanMotionControl::ShotWeapon(int weapon_paramid)
{
	//�r�ɔ�����`����
	if( weapon_paramid == ID_WEAPON_GRENADE ){
		reaction_y = DegreeToRadian(20);
	}
	else{
		//����̐ݒ�l���擾
		WeaponParameter ParamData;
		if( Param->GetWeapon(weapon_paramid, &ParamData) != 0 ){ return; }

		reaction_y = DegreeToRadian(0.5f) * ParamData.reaction;
	}
}

//! @brief �����[�h
//! @param weapon_paramid ����̎�ޔԍ�
//! @attention ���̊֐��͊g���p�̃_�~�[�֐��ł��B
void HumanMotionControl::ReloadWeapon(int weapon_paramid)
{
	//���g�p�����΍�
	UNREFERENCED_PARAMETER(weapon_paramid);
}

//! @brief ������̂Ă�
//! @attention ���̊֐��͊g���p�̃_�~�[�֐��ł��B
void HumanMotionControl::DumpWeapon()
{
	//
}

//! @brief ����̃V���b�g���[�h�؂�ւ�
//! @param weapon_paramid ����̎�ޔԍ�
//! @attention ���̊֐��͊g���p�̃_�~�[�֐��ł��B
void HumanMotionControl::ChangeShotMode(int weapon_paramid)
{
	//���g�p�����΍�
	UNREFERENCED_PARAMETER(weapon_paramid);
}

//! @brief �W�����v
//! @attention ���̊֐��͊g���p�̃_�~�[�֐��ł��B
void HumanMotionControl::Jump()
{
	//
}

//! @brief ���[�V�����v�Z�����s
//! @attention rotation_x �̑S�̂̉�]�p�x
//! @attention armrotation_y �r�̉�]�p�x
//! @attention weapon_paramid ����̎�ޔԍ�
//! @attention ReloadCnt ����̃����[�h�J�E���g
//! @attention MoveFlag �ړ�������\���t���O
//! @attention hp �̗�
//! @attention PlayerFlag �v���C���[���ǂ���
void HumanMotionControl::RunFrame(float rotation_x, float armrotation_y, int weapon_paramid, int ReloadCnt, int MoveFlag, int hp, bool PlayerFlag)
{
	int ArmModelID;
	float move_rx;

	//���C�Ȃǂɂ�锽��
	if( reaction_y > 0.0f ){
		if( reaction_y > DegreeToRadian(2) ){ reaction_y -= DegreeToRadian(2); }
		else{ reaction_y = 0.0f; }
	}
	if( reaction_y < 0.0f ){
		if( reaction_y < DegreeToRadian(2) ){ reaction_y += DegreeToRadian(2); }
		else{ reaction_y = 0.0f; }
	}

	//�����[�h���Ȃ�r�̊p�x���Đݒ�
	if( ReloadCnt > 0 ){
		reaction_y = ARMRAD_RELOADWEAPON;
	}

	if( weapon_paramid == ID_WEAPON_NONE ){	//��Ԃ�
		if( PlayerFlag == true ){
			armmodel_rotation_y = ARMRAD_NOWEAPON;
		}
		else{
			armmodel_rotation_y = armrotation_y;
		}
		ArmModelID = 0;
	}
	else{							//��������������Ă���
		//����̏����擾
		WeaponParameter paramdata;
		Param->GetWeapon(weapon_paramid, &paramdata);

		armmodel_rotation_y = 0.0f;
		ArmModelID = 0;

		//�r�̌`�Ɗp�x������
		if( paramdata.WeaponP == 0 ){
			armmodel_rotation_y = armrotation_y + reaction_y;
			ArmModelID = 1;
		}
		if( paramdata.WeaponP == 1 ){
			armmodel_rotation_y = armrotation_y + reaction_y;
			ArmModelID = 2;
		}
		if( paramdata.WeaponP == 2 ){
			armmodel_rotation_y = ARMRAD_NOWEAPON;
			ArmModelID = 0;
		}
	}

	//�i�s����������
	if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
		move_rx = DegreeToRadian(0);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_FORWARD ){
		move_rx = DegreeToRadian(0);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_BACK ){
		move_rx = DegreeToRadian(180);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_LEFT ){
		move_rx = DegreeToRadian(90);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_RIGHT ){
		move_rx = DegreeToRadian(-90);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_FORWARD | MOVEFLAG_LEFT) ){
		move_rx = DegreeToRadian(45);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_BACK | MOVEFLAG_LEFT) ){
		move_rx = DegreeToRadian(135);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_BACK | MOVEFLAG_RIGHT) ){
		move_rx = DegreeToRadian(-135);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_FORWARD | MOVEFLAG_RIGHT) ){
		move_rx = DegreeToRadian(-45);
	}
	else{
		move_rx = 0.0f;
	}

	//���̊p�x���Z�o
	if( hp <= 0 ){
		legrotation_x = rotation_x;
	}
	else{
		float move_rx2;

		//���̌��������߂�
		if( fabs(move_rx) > DegreeToRadian(90)){
			move_rx2 = move_rx + (float)M_PI;
		}
		else{
			move_rx2 = move_rx;
		}
		for(; move_rx2 > (float)M_PI; move_rx2 -= (float)M_PI*2){}
		for(; move_rx2 < (float)M_PI*-1; move_rx2 += (float)M_PI*2){}

		//���X�ɂ��̌�����
		legrotation_x = legrotation_x*0.85f + (rotation_x + move_rx2*-1)*0.15f;		// 3/4 + 1/4
	}

	//�r�̃��f���ݒ�
	armmodelid = id_armmodel[ArmModelID];

	//���̃��f���ݒ�
	legmodelid = id_legmodel;	//�����~�܂�
	if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
		legmodelid = id_walkmodel[ (walkcnt/3 % TOTAL_WALKMODE) ];	//����
		walkcnt += 1;
		runcnt = 0;
	}
	else{
		if( GetFlag(MoveFlag, (MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
			legmodelid = id_runmodel[ (runcnt/3 % TOTAL_RUNMODE) ];		//���E����
		}
		if( GetFlag(MoveFlag, MOVEFLAG_FORWARD) ){
			legmodelid = id_runmodel[ (runcnt/2 % TOTAL_RUNMODE) ];		//�O����
		}
		if( GetFlag(MoveFlag, MOVEFLAG_BACK) ){
			legmodelid = id_runmodel[ (runcnt/4 % TOTAL_RUNMODE) ];		//��둖��
		}
		walkcnt = 0;
		runcnt += 1;
	}
}

//! @brief ���[�V�����擾
//! @param arm_rotation_y �r�̊p�x���擾����|�C���^
//! @param leg_rotation_x ���̊p�x���擾����|�C���^
//! @param upmodel �㔼�g�̃��f���F���ԍ����擾����|�C���^
//! @param armmodel �r�̃��f���F���ԍ����擾����|�C���^
//! @param legmodel ���̃��f���F���ԍ����擾����|�C���^
void HumanMotionControl::GetRenderMotion(float *arm_rotation_y, float *leg_rotation_x, int *upmodel, int *armmodel, int *legmodel)
{
	*arm_rotation_y = armmodel_rotation_y;
	*leg_rotation_x = legrotation_x;

	*upmodel = id_upmodel;
	*armmodel = armmodelid;
	*legmodel = legmodelid;
}