//! @file ai.cpp
//! @brief AIcontrolクラスの定義

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

#include "ai.h"

//! コンストラクタ
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

//! ディストラクタ
AIcontrol::~AIcontrol()
{}

//! 対象クラスを設定
//! @attention この関数で設定を行わないと、クラス自体が正しく機能しません。
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

//! ランダムな整数値を返す
//! @param num 範囲
//! @return 0〜num-1
int AIcontrol::random(int num)
{
	return GetRand(num);
}

//! 人を検索
//! @param in_p4 検索する人の第4パラメータ（認識番号）
//! @param out_x X座標を受け取るポインタ
//! @param out_z Z座標を受け取るポインタ
//! @return 成功：1　失敗：0
int AIcontrol::SearchHumanPos(signed char in_p4, float *out_x, float *out_z)
{
	float x, z;
	human* thuman;

	//人を検索してクラスを取得
	thuman = ObjMgr->SearchHuman(in_p4);
	if( thuman == NULL ){ return 0; }

	//X・Z座標を取得
	thuman->GetPosData(&x, NULL, &z, NULL);
	*out_x = x;
	*out_z = z;
	return 1;
}

//! 目標地点に移動しているか確認
//! @return 到達：true　非到達：false
bool AIcontrol::CheckTargetPos()
{
	//距離を算出
	float x = posx - target_posx;
	float z = posz - target_posz;
	float r = x * x + z * z;

	if( movemode == AI_TRACKING ){	//追尾中なら
		if( r < AI_ARRIVALDIST_TRACKING * AI_ARRIVALDIST_TRACKING ){
			return true;
		}
	}
	else{							//それ以外なら
		if( r < (AI_ARRIVALDIST_PATH * AI_ARRIVALDIST_PATH) ){
			return true;
		}
	}

	return false;
}

