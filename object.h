//! @file object.h
//! @brief objectクラスの宣言

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

#ifndef OBJECT_H
#define OBJECT_H

// 注意：速度変更時は、当たり判定系の定数も要確認
#define HUMAN_PROGRESSRUN_ACCELERATION	0.7f			//!< 人の前進（走り）加速度
#define HUMAN_SIDEWAYSRUN_ACCELERATION 0.5f			//!< 人の横走り加速度
#define HUMAN_REGRESSRUN_ACCELERATION 0.38f			//!< 人の後退加速度
//! 人が斜め前に進む加速度
#define HUMAN_PROGRESSRUN_SIDEWAYSRUN_ACCELERATION ((HUMAN_PROGRESSRUN_ACCELERATION + HUMAN_SIDEWAYSRUN_ACCELERATION) / 2)
//! 人が斜め後ろに進む加速度
#define HUMAN_REGRESSRUN_SIDEWAYSRUN_ACCELERATION (HUMAN_REGRESSRUN_ACCELERATION)
#define HUMAN_PROGRESSWALK_ACCELERATION	0.35f		//!< 人が歩く加速度
#define HUMAN_ATTENUATION			0.5f		//!< 人の1フレーム当たりの減衰率　（0.0 < x < 1.0）

#define HUMAN_JUMP_SPEED (2.2f + (HUMAN_DAMAGE_SPEED))	//!< ジャンプする速度

#define HUMAN_HEIGTH 20.0f		//!< 人の高さ
#define HUMAN_DAMAGE_HEAD 2.0f	//!< 弾が 頭 に当たった場合のダメージ倍率
#define HUMAN_DAMAGE_UP 1.0f	//!< 弾が 上半身 に当たった場合のダメージ倍率
#define HUMAN_DAMAGE_LEG 0.7f	//!< 弾が 下半身 に当たった場合のダメージ倍率
#define HUMAN_DAMAGE_ZOMBIEU 15	//!< ゾンビの攻撃を受けた場合の最低ダメージ
#define HUMAN_DAMAGE_ZOMBIEA 5	//!< ゾンビの攻撃を受けた場合の追加ダメージ量
#define HUMAN_DAMAGE_SPEED (0.066f + 0.132f)	//!< 落下量
#define HUMAN_DAMAGE_MINSPEED -3.8f	//!< 落下によりダメージを受けない速度
#define HUMAN_DAMAGE_MAXSPEED -7.0f	//!< 最大落下速度
#define HUMAN_DAMAGE_MAXFALL 120	//!< 落下による最大ダメージ
#define MAX_DAMAGE_GRENADE_DISTANCE 80.0f		//!< 手榴弾によりダメージを受ける最大距離
#define HUMAN_DAMAGE_GRENADE_HEAD 100			//!< 手榴弾による 頭 への最大ダメージ
#define HUMAN_DAMAGE_GRENADE_LEG 80				//!< 手榴弾による 足 への最大ダメージ
#define SMALLOBJECT_DAMAGE_GRENADE 80			//!< 手榴弾による 小物 への最大ダメージ

#define WEAPONERRORRANGE_SCALE 0.25f	//!< 武器の反動角度の倍率（×0.25 ＝ ÷4）

#define ARMRAD_NOWEAPON DegreeToRadian((-90 + 20))			//!< 手ぶら時の腕の表示角度
#define ARMRAD_RELOADWEAPON DegreeToRadian(-20)				//!< リロード時の腕の表示角度

#define HUMAN_MAPCOLLISION_R 5.0f							//!< 人とマップの当たり判定　半径
#define HUMAN_MAPCOLLISION_HEIGTH 10.2f						//!< 人とマップの当たり判定　高さ（注：腰程度）
#define HUMAN_MAPCOLLISION_SLOPEANGLE DegreeToRadian(50)	//!< 人とマップの当たり判定　登れない斜面の角度
#define HUMAN_MAPCOLLISION_SLOPEFORCEANGLE DegreeToRadian(22.62f)		//!< 人とマップの当たり判定　登れない斜面が人を押し出す角度
#define HUMAN_DEADLINE -100.0f						//!< 人が死亡するY座標（デッドライン）
#define HUMAN_DEADADDRY DegreeToRadian(0.75f)		//!< 死体の倒れる加速度

#define BULLET_SPEEDSCALE 3				//!< 弾速の倍率
#define BULLET_DESTROYFRAME 40			//!< 弾の消滅フレーム数
#define GRENADE_DESTROYFRAME 100		//!< 手榴弾の爆発フレーム数
#define GRENADE_BOUND_ACCELERATION 0.565f 	//!< 手榴弾の反射減衰率

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 2		//!< Select include file.
#endif
#include "main.h"

