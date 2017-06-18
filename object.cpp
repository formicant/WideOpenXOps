//! @file object.cpp
//! @brief objectクラスの定義

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

//! @brief コンストラクタ
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

//! @brief ディストラクタ
object::~object()
{}

//! @brief 設定値を管理するクラスを登録
//! @attention 各関数を使用する前に実行すること。
void object::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;
}

//! @brief 座標と角度を設定
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param rx 横軸回転
void object::SetPosData(float x, float y, float z, float rx)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;
}

//! @brief 座標と角度を取得
//! @param x X座標を受け取るポインタ（NULL可）
//! @param y Y座標を受け取るポインタ（NULL可）
//! @param z Z座標を受け取るポインタ（NULL可）
//! @param rx 横軸回転を受け取るポインタ（NULL可）
void object::GetPosData(float *x, float *y, float *z, float *rx)
{
	if( x != NULL ){ *x = pos_x; }
	if( y != NULL ){ *y = pos_y; }
	if( z != NULL ){ *z = pos_z; }
	if( rx != NULL ){ *rx = rotation_x; }
}

//! @brief 有効化フラグを設定
//! @param flag 設定するフラグ
void object::SetEnableFlag(bool flag)
{
	EnableFlag = flag;
}

//! @brief 有効化フラグを取得
//! @return 現在設定されているフラグ
bool object::GetEnableFlag()
{
	return EnableFlag;
}

//! @brief モデルデータを設定
//! @param id モデル認識番号
//! @param size 描画倍率
void object::SetModel(int id, float size)
{
	id_model = id;
	model_size = size;
}

//! @brief モデルデータを取得
//! @param id モデル認識番号を受け取るポインタ（NULL可）
//! @param size 描画倍率を受け取るポインタ（NULL可）
void object::GetModel(int *id, float *size)
{
	if( id != NULL ){ *id = id_model; }
	if( size != NULL ){ *size = model_size; }
}

//! @brief テクスチャを設定
//! @param id テクスチャ認識番号
void object::SetTexture(int id)
{
	id_texture = id;
}

//! @brief テクスチャを取得
//! @return テクスチャ認識番号
int object::GetTexture()
{
	return id_texture;
}

//! @brief モデルを暗くするフラグを設定
//! @param flag フラグ
void object::SetDarkModelFlag(bool flag)
{
	DarkModelFlag = flag;
}

//! @brief 計算を実行（自由落下など）
int object::RunFrame()
{
	return 0;
}

//! @brief 描画
//! @param d3dg D3DGraphicsのポインタ
void object::Render(D3DGraphics *d3dg)
{
	if( d3dg == NULL ){ return; }
	if( EnableFlag == false ){ return; }

	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, rotation_x, 0.0f, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag);
}

//! @brief コンストラクタ
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

//! @brief ディストラクタ
human::~human()
{
	delete MotionCtrl;
}

//! @brief 設定値を管理するクラスを登録
//! @attention 各関数を使用する前に実行すること。
void human::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;

	MotionCtrl->SetParameterInfoClass(in_Param);
}

//! @brief 設定値を設定
//! @param id_param 人の種類番号
//! @param dataid ポイントのデータ番号
//! @param p4 第4パラメータ
//! @param team チーム番号
//! @param init オブジェクトを初期化
//! @warning init引数をtrueにして初期化する場合、先にSetPosData()関数で角度を設定しておくこと。
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

//! @brief 設定値を取得
//! @param id_param 人の種類番号を受け取るポインタ（NULL可）
//! @param dataid ポイントのデータ番号を受け取るポインタ（NULL可）
//! @param p4 第4パラメータを受け取るポインタ（NULL可）
//! @param team チーム番号を受け取るポインタ（NULL可）
void human::GetParamData(int *id_param, int *dataid, signed char *p4, int *team)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( dataid != NULL ){ *dataid = point_dataid; }
	if( p4 != NULL ){ *p4 = point_p4; }
	if( team != NULL ){ *team = teamid; }
}

//! @brief HPを取得
//! @return HP
int human::GetHP()
{
	return hp;
}

//! @brief HPを設定
//! @param in_hp 新たに設定するHP
//! @return 成功：true　失敗：false
//! @attention HPが1以上の人に対して実行しないと失敗します。
bool human::SetHP(int in_hp)
{
	if( hp > 0 ){
		hp = in_hp;
		return true;
	}
	return false;
}


//! @brief 死体かどうか判定
//! @return 死体：true　死体でない：false
//! @warning 完全に静止した状態を「死体」と判定します。倒れている最中の人は対象に含まれないため、hp <= 0 が全て死体と判定されるとは限りません。
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

//! @brief チーム番号を設定（上書き）
//! @param id 新しいチーム番号
void human::SetTeamID(int id)
{
	teamid = id;
}

//! @brief 無敵フラグを取得
//! @return true：無敵　false：通常
//! @attention 無敵状態の場合、銃弾・手榴弾の爆発・落下　によるダメージを一切受けません。
bool human::GetInvincibleFlag()
{
	return Invincible;
}

//! @brief 無敵フラグを設定
//! @param flag true：無敵　false：通常
//! @attention 無敵状態の場合、銃弾・手榴弾の爆発・落下　によるダメージを一切受けません。
void human::SetInvincibleFlag(bool flag)
{
	Invincible = flag;
}

//! @brief 前処理の移動量を取得
//! @param *x X軸移動量を取得するポインタ（NULL可）
//! @param *y Y軸移動量を取得するポインタ（NULL可）
//! @param *z Z軸移動量を取得するポインタ（NULL可）
void human::GetMovePos(float *x, float *y, float *z)
{
	if( x != NULL ){ *x = move_x; }
	if( y != NULL ){ *y = move_y; }
	if( z != NULL ){ *z = move_z; }
}

//! @brief モデルデータを設定
//! @param upmodel 上半身のモデル
//! @param armmodel[] 腕のモデルの配列（配列数：TOTAL_ARMMODE）
//! @param legmodel 足（静止状態）のモデル
//! @param walkmodel[] 腕のモデルの配列（配列数：TOTAL_WALKMODE）
//! @param runmodel[] 腕のモデルの配列（配列数：TOTAL_RUNMODE）
void human::SetModel(int upmodel, int armmodel[], int legmodel, int walkmodel[], int runmodel[])
{
	MotionCtrl->SetModel(upmodel, armmodel, legmodel, walkmodel, runmodel);
}

//! @brief 武器を設定
//! @param in_weapon[] 設定するweaponクラスのポインタ配列
//! @warning 通常は PickupWeapon()関数 を使用すること
void human::SetWeapon(class weapon *in_weapon[])
{
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		if( in_weapon[i] == NULL ){
			weapon[i] = NULL;
		}
		else{
			//武器を正しく拾えれば、所持武器として登録
			if( in_weapon[i]->Pickup() == 0 ){
				weapon[i] = in_weapon[i];
			}
		}
	}
}

//! @brief 武器を拾う
//! @param in_weapon[] 設定するweaponクラスのポインタ
//! @return 成功：1　失敗：0
//! @attention 人の種類が ゾンビ の場合、この関数は失敗します。
int human::PickupWeapon(class weapon *in_weapon)
{
	if( in_weapon == NULL ){
		return 0;
	}

	//人の種類が ゾンビ ならば、失敗
	HumanParameter Paraminfo;
	Param->GetHuman(id_parameter, &Paraminfo);
	if( Paraminfo.type == 2 ){
		return 0;
	}

	if( weapon[selectweapon] == NULL ){
		//武器を正しく拾えれば、所持武器として登録
		if( in_weapon->Pickup() == 0 ){
			weapon[selectweapon] = in_weapon;

			//腕の角度（反動）を設定
			class weapon *nowweapon;
			int id_param = 0;
			nowweapon = weapon[selectweapon];
			nowweapon->GetParamData(&id_param, NULL, NULL);
			MotionCtrl->PickupWeapon(id_param);

			//切り替え完了のカウント
			selectweaponcnt = 10;

			return 1;
		}
	}
	return 0;
}

//! @brief 武器を切り替える（持ち替える）
//! @param id 持ち替える武器　（-1 で次の武器）
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
void human::ChangeWeapon(int id)
{
	//体力がなければ失敗
	if( hp <= 0 ){ return; }

	//リロード中なら失敗
	if( weapon[selectweapon] != NULL ){
		if( weapon[selectweapon]->GetReloadCnt() > 0 ){ return; }
	}

	//同じ武器に切り替えようとしているなら、失敗
	if( selectweapon == id ){ return; }

	//武器切り替え中なら失敗
	if( selectweaponcnt > 0 ){ return; }

	if( id == -1 ){
		//次の武器番号を選択
		selectweapon += 1;
		if( selectweapon == TOTAL_HAVEWEAPON ){
			selectweapon = 0;
		}
	}
	else{
		//武器番号が範囲内か確認
		if( (id < 0)||((TOTAL_HAVEWEAPON -1) < id ) ){ return; }
		selectweapon = id;
	}

	//スコープ表示を解除
	SetDisableScope();

	//現在装備する武器のクラスを取得
	class weapon *nowweapon;
	nowweapon = weapon[selectweapon];

	//腕の角度（反動）を設定
	if( nowweapon == NULL ){	//手ぶら
		MotionCtrl->ChangeWeapon(ID_WEAPON_NONE);
	}
	else{
		int id_param = 0;
		nowweapon->GetParamData(&id_param, NULL, NULL);
		MotionCtrl->ChangeWeapon(id_param);
	}

	//切り替え完了のカウント
	selectweaponcnt = 10;
}

//! @brief 武器の切り替えカウントを取得
//! @return カウント数　（1以上で切り替え中）
int human::GetChangeWeaponCnt()
{
	return selectweaponcnt;
}