//! 目標地点を検索
//! @param next 次を検索する
//! @return 完了：true　失敗：false
bool AIcontrol::SearchTarget(bool next)
{
	//ポイントの情報を取得
	pointdata pdata;
	if( Points->Getdata(&pdata, target_pointid) != 0 ){
		movemode = AI_NULL;
		return false;
	}

	int nextpointp4;

	//次のポイントを検索するなら
	if( next == true ){
		nextpointp4 = pdata.p3;

		//ランダムパス処理
		if( pdata.p1 == 8 ){
			if( random(2) == 0 ){
				nextpointp4 = pdata.p2;
			}
			else{
				nextpointp4 = pdata.p3;
			}
			movemode = AI_RANDOM;
		}

		//ポイントを検索
		if( Points->SearchPointdata(&pdata, 0x08, 0, 0, 0, nextpointp4, 0) == 0 ){
			return false;
		}
	}

	//ランダムパスなら次へ
	if( pdata.p1 == 8 ){
		target_pointid = pdata.id;
		movemode = AI_RANDOM;
		return false;
	}

	//人なら座標を取得
	if( (pdata.p1 == 1)||(pdata.p1 == 6) ){
		SearchHumanPos(pdata.p4, &target_posx, &target_posz);
		return true;
	}

	//移動パスなら〜
	if( pdata.p1 == 3 ){
		//情報適用
		target_pointid = pdata.id;
		target_posx = pdata.x;
		target_posz = pdata.z;
		target_rx = pdata.r;

		//移動ステート設定
		switch(pdata.p2){
			case 0: movemode = AI_WALK; break;
			case 1: movemode = AI_RUN; break;
			case 2: movemode = AI_WAIT; break;
			case 3:
				movemode = AI_TRACKING;
				if( next == true ){
					nextpointp4 = pdata.p3;

					//ポイント（人）の情報を取得
					if( Points->SearchPointdata(&pdata, 0x08, 0, 0, 0, nextpointp4, 0) == 0 ){
						return false;
					}

					//情報保存
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

//! 目標地点に移動
void AIcontrol::MoveTarget()
{
	float x = posx - target_posx;
	float z = posz - target_posz;
	float r = x * x + z * z;
	float atan;
	int paramid;
	HumanParameter Paraminfo;
	bool zombie;

	//ゾンビかどうか判定
	ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
	Param->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 2 ){
		zombie = true;
	}
	else{
		zombie = false;
	}

	//目標地点への角度を求める
	atan = atan2(x, z) - rx + (float)M_PI;
	for(; atan > (float)M_PI; atan -= (float)M_PI*2){}
	for(; atan < (float)M_PI*-1; atan += (float)M_PI*2){}

	//大きな差があれば少しづつ旋回するだけ
	if( atan > AI_TURNRAD ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( atan < AI_TURNRAD*-1 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}

	//微々たる差なら一気に向く
	if( (atan <= AI_TURNRAD) && (atan >= AI_TURNRAD*-1) ){
		DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
		DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
		rx += atan;
	}

	//前進する
	if( zombie == true ){
		if( abs(atan) < (float)M_PI/18*2 ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
		}
	}
	else if( battlemode == AI_CAUTION ){
		if( abs(atan) < (float)M_PI/18*5 ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
		}
	}
	else if( movemode == AI_WALK ){
		if( abs(atan) < (float)M_PI/180*6 ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
		}
	}
	else if( (movemode == AI_RUN)||(movemode == AI_RUN2) ){
		if( abs(atan) < (float)M_PI/18*5 ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
		}
	}
	else if( (movemode == AI_WAIT)||(movemode == AI_STOP) ){
		if( abs(atan) < (float)M_PI/180*6 ){
			SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
		}
	}
	else{	//movemode == AI_TRACKING
		if( abs(atan) < (float)M_PI/18*2 ){
			if( r < (AI_ARRIVALDIST_WALKTRACKING * AI_ARRIVALDIST_WALKTRACKING) ){
				SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
			}
			else{
				SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			}
		}
	}

	//ジャンプ
	if( random(128) == 0 ){
		MoveJump();
	}

	//引っ掛かっていたら、左右への回転をランダムに行う
	if( random(28) == 0 ){
		if( ctrlhuman->GetMovemode(true) != 0 ){
			if( ctrlhuman->GetTotalMove() - total_move < 0.1f ){
				if( random(2) == 0 ){ SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT); }
				else{ SetFlag(moveturn_mode, AI_CTRL_TURNLEFT); }
			}
		}
	}
	total_move = ctrlhuman->GetTotalMove();
}

//! 目標地点に移動（優先的な走り用）
void AIcontrol::MoveTarget2()
{
	float x = target_posx - posx;
	float z = target_posz - posz;
	float atan, rx2, trx;

	//人の向き・視点を求める　（0.0〜2π）
	rx2 = rx*-1 + (float)M_PI/2;
	for(; rx2 > (float)M_PI*2; rx2 -= (float)M_PI*2){}
	for(; rx2 < 0.0f; rx2 += (float)M_PI*2){}

	//ポイントまでの角度を求める　（0.0〜2π）
	trx = atan2(z, x);
	if( trx < 0.0f ){ trx += (float)M_PI*2; }

	//視点を基準にポイントまでの角度を算出（-π〜π）
	atan = trx - rx2;
	for(; atan > (float)M_PI; atan -= (float)M_PI*2){}
	for(; atan < (float)M_PI*-1; atan += (float)M_PI*2){}

	//前後移動の処理
	if( abs(atan) < (float)M_PI/180*56 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
	}
	if( abs(atan) > (float)M_PI/180*123.5f ){
		SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
	}

	//左右移動の処理
	if( ((float)M_PI/180*-146 < atan)&&(atan < (float)M_PI/180*-33) ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( ((float)M_PI/180*33 < atan)&&(atan < (float)M_PI/180*146) ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}

	//ジャンプ
	if( random(128) == 0 ){
		MoveJump();
	}

	//引っ掛かっていたら、左右への回転をランダムに行う
	if( random(28) == 0 ){
		if( ctrlhuman->GetMovemode(true) != 0 ){
			if( ctrlhuman->GetTotalMove() - total_move < 0.1f ){
				if( random(2) == 0 ){ SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT); }
				else{ SetFlag(moveturn_mode, AI_CTRL_TURNLEFT); }
			}
		}
	}
	total_move = ctrlhuman->GetTotalMove();
}

//! 前後左右ランダムに移動（攻撃中用）
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

	//ランダムに移動を始める
	if( random(forwardstart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
	}
	if( random(backstart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
	}
	if( random(sidestart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVELEFT);
	}
	if( random(sidestart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
	}

	// 1/3の確率か、移動フラグが設定されていたら
	if( (random(3) == 0)||(GetFlag(moveturn_mode, (AI_CTRL_MOVEFORWARD | AI_CTRL_MOVEBACKWARD | AI_CTRL_MOVELEFT | AI_CTRL_MOVERIGHT))) ){
		float vx, vz;
		float Dist;

		if( random(2) == 0 ){
			//前方向のベクトルを計算
			vx = cos(rx*-1 + (float)M_PI/2);
			vz = sin(rx*-1 + (float)M_PI/2);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGTH, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//腰の高さにブロックがある（ぶつかる）
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 5.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//足元にブロックがない（落ちる）
			){
				//前進フラグを削除し、後退フラグを設定
				DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
				SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
			}

			//後方向のベクトルを計算
			vx = cos(rx*-1 + (float)M_PI/2 + (float)M_PI);
			vz = sin(rx*-1 + (float)M_PI/2 + (float)M_PI);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGTH, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//腰の高さにブロックがある（ぶつかる）
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 5.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//足元にブロックがない（落ちる）
			){
				//後退フラグを削除し、前進フラグを設定
				DelFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
				SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			}
		}
		else{
			//
			vx = cos(rx*-1);
			vz = sin(rx*-1);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGTH, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//腰の高さにブロックがある（ぶつかる）
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 5.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//足元にブロックがない（落ちる）
			){
				//右移動フラグを削除し、左移動フラグを設定
				DelFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
				SetFlag(moveturn_mode, AI_CTRL_MOVELEFT);
			}

			vx = cos(rx*-1 + (float)M_PI);
			vz = sin(rx*-1 + (float)M_PI);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGTH, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//腰の高さにブロックがある（ぶつかる）
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 5.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//足元にブロックがない（落ちる）
			){
				//左移動フラグを削除し、右移動フラグを設定
				DelFlag(moveturn_mode, AI_CTRL_MOVELEFT);
				SetFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
			}
		}
	}

	//攻撃対象がいるなら
	if( enemyhuman != NULL ){
		float tx, ty, tz;

		enemyhuman->GetPosData(&tx, &ty, &tz, NULL);

		float x = posx - tx;
		float y = posy - ty;
		float z = posz - tz;
		float r = x * x + y * y + z * z;

		//敵に近づきすぎたなら後退する
		if( r < 20.0f * 20.0f ){
			DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			if( random(70) == 0 ){
				SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
			}
		}
	}
}

//! その場を見まわす
void AIcontrol::TurnSeen()
{
	int turnstart, turnstop;

	//回転の開始・終了確率を設定
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

	//ランダムに回転を始める
	if( random(turnstart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( random(turnstart) == 0 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}

	if( (battlemode == AI_NORMAL)&&(movemode == AI_WAIT) ){
		//ランダムにポイントの方を向こうとする
		//「ポイントの方向を少し重視する」の再現 
		if( random(80) == 0 ){
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

	//回転をランダムに止める
	if( random(turnstop) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( random(turnstop) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}
}

//! 特定の方向を見続ける
bool AIcontrol::StopSeen()
{
	float tr;
	bool returnflag;

	tr = target_rx - rx;
	for(; tr > (float)M_PI; tr -= (float)M_PI*2){}
	for(; tr < (float)M_PI*-1; tr += (float)M_PI*2){}

	//大きな差があれば少しづつ旋回するだけ
	if( tr > AI_TURNRAD ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
		returnflag = false;
	}
	if( tr < AI_TURNRAD*-1 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
		returnflag = false;
	}

	//微々たる差なら一気に向ける。
	if( (tr <= AI_TURNRAD) && (tr >= AI_TURNRAD*-1) ){
		DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
		DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
		rx += tr;
		returnflag = true;
	}

	return returnflag;
}

//! 進行方向に障害物があればジャンプする
bool AIcontrol::MoveJump()
{
	//立ち止まっていれば処理しない
	if( ctrlhuman->GetMovemode(true) == 0 ){ return false; }

	float dist_dummy;

	float new_posx, new_posy, new_posz;

	//腰付近のあたり判定
	new_posx = posx + cos(rx*-1 + (float)M_PI/2) * (AI_CHECKJUMP_DIST + HUMAN_MAPCOLLISION_R);
	new_posy = posy + HUMAN_MAPCOLLISION_HEIGTH;
	new_posz = posz + sin(rx*-1 + (float)M_PI/2) * (AI_CHECKJUMP_DIST + HUMAN_MAPCOLLISION_R);
	if( CollD->CheckALLBlockInside(new_posx, new_posy, new_posz) == true ){
		ObjMgr->MoveJump(ctrlid);
		return true;
	}

	//体全体のあたり判定
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

//! 攻撃
//! @todo ゾンビの相手を捕まえる処理
void AIcontrol::Action()
{
	human* EnemyHuman = NULL;
	int paramid;
	HumanParameter Paraminfo;
	bool zombie;
	float posy2;
	float tx, ty, tz;

	EnemyHuman = enemyhuman;

	//座標を取得
	EnemyHuman->GetPosData(&tx, &ty, &tz, NULL);
	posy2 = posy + VIEW_HEIGHT;
	ty += VIEW_HEIGHT;

	//ゾンビかどうか判定
	ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
	Param->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 2 ){
		zombie = true;
	}
	else{
		zombie = false;
	}

	//所持している武器の種類を取得
	int weaponid = ctrlhuman->GetMainWeaponTypeNO();

	float x = posx - tx;
	float y = posy2 - ty;
	float z = posz - tz;
	float r = x * x + z * z;
	float atan;

	//自分が手榴弾 以外 を持っていれば〜
	if( weaponid != ID_WEAPON_GRENADE ){
		float mx, mz;
		float scale;
		EnemyHuman->GetMovePos(&mx, NULL, &mz);
		if( longattack == false ){ scale = 1.5f; }
		else{ scale = 0.12f; }

		//敵の移動を見超す
		x -= mx * scale;
		z -= mz * scale;
	}

	//目標地点への角度を求める
	atan = atan2(x, z) - rx + (float)M_PI;
	for(; atan > (float)M_PI; atan -= (float)M_PI*2){}
	for(; atan < (float)M_PI*-1; atan += (float)M_PI*2){}

	//大きな差があれば少しづつ旋回するだけ
	if( atan > AI_TURNRAD ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( atan < AI_TURNRAD*-1 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}

	//微々たる差なら一気に向ける
	if( (atan <= AI_TURNRAD) && (atan >= AI_TURNRAD*-1) ){
		DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
		DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
		rx += atan;
		rx += (float)M_PI/180 * (random(5) - 2);
	}

	//腕の角度
	if( zombie == true ){
		//ry = 0.0f;

		//大きな差があれば少しづつ旋回するだけ
		if( ry < AI_TURNRAD*-1 ){
			SetFlag(moveturn_mode, AI_CTRL_TURNUP);
		}
		if( ry > AI_TURNRAD ){
			SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
		}

		//微々たる差なら一気に向ける
		if( (ry <= AI_TURNRAD) && (ry >= AI_TURNRAD*-1) ){
			DelFlag(moveturn_mode, AI_CTRL_TURNUP);
			DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
			ry = 0.0f;
		}
	}
	else{
		float y2 = y;
		float addry;

		//自分が手榴弾を持っていれば〜
		if( weaponid == ID_WEAPON_GRENADE ){
			float scale;
			if( longattack == false ){ scale = 0.12f; }
			else{ scale = 0.4f; }

			//距離に応じて高さを変える
			y2 -= (sqrt(r) - 200.0f) * scale;
		}

		//自分が手ぶらならば〜
		if( weaponid == ID_WEAPON_NONE ){
			if( EnemyHuman->GetMainWeaponTypeNO() == ID_WEAPON_NONE ){	//敵も手ぶらならば〜
				addry = ARMRAD_NOWEAPON - ry;
			}
			else{														//敵が武器を持っていれば〜
				addry = (float)M_PI/18*8 - ry;
			}
		}
		else{
			addry = atan2(y2, sqrt(r)) * -1 - ry;
		}

		//大きな差があれば少しづつ旋回するだけ
		if( addry > AI_TURNRAD ){
			SetFlag(moveturn_mode, AI_CTRL_TURNUP);
		}
		if( addry < AI_TURNRAD*-1 ){
			SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
		}

		//微々たる差なら一気に向ける
		if( (addry <= AI_TURNRAD) && (addry >= AI_TURNRAD*-1) ){
			DelFlag(moveturn_mode, AI_CTRL_TURNUP);
			DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
			ry += addry;
			ry += (float)M_PI/180 * (random(5) - 2);
		}
	}

	//ゾンビ以外で手ぶらならば
	if( zombie == false ){
		if( weaponid == ID_WEAPON_NONE ){
			//一定の確率で後退する
			if( random(80) == 0 ){
				SetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
			}
		}
	}

	
	if( zombie == true ){	//ゾンビの攻撃

		//もし走っていれば、一度歩きに切り替える
		if( GetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD) ){
			DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
		}

		//敵に向かって前進する
		if( (atan <= (float)M_PI/180*25) && (atan >= (float)M_PI/180*25*-1) ){
			if( (atan <= (float)M_PI/180*15) && (atan >= (float)M_PI/180*15*-1) && (r < 24.0f*24.0f) && (actioncnt%50 > 20) ){
				//歩きを取り消し、走る
				SetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
				DelFlag(moveturn_mode, AI_CTRL_MOVEWALK);
			}
			else{
				SetFlag(moveturn_mode, AI_CTRL_MOVEWALK);
			}
		}

		/*
		//ジャンプ
		if( random(128) == 0 ){
			MoveJump();
		}
		*/

		if( (r < 9.0f*9.0f)&&( abs(y) < 10.0f) ){
			//捕まえる　（敵を引き付ける）
			EnemyHuman->AddPosOrder(atan2(z, x), 0.0f, 1.2f);

			//敵の視点をランダムに動かす
			float erx, ery;
			EnemyHuman->GetRxRy(&erx, &ery);
			switch(random(3)){
				case 0: erx -= (float)M_PI/180*2; break;
				case 1: erx += (float)M_PI/180*2; break;
				default: break;
			}
			switch(random(3)){
				case 0: ery -= (float)M_PI/180*2; break;
				case 1: ery += (float)M_PI/180*2; break;
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
	else{					//発砲する
		float ShotAngle;
		if( longattack == false ){
			//敵を捉えたと判定する、許容誤差を計算する
			ShotAngle = (float)M_PI/180*8;
			if( weaponid != ID_WEAPON_NONE ){
				WeaponParameter wparam;
				Param->GetWeapon(weaponid, &wparam);
				if( wparam.scopemode == 1 ){
					ShotAngle = (float)M_PI/180*6;
				}
				if( wparam.scopemode == 2 ){
					ShotAngle = (float)M_PI/180*4;
				}
			}

			//AIレベルごとに調整
			ShotAngle += (float)M_PI/180*0.5f * LevelParam->limitserror;

			if( movemode == AI_RUN2 ){
				ShotAngle *= 1.5f;
			}
		}
		else{
			//敵を捉えたと判定する、許容誤差を計算する
			ShotAngle = (float)M_PI/180*4;
			if( weaponid != ID_WEAPON_NONE ){
				WeaponParameter wparam;
				Param->GetWeapon(weaponid, &wparam);
				if( wparam.scopemode == 1 ){
					ShotAngle = (float)M_PI/180*3;
				}
				if( wparam.scopemode == 2 ){
					ShotAngle = (float)M_PI/180*2;
				}
			}

			//AIレベルごとに調整
			ShotAngle += (float)M_PI/180*0.2f * LevelParam->limitserror;
		}

		//敵を捉えていれば
		float atanxy = atan + atan2(y, r);
		if( atanxy < ShotAngle ){
			int rand = LevelParam->attack;
			if( longattack == true ){ rand += 1; }

			//発砲
			if( random(rand) == 0 ){
				ObjMgr->ShotWeapon(ctrlid);
			}
		}
	}

	//距離に応じて近距離・遠距離を切り替える
	//　200.0fピッタリで設定値維持
	if( r < 200.0f * 200.0f ){
		longattack = false;
	}
	if( (r > 200.0f * 200.0f)&&(movemode != AI_RUN2) ){
		longattack = true;
	}

	if( zombie == false ){
		//ランダムに移動
		MoveRandom();
	}

	actioncnt += 1;
}

//! 攻撃をキャンセル
bool AIcontrol::ActionCancel()
{
	//非戦闘化フラグが有効なら終了
	if( NoFight == true ){
		return true;
	}

	//敵が死亡したら終了
	if( enemyhuman->GetDeadFlag() == true ){
		return true;
	}

	//距離を取得
	float tx, ty, tz;
	enemyhuman->GetPosData(&tx, &ty, &tz, NULL);
	float x = posx - tx;
	float y = posy - ty;
	float z = posz - tz;
	float r = sqrt(x*x + y*y + z*z);

	//距離が離れ過ぎていたら終了
	if( (x*x + y*y + z*z) > 620.0f*620.0f ){
		return true;
	}

	if( longattack == false ){
		//適当なタイミングで敵が見えるか確認
		if( random(40) == 0 ){
			//ブロックが遮っていた（＝見えない）ならば終了
			if( CheckLookEnemy(enemyhuman, AI_SEARCH_RX, AI_SEARCH_RY, 620.0f, NULL) == false ){
				return true;
			}
		}

		//強制的に終了
		if( random(550) == 0 ){
			return true;
		}
	}
	else{
		//適当なタイミングで敵が見えるか確認
		if( random(30) == 0 ){
			//ブロックが遮っていた（＝見えない）ならば終了
			if( CheckLookEnemy(enemyhuman, AI_SEARCH_RX, AI_SEARCH_RY, 620.0f, NULL) == false ){
				return true;
			}
		}

		//強制的に終了
		if( random(450) == 0 ){
			return true;
		}
	}

	return false;
}

//! 武器を持つ
int AIcontrol::HaveWeapon()
{
	int selectweapon;
	class weapon *weapon[TOTAL_HAVEWEAPON];
	int weaponid;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}

	//武器の情報を取得
	ctrlhuman->GetWeapon(&selectweapon, weapon);

	//武器を持っていれば、武器番号を取得
	if( weapon[selectweapon] != NULL ){
		weapon[selectweapon]->GetParamData(&weaponid, NULL, NULL);
	}

	//武器を持っていないか、「ケース」ならば
	if( (weapon[selectweapon] == NULL)||(weaponid == ID_WEAPON_CASE) ){
		//次の武器を指定
		int notselectweapon = selectweapon + 1;
		if( notselectweapon == TOTAL_HAVEWEAPON ){ notselectweapon = 0; }

		//持ち替える
		if( weapon[notselectweapon] != NULL ){
			ObjMgr->ChangeWeapon(ctrlid);
			return 1;
		}
	}

	return 0;
}

//! 移動や方向転換をランダムに終了
void AIcontrol::CancelMoveTurn()
{
	int forward, back, side, updown, rightleft;

	if( battlemode == AI_ACTION ){			//攻撃中
		if( movemode == AI_RUN2 ){				//優先的な走り
			DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
			DelFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
			DelFlag(moveturn_mode, AI_CTRL_MOVELEFT);
			DelFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
			DelFlag(moveturn_mode, AI_CTRL_TURNUP);
			DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
			DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
			DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
			if( random(3) == 0 ){
				DelFlag(moveturn_mode, AI_CTRL_MOVEWALK);
			}
			return;
		}
		else{									//優先的な走り 以外
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
	else if( battlemode == AI_CAUTION ){	//警戒中
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

	//移動をランダムに止める
	if( random(forward) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVEFORWARD);
	}
	if( random(back) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD);
	}
	if( random(side) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVELEFT);
	}
	if( random(side) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVERIGHT);
	}
	if( random(3) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_MOVEWALK);
	}

	//回転をランダムに止める
	if( random(updown) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNUP);
	}
	if( random(updown) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
	}
	if( random(rightleft) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}
	if( random(rightleft) == 0 ){
		DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
}

//! 移動や方向転換を実行
void AIcontrol::ControlMoveTurn()
{
	//移動の実行
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

	//方向転換の実行
	if( GetFlag(moveturn_mode, AI_CTRL_TURNUP) ){
		ry += AI_TURNRAD;
	}
	if( GetFlag(moveturn_mode, AI_CTRL_TURNDOWN) ){
		ry -= AI_TURNRAD;
	}
	if( GetFlag(moveturn_mode, AI_CTRL_TURNLEFT) ){
		rx -= AI_TURNRAD;
	}
	if( GetFlag(moveturn_mode, AI_CTRL_TURNRIGHT) ){
		rx += AI_TURNRAD;
	}
}

//! 武器をリロード・捨てる
//! @return 捨てる：1　リロード：2　持ち替える：3　FULL/SEMI切り替え：4　何もしない：0
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

	//武器の情報を取得
	ctrlhuman->GetWeapon(&selectweapon, weapon);
	if( weapon[selectweapon] == NULL ){ return 0; }
	weapon[selectweapon]->GetParamData(&weaponid, &lnbs, &nbs);

	//武器の性能を取得
	if( Param->GetWeapon(weaponid, &paramdata) == 1 ){ return 0; }

	//スコープ解除
	ctrlhuman->SetDisableScope();

	//「ケース」なら何もしない
	if( weaponid == ID_WEAPON_CASE ){ return 0; }

	//戦闘中にグレネードを持っていれば
	if( battlemode == AI_ACTION ){
		if( weaponid == ID_WEAPON_GRENADE ){
			int nextselectweapon, nextnds;

			//次の武器の弾数を取得
			nextselectweapon = selectweapon + 1;
			nextnds = 0;
			if( nextselectweapon == TOTAL_HAVEWEAPON ){
				nextselectweapon = 0;
			}
			if( weapon[nextselectweapon] != NULL ){
				weapon[nextselectweapon]->GetParamData(NULL, NULL, &nextnds);
			}

			if( longattack == false ){
				// 1/100の確率で持ち替える
				if( (random(100) == 0)&&(nextnds > 0) ){
					ObjMgr->ChangeWeapon(ctrlid);
					return 3;
				}
			}
			else{
				// 1/66の確率で持ち替える
				if( (random(66) == 0)&&(nextnds > 0) ){
					ObjMgr->ChangeWeapon(ctrlid);
					return 3;
				}
			}
		}
	}

	//残弾数が無ければ
	if( lnbs == 0 ){
		int ways;

		//処理確率決定
		if( battlemode == AI_NORMAL ){ ways = 1; }
		else if( battlemode == AI_CAUTION ){ ways = 10; }
		else{ ways = 8; }

		// 1/waysの確率で処理
		if( random(ways) == 0 ){
			int under;

			//リロード確率
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
					// 確率は　3/4
					ways = 4;
					under = 2;
				}
				else{
					// 確率は　2/3
					ways = 3;
					under = 1;
				}
			}

			//弾が無ければ捨てる
			if( nbs == 0 ){
				ObjMgr->DumpWeapon(ctrlid);
				return 1;
			}

			//ランダムに リロード実行 or 武器を持ちかえ
			if( random(ways) <= under ){
				ObjMgr->ReloadWeapon(ctrlid);
				return 2;
			}
			//else{
				ObjMgr->ChangeWeapon(ctrlid);
				return 3;
			//}
		}
	}

	//連射切り替えが可能な武器なら
	if( paramdata.ChangeWeapon != -1 ){
		//現在の武器の連射設定を取得
		blazingmodeS = paramdata.blazingmode;

		//新たな武器の連射設定を取得
		if( Param->GetWeapon(paramdata.ChangeWeapon, &paramdata) == 1 ){ return 0; }
		blazingmodeN = paramdata.blazingmode;

		if( longattack == false ){
			//近距離攻撃中で、現在SEMI・切り替えるとFULLになるなら
			if( (blazingmodeS == false)||(blazingmodeN == true) ){
				ObjMgr->ChangeShotMode(ctrlid);	//切り替える
				return 4;
			}
		}
		else{
			//遠距離攻撃中で、現在FULL・切り替えるとSEMIになるなら
			if( (blazingmodeS == true)||(blazingmodeN == false) ){
				ObjMgr->ChangeShotMode(ctrlid);	//切り替える
				return 4;
			}
		}
	}

	return 0;
}

//! 手榴弾を投げる
//! @return 処理中：0　投げ終わった：1　手榴弾を持っていない：2
int AIcontrol::ThrowGrenade()
{
	int selectweapon;
	class weapon *weapon[TOTAL_HAVEWEAPON];
	int weaponid, nbs, i;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}

	//手榴弾を持っているか確認
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

	//手榴弾を持っていなければ、切り替える
	if( i != selectweapon ){
		ObjMgr->ChangeWeapon(ctrlid, i);
	}

	pointdata pdata;
	float posy2;

	//パスと人の高さを取得
	Points->Getdata(&pdata, target_pointid);
	posy2 = posy + VIEW_HEIGHT;

	float x = posx - target_posx;
	float y = pdata.y - posy2;
	float z = posz - target_posz;
	float r = x * x + z * z;
	float atan_rx, atan_ry;

	//目標地点への角度を求める
	atan_rx = atan2(x, z) - rx + (float)M_PI;
	for(; atan_rx > (float)M_PI; atan_rx -= (float)M_PI*2){}
	for(; atan_rx < (float)M_PI*-1; atan_rx += (float)M_PI*2){}
	atan_ry = atan2(y, sqrt(r)) - ry;

	//大きな差があれば少しづつ旋回するだけ
	if( atan_rx > AI_TURNRAD ){
		SetFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
	}
	if( atan_rx < AI_TURNRAD*-1 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNLEFT);
	}
	if( atan_ry > AI_TURNRAD ){
		SetFlag(moveturn_mode, AI_CTRL_TURNUP);
	}
	if( atan_ry < AI_TURNRAD*-1 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
	}

	//微々たる差なら一気に向ける
	if( (atan_rx <= AI_TURNRAD) && (atan_rx >= AI_TURNRAD*-1) ){
		DelFlag(moveturn_mode, AI_CTRL_TURNRIGHT);
		DelFlag(moveturn_mode, AI_CTRL_TURNLEFT);
		rx += atan_rx;
	}
	if( (atan_ry <= AI_TURNRAD) && (atan_ry >= AI_TURNRAD*-1) ){
		DelFlag(moveturn_mode, AI_CTRL_TURNUP);
		DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
		ry += atan_ry;
	}

	//投げる
	if( (abs(atan_rx) < (float)M_PI/1800*15)&&(abs(atan_ry) < (float)M_PI/1800*15) ){
		//角度を設定
		ctrlhuman->SetRxRy(rx, ry);

		if( ObjMgr->ShotWeapon(ctrlid) != 0 ){
			return 1;
		}
	}

	return 0;
}

//! 腕の角度を設定
void AIcontrol::ArmAngle()
{
	float addry;

	if( ctrlhuman->GetMainWeaponTypeNO() == ID_WEAPON_NONE ){	//手ぶら
		addry = ARMRAD_NOWEAPON - ry;
	}
	else if( (battlemode == AI_CAUTION)&&(cautioncnt > 0) ){	//警戒中
		addry = 0.0f - ry;
	}
	else{									//平常時で武器所有中
		addry = AI_WEAPON_ARMRAD - ry;
	}

	//大きな差があれば少しづつ旋回するだけ
	if( addry > AI_TURNRAD ){
		SetFlag(moveturn_mode, AI_CTRL_TURNUP);
	}
	if( addry < AI_TURNRAD*-1 ){
		SetFlag(moveturn_mode, AI_CTRL_TURNDOWN);
	}

	//微々たる差なら一気に向ける
	if( (addry <= AI_TURNRAD) && (addry >= AI_TURNRAD*-1) ){
		DelFlag(moveturn_mode, AI_CTRL_TURNUP);
		DelFlag(moveturn_mode, AI_CTRL_TURNDOWN);
		ry += addry;
	}
}

//! 敵を探す
int AIcontrol::SearchEnemy()
{
	//非戦闘化フラグが有効なら敵を見つけない
	if( NoFight == true ){
		return 0;
	}

	if( battlemode == AI_ACTION ){ return 0; }

	int weaponid = ctrlhuman->GetMainWeaponTypeNO();
	int weaponscope = 0;
	int searchloops;
	float maxDist;
	float A_rx, A_ry, B_rx, B_ry;

	//武器を持っていれば、スコープモードを取得
	if( weaponid != ID_WEAPON_NONE ){
		WeaponParameter Wparam;

		Param->GetWeapon(weaponid, &Wparam);
		weaponscope = Wparam.scopemode;
	}

	//敵の探索回数と探索範囲（距離と角度）を設定
	if( battlemode == AI_NORMAL ){
		searchloops = (LevelParam->search) * 4;

		if( weaponscope == 2 ){ maxDist = 50.0f; }
		if( weaponscope == 1 ){ maxDist = 25.0f; }
		else{ maxDist = 0.0f; }
		maxDist += 12.0f*((LevelParam->search)-2) + 350.0f;
		A_rx = (float)M_PI/180*60;
		A_ry = (float)M_PI/180*110;
		B_rx = (float)M_PI/180*40;
		B_ry = (float)M_PI/180*60;
	}
	else {	//battlemode == AI_CAUTION
		searchloops = (LevelParam->search) * 5;

		if( weaponscope == 2 ){ maxDist = 80.0f; }
		if( weaponscope == 1 ){ maxDist = 40.0f; }
		else{ maxDist = 0.0f; }
		maxDist += 15.0f*((LevelParam->search)-2) + 420.0f;
		A_rx = (float)M_PI/180*80;
		A_ry = (float)M_PI/180*130;
		B_rx = (float)M_PI/180*50;
		B_ry = (float)M_PI/180*80;
	}

	//指定回数、敵を探索
	for(int i=0; i<searchloops; i++){
		int targetid = random(MAX_HUMAN);

		if( CheckLookEnemy(targetid, A_rx, A_ry, 200.0f, NULL) == true ){
			longattack = false;
			return 1;
		}

		if( CheckLookEnemy(targetid, B_rx, B_ry, maxDist, NULL) == true ){
			if( random(4) == 0 ){
				if( movemode == AI_RUN2 ){ longattack = false; }
				else{ longattack = true; }
				return 2;
			}
		}
	}
	return 0;
}

//! 敵を探す（遠距離攻撃中に近距離を探す）
int AIcontrol::SearchShortEnemy()
{
	float A_rx, A_ry;
	A_rx = (float)M_PI/180*100;
	A_ry = (float)M_PI/180*52;

	for(int i=0; i<3; i++){
		int targetid = random(MAX_HUMAN);

		if( CheckLookEnemy(targetid, A_rx, A_ry, 200.0f, NULL) == true ){
			longattack = false;
			return 1;
		}
	}
	return 0;
}

//! 敵が見えるか判定
bool AIcontrol::CheckLookEnemy(int id, float search_rx, float search_ry, float maxDist, float *out_minDist)
{
	if( ObjMgr == NULL ){ return false; }

	//人のオブジェクトを取得
	class human* thuman;
	thuman = ObjMgr->GeHumanObject(id);
	if( thuman == NULL ){ return false; }

	//同名関数をオーバーロード
	return CheckLookEnemy(thuman, search_rx, search_ry, maxDist, out_minDist);
}

//! 敵が見えるか判定
bool AIcontrol::CheckLookEnemy(class human* thuman, float search_rx, float search_ry, float maxDist, float *out_minDist)
{
	//return false;

	//無効なデータなら終了
	if( ctrlhuman == NULL ){ return false; }
	if( thuman == NULL ){ return false; }
	if( thuman->GetDrawFlag() == false ){ return false; }
	if( thuman->GetDeadFlag() == true ){ return false; }

	//自分と敵の座標を取得
	int ctrlteam, targetteam;
	float tx, ty, tz;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &ctrlteam);
	thuman->GetParamData(NULL, NULL, NULL, &targetteam);
	thuman->GetPosData(&tx, &ty, &tz, NULL);

	//自分と敵が同一人物でなければ
	if( ctrlteam != targetteam ){
		float Dist;
		float Dist_dummy; 
		float x, y, z;

		//距離を取得
		x = tx - posx;
		y = ty - posy;
		z = tz - posz;
		Dist = sqrt(x*x + y*y + z*z);

		//判定を行う距離より近ければ
		if( Dist < maxDist ){
			float rx2, trx, mrx;

			//人の向き・視点を求める　（0.0〜2π）
			rx2 = rx*-1 + (float)M_PI/2;
			for(; rx2 > (float)M_PI*2; rx2 -= (float)M_PI*2){}
			for(; rx2 < 0.0f; rx2 += (float)M_PI*2){}

			//ポイントまでの角度を求める　（0.0〜2π）
			trx = atan2(z, x);
			if( trx < 0.0f ){ trx += (float)M_PI*2; }

			//視点を基準にポイントまでの角度を算出（-π〜π）
			mrx = trx - rx2;
			for(; mrx > (float)M_PI; mrx -= (float)M_PI*2){}
			for(; mrx < (float)M_PI*-1; mrx += (float)M_PI*2){}

			//角度上、視界に入っていれば
			if( (abs(mrx) < search_rx/2)&&(abs(atan2(y, sqrt(x*x + z*z))) < search_ry/2) ){
				//ブロックが遮っていなければ　（レイで当たり判定を行い、当たっていなければ）
				if( CollD->CheckALLBlockIntersectRay(posx, posy + VIEW_HEIGHT, posz, x/Dist, y/Dist, z/Dist, NULL, NULL, &Dist_dummy, Dist) == false ){
					if( out_minDist != NULL ){ *out_minDist = Dist; }
					enemyhuman = thuman;
					return true;
				}
			}
		}
	}
	return false;
}

