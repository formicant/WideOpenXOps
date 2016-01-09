//! @file ai.h
//! @brief AIcontrol�N���X�̐錾

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

#ifndef AI_H
#define AI_H

#define AI_TOTALHUMAN_SCALE (MAX_HUMAN/24)		//!< 24�l������̍ő�l���̔{��

#define AI_ADDTURNRAD DegreeToRadian(0.8f)	//!< AI�̐���i��]�j�\��
#define AI_SEARCH_RX DegreeToRadian(110)	//!< �G�𔭌����鉡���p�x
#define AI_SEARCH_RY DegreeToRadian(60)		//!< �G�𔭌�����c���p�x

#define AI_ARRIVALDIST_PATH 5.0f		//!< �p�X�ɓ��B�����Ɣ��肷�鋗��
#define AI_ARRIVALDIST_TRACKING 18.0f	//!< �ǔ��Ώۂɓ��B�����Ɣ��肷�鋗��
#define AI_ARRIVALDIST_WALKTRACKING 24.0f	//!< �ǔ��Ώۂցi���炸�Ɂj�����ċ߂Â�����
#define AI_CHECKJUMP_HEIGHT 0.3f		//!< �O�Ƀu���b�N�����������肷�鍂��
#define AI_CHECKJUMP_DIST 2.0f			//!< �O�Ƀu���b�N�����������肷�鋗��
#define AI_CHECKJUMP2_DIST 10.0f			//!< �O�Ƀu���b�N�����������肷�鋗���i�D��I�ȑ���j
#define AI_CHECKBULLET 20.0f			//!< �e���߂���ʂ����Ɣ��肷�鋗��

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! @brief AI������s���N���X
//! @details �p�X�ɂ��ړ��A����ɂ���G�ւ̔F����U���A����̎g�������@�Ȃǂ�AI�̐���S�ʂ��s���܂��B
class AIcontrol
{
	class ObjectManager *ObjMgr;		//!< ObjectManager�N���X�̃|�C���^
	class BlockDataInterface *blocks;	//!< �u���b�N�f�[�^���Ǘ�����N���X�ւ̃|�C���^
	class PointDataInterface *Points;	//!< �|�C���g�f�[�^���Ǘ�����N���X�ւ̃|�C���^
	class ParameterInfo *Param;		//!< �ݒ�l���Ǘ�����N���X�ւ̃|�C���^
	class Collision *CollD;				//!< �����蔻����Ǘ�����N���X�ւ̃|�C���^
	class SoundManager *GameSound;		//!< �Q�[�����ʉ��Đ��N���X�ւ̃|�C���^

	int AIlevel;		//!< AI���x��
	int battlemode;		//!< �퓬���[�h
	int movemode;		//!< �ړ����[�h
	bool hold;		//!< �ړ��p�X��ǂ܂Ȃ�
	bool NoFight;	//!< ��퓬���t���O
	float posx;		//!< X���W
	float posy;		//!< Y���W
	float posz;		//!< Z���W
	float rx;		//!< X����]�p�x
	float ry;		//!< Y����]�p�x
	float addrx;	//!< X����]�p�����x
	float addry;	//!< Y����]�p�����x
	int target_pointid;		//!< �p�X�n�@�^�[�Q�b�g�̃f�[�^�ԍ�
	float target_posx;		//!< �p�X�n�@�^�[�Q�b�g��X���W
	float target_posz;		//!< �p�X�n�@�^�[�Q�b�g��Z���W
	float target_rx;		//!< �p�X�n�@�^�[�Q�b�g�̐����p�x
	float total_move;		//!< ���v�ړ���
	int waitcnt;		//!< ���ԑ҂��J�E���g
	int movejumpcnt;	//!< �W�����v����J�E���g
	int gotocnt;		//!< �ړ��J�E���g
	int moveturn_mode;	//!< �ړ��������]�����̃t���O
	int cautioncnt;		//!< �x�����J�E���g
	int actioncnt;		//!< �U�����J�E���g
	bool longattack;	//!< �ߋ����E�������t���O
	AIParameter *LevelParam;	//!< AI�̐��\�l

