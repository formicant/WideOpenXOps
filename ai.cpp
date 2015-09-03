//! @file ai.cpp
//! @brief AIcontrol�N���X�̒�`

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

#include "ai.h"

//! @brief �R���X�g���N�^
AIcontrol::AIcontrol(class ObjectManager *in_ObjMgr, int in_ctrlid, class BlockDataInterface *in_blocks, class PointDataInterface *in_Points, class ParameterInfo *in_Param, class Collision *in_CollD, class SoundManager *in_GameSound)
{
	ObjMgr = in_ObjMgr;
	ctrlid = in_ctrlid;
	ctrlhuman = in_ObjMgr->GeHumanObject(in_ctrlid);
	blocks = in_blocks;
	Points = in_Points;
	Param = in_Param;
	CollD = in_CollD;
	GameSound = in_GameSound;

	battlemode = AI_NORMAL;
	movemode = AI_WAIT;
	addrx = 0.0f;
	addry = 0.0f;
	target_pointid = -1;
	target_posx = 0.0f;
	target_posz = 0.0f;
	target_rx = 0.0f;
	total_move = 0.0f;
	waitcnt = 0;
	movejumpcnt = 1*((int)GAMEFPS);
	gotocnt = 0;
	moveturn_mode = 0;
	longattack = false;
}

//! @brief �f�B�X�g���N�^
AIcontrol::~AIcontrol()
{}

//! @brief �ΏۃN���X��ݒ�
//! @attention ���̊֐��Őݒ���s��Ȃ��ƁA�N���X���̂��������@�\���܂���B
void AIcontrol::SetClass(class ObjectManager *in_ObjMgr, int in_ctrlid, class BlockDataInterface *in_blocks, class PointDataInterface *in_Points, class ParameterInfo *in_Param, class Collision *in_CollD, class SoundManager *in_GameSound)
{
	ObjMgr = in_ObjMgr;
	ctrlid = in_ctrlid;
	ctrlhuman = in_ObjMgr->GeHumanObject(in_ctrlid);
	blocks = in_blocks;
	Points = in_Points;
	Param = in_Param;
	CollD = in_CollD;
	GameSound = in_GameSound;
}

//! @brief �l������
//! @param in_p4 ��������l�̑�4�p�����[�^�i�F���ԍ��j
//! @param out_x X���W���󂯎��|�C���^
//! @param out_z Z���W���󂯎��|�C���^
//! @return �����F1�@���s�F0
int AIcontrol::SearchHumanPos(signed char in_p4, float *out_x, float *out_z)
{
	float x, z;
	human* thuman;

	//�l���������ăN���X���擾
	thuman = ObjMgr->SearchHuman(in_p4);
	if( thuman == NULL ){ return 0; }

	//X�EZ���W���擾
	thuman->GetPosData(&x, NULL, &z, NULL);
	*out_x = x;
	*out_z = z;
	return 1;
}

//! @brief �ڕW�n�_�Ɉړ����Ă��邩�m�F
//! @return ���B�Ftrue�@�񓞒B�Ffalse
bool AIcontrol::CheckTargetPos()
{
	//�������Z�o
	float x = posx - target_posx;
	float z = posz - target_posz;
	float r = x * x + z * z;

	if( movemode == AI_TRACKING ){	//�ǔ����Ȃ�
		if( r < AI_ARRIVALDIST_TRACKING * AI_ARRIVALDIST_TRACKING ){
			return true;
		}
	}
	else{							//����ȊO�Ȃ�
		if( r < (AI_ARRIVALDIST_PATH * AI_ARRIVALDIST_PATH) ){
			return true;
		}
	}

	return false;
}

//! @brief �ڕW�n�_������
//! @param next ������������
//! @return �����Ftrue�@���s�Ffalse
bool AIcontrol::SearchTarget(bool next)
{
	//�|�C���g�̏����擾
	pointdata pdata;
	if( Points->Getdata(&pdata, target_pointid) != 0 ){
		movemode = AI_NULL;
		return false;
	}

	signed char nextpointp4;

	//���̃|�C���g����������Ȃ�
	if( next == true ){
		nextpointp4 = pdata.p3;

		//�����_���p�X����
		if( pdata.p1 == 8 ){
			if( GetRand(2) == 0 ){
				nextpointp4 = pdata.p2;
			}
			else{
				nextpointp4 = pdata.p3;
			}
			movemode = AI_RANDOM;
		}

		//�|�C���g������
		if( Points->SearchPointdata(&pdata, 0x08, 0, 0, 0, nextpointp4, 0) == 0 ){
			return false;
		}
	}

	//�����_���p�X�Ȃ玟��
	if( pdata.p1 == 8 ){
		target_pointid = pdata.id;
		movemode = AI_RANDOM;
		return false;
	}

	//�l�Ȃ���W���擾
	if( (pdata.p1 == 1)||(pdata.p1 == 6) ){
		SearchHumanPos(pdata.p4, &target_posx, &target_posz);
		return true;
	}

	//�ړ��p�X�Ȃ�`
	if( pdata.p1 == 3 ){
		//���K�p
		target_pointid = pdata.id;
		target_posx = pdata.x;
		target_posz = pdata.z;
		target_rx = pdata.r;

		//�ړ��X�e�[�g�ݒ�
		switch(pdata.p2){
			case 0: movemode = AI_WALK; break;
			case 1: movemode = AI_RUN; break;
			case 2: movemode = AI_WAIT; break;
			case 3:
				movemode = AI_TRACKING;
				if( next == true ){
					nextpointp4 = pdata.p3;

					//�|�C���g�i�l�j�̏����擾
					if( Points->SearchPointdata(&pdata, 0x08, 0, 0, 0, nextpointp4, 0) == 0 ){
						return false;
					}

					//���ۑ�
					target_pointid = pdata.id;
					target_posx = pdata.x;
					target_posz = pdata.z;
				}
				break;
			case 4: movemode = AI_WAIT; break;
			case 5: movemode = AI_STOP; break;
			case 6: movemode = AI_GRENADE; break;
			case 7: movemode = AI_RUN2; break;
			default: break;
		}
		return true;
	}
	
	movemode = AI_NULL;
	return false;
}