//! 死体があるか確認
bool AIcontrol::CheckCorpse(int id)
{
	//クラス設定がおかしければ処理しない
	if( ObjMgr == NULL ){ return false; }
	if( ctrlhuman == NULL ){ return false; }

	//ターゲットのクラスを取得
	class human* thuman;
	thuman = ObjMgr->GeHumanObject(id);
	if( thuman == NULL ){ return false; }
	if( thuman->GetDrawFlag() == false ){ return false; }

	//死亡していれば
	if( thuman->GetDeadFlag() == true ){

		//チーム番号を取得
		int ctrlteam, targetteam;
		ctrlhuman->GetParamData(NULL, NULL, NULL, &ctrlteam);
		thuman->GetParamData(NULL, NULL, NULL, &targetteam);

		//味方ならば
		if( ctrlteam == targetteam ){
			float tposx, tposy, tposz;
			float x, y, z, r;
			float atan;

			//座標から距離を取得
			thuman->GetPosData(&tposx, &tposy, &tposz, NULL);
			x = posx - tposx;
			y = posy - tposy;
			z = posz - tposz;
			r = x*x + y*y + z*z;

			if( r < 22.0f * 22.0f ){
				//死体への角度を求める
				atan = atan2(x, z) - rx + (float)M_PI;
				for(; atan > (float)M_PI; atan -= (float)M_PI*2){}
				for(; atan < (float)M_PI*-1; atan += (float)M_PI*2){}

				if( abs(atan) < (float)M_PI/18*4 ){
					return true;
				}
			}
		}
	}
	return false;
}

