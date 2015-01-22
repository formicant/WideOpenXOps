//! @file objectmanager.cpp
//! @brief ObjectManagerクラスの定義

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
// * Neither the name of the OpenXOPS Project nor the　names of its contributors 
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

//! コンストラクタ
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
	Player_HumanID = 0;
	AddHumanIndex_TextureID = -1;

	GameParamInfo = NULL;
	d3dg = NULL;
	Resource = NULL;
	BlockData = NULL;
	PointData = NULL;
	CollD = NULL;
}

//! ディストラクタ
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
}

//! 参照するクラスを設定
//! @param in_GameParamInfo ゲーム設定データ管理クラス
//! @param in_d3dg 描画処理クラス
//! @param in_Resource リソース管理クラス
//! @param in_BlockData ブロックデータ管理クラス
//! @param in_PointData ポイントデータ管理クラス
//! @param in_CollD 当たり判定処理クラス
//! @param in_GameSound 効果音再生クラス
//! @param in_MIFdata MIFコントロールクラス
//! @attention この関数で設定を行わないと、クラス自体が正しく機能しません。
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
	Resource->GetBulletModelTexture(&bulletmodel, &bullettexture);
	for(int i=0; i<MAX_BULLET; i++){
		BulletIndex[i].SetModel(bulletmodel, 1.0f);
		BulletIndex[i].SetTexture(bullettexture);
	}

	int grenademodel, grenadetexture;
	float model_size = 1.0f;
	WeaponParameter ParamData;

	//リソースとモデルサイズを取得
	Resource->GetWeaponModelTexture(ID_WEAPON_GRENADE, &grenademodel, &grenadetexture);
	if( GameParamInfo->GetWeapon(ID_WEAPON_GRENADE, &ParamData) == 0 ){
		model_size = ParamData.size;
	}

	//適用
	for(int i=0; i<MAX_GRENADE; i++){
		GrenadeIndex[i].SetModel(grenademodel, model_size);
		GrenadeIndex[i].SetTexture(grenadetexture);
	}
}

//! 人追加
//! @param data 人のポイントデータ　（pointdata構造体）
//! @param infodata 参照する人情報のポイントデータ　（〃）
//! @return 成功：データ番号（0以上）　失敗：-1
//! @attention 無効な人の種類番号が指定された場合は 通称：謎人間 が登場します。テクスチャはマップテクスチャ0番が使用され、HPは 0 が指定（＝即死）されます。
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

	//人のモデル番号を取得
	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);

	for(int j=0; j<MAX_HUMAN; j++){
		if( HumanIndex[j].GetDrawFlag() == false ){
			//初期化する
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
			HumanIndex[j].SetDrawFlag(true);
			Humanindexid = j;
			break;
		}
	}

	//初期化できなかったらエラーとして返す
	if( Humanindexid == -1 ){
		return -1;
	}

	//人の設定データが正しく読めていれば〜
	if( GetHumanFlag == 0 ){
		//武器Bを仮想武器として追加
		Weaponindexid = AddVisualWeaponIndex(HumanParam.Weapon[0], true);
		if( Weaponindexid != -1 ){
			//成功すれば配列に記録
			Weapon[0] = &WeaponIndex[Weaponindexid];
		}
		if( data.p1 == 1 ){
			//武器Aの仮想武器として追加
			Weaponindexid = AddVisualWeaponIndex(HumanParam.Weapon[1], true);
			if( Weaponindexid != -1 ){
				//成功すれば配列に記録
				Weapon[1] = &WeaponIndex[Weaponindexid];
			}
		}

		//人に持たせる
		HumanIndex[Humanindexid].SetWeapon(Weapon);
	}

	//発砲フラグを初期化
	Human_ShotFlag[Humanindexid] = false;

	//プレイヤーならば、番号を記録
	if( ( (data.p1 == 1)||(data.p1 == 6) )&&(data.p4 == 0) ){
		Player_HumanID = Humanindexid;
	}

	return Humanindexid;
}

//! 人追加（ゲーム中用）
//! @param px X座標
//! @param py Y座標
//! @param pz Z座標
//! @param rx X軸向き
//! @param paramID 種類番号
//! @param TeamID チーム番号
//! @param WeaponID 武器種類番号の配列（要素数：TOTAL_HAVEWEAPON）
//! @return 成功：データ番号（0以上）　失敗：-1
//! @attention 無効な人の種類番号が指定された場合は 通称：謎人間 が登場します。テクスチャはマップテクスチャ0番が使用され、HPは 0 が指定（＝即死）されます。
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

	//人のモデル番号を取得
	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);

	for(int i=0; i<MAX_HUMAN; i++){
		if( HumanIndex[i].GetDrawFlag() == false ){
			//初期化する
			HumanIndex[i].SetPosData(px, py, pz, rx);
			HumanIndex[i].SetParamData(paramID, -1, 0, TeamID, true);
			if( GetHumanFlag == 0 ){
				//読み込めなければ、前回読み込んだテクスチャ番号を利用
				//読み込めれば、今回読み込むテクスチャ番号を上書き
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
				//今回読み込むテクスチャ番号を上書き
				AddHumanIndex_TextureID = d3dg->GetMapTextureID(0);

				HumanIndex[i].SetTexture(AddHumanIndex_TextureID);
				HumanIndex[i].SetModel(upmodel[0], armmodel, legmodel, walkmodel, runmodel);
			}
			HumanIndex[i].SetDrawFlag(true);
			Humanindexid = i;
			break;
		}
	}

	//初期化できなかったらエラーとして返す
	if( Humanindexid == -1 ){
		return -1;
	}

	if( GetHumanFlag == 0 ){
		//仮想武器を追加
		for(int i=0; i<TOTAL_HAVEWEAPON; i++){
			Weaponindexid = AddVisualWeaponIndex(WeaponID[i], true);
			if( Weaponindexid != -1 ){
				//成功すれば配列に記録
				Weapon[i] = &WeaponIndex[Weaponindexid];
			}
		}

		//人に持たせる
		HumanIndex[Humanindexid].SetWeapon(Weapon);
	}

	return Humanindexid;
}

//! 武器追加
//! @param data 武器のポイントデータ　（pointdata構造体）
//! @return 成功：データ番号（0以上）　失敗：-1
int ObjectManager::AddWeaponIndex(pointdata data)
{
	//武器番号と弾数を仮登録
	int WeaponID = data.p2;
	int nbs = (unsigned char)data.p3;

	//ランダムな武器ならば
	if( data.p1 == 7 ){
		WeaponParameter WeaponParam;

		if( GetRand(2) == 0 ){
			if( GameParamInfo->GetWeapon(data.p2, &WeaponParam) == 1 ){ return -1; }
			nbs = (int)WeaponParam.nbsmax * TOTAL_WEAPON_AUTOBULLET;		//弾数上書き
		}
		else{
			if( GameParamInfo->GetWeapon(data.p3, &WeaponParam) == 1 ){ return -1; }
			WeaponID = data.p3;						//武器番号上書き
			nbs = (int)WeaponParam.nbsmax * TOTAL_WEAPON_AUTOBULLET;		//弾数上書き
		}
	}

	//NONEならば失敗
	if( WeaponID == ID_WEAPON_NONE ){ return -1; }

	//モデルとテクスチャを取得
	int model, texture;
	if( Resource->GetWeaponModelTexture(WeaponID, &model, &texture) == 1 ){
		return -1;
	}

	//設定値を取得
	WeaponParameter WeaponParam;
	if( GameParamInfo->GetWeapon(data.p2, &WeaponParam) == 1 ){ return -1; }

	for(int i=0; i<MAX_WEAPON; i++){
		if( WeaponIndex[i].GetDrawFlag() == false ){
			//初期化する
			WeaponIndex[i].SetPosData(data.x, data.y, data.z, data.r);
			WeaponIndex[i].SetParamData(WeaponID, 0, nbs, true);
			WeaponIndex[i].SetModel(model, WeaponParam.size);
			WeaponIndex[i].SetTexture(texture);
			WeaponIndex[i].SetDrawFlag(true);
			WeaponIndex[i].RunReload();
			return i;
		}
	}
	return -1;
}