//! @brief �ڕW�n�_�Ɉړ�
void AIcontrol::MoveTarget()
{
	float r, atan;
	int paramid;
	HumanParameter Paraminfo;
	bool zombie;

	//�]���r���ǂ�������
	ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
	Param->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 2 ){
		zombie = true;
	}
	else{
		zombie = false;
	}

	//��x�S�Ă̓������~�߂�
	moveturn_mode = 0;

	//�ڕW�n�_�ւ̊p�x�����߂�
	CheckTargetAngle(posx, 0.0f, posz, rx*-1 + (float)M_PI/2, 0.0f, target_posx, 0.0f, target_posz, 0.0f, &atan, NULL, &r);

	//����
	if( atan > DegreeToRadian(0.5f) ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}
	if( atan < DegreeToRadian(-0.5f) ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}

	//�O�i����
	if( zombie == true ){
		if( fabs(atan) < DegreeToRadian(20) ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
		}
	}
	else if( battlemode == AI_CAUTION ){
		if( fabs(atan) < DegreeToRadian(50) ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
		}
	}
	else if( movemode == AI_WALK ){
		if( fabs(atan) < DegreeToRadian(6) ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
		}
	}
	else if( (movemode == AI_RUN)||(movemode == AI_RUN2) ){
		if( fabs(atan) < DegreeToRadian(50) ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
		}
	}
	else if( (movemode == AI_WAIT)||(movemode == AI_STOP) ){
		if( fabs(atan) < DegreeToRadian(6) ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
		}
	}
	else{	//movemode == AI_TRACKING
		if( fabs(atan) < DegreeToRadian(20) ){
			if( r < (AI_ARRIVALDIST_WALKTRACKING * AI_ARRIVALDIST_WALKTRACKING) ){
				SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
			}
			else{
				SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			}
		}
	}

	//�W�����v
	if( GetRand(16) == 0 ){
		MoveJump();
	}

	//�����|�����Ă�����A���E�ւ̉�]�������_���ɍs��
	if( GetRand(28) == 0 ){
		if( ctrlhuman->GetMovemode(true) != 0 ){
			if( ctrlhuman->GetTotalMove() - total_move < 0.1f ){
				if( GetRand(2) == 0 ){ SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT); }
				else{ SetFlag(moveturn_mode, AI_CTRL_TURNLEFT); }
			}
		}
	}
	total_move = ctrlhuman->GetTotalMove();
}

//! @brief �ڕW�n�_�Ɉړ��i�D��I�ȑ���p�j
void AIcontrol::MoveTarget2()
{
	float atan;

	//�ڕW�n�_�ւ̊p�x�����߂�
	CheckTargetAngle(posx, 0.0f, posz, rx*-1 + (float)M_PI/2, 0.0f, target_posx, 0.0f, target_posz, 0.0f, &atan, NULL, NULL);

	//�O��ړ��̏���
	if( fabs(atan) < DegreeToRadian(56) ){
		SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
	}
	if( fabs(atan) > DegreeToRadian(123.5f) ){
		SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
	}

	//���E�ړ��̏���
	if( (DegreeToRadian(-146) < atan)&&(atan < DegreeToRadian(-33)) ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( (DegreeToRadian(33) < atan)&&(atan < DegreeToRadian(146)) ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}

	//�W�����v
	if( GetRand(16) == 0 ){
		MoveJump();
	}

	//�����|�����Ă�����A���E�ւ̉�]�������_���ɍs��
	if( GetRand(28) == 0 ){
		if( ctrlhuman->GetMovemode(true) != 0 ){
			if( ctrlhuman->GetTotalMove() - total_move < 0.1f ){
				if( GetRand(2) == 0 ){ SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT); }
				else{ SetFlag(moveturn_mode, AI_CTRL_TURNLEFT); }
			}
		}
	}
	total_move = ctrlhuman->GetTotalMove();
}

//! @brief �O�㍶�E�����_���Ɉړ��i�U�����p�j
void AIcontrol::MoveRandom()
{
	int forwardstart, backstart, sidestart;

	if( longattack == false ){
		forwardstart = 80;
		backstart = 90;
		sidestart = 70;
	}
	else{
		forwardstart = 120;
		backstart = 150;
		sidestart = 130;
	}

	//�����_���Ɉړ����n�߂�
	if( GetRand(forwardstart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
	}
	if( GetRand(backstart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
	}
	if( GetRand(sidestart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVELEFT);
	}
	if( GetRand(sidestart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
	}

	//����������Ă��炸�A��Ԃ�Ȃ��
	if( ctrlhuman->GetMainWeaponTypeNO() == ID_WEAPON_NONE ){
		// 1/80�̊m���ŉ�����n�߂�
		if( GetRand(80) == 0 ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
		}
	}

	// 1/3�̊m�����A�ړ��t���O���ݒ肳��Ă�����
	if( (GetRand(3) == 0)||(GetFlag(moveturn_mode, (AI_CTRL_MOVEFORWARD | AI_CTRL_MOVEBACKWARD | AI_CTRL_MOVELEFT | AI_CTRL_MOVERIGHT))) ){
		float vx, vz;
		float Dist;

		if( GetRand(2) == 0 ){
			//�O�����̃x�N�g�����v�Z
			vx = cos(rx*-1 + (float)M_PI/2);
			vz = sin(rx*-1 + (float)M_PI/2);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGTH, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//���̍����Ƀu���b�N������i�Ԃ���j
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 1.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//�����Ƀu���b�N���Ȃ��i������j
			){
				//�O�i�t���O���폜���A��ރt���O��ݒ�
				DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
				SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
			}

			//������̃x�N�g�����v�Z
			vx = cos(rx*-1 + (float)M_PI/2 + (float)M_PI);
			vz = sin(rx*-1 + (float)M_PI/2 + (float)M_PI);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGTH, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//���̍����Ƀu���b�N������i�Ԃ���j
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 1.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//�����Ƀu���b�N���Ȃ��i������j
			){
				//��ރt���O���폜���A�O�i�t���O��ݒ�
				DelFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
				SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			}
		}
		else{
			//
			vx = cos(rx*-1);
			vz = sin(rx*-1);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGTH, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//���̍����Ƀu���b�N������i�Ԃ���j
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 1.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//�����Ƀu���b�N���Ȃ��i������j
			){
				//�E�ړ��t���O���폜���A���ړ��t���O��ݒ�
				DelFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
				SetFlag(moveturn_mode, AI_CTRL_MOVELEFT);
			}

			vx = cos(rx*-1 + (float)M_PI);
			vz = sin(rx*-1 + (float)M_PI);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGTH, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//���̍����Ƀu���b�N������i�Ԃ���j
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 1.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//�����Ƀu���b�N���Ȃ��i������j
			){
				//���ړ��t���O���폜���A�E�ړ��t���O��ݒ�
				DelFlag(moveturn_mode, AI_CTRL_MOVELEFT);
				SetFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
			}
		}
	}

	//�U���Ώۂ�����Ȃ�
	if( enemyhuman != NULL ){
		float tx, ty, tz;

		enemyhuman->GetPosData(&tx, &ty, &tz, NULL);

		float x = posx - tx;
		float y = posy - ty;
		float z = posz - tz;
		float r = x * x + y * y + z * z;

		//�G�ɋ߂Â��������Ȃ��ނ���
		if( r < 20.0f * 20.0f ){
			DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			if( GetRand(70) == 0 ){
				SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
			}
		}
	}
}