#define HUMAN_DEADBODY_COLLISION	//!< @brief 人が倒れる際にマップと当たり判定を実施するか @details 定数宣言有効：当たり判定を実施　定数宣言無効（コメント化）：そのまま倒すだけ

//! @brief オブジェクト管理クラス（基底クラス）
//! @details 3Dで座標管理や描画を行うオブジェクト全般を管理するクラス群の基底クラスです。
class object
{
protected:
	class ParameterInfo *Param;	//!< 設定値を管理するクラスへのポインタ
	float pos_x;		//!< X座標
	float pos_y;		//!< Y座標
	float pos_z;		//!< Z座標
	float rotation_x;	//!< 回転角度
	float model_size;	//!< 表示サイズ
	int id_parameter;	//!< データの種類
	int id_model;		//!< モデル認識番号
	int id_texture;		//!< テクスチャ認識番号
	bool EnableFlag;	//!< 有効化フラグ

public:
	object(class ParameterInfo *in_Param = NULL, float x = 0.0f, float y = 0.0f, float z = 0.0f, float rx = 0.0f, float size = 1.0f, bool flag = true);
	~object();
	virtual void SetParameterInfoClass(class ParameterInfo *in_Param);
	virtual void SetPosData(float x, float y, float z, float rx);
	virtual void GetPosData(float *x, float *y, float *z, float *rx);
	virtual void SetEnableFlag(bool flag);
	virtual bool GetEnableFlag();
	virtual void SetModel(int id, float size);
	virtual void GetModel(int *id, float *size);
	virtual void SetTexture(int id);
	virtual int GetTexture();
	virtual int RunFrame();
	virtual void Render(class D3DGraphics *d3dg);
};

//! @brief 人管理クラス
class human : public object
{
protected:
	int point_dataid;		//!< ポイントのデータ番号
	signed char point_p4;	//!< ポイントの認識番号
	int teamid;				//!< チーム番号
	float move_x;			//!< X軸速度
	float move_y;			//!< Y軸（落下）速度
	float move_z;			//!< Z軸速度
	bool move_y_flag;		//!< Y軸移動フラグ
	float rotation_y;	//!< 全体の回転角度
	float armrotation_y;	//!< 腕の回転角度
	float reaction_y;		//!< 腕を上げ下げする角度
	float legrotation_x;	//!< 足の回転角度
	float upmodel_size;		//!< 上半身表示サイズ
	float armmodel_size;	//!< 腕表示サイズ
	float legmodel_size;	//!< 足表示サイズ
	class weapon *weapon[TOTAL_HAVEWEAPON];		//!< 武器
	int selectweapon;			//!< 武器A/Bの選択
	int selectweaponcnt;		//!< 武器の切り替えカウント
	int hp;						//!< 体力
#ifdef HUMAN_DEADBODY_COLLISION
	int deadstate;					//!< 死体になっているか
#endif
	float add_ry;					//!< 死体の倒れる加速度
	int id_upmodel;							//!< 上半身
	int id_armmodel[TOTAL_ARMMODE];			//!< 腕
	int id_legmodel;						//!< 足（静止）
	int id_walkmodel[TOTAL_WALKMODE];		//!< 足（歩く）
	int id_runmodel[TOTAL_RUNMODE];			//!< 足（走る）
	float move_rx;		//!< 移動角度
	int MoveFlag;		//!< 移動方向を表すフラグ
	int MoveFlag_lt;	//!< （前回の）移動方向を表すフラグ
	int scopemode;		//!< スコープ使用モード
	bool HitFlag;		//!< 被弾を表すフラグ
	int walkcnt;		//!< 歩くモーションのカウント
	int runcnt;			//!< 走るモーションのカウント
	float totalmove;	//!< 合計移動量
	int StateGunsightErrorRange;		//!< 照準の状態誤差
	int ReactionGunsightErrorRange;		//!< 照準の反動誤差
	bool Invincible;	//!< 無敵フラグ