//! 仮想武器追加
//! @param WeaponID 武器の種類番号
//! @param loadbullet 弾をロードする
//! @return 成功：データ番号（0以上）　失敗：-1
//! @attention 人が最初から所持している武器を、武器データ（オブジェクト）として追加処理するための関数です。
//! @attention 武器単体として配置する場合は、AddWeaponIndex()関数を使用してください。
int ObjectManager::AddVisualWeaponIndex(int WeaponID, bool loadbullet)
{
	//NONEならば失敗
	if( WeaponID == ID_WEAPON_NONE ){ return -1; }

	//情報を取得
	WeaponParameter WeaponParam;
	if( GameParamInfo->GetWeapon(WeaponID, &WeaponParam) == 1 ){ return -1; }

	//モデルとテクスチャを取得
	int model, texture;
	if( Resource->GetWeaponModelTexture(WeaponID, &model, &texture) == 1 ){
		return -1;
	}

	for(int i=0; i<MAX_WEAPON; i++){
		if( WeaponIndex[i].GetDrawFlag() == false ){
			//初期化
			WeaponIndex[i].SetPosData(0.0f, 0.0f, 0.0f, 0.0f);
			if( loadbullet == false ){
				WeaponIndex[i].SetParamData(WeaponID, 0, 0, true);
			}
			else{
				WeaponIndex[i].SetParamData(WeaponID, WeaponParam.nbsmax, WeaponParam.nbsmax * TOTAL_WEAPON_AUTOBULLET, true);
			}
			WeaponIndex[i].SetModel(model, WeaponParam.size);
			WeaponIndex[i].SetTexture(texture);
			WeaponIndex[i].SetDrawFlag(true);
			return i;
		}
	}
	return -1;
}

//! 小物追加
//! @param data 小物のポイントデータ　（pointdata構造体）
//! @return 成功：データ番号（0以上）　失敗：-1
int ObjectManager::AddSmallObjectIndex(pointdata data)
{
	//モデルとテクスチャを取得
	int model, texture;
	if( Resource->GetSmallObjectModelTexture(data.p2, &model, &texture) == 1 ){
		return -1;
	}

	for(int j=0; j<MAX_SMALLOBJECT; j++){
		if( SmallObjectIndex[j].GetDrawFlag() == false ){
			//初期化
			SmallObjectIndex[j].SetPosData(data.x, data.y, data.z, data.r);
			SmallObjectIndex[j].SetParamData(data.p2, data.p4, true);
			SmallObjectIndex[j].SetModel(model, 5.0f);
			SmallObjectIndex[j].SetTexture(texture);
			SmallObjectIndex[j].SetDrawFlag(true);

			//位置修正フラグが有効ならば、マップと判定
			if( data.p3 !=0 ){
				SmallObjectIndex[j].CollisionMap(CollD);
			}
			return j;
		}
	}
	return -1;
}

//! 出血させる
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
void ObjectManager::SetHumanBlood(float x, float y, float z)
{
	if( GameConfig.GetBloodFlag() == true ){
		for(int i=0; i<MAX_EFFECT; i++){
			if( EffectIndex[i].GetDrawFlag() == false ){
				EffectIndex[i].SetPosData(x+1.0f, y+1.0f, z+1.0f, 0.0f);
				EffectIndex[i].SetParamData(10.0f, (float)M_PI/18*GetRand(18), (int)GAMEFPS * 1, Resource->GetEffectBloodTexture(), EFFECT_FALL, true);
				EffectIndex[i].SetDrawFlag(true);
				break;
			}
		}
		for(int i=0; i<MAX_EFFECT; i++){
			if( EffectIndex[i].GetDrawFlag() == false ){
				EffectIndex[i].SetPosData(x-1.0f, y-1.0f, z-1.0f, 0.0f);
				EffectIndex[i].SetParamData(10.0f, (float)M_PI/18*GetRand(18), (int)GAMEFPS * 1, Resource->GetEffectBloodTexture(), EFFECT_FALL, true);
				EffectIndex[i].SetDrawFlag(true);
				break;
			}
		}
	}
}

//! 人同士の当たり判定
//! @param in_humanA 対象の人オブジェクトA
//! @param in_humanB 対象の人オブジェクトB
//! @return 当たっている：true　当たっていない：false
//! @warning in_humanAとin_humanBで区別はありません。
//! @warning in_humanAとin_humanBでの組み合せは、1フレーム間に1度だけ実行してください。
//! @attention 両クラスは自動的にAddPosOrder()を用いて、お互いを押し合います。
bool ObjectManager::CollideHuman(human *in_humanA, human *in_humanB)
{
	float h1_x, h1_y, h1_z;
	float h2_x, h2_y, h2_z;
	float angle, length;

	//初期化されていないか、死亡して入れば判定しない
	if( in_humanA->GetDrawFlag() == false ){ return false; }
	if( in_humanB->GetDrawFlag() == false ){ return false; }
	if( in_humanA->GetHP() <= 0 ){ return false; }
	if( in_humanB->GetHP() <= 0 ){ return false; }

	//お互いの座標を取得
	in_humanA->GetPosData(&h1_x, &h1_y, &h1_z, NULL);
	in_humanB->GetPosData(&h2_x, &h2_y, &h2_z, NULL);

	//円柱の当たり判定
	if( CollideCylinder(h1_x, h1_y, h1_z, 3.0f, HUMAN_HEIGTH-0.5f, h2_x, h2_y, h2_z, 3.0f, HUMAN_HEIGTH-0.5f, &angle, &length) == true ){
		//めり込んだ分だけ押し出す
		in_humanA->AddPosOrder(angle, 0.0f, length/2);
		in_humanB->AddPosOrder(angle + (float)M_PI, 0.0f, length/2);
		return true;
	}

	return false;
}