//! @brief ���̏�����܂킷
void AIcontrol::TurnSeen()
{
	int turnstart, turnstop;

	//��]�̊J�n�E�I���m����ݒ�
	if( battlemode == AI_ACTION ){
		return;
	}
	else if( battlemode == AI_CAUTION ){
		turnstart = 20;
		turnstop = 20;
	}
	else{
		if( movemode == AI_TRACKING ){ turnstart = 65; }
		else{ turnstart = 85; }
		turnstop = 18;
	}

	//�����_���ɉ�]���n�߂�
	if( GetRand(turnstart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( GetRand(turnstart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}

	if( (battlemode == AI_NORMAL)&&(movemode == AI_WAIT) ){
		//�����_���Ƀ|�C���g�̕����������Ƃ���
		//�u�|�C���g�̕����������d������v�̍Č� 
		if( GetRand(80) == 0 ){
			float tr;
			tr = target_rx - rx;
			for(; tr > (float)M_PI; tr -= (float)M_PI*2){}
			for(; tr < (float)M_PI*-1; tr += (float)M_PI*2){}

			if( tr > 0.0f ){
				SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
			}
			if( tr < 0.0f ){
				SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
			}
		}
	}

	//��]�������_���Ɏ~�߂�
	if( GetRand(turnstop) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( GetRand(turnstop) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}
}

//! @brief ����̕�������������
bool AIcontrol::StopSeen()
{
	float tr;
	bool returnflag = false;

	tr = target_rx - rx;
	for(; tr > (float)M_PI; tr -= (float)M_PI*2){}
	for(; tr < (float)M_PI*-1; tr += (float)M_PI*2){}

	DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);

	//����
	if( tr > DegreeToRadian(2.5f) ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
		returnflag = false;
	}
	if( tr < DegreeToRadian(-2.5f) ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
		returnflag = false;
	}

	//��������Ɍ����Ă��邩����
	if( fabs(tr) <= DegreeToRadian(2.5f) ){
		returnflag = true;
	}

	return returnflag;
}

//! @brief �i�s�����ɏ�Q��������΃W�����v����
bool AIcontrol::MoveJump()
{
	//�����~�܂��Ă���Ώ������Ȃ�
	if( ctrlhuman->GetMovemode(false) == 0 ){ return false; }

	float dist_dummy;

	float new_posx, new_posy, new_posz;

	//���t�߂̂����蔻��
	new_posx = posx + cos(rx*-1 + (float)M_PI/2) * (AI_CHECKJUMP_DIST + HUMAN_MAPCOLLISION_R);
	new_posy = posy + HUMAN_MAPCOLLISION_HEIGTH;
	new_posz = posz + sin(rx*-1 + (float)M_PI/2) * (AI_CHECKJUMP_DIST + HUMAN_MAPCOLLISION_R);
	if( CollD->CheckALLBlockInside(new_posx, new_posy, new_posz) == true ){
		ObjMgr->MoveJump(ctrlid);
		return true;
	}

	//�̑S�̂̂����蔻��
	new_posx = posx + cos(rx*-1 + (float)M_PI/2) * AI_CHECKJUMP_DIST;
	new_posy = posy + AI_CHECKJUMP_HEIGHT;
	new_posz = posz + sin(rx*-1 + (float)M_PI/2) * AI_CHECKJUMP_DIST;
	if( CollD->CheckALLBlockInside(new_posx, new_posy, new_posz) == true ){
		ObjMgr->MoveJump(ctrlid);
		return true;
	}
	else if( CollD->CheckALLBlockIntersectRay(new_posx, new_posy, new_posz, 0.0f, 1.0f, 0.0f, NULL, NULL, &dist_dummy, HUMAN_HEIGTH - AI_CHECKJUMP_HEIGHT) == true ){
		ObjMgr->MoveJump(ctrlid);
		return true;
	}

	return false;
}

//! @brief �U��
//! @todo �]���r�̑����߂܂��鏈��
void AIcontrol::Action()
{
	human* EnemyHuman = NULL;
	int paramid;
	HumanParameter Paraminfo;
	bool zombie;
	float posy2;
	float tx, ty, tz;

	EnemyHuman = enemyhuman;

	//���W���擾
	EnemyHuman->GetPosData(&tx, &ty, &tz, NULL);
	posy2 = posy + VIEW_HEIGHT;
	ty += VIEW_HEIGHT;

	//�]���r���ǂ�������
	ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
	Param->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 2 ){
		zombie = true;
	}
	else{
		zombie = false;
	}

	//�������Ă��镐��̎�ނ��擾
	int weaponid = ctrlhuman->GetMainWeaponTypeNO();

	float atanx, atany, r;

	//��������֒e�������Ă���΁`
	if( weaponid == ID_WEAPON_GRENADE ){
		if( zombie == false ){
			float x = posx - tx;
			float z = posz - tz;
			float r = x * x + z * z;
			float scale;
			if( longattack == false ){ scale = 0.12f; }
			else{ scale = 0.4f; }

			//�����ɉ����č�����ς���
			ty += (sqrt(r) - 200.0f) * scale;
		}
	}
	else{
		float mx, mz;
		float scale;
		EnemyHuman->GetMovePos(&mx, NULL, &mz);
		if( longattack == false ){ scale = 1.5f; }
		else{
			float x = posx - tx;
			float z = posz - tz;
			float r = x * x + z * z;

			scale = sqrt(r) * 0.12f;
		}

		//�G�̈ړ���������
		tx += mx * scale;
		tz += mz * scale;
	}

	//�ڕW�n�_�ւ̊p�x�����߂�
	CheckTargetAngle(posx, posy2, posz, rx*-1 + (float)M_PI/2, ry, tx, ty, tz, 0.0f, &atanx, &atany, &r);

	//������ς���^�C�~���O������
	int randr = LevelParam->aiming;
	if( longattack == false ){ randr += 1; }
	else{ randr += 2; }

	if( randr != 0 ){
		//����
		if( atanx > 0.0f ){
			SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
			DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
		}
		if( atanx < 0.0f ){
			SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
			DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
		}

		//�r�̊p�x
		if( zombie == true ){
			//ry = 0.0f;

			//����
			if( ry < 0.0f ){
				SetFlag(moveturn_mode, AI_CTRL_TURNUP);
				DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
			}
			if( ry > 0.0f ){
				SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
				DelFlag(moveturn_mode, AI_CTRL_TURNUP);
			}
		}
		else{
			//��������Ԃ�Ȃ�΁`
			if( weaponid == ID_WEAPON_NONE ){
				if( EnemyHuman->GetMainWeaponTypeNO() == ID_WEAPON_NONE ){	//�G����Ԃ�Ȃ�΁`
					//���Ɍ���������
					SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
					DelFlag(moveturn_mode, AI_CTRL_TURNUP);
				}
				else{														//�G������������Ă���΁`
					//��Ɍ���������
					SetFlag(moveturn_mode, AI_CTRL_TURNUP);
					DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
				}
			}
			else{
				//����
				if( atany > 0.0f ){
					SetFlag(moveturn_mode, AI_CTRL_TURNUP);
					DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
				}
				if( atany < 0.0f ){
					SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
					DelFlag(moveturn_mode, AI_CTRL_TURNUP);
				}
			}
		}
	}

	//�]���r�ȊO�Ŏ�Ԃ�Ȃ��
	if( zombie == false ){
		if( weaponid == ID_WEAPON_NONE ){
			//���̊m���Ō�ނ���
			if( GetRand(80) == 0 ){
				SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
			}
		}
	}

	
	if( zombie == true ){	//�]���r�̍U��
		float y = posy2 - ty;

		//���������Ă���΁A��x�����ɐ؂�ւ���
		if( GetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD) ){
			DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
		}

		//�G�Ɍ������đO�i����
		if( fabs(atanx) <= DegreeToRadian(25) ){
			if( (fabs(atanx) <= DegreeToRadian(15)) && (r < 24.0f*24.0f) && (actioncnt%50 > 20) ){
				//�������������A����
				SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
				DelFlag(moveturn_mode, AI_CTRL_MOVEWALK);
			}
			else{
				SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
			}
		}

		/*
		//�W�����v
		if( GetRand(16) == 0 ){
			MoveJump();
		}
		*/

		if( (r < 9.0f*9.0f)&&( fabs(y) < 10.0f) ){
			float x = posx - tx;
			float z = posz - tz;

			//�߂܂���@�i�G�������t����j
			EnemyHuman->AddPosOrder(atan2(z, x), 0.0f, 0.5f);

			//�G�̎��_�������_���ɓ�����
			float erx, ery;
			EnemyHuman->GetRxRy(&erx, &ery);
			switch(GetRand(3)){
				case 0: erx -= DegreeToRadian(2); break;
				case 1: erx += DegreeToRadian(2); break;
				default: break;
			}
			switch(GetRand(3)){
				case 0: ery -= DegreeToRadian(2); break;
				case 1: ery += DegreeToRadian(2); break;
				default: break;
			}
			EnemyHuman->SetRxRy(erx, ery);
		}

		if( actioncnt%50 == 0){
			if( ObjMgr->CheckZombieAttack(ctrlhuman, EnemyHuman) == true ){
				ObjMgr->HitZombieAttack(EnemyHuman);
			}
		}
	}
	else{					//���C����
		float ShotAngle;
		if( longattack == false ){
			//�G�𑨂����Ɣ��肷��A���e�덷���v�Z����
			ShotAngle = DegreeToRadian(8);
			if( weaponid != ID_WEAPON_NONE ){
				WeaponParameter wparam;
				Param->GetWeapon(weaponid, &wparam);
				if( wparam.scopemode == 1 ){
					ShotAngle = DegreeToRadian(6);
				}
				if( wparam.scopemode == 2 ){
					ShotAngle = DegreeToRadian(4);
				}
			}

			//AI���x�����Ƃɒ���
			ShotAngle += DegreeToRadian(0.5f) * LevelParam->limitserror;

			if( movemode == AI_RUN2 ){
				ShotAngle *= 1.5f;
			}
		}
		else{
			//�G�𑨂����Ɣ��肷��A���e�덷���v�Z����
			ShotAngle = DegreeToRadian(4);
			if( weaponid != ID_WEAPON_NONE ){
				WeaponParameter wparam;
				Param->GetWeapon(weaponid, &wparam);
				if( wparam.scopemode == 1 ){
					ShotAngle = DegreeToRadian(3);
				}
				if( wparam.scopemode == 2 ){
					ShotAngle = DegreeToRadian(2);
				}
			}

			//AI���x�����Ƃɒ���
			ShotAngle += DegreeToRadian(0.2f) * LevelParam->limitserror;
		}

		//�G�𑨂��Ă����
		float atanxy = fabs(atanx) + fabs(atany);
		if( atanxy < ShotAngle ){
			int rand = LevelParam->attack;
			if( longattack == true ){ rand += 1; }

			//���C
			if( GetRand(rand) == 0 ){
				ObjMgr->ShotWeapon(ctrlid);
			}
		}
	}

	//�����ɉ����ċߋ����E��������؂�ւ���
	//�@200.0f�s�b�^���Őݒ�l�ێ�
	if( r < 200.0f * 200.0f ){
		longattack = false;
	}
	if( (r > 200.0f * 200.0f)&&(movemode != AI_RUN2) ){
		longattack = true;
	}

	if( zombie == false ){
		//�����_���Ɉړ�
		MoveRandom();
	}

	actioncnt += 1;
}

//! @brief �U�����L�����Z��
bool AIcontrol::ActionCancel()
{
	//��퓬���t���O���L���Ȃ�I��
	if( NoFight == true ){
		return true;
	}

	//�G�����S������I��
	if( enemyhuman->GetDeadFlag() == true ){
		return true;
	}

	//�������擾
	float tx, ty, tz;
	enemyhuman->GetPosData(&tx, &ty, &tz, NULL);
	float x = posx - tx;
	float y = posy - ty;
	float z = posz - tz;
	float r = x*x + y*y + z*z;

	//����������߂��Ă�����I��
	if( r > 620.0f*620.0f ){
		return true;
	}

	if( longattack == false ){
		//�K���ȃ^�C�~���O�œG�������邩�m�F
		if( GetRand(40) == 0 ){
			//�u���b�N���Ղ��Ă����i�������Ȃ��j�Ȃ�ΏI��
			if( CheckLookEnemy(enemyhuman, AI_SEARCH_RX, AI_SEARCH_RY, 620.0f, NULL) == false ){
				return true;
			}
		}

		//�����I�ɏI��
		if( GetRand(550) == 0 ){
			return true;
		}
	}
	else{
		//�K���ȃ^�C�~���O�œG�������邩�m�F
		if( GetRand(30) == 0 ){
			//�u���b�N���Ղ��Ă����i�������Ȃ��j�Ȃ�ΏI��
			if( CheckLookEnemy(enemyhuman, AI_SEARCH_RX, AI_SEARCH_RY, 620.0f, NULL) == false ){
				return true;
			}
		}

		//�����I�ɏI��
		if( GetRand(450) == 0 ){
			return true;
		}
	}

	return false;
}

//! @brief ���������
int AIcontrol::HaveWeapon()
{
	int selectweapon;
	class weapon *weapon[TOTAL_HAVEWEAPON];
	int nbs;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}

	//����̏����擾
	ctrlhuman->GetWeapon(&selectweapon, weapon);

	//����������Ă���΁A���v�e�����擾
	if( weapon[selectweapon] != NULL ){
		weapon[selectweapon]->GetParamData(NULL, NULL, &nbs);
	}

	//����������Ă��Ȃ����A�e��0�Ȃ��
	if( (weapon[selectweapon] == NULL)||(nbs == 0) ){
		//���̕�����w��
		int notselectweapon = selectweapon + 1;
		if( notselectweapon == TOTAL_HAVEWEAPON ){ notselectweapon = 0; }

		//���̕���������Ă����
		if( weapon[notselectweapon] != NULL ){
			//���̕���̒e����0�łȂ����
			weapon[notselectweapon]->GetParamData(NULL, NULL, &nbs);
			if( nbs > 0 ){
				//�����ւ���
				ObjMgr->ChangeWeapon(ctrlid);
				return 1;
			}
		}
	}

	return 0;
}