//! @brief 武器を取得
//! @param out_selectweapon 選択されている武器　（0 ～ [TOTAL_HAVEWEAPON]-1）
//! @param out_weapon 受け取るweaponクラスのポインタ配列　（配列数：TOTAL_HAVEWEAPON）
void human::GetWeapon(int *out_selectweapon, class weapon *out_weapon[])
{
	*out_selectweapon = selectweapon;
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		out_weapon[i] = weapon[i];
	}
}

//! @brief 現在装備している武器の種類番号を取得
//! @return 武器の種類番号（0 ～ TOTAL_PARAMETERINFO_WEAPON -1）
//! @attention 現在手に持っている武器の種類番号です。　GetWeapon()関数 を用いて調べるのと同等です。
int human::GetMainWeaponTypeNO()
{
	if( weapon[selectweapon] == NULL ){
		return ID_WEAPON_NONE;
	}

	int id_param;
	weapon[selectweapon]->GetParamData(&id_param, NULL, NULL);
	return id_param;
}

//! @brief 連射設定を取得
//! @return 連射可能：true　連射不可：false
bool human::GetWeaponBlazingmode()
{
	int param_id;
	WeaponParameter data;

	//武器を装備していなければ、失敗
	if( weapon[selectweapon] == NULL ){ return false; }

	//装備している武器の種類番号を取得
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	//連射設定を返す
	Param->GetWeapon(param_id, &data);
	return data.blazingmode;
}

//! @brief 発砲処理
//! @param weapon_paramid 発砲した武器の番号を受け取るポインタ
//! @param GunsightErrorRange 発砲した際の照準誤差を受け取るポインタ
//! @return 成功：1　失敗：0
//! @attention 弾オブジェクトの処理や、発砲音の再生は別途行う必要があります。
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
bool human::ShotWeapon(int *weapon_paramid, int *GunsightErrorRange)
{
	int param_id;

	//武器切り替え中なら失敗
	if( selectweaponcnt > 0 ){ return false; }

	//武器を装備していなければ、失敗
	if( weapon[selectweapon] == NULL ){ return false; }

	//弾の発射処理を行う
	if( weapon[selectweapon]->Shot() != 0 ){ return false; }

	//武器の種類番号を取得
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);


	//武器の設定値を取得
	WeaponParameter ParamData;
	if( Param->GetWeapon(param_id, &ParamData) != 0 ){ return false; }

	//精密スコープの武器でスコープを覗いていなければ、誤差 20。
	if( (scopemode == 0)&&(ParamData.scopemode == 2) ){
		ReactionGunsightErrorRange = 20;
	}


	//武器の種類番号と誤差を返す
	*weapon_paramid = param_id;
	*GunsightErrorRange = StateGunsightErrorRange + ReactionGunsightErrorRange;


	//精密スコープ以外の武器を、スコープを使わずに使っていたら、設定された誤差を加算。
	if( (ParamData.scopemode != 2)&&(scopemode == 0) ){
		ReactionGunsightErrorRange += ParamData.reaction;
	}

	//スコープを使用している状態の反動を加算
	if( (ParamData.scopemode == 1)&&(scopemode != 0) ){
		armrotation_y += DegreeToRadian(1) * (WEAPONERRORRANGE_SCALE * ParamData.reaction);
	}
	if( ParamData.scopemode == 2 ){
		armrotation_y += DegreeToRadian(1) * (WEAPONERRORRANGE_SCALE * ParamData.reaction);
	}

	//腕に反動を伝える
	MotionCtrl->ShotWeapon(param_id);

	//武器が無くなっていれば、装備から外した扱いに。　（手榴弾用）
	if( weapon[selectweapon]->GetEnableFlag() == false ){
		weapon[selectweapon] = NULL;
	}
	return true;
}

//! @brief リロード
//! @return 成功：true　失敗：false
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
bool human::ReloadWeapon()
{
	//武器切り替え中なら失敗
	if( selectweaponcnt > 0 ){ return false; }

	//何かしらの武器を装備していれば～
	if( weapon[selectweapon] != NULL ){
		//リロード中なら失敗
		if( weapon[selectweapon]->GetReloadCnt() > 0 ){ return false; }

		//リロード処理を開始
		if( weapon[selectweapon]->StartReload() != 0 ){ return false; }

		//スコープモードを解除
		SetDisableScope();

		//モーション実行
		class weapon *nowweapon;
		int id_param = 0;
		nowweapon = weapon[selectweapon];
		nowweapon->GetParamData(&id_param, NULL, NULL);
		MotionCtrl->ReloadWeapon(id_param);

		return true;
	}
	return false;
}

//! @brief 武器を捨てる
//! @return 成功：true　失敗：false
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
bool human::DumpWeapon()
{
	//武器切り替え中なら失敗
	if( selectweaponcnt > 0 ){ return false; }

	//何かしらの武器を装備していれば～
	if( weapon[selectweapon] != NULL ){
		//リロード中なら失敗
		if( weapon[selectweapon]->GetReloadCnt() > 0 ){ return false; }

		//武器を捨て、装備を解除
		weapon[selectweapon]->Dropoff(pos_x, pos_y, pos_z, rotation_x, 1.63f);
		weapon[selectweapon] = NULL;

		//スコープモードを解除
		SetDisableScope();

		//モーション実行
		MotionCtrl->DumpWeapon();

		return true;
	}

	return false;
}

//! @brief 武器のショットモード切り替え
//! @return 成功：0　失敗：1
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
int human::ChangeShotMode()
{
	//武器を装備してなければ失敗
	if( weapon[selectweapon] == NULL ){ return 1; }

	//装備している武器の情報を取得
	int param_id, lnbs, nbs;
	weapon[selectweapon]->GetParamData(&param_id, &lnbs, &nbs);

	//武器のショットモード切り替え先（新しい武器番号）を調べる
	WeaponParameter ParamData;
	int ChangeWeapon;
	if( Param->GetWeapon(param_id, &ParamData) != 0 ){ return 1; }
	ChangeWeapon = ParamData.ChangeWeapon;

	//新しい武器番号が正しいか確認
	if( ChangeWeapon == param_id ){ return 1; }
	if( (ChangeWeapon < 0)||(TOTAL_PARAMETERINFO_WEAPON-1 < ChangeWeapon) ){ return 1; }

	//設定を適用
	weapon[selectweapon]->SetParamData(ChangeWeapon, lnbs, nbs, false);

	//モーション実行
	MotionCtrl->ChangeShotMode(ChangeWeapon);

	return 0;
}

//! @brief 前進（走り）を設定
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
void human::SetMoveForward()
{
	SetFlag(MoveFlag, MOVEFLAG_FORWARD);
}

//! @brief 後退を設定
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
void human::SetMoveBack()
{
	SetFlag(MoveFlag, MOVEFLAG_BACK);
}

//! @brief 左走りを設定
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
void human::SetMoveLeft()
{
	SetFlag(MoveFlag, MOVEFLAG_LEFT);
}

//! @brief 右走りを設定
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
void human::SetMoveRight()
{
	SetFlag(MoveFlag, MOVEFLAG_RIGHT);
}

//! @brief 歩きを設定
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
void human::SetMoveWalk()
{
	SetFlag(MoveFlag, MOVEFLAG_WALK);
}

//! @brief 人の移動モードを取得
//! @param nowdata 現在の値を取得：true　前フレームの値を使用：false
//! @return 歩き：1　走り：2　移動してない：0
int human::GetMovemode(bool nowdata)
{
	//使用されていないか、処理されていなければ終了
	if( EnableFlag == false ){ return 0; }
	if( hp <= 0 ){ return 0; }

	if( nowdata == false ){	//前のデータ
		//歩きならば 1
		if( GetFlag(MoveFlag_lt, MOVEFLAG_WALK) ){
			return 1;
		}
		//走りならば 2
		if( GetFlag(MoveFlag_lt, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
			return 2;
		}
	}
	else{					//現在のデータ
		//歩きならば 1
		if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
			return 1;
		}
		//走りならば 2
		if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
			return 2;
		}
	}

	//移動してない
	return 0;
}

//! @brief スコープを設定
//! @return 成功：true　失敗：false
bool human::SetEnableScope()
{
	int param_id;
	WeaponParameter data;

	//何も武器を装備してなければ失敗
	if( weapon[selectweapon] == NULL ){ return false; }

	//武器の種類番号を取得
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	//武器の設定値を取得
	Param->GetWeapon(param_id, &data);

	//スコープを設定
	scopemode = data.scopemode;
	return true;
}

//! @brief スコープを解除
void human::SetDisableScope()
{
	scopemode = 0;
}

//! @brief スコープ設定を取得
int human::GetScopeMode()
{
	return scopemode;
}

//! @brief 横軸と縦軸の向きを取得
//! @param rx 横軸を取得するポインタ
//! @param ry 縦軸を取得するポインタ
void human::GetRxRy(float *rx, float *ry)
{
	*rx = rotation_x;
	*ry = armrotation_y;
}

//! @brief 横軸と縦軸の向きを設定
//! @param rx 設定する横軸
//! @param ry 設定する縦軸
void human::SetRxRy(float rx, float ry)
{
	rotation_x = rx;
	armrotation_y = ry;
}

//! @brief 全体の回転角度取得
//! @return 縦軸を取得するポインタ
//! @warning 死亡して倒れる際の角度です。GetRxRy()関数で受け取る値とは異なります。
float human::GetDeadRy()
{
	return rotation_y;
}

//! @brief ジャンプ
//! @return 成功：0　失敗：1
//! @attention ゲーム上から直接呼び出すことは避け、ObjectManagerクラスから呼び出してください。
int human::Jump()
{
	//地面に触れており、落下速度が0.0ならば
	if( move_y_flag == false ){
		if( move_y == 0.0f ){
			move_y = HUMAN_JUMP_SPEED;

			//モーション実行
			MotionCtrl->Jump();
			return 0;
		}
	}
	return 1;
}

//! @brief 押しだす・力を加える
//! @param rx 横軸
//! @param ry 縦軸
//! @param speed 速度
void human::AddPosOrder(float rx, float ry, float speed)
{
	move_x += cos(rx) * cos(ry) * speed;
	move_y += sin(ry) * speed;
	move_z += sin(rx) * cos(ry) * speed;
}