//! 弾の当たり判定と処理
//! @param in_bullet 対象の弾オブジェクト
//! @return 当たった：true　当たっていない：false
//! @attention 判定を行う対象は「マップ」「人（頭・上半身・下半身）」「小物」です。
//! @attention 判定に限らず、ダメージ計算や効果音再生まで一貫して行います。
bool ObjectManager::CollideBullet(bullet *in_bullet)
{
	//使用されていない弾丸ならば、処理せずに返す。
	if( in_bullet->GetDrawFlag() == false ){ return false; }

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

	//弾丸の座標を取得し、ベクトルを算出。
	in_bullet->GetPosData(&bx, &by, &bz, &brx, &bry);
	in_bullet->GetParamData(&attacks, &penetration, &speed, &teamid, &humanid);
	vx = cos(brx)*cos(bry);
	vy = sin(bry);
	vz = sin(brx)*cos(bry);

	CheckDist = 0;
	CollideFlag = false;

	for(float TotalDist=0.0f; TotalDist<speed; TotalDist+=CheckDist){
		CheckDist = speed - TotalDist;

		//貫通力が残っていなければ
		if( penetration < 0 ){
			//弾は無効にする
			in_bullet->SetDrawFlag(false);
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

		//マップとの当たり判定
		if( CollD->CheckALLBlockIntersectRay(bvx, bvy, bvz, vx, vy, vz, &id, &face, &Dist, (float)speed - TotalDist) == true ){
			map_Dist = Dist;
		}

		//人との当たり判定
		for(int i=0; i<MAX_HUMAN; i++){
			//使用されていないか、死亡していれば処理しない。
			if( HumanIndex[i].GetDrawFlag() == false ){ continue; }
			if( HumanIndex[i].GetHP() <= 0 ){ continue; }

			//座標を取得
			float ox, oy, oz;
			int h_teamid;
			HumanIndex[i].GetPosData(&ox, &oy, &oz, NULL);
			HumanIndex[i].GetParamData(NULL, NULL, NULL, &h_teamid);

			//同じチーム番号（味方）なら処理しない
			if( h_teamid == teamid ){ continue; }

			//人全体の当たり判定
			if( CollideAABBRay(ox-3.0f, oy, oz-3.0f, ox+3.0f, oy+HUMAN_HEIGTH, oz+3.0f, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){

				//頭の当たり判定
				if( CollideAABBRay(ox-1.0f, oy+17.5f, oz-1.0f, ox+1.0f, oy+20.0f, oz+1.0f, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){
					if( Dist < HumanHead_Dist ){
						HumanHead_id = i;
						HumanHead_Dist = Dist;
					}
				}

				//上半身と当たり判定
				if( CollideAABBRay(ox-3.0f, oy+10.0f, oz-3.0f, ox+3.0f, oy+17.5f, oz+3.0f, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){
					if( Dist < HumanUp_Dist ){
						HumanUp_id = i;
						HumanUp_Dist = Dist;
					}
				}

				//足と当たり判定
				if( CollideAABBRay(ox-3.0f, oy, oz-3.0f, ox+3.0f, oy+10.0f, oz+3.0f, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){
					if( Dist < HumanLeg_Dist ){
						HumanLeg_id = i;
						HumanLeg_Dist = Dist;
					}
				}

			}
		}

		//小物との当たり判定
		for(int i=0; i<MAX_SMALLOBJECT; i++){
			//使用されていなければ処理しない
			if( SmallObjectIndex[i].GetDrawFlag() == false ){ continue; }

			//座標を取得
			float ox, oy, oz;
			int id;
			float decide;
			SmallObjectIndex[i].GetPosData(&ox, &oy, &oz, NULL);
			SmallObjectIndex[i].GetParamData(&id, NULL);

			//当たり判定の大きさを取得
			if( id == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
				decide = (float)MIFdata->GetAddSmallobjectDecide()*SMALLOBJECT_SCALE;
			}
			else{
				SmallObjectParameter Param;
				GameParamInfo->GetSmallObject(id, &Param);
				decide = (float)Param.decide*SMALLOBJECT_SCALE;
			}

			//当たり判定
			if( CollideSphereRay(ox, oy, oz, decide, bvx, bvy, bvz, vx, vy, vz, &Dist, (float)speed - TotalDist) == true ){
				if( Dist < SmallObject_Dist ){
					SmallObject_id = i;
					SmallObject_Dist = Dist;
				}
			}
		}

		//マップとの衝突距離が最短ならば〜
		if( (map_Dist <= speed)&&(map_Dist < HumanHead_Dist)&&(map_Dist < HumanUp_Dist)&&(map_Dist < HumanLeg_Dist)&&(map_Dist < SmallObject_Dist) ){
			//弾がマップに当たった処理
			HitBulletMap(bx + vx*(map_Dist+TotalDist), by + vy*(map_Dist+TotalDist), bz + vz*(map_Dist+TotalDist));

			int Penetration_Dist;

			//マップにめり込んでいる量を調べる
			//　　最終地点がマップにめり込んでいるならば
			if( CollD->CheckALLBlockIntersectRay(bx + vx*speed, by + vy*speed, bz + vz*speed, vx*-1, vy*-1, vz*-1, &id, &face, &Dist, (float)speed - map_Dist+TotalDist) == true ){
				//発射地点と最終地点、それぞれからの激突点から貫通距離を求める
				Penetration_Dist = (int)( ((float)speed - map_Dist+TotalDist - Dist) / 2 );
			}
			else{
				//衝突点から最終地点まで移動距離
				Penetration_Dist = (int)( ((float)speed - map_Dist+TotalDist) / 2 );
			}

			//攻撃力と貫通力を計算
			for(int i=0; i<Penetration_Dist; i++){
				attacks = (int)((float)attacks * 0.6f);
			}
			penetration -= Penetration_Dist;

			CheckDist = map_Dist + 1.0f;
		}

		//人の頭との衝突距離が最短ならば〜
		if( (HumanHead_Dist <= speed)&&(HumanHead_Dist < map_Dist)&&(HumanHead_Dist < HumanUp_Dist)&&(HumanHead_Dist < HumanLeg_Dist)&&(HumanHead_Dist < SmallObject_Dist) ){
			//人に当たった処理
			HitBulletHuman(HumanHead_id, 0, bx + vx*(HumanHead_Dist+TotalDist), by + vy*(HumanHead_Dist+TotalDist), bz + vz*(HumanHead_Dist+TotalDist), brx, attacks, humanid);

			//攻撃力と貫通力を計算
			attacks = (int)((float)attacks * 0.6f);
			penetration -= 1;

			CheckDist = HumanHead_Dist + 1.0f;
		}

		//人の上半身との衝突距離が最短ならば〜
		if( (HumanUp_Dist <= speed)&&(HumanUp_Dist < map_Dist)&&(HumanUp_Dist < HumanHead_Dist)&&(HumanUp_Dist < HumanLeg_Dist)&&(HumanUp_Dist < SmallObject_Dist) ){
			//人に当たった処理
			HitBulletHuman(HumanUp_id, 1, bx + vx*(HumanUp_Dist+TotalDist), by + vy*(HumanUp_Dist+TotalDist), bz + vz*(HumanUp_Dist+TotalDist), brx, attacks, humanid);

			//攻撃力と貫通力を計算
			attacks = (int)((float)attacks * 0.6f);
			penetration -= 1;

			CheckDist = HumanUp_Dist + 1.0f;
		}

		//人の足との衝突距離が最短ならば〜
		if( (HumanLeg_Dist <= speed)&&(HumanLeg_Dist < map_Dist)&&(HumanLeg_Dist < HumanHead_Dist)&&(HumanLeg_Dist < HumanUp_Dist)&&(HumanLeg_Dist < SmallObject_Dist) ){
			//人に当たった処理
			HitBulletHuman(HumanLeg_id, 2, bx + vx*(HumanLeg_Dist+TotalDist), by + vy*(HumanLeg_Dist+TotalDist), bz + vz*(HumanLeg_Dist+TotalDist), brx, attacks, humanid);

			//攻撃力と貫通力を計算
			attacks = (int)((float)attacks * 0.7f);
			penetration -= 1;

			CheckDist = HumanLeg_Dist + 1.0f;
		}

		//小物との衝突距離が最短ならば〜
		if( (SmallObject_Dist <= speed)&&(SmallObject_Dist < map_Dist)&&(SmallObject_Dist < HumanHead_Dist)&&(SmallObject_Dist < HumanUp_Dist)&&(SmallObject_Dist < HumanLeg_Dist) ){
			//小物に当たった処理
			HitBulletSmallObject(SmallObject_id, bx + vx*(SmallObject_Dist+TotalDist), by + vy*(SmallObject_Dist+TotalDist), bz + vz*(SmallObject_Dist+TotalDist), attacks);

			//小物の種類番号を取得
			int id;
			int decide;
			SmallObjectIndex[SmallObject_id].GetParamData(&id, NULL);

			//小物の設定値を取得
			SmallObjectParameter Param;
			GameParamInfo->GetSmallObject(id, &Param);
			decide = (int)( (float)Param.decide*SMALLOBJECT_SCALE );

			//貫通力を計算
			for(int i=0; i<decide; i++){
				attacks = (int)((float)attacks * 0.7f);
			}

			CheckDist = SmallObject_Dist + 1.0f;
		}

		//設定を適用（特に攻撃力・貫通力）
		in_bullet->SetParamData(attacks, penetration, speed, teamid, humanid, false);
	}

	return CollideFlag;
}

//! 弾がマップに当たった処理
//! @param x 着弾X座標
//! @param y 着弾Y座標
//! @param z 着弾Z座標
void ObjectManager::HitBulletMap(float x, float y, float z)
{
	//エフェクト（煙）を表示
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetDrawFlag() == false ){
			EffectIndex[i].SetPosData(x, y, z, 0.0f);
			EffectIndex[i].SetParamData(5.0f, (float)M_PI/18*GetRand(18), (int)(GAMEFPS * 0.5f), Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY, true);
			EffectIndex[i].SetDrawFlag(true);
			break;
		}
	}

	//効果音を再生
	GameSound->HitMap(x, y, z);
}

//! 弾が人に当たった処理
//! @param HitHuman_id 人の番号
//! @param Hit_id 被弾箇所（頭：0　胴体：1　足：2）
//! @param x 被弾X座標
//! @param y 被弾Y座標
//! @param z 被弾Z座標
//! @param brx 水平角度
//! @param attacks 攻撃力
//! @param Shothuman_id 発射した人の番号
void ObjectManager::HitBulletHuman(int HitHuman_id, int Hit_id, float x, float y, float z, float brx, int attacks, int Shothuman_id)
{
	//使用されていないか、死亡していれば処理しない。
	if( HumanIndex[HitHuman_id].GetDrawFlag() == false ){ return; }
	if( HumanIndex[HitHuman_id].GetHP() <= 0 ){ return; }

	//人にダメージと衝撃を与える
	if( Hit_id == 0 ){ HumanIndex[HitHuman_id].HitBulletHead(attacks); }
	if( Hit_id == 1 ){ HumanIndex[HitHuman_id].HitBulletUp(attacks); }
	if( Hit_id == 2 ){ HumanIndex[HitHuman_id].HitBulletLeg(attacks); }
	HumanIndex[HitHuman_id].AddPosOrder(brx, 0.0f, 2.0f);

	//エフェクト（血）を表示
	SetHumanBlood(x, y, z);

	//効果音を再生
	GameSound->HitHuman(x, y, z);

	//弾を発射した人の成果に加算
	Human_ontarget[Shothuman_id] += 1;
	if( Hit_id == 0 ){ Human_headshot[Shothuman_id] += 1; }
	if( HumanIndex[HitHuman_id].GetHP() <= 0 ){
		Human_kill[Shothuman_id] += 1;
	}
}

//! 弾が小物に当たった処理
//! @param HitSmallObject_id 小物の番号
//! @param x 着弾X座標
//! @param y 着弾Y座標
//! @param z 着弾Z座標
//! @param attacks 攻撃力
void ObjectManager::HitBulletSmallObject(int HitSmallObject_id, float x, float y, float z, int attacks)
{
	int hp;

	//使用されていなければ処理しない。
	if( SmallObjectIndex[HitSmallObject_id].GetDrawFlag() == false ){ return; }

	//体力がなければ処理しない
	hp = SmallObjectIndex[HitSmallObject_id].GetHP();
	if( hp <= 0 ){ return; }

	//小物にダメージを与える
	SmallObjectIndex[HitSmallObject_id].HitBullet(attacks);

	//エフェクト（煙）を表示
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetDrawFlag() == false ){
			EffectIndex[i].SetPosData(x, y, z, 0.0f);
			EffectIndex[i].SetParamData(5.0f, (float)M_PI/18*GetRand(18), (int)(GAMEFPS * 0.5f), Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY, true);
			EffectIndex[i].SetDrawFlag(true);
			break;
		}
	}

	//効果音を再生
	int id;
	SmallObjectIndex[HitSmallObject_id].GetParamData(&id, NULL);
	GameSound->HitSmallObject(x, y, z, id);
}

//! 武器を拾う
//! @param in_human 対象の人オブジェクト
//! @param in_weapon 対象の武器オブジェクト
void ObjectManager::PickupWeapon(human *in_human, weapon *in_weapon)
{
	//無効な人ならば処理しない
	if( in_human->GetDrawFlag() == false ){ return; }
	if( in_human->GetHP() <= 0 ){ return; }

	//初期化されている武器で、かつ誰も使っていない武器ならば〜
	if( (in_weapon->GetDrawFlag() == true)&&(in_weapon->GetUsingFlag() == false) ){
		float human_x, human_y, human_z;
		float weapon_x, weapon_y, weapon_z;
		float x, z;
		float r;

		//人と武器の座標を取得
		in_human->GetPosData(&human_x, &human_y, &human_z, NULL);
		in_weapon->GetPosData(&weapon_x, &weapon_y, &weapon_z, NULL);

		//高さが範囲内ならば
		if( (human_y-2.0f <= weapon_y)&&(human_y+16.0f > weapon_y) ){
			//距離を計算
			x = human_x - weapon_x;
			z = human_z - weapon_z;
			r = x*x + z*z;
			//距離も範囲内ならば、拾わせる。
			if( r < (5.0f * 5.0f) ){
				in_human->PickupWeapon(in_weapon);
			}
		}
	}
}

//! オブジェクトを解放
void ObjectManager::CleanupPointDataToObject()
{
	for(int i=0; i<MAX_HUMAN; i++){
		HumanIndex[i].SetDrawFlag(false);
	}

	for(int i=0; i<MAX_WEAPON; i++){
		WeaponIndex[i].SetDrawFlag(false);
	}

	for(int i=0; i<MAX_SMALLOBJECT; i++){
		SmallObjectIndex[i].SetDrawFlag(false);
	}

	for(int i=0; i<MAX_BULLET; i++){
		BulletIndex[i].SetDrawFlag(false);
	}

	for(int i=0; i<MAX_GRENADE; i++){
		GrenadeIndex[i].SetDrawFlag(false);
	}

	for(int i=0; i<MAX_EFFECT; i++){
		EffectIndex[i].SetDrawFlag(false);
	}


	if( Resource != NULL ){
		Resource->CleanupHumanTexture();
	}
}

//! ポイントデータを元にオブジェクトを配置
void ObjectManager::LoadPointData()
{
	Player_HumanID = 0;

	/*
	//人情報ポイントを探す
	for(int i=0; i<PointData->GetTotaldatas(); i++){
		pointdata data;
		PointData->Getdata(&data, i);

		if( data.p1 == 4 ){
			//人のテクスチャを登録
			Resource->AddHumanTexture(data.p2);
		}
	}
	*/

	//人・武器・小物を探す
	for(int i=0; i<PointData->GetTotaldatas(); i++){
		pointdata data;
		PointData->Getdata(&data, i);

		//人ならば
		if( (data.p1 == 1)||(data.p1 == 6) ){
			pointdata humaninfodata;

			//人情報ポイントを探す
			if( PointData->SearchPointdata(&humaninfodata, 0x01 + 0x08, 4, 0, 0, data.p2, 0) == 0 ){
				//continue;

				//人情報ポイントが見つからなかったら、とりあえず「特殊 黒 A」として追加。（バグの再現）

				HumanParameter HumanParam;
				int Weapon[TOTAL_HAVEWEAPON];
				GameParamInfo->GetHuman(0, &HumanParam);
				for(int j=0; j<TOTAL_HAVEWEAPON; j++){
					Weapon[j] = HumanParam.Weapon[j];
				}

				//人のテクスチャを登録
				Resource->AddHumanTexture(0);

				//人を追加
				if( data.p1 == 6 ){
					Weapon[1] = ID_WEAPON_NONE;
				}

				//プレイヤーならば、番号を記録
				if( (data.p4 == 0)&&(Player_HumanID == 0) ){
					Player_HumanID = AddHumanIndex(data.x, data.y, data.z, data.r, 0, i, Weapon);
				}
				else{
					AddHumanIndex(data.x, data.y, data.z, data.r, 0, i, Weapon);
				}
			}
			else{
				//人のテクスチャを登録
				Resource->AddHumanTexture(humaninfodata.p2);

				//人として追加
				AddHumanIndex(data, humaninfodata);
			}
		}

		//武器ならば
		if( (data.p1 == 2)||(data.p1 == 7) ){
			AddWeaponIndex(data);
		}

		//小物ならば
		if( data.p1 == 5 ){
			AddSmallObjectIndex(data);
		}
	}
}

//! プレイヤー番号を取得
//! @return プレイヤーのデータ番号
int ObjectManager::GetPlayerID()
{
	return Player_HumanID;
}

//! プレイヤー番号を設定
//! @param id プレイヤーのデータ番号
void ObjectManager::SetPlayerID(int id)
{
	Player_HumanID = id;
}

//! 指定したデータ番号のhumanクラスを取得
//! @param id データ番号
//! @return 人オブジェクトのポインタ　（無効なデータ番号で NULL）
human* ObjectManager::GeHumanObject(int id)
{
	if( (id < 0)||(MAX_HUMAN-1 < id) ){ return NULL; }
	return &(HumanIndex[id]);
}

//! プレイヤーのhumanクラスを取得
//! @return 人オブジェクト（プレイヤー）のポインタ
human* ObjectManager::GetPlayerHumanObject()
{
	return GeHumanObject(Player_HumanID);
}

//! 指定したデータ番号のweaponクラスを取得
//! @param id データ番号
//! @return 武器オブジェクトのポインタ　（無効なデータ番号で NULL）
weapon* ObjectManager::GeWeaponObject(int id)
{
	if( (id < 0)||(MAX_WEAPON-1 < id) ){ return NULL; }
	return &(WeaponIndex[id]);
}

//! 指定したデータ番号のbulletクラスを取得
//! @param id データ番号
//! @return 弾オブジェクトのポインタ　（無効なデータ番号で NULL）
bullet* ObjectManager::GeBulletObject(int id)
{
	if( (id < 0)||(MAX_BULLET-1 < id) ){ return NULL; }
	return &(BulletIndex[id]);
}

//! 使用されていないbulletクラスを取得
//! @return 現在未使用の弾オブジェクトのポインタ　（失敗すると NULL）
bullet* ObjectManager::GetNewBulletObject()
{
	for(int i=0; i<MAX_BULLET; i++){
		if( BulletIndex[i].GetDrawFlag() == false ){
			return &(BulletIndex[i]);
		}
	}
	return NULL;
}

//! 使用されていないgrenadeクラスを取得
//! @return 現在未使用の手榴弾オブジェクトのポインタ　（失敗すると NULL）
grenade* ObjectManager::GetNewGrenadeObject()
{
	for(int i=0; i<MAX_GRENADE; i++){
		if( GrenadeIndex[i].GetDrawFlag() == false ){
			return &(GrenadeIndex[i]);
		}
	}
	return NULL;
}

//! 人を検索
//! @param p4 検索対象の認識番号
//! @return 該当したhumanクラスのポインタ　（見つからない場合はNULL）
//! @attention 複数該当する場合、最初に該当したデータを返します。
human* ObjectManager::SearchHuman(signed char p4)
{
	signed char humanp4;

	for(int i=0; i<MAX_HUMAN; i++){
		//使われていない人ならば処理しない
		if( HumanIndex[i].GetDrawFlag() == false ){ continue; }

		//第4パラメータを取得
		HumanIndex[i].GetParamData(NULL, NULL, &humanp4, NULL);

		//指定されたp4と一致すれば返す
		if( humanp4 == p4 ){
			return &(HumanIndex[i]);
		}
	}
	return NULL;
}

//! 小物を検索
//! @param p4 検索対象の認識番号
//! @return 該当したsmallobjectクラスのポインタ　（見つからない場合はNULL）
//! @attention 複数該当する場合、最初に該当したデータを返します。
smallobject* ObjectManager::SearchSmallobject(signed char p4)
{
	signed char smallobjectp4;

	for(int i=0; i<MAX_SMALLOBJECT; i++){
		//使われていない人ならば処理しない
		//　【破壊積みのオブジェクトも判定するため、無効】
		//if( SmallObjectIndex[i].GetDrawFlag() == false ){ continue; }

		//第4パラメータを取得
		SmallObjectIndex[i].GetParamData(NULL, &smallobjectp4);

		//指定されたp4と一致すれば返す
		if( smallobjectp4 == p4 ){
			return &(SmallObjectIndex[i]);
		}
	}
	return NULL;
}

//! 前進（走り）を実行
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
void ObjectManager::MoveForward(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//オブジェクトにフラグを設定
	HumanIndex[human_id].SetMoveForward();

	//走る足音追加
	float posx, posy, posz;
	int teamid;
	HumanIndex[human_id].GetPosData(&posx, &posy, &posz, NULL);
	HumanIndex[human_id].GetParamData(NULL, NULL, NULL, &teamid);
	GameSound->SetFootsteps(posx, posy, posz, teamid);
}

//! 後退を実行
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
void ObjectManager::MoveBack(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//オブジェクトにフラグを設定
	HumanIndex[human_id].SetMoveBack();

	//走る足音追加
	float posx, posy, posz;
	int teamid;
	HumanIndex[human_id].GetPosData(&posx, &posy, &posz, NULL);
	HumanIndex[human_id].GetParamData(NULL, NULL, NULL, &teamid);
	GameSound->SetFootsteps(posx, posy, posz, teamid);
}

//! 左走りを実行
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
void ObjectManager::MoveLeft(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//オブジェクトにフラグを設定
	HumanIndex[human_id].SetMoveLeft();

	//走る足音追加
	float posx, posy, posz;
	int teamid;
	HumanIndex[human_id].GetPosData(&posx, &posy, &posz, NULL);
	HumanIndex[human_id].GetParamData(NULL, NULL, NULL, &teamid);
	GameSound->SetFootsteps(posx, posy, posz, teamid);
}

//! 右走りを実行
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
void ObjectManager::MoveRight(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//オブジェクトにフラグを設定
	HumanIndex[human_id].SetMoveRight();

	//走る足音追加
	float posx, posy, posz;
	int teamid;
	HumanIndex[human_id].GetPosData(&posx, &posy, &posz, NULL);
	HumanIndex[human_id].GetParamData(NULL, NULL, NULL, &teamid);
	GameSound->SetFootsteps(posx, posy, posz, teamid);
}

//! 歩きを実行
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
void ObjectManager::MoveWalk(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//オブジェクトにフラグを設定
	HumanIndex[human_id].SetMoveWalk();
}

//! ジャンプ
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
void ObjectManager::MoveJump(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	HumanIndex[human_id].Jump();
}

//! 発砲
//! @param human_id 発砲する人番号
//! @return 通常弾発射：1　手榴弾発射：2　失敗：0
int ObjectManager::ShotWeapon(int human_id)
{
	//値の範囲をチェック
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

	//人の座標と角度を取得
	MyHuman->GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	MyHuman->GetParamData(NULL, NULL, NULL, &teamid);
	MyHuman->GetRxRy(&rotation_x, &armrotation_y);

	//対象者がプレイヤー自身か判定
	if( MyHuman == &(HumanIndex[Player_HumanID]) ){
		playerflag = true;
	}
	else{
		playerflag = false;
	}

	//弾の発射を要求
	if( MyHuman->ShotWeapon(&weapon_paramid, &GunsightErrorRange) == false ){ return 0; }

	//武器の情報を取得
	if( GameParamInfo->GetWeapon(weapon_paramid, &ParamData) != 0 ){ return 0; }

	//誤差の範囲を計算
	int ErrorRange;
	ErrorRange = GunsightErrorRange;
	if( ErrorRange < ParamData.ErrorRangeMIN ){ ErrorRange = ParamData.ErrorRangeMIN; }

	//手榴弾か判定
	if( weapon_paramid == ID_WEAPON_GRENADE ){ grenadeflag = true; }
	else{ grenadeflag = false; }

	class bullet* newbullet;
	class grenade* newgrenade;

	//（ショットガンなど）発射する弾の数分繰り返す
	for(int i=0; i<ParamData.burst; i++){
		//発射角度を決定
		float rx, ry;
		rx = rotation_x*-1 + (float)M_PI/2;
		ry = armrotation_y;

		//誤差分 加算する
		float a;
		a = (float)M_PI/180 * GetRand(360);
		rx += cos(a)*ErrorRange * ((float)M_PI/180*0.15f);
		ry += sin(a)*ErrorRange * ((float)M_PI/180*0.15f);

		//手榴弾でなければ
		if( grenadeflag == false ){
			int attacks;

			//（ショットガンなど）発射する弾が複数あれば
			if( ParamData.burst > 1 ){
				//1個の弾当たりの攻撃力を算出
				//　　全弾合わせて、攻撃力の2倍になるようにする。
				attacks = (int)( (float)ParamData.attacks / ((float)ParamData.burst/2) );
			}
			else{
				//そのまま攻撃力へ反映
				attacks = ParamData.attacks;
			}

			//発射する未使用のオブジェクトを取得
			newbullet = GetNewBulletObject();
			if( newbullet == NULL ){ break; }

			//銃弾を発射
			newbullet->SetPosData(pos_x, pos_y + WEAPONSHOT_HEIGHT, pos_z, rx, ry);
			newbullet->SetParamData(attacks, ParamData.penetration, ParamData.speed * BULLET_SPEEDSCALE, teamid, human_id, true);
			newbullet->SetDrawFlag(true);
		}
		else{
			//発射する未使用のオブジェクトを取得
			newgrenade = GetNewGrenadeObject();
			if( newgrenade == NULL ){ break; }

			//手榴弾発射
			newgrenade->SetPosData(pos_x, pos_y + WEAPONSHOT_HEIGHT, pos_z, rx, ry);
			newgrenade->SetParamData(8.0f, human_id, true);
			newgrenade->SetDrawFlag(true);
		}

		//誤差を加算（ショットガン用）
		ErrorRange += ParamData.ErrorRangeMIN;
	}

	//手榴弾でなければ
	if( grenadeflag == false ){
		//発砲フラグを設定
		Human_ShotFlag[human_id] = true;
	}

	if( ParamData.soundvolume > 0 ){
		//銃声を再生
		GameSound->ShotWeapon(pos_x, pos_y + WEAPONSHOT_HEIGHT, pos_z, weapon_paramid, teamid, playerflag);
	}

	if( grenadeflag == true ){
		return 2;
	}
	return 1;
}

//! マズルフラッシュを設定
//! @param humanid 人の番号
//! @attention この関数の呼び出しタイミングを誤ると、銃口に対してマズルフラッシュが合いません。
void ObjectManager::ShotWeaponEffect(int humanid)
{
	float pos_x, pos_y, pos_z;
	float rotation_x, armrotation_y;
	int weapon_paramid;
	WeaponParameter ParamData;
	float x, y, z;

	//人の座標と角度を取得
	HumanIndex[humanid].GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	HumanIndex[humanid].GetRxRy(&rotation_x, &armrotation_y);

	//武器の情報を取得
	weapon_paramid = HumanIndex[humanid].GetMainWeaponTypeNO();
	if( GameParamInfo->GetWeapon(weapon_paramid, &ParamData) != 0 ){ return; }

	//マズルフラッシュと煙のサイズを決定
	float flashsize = 3.0f;
	if( ParamData.silencer == true ){
		flashsize = 1.0f;
	}

	//行列でエフェクト座標を計算
	d3dg->SetWorldTransformHumanWeapon(pos_x, pos_y + 16.0f, pos_z, ParamData.flashx/10, ParamData.flashy/10, ParamData.flashz/10, rotation_x, armrotation_y*-1, 1.0f);
	d3dg->GetWorldTransformPos(&x, &y, &z);
	d3dg->ResetWorldTransform();

	//マズルフラッシュ描画
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetDrawFlag() == false ){
			//エフェクト設定
			EffectIndex[i].SetPosData(x, y, z, 0.0f);
			EffectIndex[i].SetParamData(flashsize, (float)M_PI/18*GetRand(18), 1, Resource->GetEffectMflashTexture(), EFFECT_NORMAL, true);
			EffectIndex[i].SetDrawFlag(true);
			break;
		}
	}

	//エフェクト（煙）の表示
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetDrawFlag() == false ){
			//エフェクト設定
			EffectIndex[i].SetPosData(x, y, z, 0.0f);
			EffectIndex[i].SetParamData(flashsize, (float)M_PI/18*GetRand(18), (int)(GAMEFPS/3), Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION, true);
			EffectIndex[i].SetDrawFlag(true);
			break;
		}
	}

	//行列でエフェクト座標を計算
	d3dg->SetWorldTransformHumanWeapon(pos_x, pos_y + 16.0f, pos_z, ParamData.yakkyou_px/10, ParamData.yakkyou_py/10, ParamData.yakkyou_pz/10, rotation_x, armrotation_y*-1, 1.0f);
	d3dg->GetWorldTransformPos(&x, &y, &z);
	d3dg->ResetWorldTransform();

	//薬莢描画
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetDrawFlag() == false ){
			//エフェクト設定
			EffectIndex[i].SetPosData(x, y, z, 0.0f);
			EffectIndex[i].SetParamData(/*3.0f*/2.0f, (float)M_PI/18*GetRand(18), (int)(GAMEFPS/2), Resource->GetEffectYakkyouTexture(), EFFECT_ROTATION | EFFECT_FALL, true);
			EffectIndex[i].SetDrawFlag(true);
			break;
		}
	}
}