//! @brief �ړ�������]���������_���ɏI��
void AIcontrol::CancelMoveTurn()
{
	int forward, back, side, updown, rightleft;

	if( battlemode == AI_ACTION ){			//�U����
		if( movemode == AI_RUN2 ){				//�D��I�ȑ���
			DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			DelFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
			DelFlag(moveturn_mode, AI_CTRL_MOVELEFT);
			DelFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
			DelFlag(moveturn_mode, AI_CTRL_TURNUP);
			DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
			DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
			DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
			if( GetRand(3) == 0 ){
				DelFlag(moveturn_mode, AI_CTRL_MOVEWALK);
			}
			return;
		}
		else{									//�D��I�ȑ��� �ȊO
			if( longattack == false ){
				forward = 6;
				back = 6;
				side = 7;
				updown = 5;
				rightleft = 6;
			}
			else{
				forward = 5;
				back = 4;
				side = 5;
				updown = 3;
				rightleft = 3;
			}
		}
	}
	else if( battlemode == AI_CAUTION ){	//�x����
		forward = 10;
		back = 10;
		side = 10;
		updown = 14;
		rightleft = 20;
	}
	else{
		forward = 12;
		back = 12;
		side = 12;
		updown = 15;
		rightleft = 18;
	}

	//�ړ��������_���Ɏ~�߂�
	if( GetRand(forward) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
	}
	if( GetRand(back) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
	}
	if( GetRand(side) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVELEFT);
	}
	if( GetRand(side) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
	}
	if( GetRand(3) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVEWALK);
	}

	//��]�������_���Ɏ~�߂�
	if( GetRand(updown) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNUP);
	}
	if( GetRand(updown) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
	}
	if( GetRand(rightleft) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}
	if( GetRand(rightleft) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
}

