﻿//! @file objectmanager.h
//! @brief ObjectManagerクラスの宣言

//--------------------------------------------------------------------------------
// 
// OpenXOPS
// Copyright (c) 2014-2018, OpenXOPS Project / [-_-;](mikan) All rights reserved.
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

#define MAX_HUMAN 96			//!< 最大人数　【警告】：AIレベル調整の関係上‘24の倍数’にすること！
#define MAX_WEAPON 200			//!< 武器の最大数
#define MAX_SMALLOBJECT 40		//!< 小物の最大数

#define MAX_BULLET 128			//!< 銃弾の最大数
#define MAX_GRENADE 32			//!< 手榴弾の最大数
#define MAX_EFFECT 256			//!< エフェクトの最大数

#define WEAPONSHOT_HEIGHT (VIEW_HEIGHT)	//!< 弾を発射する高さ

#define TOTAL_WEAPON_AUTOBULLET 3	//!< 初期化時に自動的に補てんされる弾数（装弾数の何倍か）

#define HUMAN_HUMANCOLLISION_R 2.5f			//!< 人同士の当たり判定の半径

#define HUMAN_BULLETCOLLISION_HEAD_H 2.8f	//!< 頭の当たり判定の高さ
#define HUMAN_BULLETCOLLISION_HEAD_R 1.3f	//!< 頭の当たり判定の半径
#define HUMAN_BULLETCOLLISION_UP_H 7.5f		//!< 上半身の当たり判定の高さ
#define HUMAN_BULLETCOLLISION_UP_R 2.5f		//!< 上半身の当たり判定の半径
#define HUMAN_BULLETCOLLISION_LEG_H 10.0f	//!< 足の当たり判定の高さ
#define HUMAN_BULLETCOLLISION_LEG_R 2.5f	//!< 足の当たり判定の半径

#define SMALLOBJECT_COLLISIONSCALE 0.13f	//!< 小物当たり判定の倍率

#define MAX_OBJECTMANAGER_LOGLEN 3						//!< オブジェクトログの最大行数
#define MAX_OBJECTMANAGER_LOGLINES 64					//!< オブジェクトログの最大文字数
#define MAX_OBJECTMANAGER_LOGCNT (int)(5.0f*GAMEFPS)	//!< オブジェクトログの表示カウント数

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! エフェクト描画計算用構造体
struct effectdata{
	int id;		//!< データ番号
	float dist;	//!< 距離
};

//! @brief オブジェクト管理クラス
//! @details 各オブジェクトの初期化・計算・描画などを行い管理します。
class ObjectManager
{
	class human *HumanIndex;					//!< 人オブジェクト
	class weapon *WeaponIndex;					//!< 武器オブジェクト
	class smallobject *SmallObjectIndex;		//!< 小物オブジェクト
	class bullet *BulletIndex;					//!< 弾オブジェクト
	class grenade *GrenadeIndex;				//!< 手榴弾オブジェクト
	class effect *EffectIndex;					//!< エフェクトオブジェクト

	unsigned int framecnt;		//!< フレーム数のカウント

	float *Human_ontarget;	//!< 命中数
	int *Human_kill;		//!< 倒した敵の数
	int *Human_headshot;	//!< 敵の頭部に命中した数
	bool *Human_ShotFlag;	//!< 発砲フラグ（マズルフラッシュ用）
	class BulletObjectHumanIndex *BulletObj_HumanIndex;		//!< 弾対人判定用オブジェクト

	bool AddCollisionFlag;	//!< 追加の当たり判定
	bool FriendlyFire;		//!< FF（同士討ち）有効化

	int Player_HumanID;		//!< プレイヤーが操作する人オブジェクトのID

	bool *Human_FrameTextureRefresh;	//!< 人のテクスチャリフレッシュフラグ（テクスチャ偽造人間（謎人間）用）

	ParameterInfo *GameParamInfo;		//!< ゲームの設定値
	D3DGraphics *d3dg;					//!< 描画クラス
	ResourceManager *Resource;			//!< リソース管理
	BlockDataInterface *BlockData;		//!< ブロックデータ管理クラス
	PointDataInterface *PointData;		//!< ポイントデータ管理クラス
	Collision *CollD;					//!< 当たり判定管理クラス
	SoundManager *GameSound;				//!< ゲーム効果音管理クラス
	MIFInterface *MIFdata;				//!< MIFコントロールクラス