//! パスによる移動
void AIcontrol::MovePath()
{
	if( movemode == AI_NULL ){			//異常なパス
		//
	}
	else if( movemode == AI_GRENADE ){	//手榴弾パス
		if( ThrowGrenade() != 0 ){
			SearchTarget(true);
		}
	}
	else{								//その他パス
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

//! 攻撃メイン処理
//! @return 不変：false　変更：true
bool AIcontrol::ActionMain()
{
	int newbattlemode = AI_ACTION;

	//攻撃処理
	Action();

	if( movemode == AI_RUN2 ){				//優先的な走り
		//目標地点へ移動
		if( CheckTargetPos() == true ){
			newbattlemode = AI_NORMAL;
			SearchTarget(true);
		}
		else{
			MoveTarget2();
		}
	}
	else{									//優先的な走り 以外
		//遠距離攻撃中なら、近くの敵を探す
		if( longattack == true ){
			SearchShortEnemy();
		}
	}

	//攻撃終了判定
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

	//設定を判定
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! 警戒メイン処理
//! @return 不変：false　変更：true
bool AIcontrol::CautionMain()
{
	int newbattlemode = AI_CAUTION;

	//座標とチーム番号を取得
	int teamid;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &teamid);

	//被弾と音の状況を取得
	bool HitFlag = ctrlhuman->CheckHit();
	soundlist soundlist[MAX_SOUNDMGR_LIST];
	int soundlists = GameSound->GetWorldSound(posx, posy + VIEW_HEIGHT, posz, teamid, soundlist);

	//非戦闘化フラグが有効なら、音は聞こえないことにする
	if( NoFight == true ){
		soundlists = 0;
	}

	//回転と腕の角度
	TurnSeen();
	ArmAngle();

	//メイン処理
	if( enemyhuman != NULL ){		//既に敵を見つけていれば
		newbattlemode = AI_ACTION;
		actioncnt = 0;
	}
	else if( SearchEnemy() != 0 ){		//敵が見つかれば
		newbattlemode = AI_ACTION;
		actioncnt = 0;
	}
	else if( (HitFlag == true)||(soundlists > 0) ){	//被弾したか音が聞こえた
		cautioncnt = 160;			//警戒を再開
	}
	else if( cautioncnt == 0 ){		//警戒を終了するなら
		if( CheckTargetPos() == false ){				//警戒開始地点より離れているか
			MoveTarget();				//警戒開始地点に近づく
		}
		else{
			newbattlemode = AI_NORMAL;

			//警戒待ちパスなら次へ進める
			pointdata pdata;
			Points->Getdata(&pdata, target_pointid);
			if( (pdata.p1 == 3)&&(pdata.p2 == 4) ){
				SearchTarget(true);
			}
		}
	}
	else if( cautioncnt < 100 ){	//100フレームを切ったら、ランダムに警戒終了（カウント：0に）
		if( random(50) == 0 ){ cautioncnt = 0; }
	}
	else{ cautioncnt -= 1; }

	//追尾中で対象から離れすぎたら、ランダムに警戒終了
	if( (movemode == AI_TRACKING)&&(random(3) == 0) ){
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

	//設定を判定
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! 通常メイン処理
//! @return 不変：false　変更：true
bool AIcontrol::NormalMain()
{
	int newbattlemode = AI_NORMAL;

	if( hold == false ){
		SearchTarget(false);
	}
	enemyhuman = NULL;

	//座標とチーム番号を取得
	int teamid;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &teamid);

	//被弾と音の状況を取得
	bool HitFlag = ctrlhuman->CheckHit();
	soundlist soundlist[MAX_SOUNDMGR_LIST];
	int soundlists = GameSound->GetWorldSound(posx, posy + VIEW_HEIGHT, posz, teamid, soundlist);

	//非戦闘化フラグが有効なら、音は聞こえないことにする
	if( NoFight == true ){
		soundlists = 0;
	}

	//ランダムパスなら処理実行
	if( movemode == AI_RANDOM ){
		SearchTarget(true);
	}

	//腕の角度を設定
	if( movemode != AI_GRENADE ){
		ArmAngle();
	}

	if( movemode == AI_RUN2 ){		//優先的な走りの処理
		//敵を見つけたら攻撃に入る
		if( SearchEnemy() != 0 ){
			newbattlemode = AI_ACTION;
		}
		else{
			MovePath();		//移動実行
		}
	}
	else{							//優先的な走り以外の処理
		//警戒判定に入る処理
		if(
			(SearchEnemy() != 0)||							//敵を見つけた
			(HitFlag == true)||(soundlists > 0)||	//被弾したか音が聞こえた
			(CheckCorpse( random(MAX_HUMAN) ) == true)	//死体を見つけた
		){
			newbattlemode = AI_CAUTION;
			cautioncnt = 160;
			target_posx = posx;
			target_posz = posz;
		}
		else{
			MovePath();		//移動実行
		}
	}

	//設定を判定
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! 初期化系関数
void AIcontrol::Init()
{
	//クラス設定がおかしければ処理しない
	if( ctrlhuman == NULL ){ return; }
	if( blocks == NULL ){ return; }
	if( Points == NULL ){ return; }
	if( CollD == NULL ){ return; }

	//使用されていない人なら処理しない
	if( ctrlhuman->GetDrawFlag() == false ){ return; }

	//ステートを初期化
	hold = false;
	NoFight = false;
	battlemode = AI_NORMAL;
	movemode = AI_NULL;
	enemyhuman = NULL;
	waitcnt = 0;
	gotocnt = 0;
	moveturn_mode = 0x00;
	cautioncnt = 0;
	actioncnt = 0;
	longattack = false;

	//AIレベルと設定値を取得
	int paramid;
	HumanParameter paramdata;
	//target_pointid = in_target_pointid;
	ctrlhuman->GetParamData(&paramid, &target_pointid, NULL, NULL);
	Param->GetHuman(paramid, &paramdata);
	AIlevel = paramdata.AIlevel;
	Param->GetAIlevel(AIlevel, &LevelParam);

	//次のポイントを検索
	SearchTarget(true);
}

//! 指定した場所へ待機させる
//! @param px X座標
//! @param pz Z座標
//! @param rx 重視する向き
//! @attention 移動パスに関わらず、指定した座標への待機を強制します。Init()関数を再度実行するまで元に戻せません。
void AIcontrol::SetHoldWait(float px, float pz, float rx)
{
	movemode = AI_WAIT;
	hold = true;
	target_posx = px;
	target_posz = pz;
	target_rx = rx;
}

//! 指定した人を追尾させる
//! @param id 人のデータ番号
//! @attention 移動パスに関わらず、指定した人への追尾を強制します。Init()関数を再度実行するまで元に戻せません。
void AIcontrol::SetHoldTracking(int id)
{
	movemode = AI_TRACKING;
	hold = false;
	target_pointid = id;
}

//! 強制的に警戒させる
//! @warning 優先的な走り を実行中の場合、この関数は何もしません。
void AIcontrol::SetCautionMode()
{
	//優先的な走りならば何もしない
	if( movemode == AI_RUN2 ){ return; }

	if( battlemode == AI_NORMAL ){
		target_posx = posx;
		target_posz = posz;
	}
	battlemode = AI_CAUTION;
	cautioncnt = 160;
}

//! 非戦闘化フラグを設定
//! @param flag true：戦闘を行わない（非戦闘化）　false：戦闘を行う（通常）
//! @attention フラグを有効にすると敵を認識しなくなります。
void AIcontrol::SetNoFightFlag(bool flag)
{
	NoFight = flag;
}

//! 処理系関数
void AIcontrol::Process()
{
	//クラス設定がおかしければ処理しない
	if( ctrlhuman == NULL ){ return; }
	if( blocks == NULL ){ return; }
	if( Points == NULL ){ return; }
	if( CollD == NULL ){ return; }

	//無効な人クラスなら処理しない
	if( ctrlhuman->GetDrawFlag() == false ){ return; }

	//死亡したら
	if( ctrlhuman->GetHP() <= 0 ){
		battlemode = AI_DEAD;
		movemode = AI_DEAD;
		return;
	}

	//座標と角度を取得
	ctrlhuman->GetPosData(&posx, &posy, &posz, NULL);
	ctrlhuman->GetRxRy(&rx, &ry);

	//ランダムに動作を止める
	CancelMoveTurn();

	//攻撃中か警戒中ならば
	if( (battlemode == AI_ACTION)||(battlemode == AI_CAUTION) ){
		//武器を持つ
		HaveWeapon();
	}

	//主計算実行
	if( battlemode == AI_ACTION ){			//攻撃中
		ActionMain();
	}
	else if( battlemode == AI_CAUTION ){	//警戒中
		CautionMain();
	}
	else{									//平常時
		NormalMain();
	}

	//移動・方向転換処理
	ControlMoveTurn();

	//武器を取り扱い
	ControlWeapon();

	//角度を適用
	ctrlhuman->SetRxRy(rx, ry);
}