//! @brief �ړ�������]�������s
void AIcontrol::ControlMoveTurn()
{
	//�ړ��̎��s
	if( GetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD) ){
		ObjMgr->MoveForward(ctrlid);
	}
	if( GetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD) ){
		ObjMgr->MoveBack(ctrlid);
	}
	if( GetFlag(moveturn_mode, AI_CTRL_MOVELEFT) ){
		ObjMgr->MoveLeft(ctrlid);
	}
	if( GetFlag(moveturn_mode, AI_CTRL_MOVERIGHT) ){
		ObjMgr->MoveRight(ctrlid);
	}
	if( GetFlag(moveturn_mode, AI_CTRL_MOVEWALK) ){
		ObjMgr->MoveWalk(ctrlid);
	}

	//�����]���̎��s�i��]���x�̉��Z�j
	if( GetFlag(moveturn_mode, AI_CTRL_TURNUP) ){
		addry += AI_ADDTURNRAD;
	}
	if( GetFlag(moveturn_mode, AI_CTRL_TURNDOWN) ){
		addry -= AI_ADDTURNRAD;
	}
	if( GetFlag(moveturn_mode, AI_CTRL_TURNLEFT) ){
		addrx -= AI_ADDTURNRAD;
	}
	if( GetFlag(moveturn_mode, AI_CTRL_TURNRIGHT) ){
		addrx += AI_ADDTURNRAD;
	}

	//�p�x�ɉ��Z
	rx += addrx;
	ry += addry;

	//��]���x�̌���
	addrx *= 0.8f;
	addry *= 0.8f;

	//0.0f�֕␳
	if( fabs(addrx) < DegreeToRadian(0.2f) ){ addrx = 0.0f; }
	if( fabs(addry) < DegreeToRadian(0.2f) ){ addry = 0.0f; }

	//�c�̉�]�͈͂����߂�
	if( ry > DegreeToRadian(70) ){ ry = DegreeToRadian(70); }
	if( ry < DegreeToRadian(-70) ){ ry = DegreeToRadian(-70); }
}

//! @brief ����������[�h�E�̂Ă�
//! @return �̂Ă�F1�@�����[�h�F2�@�����ւ���F3�@FULL/SEMI�؂�ւ��F4�@�������Ȃ��F0
int AIcontrol::ControlWeapon()
{
	int selectweapon;
	class weapon *weapon[TOTAL_HAVEWEAPON];
	int weaponid, lnbs, nbs;
	WeaponParameter paramdata;
	bool blazingmodeS, blazingmodeN;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}

	//����̏����擾
	ctrlhuman->GetWeapon(&selectweapon, weapon);
	if( weapon[selectweapon] == NULL ){ return 0; }
	weapon[selectweapon]->GetParamData(&weaponid, &lnbs, &nbs);

	//����̐��\���擾
	if( Param->GetWeapon(weaponid, &paramdata) == 1 ){ return 0; }

	//�X�R�[�v����
	ctrlhuman->SetDisableScope();

	//�u�P�[�X�v�Ȃ牽�����Ȃ�
	if( weaponid == ID_WEAPON_CASE ){ return 0; }

	//�퓬���ɃO���l�[�h�������Ă����
	if( battlemode == AI_ACTION ){
		if( weaponid == ID_WEAPON_GRENADE ){
			int nextselectweapon, nextnds;

			//���̕���̒e�����擾
			nextselectweapon = selectweapon + 1;
			nextnds = 0;
			if( nextselectweapon == TOTAL_HAVEWEAPON ){
				nextselectweapon = 0;
			}
			if( weapon[nextselectweapon] != NULL ){
				weapon[nextselectweapon]->GetParamData(NULL, NULL, &nextnds);
			}

			if( longattack == false ){
				// 1/100�̊m���Ŏ����ւ���
				if( (GetRand(100) == 0)&&(nextnds > 0) ){
					ObjMgr->ChangeWeapon(ctrlid);
					return 3;
				}
			}
			else{
				// 1/66�̊m���Ŏ����ւ���
				if( (GetRand(66) == 0)&&(nextnds > 0) ){
					ObjMgr->ChangeWeapon(ctrlid);
					return 3;
				}
			}
		}
	}

	//�c�e�����������
	if( lnbs == 0 ){
		int ways;

		//�����m������
		if( battlemode == AI_NORMAL ){ ways = 1; }
		else if( battlemode == AI_CAUTION ){ ways = 10; }
		else{ ways = 8; }

		// 1/ways�̊m���ŏ���
		if( GetRand(ways) == 0 ){
			int under;

			//�����[�h�m��
			if( battlemode == AI_NORMAL ){
				ways = 1;
				under = 0;
			}
			else if( battlemode == AI_CAUTION ){
				ways = 5;
				under = 3;
			}
			else{
				if( longattack == false ){
					// �m���́@3/4
					ways = 4;
					under = 2;
				}
				else{
					// �m���́@2/3
					ways = 3;
					under = 1;
				}
			}

			//�e��������Ύ̂Ă�
			if( nbs == 0 ){
				ObjMgr->DumpWeapon(ctrlid);
				return 1;
			}

			//�����_���� �����[�h���s or �������������
			if( GetRand(ways) <= under ){
				ObjMgr->ReloadWeapon(ctrlid);
				return 2;
			}
			//else{
				ObjMgr->ChangeWeapon(ctrlid);
				return 3;
			//}
		}
	}

	//�A�ː؂�ւ����\�ȕ���Ȃ�
	if( paramdata.ChangeWeapon != -1 ){
		//���݂̕���̘A�ːݒ���擾
		blazingmodeS = paramdata.blazingmode;

		//�V���ȕ���̘A�ːݒ���擾
		if( Param->GetWeapon(paramdata.ChangeWeapon, &paramdata) == 1 ){ return 0; }
		blazingmodeN = paramdata.blazingmode;

		if( longattack == false ){
			//�ߋ����U�����ŁA����SEMI�E�؂�ւ����FULL�ɂȂ�Ȃ�
			if( (blazingmodeS == false)||(blazingmodeN == true) ){
				ObjMgr->ChangeShotMode(ctrlid);	//�؂�ւ���
				return 4;
			}
		}
		else{
			//�������U�����ŁA����FULL�E�؂�ւ����SEMI�ɂȂ�Ȃ�
			if( (blazingmodeS == true)||(blazingmodeN == false) ){
				ObjMgr->ChangeShotMode(ctrlid);	//�؂�ւ���
				return 4;
			}
		}
	}

	return 0;
}