//! @brief 弾が 頭 にヒット
//! @param attacks 弾の攻撃力
void human::HitBulletHead(int attacks)
{
	if( Invincible == false ){
		hp -= (int)((float)attacks * HUMAN_DAMAGE_HEAD) + GetRand(16);
	}
	ReactionGunsightErrorRange = 15;
}

//! @brief 弾が 上半身 にヒット
//! @param attacks 弾の攻撃力
void human::HitBulletUp(int attacks)
{
	if( Invincible == false ){
		hp -= (int)((float)attacks * HUMAN_DAMAGE_UP) + GetRand(8);
	}
	ReactionGunsightErrorRange = 12;
}

//! @brief 弾が 下半身 にヒット
//! @param attacks 弾の攻撃力
void human::HitBulletLeg(int attacks)
{
	if( Invincible == false ){
		hp -= (int)((float)attacks * HUMAN_DAMAGE_LEG) + GetRand(6);
	}
	ReactionGunsightErrorRange = 8;
}

//! @brief ゾンビの攻撃がヒット
void human::HitZombieAttack()
{
	if( Invincible == false ){
		hp -= HUMAN_DAMAGE_ZOMBIEU + GetRand(HUMAN_DAMAGE_ZOMBIEA);
	}
	ReactionGunsightErrorRange = 10;
}

//! @brief 手榴弾の爆風がヒット
//! @param attacks 爆風の攻撃力
//! @attention 距離による計算を事前に済ませてください。
void human::HitGrenadeExplosion(int attacks)
{
	if( Invincible == false ){
		hp -= attacks;
	}
	ReactionGunsightErrorRange = 10;
}

//! @brief 被弾フラグをセット
//! @param rx 被弾した方向
void human::SetHitFlag(float rx)
{
	HitFlag = true;
	Hit_rx = rx;
}

//! @brief 被弾したかチェックする
//! @param rx 被弾した方向を受け取るポインタ（NULL可）
//! @return 被弾した：true　被弾してない：false
//! @attention 実行すると、フラグは false に初期化されます。
bool human::CheckHit(float *rx)
{
	bool returnflag = HitFlag;
	if( rx != NULL ){ *rx = Hit_rx; }
	HitFlag = false;
	//Hit_rx = 0.0f;
	return returnflag;
}

//! @brief 合計移動量を取得
//! @return 合計移動量
float human::GetTotalMove()
{
	return totalmove;
}

//! @brief 照準の状態誤差の処理
//!	@attention ControlProcess()より前で処理すること
void human::GunsightErrorRange()
{
	//初期化
	StateGunsightErrorRange = 0;

	//各操作による誤差を設定
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

	//何か武器を装備していれば
	if( weapon[selectweapon] != NULL ){
		//武器の設定データを取得
		int param;
		WeaponParameter data;
		weapon[selectweapon]->GetParamData(&param, NULL, NULL);
		Param->GetWeapon(param, &data);

		//誤差を 1 減らす
		ReactionGunsightErrorRange -= 1;

		//誤差の範囲を補正
		if( ReactionGunsightErrorRange < 0 ){ ReactionGunsightErrorRange = 0; }
		if( ReactionGunsightErrorRange > data.ErrorRangeMAX ){ ReactionGunsightErrorRange = data.ErrorRangeMAX; }
	}
	else{
		ReactionGunsightErrorRange = 0;
	}
}