//! 武器をリロード
//! @param human_id 対象の人番号
void ObjectManager::ReloadWeapon(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//無効な人ならば処理しない
	if( HumanIndex[human_id].GetDrawFlag() == false ){ return; }
	if( HumanIndex[human_id].GetHP() <= 0 ){ return; }

	//リロードを実行
	if( HumanIndex[human_id].ReloadWeapon() == true ){
		float x, y, z;
		int id;

		//人の座標とチーム番号を取得
		HumanIndex[human_id].GetPosData(&x, &y, &z, NULL);
		HumanIndex[human_id].GetParamData(NULL, NULL, NULL, &id);
		y += 16.0f;

		//音源を配置
		GameSound->ReloadWeapon(x, y, z, id);
	}
}

//! 武器を切り替える（持ち替える）
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
//! @param id 持ち替える武器　（-1 で次の武器）
void ObjectManager::ChangeWeapon(int human_id, int id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	HumanIndex[human_id].ChangeWeapon(id);
}

//! 武器を捨てる
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
//! @return 成功：true　失敗：false
bool ObjectManager::DumpWeapon(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return false; }

	return HumanIndex[human_id].DumpWeapon();
}

//! スコープモードを切り替え
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
void ObjectManager::ChangeScopeMode(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	if( HumanIndex[human_id].GetScopeMode() == 0 ){	//スコープを使用していなければ、スコープを設定
		HumanIndex[human_id].SetEnableScope();
	}
	else{											//使用中なら、解除
		HumanIndex[human_id].SetDisableScope();
	}
}