//! @brief ��֒e�𓊂���
//! @return �������F0�@�����I������F1�@��֒e�������Ă��Ȃ��F2
//! @attention ��֒e�𓊂���p�X�ɉ����������ł��B�G�������̍U����Action()�֐����Q�Ƃ��Ă��������B
int AIcontrol::ThrowGrenade()
{
	int selectweapon;
	class weapon *weapon[TOTAL_HAVEWEAPON];
	int weaponid, nbs, i;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}

	//��֒e�������Ă��邩�m�F
	ctrlhuman->GetWeapon(&selectweapon, weapon);
	for(i=0; i<TOTAL_HAVEWEAPON; i++){
		if( weapon[i] != NULL ){
			weapon[i]->GetParamData(&weaponid, NULL, &nbs);
			if( weaponid == ID_WEAPON_GRENADE ){
				break;
			}
		}
	}
	if( i == TOTAL_HAVEWEAPON ){
		return 2;
	}

	//��֒e�������Ă��Ȃ���΁A�؂�ւ���
	if( i != selectweapon ){
		ObjMgr->ChangeWeapon(ctrlid, i);
	}

	pointdata pdata;
	float posy2;
	float atan_rx, atan_ry;

	//�p�X�Ɛl�̍������擾
	Points->Getdata(&pdata, target_pointid);
	posy2 = posy + VIEW_HEIGHT;

	//��x�S�Ă̓������~�߂�
	moveturn_mode = 0;

	//�ڕW�n�_�ւ̊p�x�����߂�
	CheckTargetAngle(posx, posy2, posz, rx*-1 + (float)M_PI/2, ry, pdata.x, pdata.y, pdata.z, 0.0f, &atan_rx, &atan_ry, NULL);

	//����
	if( atan_rx > 0.0f ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}
	if( atan_rx < 0.0f ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( atan_ry > 0.0f ){
		SetFlag(moveturn_mode, AI_CTRL_TURNUP);
	}
	if( atan_ry < 0.0f ){
		SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
	}

	//������
	if( (fabs(atan_rx) < DegreeToRadian(1.5f))&&(fabs(atan_ry) < DegreeToRadian(1.5f)) ){
		//�p�x��ݒ�
		ctrlhuman->SetRxRy(rx, ry);

		if( ObjMgr->ShotWeapon(ctrlid) != 0 ){
			return 1;
		}
	}

	return 0;
}

//! @brief �r�̊p�x��ݒ�
void AIcontrol::ArmAngle()
{
	DelFlag(moveturn_mode, AI_CTRL_TURNUP);
	DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);

	if( ctrlhuman->GetMainWeaponTypeNO() == ID_WEAPON_NONE ){	//��Ԃ�
		//���Ɍ���������
		SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
	}
	else if( (battlemode == AI_CAUTION)&&(cautioncnt > 0) ){	//�x����
		float addry2 = 0.0f - ry;

		//����
		if( addry2 > DegreeToRadian(1.0f) ){
			SetFlag(moveturn_mode, AI_CTRL_TURNUP);
		}
		if( addry2 < DegreeToRadian(-1.0f) ){
			SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
		}
	}
	else{									//���펞�ŕ��폊�L��
		//����
		if( ry < DegreeToRadian(-32) ){
			SetFlag(moveturn_mode, AI_CTRL_TURNUP);
		}
		if( ry > DegreeToRadian(-28) ){
			SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
		}
	}
}

//! @brief �G��T��
int AIcontrol::SearchEnemy()
{
	//��퓬���t���O���L���Ȃ�G�������Ȃ�
	if( NoFight == true ){
		return 0;
	}

	if( battlemode == AI_ACTION ){ return 0; }

	int weaponid = ctrlhuman->GetMainWeaponTypeNO();
	int weaponscope = 0;
	int searchloops;
	float maxDist;
	float A_rx, A_ry, B_rx, B_ry;

	//����������Ă���΁A�X�R�[�v���[�h���擾
	if( weaponid != ID_WEAPON_NONE ){
		WeaponParameter Wparam;

		Param->GetWeapon(weaponid, &Wparam);
		weaponscope = Wparam.scopemode;
	}

	//�G�̒T���񐔂ƒT���͈́i�����Ɗp�x�j��ݒ�
	if( battlemode == AI_NORMAL ){
		searchloops = (LevelParam->search) * AI_TOTALHUMAN_SCALE;

		if( weaponscope == 2 ){ maxDist = 50.0f; }
		if( weaponscope == 1 ){ maxDist = 25.0f; }
		else{ maxDist = 0.0f; }
		maxDist += 12.0f*((LevelParam->search)-2) + 350.0f;
		A_rx = DegreeToRadian(60);
		A_ry = DegreeToRadian(110);
		B_rx = DegreeToRadian(40);
		B_ry = DegreeToRadian(60);
	}
	else {	//battlemode == AI_CAUTION
		searchloops = (LevelParam->search) * AI_TOTALHUMAN_SCALE + 4;

		if( weaponscope == 2 ){ maxDist = 80.0f; }
		if( weaponscope == 1 ){ maxDist = 40.0f; }
		else{ maxDist = 0.0f; }
		maxDist += 15.0f*((LevelParam->search)-2) + 420.0f;
		A_rx = DegreeToRadian(80);
		A_ry = DegreeToRadian(130);
		B_rx = DegreeToRadian(50);
		B_ry = DegreeToRadian(80);
	}

	//�w��񐔁A�G��T��
	for(int i=0; i<searchloops; i++){
		int targetid = GetRand(MAX_HUMAN);

		if( CheckLookEnemy(targetid, A_rx, A_ry, 200.0f, NULL) == true ){
			longattack = false;
			return 1;
		}

		if( CheckLookEnemy(targetid, B_rx, B_ry, maxDist, NULL) == true ){
			if( GetRand(4) == 0 ){
				if( movemode == AI_RUN2 ){ longattack = false; }
				else{ longattack = true; }
				return 2;
			}
		}
	}
	return 0;
}

//! @brief �G��T���i�������U�����ɋߋ�����T���j
int AIcontrol::SearchShortEnemy()
{
	float A_rx, A_ry;
	A_rx = DegreeToRadian(100);
	A_ry = DegreeToRadian(52);

	for(int i=0; i<3; i++){
		int targetid = GetRand(MAX_HUMAN);

		if( CheckLookEnemy(targetid, A_rx, A_ry, 200.0f, NULL) == true ){
			longattack = false;
			return 1;
		}
	}
	return 0;
}

//! @brief �G�������邩����
bool AIcontrol::CheckLookEnemy(int id, float search_rx, float search_ry, float maxDist, float *out_minDist)
{
	if( ObjMgr == NULL ){ return false; }

	//�l�̃I�u�W�F�N�g���擾
	class human* thuman;
	thuman = ObjMgr->GeHumanObject(id);
	if( thuman == NULL ){ return false; }

	//�����֐����I�[�o�[���[�h
	return CheckLookEnemy(thuman, search_rx, search_ry, maxDist, out_minDist);
}