//! @brief 死亡判定と倒れる処理
//! @return 静止した死体：4　倒れ終わった直後：3　倒れている最中：2　倒れ始める：1　何もしない：0
int human::CheckAndProcessDead(class Collision *CollD)
{
#ifdef HUMAN_DEADBODY_COLLISION

	//メモ：
	//
	//状態：0
	//［何もしない］
	//　HPが0以下で、頭が付かぬなら　状態：1
	//　HPが0以下で、壁に頭が付くなら　状態：4
	//
	//状態：1
	//［傾き始める］
	//　135度行ったら　状態：2
	//　壁に頭を打ったら　状態：3
	//
	//状態：2
	//［落下］
	//　足が地面に付いたら　状態：4
	//
	//状態：3
	//［足を滑らせる］
	//　　
	//状態：4
	//［固定］

	float check_posx, check_posy, check_posz;

	if( deadstate == 0 ){
		if( hp <= 0 ){		//HPが 0 以下になった（死亡した）瞬間なら、倒し始める
			float tr;

			//最後に攻撃を受けた方向を計算
			tr = Hit_rx - rotation_x;
			for(; tr > (float)M_PI; tr -= (float)M_PI*2){}
			for(; tr < (float)M_PI*-1; tr += (float)M_PI*2){}

			//体の角度
			if( ((float)M_PI/2*-1 < tr)&&(tr < (float)M_PI/2) ){
				add_ry = HUMAN_DEADADDRY;
			}
			else{
				add_ry = HUMAN_DEADADDRY * -1;
			}

			//腕の角度
			if( armrotation_y < 0.0f ){
				armrotation_y = DegreeToRadian(-90);
			}
			else{
				armrotation_y = DegreeToRadian(90);
			}

			//死体が埋まらぬよう、高さを +1.0 する
			pos_y += 1.0f;

			//所持している武器を全て捨てる
			for(int i=0; i<TOTAL_HAVEWEAPON; i++){
				if( weapon[i] != NULL ){
					weapon[i]->Dropoff(pos_x, pos_y, pos_z, DegreeToRadian(10)*GetRand(36), 1.5f);
					weapon[i] = NULL;
				}
			}

			//スコープモードを解除
			SetDisableScope();

			//次のフレームの頭の座標を取得
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
		//135度以上倒れていれば
		if( fabs(rotation_y) >= DegreeToRadian(135) ){
			deadstate = 2;
			return 2;
		}

		if( pos_y <= (HUMAN_DEADLINE + 10.0f) ){
			//90度以上倒れていれば
			if( fabs(rotation_y) >= DegreeToRadian(90) ){
				deadstate = 4;
				return 2;
			}
		}

		//前後に倒す
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
			//次のフレームの頭の座標を取得
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

		//次のフレームの足の座標
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

		//90度以上倒れていれば
		if( fabs(rotation_y) >= DegreeToRadian(90) ){
			deadstate = 4;
			return 2;
		}

		//前後に倒す
		if( rotation_y > 0.0f ){		//rotation_y < DegreeToRadian(90)
			add_ry += HUMAN_DEADADDRY;
		}
		else if( rotation_y < 0.0f ){	//rotation_y > DegreeToRadian(-90)
			add_ry -= HUMAN_DEADADDRY;
		}

		//次のフレームの足の座標を取得
		check_posx = pos_x - cos(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posy = pos_y + 0.1f;
		check_posz = pos_z - sin(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;

		if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
			deadstate = 4;
			return 2;
		}

		//次のフレームの頭の座標を取得
		check_posx = pos_x - cos(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posy = pos_y + cos(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posz = pos_z - sin(rotation_x*-1 - (float)M_PI/2) * sin(rotation_y + add_ry) * HUMAN_HEIGHT;

		if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
			deadstate = 4;
			return 2;
		}

		//足の座標を移動
		pos_x -= cos(rotation_x*-1 - (float)M_PI/2) * sin(add_ry) * HUMAN_HEIGHT;
		pos_z -= sin(rotation_x*-1 - (float)M_PI/2) * sin(add_ry) * HUMAN_HEIGHT;

		rotation_y += add_ry;
		return 2;
	}

	if( deadstate == 4 ){
		//何もしない（固定）
		deadstate = 5;
		return 3;
	}

	if( deadstate == 5 ){
		//何もしない（固定）
		return 4;
	}

	return 0;
#else
	if( fabs(rotation_y) >= DegreeToRadian(90) ){
		return 4;
	}
	else if( rotation_y > 0.0f ){		//倒れ始めていれば、そのまま倒れる。
		add_ry += HUMAN_DEADADDRY;
		rotation_y += add_ry;
		if( rotation_y >= DegreeToRadian(90) ){
			rotation_y = DegreeToRadian(90);
			return 3;
		}
		return 2;
	}
	else if( rotation_y < 0.0f ){	//倒れ始めていれば、そのまま倒れる。
		add_ry -= HUMAN_DEADADDRY;
		rotation_y += add_ry;
		if( rotation_y <= DegreeToRadian(-90) ){
			rotation_y = DegreeToRadian(-90);
			return 3;
		}
		return 2;
	}
	else if( hp <= 0 ){		//HPが 0 以下になった（死亡した）瞬間なら、倒し始める
		//体の角度・腕の角度
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

		//死体が埋まらぬよう、高さを +1.0 する
		pos_y += 1.0f;

		//所持している武器を全て捨てる
		for(int i=0; i<TOTAL_HAVEWEAPON; i++){
			if( weapon[i] != NULL ){
				weapon[i]->Dropoff(pos_x, pos_y, pos_z, DegreeToRadian(10)*GetRand(36), 1.5f);
				weapon[i] = NULL;
			}
		}

		//スコープモードを解除
		SetDisableScope();

		return 1;
	}

	return 0;
#endif
}

//! @brief 操作による移動計算
//! @attention 実行すると、各キーフラグは false に初期化されます。
void human::ControlProcess()
{
	//進行方向と速度を決定
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

	//フラグをバックアップ
	MoveFlag_lt = MoveFlag;

	//キーフラグを元に戻す
	MoveFlag = 0x00;
}

//! @brief マップとの当たり判定
//! @param CollD Collisionクラスのポインタ
//! @param inblockdata BlockDataInterfaceクラスのポインタ
//! @param AddCollisionFlag 追加の当たり判定フラグ
//! @param FallDist Y軸の移動量を取得するポインタ
//! @param nowmove_x X軸の移動量を取得するポインタ
//! @param nowmove_z Z軸の移動量を取得するポインタ
//! @return ブロックに埋まっている：true　埋まっていない：false
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

	//足元ギリギリは当たり判定から除外する
	offset = 0.1f;

	//　上下方向の当たり判定（ジャンプ・自然落下）
	//--------------------------------------------------

	//足元がブロックに埋まっていなければ
	if( CollD->CheckALLBlockInside(pos_x, pos_y + offset, pos_z) == false ){
		//落下速度を計算
		move_y -= HUMAN_DAMAGE_SPEED;
		if( move_y < HUMAN_DAMAGE_MAXSPEED ){ move_y = HUMAN_DAMAGE_MAXSPEED; }

		if( move_y > 0.0f ){
			//上方向へ当たり判定
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + HUMAN_HEIGHT, pos_z, 0, 1, 0, NULL, NULL, &Dist, move_y) == true ){
				CollD->CheckALLBlockIntersectRay(pos_x, pos_y + HUMAN_HEIGHT, pos_z, 0, 1, 0, NULL, NULL, &Dist, move_y);

				FallDistance = Dist;
				move_y = 0.0f;
			}
			else{
				FallDistance = move_y;
			}
			move_y_flag = true;

			//DummyRay() 関数の結果に関わらず FallDistance = move_y; でも良いのだろうか？
		}
		else{
			 int id, face;
			 struct blockdata bdata;

			//下方向へ当たり判定
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + offset, pos_z, 0, -1, 0, NULL, NULL, &Dist, move_y*-1 + offset) == true ){
				if( CollD->CheckALLBlockIntersectRay(pos_x, pos_y + offset, pos_z, 0, -1, 0, &id, &face, &Dist, move_y*-1 + offset) == false ){
					//もし、-DummyRay()関数ではブロックが見つかるのに -Ray()関数で見つからないなら、おそらくブロックが歪んでいる。
					//とりあず適当な値で誤魔化し、微振動を防ぐ。
					Dist = offset;
				}

				if( Invincible == false ){
					//ダメージ計算
					if( move_y > HUMAN_DAMAGE_MINSPEED ){ hp -= 0; }
					else{ hp -= (int)((float)HUMAN_DAMAGE_MAXFALL / fabs(HUMAN_DAMAGE_MAXSPEED - (HUMAN_DAMAGE_MINSPEED)) * fabs(move_y - (HUMAN_DAMAGE_MINSPEED))); }
				}

				FallDistance = (Dist - offset) * -1;
				move_y = 0.0f;
				
				inblockdata->Getdata(&bdata, id);

				//斜面に立っているなら
				if( acos(bdata.material[face].vy) > HUMAN_MAPCOLLISION_SLOPEANGLE ){
					//地面と認めない　（ジャンプ対策）
					move_y_flag = true;

					float angvx = atan2(bdata.material[face].vz, bdata.material[face].vx);
					float angvy = acos(bdata.material[face].vy);

					//押し出す力の決定
					//y = -0.6*x*x + 1.9*x - 0.2
					float force = -0.6f*angvy*angvy + 1.9f*angvy - 0.2f;

					//押し出す
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
	else{	//埋まっている
		FallDistance = move_y;
		move_y = 0.0f;
		move_y_flag = false;
	}


	//　水平方向の当たり判定（移動）
	//--------------------------------------------------

	if( (move_x2*move_x2 + move_z2*move_z2) ){
		int surface;
		float ang = atan2(move_z2, move_x2);
		float newpos_x, newpos_y, newpos_z;

		//腰付近を当たり判定
		for(int i=0; i<MAX_BLOCKS; i++){
			float min_x, min_y, min_z, max_x, max_y, max_z;
			CollD->GetBlockPosMINMAX(i, &min_x, &min_y, &min_z, &max_x, &max_y, &max_z);

			//腰付近をAABBで荒削り
			if( CollideBoxAABB(min_x, min_y, min_z, max_x, max_y, max_z,
				pos_x - HUMAN_MAPCOLLISION_R - 1.0f, pos_y + HUMAN_MAPCOLLISION_HEIGHT - 1.0f, pos_z - HUMAN_MAPCOLLISION_R - 1.0f, pos_x + HUMAN_MAPCOLLISION_R + 1.0f, pos_y + HUMAN_MAPCOLLISION_HEIGHT + 1.0f, pos_z + HUMAN_MAPCOLLISION_R + 1.0f) == true
			){
				surface = -1;
				CollD->CheckBlockInside(i, pos_x, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z, false, &surface);

				if( surface != -1 ){
					//HUMAN_MAPCOLLISION_R 分の先を調べる
					if( CollD->CheckBlockInside(i, pos_x + cos(ang)*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z + sin(ang)*HUMAN_MAPCOLLISION_R, true, NULL) == true ){
						CollD->ScratchVector(i, surface, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
					}

					//左右90度づつを調べる
					if( CollD->CheckBlockInside(i, pos_x + cos(ang + (float)M_PI/2)*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z + sin(ang + (float)M_PI/2)*HUMAN_MAPCOLLISION_R, true, NULL) == true ){
						if( CollD->CheckPolygonFrontRx(i, surface, ang) == true ){		//進行方向に対して表向きなら～
							CollD->ScratchVector(i, surface, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
						}
					}
					if( CollD->CheckBlockInside(i, pos_x + cos(ang - (float)M_PI/2)*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z + sin(ang - (float)M_PI/2)*HUMAN_MAPCOLLISION_R, true, NULL) == true ){
						if( CollD->CheckPolygonFrontRx(i, surface, ang) == true ){		//進行方向に対して表向きなら～
							CollD->ScratchVector(i, surface, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
						}
					}
				}
			}
		}

		//進行方向を示すベクトルを算出
		vx = move_x2;
		vz = move_z2;
		speed = sqrt(vx*vx + vz*vz);
		if( speed > 0.0f ){
			vx = vx / speed;
			vz = vz / speed;
		}

		//頭を当たり判定
		if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + HUMAN_HEIGHT, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == true ){
			CollD->CheckALLBlockIntersectRay(pos_x, pos_y + FallDistance + HUMAN_HEIGHT, pos_z, vx, 0, vz, &id, &face, &Dist, speed);
			CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
		}

		if( AddCollisionFlag == true ){
			//腰付近の追加当たり判定
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + HUMAN_MAPCOLLISION_ADD_HEIGHT_A, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == true ){
				CollD->CheckALLBlockIntersectRay(pos_x, pos_y + FallDistance + HUMAN_MAPCOLLISION_ADD_HEIGHT_A, pos_z, vx, 0, vz, &id, &face, &Dist, speed);
				CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
			}

			//腰付近の追加当たり判定
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + HUMAN_MAPCOLLISION_ADD_HEIGHT_B, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == true ){
				CollD->CheckALLBlockIntersectRay(pos_x, pos_y + FallDistance + HUMAN_MAPCOLLISION_ADD_HEIGHT_B, pos_z, vx, 0, vz, &id, &face, &Dist, speed);
				CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
			}
		}

		//足元がブロックに埋まっていなければ
		if( CollD->CheckALLBlockInside(pos_x, pos_y + offset, pos_z) == false ){

			//進行方向を示すベクトルを算出
			vx = move_x2;
			vz = move_z2;
			speed = sqrt(vx*vx + vz*vz);
			if( speed > 0.0f ){
				vx = vx / speed;
				vz = vz / speed;
			}

			//足元を当たり判定
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + offset, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == true ){
				CollD->CheckALLBlockIntersectRay(pos_x, pos_y + offset, pos_z, vx, 0, vz, &id, &face, &Dist, speed);

				struct blockdata bdata;
				inblockdata->Getdata(&bdata, id);
				float surface_ang = acos(bdata.material[face].vy);

				if( surface_ang > DegreeToRadian((90.0f-1.0f)) ){			//壁なら
					//乗り越えられる高さか調べる
					if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y + 3.5f + offset, pos_z, vx, 0, vz, NULL, NULL, &Dist, speed) == false ){
						//人を上に持ち上げる
						FallDistance = 0.4f;
						move_y = 0.0f;
					}

					//足元を当たり判定
					CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
				}
				else if( surface_ang > HUMAN_MAPCOLLISION_SLOPEANGLE ){		//斜面～壁なら
					//足元を当たり判定
					CollD->ScratchVector(id, face, move_x2, vy, move_z2, &move_x2, &vy, &move_z2);
				}
				else{														//水平～斜面なら
					//地面と認めない　（ジャンプ対策）
					move_y_flag = true;

					//移動先の位置を計算
					newpos_x = pos_x + move_x2;
					newpos_y = pos_y + FallDistance;
					newpos_z = pos_z + move_z2;

					//移動先の高さを調べる
					if( CollD->CheckALLBlockInside(newpos_x, newpos_y + HUMAN_HEIGHT, newpos_z) == false ){
						if( CollD->CheckALLBlockIntersectRay(newpos_x, newpos_y + HUMAN_HEIGHT, newpos_z, 0, -1, 0, NULL, NULL, &Dist, HUMAN_HEIGHT) == true ){
							float height = HUMAN_HEIGHT - Dist;

							//人を上に持ち上げる
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

		//移動先の位置を計算
		newpos_x = pos_x + move_x2;
		newpos_y = pos_y + FallDistance;
		newpos_z = pos_z + move_z2;

		//全身を改めて確認
		if(
			(CollD->CheckALLBlockInside(newpos_x, newpos_y + offset, newpos_z) == true)||
			(CollD->CheckALLBlockIntersectRay(newpos_x, newpos_y + offset, newpos_z, 0, 1, 0, NULL, NULL, &Dist, HUMAN_HEIGHT - offset - 1.0f) == true)
		){
			//めり込むなら移動しない
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

//! @brief 計算を実行（当たり判定）
//! @param CollD Collisionのポインタ
//! @param inblockdata BlockDataInterfaceのポインタ
//! @param AddCollisionFlag 追加の当たり判定フラグ
//! @param player 対象の人物がプレイヤーかどうか
//! @param F5mode 上昇機能（F5裏技）のフラグ　（有効：true　無効：false）
//! @return 処理なし：0　通常処理：1　死亡して倒れ終わった直後：2　静止した死体：3　地形により死亡した直後：4
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

	//武器切り替えカウント
	if( selectweaponcnt > 0 ){
		selectweaponcnt -= 1;
	}

	//リロードカウント取得
	if( weapon[selectweapon] != NULL ){
		WeaponReloadCnt = weapon[selectweapon]->GetReloadCnt();
	}
	else{
		WeaponReloadCnt = 0;
	}

	//照準の状態誤差の処理
	GunsightErrorRange();

	//死亡判定と倒れる処理
	CheckDead = CheckAndProcessDead(CollD);
	if( CheckDead == 3 ){ return 2; }

	if( CheckDead == 0 ){
		//進行方向と速度を決定
		ControlProcess();

		//マップとの当たり判定
		hp_old = hp;
		MapCollisionDetection(CollD, inblockdata, AddCollisionFlag, &FallDistance, &nowmove_x, &nowmove_z);

		//移動するなら
		if( (nowmove_x*nowmove_x + nowmove_z*nowmove_z) > 0.0f ){
			totalmove += sqrt(nowmove_x*nowmove_x + nowmove_z*nowmove_z);
		}

		//座標移動
		pos_x += nowmove_x;
		pos_z += nowmove_z;

		//移動量を減衰
		move_x *= HUMAN_ATTENUATION;
		move_z *= HUMAN_ATTENUATION;

		//F5を使用していなければ、計算結果を反映
		if( F5mode == false ){
			pos_y += FallDistance;
		}
		else{
			move_y = 0.0f;
			pos_y += 5.0f;	//使用していれば、強制的に上昇
		}


		//-100.0より下に落ちたら、死亡
		if( pos_y < HUMAN_DEADLINE ){
			pos_y = HUMAN_DEADLINE;
			hp = 0;
			return 4;
		}

		//今回のマップとの当たり判定でHPがゼロになったなら、地形による死亡
		if( (hp_old >0)&&(hp <= 0) ){
			return 4;
		}
	}


	//現在装備する武器のクラスを取得
	class weapon *nowweapon;
	int weapon_paramid;
	nowweapon = weapon[selectweapon];

	if( nowweapon == NULL ){		//手ぶら
		weapon_paramid = ID_WEAPON_NONE;
	}
	else{							//何か武器を持っている
		nowweapon->GetParamData(&weapon_paramid, NULL, NULL);
	}

	//モーション計算
	MotionCtrl->RunFrame(rotation_x, armrotation_y, weapon_paramid, WeaponReloadCnt, MoveFlag_lt, hp, player);

	if( CheckDead != 0 ){ return 3; }
	return 1;
}

//! @brief 標準誤差を取得
int human::GetGunsightErrorRange()
{
	return StateGunsightErrorRange + ReactionGunsightErrorRange;
}

//! @brief 描画
//! @param d3dg D3DGraphicsのポインタ
//! @param Resource ResourceManagerのポインタ
//! @param DrawArm 腕と武器のみ描画する
//! @param player 対象の人物がプレイヤーかどうか
//! @todo 腕の位置を行列で求める
//! @todo 死体の部位の高さ（Y軸）がおかしい
void human::Render(class D3DGraphics *d3dg, class ResourceManager *Resource, bool DrawArm, bool player)
{
	//未使用引数対策
	UNREFERENCED_PARAMETER(player);

	//正しく初期化されていなければ、処理しない
	if( d3dg == NULL ){ return; }
	if( EnableFlag == false ){ return; }

	//モーション取得
	int upmodel, armmodel, legmodel;
	float armry, legrx;
	MotionCtrl->GetRenderMotion(&armry, &legrx, &upmodel, &armmodel, &legmodel);

	if( DrawArm == false ){
		//上半身を描画
		d3dg->SetWorldTransform(pos_x, pos_y - 1.0f, pos_z, rotation_x + (float)M_PI, rotation_y, upmodel_size);
		d3dg->RenderModel(upmodel, id_texture, DarkModelFlag);

		//足を描画
		d3dg->SetWorldTransform(pos_x, pos_y, pos_z, legrx + (float)M_PI, rotation_y, legmodel_size);
		d3dg->RenderModel(legmodel, id_texture, DarkModelFlag);
	}

	//現在装備する武器のクラスを取得
	class weapon *nowweapon;
	nowweapon = weapon[selectweapon];

	//腕を描画
	if( rotation_y != 0.0f ){		//死亡して倒れている or 倒れ始めた
		float x = pos_x + cos(rotation_x*-1 - (float)M_PI/2)*sin(rotation_y)*16.0f;
		float y = pos_y + cos(rotation_y)*16.0f;
		float z = pos_z + sin(rotation_x*-1 - (float)M_PI/2)*sin(rotation_y)*16.0f;
		d3dg->SetWorldTransform(x, y, z, rotation_x + (float)M_PI, armrotation_y + rotation_y, armmodel_size);
		d3dg->RenderModel(armmodel, id_texture, DarkModelFlag);
	}
	else if( nowweapon == NULL ){	//手ぶら
		d3dg->SetWorldTransform(pos_x, pos_y + 16.0f, pos_z, rotation_x + (float)M_PI, armry, armmodel_size);
		d3dg->RenderModel(armmodel, id_texture, DarkModelFlag);
	}
	else{							//何か武器を持っている
		//武器のモデルとテクスチャを取得
		int id_param;
		WeaponParameter paramdata;
		int model, texture;
		nowweapon->GetParamData(&id_param, NULL, NULL);
		Param->GetWeapon(id_param, &paramdata);
		Resource->GetWeaponModelTexture(id_param, &model, &texture);

		//腕を描画
		d3dg->SetWorldTransform(pos_x, pos_y + 16.0f, pos_z, rotation_x + (float)M_PI, armry, armmodel_size);
		d3dg->RenderModel(armmodel, id_texture, DarkModelFlag);

		//武器を描画
		d3dg->SetWorldTransformHumanWeapon(pos_x, pos_y + 16.0f, pos_z, paramdata.mx/10*-1, paramdata.my/10, paramdata.mz/10*-1, rotation_x + (float)M_PI, armry, paramdata.size);
		d3dg->RenderModel(model, texture, DarkModelFlag);
	}
}

//! @brief コンストラクタ
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

//! @brief ディストラクタ
weapon::~weapon()
{}

//! @brief 座標と角度を設定
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param rx 横軸回転
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

//! @brief 設定値を設定
//! @param id_param 武器の種類番号
//! @param lnbs 装弾数
//! @param nbs 合計弾数
//! @param init オブジェクトを初期化
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

//! @brief 設定値を取得
//! @param id_param 武器の種類番号を受け取るポインタ（NULL可）
//! @param lnbs 装弾数を受け取るポインタ（NULL可）
//! @param nbs 合計弾数を受け取るポインタ（NULL可）
void weapon::GetParamData(int *id_param, int *lnbs, int *nbs)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( lnbs != NULL ){ *lnbs = Loadbullets; }
	if( nbs != NULL ){ *nbs = bullets; }
}

//! @brief 武器の使用状況の取得
//! @return 使用中：true　未使用：false
bool weapon::GetUsingFlag()
{
	return usingflag;
}

//! @brief 武器を拾う
//! @return 成功：0　失敗：1
int weapon::Pickup()
{
	if( usingflag == true ){ return 1; }
	usingflag = true;
	return 0;
}

//! @brief 武器を捨てる
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param rx 横軸回転
//! @param speed 捨てる初速
void weapon::Dropoff(float x, float y, float z, float rx, float speed)
{
	//描画する座標と角度を設定
	move_x = cos(rx*-1 + (float)M_PI/2) * speed;
	move_y = 0.0f;
	move_z = sin(rx*-1 + (float)M_PI/2) * speed;
	pos_x = x + cos(rx*-1 + (float)M_PI/2) * 5.0f;
	pos_y = y + 16.0f + move_y;
	pos_z = z + sin(rx*-1 + (float)M_PI/2) * 5.0f;
	rotation_x = rx + (float)M_PI;

	//未使用（未装備）に設定し、座標移動を有効に
	usingflag = false;
	motionflag = true;
}

//! @brief 発砲
//! @return 成功：0　失敗：1
//! @attention 連射間隔も考慮されます。
//! @attention 関数が失敗するのは、いずれかの条件です。　「連射間隔に満たない」「リロード実行中」「弾がない」「無効な武器の種類が設定されている」
int weapon::Shot()
{
	//クラスが設定されていなければ失敗
	if( Param == NULL ){ return 1; }

	//発射間隔に満たないか、リロード中か、弾が無ければ失敗
	if( shotcnt > 0 ){ return 1; }
	if( reloadcnt > 0 ){ return 1; }
	if( Loadbullets == 0 ){ return 1; }

	//設定値を取得
	WeaponParameter ParamData;
	if( Param->GetWeapon(id_parameter, &ParamData) == 1 ){ return 1; }

	//武器が手榴弾ならば～
	if( id_parameter == ID_WEAPON_GRENADE ){
		//弾を減らし、連射カウントを設定
		bullets -= 1;
		Loadbullets -= 1;
		shotcnt = ParamData.blazings;

		if( (bullets - Loadbullets) <= 0 ){		//（リロードしていない）弾が無くなれば、武器ごと消滅させる。
			EnableFlag = false;
			usingflag = false;
		}
		else if( Loadbullets <= 0 ){			//自動リロード
			Loadbullets = 1;
		}
		return 0;
	}

	//弾を減らし、連射カウントを設定
	Loadbullets -= 1;
	bullets -= 1;
	shotcnt = ParamData.blazings;
	return 0;
}

//! @brief リロードを開始
//! @return 成功：0　失敗：1
//! @attention リロード時間も考慮されます。
//! @attention 関数が失敗するのは、いずれかの条件です。　「リロード実行中」「弾がない」「無効な武器の種類が設定されている」
int weapon::StartReload()
{
	//クラスが設定されていなければ失敗
	if( Param == NULL ){ return 1; }

	//リロード中か、弾が無ければ失敗
	if( reloadcnt > 0 ){ return 1; }
	if( (bullets - Loadbullets) == 0 ){ return 1; }

	//武器の性能値を取得
	WeaponParameter ParamData;
	if( Param->GetWeapon(id_parameter, &ParamData) != 0 ){ return 1; }

	//リロードカウントを設定
	reloadcnt = ParamData.reloads + 1;
	return 0;
}

//! @brief リロードを実行
//! @attention StartReload()関数と異なり、瞬時に弾を補充します。リロード時間は考慮されません。
//! @attention リロード時間を考慮する場合、StartReload()関数を呼び出してください。この関数は自動的に実行されます。
int weapon::RunReload()
{
	//クラスが設定されていなければ失敗
	if( Param == NULL ){ return 1; }

	//弾が無ければ失敗
	if( (bullets - Loadbullets) == 0 ){ return 1; }

	//武器の性能値から、装填する弾数を取得
	WeaponParameter ParamData;
	int nbsmax = 0;
	if( Param->GetWeapon(id_parameter, &ParamData) == 0 ){
		nbsmax = ParamData.nbsmax;
	}

	if( (bullets - Loadbullets) < nbsmax ){	//残りの弾数より装填する弾数が多ければ
		bullets = (bullets - Loadbullets);
		Loadbullets = bullets;
	}
	else{									//残りの弾数の方が多ければ
		bullets -= Loadbullets;
		Loadbullets = nbsmax;
	}

	return 0;
}

//! @brief リロードカウントを取得
//! @return カウント数　（リロード中：1以上）
int weapon::GetReloadCnt()
{
	return reloadcnt;
}

//! @brief 武器の種類・装弾数の変更
//! @param Resource ResourceManagerのポインタ
//! @param id_param 種類番号
//! @param lnbs 装弾数
//! @param nbs 合計弾数
//! @return 成功：1　失敗：0
//! @attention プレイヤーによる裏技（F6・F7）用に用意された関数です。手榴弾が選択された場合、自動的に弾を補充します。
//! @attention 使用されていない武器オブジェクトに対して実行すると、この関数は失敗します。
bool weapon::ResetWeaponParam(class ResourceManager *Resource, int id_param, int lnbs, int nbs)
{
	//初期化されていなければ、失敗
	if( EnableFlag == false ){ return 0; }

	//指定された設定値へ上書き
	id_parameter = id_param;
	bullets = nbs;
	Loadbullets = lnbs;

	//もし手榴弾ならば、自動リロード
	if( id_param == ID_WEAPON_GRENADE ){
		if( (bullets > 0)&&(Loadbullets == 0) ){
			Loadbullets = 1;
		}
	}

	//モデルとテクスチャを設定
	Resource->GetWeaponModelTexture(id_param, &id_model, &id_texture);
	WeaponParameter param;
	if( Param->GetWeapon(id_param, &param) == 0 ){
		model_size = param.size;
	}

	return 1;
}

//! @brief 計算を実行（自由落下）
//! @param CollD Collisionのポインタ
int weapon::RunFrame(class Collision *CollD)
{
	//クラスが設定されていなければ失敗
	if( CollD == NULL ){ return 0; }

	//初期化されていなければ、失敗
	if( EnableFlag == false ){ return 0; }

	//連射カウントが残っていれば、1 減らす
	if( shotcnt > 0 ){
		shotcnt -= 1;
	}
	else if( reloadcnt > 0 ){
		//リロードカウントが残っていれば 1 減らし、カウントが 0 ならばリロード処理を実行
		reloadcnt -= 1;
		if( reloadcnt == 0 ){
			RunReload();
		}
	}

	//誰にも使われておらず、移動フラグが有効ならば～
	if( (usingflag == false)&&(motionflag == true) ){
		float Dist;
		float maxDist;

		//移動速度を更新
		move_x *= 0.96f;
		move_z *= 0.96f;
		if( move_y > -1.8f ){
			move_y -= 0.3f;
		}

		//ブロックに埋まっていれば処理しない
		if( CollD->CheckALLBlockInside(pos_x, pos_y, pos_z) == true ){
			motionflag = false;
			return 0;
		}

		//水平の移動速度を求める
		maxDist = sqrt(move_x*move_x + move_z*move_z);
		if( maxDist < 0.1f ){
			maxDist = 0.0f;
			move_x = 0.0f;
			move_z = 0.0f;
		}

		//ブロックに接していれば、それ以上は水平移動しない。
		if( maxDist > 0.0f ){
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y, pos_z, move_x/maxDist, 0, move_z/maxDist, NULL, NULL, &Dist, maxDist) == true ){
				//ブロックに埋まらないように手前に
				Dist -= 0.1f;

				//接している座標まで移動
				pos_x += move_x/maxDist * Dist;
				pos_z += move_z/maxDist * Dist;

				//移動量を 0 に
				move_x = 0.0f;
				move_z = 0.0f;
			}
		}

		//ブロックに接していれば、そこまで落下する
		if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y, pos_z, 0, -1, 0, NULL, NULL, &Dist, fabs(move_y)) == true ){
			CollD->CheckALLBlockIntersectRay(pos_x, pos_y, pos_z, 0, -1, 0, NULL, NULL, &Dist, fabs(move_y));
			pos_y -= Dist - 0.2f;
			motionflag = false;
			return 0;
		}

		//座標を反映
		pos_x += move_x;
		pos_y += move_y;
		pos_z += move_z;
	}

	return 0;
}

//! @brief 描画
//! @param d3dg D3DGraphicsのポインタ
void weapon::Render(class D3DGraphics *d3dg)
{
	//クラスが設定されていなければ失敗
	if( d3dg == NULL ){ return; }

	//初期化されてないか、誰かに使われていれば処理しない
	if( EnableFlag == false ){ return; }
	if( usingflag == true ){ return; }

	//武器を描画
	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, rotation_x, 0.0f, (float)M_PI/2, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag);
}

