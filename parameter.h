//! @file parameter.h
//! @brief ParameterInfoクラスの宣言

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

// 人・武器・小物 設定データの確認作業
// 　Mandaさん　（2014年12月）

#ifndef PARAMETER_H
#define PARAMETER_H

#define TOTAL_PARAMETERINFO_HUMAN 43			//!< 人の設定数
#define TOTAL_HUMANTEXTURE 30					//!< 人のテクスチャの合計枚数
#define TOTAL_PARAMETERINFO_WEAPON 23			//!< 武器の設定数
#define TOTAL_PARAMETERINFO_SMALLOBJECT 12	//!< 小物の設定数
#define TOTAL_OFFICIALMISSION 58			//!< 標準ミッションの数
#define TOTAL_PARAMETERINFO_AILEVEL 6		//!< AIレベルの設定数

#define ID_WEAPON_NONE 0		//!< 武器：なし として取り扱う武器番号
#define ID_WEAPON_GRENADE 13	//!< 手榴弾として取り扱う武器番号
#define ID_WEAPON_CASE 15		//!< ケースとして取り扱う武器番号

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

//! 人設定用の構造体
struct HumanParameter{
	int texture;	//!< テクスチャ番号
	int model;		//!< モデル番号
	int hp;			//!< 体力
	int AIlevel;	//!< AIレベル
	int Weapon[TOTAL_HAVEWEAPON];	//!< 武器
	int type;		//!< 種類
};

//! 武器設定用の構造体
struct WeaponParameter{
	char *name;		//!< 武器名
	char *model;	//!< モデルファイル名
	char *texture;	//!< テクスチャファイル名
	int attacks;	//!< 攻撃力
	int penetration;	//!< 貫通力
	int blazings;	//!< 連射間隔
	int speed;		//!< 弾速
	int nbsmax;		//!< 装弾数
	int reloads;	//!< リロード時間
	int reaction;		//!< 反動角度
	int ErrorRangeMIN;	//!< 弾道誤差 最小
	int ErrorRangeMAX;	//!< 弾道誤差 最大
	float mx;		//!< 銃の位置：X
	float my;		//!< 銃の位置：Y
	float mz;		//!< 銃の位置：Z
	float flashx;	//!< マズルフラッシュの位置：X
	float flashy;	//!< マズルフラッシュの位置：Y
	float flashz;	//!< マズルフラッシュの位置：Z
	float yakkyou_px;	//!< 薬莢排出 位置：X
	float yakkyou_py;	//!< 薬莢排出 位置：Y
	float yakkyou_pz;	//!< 薬莢排出 位置：Z
	float yakkyou_sx;	//!< 薬莢排出 初速：X
	float yakkyou_sy;	//!< 薬莢排出 初速：Y
	bool blazingmode;	//!< 連射
	int scopemode;	//!< スコープモード
	float size;		//!< 表示倍率
	int soundid;	//!< 発砲音の番号
	int soundvolume;	//!< 発砲音の音量
	bool silencer;	//!< サイレンサーの有無
	int WeaponP;	//!< 装備方法
	int ChangeWeapon;	//!< 切り替える武器対象
	int burst;			//!< 発射弾数
};

//! 小物設定用の構造体
struct SmallObjectParameter{
	char *model;	//!< モデル名
	char *texture;	//!< テクスチャ名
	int decide;		//!< 当たり判定の大きさ
	int hp;			//!< 耐久力
	int sound;		//!< 命中時の効果音番号
	int jump;		//!< 飛び具合
};

//! AIレベルによる設定（性能値）用構造体
struct AIParameter{
	int aiming;			//!< エイミング能力
	int attack;			//!< 攻撃性
	int search;			//!< 発見能力
	int limitserror;	//!< 許容誤差
};

//! @brief 設定された値を管理するクラス
//! @details 予め固定された設定値を管理します。
//! @details 具体的には、人・武器・小物から、標準ミッションに関係する設定値などです。
class ParameterInfo
{
	HumanParameter *Human;						//!< 人の情報を格納するポインタ
	char *HumanTexturePath[TOTAL_HUMANTEXTURE];		//!< 人のテクスチャを格納するポインタ
	WeaponParameter *Weapon;						//!< 武器の情報を格納するポインタ
	SmallObjectParameter *SmallObject;			//!< 小物の情報を格納するポインタ
	char *missionname[TOTAL_OFFICIALMISSION];		//!< 標準ミッションのミッション識別名
	char *missionfullname[TOTAL_OFFICIALMISSION];	//!< 標準ミッションのミッション正式名称
	char *missiondirectory[TOTAL_OFFICIALMISSION];	//!< 標準ミッションのデータディレクトリ
	char *missiontxt[TOTAL_OFFICIALMISSION];		//!< 標準ミッションのファイル名（.pd1/.txt）
	AIParameter *AIlevel;						//!< AIの性能値

public:
	ParameterInfo();
	~ParameterInfo();
	void InitInfo();
	void DestroyInfo();
	int GetHuman(int id, HumanParameter *out_data);
	int GetHumanTexturePath(int id, char *out_str);
	int GetWeapon(int id, WeaponParameter *out_data);
	int GetSmallObject(int id, SmallObjectParameter *out_data);
	int GetOfficialMission(int id, char *name, char *fullname, char* directory, char *txt);
	int GetAIlevel(int level, AIParameter **out_AIlevel);
};

#endif