//! 武器のショットモード切り替え
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
//! @return 成功：0　失敗：1
int ObjectManager::ChangeShotMode(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return 1; }

	return HumanIndex[human_id].ChangeShotMode();
}

//! 裏技・所持している武器の弾を追加
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
//! @return 成功：true　失敗：false
bool ObjectManager::CheatAddBullet(int human_id)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return false; }

	int selectweapon;
	weapon *weapon[TOTAL_HAVEWEAPON];
	int id_param, lnbs, nbs;
	WeaponParameter ParamData;

	//所持している武器を取得
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}
	HumanIndex[human_id].GetWeapon(&selectweapon, weapon);

	//何かしらの武器を持っていれば
	if( weapon[selectweapon] != NULL ){
		//武器の種類と弾数、武器の設定値を取得
		weapon[selectweapon]->GetParamData(&id_param, &lnbs, &nbs);
		if( GameParamInfo->GetWeapon(id_param, &ParamData) == 0 ){
			//最大弾数分加算し、適用
			nbs += ParamData.nbsmax;
			weapon[selectweapon]->ResetWeaponParam(Resource, id_param, lnbs, nbs);
			return true;
		}
	}

	return false;
}

//! 裏技・所持している武器を変更
//! @param human_id 人の番号（0〜MAX_HUMAN-1）
//! @param new_weaponID 新たに設定する武器の種類番号
//! @return 成功：true　失敗：false
bool ObjectManager::CheatNewWeapon(int human_id, int new_weaponID)
{
	//値の範囲をチェック
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return false; }
	if( (new_weaponID < 0)||(TOTAL_PARAMETERINFO_WEAPON <= new_weaponID) ){ return false; }

	human *myHuman = &(HumanIndex[human_id]);

	HumanParameter humandata;
	int selectweapon;
	weapon *weapon[TOTAL_HAVEWEAPON];
	int id_param, lnbs, nbs;

	//ゾンビならば処理しない
	myHuman->GetParamData(&id_param, NULL, NULL, NULL);
	GameParamInfo->GetHuman(id_param, &humandata);
	if( humandata.type == 2 ){
		return false;
	}

	//所持している武器を取得
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}
	myHuman->GetWeapon(&selectweapon, weapon);

	//現在武器を所有しておらず、新たに持たせる武器もないなら
	if( (weapon[selectweapon] == NULL)&&(new_weaponID == ID_WEAPON_NONE) ){
		return false;		//処理自体が無意味
	}

	if( weapon[selectweapon] == NULL ){		//武器を所有していなければ
		int dataid = -1;

		//新しい武器を配置
		dataid = AddVisualWeaponIndex(new_weaponID, false);

		//武器が配置できれば、武器を拾わせる
		if( dataid != -1 ){
			myHuman->PickupWeapon( &(WeaponIndex[dataid]) );
			return true;
		}
	}
	else{									//武器を所有していれば
		if( new_weaponID == ID_WEAPON_NONE ){			//武器を消すなら
			//一度武器を捨てた上で、その武器を削除
			if( myHuman->DumpWeapon() == true ){
				weapon[selectweapon]->SetDrawFlag(false);
				return true;
			}
		}
		else{											//武器を変更するなら
			//武器設定を取得（弾数設定を引き継ぐため）
			weapon[selectweapon]->GetParamData(NULL, &lnbs, &nbs);

			//武器設定を適用
			weapon[selectweapon]->ResetWeaponParam(Resource, new_weaponID, lnbs, nbs);

			return true;
		}
	}

	return false;
}