//! @brief コンストラクタ
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

	hp = 0;		//暫定
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

//! @brief ディストラクタ
smallobject::~smallobject()
{}

//! @brief MIFデータを管理するクラスを設定
//! @param in_MIFdata MIFInterfaceクラスのポインタ
void smallobject::SetMIFInterfaceClass(class MIFInterface *in_MIFdata)
{
	MIFdata = in_MIFdata;
}

//! @brief 設定値を設定
//! @param id_param 小物の種類番号
//! @param p4 第4パラメータ
//! @param init オブジェクトを初期化
void smallobject::SetParamData(int id_param, signed char p4, bool init)
{
	rotation_y = 0.0f;
	id_parameter = id_param;
	point_p4 = p4;

	if( init == true ){
		hp = 0;		//暫定
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

//! @brief 設定値を取得
//! @param id_param 小物の種類番号を受け取るポインタ（NULL可）
//! @param p4 第4パラメータを受け取るポインタ（NULL可）
void smallobject::GetParamData(int *id_param, signed char *p4)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( p4 != NULL ){ *p4 = point_p4; }
}

//! @brief 体力を取得
//! @return 体力値
int smallobject::GetHP()
{
	return hp;
}

//! @brief ブロックの上に移動
//! @param CollD Collisionのポインタ
//! @return 元の座標からの移動量（0で移動なし）
float smallobject::CollisionMap(class Collision *CollD)
{
	//クラスが設定されていなければ失敗
	if( CollD == NULL ){ return 0.0f; }

	float Dist;
	SmallObjectParameter ParamData;

	//ブロックに埋まっていれば、そのまま
	if( CollD->CheckALLBlockInside(pos_x, pos_y, pos_z) == true ){ return 0.0f; }

	//下方向に当たり判定
	if( CollD->CheckALLBlockIntersectRay(pos_x, pos_y, pos_z, 0, -1, 0, NULL, NULL, &Dist, 1000.0f) == true ){
		//当たり判定の大きさを取得
		if( id_parameter == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
			Dist -= (float)MIFdata->GetAddSmallobjectDecide()/10.0f;
		}
		else{
			if( Param->GetSmallObject(id_parameter, &ParamData) == 0 ){
				Dist -= (float)ParamData.decide/10.0f;
			}
		}

		//座標を移動する
		pos_y -= Dist;
		return Dist;
	}

	return 0.0f;
}

//! @brief 弾がヒットした
//! @param attacks 弾の攻撃力
void smallobject::HitBullet(int attacks)
{
	hp -= attacks;
	if( hp <= 0 ){
		Destruction();
	}
}

//! @brief 手榴弾の爆風がヒットした
//! @param attacks 爆風の攻撃力
//! @attention 距離による計算を事前に済ませてください。
void smallobject::HitGrenadeExplosion(int attacks)
{
	hp -= attacks;
	if( hp <= 0 ){
		Destruction();
	}
}

//! @brief 小物を破壊する
//! @attention 通常は HitBullet()関数 および GrenadeExplosion()関数 から自動的に実行されるため、直接呼び出す必要はありません。
void smallobject::Destruction()
{
	//EnableFlag = false;
	//return;

	int jump;

	//飛び具合を取得
	if( id_parameter == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
		jump = MIFdata->GetAddSmallobjectJump();
	}
	else{
		SmallObjectParameter paramdata;
		Param->GetSmallObject(id_parameter, &paramdata);
		jump = paramdata.jump;
	}

	//飛ばす
	hp = 0;
	jump_cnt = jump;

	//姿勢設定
	jump_rx = DegreeToRadian(10) * GetRand(36);
	move_rx = (float)jump * 0.04243f;
	add_rx = DegreeToRadian(1) * GetRand(20);
	add_ry = DegreeToRadian(1) * GetRand(20);
}

//! @brief 計算を実行（破壊時の移動など）
int smallobject::RunFrame()
{
	//描画されていないか、体力が残っていなければ処理しない。
	if( EnableFlag == false ){ return 0; }
	if( hp > 0 ){ return 0; }

	int cnt;
	int jump;

	//飛び具合を取得
	if( id_parameter == TOTAL_PARAMETERINFO_SMALLOBJECT+1 -1 ){
		jump = MIFdata->GetAddSmallobjectJump();
	}
	else{
		SmallObjectParameter paramdata;
		Param->GetSmallObject(id_parameter, &paramdata);
		jump = paramdata.jump;
	}
	
	//吹き飛んでいるカウント数を計算
	cnt = jump - jump_cnt;

	//姿勢から座標・角度を計算
	pos_x += cos(jump_rx) * move_rx;
	pos_y += jump_cnt * 0.1f;
	pos_z += sin(jump_rx) * move_rx;
	rotation_x += add_rx;
	rotation_y += add_ry;

	jump_cnt -= 1;

	//1秒飛んでいたら描画終了
	if( cnt > (int)GAMEFPS ){
		EnableFlag = false;
		return 2;
	}

	return 1;
}

//! @brief 描画
//! @param d3dg D3DGraphicsのポインタ
void smallobject::Render(D3DGraphics *d3dg)
{
	//クラスが設定されていなければ失敗
	if( d3dg == NULL ){ return; }

	//初期化されていなければ処理しない。
	if( EnableFlag == false ){ return; }

	//描画
	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, rotation_x, rotation_y, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag);
}