	int SearchHumanPos(signed char in_p4, float *out_x, float *out_z);
	bool CheckTargetPos();
	bool SearchTarget(bool next);
	void MoveTarget();
	void MoveTarget2();
	void MoveRandom();
	void TurnSeen();
	bool StopSeen();
	bool MoveJump();
	void Action();
	bool ActionCancel();
	int HaveWeapon();
	void CancelMoveTurn();
	void ControlMoveTurn();
	int ControlWeapon();
	int ThrowGrenade();
	void ArmAngle();
	int SearchEnemy();
	int SearchShortEnemy();
	bool CheckLookEnemy(int id, float search_rx, float search_ry, float maxDist, float *out_minDist);
	bool CheckLookEnemy(class human* thuman, float search_rx, float search_ry, float maxDist, float *out_minDist);
	bool CheckCorpse(int id);
	void MovePath();
	bool ActionMain();
	bool CautionMain();
	bool NormalMain();

	int ctrlid;					//!< �������g�i����Ώہj�̐l�ԍ�
	class human *ctrlhuman;		//!< �������g�i����Ώہj��human�N���X
	class human *enemyhuman;	//!< �U���Ώۂ�human�N���X

public:
	AIcontrol(class ObjectManager *in_ObjMgr = NULL, int in_ctrlid = -1, class BlockDataInterface *in_blocks = NULL, class PointDataInterface *in_Points = NULL, class ParameterInfo *in_Param = NULL, class Collision *in_CollD = NULL, class SoundManager *in_GameSound = NULL);
	~AIcontrol();
	void SetClass(class ObjectManager *in_ObjMgr, int in_ctrlid, class BlockDataInterface *in_blocks, class PointDataInterface *in_Points, class ParameterInfo *in_Param, class Collision *in_CollD, class SoundManager *in_GameSound);
	void Init();
	void SetHoldWait(float px, float pz, float rx);
	void SetHoldTracking(int id);
	void SetCautionMode();
	void SetNoFightFlag(bool flag);
	void Process();
};

//! AI�̐��䃂�[�h��\���萔
enum AImode {
	AI_DEAD = 0,	//!< ���S���Ă���l
	AI_ACTION,		//!< �퓬���̐l
	AI_CAUTION,		//!< �x�����̐l
	AI_NORMAL,		//!< �ʏ�̃��[�h

	AI_WALK,		//!< �ړ��p�X�ɂ���ĕ����Ă���l
	AI_RUN,			//!< �ړ��p�X�ɂ���đ����Ă���l
	AI_WAIT,		//!< �p�X�ɂ���đҋ@���Ă���l
	AI_STOP,		//!< �p�X�ɂ���Ď��ԑ҂������Ă���l
	AI_TRACKING,	//!< ����̐l��ǔ�����
	AI_GRENADE,		//!< ��֒e�𓊂���
	AI_RUN2,		//!< �D��I�ȑ���
	AI_RANDOM,		//!< �����_���p�X������
	AI_NULL			//!< �p�X�Ȃ�
};

//! AI�̑��샂�[�h��\���萔
enum AIcontrolFlag {
	AI_CTRL_MOVEFORWARD = 0x0001,
	AI_CTRL_MOVEBACKWARD = 0x0002,
	AI_CTRL_MOVELEFT = 0x0004,
	AI_CTRL_MOVERIGHT = 0x0008,
	AI_CTRL_MOVEWALK = 0x0010,
	AI_CTRL_TURNUP = 0x0100,
	AI_CTRL_TURNDOWN = 0x0200,
	AI_CTRL_TURNLEFT = 0x0400,
	AI_CTRL_TURNRIGHT = 0x0800
};

#endif