	class ObjectManagerLog *ObjectLog;	//!< オブジェクトログクラス

	int AddHumanIndex(pointdata data, pointdata infodata, bool RandomFlag);
	int AddWeaponIndex(pointdata data);
	int AddSmallObjectIndex(pointdata data);
	void SetHumanBlood(float x, float y, float z, int damage, bool CollideMap);
	bool CollideHuman(human *in_humanA, human *in_humanB);
	bool CollideBullet(bullet *in_bullet);
	void HitBulletMap(float x, float y, float z, int teamID);
	void HitBulletHuman(int HitHuman_id, int Hit_id, float x, float y, float z, float brx, int attacks, int humanid, float ontargetcnt);
	void HitBulletSmallObject(int HitSmallObject_id, float x, float y, float z, int attacks, int teamID);
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
	void SetAddCollisionFlag(bool flag);
	bool GetFriendlyFireFlag();
	void SetFriendlyFireFlag(bool flag);
	int GetPlayerID();
	void SetPlayerID(int id);
	human* GetHumanObject(int id);
	human* GetPlayerHumanObject();
	int GetHumanObjectID(human* object);
	weapon* GetWeaponObject(int id);
	smallobject* GetSmallObject(int id);
	bullet* GetBulletObject(int id);
	int GetBulletObjectID(bullet* object);
	bullet* GetNewBulletObject();
	grenade* GetNewGrenadeObject();
	human* SearchHuman(signed char p4);
	smallobject* SearchSmallobject(signed char p4);
	void GetTotalObjects(int *HumanCnt, int *WeaponCnt, int *SmallobjectCnt, int *BulletCnt, int *GrenadeCnt, int *EffectCnt);
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
	void HitZombieAttack(human* MyHuman, human* EnemyHuman);
	bool HumanResuscitation(int id);
	int CheckGameOverorComplete();
	bool GetObjectInfoTag(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, int *color, char *infostr);
	int Process(int cmdF5id, bool demomode, float camera_rx, float camera_ry, bool screen);
	bool GetHumanShotInfo(int id, float *ontarget, int *kill, int *headshot);
	void Render(float camera_x, float camera_y, float camera_z, int HidePlayer);
	void RenderLog(int x, int y);
	void Cleanup();
};

//! @brief 弾対人判定用クラス
//! @details 弾と人の当たり判定の有無を管理するクラスです。ObjectManagerクラス内で使用します。
class BulletObjectHumanIndex
{
	bool *HumanIndex;		//!< リスト

public:
	BulletObjectHumanIndex();
	~BulletObjectHumanIndex();
	void Init();
	bool GetIndexFlag(int id);
	void SetIndexFlag(int id);
};

//! @brief オブジェクトログクラス
//! @details オブジェクト管理クラス ObjectManager のログを記録・表示します。
class ObjectManagerLog
{
	D3DGraphics *d3dg;							//!< 描画クラス

	int TextCnt[MAX_OBJECTMANAGER_LOGLEN];		//!< フレーム数
	char *TextStr[MAX_OBJECTMANAGER_LOGLEN];	//!< 文字列
	int TextColor[MAX_OBJECTMANAGER_LOGLEN];	//!< 文字列の色

	bool AddTextLog(int cnt, char *str, int color);

public:
	ObjectManagerLog();
	~ObjectManagerLog();
	void SetClass(D3DGraphics *in_d3dg);
	void ClearLog();
	void InfoLog(char *str);
	void AddHuman(int humanID, int TeamID, int PlayerTeamID);
	void DiedHuman(int ShothumanID, int HitHumanID, int ShothumanTeamID, int HitHumanTeamID, int PlayerTeamID);
	void ReviveHuman(int humanID, int TeamID, int PlayerTeamID);
	void AddSmallObject(int objID);
	void BreakSmallObject(int objID);
	void Process();
	void Render(int x, int y);
};

#endif