//! @brief コンストラクタ
bullet::bullet(int modelid, int textureid)
{
	model_size = 1.0f;
	id_model = modelid;
	id_texture = textureid;
	EnableFlag = false;
}

//! @brief ディストラクタ
bullet::~bullet()
{}

//! @brief 座標と角度を設定
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param rx 横軸回転
//! @param ry 縦軸回転
void bullet::SetPosData(float x, float y, float z, float rx, float ry)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;
	rotation_y = ry;
}

//! @brief 設定値を設定
//! @param _attacks 攻撃力
//! @param _penetration 貫通力
//! @param _speed 弾速
//! @param _teamid チーム番号
//! @param _humanid 人のデータ番号
//! @param init オブジェクトを初期化
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

//! @brief 座標と角度を取得
//! @param x X座標を受け取るポインタ（NULL可）
//! @param y Y座標を受け取るポインタ（NULL可）
//! @param z Z座標を受け取るポインタ（NULL可）
//! @param rx 横軸回転を受け取るポインタ（NULL可）
//! @param ry 縦軸回転を受け取るポインタ（NULL可）
void bullet::GetPosData(float *x, float *y, float *z, float *rx, float *ry)
{
	if( x != NULL ){ *x = pos_x; }
	if( y != NULL ){ *y = pos_y; }
	if( z != NULL ){ *z = pos_z; }
	if( rx != NULL ){ *rx = rotation_x; }
	if( ry != NULL ){ *ry = rotation_y; }
}