//! @brief �G�������邩����
bool AIcontrol::CheckLookEnemy(class human* thuman, float search_rx, float search_ry, float maxDist, float *out_minDist)
{
	//return false;

	//�����ȃf�[�^�Ȃ�I��
	if( ctrlhuman == NULL ){ return false; }
	if( thuman == NULL ){ return false; }
	if( thuman->GetEnableFlag() == false ){ return false; }
	if( thuman->GetDeadFlag() == true ){ return false; }

	//�����ƓG�̍��W���擾
	int ctrlteam, targetteam;
	float tx, ty, tz;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &ctrlteam);
	thuman->GetParamData(NULL, NULL, NULL, &targetteam);
	thuman->GetPosData(&tx, &ty, &tz, NULL);

	//�����ƓG������l���łȂ����
	if( ctrlteam != targetteam ){
		float mrx, mry;
		float Dist2 , Dist;
		float Dist_dummy; 

		//�����𔻒肵�A�p�x���擾
		if( CheckTargetAngle(posx, posy, posz, rx*-1 + (float)M_PI/2, 0.0f, tx, ty, tz, maxDist, &mrx, &mry, &Dist2) == true ){
			//�p�x��A���E�ɓ����Ă����
			if( (fabs(mrx) < search_rx/2)&&(fabs(mry) < search_ry/2) ){
				float vx, vy, vz;

				Dist = sqrt(Dist2);

				//�x�N�g�����擾
				vx = (tx - posx)/Dist;
				vy = (ty - posy)/Dist;
				vz = (tz - posz)/Dist;

				//�u���b�N���Ղ��Ă��Ȃ���΁@�i���C�œ����蔻����s���A�������Ă��Ȃ���΁j
				if( CollD->CheckALLBlockIntersectRay(posx, posy + VIEW_HEIGHT, posz, vx, vy, vz, NULL, NULL, &Dist_dummy, Dist) == false ){
					if( out_minDist != NULL ){ *out_minDist = Dist; }
					enemyhuman = thuman;
					return true;
				}
			}
		}
	}
	return false;
}

//! @brief ���̂����邩�m�F
bool AIcontrol::CheckCorpse(int id)
{
	//�N���X�ݒ肪����������Ώ������Ȃ�
	if( ObjMgr == NULL ){ return false; }
	if( ctrlhuman == NULL ){ return false; }

	//�^�[�Q�b�g�̃N���X���擾
	class human* thuman;
	thuman = ObjMgr->GeHumanObject(id);
	if( thuman == NULL ){ return false; }
	if( thuman->GetEnableFlag() == false ){ return false; }

	//���S���Ă����
	if( thuman->GetDeadFlag() == true ){

		//�`�[���ԍ����擾
		int ctrlteam, targetteam;
		ctrlhuman->GetParamData(NULL, NULL, NULL, &ctrlteam);
		thuman->GetParamData(NULL, NULL, NULL, &targetteam);

		//�����Ȃ��
		if( ctrlteam == targetteam ){
			float tposx, tposy, tposz;
			float atan;

			//���W���狗�����擾
			thuman->GetPosData(&tposx, &tposy, &tposz, NULL);

			//�����Ɗp�x���v�Z
			if( CheckTargetAngle(posx, posy, posz, rx*-1 + (float)M_PI/2, 0.0f, tposx, tposy, tposz, 22.0f, &atan, NULL, NULL) == true ){
				if( fabs(atan) < DegreeToRadian(40) ){
					return true;
				}
			}
		}
	}
	return false;
}

//! @brief �p�X�ɂ��ړ�
void AIcontrol::MovePath()
{
	if( movemode == AI_NULL ){			//�ُ�ȃp�X
		//
	}
	else if( movemode == AI_GRENADE ){	//��֒e�p�X
		if( ThrowGrenade() != 0 ){
			SearchTarget(true);
		}
	}
	else{								//���̑��p�X
		if( CheckTargetPos() == false ){
			MoveTarget();
		}
		else if( (movemode == AI_WAIT)||(movemode == AI_TRACKING) ){
			TurnSeen();
		}
		else if( (movemode == AI_STOP)&&(waitcnt < ((int)GAMEFPS)*5) ){
			if( StopSeen() == true ){
				waitcnt += 1;
			}
		}
		else{
			waitcnt = 0;
			SearchTarget(true);
		}
	}
}

