//! @file event.cpp
//! @brief EventControl�N���X�̒�`

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

#include "event.h"

//! @brief �R���X�g���N�^
EventControl::EventControl(PointDataInterface *in_Point, ObjectManager *in_ObjMgr)
{
	Point = in_Point;
	ObjMgr = in_ObjMgr;
	nextp4 = 0;
	waitcnt = 0;
}

//! @brief �f�B�X�g���N�^
EventControl::~EventControl()
{}

//! @brief �ΏۃN���X��ݒ�
//! @param in_Point �|�C���g�f�[�^�Ǘ��N���X
//! @param in_ObjMgr �I�u�W�F�N�g�Ǘ��N���X
//! @attention ���̊֐��Őݒ���s��Ȃ��ƁA�N���X���̂��������@�\���܂���B
void EventControl::SetClass(PointDataInterface *in_Point, ObjectManager *in_ObjMgr)
{
	Point = in_Point;
	ObjMgr = in_ObjMgr;
}

//! @brief ���Z�b�g
//! @param EntryP4 �C�x���g�������J�n����F���ԍ��@�i-100�A-110�A-120�@�Ȃǁj
void EventControl::Reset(signed char EntryP4)
{
	nextp4 = EntryP4;
	waitcnt = 0;
}

//! @brief ���ɏ�������F���ԍ�
//! @return P4�F�F���ԍ�
signed char EventControl::GetNextP4()
{
	return nextp4;
}

//! @brief �Ώۂ̐l�����P�[�X�������Ă��邩�`�F�b�N
//! @param in_human ���ׂ�Ώۂ�human�̃|�C���^
//! @return �����Ă���Ftrue�@�����Ă��Ȃ��Ffalse
bool EventControl::CheckHaveCase(human *in_human)
{
	int selectweapon;
	weapon *out_weapon[TOTAL_HAVEWEAPON];
	int weaponid;

	//���L���镐��I�u�W�F�N�g��S�Ď擾
	in_human->GetWeapon(&selectweapon, out_weapon);

	//�S�Ē��ׂ�
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		if( out_weapon[i] != NULL ){

			//����̎�ޔԍ����擾���A���ꂪ�P�[�X���ǂ����B
			out_weapon[i]->GetParamData(&weaponid, NULL, NULL);
			if( weaponid == ID_WEAPON_CASE ){
				return true;
			}
		}
	}
	return false;
}

//! @brief ���������s
//! @param endcnt �I���J�E���g�̃|�C���^
//! @param complete �~�b�V���������t���O�̃|�C���^
//! @param MessageID �C�x���g���b�Z�[�W�ԍ��̃|�C���^
//! @param SetMessageID �C�x���g���b�Z�[�W�ǂݏo���t���O
//! @return ���s�����X�e�b�v��
//! @attention �n���ꂽ�����ɕύX���Ȃ��ꍇ�A�n���ꂽ�����̃f�[�^�͑��삳��܂���B
//! @attention SetMessageID �́A���b�Z�[�W�C�x���g�����s���ꂽ�ꍇ�� true �ɂȂ�܂��B<b>�O�񂩂�ύX���ꂽ�Ƃ͌���܂���B</b>
int EventControl::Execution(int *endcnt, bool *complete, int *MessageID, bool *SetMessageID)
{
	pointdata data, pathdata;
	int cnt = 0;
	human* thuman;
	smallobject* tsmallobject;
	float hx, hy, hz;
	float x, y, z;
	int pid;

	for(int i=0; i<TOTAL_EVENTFRAMESTEP; i++){
		cnt = i;

		//���̃|�C���g��T��
		if( Point->SearchPointdata(&data, 0x08, 0, 0, 0, nextp4, 0) == 0 ){ return cnt; }
		if( (data.p1 < 10)||(19 < data.p1) ){ return cnt; }

		switch(data.p1){
			case 10:	//�C���B��
				*endcnt += 1;
				*complete = true;
				return cnt;

			case 11:	//�C�����s
				*endcnt += 1;
				*complete = false;
				return cnt;

			case 12:	//���S�҂�
				thuman = ObjMgr->SearchHuman(data.p2);
				if( thuman == NULL ){ return cnt; }
				if( thuman->GetDeadFlag() == false ){ return cnt; }
				nextp4 = data.p3;
				break;

			case 13:	//�����҂�
				thuman = ObjMgr->SearchHuman(data.p2);
				if( thuman == NULL ){ return cnt; }
				thuman->GetPosData(&hx, &hy, &hz, NULL);
				x = hx - data.x;
				y = hy - data.y;
				z = hz - data.z;
				if( sqrt(x*x + y*y+ + z*z) > DISTANCE_CHECKPOINT ){ return cnt; }
				nextp4 = data.p3;
				break;

			case 14:	//�����ɕύX
				if( Point->SearchPointdata(&pid, 0x08, 0, 0, 0, data.p2, 0) > 0 ){
					//�Ώۂ�AI�p�X�Ȃ�΁A�����I�Ƀp�����[�^������������
					Point->Getdata(&pathdata, pid);
					if( (pathdata.p1 == 3)||(pathdata.p1 == 8) ){
						pathdata.p2 = 0;
					}
					Point->SetParam(pid, pathdata.p1, pathdata.p2, pathdata.p3, pathdata.p4);
				}
				nextp4 = data.p3;
				break;

			case 15:	//�����j��҂�
				tsmallobject = ObjMgr->SearchSmallobject(data.p2);
				if( tsmallobject == NULL ){ return cnt; }
				if( tsmallobject->GetDrawFlag() == true ){ return cnt; }
				nextp4 = data.p3;
				break;

			case 16:	//�P�[�X�҂�
				thuman = ObjMgr->SearchHuman(data.p2);
				if( thuman == NULL ){ return cnt; }
				thuman->GetPosData(&hx, &hy, &hz, NULL);
				x = hx - data.x;
				y = hy - data.y;
				z = hz - data.z;
				if( sqrt(x*x + y*y+ + z*z) > DISTANCE_CHECKPOINT ){ return cnt; }
				if( CheckHaveCase(thuman) == false ){ return cnt; }
				nextp4 = data.p3;
				break;

			case 17:	//���ԑ҂�
				if( (int)GAMEFPS * ((unsigned char)data.p2) > waitcnt ){
					waitcnt += 1;
					return cnt;
				}
				waitcnt = 0;
				nextp4 = data.p3;
				break;

			case 18:	//���b�Z�[�W
				if( (0 <= data.p2)&&(data.p2 < MAX_POINTMESSAGES) ){
					*MessageID = data.p2;
				}
				nextp4 = data.p3;
				//break;
				*SetMessageID = true;
				return cnt;

			case 19:	//�`�[���ύX
				thuman = ObjMgr->SearchHuman(data.p2);
				if( thuman == NULL ){ return cnt; }
				thuman->SetTeamID(0);
				nextp4 = data.p3;
				break;
		}
	}
	return cnt;
}