//! @brief 設定値を取得
//! @param _attacks 攻撃力を受け取るポインタ（NULL可）
//! @param _penetration 貫通力を受け取るポインタ（NULL可）
//! @param _speed 弾速を受け取るポインタ（NULL可）
//! @param _teamid チーム番号を受け取るポインタ（NULL可）
//! @param _humanid 人のデータ番号を受け取るポインタ（NULL可）
void bullet::GetParamData(int *_attacks, int *_penetration, int *_speed, int *_teamid, int *_humanid)
{
	if( _attacks != NULL ){ *_attacks = attacks; }
	if( _penetration != NULL ){ *_penetration = penetration; }
	if( _speed != NULL ){ *_speed = speed; }
	if( _teamid != NULL ){ *_teamid = teamid; }
	if( _humanid != NULL ){ *_humanid = humanid; }
}

//! @brief 計算を実行（弾の進行・時間消滅）
int bullet::RunFrame()
{
	//初期化されていなければ処理しない
	if( EnableFlag == false ){ return 0; }

	//消滅時間を過ぎていれば、オブジェクトを無効化
	if( cnt > BULLET_DESTROYFRAME ){
		EnableFlag = false;
		return 0;
	}

	//移動処理
	pos_x += cos(rotation_x)*cos(rotation_y)*speed;
	pos_y += sin(rotation_y)*speed;
	pos_z += sin(rotation_x)*cos(rotation_y)*speed;
	cnt += 1;

	return 0;
}

//! @brief 描画
//! @param d3dg D3DGraphicsのポインタ
void bullet::Render(class D3DGraphics *d3dg)
{
	//クラスが設定されていなければ失敗
	if( d3dg == NULL ){ return; }

	//初期化されていなければ処理しない。
	if( EnableFlag == false ){ return; }

	//弾を移動前だったら描画しない
	//　弾が頭から突き抜けて見える対策
	if( cnt == 0 ){ return; }

	//描画
	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, (rotation_x * -1 - (float)M_PI/2), rotation_y, model_size);
	d3dg->RenderModel(id_model, id_texture, false);
}

//! @brief コンストラクタ
grenade::grenade(int modelid, int textureid) : bullet(modelid, textureid)
{
	if( Param != NULL ){
		WeaponParameter ParamData;
		if( Param->GetWeapon(ID_WEAPON_GRENADE, &ParamData) == 0 ){
			model_size = ParamData.size;
		}
	}
}

//! @brief ディストラクタ
grenade::~grenade()
{}

//! @brief 座標と情報を設定
//! @param speed 初速
//! @param _teamid チーム番号
//! @param _humanid 人のデータ番号
//! @param init オブジェクトを初期化
//! @attention 先に SetPosData() を実行してください。
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

//! @brief 設定値を取得
//! @param _speed 速度を受け取るポインタ（NULL可）
//! @param _teamid チーム番号を受け取るポインタ（NULL可）
//! @param _humanid 人のデータ番号を受け取るポインタ（NULL可）
void grenade::GetParamData(float *_speed, int *_teamid, int *_humanid)
{
	if( _speed != NULL ){ *_speed = GetSpeed(); }
	if( _teamid != NULL ){ *_teamid = teamid; }
	if( _humanid != NULL ){ *_humanid = humanid; }
}

//! @brief 速度を取得
//! @return 速度
float grenade::GetSpeed()
{
	return sqrt(move_x*move_x + move_y*move_y + move_z*move_z);
}

//! @brief 計算を実行（手榴弾の進行・爆発）
//! @return 爆発：2　バウンド・跳ね返り：1　それ以外：0
int grenade::RunFrame(class Collision *CollD)
{
	//初期化されていなければ処理しない
	if( EnableFlag == false ){ return 0; }

	//時間を過ぎていれば、オブジェクトを無効化し、「爆発」として返す。
	if( cnt > GRENADE_DESTROYFRAME ){
		EnableFlag = false;
		return 2;
	}

	//静止していれば処理しない
	if( (move_x == 0.0f)&&(move_y == 0.0f)&&(move_z == 0.0f) ){
		cnt += 1;
		return 0;
	}

	/*
	//静止に近い状態ならば、移動処理をしない。
	if( (move_x*move_x + move_y*move_y + move_z*move_z) < 0.1f*0.1f ){
		cnt += 1;
		return 0;
	}
	*/

	//移動速度を計算
	move_x *= 0.98f;
	move_y = (move_y - 0.17f) * 0.98f;
	move_z *= 0.98f;

	int id, face;
	float Dist;
	float maxDist = sqrt(move_x*move_x + move_y*move_y + move_z*move_z);

	//マップに対して当たり判定を実行
	if( CollD->CheckALLBlockIntersectRay(pos_x, pos_y, pos_z, move_x/maxDist, move_y/maxDist, move_z/maxDist, &id, &face, &Dist, maxDist) == true ){
		float vx, vy, vz;

		//マップと衝突する座標まで移動
		pos_x += move_x/maxDist * (Dist - 0.1f);
		pos_y += move_y/maxDist * (Dist - 0.1f);
		pos_z += move_z/maxDist * (Dist - 0.1f);

		//反射するベクトルを求める
		CollD->ReflectVector(id, face, move_x, move_y, move_z, &vx, &vy, &vz);

		//減速
		move_x = vx * GRENADE_BOUND_ACCELERATION;
		move_y = vy * GRENADE_BOUND_ACCELERATION;
		move_z = vz * GRENADE_BOUND_ACCELERATION;

		cnt += 1;
		return 1;
	}

	//座標を移動
	pos_x += move_x;
	pos_y += move_y;
	pos_z += move_z;

	cnt += 1;
	return 0;
}

//! @brief 描画
//! @param d3dg D3DGraphicsのポインタ
void grenade::Render(class D3DGraphics *d3dg)
{
	//クラスが設定されていなければ失敗
	if( d3dg == NULL ){ return; }

	//初期化されていなければ処理しない。
	if( EnableFlag == false ){ return; }

	//描画
	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, (rotation_x * -1 - (float)M_PI/2), 0.0f, (float)M_PI/2, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag);
}

//! @brief コンストラクタ
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

//! @brief ディストラクタ
effect::~effect()
{}

//! @brief 設定値を設定
//! @param in_move_x X軸移動量
//! @param in_move_y Y軸移動量
//! @param in_move_z Z軸移動量
//! @param size 描画倍率
//! @param rotation 回転角度
//! @param count 描画フレーム数
//! @param texture テクスチャの認識番号
//! @param settype エフェクトの種類　（Effect_Type を組み合せる）
//! @param init オブジェクトを初期化
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

//! @brief 横軸と縦軸の向きを設定
//! @param rx 設定する横軸
//! @param ry 設定する縦軸
void effect::SetRxRy(float rx, float ry)
{
	rotation_x = rx;
	rotation_y = ry;
}

//! テクスチャ認識番号を取得
//! @return テクスチャ認識番号
int effect::GetTextureID()
{
	return id_texture;
}

//! エフェクトの移動量を取得
//! @param mx X軸移動量を受け取るポインタ
//! @param my Y軸移動量を受け取るポインタ
//! @param mz Z軸移動量を受け取るポインタ
void effect::GetMove(float *mx, float *my, float *mz)
{
	*mx = move_x;
	*my = move_y;
	*mz = move_z;
}