	void GunsightErrorRange();
	int CheckAndProcessDead(class Collision *CollD);
	void ControlProcess();
	bool MapCollisionDetection(class Collision *CollD, class BlockDataInterface *inblockdata, float *FallDist, float *nowmove_x, float *nowmove_z);

public:
	human(class ParameterInfo *in_Param = NULL, float x = 0.0f, float y = 0.0f, float z = 0.0f, float rx = 0.0f, int id_param = -1, int dataid = 0, signed char p4 = 0, int team = 0, bool flag = false);
	~human();
	virtual void SetParamData(int id_param, int dataid, signed char p4, int team, bool init);
	virtual void GetParamData(int *id_param, int *dataid, signed char *p4, int *team);
	virtual void GetMovePos(float *x, float *y, float *z);
	virtual void SetModel(int upmodel, int armmodel[], int legmodel, int walkmodel[], int runmodel[]);
	virtual int GetHP();
	virtual bool SetHP(int in_hp);
	virtual bool GetDeadFlag();
	virtual void SetTeamID(int id);
	virtual bool GetInvincibleFlag();
	virtual void SetInvincibleFlag(bool flag);
	virtual void SetWeapon(class weapon *in_weapon[]);
	virtual int PickupWeapon(class weapon *in_weapon);
	virtual void ChangeWeapon(int id = -1);
	virtual int GetChangeWeaponCnt();
	virtual void GetWeapon(int *out_selectweapon, class weapon *out_weapon[]);
	virtual int GetMainWeaponTypeNO();
	virtual bool GetWeaponBlazingmode();
	virtual bool ShotWeapon(int *weapon_paramid, int *GunsightErrorRange);
	virtual bool ReloadWeapon();
	virtual bool DumpWeapon();
	virtual int ChangeShotMode();
	virtual void SetMoveForward();
	virtual void SetMoveBack();
	virtual void SetMoveLeft();
	virtual void SetMoveRight();
	virtual void SetMoveWalk();
	virtual int GetMovemode(bool nowdata);
	virtual bool SetEnableScope();
	virtual void SetDisableScope();
	virtual int GetScopeMode();
	virtual void GetRxRy(float *rx, float *ry);
	virtual void SetRxRy(float rx, float ry);
	virtual float GetDeadRy();
	virtual int Jump();
	virtual void AddPosOrder(float rx, float ry, float speed);
	virtual void HitBulletHead(int attacks);
	virtual void HitBulletUp(int attacks);
	virtual void HitBulletLeg(int attacks);
	virtual void HitZombieAttack();
	virtual void HitGrenadeExplosion(int attacks);
	virtual void SetHitFlag();
	virtual bool CheckHit();
	virtual float GetTotalMove();
	virtual int RunFrame(class Collision *CollD, class BlockDataInterface *inblockdata, bool F5mode);
	virtual int GetGunsightErrorRange();
	virtual void Render(class D3DGraphics *d3dg, class ResourceManager *Resource, bool DrawArm, bool player);
};

//! @brief 武器管理クラス
class weapon : public object
{
protected:
	float move_x;		//!< X軸移動量
	float move_y;		//!< Y軸移動量
	float move_z;		//!< Z軸移動量
	bool usingflag;		//!< 使用中を表すフラグ
	int bullets;		//!< 合計弾数
	int Loadbullets;	//!< 装弾数
	int shotcnt;		//!< 連射カウント
	int reloadcnt;		//!< リロードカウント
	bool motionflag;	//!< 座標移動中を表すフラグ

public:
	weapon(class ParameterInfo *in_Param = NULL, float x = 0.0f, float y = 0.0f, float z = 0.0f, float rx = 0.0f, int id_param = 0, int nbs = 0, bool flag = false);
	~weapon();
	virtual void SetPosData(float x, float y, float z, float rx);
	virtual void SetParamData(int id_param, int lnbs, int nbs, bool init);
	virtual void GetParamData(int *id_param, int *lnbs, int *nbs);
	virtual bool GetUsingFlag();
	virtual int Pickup();
	virtual void Dropoff(float x, float y, float z, float rx, float speed);
	virtual int Shot();
	virtual int StartReload();
	virtual int RunReload();
	virtual int GetReloadCnt();
	virtual bool ResetWeaponParam(class ResourceManager *Resource, int id_param, int lnbs, int nbs);
	virtual int RunFrame(class Collision *CollD);
	virtual void Render(class D3DGraphics *d3dg);
};

//! @brief 小物管理クラス
class smallobject : public object
{
protected:
	class MIFInterface *MIFdata;		//!< 設定値を管理するクラスへのポインタ
	float rotation_y;		//!< 回転角度
	signed char point_p4;	//!< ポイントの認識番号
	int hp;					//!< 体力
	float jump_rx;		//!< 飛ばす横軸角度
	float move_rx;		//!< 飛ばす横軸移動量
	float add_rx;		//!< 飛ばす横軸回転量
	float add_ry;		//!< 飛ばす縦軸回転量
	int jump_cnt;		//!< 飛ばす上昇カウント

public:
	smallobject(class ParameterInfo *in_Param = NULL, class MIFInterface *in_MIFdata = NULL, float x = 0.0f, float y = 0.0f, float z = 0.0f, float rx = 0.0f, int id_param = 0, signed char p4 = 0, bool flag = false);
	~smallobject();
	virtual void SetMIFInterfaceClass(class MIFInterface *in_MIFdata);
	virtual void SetParamData(int id_param, signed char p4, bool init);
	virtual void GetParamData(int *id_param, signed char *p4);
	virtual int GetHP();
	virtual float CollisionMap(class Collision *CollD);
	virtual void HitBullet(int attacks);
	virtual void HitGrenadeExplosion(int attacks);
	virtual void Destruction();
	virtual int RunFrame();
	virtual void Render(D3DGraphics *d3dg);
};