//! ゾンビの攻撃を受けるか判定
//! @param MyHuman 攻撃する人オブジェクト（ゾンビ側）のポインタ
//! @param EnemyHuman 攻撃を受けた人オブジェクトのポインタ
//! @return 成立：true　不成立：false
//! @warning MyHuman はゾンビ以外も指定できます。<b>ゾンビかどうかは判定しない</b>ため、ゾンビであるか予め確認しておく必要があります。
//! @attention 判定のみを実施します。この判定が成立したら HitZombieAttack()関数 を呼び出してください。 
bool ObjectManager::CheckZombieAttack(human* MyHuman, human* EnemyHuman)
{
	if( MyHuman == NULL ){ return false; }
	if( EnemyHuman == NULL ){ return false; }

	//使用されていないか、死亡していれば処理しない。
	if( MyHuman->GetDrawFlag() == false ){ return false; }
	if( MyHuman->GetHP() <= 0 ){ return false; }
	if( EnemyHuman->GetDrawFlag() == false ){ return false; }
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

	//味方ならば処理しない
	if( mteam == tteam ){ return false; }

	//攻撃ポイント（腕の先端）を求める
	AttackPoint_x = mx + cos(mrx*-1 + (float)M_PI/2) * 2.0f;
	AttackPoint_y = my + VIEW_HEIGHT - 0.5f;
	AttackPoint_z = mz + sin(mrx*-1 + (float)M_PI/2) * 2.0f;
	ax = AttackPoint_x - tx;
	az = AttackPoint_z - tz;

	//敵（攻撃対象）が攻撃ポイントに触れていれば、当たっている
	if( (ax*ax + az*az) < 3.3f*3.3f ){
		if( (AttackPoint_y >= ty)&&(AttackPoint_y <= (ty + HUMAN_HEIGTH)) ){
			return true;
		}
	}

	return false;
}