//! エフェクトのマップ当たり判定フラグ取得
//! @return 有効：true　無効：false
bool effect::GetCollideMapFlag()
{
	if( type&EFFECT_COLLIDEMAP ){
		return true;
	}
	//else{
		return false;
	//}
}

//! @brief 計算を実行（ビルボード化）
//! @param camera_rx カメラの横軸角度
//! @param camera_ry カメラの縦軸角度
//! @return 処理実行：1　描画最終フレーム：2　処理なし：0
int effect::RunFrame(float camera_rx, float camera_ry)
{
	//初期化されていなければ処理しない
	if( EnableFlag == false ){ return 0; }

	//カウントが終了したら、処理しないように設定
	if( cnt <= 0 ){
		EnableFlag = false;
		return 2;
	}

	if( (type & EFFECT_NOBILLBOARD) == 0 ){
		//カメラ座標を適用し、ビルボード化する
		rotation_x = camera_rx*-1;
		rotation_y = camera_ry;
	}

	//座標移動
	pos_x += move_x;
	pos_y += move_y;
	pos_z += move_z;

	//特殊処理を実行
	if( type & EFFECT_DISAPPEAR ){	//消す
		alpha -= set_alpha/setcnt;
	}
	if( type & EFFECT_DISAPPEARHALF ){	//半分の時間で消す
		if( setcnt/2 > cnt ){
			alpha -= set_alpha/(setcnt/2);
		}
	}
	if( type & EFFECT_MAGNIFY ){	//拡大
		model_size += model_size/50;
	}
	if( type & EFFECT_ROTATION ){	//回転
		if( rotation_texture > 0.0f ){
			rotation_texture += DegreeToRadian(1);
		}
		else{
			rotation_texture -= DegreeToRadian(1);
		}
	}
	if( type & EFFECT_FALL ){		//落下
		move_y = (move_y - 0.17f) * 0.98f;
	}

	//カウントを 1 引く
	cnt -= 1;
	return 1;
}

//! @brief 描画
//! @param d3dg D3DGraphicsのポインタ
void effect::Render(class D3DGraphics *d3dg)
{
	//クラスが設定されていなければ失敗
	if( d3dg == NULL ){ return; }

	//初期化されていなければ処理しない。
	if( EnableFlag == false ){ return; }

	//描画
	d3dg->SetWorldTransformEffect(pos_x, pos_y, pos_z, rotation_x, rotation_y, rotation_texture, model_size);
	d3dg->RenderBoard(id_texture, alpha);
}

//! @brief コンストラクタ
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

//! @brief ディストラクタ
HumanMotionControl::~HumanMotionControl()
{}

//! @brief 設定値を管理するクラスを登録
//! @attention 各関数を使用する前に実行すること。
void HumanMotionControl::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;
}

//! @brief モデルデータを設定
//! @param upmodel 上半身のモデル
//! @param armmodel[] 腕のモデルの配列（配列数：TOTAL_ARMMODE）
//! @param legmodel 足（静止状態）のモデル
//! @param walkmodel[] 腕のモデルの配列（配列数：TOTAL_WALKMODE）
//! @param runmodel[] 腕のモデルの配列（配列数：TOTAL_RUNMODE）
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

//! @brief 初期化系関数
//! @param rx 回転角度
void HumanMotionControl::Init(float rx)
{
	reaction_y = 0.0f;
	armmodel_rotation_y = 0.0f;
	legrotation_x = rx;
	walkcnt = 0;
	runcnt = 0;
}

//! @brief 武器を拾う
//! @param weapon_paramid 武器の種類番号
void HumanMotionControl::PickupWeapon(int weapon_paramid)
{
	//未使用引数対策
	UNREFERENCED_PARAMETER(weapon_paramid);

	//腕の角度（反動）を設定
	reaction_y = DegreeToRadian(-20);
}

//! @brief 武器を切り替える（持ち替える）
//! @param weapon_paramid 武器の種類番号
void HumanMotionControl::ChangeWeapon(int weapon_paramid)
{
	//未使用引数対策
	UNREFERENCED_PARAMETER(weapon_paramid);

	//腕の角度（反動）を設定
	reaction_y = DegreeToRadian(-20);
}

//! @brief 発砲処理
//! @param weapon_paramid 武器の種類番号
void HumanMotionControl::ShotWeapon(int weapon_paramid)
{
	//腕に反動を伝える
	if( weapon_paramid == ID_WEAPON_GRENADE ){
		reaction_y = DegreeToRadian(20);
	}
	else{
		//武器の設定値を取得
		WeaponParameter ParamData;
		if( Param->GetWeapon(weapon_paramid, &ParamData) != 0 ){ return; }

		reaction_y = DegreeToRadian(0.5f) * ParamData.reaction;
	}
}

//! @brief リロード
//! @param weapon_paramid 武器の種類番号
//! @attention この関数は拡張用のダミー関数です。
void HumanMotionControl::ReloadWeapon(int weapon_paramid)
{
	//未使用引数対策
	UNREFERENCED_PARAMETER(weapon_paramid);
}

//! @brief 武器を捨てる
//! @attention この関数は拡張用のダミー関数です。
void HumanMotionControl::DumpWeapon()
{
	//
}

//! @brief 武器のショットモード切り替え
//! @param weapon_paramid 武器の種類番号
//! @attention この関数は拡張用のダミー関数です。
void HumanMotionControl::ChangeShotMode(int weapon_paramid)
{
	//未使用引数対策
	UNREFERENCED_PARAMETER(weapon_paramid);
}

//! @brief ジャンプ
//! @attention この関数は拡張用のダミー関数です。
void HumanMotionControl::Jump()
{
	//
}

//! @brief モーション計算を実行
//! @attention rotation_x 体全体の回転角度
//! @attention armrotation_y 腕の回転角度
//! @attention weapon_paramid 武器の種類番号
//! @attention ReloadCnt 武器のリロードカウント
//! @attention MoveFlag 移動方向を表すフラグ
//! @attention hp 体力
//! @attention PlayerFlag プレイヤーかどうか
void HumanMotionControl::RunFrame(float rotation_x, float armrotation_y, int weapon_paramid, int ReloadCnt, int MoveFlag, int hp, bool PlayerFlag)
{
	int ArmModelID;
	float move_rx;

	//発砲などによる反動
	if( reaction_y > 0.0f ){
		if( reaction_y > DegreeToRadian(2) ){ reaction_y -= DegreeToRadian(2); }
		else{ reaction_y = 0.0f; }
	}
	if( reaction_y < 0.0f ){
		if( reaction_y < DegreeToRadian(2) ){ reaction_y += DegreeToRadian(2); }
		else{ reaction_y = 0.0f; }
	}

	//リロード中なら腕の角度を再設定
	if( ReloadCnt > 0 ){
		reaction_y = ARMRAD_RELOADWEAPON;
	}

	if( weapon_paramid == ID_WEAPON_NONE ){	//手ぶら
		if( PlayerFlag == true ){
			armmodel_rotation_y = ARMRAD_NOWEAPON;
		}
		else{
			armmodel_rotation_y = armrotation_y;
		}
		ArmModelID = 0;
	}
	else{							//何か武器を持っている
		//武器の情報を取得
		WeaponParameter paramdata;
		Param->GetWeapon(weapon_paramid, &paramdata);

		armmodel_rotation_y = 0.0f;
		ArmModelID = 0;

		//腕の形と角度を決定
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

	//進行方向を決定
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

	//足の角度を算出
	if( hp <= 0 ){
		legrotation_x = rotation_x;
	}
	else{
		float move_rx2;

		//足の向きを求める
		if( fabs(move_rx) > DegreeToRadian(90)){
			move_rx2 = move_rx + (float)M_PI;
		}
		else{
			move_rx2 = move_rx;
		}
		for(; move_rx2 > (float)M_PI; move_rx2 -= (float)M_PI*2){}
		for(; move_rx2 < (float)M_PI*-1; move_rx2 += (float)M_PI*2){}

		//徐々にその向きに
		legrotation_x = legrotation_x*0.85f + (rotation_x + move_rx2*-1)*0.15f;		// 3/4 + 1/4
	}

	//腕のモデル設定
	armmodelid = id_armmodel[ArmModelID];

	//足のモデル設定
	legmodelid = id_legmodel;	//立ち止まり
	if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
		legmodelid = id_walkmodel[ (walkcnt/3 % TOTAL_WALKMODE) ];	//歩き
		walkcnt += 1;
		runcnt = 0;
	}
	else{
		if( GetFlag(MoveFlag, (MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
			legmodelid = id_runmodel[ (runcnt/3 % TOTAL_RUNMODE) ];		//左右走り
		}
		if( GetFlag(MoveFlag, MOVEFLAG_FORWARD) ){
			legmodelid = id_runmodel[ (runcnt/2 % TOTAL_RUNMODE) ];		//前走り
		}
		if( GetFlag(MoveFlag, MOVEFLAG_BACK) ){
			legmodelid = id_runmodel[ (runcnt/4 % TOTAL_RUNMODE) ];		//後ろ走り
		}
		walkcnt = 0;
		runcnt += 1;
	}
}

//! @brief モーション取得
//! @param arm_rotation_y 腕の角度を取得するポインタ
//! @param leg_rotation_x 足の角度を取得するポインタ
//! @param upmodel 上半身のモデル認識番号を取得するポインタ
//! @param armmodel 腕のモデル認識番号を取得するポインタ
//! @param legmodel 足のモデル認識番号を取得するポインタ
void HumanMotionControl::GetRenderMotion(float *arm_rotation_y, float *leg_rotation_x, int *upmodel, int *armmodel, int *legmodel)
{
	*arm_rotation_y = armmodel_rotation_y;
	*leg_rotation_x = legrotation_x;

	*upmodel = id_upmodel;
	*armmodel = armmodelid;
	*legmodel = legmodelid;
}