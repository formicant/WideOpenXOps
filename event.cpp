//! @file event.cpp
//! @brief EventControlクラスの定義

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

#include "event.h"

//! @brief コンストラクタ
EventControl::EventControl(PointDataInterface *in_Point, ObjectManager *in_ObjMgr)
{
	Point = in_Point;
	ObjMgr = in_ObjMgr;
	nextp4 = 0;
	waitcnt = 0;
}

//! @brief ディストラクタ
EventControl::~EventControl()
{}

//! @brief 対象クラスを設定
//! @param in_Point ポイントデータ管理クラス
//! @param in_ObjMgr オブジェクト管理クラス
//! @attention この関数で設定を行わないと、クラス自体が正しく機能しません。
void EventControl::SetClass(PointDataInterface *in_Point, ObjectManager *in_ObjMgr)
{
	Point = in_Point;
	ObjMgr = in_ObjMgr;
}

//! @brief リセット
//! @param EntryP4 イベント処理を開始する認識番号　（-100、-110、-120　など）
void EventControl::Reset(signed char EntryP4)
{
	nextp4 = EntryP4;
	waitcnt = 0;
}

//! @brief 次に処理する認識番号
//! @return P4：認識番号
signed char EventControl::GetNextP4()
{
	return nextp4;
}

//! @brief 対象の人物がケースを持っているかチェック
//! @param in_human 調べる対象のhumanのポインタ
//! @return 持っている：true　持っていない：false
bool EventControl::CheckHaveCase(human *in_human)
{
	int selectweapon;
	weapon *out_weapon[TOTAL_HAVEWEAPON];
	int weaponid;

	//所有する武器オブジェクトを全て取得
	in_human->GetWeapon(&selectweapon, out_weapon);

	//全て調べる
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		if( out_weapon[i] != NULL ){

			//武器の種類番号を取得し、それがケースかどうか。
			out_weapon[i]->GetParamData(&weaponid, NULL, NULL);
			if( weaponid == ID_WEAPON_CASE ){
				return true;
			}
		}
	}
	return false;
}

//! @brief 処理を実行
//! @param endcnt 終了カウントのポインタ
//! @param complete ミッション成功フラグのポインタ
//! @param MessageID イベントメッセージ番号のポインタ
//! @param SetMessageID イベントメッセージ読み出しフラグ
//! @return 実行したステップ数
//! @attention 渡された引数に変更がない場合、渡された引数のデータは操作されません。
//! @attention SetMessageID は、メッセージイベントが実行された場合に true になります。<b>前回から変更されたとは限りません。</b>
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

		//次のポイントを探す
		if( Point->SearchPointdata(&data, 0x08, 0, 0, 0, nextp4, 0) == 0 ){ return cnt; }
		if( (data.p1 < 10)||(19 < data.p1) ){ return cnt; }

		switch(data.p1){
			case 10:	//任務達成
				*endcnt += 1;
				*complete = true;
				return cnt;

			case 11:	//任務失敗
				*endcnt += 1;
				*complete = false;
				return cnt;

			case 12:	//死亡待ち
				thuman = ObjMgr->SearchHuman(data.p2);
				if( thuman == NULL ){ return cnt; }
				if( thuman->GetDeadFlag() == false ){ return cnt; }
				nextp4 = data.p3;
				break;

			case 13:	//到着待ち
				thuman = ObjMgr->SearchHuman(data.p2);
				if( thuman == NULL ){ return cnt; }
				thuman->GetPosData(&hx, &hy, &hz, NULL);
				x = hx - data.x;
				y = hy - data.y;
				z = hz - data.z;
				if( sqrt(x*x + y*y+ + z*z) > DISTANCE_CHECKPOINT ){ return cnt; }
				nextp4 = data.p3;
				break;

			case 14:	//歩きに変更
				if( Point->SearchPointdata(&pid, 0x08, 0, 0, 0, data.p2, 0) > 0 ){
					//対象がAIパスならば、強制的にパラメータを書き換える
					Point->Getdata(&pathdata, pid);
					if( (pathdata.p1 == 3)||(pathdata.p1 == 8) ){
						pathdata.p2 = 0;
					}
					Point->SetParam(pid, pathdata.p1, pathdata.p2, pathdata.p3, pathdata.p4);
				}
				nextp4 = data.p3;
				break;

			case 15:	//小物破壊待ち
				tsmallobject = ObjMgr->SearchSmallobject(data.p2);
				if( tsmallobject == NULL ){ return cnt; }
				if( tsmallobject->GetDrawFlag() == true ){ return cnt; }
				nextp4 = data.p3;
				break;

			case 16:	//ケース待ち
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

			case 17:	//時間待ち
				if( (int)GAMEFPS * ((unsigned char)data.p2) > waitcnt ){
					waitcnt += 1;
					return cnt;
				}
				waitcnt = 0;
				nextp4 = data.p3;
				break;

			case 18:	//メッセージ
				if( (0 <= data.p2)&&(data.p2 < MAX_POINTMESSAGES) ){
					*MessageID = data.p2;
				}
				nextp4 = data.p3;
				//break;
				*SetMessageID = true;
				return cnt;

			case 19:	//チーム変更
				thuman = ObjMgr->SearchHuman(data.p2);
				if( thuman == NULL ){ return cnt; }
				thuman->SetTeamID(0);
				nextp4 = data.p3;
				break;
		}
	}
	return cnt;
}