//! ゾンビの攻撃を受けた処理
//! @param EnemyHuman 攻撃を受けた人オブジェクトのポインタ
void ObjectManager::HitZombieAttack(human* EnemyHuman)
{
	if( EnemyHuman == NULL ){ return; }

	//使用されていないか、死亡していれば処理しない。
	if( EnemyHuman->GetDrawFlag() == false ){ return; }
	if( EnemyHuman->GetHP() <= 0 ){ return; }

	float tx, ty, tz;

	EnemyHuman->GetPosData(&tx, &ty, &tz, NULL);
	ty += VIEW_HEIGHT;

	//ダメージなどを計算
	EnemyHuman->HitZombieAttack();

	//エフェクト（血）を表示
	SetHumanBlood(tx, ty, tz);

	//効果音を再生
	GameSound->HitHuman(tx, ty, tz);
}

//! ゲームクリアー・ゲームオーバーの判定
//! @return ゲームクリアー：1　ゲームオーバー：2　判定なし：0
//! @attention ゲームクリア―とゲームオーバーが同時に成立する条件では、本家XOPSと同様に「ゲームクリアー」と判定されます。
int ObjectManager::CheckGameOverorComplete()
{
	//メモ：
	//
	//　本来は、ゲームオーバー判定を先に行い、次にゲームクリアー判定を実装した方が効率的です。
	//　プレイヤーのHPが 0 ならば即ゲームオーバーとして判定し、敵の中でHPが残っている者を見つけた時点で 判定なし、
	//　2つの条件に当てはまらなければ、自動的にゲームクリアーとなります。
	//
	//　しかし、本家XOPSはゲームクリアーの判定を優先するため、先にゲームクリアーの判定を行っています。
	//　生きている敵の数を（総HPとして）数え、敵の数（総HP）が 0 ならば、ゲームクリアーと判定します。
	//　まぁこのように1つの関数で処理しようとせずに、ゲームクリアーとゲームオーバーで関数自体を分けても良いのですがね・・。

	//ゲームクリアー判定
	int MyTeamid, teamid;
	int TotalEnemyHP = 0;
	HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &MyTeamid);	//プレイヤーのチーム番号を取得
	for(int i=0; i<MAX_HUMAN; i++){
		//初期化されていなければ処理しない
		if( HumanIndex[i].GetDrawFlag() == false ){ continue; }

		//調べる対象のチーム番号を取得
		HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);

		//異なるチーム番号（＝敵）ならば
		if( teamid != MyTeamid ){
			//生きていれば、敵のHPとして加算
			//if( HumanIndex[i].GetHP() > 0 ){
			if( HumanIndex[i].GetDeadFlag() == false ){
				TotalEnemyHP += HumanIndex[i].GetHP();
			}
		}
	}
	if( TotalEnemyHP == 0 ){	//全敵のHPが 0 ならば
		return 1;
	}


	//ゲームオーバー判定
	if( HumanIndex[Player_HumanID].GetDrawFlag() == true ){	//操作対象が有効ならば　（注：裏技による変更対策）
		if( HumanIndex[Player_HumanID].GetDeadFlag() == true ){		//プレイヤーが死亡していれば
			return 2;
		}
	}

	return 0;
}