//! @brief �U�����C������
//! @return �s�ρFfalse�@�ύX�Ftrue
bool AIcontrol::ActionMain()
{
	int newbattlemode = AI_ACTION;

	//�U������
	Action();

	if( movemode == AI_RUN2 ){				//�D��I�ȑ���
		//�ڕW�n�_�ֈړ�
		if( CheckTargetPos() == true ){
			newbattlemode = AI_NORMAL;
			SearchTarget(true);
		}
		else{
			MoveTarget2();
		}
	}
	else{									//�D��I�ȑ��� �ȊO
		//�������U�����Ȃ�A�߂��̓G��T��
		if( longattack == true ){
			SearchShortEnemy();
		}
	}

	//�U���I������
	if( ActionCancel() == true ){
		enemyhuman = NULL;

		if( movemode == AI_RUN2 ){
			newbattlemode = AI_NORMAL;
		}
		else{
			newbattlemode = AI_CAUTION;
			cautioncnt = 160;
		}
	}

	//�ݒ�𔻒�
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! @brief �x�����C������
//! @return �s�ρFfalse�@�ύX�Ftrue
bool AIcontrol::CautionMain()
{
	int newbattlemode = AI_CAUTION;

	//���W�ƃ`�[���ԍ����擾
	int teamid;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &teamid);

	//��e�Ɖ��̏󋵂��擾
	bool HitFlag = ctrlhuman->CheckHit();
	soundlist soundlist[MAX_SOUNDMGR_LIST];
	int soundlists = GameSound->GetWorldSound(posx, posy + VIEW_HEIGHT, posz, teamid, soundlist);

	//��퓬���t���O���L���Ȃ�A���͕������Ȃ����Ƃɂ���
	if( NoFight == true ){
		soundlists = 0;
	}

	//��]�Ƙr�̊p�x
	TurnSeen();
	ArmAngle();

	//���C������
	if( enemyhuman != NULL ){		//���ɓG�������Ă����
		newbattlemode = AI_ACTION;
		actioncnt = 0;
	}
	else if( SearchEnemy() != 0 ){		//�G���������
		newbattlemode = AI_ACTION;
		actioncnt = 0;
	}
	else if( (HitFlag == true)||(soundlists > 0) ){	//��e������������������
		cautioncnt = 160;			//�x�����ĊJ
	}
	else if( cautioncnt == 0 ){		//�x�����I������Ȃ�
		if( CheckTargetPos() == false ){				//�x���J�n�n�_��藣��Ă��邩
			MoveTarget();				//�x���J�n�n�_�ɋ߂Â�
		}
		else{
			newbattlemode = AI_NORMAL;

			//�x���҂��p�X�Ȃ玟�֐i�߂�
			pointdata pdata;
			Points->Getdata(&pdata, target_pointid);
			if( (pdata.p1 == 3)&&(pdata.p2 == 4) ){
				SearchTarget(true);
			}
		}
	}
	else if( cautioncnt < 100 ){	//100�t���[����؂�����A�����_���Ɍx���I���i�J�E���g�F0�Ɂj
		if( GetRand(50) == 0 ){ cautioncnt = 0; }
	}
	else{ cautioncnt -= 1; }

	//�ǔ����őΏۂ��痣�ꂷ������A�����_���Ɍx���I��
	if( (movemode == AI_TRACKING)&&(GetRand(3) == 0) ){
		pointdata pdata;
		float x, z;
		float tx, tz;
		Points->Getdata(&pdata, target_pointid);
		SearchHumanPos(pdata.p4, &tx, &tz);
		x = posx - tx;
		z = posz - tz;
		if( (x*x + z*z) > 25.0f*25.0f ){
			cautioncnt = 0;
		}
	}

	//�ݒ�𔻒�
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! @brief �ʏ탁�C������
//! @return �s�ρFfalse�@�ύX�Ftrue
bool AIcontrol::NormalMain()
{
	int newbattlemode = AI_NORMAL;

	if( hold == false ){
		SearchTarget(false);
	}
	enemyhuman = NULL;

	//���W�ƃ`�[���ԍ����擾
	int teamid;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &teamid);

	//��e�Ɖ��̏󋵂��擾
	bool HitFlag = ctrlhuman->CheckHit();
	soundlist soundlist[MAX_SOUNDMGR_LIST];
	int soundlists = GameSound->GetWorldSound(posx, posy + VIEW_HEIGHT, posz, teamid, soundlist);

	//��퓬���t���O���L���Ȃ�A���͕������Ȃ����Ƃɂ���
	if( NoFight == true ){
		soundlists = 0;
	}

	//�����_���p�X�Ȃ珈�����s
	if( movemode == AI_RANDOM ){
		SearchTarget(true);
	}

	if( movemode == AI_RUN2 ){		//�D��I�ȑ���̏���
		//�G����������U���ɓ���
		if( SearchEnemy() != 0 ){
			newbattlemode = AI_ACTION;
		}
		else{
			MovePath();		//�ړ����s
		}
	}
	else{							//�D��I�ȑ���ȊO�̏���
		//�x������ɓ��鏈��
		if(
			(SearchEnemy() != 0)||							//�G��������
			(HitFlag == true)||(soundlists > 0)||	//��e������������������
			(CheckCorpse( GetRand(MAX_HUMAN) ) == true)	//���̂�������
		){
			newbattlemode = AI_CAUTION;
			cautioncnt = 160;
			target_posx = posx;
			target_posz = posz;
		}
		else{
			MovePath();		//�ړ����s
		}
	}

	//�r�̊p�x��ݒ�
	if( movemode != AI_GRENADE ){
		ArmAngle();
	}

	//�ݒ�𔻒�
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! @brief �������n�֐�
void AIcontrol::Init()
{
	//�N���X�ݒ肪����������Ώ������Ȃ�
	if( ctrlhuman == NULL ){ return; }
	if( blocks == NULL ){ return; }
	if( Points == NULL ){ return; }
	if( CollD == NULL ){ return; }

	//�g�p����Ă��Ȃ��l�Ȃ珈�����Ȃ�
	if( ctrlhuman->GetEnableFlag() == false ){ return; }

	//�X�e�[�g��������
	hold = false;
	NoFight = false;
	battlemode = AI_NORMAL;
	movemode = AI_NULL;
	enemyhuman = NULL;
	addrx = 0.0f;
	addry = 0.0f;
	waitcnt = 0;
	gotocnt = 0;
	moveturn_mode = 0x00;
	cautioncnt = 0;
	actioncnt = 0;
	longattack = false;

	//AI���x���Ɛݒ�l���擾
	int paramid;
	HumanParameter paramdata;
	//target_pointid = in_target_pointid;
	ctrlhuman->GetParamData(&paramid, &target_pointid, NULL, NULL);
	Param->GetHuman(paramid, &paramdata);
	AIlevel = paramdata.AIlevel;
	Param->GetAIlevel(AIlevel, &LevelParam);

	//���̃|�C���g������
	SearchTarget(true);
}

//! @brief �w�肵���ꏊ�֑ҋ@������
//! @param px X���W
//! @param pz Z���W
//! @param rx �d���������
//! @attention �ړ��p�X�Ɋւ�炸�A�w�肵�����W�ւ̑ҋ@���������܂��BInit()�֐����ēx���s����܂Ō��ɖ߂��܂���B
void AIcontrol::SetHoldWait(float px, float pz, float rx)
{
	movemode = AI_WAIT;
	hold = true;
	target_posx = px;
	target_posz = pz;
	target_rx = rx;
}

//! @brief �w�肵���l��ǔ�������
//! @param id �l�̃f�[�^�ԍ�
//! @attention �ړ��p�X�Ɋւ�炸�A�w�肵���l�ւ̒ǔ����������܂��BInit()�֐����ēx���s����܂Ō��ɖ߂��܂���B
void AIcontrol::SetHoldTracking(int id)
{
	movemode = AI_TRACKING;
	hold = false;
	target_pointid = id;
}

//! @brief �����I�Ɍx��������
//! @warning �D��I�ȑ��� �����s���̏ꍇ�A���̊֐��͉������܂���B
void AIcontrol::SetCautionMode()
{
	//�D��I�ȑ���Ȃ�Ή������Ȃ�
	if( movemode == AI_RUN2 ){ return; }

	if( battlemode == AI_NORMAL ){
		target_posx = posx;
		target_posz = posz;
	}
	battlemode = AI_CAUTION;
	cautioncnt = 160;
}

//! @brief ��퓬���t���O��ݒ�
//! @param flag true�F�퓬���s��Ȃ��i��퓬���j�@false�F�퓬���s���i�ʏ�j
//! @attention �t���O��L���ɂ���ƓG��F�����Ȃ��Ȃ�܂��B
void AIcontrol::SetNoFightFlag(bool flag)
{
	NoFight = flag;
}

//! @brief �����n�֐�
void AIcontrol::Process()
{
	//�N���X�ݒ肪����������Ώ������Ȃ�
	if( ctrlhuman == NULL ){ return; }
	if( blocks == NULL ){ return; }
	if( Points == NULL ){ return; }
	if( CollD == NULL ){ return; }

	//�����Ȑl�N���X�Ȃ珈�����Ȃ�
	if( ctrlhuman->GetEnableFlag() == false ){ return; }

	//���S������
	if( ctrlhuman->GetHP() <= 0 ){
		battlemode = AI_DEAD;
		movemode = AI_DEAD;
		return;
	}

	//���W�Ɗp�x���擾
	ctrlhuman->GetPosData(&posx, &posy, &posz, NULL);
	ctrlhuman->GetRxRy(&rx, &ry);

	//�����_���ɓ�����~�߂�
	CancelMoveTurn();

	//�U�������x�����Ȃ��
	if( (battlemode == AI_ACTION)||(battlemode == AI_CAUTION) ){
		//���������
		HaveWeapon();
	}

	//��v�Z���s
	if( battlemode == AI_ACTION ){			//�U����
		ActionMain();
	}
	else if( battlemode == AI_CAUTION ){	//�x����
		CautionMain();
	}
	else{									//���펞
		NormalMain();
	}

	//�ړ��E�����]������
	ControlMoveTurn();

	//�������舵��
	ControlWeapon();

	//�p�x��K�p
	ctrlhuman->SetRxRy(rx, ry);
}