//! @brief 弾丸管理クラス
class bullet : public object
{
protected:
	float rotation_y;	//!< 回転角度
	int attacks;		//!< 攻撃力
	int penetration;	//!< 貫通力
	int speed;			//!< 弾速
	int teamid;			//!< チーム番号
	int humanid;		//!< 人のデータ番号
	int cnt;			//!< カウント

public:
	bullet(int modelid = -1, int textureid = -1);
	~bullet();
	virtual void SetPosData(float x, float y, float z, float rx, float ry);
	virtual void SetParamData(int _attacks, int _penetration, int _speed, int _teamid, int _humanid, bool init);
	virtual void GetPosData(float *x, float *y, float *z, float *rx, float *ry);
	virtual void GetParamData(int *_attacks, int *_penetration, int *_speed, int *_teamid, int *_humanid);
	virtual int RunFrame();
	virtual void Render(class D3DGraphics *d3dg);
};

//! @brief 手榴弾管理クラス
class grenade : public bullet
{
	float move_x;		//!< X軸移動量
	float move_y;		//!< Y軸移動量
	float move_z;		//!< Y軸移動量

public:
	grenade(int modelid = -1, int textureid = -1);
	~grenade();
	void SetParamData(float speed, int _teamid, int _humanid, bool init);
	void GetParamData(float *_speed, int *_teamid, int *_humanid);
	float GetSpeed();
	int RunFrame(class Collision *CollD);
	virtual void Render(D3DGraphics *d3dg);
};

//! @brief エフェクト管理クラス
class effect : public object
{
protected:
	int type;			//!< 種類
	float rotation_y;	//!< 回転角度
	float move_x;		//!< X軸移動量
	float move_y;		//!< Y軸移動量
	float move_z;		//!< Z軸移動量
	float rotation_texture;	//!< 回転角度
	float alpha;			//!< 透明度
	float set_alpha;		//!< スタート時の透明度
	int cnt;			//!< カウント
	int setcnt;			//!< 設定されたカウント

public:
	effect(float x = 0.0f, float y = 0.0f, float z = 0.0f, float size = 1.0f, float rotation = 0.0f, int count = 0, int texture = 0, int settype = 0);
	~effect();
	virtual void SetParamData(float in_move_x, float in_move_y, float in_move_z, float size, float rotation, int count, int texture, int settype, bool init);
	virtual void SetRxRy(float rx, float ry);
	virtual int GetTextureID();
	virtual void GetMove(float *mx, float *my, float *mz);
	virtual bool GetCollideMapFlag();
	virtual int RunFrame(float camera_rx, float camera_ry);
	virtual void Render(class D3DGraphics *d3dg);
};

//! 人の足の状態を示す定数
enum Human_LegState {
	LEG_STOP = 0,
	LEG_WALK,
	LEG_RUN
};

//! 人の移動操作を表すフラグ
enum Human_MoveFlag {
	MOVEFLAG_FORWARD = 0x01,
	MOVEFLAG_BACK = 0x02,
	MOVEFLAG_LEFT = 0x04,
	MOVEFLAG_RIGHT = 0x08,
	MOVEFLAG_WALK = 0x10,
};

//! エフェクトの種類を表す定数
enum Effect_Type {
	EFFECT_NORMAL = 0x00,			//!< ノーマル
	EFFECT_DISAPPEAR = 0x01,		//!< 消す
	EFFECT_DISAPPEARHALF = 0x02,	//!< 半分の時間で消す
	EFFECT_TRANSLUCENT = 0x04,		//!< 半透明
	EFFECT_MAGNIFY = 0x08,			//!< 拡大
	EFFECT_ROTATION = 0x10,			//!< 回転
	EFFECT_FALL = 0x20,				//!< 落下
	EFFECT_NOBILLBOARD = 0x40,		//!< ビルボード化しない
	EFFECT_COLLIDEMAP = 0x80		//!< マップとのあたり判定
};

#endif