//! オブジェクトの主計算処理
//! @param cmdF5id 上昇機能（F5裏技）させる人データ番号（-1で機能無効）
//! @param camera_x カメラのX座標
//! @param camera_y カメラのY座標
//! @param camera_z カメラのZ座標
//! @param camera_rx カメラの横軸角度
//! @param camera_ry カメラの縦軸角度
//! @return 常に 0
//! @attention 一般的に cmdF5id は、F5裏技使用中はプレイヤー番号（GetPlayerID()関数で取得）、未使用時は -1 を指定します。
int ObjectManager::Process(int cmdF5id, float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry)
{
	//このフレームの戦歴を初期化
	for(int i=0; i<MAX_HUMAN; i++){
		Human_ontarget[i] = 0;
		Human_kill[i] = 0;
		Human_headshot[i] = 0;
	}

	//人オブジェクトの処理
	for(int i=0; i<MAX_HUMAN; i++){
		if( i == cmdF5id ){
			HumanIndex[i].RunFrame(CollD, BlockData, true);
		}
		else{
			HumanIndex[i].RunFrame(CollD, BlockData, false);
		}
	}

	//武器オブジェクトの処理
	for(int i=0; i<MAX_WEAPON; i++){
		WeaponIndex[i].RunFrame(CollD);
	}

	//小物オブジェクトの処理
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		SmallObjectIndex[i].RunFrame();
	}

	//弾オブジェクトの処理
	for(int i=0; i<MAX_BULLET; i++){
		float bx, by, bz, brx, bry;
		int speed;
		float mx, my, mz;

		CollideBullet(&BulletIndex[i]);		//当たり判定を実行
		BulletIndex[i].RunFrame();	//主計算

		if( BulletIndex[i].GetDrawFlag() == true ){
			//弾の座標と角度を取得
			BulletIndex[i].GetParamData(NULL, NULL, &speed, NULL, NULL);
			BulletIndex[i].GetPosData(&bx, &by, &bz, &brx, &bry);
			mx = cos(brx)*cos(bry)*speed;
			my = sin(bry)*speed;
			mz = sin(brx)*cos(bry)*speed;
			GameSound->PassingBullet(bx, by, bz, mx, my, mz);
		}
	}

	//武器を発砲した際のエフェクトを出す
	for(int i=0; i<MAX_HUMAN; i++){
		if( Human_ShotFlag[i] == true ){
			ShotWeaponEffect(i);
			Human_ShotFlag[i] = false;
		}
	}

	//エフェクトオブジェクトの処理
	for(int i=0; i<MAX_EFFECT; i++){
		EffectIndex[i].RunFrame(camera_rx, camera_ry);
	}

	//手榴弾の処理
	for(int i=0; i<MAX_GRENADE; i++){
		float speed = GrenadeIndex[i].GetSpeed();

		//主計算
		int rcr = GrenadeIndex[i].RunFrame(CollD, BlockData);

		//バウンド・跳ね返ったならば
		if( rcr == 1 ){
			if( speed > 3.4f ){
				//座標を取得し、効果音再生
				float x, y, z;
				GrenadeIndex[i].GetPosData(&x, &y, &z, NULL, NULL);
				GameSound->GrenadeBound(x, y, z);
			}
		}

		//爆発したなら
		if( rcr == 2 ){
			//座標を取得
			float gx, gy, gz;
			int humanid;
			GrenadeIndex[i].GetPosData(&gx, &gy, &gz, NULL, NULL);
			GrenadeIndex[i].GetParamData(NULL, NULL, NULL, NULL, &humanid);

			//エフェクト（フラッシュ）の表示
			for(int j=0; j<MAX_EFFECT; j++){
				if( EffectIndex[j].GetDrawFlag() == false ){
					EffectIndex[j].SetPosData(gx, gy, gz, 0.0f);
					EffectIndex[j].SetParamData(30.0f, 0.0f, 2, Resource->GetEffectMflashTexture(), EFFECT_NORMAL, true);
					EffectIndex[j].SetDrawFlag(true);
					break;
				}
			}

			//エフェクト（煙）の表示
			float rnd = (float)M_PI/18*GetRand(18);
			for(int j=0; j<MAX_EFFECT; j++){
				if( EffectIndex[j].GetDrawFlag() == false ){
					EffectIndex[j].SetPosData(gx+1.0f, gy+1.0f, gz+1.0f, 0.0f);
					EffectIndex[j].SetParamData(10.0f, rnd, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION, true);
					EffectIndex[j].SetDrawFlag(true);
					break;
				}
			}
			for(int j=0; j<MAX_EFFECT; j++){
				if( EffectIndex[j].GetDrawFlag() == false ){
					EffectIndex[j].SetPosData(gx-1.0f, gy-1.0f, gz-1.0f, 0.0f);
					EffectIndex[j].SetParamData(10.0f, rnd*-1, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION, true);
					EffectIndex[j].SetDrawFlag(true);
					break;
				}
			}
			for(int j=0; j<MAX_EFFECT; j++){
				if( EffectIndex[j].GetDrawFlag() == false ){
					EffectIndex[j].SetPosData(gx-1.0f, gy-1.0f, gz+1.0f, 0.0f);
					EffectIndex[j].SetParamData(10.0f, rnd, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION, true);
					EffectIndex[j].SetDrawFlag(true);
					break;
				}
			}
			for(int j=0; j<MAX_EFFECT; j++){
				if( EffectIndex[j].GetDrawFlag() == false ){
					EffectIndex[j].SetPosData(gx+1.0f, gy+1.0f, gz-1.0f, 0.0f);
					EffectIndex[j].SetParamData(10.0f, rnd*-1, (int)GAMEFPS * 3, Resource->GetEffectSmokeTexture(), EFFECT_DISAPPEAR | EFFECT_MAGNIFY | EFFECT_ROTATION, true);
					EffectIndex[j].SetDrawFlag(true);
					break;
				}
			}

			//効果音を再生
			GameSound->GrenadeExplosion(gx, gy, gz);

			//人に爆風の当たり判定
			for(int j=0; j<MAX_HUMAN; j++){
				if( HumanIndex[j].GrenadeExplosion(CollD, &(GrenadeIndex[i])) ){
					float hx, hy, hz;
					float x, y, y2, z;
					float arx, ary;

					//倒していれば、発射した人の成果に加算
					if( HumanIndex[j].GetHP() <= 0 ){
						Human_kill[humanid] += 1;
					}

					//エフェクト（血）を表示
					HumanIndex[j].GetPosData(&hx, &hy, &hz, NULL);
					SetHumanBlood(hx, hy+15.0f, hz);

					//人と手榴弾の距離を算出
					x = gx - hx;
					y = gy - (hy + 1.0f);
					z = gz - hz;

					//角度を求める
					arx = atan2(z, x);

					if( sin(atan2(y, sqrt(x*x + z*z))) < 0.0f ){		//上方向に飛ぶなら、角度を計算
						y2 = gy - (hy + HUMAN_HEIGTH);
						ary = atan2(y2, sqrt(x*x + z*z)) + (float)M_PI;
					}
					else{		//下方向に飛ぶなら、垂直角度は無効。（爆風で地面にめり込むのを防止）
						ary = 0.0f;
					}

					//爆風による風圧
					HumanIndex[j].AddPosOrder(arx, ary, 2.2f/MAX_DAMAGE_GRENADE_DISTANCE * (MAX_DAMAGE_GRENADE_DISTANCE - sqrt(x*x + y*y + z*z)));
				}
			}

			//小物に爆風の当たり判定
			for(int j=0; j<MAX_SMALLOBJECT; j++){
				//もし爆風に当たったら
				if( SmallObjectIndex[j].GrenadeExplosion(CollD, &(GrenadeIndex[i])) ){
					//小物から効果音を発する
					float sx, sy, sz;
					int id;
					SmallObjectIndex[j].GetPosData(&sx, &sy, &sz, NULL);
					SmallObjectIndex[j].GetParamData(&id, NULL);
					GameSound->HitSmallObject(sx, sy, sz, id);
				}
			}
		}
	}


	//武器を拾う処理
	for(int i=0; i<MAX_HUMAN; i++){
		//武器を拾うだけで毎フレーム総当たりで処理する意味はないので、各武器2フレームに1回処理にケチってます　（＾＾；
		//フレーム数が偶数目の時は、データ番号が偶数の武器。奇数の時は、奇数の武器を処理。
		for(int j=(framecnt%2); j<MAX_WEAPON; j+=2){
			PickupWeapon(&HumanIndex[i], &WeaponIndex[j]);
		}
	}

	for(int i=0; i<MAX_HUMAN; i++){
		for(int j=i+1; j<MAX_HUMAN; j++){
			//人同士の当たり判定（押し合い）
			CollideHuman(&HumanIndex[i], &HumanIndex[j]);
		}
	}

	framecnt += 1;

	return 0;
}

//! 現フレームの 命中率・倒した敵の数・ヘットショット数 の取得
//! @param id 取得する人のデータ番号
//! @param ontarget 命中数を受け取るポインタ
//! @param kill 倒した敵の数を受け取るポインタ
//! @param headshot 敵の頭部に命中した数を受け取るポインタ
//! @return 成功：true　失敗：false
bool ObjectManager::GetHumanShotInfo(int id, int *ontarget, int *kill, int *headshot)
{
	if( (id < 0)||(MAX_HUMAN-1 < id) ){ return false; }
	*ontarget = Human_ontarget[id];
	*kill = Human_kill[id];
	*headshot = Human_headshot[id];
	return true;
}

//!エフェクトをソートする
//! @param camera_x カメラのX座標
//! @param camera_y カメラのY座標
//! @param camera_z カメラのZ座標
//! @param data 結果を受け取る effectdata構造体　（要素数：MAX_EFFECT）
//! @return 有効なエフェクトの数
int ObjectManager::SortEffect(float camera_x, float camera_y, float camera_z, effectdata data[])
{
	int cnt = 0;

	//カメラからの距離を求めつつ、総数を数える。
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetDrawFlag() == true ){
			float ex, ey, ez;
			float x, y, z;

			//座標を取得し、距離を計算、データに登録。
			EffectIndex[i].GetPosData(&ex, &ey, &ez, NULL);
			x = ex - camera_x;
			y = ey - camera_y;
			z = ez - camera_z;
			data[cnt].id = i;
			data[cnt].dist = x*x + y*y + z*z;

			//カウントを加算
			cnt += 1;
		}
	}

	//単純挿入ソート
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

//! オブジェクトの描画処理
//! @param camera_x カメラのX座標
//! @param camera_y カメラのY座標
//! @param camera_z カメラのZ座標
//! @param HidePlayer プレイヤーの非表示設定　（表示：0　非表示：1　腕と武器のみ表示：2）
void ObjectManager::Render(float camera_x, float camera_y, float camera_z, int HidePlayer)
{
	//ワールド座標を原点へ
	d3dg->ResetWorldTransform();

	//人描画
	for(int i=0; i<MAX_HUMAN; i++){
		if( HidePlayer == 0 ){
			HumanIndex[i].Render(d3dg, Resource, false);
		}
		else if( Player_HumanID != i ){	// HidePlayer != 0
			HumanIndex[i].Render(d3dg, Resource, false);
		}
		else if( HidePlayer == 2 ){		// Player_HumanID == i
			HumanIndex[i].Render(d3dg, Resource, true);
		}
	}

	for(int i=0; i<MAX_HUMAN; i++){
		if( (HidePlayer == false)||(Player_HumanID != i) ){
			HumanIndex[i].Render(d3dg, Resource, true);
		}
	}

	//武器描画
	for(int i=0; i<MAX_WEAPON; i++){
		WeaponIndex[i].Render(d3dg);
	}

	//小物描画
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		SmallObjectIndex[i].Render(d3dg);
	}

	//弾描画
	for(int i=0; i<MAX_BULLET; i++){
		BulletIndex[i].Render(d3dg);
	}

	//手榴弾描画
	for(int i=0; i<MAX_GRENADE; i++){
		GrenadeIndex[i].Render(d3dg);
	}

	//エフェクト描画
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

//! データの解放
void ObjectManager::Cleanup()
{
	//ポイントデータ解放
	CleanupPointDataToObject();
}