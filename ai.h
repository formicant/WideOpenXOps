//! @file ai.h
//! @brief AIcontrolクラスの宣言

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

#define AI_TOTALHUMAN_SCALE (MAX_HUMAN/24)		//!< 24人あたりの最大人数の倍率

#define AI_ADDTURNRAD DegreeToRadian(0.8f)	//!< AIの旋回（回転）能力
#define AI_SEARCH_RX DegreeToRadian(110)	//!< 敵を発見する横軸角度
#define AI_SEARCH_RY DegreeToRadian(60)		//!< 敵を発見する縦軸角度

#define AI_ARRIVALDIST_PATH 5.0f		//!< パスに到達したと判定する距離
#define AI_ARRIVALDIST_TRACKING 18.0f	//!< 追尾対象に到達したと判定する距離
#define AI_ARRIVALDIST_WALKTRACKING 24.0f	//!< 追尾対象へ（走らずに）歩いて近づく距離
#define AI_CHECKJUMP_HEIGHT 0.3f		//!< 前にブロックが無いか判定する高さ
#define AI_CHECKJUMP_DIST 2.0f			//!< 前にブロックが無いか判定する距離
#define AI_CHECKJUMP2_DIST 10.0f			//!< 前にブロックが無いか判定する距離（優先的な走り）
#define AI_CHECKBULLET 20.0f			//!< 弾が近くを通ったと判定する距離

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! @brief AI制御を行うクラス
//! @details パスによる移動、視野にいる敵への認識や攻撃、武器の使い分け　などのAIの制御全般を行います。
class AIcontrol
{
	class ObjectManager *ObjMgr;		//!< ObjectManagerクラスのポインタ
	class BlockDataInterface *blocks;	//!< ブロックデータを管理するクラスへのポインタ
	class PointDataInterface *Points;	//!< ポイントデータを管理するクラスへのポインタ
	class ParameterInfo *Param;		//!< 設定値を管理するクラスへのポインタ
	class Collision *CollD;				//!< 当たり判定を管理するクラスへのポインタ
	class SoundManager *GameSound;		//!< ゲーム効果音再生クラスへのポインタ

	int AIlevel;		//!< AIレベル
	int battlemode;		//!< 戦闘モード
	int movemode;		//!< 移動モード
	bool hold;		//!< 移動パスを読まない
	bool NoFight;	//!< 非戦闘化フラグ
	float posx;		//!< X座標
	float posy;		//!< Y座標
	float posz;		//!< Z座標
	float rx;		//!< X軸回転角度
	float ry;		//!< Y軸回転角度
	float addrx;	//!< X軸回転角加速度
	float addry;	//!< Y軸回転角加速度
	int target_pointid;		//!< パス系　ターゲットのデータ番号
	float target_posx;		//!< パス系　ターゲットのX座標
	float target_posz;		//!< パス系　ターゲットのZ座標
	float target_rx;		//!< パス系　ターゲットの水平角度
	float total_move;		//!< 合計移動量
	int waitcnt;		//!< 時間待ちカウント
	int movejumpcnt;	//!< ジャンプ判定カウント
	int gotocnt;		//!< 移動カウント
	int moveturn_mode;	//!< 移動方向や回転方向のフラグ
	int cautioncnt;		//!< 警戒中カウント
	int actioncnt;		//!< 攻撃中カウント
	bool longattack;	//!< 近距離・遠距離フラグ
	AIParameter *LevelParam;	//!< AIの性能値

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

	int ctrlid;					//!< 自分自身（制御対象）の人番号
	class human *ctrlhuman;		//!< 自分自身（制御対象）のhumanクラス
	class human *enemyhuman;	//!< 攻撃対象のhumanクラス

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

//! AIの制御モードを表す定数
enum AImode {
	AI_DEAD = 0,	//!< 死亡している人
	AI_ACTION,		//!< 戦闘中の人
	AI_CAUTION,		//!< 警戒中の人
	AI_NORMAL,		//!< 通常のモード

	AI_WALK,		//!< 移動パスによって歩いている人
	AI_RUN,			//!< 移動パスによって走っている人
	AI_WAIT,		//!< パスによって待機している人
	AI_STOP,		//!< パスによって時間待ちをしている人
	AI_TRACKING,	//!< 特定の人を追尾する
	AI_GRENADE,		//!< 手榴弾を投げる
	AI_RUN2,		//!< 優先的な走り
	AI_RANDOM,		//!< ランダムパス処理中
	AI_NULL			//!< パスなし
};

//! AIの操作モードを表す定数
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