//! @file parameter.cpp
//! @brief ParameterInfoクラスの定義

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

#include "parameter.h"

//! @brief コンストラクタ
ParameterInfo::ParameterInfo()
{
	Human = NULL;
	for(int i=0; i<TOTAL_HUMANTEXTURE; i++){
		HumanTexturePath[i] = '\0';
	}
	Weapon = NULL;
	SmallObject = NULL;
	Bullet = NULL;
	for(int i=0; i<TOTAL_OFFICIALMISSION; i++){
		missionname[i] = '\0';
		missionfullname[i] = '\0';
		missiondirectory[i] = '\0';
		missiontxt[i] = '\0';
	}
	AIlevel = NULL;
}

//! @brief ディストラクタ
ParameterInfo::~ParameterInfo()
{
	DestroyInfo();
}

//! @brief 初期化（パラメータの設定）
//! @attention この関数を呼び出さないと、クラス自体が正しく機能しません。
void ParameterInfo::InitInfo()
{
#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_INIT, "設定値", "");
#endif

	Human = new HumanParameter[TOTAL_PARAMETERINFO_HUMAN];
	Weapon = new WeaponParameter[TOTAL_PARAMETERINFO_WEAPON];
	SmallObject = new SmallObjectParameter[TOTAL_PARAMETERINFO_SMALLOBJECT];
	Bullet = new BulletParameter[TOTAL_PARAMETERINFO_BULLET];
	AIlevel = new AIParameter[TOTAL_PARAMETERINFO_AILEVEL];

	//特殊 黒 A
	Human[0].texture = 14;
	Human[0].model = 5;
	Human[0].hp = 120;
	Human[0].AIlevel = 3;
	Human[0].Weapon[0] = 21;
	Human[0].Weapon[1] = 8;
	Human[0].type = 0;
	//特殊 黒 B
	Human[1].texture = 14;
	Human[1].model = 5;
	Human[1].hp = 120;
	Human[1].AIlevel = 3;
	Human[1].Weapon[0] = 21;
	Human[1].Weapon[1] = 2;
	Human[1].type = 0;
	//特殊 緑 A
	Human[2].texture = 16;
	Human[2].model = 0;
	Human[2].hp = 110;
	Human[2].AIlevel = 2;
	Human[2].Weapon[0] = 3;
	Human[2].Weapon[1] = 6;
	Human[2].type = 0;
	//特殊 緑 B
	Human[3].texture = 16;
	Human[3].model = 0;
	Human[3].hp = 110;
	Human[3].AIlevel = 2;
	Human[3].Weapon[0] = 3;
	Human[3].Weapon[1] = 10;
	Human[3].type = 0;
	//特殊 緑 C
	Human[4].texture = 16;
	Human[4].model = 0;
	Human[4].hp = 110;
	Human[4].AIlevel = 2;
	Human[4].Weapon[0] = 3;
	Human[4].Weapon[1] = 2;
	Human[4].type = 0;
	//特殊 白 A
	Human[5].texture = 18;
	Human[5].model = 0;
	Human[5].hp = 110;
	Human[5].AIlevel = 2;
	Human[5].Weapon[0] = 4;
	Human[5].Weapon[1] = 7;
	Human[5].type = 0;
	//特殊 白 B
	Human[6].texture = 18;
	Human[6].model = 0;
	Human[6].hp = 110;
	Human[6].AIlevel = 2;
	Human[6].Weapon[0] = 4;
	Human[6].Weapon[1] = 2;
	Human[6].type = 0;
	//ハゲ
	Human[7].texture = 5;
	Human[7].model = 0;
	Human[7].hp = 100;
	Human[7].AIlevel = 3;
	Human[7].Weapon[0] = 4;
	Human[7].Weapon[1] = 0;
	Human[7].type = 0;
	//特殊 紫
	Human[8].texture = 17;
	Human[8].model = 0;
	Human[8].hp = 110;
	Human[8].AIlevel = 3;
	Human[8].Weapon[0] = 22;
	Human[8].Weapon[1] = 14;
	Human[8].type = 0;
	//特殊 青
	Human[9].texture = 15;
	Human[9].model = 5;
	Human[9].hp = 120;
	Human[9].AIlevel = 2;
	Human[9].Weapon[0] = 3;
	Human[9].Weapon[1] = 1;
	Human[9].type = 0;
	//戦闘用ロボット
	Human[10].texture = 13;
	Human[10].model = 0;
	Human[10].hp = 2500;
	Human[10].AIlevel = 4;
	Human[10].Weapon[0] = 6;
	Human[10].Weapon[1] = 12;
	Human[10].type = 1;
	//スーツ 黒 SG
	Human[11].texture = 4;
	Human[11].model = 1;
	Human[11].hp = 100;
	Human[11].AIlevel = 1;
	Human[11].Weapon[0] = 18;
	Human[11].Weapon[1] = 0;
	Human[11].type = 0;
	//スーツ 黒
	Human[12].texture = 4;
	Human[12].model = 0;
	Human[12].hp = 100;
	Human[12].AIlevel = 1;
	Human[12].Weapon[0] = 5;
	Human[12].Weapon[1] = 0;
	Human[12].type = 0;
	//スーツ 灰
	Human[13].texture = 11;
	Human[13].model = 0;
	Human[13].hp = 100;
	Human[13].AIlevel = 0;
	Human[13].Weapon[0] = 0;
	Human[13].Weapon[1] = 0;
	Human[13].type = 0;
	//警察官
	Human[14].texture = 8;
	Human[14].model = 2;
	Human[14].hp = 100;
	Human[14].AIlevel = 1;
	Human[14].Weapon[0] = 3;
	Human[14].Weapon[1] = 0;
	Human[14].type = 0;
	//スーツ 茶
	Human[15].texture = 9;
	Human[15].model = 0;
	Human[15].hp = 100;
	Human[15].AIlevel = 0;
	Human[15].Weapon[0] = 0;
	Human[15].Weapon[1] = 0;
	Human[15].type = 0;
	//シャツ男 1
	Human[16].texture = 23;
	Human[16].model = 0;
	Human[16].hp = 100;
	Human[16].AIlevel = 0;
	Human[16].Weapon[0] = 17;
	Human[16].Weapon[1] = 0;
	Human[16].type = 0;
	//中東兵
	Human[17].texture = 6;
	Human[17].model = 0;
	Human[17].hp = 100;
	Human[17].AIlevel = 1;
	Human[17].Weapon[0] = 17;
	Human[17].Weapon[1] = 10;
	Human[17].type = 0;
	//女
	Human[18].texture = 25;
	Human[18].model = 3;
	Human[18].hp = 90;
	Human[18].AIlevel = 0;
	Human[18].Weapon[0] = 0;
	Human[18].Weapon[1] = 0;
	Human[18].type = 0;
	//金髪男
	Human[19].texture = 0;
	Human[19].model = 0;
	Human[19].hp = 100;
	Human[19].AIlevel = 0;
	Human[19].Weapon[0] = 0;
	Human[19].Weapon[1] = 0;
	Human[19].type = 0;
	//市民 1
	Human[20].texture = 1;
	Human[20].model = 0;
	Human[20].hp = 100;
	Human[20].AIlevel = 0;
	Human[20].Weapon[0] = 0;
	Human[20].Weapon[1] = 0;
	Human[20].type = 0;
	//市民 2
	Human[21].texture = 2;
	Human[21].model = 0;
	Human[21].hp = 100;
	Human[21].AIlevel = 0;
	Human[21].Weapon[0] = 0;
	Human[21].Weapon[1] = 0;
	Human[21].type = 0;
	//シャツ男 1 SG
	Human[22].texture = 23;
	Human[22].model = 1;
	Human[22].hp = 100;
	Human[22].AIlevel = 0;
	Human[22].Weapon[0] = 0;
	Human[22].Weapon[1] = 0;
	Human[22].type = 0;
	//金髪男 SG
	Human[23].texture = 0;
	Human[23].model = 1;
	Human[23].hp = 100;
	Human[23].AIlevel = 0;
	Human[23].Weapon[0] = 0;
	Human[23].Weapon[1] = 0;
	Human[23].type = 0;
	//市民 1 SG
	Human[24].texture = 1;
	Human[24].model = 1;
	Human[24].hp = 100;
	Human[24].AIlevel = 0;
	Human[24].Weapon[0] = 0;
	Human[24].Weapon[1] = 0;
	Human[24].type = 0;
	//市民 2 SG
	Human[25].texture = 2;
	Human[25].model = 1;
	Human[25].hp = 100;
	Human[25].AIlevel = 0;
	Human[25].Weapon[0] = 0;
	Human[25].Weapon[1] = 0;
	Human[25].type = 0;
	//兵士 1 A
	Human[26].texture = 19;
	Human[26].model = 4;
	Human[26].hp = 115;
	Human[26].AIlevel = 2;
	Human[26].Weapon[0] = 5;
	Human[26].Weapon[1] = 9;
	Human[26].type = 0;
	//兵士 1 B
	Human[27].texture = 20;
	Human[27].model = 4;
	Human[27].hp = 115;
	Human[27].AIlevel = 2;
	Human[27].Weapon[0] = 5;
	Human[27].Weapon[1] = 12;
	Human[27].type = 0;
	//兵士 2
	Human[28].texture = 21;
	Human[28].model = 4;
	Human[28].hp = 115;
	Human[28].AIlevel = 2;
	Human[28].Weapon[0] = 13;
	Human[28].Weapon[1] = 10;
	Human[28].type = 0;
	//ゾンビ 1
	Human[29].texture = 26;
	Human[29].model = 0;
	Human[29].hp = 1000;
	Human[29].AIlevel = 0;
	Human[29].Weapon[0] = 0;
	Human[29].Weapon[1] = 0;
	Human[29].type = 2;
	//ゾンビ 2
	Human[30].texture = 27;
	Human[30].model = 0;
	Human[30].hp = 1000;
	Human[30].AIlevel = 0;
	Human[30].Weapon[0] = 0;
	Human[30].Weapon[1] = 0;
	Human[30].type = 2;
	//ゾンビ 3
	Human[31].texture = 28;
	Human[31].model = 0;
	Human[31].hp = 1000;
	Human[31].AIlevel = 0;
	Human[31].Weapon[0] = 0;
	Human[31].Weapon[1] = 0;
	Human[31].type = 2;
	//ゾンビ 4
	Human[32].texture = 29;
	Human[32].model = 3;
	Human[32].hp = 950;
	Human[32].AIlevel = 0;
	Human[32].Weapon[0] = 0;
	Human[32].Weapon[1] = 0;
	Human[32].type = 2;
	//スーツ 紺
	Human[33].texture = 12;
	Human[33].model = 0;
	Human[33].hp = 100;
	Human[33].AIlevel = 0;
	Human[33].Weapon[0] = 0;
	Human[33].Weapon[1] = 0;
	Human[33].type = 0;
	//スーツ 紺 SG
	Human[34].texture = 12;
	Human[34].model = 1;
	Human[34].hp = 100;
	Human[34].AIlevel = 1;
	Human[34].Weapon[0] = 17;
	Human[34].Weapon[1] = 0;
	Human[34].type = 0;
	//将軍
	Human[35].texture = 7;
	Human[35].model = 0;
	Human[35].hp = 95;
	Human[35].AIlevel = 1;
	Human[35].Weapon[0] = 17;
	Human[35].Weapon[1] = 0;
	Human[35].type = 0;
	//スーツ 青
	Human[36].texture = 10;
	Human[36].model = 0;
	Human[36].hp = 100;
	Human[36].AIlevel = 0;
	Human[36].Weapon[0] = 0;
	Human[36].Weapon[1] = 0;
	Human[36].type = 0;
	//スーツ 青 SG
	Human[37].texture = 10;
	Human[37].model = 1;
	Human[37].hp = 100;
	Human[37].AIlevel = 1;
	Human[37].Weapon[0] = 18;
	Human[37].Weapon[1] = 0;
	Human[37].type = 0;
	//シャツ男 2 SG
	Human[38].texture = 24;
	Human[38].model = 1;
	Human[38].hp = 100;
	Human[38].AIlevel = 1;
	Human[38].Weapon[0] = 6;
	Human[38].Weapon[1] = 0;
	Human[38].type = 0;
	//兵士 3
	Human[39].texture = 22;
	Human[39].model = 0;
	Human[39].hp = 105;
	Human[39].AIlevel = 3;
	Human[39].Weapon[0] = 5;
	Human[39].Weapon[1] = 20;
	Human[39].type = 0;
	//兵士 3 SG
	Human[40].texture = 22;
	Human[40].model = 1;
	Human[40].hp = 105;
	Human[40].AIlevel = 3;
	Human[40].Weapon[0] = 21;
	Human[40].Weapon[1] = 0;
	Human[40].type = 0;
	//ゲイツ
	Human[41].texture = 3;
	Human[41].model = 0;
	Human[41].hp = 100;
	Human[41].AIlevel = 0;
	Human[41].Weapon[0] = 0;
	Human[41].Weapon[1] = 0;
	Human[41].type = 0;
	//ゲイツ SG
	Human[42].texture = 3;
	Human[42].model = 1;
	Human[42].hp = 100;
	Human[42].AIlevel = 4;
	Human[42].Weapon[0] = 22;
	Human[42].Weapon[1] = 0;
	Human[42].type = 0;

	HumanTexturePath[0] = "./data/model/civ1.bmp";
	HumanTexturePath[1] = "./data/model/civ2.bmp";
	HumanTexturePath[2] = "./data/model/civ3.bmp";
	HumanTexturePath[3] = "./data/model/gates.bmp";
	HumanTexturePath[4] = "./data/model/gs.bmp";
	HumanTexturePath[5] = "./data/model/hage.bmp";
	HumanTexturePath[6] = "./data/model/islam.bmp";
	HumanTexturePath[7] = "./data/model/islam2.bmp";
	HumanTexturePath[8] = "./data/model/police.bmp";
	HumanTexturePath[9] = "./data/model/riiman.bmp";
	HumanTexturePath[10] = "./data/model/riiman_b.bmp";
	HumanTexturePath[11] = "./data/model/riiman_g.bmp";
	HumanTexturePath[12] = "./data/model/riiman_k.bmp";
	HumanTexturePath[13] = "./data/model/robot.bmp";
	HumanTexturePath[14] = "./data/model/soldier_black.bmp";
	HumanTexturePath[15] = "./data/model/soldier_blue.bmp";
	HumanTexturePath[16] = "./data/model/soldier_green.bmp";
	HumanTexturePath[17] = "./data/model/soldier_violet.bmp";
	HumanTexturePath[18] = "./data/model/soldier_white.bmp";
	HumanTexturePath[19] = "./data/model/soldier0.bmp";
	HumanTexturePath[20] = "./data/model/soldier1.bmp";
	HumanTexturePath[21] = "./data/model/soldier2.bmp";
	HumanTexturePath[22] = "./data/model/soldier3.bmp";
	HumanTexturePath[23] = "./data/model/syatu.bmp";
	HumanTexturePath[24] = "./data/model/syatu2.bmp";
	HumanTexturePath[25] = "./data/model/woman.bmp";
	HumanTexturePath[26] = "./data/model/zombie1.bmp";
	HumanTexturePath[27] = "./data/model/zombie2.bmp";
	HumanTexturePath[28] = "./data/model/zombie3.bmp";
	HumanTexturePath[29] = "./data/model/zombie4.bmp";


	Weapon[0].name = "NONE";		// ID_WEAPON_NONE 定数を要設定
	Weapon[0].model = "";
	Weapon[0].texture= "";
	Weapon[0].attacks = 0;
	Weapon[0].penetration = 0;
	Weapon[0].blazings = 0;
	Weapon[0].speed = 0;
	Weapon[0].nbsmax = 0;
	Weapon[0].reloads = 0;
	Weapon[0].reaction = 0;
	Weapon[0].ErrorRangeMIN = 0;
	Weapon[0].ErrorRangeMAX = 0;
	Weapon[0].mx = 0.0f;
	Weapon[0].my = 0.0f;
	Weapon[0].mz = 0.0f;
	Weapon[0].flashx = 0.0f;
	Weapon[0].flashy = 0.0f;
	Weapon[0].flashz = 0.0f;
	Weapon[0].yakkyou_px = 0.0f;
	Weapon[0].yakkyou_py = 0.0f;
	Weapon[0].yakkyou_pz = 0.0f;
	Weapon[0].yakkyou_sx = 0.0f;
	Weapon[0].yakkyou_sy = 0.0f;
	Weapon[0].blazingmode = true;
	Weapon[0].scopemode = 0;
	Weapon[0].size = 9.0f;
	Weapon[0].soundid = 0;
	Weapon[0].soundvolume = 0;
	Weapon[0].silencer = false;
	Weapon[0].WeaponP = 2;
	Weapon[0].ChangeWeapon = -1;
	Weapon[0].burst = 0;
	Weapon[1].name = "MP5";
	Weapon[1].model = "./data/model/weapon/mp5.x";
	Weapon[1].texture= "./data/model/weapon/mp5.bmp";
	Weapon[1].attacks = 44;
	Weapon[1].penetration = 0;
	Weapon[1].blazings = 4;
	Weapon[1].speed = 10;
	Weapon[1].nbsmax = 30;
	Weapon[1].reloads = 50;
	Weapon[1].reaction = 6;
	Weapon[1].ErrorRangeMIN = 2;
	Weapon[1].ErrorRangeMAX = 21;
	Weapon[1].mx = 8.0f;
	Weapon[1].my = 9.0f;
	Weapon[1].mz = 35.0f;
	Weapon[1].flashx = 8.0f;
	Weapon[1].flashy = 15.0f;
	Weapon[1].flashz = 68.0f;
	Weapon[1].yakkyou_px = 8.0f;
	Weapon[1].yakkyou_py = 12.0f;
	Weapon[1].yakkyou_pz = 35.0f;
	Weapon[1].yakkyou_sx = 5.0f;
	Weapon[1].yakkyou_sy = 7.0f;
	Weapon[1].blazingmode = true;
	Weapon[1].scopemode = 0;
	Weapon[1].size = 7.5f;
	Weapon[1].soundid = 0;
	Weapon[1].soundvolume = 92;
	Weapon[1].silencer = false;
	Weapon[1].WeaponP = 0;
	Weapon[1].ChangeWeapon = -1;
	Weapon[1].burst = 1;
	Weapon[2].name = "PSG1";
	Weapon[2].model = "./data/model/weapon/psg1.x";
	Weapon[2].texture= "./data/model/weapon/psg1.bmp";
	Weapon[2].attacks = 70;
	Weapon[2].penetration = 2;
	Weapon[2].blazings = 7;
	Weapon[2].speed = 14;
	Weapon[2].nbsmax = 5;
	Weapon[2].reloads = 58;
	Weapon[2].reaction = 15;
	Weapon[2].ErrorRangeMIN = 0;
	Weapon[2].ErrorRangeMAX = 19;
	Weapon[2].mx = 8.0f;
	Weapon[2].my = 8.0f;
	Weapon[2].mz = 44.0f;
	Weapon[2].flashx = 8.0f;
	Weapon[2].flashy = 13.0f;
	Weapon[2].flashz = 108.0f;
	Weapon[2].yakkyou_px = 8.0f;
	Weapon[2].yakkyou_py = 13.0f;
	Weapon[2].yakkyou_pz = 38.0f;
	Weapon[2].yakkyou_sx = 6.0f;
	Weapon[2].yakkyou_sy = 6.0f;
	Weapon[2].blazingmode = false;
	Weapon[2].scopemode = 2;
	Weapon[2].size = 8.0f;
	Weapon[2].soundid = 1;
	Weapon[2].soundvolume = 98;
	Weapon[2].silencer = false;
	Weapon[2].WeaponP = 0;
	Weapon[2].ChangeWeapon = -1;
	Weapon[2].burst = 1;
	Weapon[3].name = "M92F";
	Weapon[3].model = "./data/model/weapon/m92f.x";
	Weapon[3].texture= "./data/model/weapon/m92f.bmp";
	Weapon[3].attacks = 41;
	Weapon[3].penetration = 0;
	Weapon[3].blazings = 5;
	Weapon[3].speed = 9;
	Weapon[3].nbsmax = 15;
	Weapon[3].reloads = 40;
	Weapon[3].reaction = 10;
	Weapon[3].ErrorRangeMIN = 4;
	Weapon[3].ErrorRangeMAX = 26;
	Weapon[3].mx = 2.0f;
	Weapon[3].my = 7.0f;
	Weapon[3].mz = 57.0f;
	Weapon[3].flashx = 2.0f;
	Weapon[3].flashy = 15.0f;
	Weapon[3].flashz = 73.0f;
	Weapon[3].yakkyou_px = 2.0f;
	Weapon[3].yakkyou_py = 15.0f;
	Weapon[3].yakkyou_pz = 56.0f;
	Weapon[3].yakkyou_sx = 4.0f;
	Weapon[3].yakkyou_sy = 4.0f;
	Weapon[3].blazingmode = false;
	Weapon[3].scopemode = 0;
	Weapon[3].size = 7.0f;
	Weapon[3].soundid = 1;
	Weapon[3].soundvolume = 90;
	Weapon[3].silencer = false;
	Weapon[3].WeaponP = 1;
	Weapon[3].ChangeWeapon = -1;
	Weapon[3].burst = 1;
	Weapon[4].name = "GLOCK18 SEMI";
	Weapon[4].model = "./data/model/weapon/glock18.x";
	Weapon[4].texture= "./data/model/weapon/glock18.bmp";
	Weapon[4].attacks = 38;
	Weapon[4].penetration = 0;
	Weapon[4].blazings = 5;
	Weapon[4].speed = 9;
	Weapon[4].nbsmax = 19;
	Weapon[4].reloads = 38;
	Weapon[4].reaction = 9;
	Weapon[4].ErrorRangeMIN = 4;
	Weapon[4].ErrorRangeMAX = 26;
	Weapon[4].mx = 2.0f;
	Weapon[4].my = 8.0f;
	Weapon[4].mz = 56.0f;
	Weapon[4].flashx = 2.0f;
	Weapon[4].flashy = 13.0f;
	Weapon[4].flashz = 70.0f;
	Weapon[4].yakkyou_px = 2.0f;
	Weapon[4].yakkyou_py = 14.0f;
	Weapon[4].yakkyou_pz = 54.0f;
	Weapon[4].yakkyou_sx = 4.0f;
	Weapon[4].yakkyou_sy = 4.0f;
	Weapon[4].blazingmode = false;
	Weapon[4].scopemode = 0;
	Weapon[4].size = 5.0f;
	Weapon[4].soundid = 0;
	Weapon[4].soundvolume = 90;
	Weapon[4].silencer = false;
	Weapon[4].WeaponP = 1;
	Weapon[4].ChangeWeapon = 16;
	Weapon[4].burst = 1;
	Weapon[5].name = "DESERT EAGLE";
	Weapon[5].model = "./data/model/weapon/de.x";
	Weapon[5].texture= "./data/model/weapon/de.bmp";
	Weapon[5].attacks = 68;
	Weapon[5].penetration = 1;
	Weapon[5].blazings = 5;
	Weapon[5].speed = 10;
	Weapon[5].nbsmax = 7;
	Weapon[5].reloads = 41;
	Weapon[5].reaction = 13;
	Weapon[5].ErrorRangeMIN = 4;
	Weapon[5].ErrorRangeMAX = 26;
	Weapon[5].mx = 2.0f;
	Weapon[5].my = 11.0f;
	Weapon[5].mz = 59.0f;
	Weapon[5].flashx = 2.0f;
	Weapon[5].flashy = 14.0f;
	Weapon[5].flashz = 75.0f;
	Weapon[5].yakkyou_px = 2.0f;
	Weapon[5].yakkyou_py = 15.0f;
	Weapon[5].yakkyou_pz = 56.0f;
	Weapon[5].yakkyou_sx = 5.0f;
	Weapon[5].yakkyou_sy = 4.0f;
	Weapon[5].blazingmode = false;
	Weapon[5].scopemode = 0;
	Weapon[5].size = 11.0f;
	Weapon[5].soundid = 2;
	Weapon[5].soundvolume = 96;
	Weapon[5].silencer = false;
	Weapon[5].WeaponP = 1;
	Weapon[5].ChangeWeapon = -1;
	Weapon[5].burst = 1;
	Weapon[6].name = "MAC10";
	Weapon[6].model = "./data/model/weapon/mac10.x";
	Weapon[6].texture= "./data/model/weapon/mac10.bmp";
	Weapon[6].attacks = 47;
	Weapon[6].penetration = 0;
	Weapon[6].blazings = 3;
	Weapon[6].speed = 10;
	Weapon[6].nbsmax = 30;
	Weapon[6].reloads = 45;
	Weapon[6].reaction = 9;
	Weapon[6].ErrorRangeMIN = 4;
	Weapon[6].ErrorRangeMAX = 25;
	Weapon[6].mx = 2.0f;
	Weapon[6].my = 4.0f;
	Weapon[6].mz = 54.0f;
	Weapon[6].flashx = 2.0f;
	Weapon[6].flashy = 15.0f;
	Weapon[6].flashz = 73.0f;
	Weapon[6].yakkyou_px = 2.0f;
	Weapon[6].yakkyou_py = 16.0f;
	Weapon[6].yakkyou_pz = 52.0f;
	Weapon[6].yakkyou_sx = 2.0f;
	Weapon[6].yakkyou_sy = 5.0f;
	Weapon[6].blazingmode = true;
	Weapon[6].scopemode = 0;
	Weapon[6].size = 5.5f;
	Weapon[6].soundid = 3;
	Weapon[6].soundvolume = 90;
	Weapon[6].silencer = false;
	Weapon[6].WeaponP = 1;
	Weapon[6].ChangeWeapon = -1;
	Weapon[6].burst = 1;
	Weapon[7].name = "UMP";
	Weapon[7].model = "./data/model/weapon/ump.x";
	Weapon[7].texture= "./data/model/weapon/ump.bmp";
	Weapon[7].attacks = 50;
	Weapon[7].penetration = 0;
	Weapon[7].blazings = 5;
	Weapon[7].speed = 10;
	Weapon[7].nbsmax = 25;
	Weapon[7].reloads = 50;
	Weapon[7].reaction = 7;
	Weapon[7].ErrorRangeMIN = 2;
	Weapon[7].ErrorRangeMAX = 19;
	Weapon[7].mx = 8.0f;
	Weapon[7].my = 6.0f;
	Weapon[7].mz = 28.0f;
	Weapon[7].flashx = 8.0f;
	Weapon[7].flashy = 13.0f;
	Weapon[7].flashz = 64.0f;
	Weapon[7].yakkyou_px = 8.0f;
	Weapon[7].yakkyou_py = 11.0f;
	Weapon[7].yakkyou_pz = 41.0f;
	Weapon[7].yakkyou_sx = 4.0f;
	Weapon[7].yakkyou_sy = 4.0f;
	Weapon[7].blazingmode = true;
	Weapon[7].scopemode = 0;
	Weapon[7].size = 6.0f;
	Weapon[7].soundid = 4;
	Weapon[7].soundvolume = 94;
	Weapon[7].silencer = false;
	Weapon[7].WeaponP = 0;
	Weapon[7].ChangeWeapon = -1;
	Weapon[7].burst = 1;
	Weapon[8].name = "P90";
	Weapon[8].model = "./data/model/weapon/p90.x";
	Weapon[8].texture= "./data/model/weapon/p90.bmp";
	Weapon[8].attacks = 42;
	Weapon[8].penetration = 1;
	Weapon[8].blazings = 4;
	Weapon[8].speed = 11;
	Weapon[8].nbsmax = 50;
	Weapon[8].reloads = 68;
	Weapon[8].reaction = 6;
	Weapon[8].ErrorRangeMIN = 3;
	Weapon[8].ErrorRangeMAX = 18;
	Weapon[8].mx = 8.0f;
	Weapon[8].my = 10.0f;
	Weapon[8].mz = 30.0f;
	Weapon[8].flashx = 8.0f;
	Weapon[8].flashy = 12.0f;
	Weapon[8].flashz = 59.0f;
	Weapon[8].yakkyou_px = 8.0f;
	Weapon[8].yakkyou_py = 4.0f;
	Weapon[8].yakkyou_pz = 18.0f;
	Weapon[8].yakkyou_sx = -1.0f;
	Weapon[8].yakkyou_sy = 0.0f;
	Weapon[8].blazingmode = true;
	Weapon[8].scopemode = 0;
	Weapon[8].size = 6.5f;
	Weapon[8].soundid = 3;
	Weapon[8].soundvolume = 92;
	Weapon[8].silencer = false;
	Weapon[8].WeaponP = 0;
	Weapon[8].ChangeWeapon = -1;
	Weapon[8].burst = 1;
	Weapon[9].name = "M4";
	Weapon[9].model = "./data/model/weapon/m4.x";
	Weapon[9].texture= "./data/model/weapon/m4.bmp";
	Weapon[9].attacks = 61;
	Weapon[9].penetration = 1;
	Weapon[9].blazings = 4;
	Weapon[9].speed = 11;
	Weapon[9].nbsmax = 30;
	Weapon[9].reloads = 54;
	Weapon[9].reaction = 8;
	Weapon[9].ErrorRangeMIN = 3;
	Weapon[9].ErrorRangeMAX = 19;
	Weapon[9].mx = 9.0f;
	Weapon[9].my = 6.0f;
	Weapon[9].mz = 37.0f;
	Weapon[9].flashx = 9.0f;
	Weapon[9].flashy = 13.0f;
	Weapon[9].flashz = 91.0f;
	Weapon[9].yakkyou_px = 10.0f;
	Weapon[9].yakkyou_py = 12.0f;
	Weapon[9].yakkyou_pz = 40.0f;
	Weapon[9].yakkyou_sx = 4.0f;
	Weapon[9].yakkyou_sy = 6.0f;
	Weapon[9].blazingmode = true;
	Weapon[9].scopemode = 0;
	Weapon[9].size = 6.8f;
	Weapon[9].soundid = 3;
	Weapon[9].soundvolume = 96;
	Weapon[9].silencer = false;
	Weapon[9].WeaponP = 0;
	Weapon[9].ChangeWeapon = -1;
	Weapon[9].burst = 1;
	Weapon[10].name = "AK47";
	Weapon[10].model = "./data/model/weapon/ak47.x";
	Weapon[10].texture= "./data/model/weapon/ak47.bmp";
	Weapon[10].attacks = 65;
	Weapon[10].penetration = 1;
	Weapon[10].blazings = 4;
	Weapon[10].speed = 11;
	Weapon[10].nbsmax = 30;
	Weapon[10].reloads = 52;
	Weapon[10].reaction = 9;
	Weapon[10].ErrorRangeMIN = 3;
	Weapon[10].ErrorRangeMAX = 20;
	Weapon[10].mx = 9.0f;
	Weapon[10].my = 11.0f;
	Weapon[10].mz = 39.0f;
	Weapon[10].flashx = 9.0f;
	Weapon[10].flashy = 9.0f;
	Weapon[10].flashz = 86.0f;
	Weapon[10].yakkyou_px = 9.0f;
	Weapon[10].yakkyou_py = 12.0f;
	Weapon[10].yakkyou_pz = 41.0f;
	Weapon[10].yakkyou_sx = 5.0f;
	Weapon[10].yakkyou_sy = 5.0f;
	Weapon[10].blazingmode = true;
	Weapon[10].scopemode = 0;
	Weapon[10].size = 6.0f;
	Weapon[10].soundid = 1;
	Weapon[10].soundvolume = 96;
	Weapon[10].silencer = false;
	Weapon[10].WeaponP = 0;
	Weapon[10].ChangeWeapon = -1;
	Weapon[10].burst = 1;
	Weapon[11].name = "AUG";
	Weapon[11].model = "./data/model/weapon/aug.x";
	Weapon[11].texture= "./data/model/weapon/aug.bmp";
	Weapon[11].attacks = 60;
	Weapon[11].penetration = 1;
	Weapon[11].blazings = 5;
	Weapon[11].speed = 11;
	Weapon[11].nbsmax = 30;
	Weapon[11].reloads = 56;
	Weapon[11].reaction = 7;
	Weapon[11].ErrorRangeMIN = 2;
	Weapon[11].ErrorRangeMAX = 18;
	Weapon[11].mx = 8.0f;
	Weapon[11].my = 10.0f;
	Weapon[11].mz = 37.0f;
	Weapon[11].flashx = 8.0f;
	Weapon[11].flashy = 12.0f;
	Weapon[11].flashz = 77.0f;
	Weapon[11].yakkyou_px = 8.0f;
	Weapon[11].yakkyou_py = 11.0f;
	Weapon[11].yakkyou_pz = 15.0f;
	Weapon[11].yakkyou_sx = 3.0f;
	Weapon[11].yakkyou_sy = 5.0f;
	Weapon[11].blazingmode = true;
	Weapon[11].scopemode = 1;
	Weapon[11].size = 7.8f;
	Weapon[11].soundid = 4;
	Weapon[11].soundvolume = 98;
	Weapon[11].silencer = false;
	Weapon[11].WeaponP = 0;
	Weapon[11].ChangeWeapon = -1;
	Weapon[11].burst = 1;
	Weapon[12].name = "M249";
	Weapon[12].model = "./data/model/weapon/m249.x";
	Weapon[12].texture= "./data/model/weapon/m249.bmp";
	Weapon[12].attacks = 59;
	Weapon[12].penetration = 1;
	Weapon[12].blazings = 4;
	Weapon[12].speed = 11;
	Weapon[12].nbsmax = 100;
	Weapon[12].reloads = 85;
	Weapon[12].reaction = 7;
	Weapon[12].ErrorRangeMIN = 4;
	Weapon[12].ErrorRangeMAX = 22;
	Weapon[12].mx = 8.0f;
	Weapon[12].my = 12.0f;
	Weapon[12].mz = 44.0f;
	Weapon[12].flashx = 9.0f;
	Weapon[12].flashy = 13.0f;
	Weapon[12].flashz = 102.0f;
	Weapon[12].yakkyou_px = 9.0f;
	Weapon[12].yakkyou_py = 11.0f;
	Weapon[12].yakkyou_pz = 45.0f;
	Weapon[12].yakkyou_sx = 5.0f;
	Weapon[12].yakkyou_sy = 4.0f;
	Weapon[12].blazingmode = true;
	Weapon[12].scopemode = 0;
	Weapon[12].size = 7.5f;
	Weapon[12].soundid = 3;
	Weapon[12].soundvolume = 98;
	Weapon[12].silencer = false;
	Weapon[12].WeaponP = 0;
	Weapon[12].ChangeWeapon = -1;
	Weapon[12].burst = 1;
	Weapon[13].name = "GRENADE";		// ID_WEAPON_GRENADE 定数を要設定
	Weapon[13].model = "./data/model/weapon/grenade.x";
	Weapon[13].texture= "./data/model/weapon/grenade.bmp";
	Weapon[13].attacks = 0;
	Weapon[13].penetration = 0;
	Weapon[13].blazings = 35;
	Weapon[13].speed = 0;
	Weapon[13].nbsmax = 1;
	Weapon[13].reloads = 0;
	Weapon[13].reaction = 0;
	Weapon[13].ErrorRangeMIN = 6;
	Weapon[13].ErrorRangeMAX = 20;
	Weapon[13].mx = 1.0f;
	Weapon[13].my = 6.0f;
	Weapon[13].mz = 57.0f;
	Weapon[13].flashx = 1.0f;
	Weapon[13].flashy = 6.0f;
	Weapon[13].flashz = 57.0f;
	Weapon[13].yakkyou_px = 0.0f;
	Weapon[13].yakkyou_py = 0.0f;
	Weapon[13].yakkyou_pz = 0.0f;
	Weapon[13].yakkyou_sx = 0.0f;
	Weapon[13].yakkyou_sy = 0.0f;
	Weapon[13].blazingmode = false;
	Weapon[13].scopemode = 0;
	Weapon[13].size = 8.0f;
	Weapon[13].soundid = 0;
	Weapon[13].soundvolume = 0;
	Weapon[13].silencer = false;
	Weapon[13].WeaponP = 1;
	Weapon[13].ChangeWeapon = -1;
	Weapon[13].burst = 1;
	Weapon[14].name = "MP5SD";
	Weapon[14].model = "./data/model/weapon/mp5sd.x";
	Weapon[14].texture= "./data/model/weapon/mp5sd.bmp";
	Weapon[14].attacks = 37;
	Weapon[14].penetration = 0;
	Weapon[14].blazings = 4;
	Weapon[14].speed = 9;
	Weapon[14].nbsmax = 30;
	Weapon[14].reloads = 50;
	Weapon[14].reaction = 6;
	Weapon[14].ErrorRangeMIN = 2;
	Weapon[14].ErrorRangeMAX = 21;
	Weapon[14].mx = 7.0f;
	Weapon[14].my = 8.0f;
	Weapon[14].mz = 35.0f;
	Weapon[14].flashx = 8.0f;
	Weapon[14].flashy = 12.0f;
	Weapon[14].flashz = 84.0f;
	Weapon[14].yakkyou_px = 8.0f;
	Weapon[14].yakkyou_py = 12.0f;
	Weapon[14].yakkyou_pz = 35.0f;
	Weapon[14].yakkyou_sx = 5.0f;
	Weapon[14].yakkyou_sy = 6.0f;
	Weapon[14].blazingmode = true;
	Weapon[14].scopemode = 0;
	Weapon[14].size = 7.5f;
	Weapon[14].soundid = 13;
	Weapon[14].soundvolume = 84;
	Weapon[14].silencer = true;
	Weapon[14].WeaponP = 0;
	Weapon[14].ChangeWeapon = -1;
	Weapon[14].burst = 1;
	Weapon[15].name = "CASE";		// ID_WEAPON_CASE 定数を要設定
	Weapon[15].model = "./data/model/weapon/case.x";
	Weapon[15].texture= "./data/model/weapon/case.bmp";
	Weapon[15].attacks = 0;
	Weapon[15].penetration = 0;
	Weapon[15].blazings = 0;
	Weapon[15].speed = 0;
	Weapon[15].nbsmax = 0;
	Weapon[15].reloads = 0;
	Weapon[15].reaction = 0;
	Weapon[15].ErrorRangeMIN = 0;
	Weapon[15].ErrorRangeMAX = 0;
	Weapon[15].mx = 15.0f;
	Weapon[15].my = -2.0f;
	Weapon[15].mz = 35.0f;
	Weapon[15].flashx = 0.0f;
	Weapon[15].flashy = 0.0f;
	Weapon[15].flashz = 0.0f;
	Weapon[15].yakkyou_px = 0.0f;
	Weapon[15].yakkyou_py = 0.0f;
	Weapon[15].yakkyou_pz = 0.0f;
	Weapon[15].yakkyou_sx = 0.0f;
	Weapon[15].yakkyou_sy = 0.0f;
	Weapon[15].blazingmode = true;
	Weapon[15].scopemode = 0;
	Weapon[15].size = 5.0f;
	Weapon[15].soundid = 0;
	Weapon[15].soundvolume = 0;
	Weapon[15].silencer = false;
	Weapon[15].WeaponP = 2;
	Weapon[15].ChangeWeapon = -1;
	Weapon[15].burst = 1;
	Weapon[16].name = "GLOCK18 FULL";
	Weapon[16].model = "./data/model/weapon/glock18.x";
	Weapon[16].texture= "./data/model/weapon/glock18.bmp";
	Weapon[16].attacks = 38;
	Weapon[16].penetration = 0;
	Weapon[16].blazings = 3;
	Weapon[16].speed = 9;
	Weapon[16].nbsmax = 19;
	Weapon[16].reloads = 38;
	Weapon[16].reaction = 9;
	Weapon[16].ErrorRangeMIN = 4;
	Weapon[16].ErrorRangeMAX = 26;
	Weapon[16].mx = 2.0f;
	Weapon[16].my = 8.0f;
	Weapon[16].mz = 56.0f;
	Weapon[16].flashx = 2.0f;
	Weapon[16].flashy = 13.0f;
	Weapon[16].flashz = 70.0f;
	Weapon[16].yakkyou_px = 2.0f;
	Weapon[16].yakkyou_py = 14.0f;
	Weapon[16].yakkyou_pz = 54.0f;
	Weapon[16].yakkyou_sx = 4.0f;
	Weapon[16].yakkyou_sy = 4.0f;
	Weapon[16].blazingmode = true;
	Weapon[16].scopemode = 0;
	Weapon[16].size = 5.0f;
	Weapon[16].soundid = 0;
	Weapon[16].soundvolume = 90;
	Weapon[16].silencer = false;
	Weapon[16].WeaponP = 1;
	Weapon[16].ChangeWeapon = 4;
	Weapon[16].burst = 1;
	Weapon[17].name = "M1911";
	Weapon[17].model = "./data/model/weapon/cg.x";
	Weapon[17].texture= "./data/model/weapon/cg.bmp";
	Weapon[17].attacks = 46;
	Weapon[17].penetration = 0;
	Weapon[17].blazings = 5;
	Weapon[17].speed = 9;
	Weapon[17].nbsmax = 7;
	Weapon[17].reloads = 35;
	Weapon[17].reaction = 11;
	Weapon[17].ErrorRangeMIN = 5;
	Weapon[17].ErrorRangeMAX = 26;
	Weapon[17].mx = 2.0f;
	Weapon[17].my = 7.0f;
	Weapon[17].mz = 61.0f;
	Weapon[17].flashx = 2.0f;
	Weapon[17].flashy = 14.0f;
	Weapon[17].flashz = 74.0f;
	Weapon[17].yakkyou_px = 2.0f;
	Weapon[17].yakkyou_py = 16.0f;
	Weapon[17].yakkyou_pz = 57.0f;
	Weapon[17].yakkyou_sx = 4.0f;
	Weapon[17].yakkyou_sy = 3.0f;
	Weapon[17].blazingmode = false;
	Weapon[17].scopemode = 0;
	Weapon[17].size = 6.8f;
	Weapon[17].soundid = 3;
	Weapon[17].soundvolume = 94;
	Weapon[17].silencer = false;
	Weapon[17].WeaponP = 1;
	Weapon[17].ChangeWeapon = -1;
	Weapon[17].burst = 1;
	Weapon[18].name = "GLOCK17";
	Weapon[18].model = "./data/model/weapon/glock18.x";
	Weapon[18].texture= "./data/model/weapon/glock17.bmp";
	Weapon[18].attacks = 38;
	Weapon[18].penetration = 0;
	Weapon[18].blazings = 5;
	Weapon[18].speed = 9;
	Weapon[18].nbsmax = 17;
	Weapon[18].reloads = 37;
	Weapon[18].reaction = 9;
	Weapon[18].ErrorRangeMIN = 4;
	Weapon[18].ErrorRangeMAX = 26;
	Weapon[18].mx = 2.0f;
	Weapon[18].my = 8.0f;
	Weapon[18].mz = 56.0f;
	Weapon[18].flashx = 2.0f;
	Weapon[18].flashy = 13.0f;
	Weapon[18].flashz = 70.0f;
	Weapon[18].yakkyou_px = 2.0f;
	Weapon[18].yakkyou_py = 14.0f;
	Weapon[18].yakkyou_pz = 54.0f;
	Weapon[18].yakkyou_sx = 4.0f;
	Weapon[18].yakkyou_sy = 4.0f;
	Weapon[18].blazingmode = false;
	Weapon[18].scopemode = 0;
	Weapon[18].size = 5.0f;
	Weapon[18].soundid = 0;
	Weapon[18].soundvolume = 90;
	Weapon[18].silencer = false;
	Weapon[18].WeaponP = 1;
	Weapon[18].ChangeWeapon = -1;
	Weapon[18].burst = 1;
	Weapon[19].name = "M1";
	Weapon[19].model = "./data/model/weapon/m1.x";
	Weapon[19].texture= "./data/model/weapon/m1.bmp";
	Weapon[19].attacks = 90;
	Weapon[19].penetration = 0;
	Weapon[19].blazings = 8;
	Weapon[19].speed = 9;
	Weapon[19].nbsmax = 7;
	Weapon[19].reloads = 100;
	Weapon[19].reaction = 18;
	Weapon[19].ErrorRangeMIN = 5;
	Weapon[19].ErrorRangeMAX = 23;
	Weapon[19].mx = 8.0f;
	Weapon[19].my = 7.0f;
	Weapon[19].mz = 45.0f;
	Weapon[19].flashx = 8.0f;
	Weapon[19].flashy = 11.0f;
	Weapon[19].flashz = 94.0f;
	Weapon[19].yakkyou_px = 8.0f;
	Weapon[19].yakkyou_py = 11.0f;
	Weapon[19].yakkyou_pz = 42.0f;
	Weapon[19].yakkyou_sx = 2.0f;
	Weapon[19].yakkyou_sy = 4.0f;
	Weapon[19].blazingmode = false;
	Weapon[19].scopemode = 0;
	Weapon[19].size = 7.7f;
	Weapon[19].soundid = 2;
	Weapon[19].soundvolume = 98;
	Weapon[19].silencer = false;
	Weapon[19].WeaponP = 0;
	Weapon[19].ChangeWeapon = -1;
	Weapon[19].burst = 6;
	Weapon[20].name = "FAMAS";
	Weapon[20].model = "./data/model/weapon/famas.x";
	Weapon[20].texture= "./data/model/weapon/famas.bmp";
	Weapon[20].attacks = 55;
	Weapon[20].penetration = 1;
	Weapon[20].blazings = 3;
	Weapon[20].speed = 11;
	Weapon[20].nbsmax = 25;
	Weapon[20].reloads = 55;
	Weapon[20].reaction = 7;
	Weapon[20].ErrorRangeMIN = 3;
	Weapon[20].ErrorRangeMAX = 19;
	Weapon[20].mx = 9.0f;
	Weapon[20].my = 10.0f;
	Weapon[20].mz = 32.0f;
	Weapon[20].flashx = 9.0f;
	Weapon[20].flashy = 12.0f;
	Weapon[20].flashz = 70.0f;
	Weapon[20].yakkyou_px = 9.0f;
	Weapon[20].yakkyou_py = 11.0f;
	Weapon[20].yakkyou_pz = 14.0f;
	Weapon[20].yakkyou_sx = 3.0f;
	Weapon[20].yakkyou_sy = 5.0f;
	Weapon[20].blazingmode = true;
	Weapon[20].scopemode = 0;
	Weapon[20].size = 8.0f;
	Weapon[20].soundid = 4;
	Weapon[20].soundvolume = 96;
	Weapon[20].silencer = false;
	Weapon[20].WeaponP = 0;
	Weapon[20].ChangeWeapon = -1;
	Weapon[20].burst = 1;
	Weapon[21].name = "MK23";
	Weapon[21].model = "./data/model/weapon/mk23.x";
	Weapon[21].texture= "./data/model/weapon/mk23.bmp";
	Weapon[21].attacks = 48;
	Weapon[21].penetration = 0;
	Weapon[21].blazings = 5;
	Weapon[21].speed = 9;
	Weapon[21].nbsmax = 12;
	Weapon[21].reloads = 42;
	Weapon[21].reaction = 10;
	Weapon[21].ErrorRangeMIN = 2;
	Weapon[21].ErrorRangeMAX = 26;
	Weapon[21].mx = 2.0f;
	Weapon[21].my = 11.0f;
	Weapon[21].mz = 60.0f;
	Weapon[21].flashx = 2.0f;
	Weapon[21].flashy = 15.0f;
	Weapon[21].flashz = 74.0f;
	Weapon[21].yakkyou_px = 1.0f;
	Weapon[21].yakkyou_py = 16.0f;
	Weapon[21].yakkyou_pz = 60.0f;
	Weapon[21].yakkyou_sx = 5.0f;
	Weapon[21].yakkyou_sy = 4.0f;
	Weapon[21].blazingmode = false;
	Weapon[21].scopemode = 0;
	Weapon[21].size = 5.0f;
	Weapon[21].soundid = 4;
	Weapon[21].soundvolume = 92;
	Weapon[21].silencer = false;
	Weapon[21].WeaponP = 1;
	Weapon[21].ChangeWeapon = -1;
	Weapon[21].burst = 1;
	Weapon[22].name = "MK23 SD";
	Weapon[22].model = "./data/model/weapon/mk23sd.x";
	Weapon[22].texture= "./data/model/weapon/mk23.bmp";
	Weapon[22].attacks = 39;
	Weapon[22].penetration = 0;
	Weapon[22].blazings = 5;
	Weapon[22].speed = 9;
	Weapon[22].nbsmax = 12;
	Weapon[22].reloads = 43;
	Weapon[22].reaction = 9;
	Weapon[22].ErrorRangeMIN = 3;
	Weapon[22].ErrorRangeMAX = 26;
	Weapon[22].mx = 2.0f;
	Weapon[22].my = 11.0f;
	Weapon[22].mz = 60.0f;
	Weapon[22].flashx = 2.0f;
	Weapon[22].flashy = 15.0f;
	Weapon[22].flashz = 96.0f;
	Weapon[22].yakkyou_px = 1.0f;
	Weapon[22].yakkyou_py = 16.0f;
	Weapon[22].yakkyou_pz = 60.0f;
	Weapon[22].yakkyou_sx = 5.0f;
	Weapon[22].yakkyou_sy = 4.0f;
	Weapon[22].blazingmode = false;
	Weapon[22].scopemode = 0;
	Weapon[22].size = 5.0f;
	Weapon[22].soundid = 13;
	Weapon[22].soundvolume = 86;
	Weapon[22].silencer = true;
	Weapon[22].WeaponP = 1;
	Weapon[22].ChangeWeapon = -1;
	Weapon[22].burst = 1;


	//缶
	SmallObject[0].model = "./data/article/can.x";
	SmallObject[0].texture = "./data/article/can.bmp";
	SmallObject[0].decide = 10;
	SmallObject[0].hp = 6;
	SmallObject[0].sound = 0;
	SmallObject[0].jump = 10;
	//パソコン
	SmallObject[1].model = "./data/article/pc.x";
	SmallObject[1].texture = "./data/article/pc.bmp";
	SmallObject[1].decide = 29;
	SmallObject[1].hp = 60;
	SmallObject[1].sound = 0;
	SmallObject[1].jump = 5;
	//パソコン キーボード上
	SmallObject[2].model = "./data/article/pc2.x";
	SmallObject[2].texture = "./data/article/pc.bmp";
	SmallObject[2].decide = 29;
	SmallObject[2].hp = 60;
	SmallObject[2].sound = 0;
	SmallObject[2].jump = 5;
	//パソコン 本体逆
	SmallObject[3].model = "./data/article/pc3.x";
	SmallObject[3].texture = "./data/article/pc.bmp";
	SmallObject[3].decide = 29;
	SmallObject[3].hp = 60;
	SmallObject[3].sound = 0;
	SmallObject[3].jump = 5;
	//パソコン ワイド
	SmallObject[4].model = "./data/article/pc_w.x";
	SmallObject[4].texture = "./data/article/pc.bmp";
	SmallObject[4].decide = 29;
	SmallObject[4].hp = 60;
	SmallObject[4].sound = 0;
	SmallObject[4].jump = 5;
	//パソコン ワイド キーボード上
	SmallObject[5].model = "./data/article/pc2_w.x";
	SmallObject[5].texture = "./data/article/pc.bmp";
	SmallObject[5].decide = 29;
	SmallObject[5].hp = 60;
	SmallObject[5].sound = 0;
	SmallObject[5].jump = 5;
	//パソコン ワイド 本体逆
	SmallObject[6].model = "./data/article/pc3_w.x";
	SmallObject[6].texture = "./data/article/pc.bmp";
	SmallObject[6].decide = 29;
	SmallObject[6].hp = 60;
	SmallObject[6].sound = 0;
	SmallObject[6].jump = 5;
	//椅子
	SmallObject[7].model = "./data/article/isu.x";
	SmallObject[7].texture = "./data/article/isu.bmp";
	SmallObject[7].decide = 56;
	SmallObject[7].hp = 50;
	SmallObject[7].sound = 1;
	SmallObject[7].jump = 6;
	//ダンボール
	SmallObject[8].model = "./data/article/dan.x";
	SmallObject[8].texture = "./data/article/dan.bmp";
	SmallObject[8].decide = 28;
	SmallObject[8].hp = 35;
	SmallObject[8].sound = 1;
	SmallObject[8].jump = 8;
	//パソコン 起動中
	SmallObject[9].model = "./data/article/pc.x";
	SmallObject[9].texture = "./data/article/pc_sw.bmp";
	SmallObject[9].decide = 29;
	SmallObject[9].hp = 60;
	SmallObject[9].sound = 0;
	SmallObject[9].jump = 5;
	//パソコン 起動中 暗
	SmallObject[10].model = "./data/article/pc.x";
	SmallObject[10].texture = "./data/article/pc_d.bmp";
	SmallObject[10].decide = 29;
	SmallObject[10].hp = 60;
	SmallObject[10].sound = 0;
	SmallObject[10].jump = 5;
	//パイロン
	SmallObject[11].model = "./data/article/cone.x";
	SmallObject[11].texture = "./data/article/cone.bmp";
	SmallObject[11].decide = 35;
	SmallObject[11].hp = 30;
	SmallObject[11].sound = 1;
	SmallObject[11].jump = 7;


	//銃弾オブジェクト
	Bullet[0].model = "./data/model/bullet.x";
	Bullet[0].texture = "./data/model/bullet.bmp";
	Bullet[0].size = 1.0f;
	//手榴弾オブジェクト
	Bullet[1].model = Weapon[ID_WEAPON_GRENADE].model;
	Bullet[1].texture = Weapon[ID_WEAPON_GRENADE].texture;
	Bullet[1].size = Weapon[ID_WEAPON_GRENADE].size;


	missionname[0]		= "TRAINING YARD";
	missionfullname[0]	= "TRAINING YARD training";
	missiondirectory[0]	= "data\\map0\\";
	missiontxt[0]		= "tr";
	missionname[1]		= "UNDERGROUND_EXT";
	missionfullname[1]	= "UNDERGROUND extermination";
	missiondirectory[1]	= "data\\map5\\";
	missiontxt[1]		= "ext";
	missionname[2]		= "BUILDING_EXT";
	missionfullname[2]	= "BUILDING extermination";
	missiondirectory[2]	= "data\\map1\\";
	missiontxt[2]		= "ext";
	missionname[3]		= "SNOW BASE_EXT";
	missionfullname[3]	= "SNOW BASE extermination";
	missiondirectory[3]	= "data\\map2\\";
	missiontxt[3]		= "ext";
	missionname[4]		= "MBASE_EXT";
	missionfullname[4]	= "MILITARY BASE extermination";
	missiondirectory[4]	= "data\\map4\\";
	missiontxt[4]		= "ext";
	missionname[5]		= "WAREHOUSE_EXT";
	missionfullname[5]	= "WAREHOUSE AREA extermination";
	missiondirectory[5]	= "data\\map7\\";
	missiontxt[5]		= "ext";
	missionname[6]		= "DUEL_EXT";
	missionfullname[6]	= "DUEL extermination";
	missiondirectory[6]	= "data\\map9\\";
	missiontxt[6]		= "ext";
	missionname[7]		= "STATION_EXT";
	missionfullname[7]	= "SUBWAY STATION extermination";
	missiondirectory[7]	= "data\\map6\\";
	missiontxt[7]		= "ext";
	missionname[8]		= "MAZE_EXT";
	missionfullname[8]	= "MAZE extermination";
	missiondirectory[8]	= "data\\map13\\";
	missiontxt[8]		= "ext";
	missionname[9]		= "RUINS_EXT";
	missionfullname[9]	= "RUINS extermination";
	missiondirectory[9]	= "data\\map14\\";
	missiontxt[9]		= "ext";
	missionname[10]		= "URBAN_EXT";
	missionfullname[10]	= "URBAN extermination";
	missiondirectory[10]= "data\\map8\\";
	missiontxt[10]		= "ext";
	missionname[11]		= "UNDERGROUND_EXT2";
	missionfullname[11]	= "UNDERGROUND extermination2";
	missiondirectory[11]= "data\\map5\\";
	missiontxt[11]		= "ext2";
	missionname[12]		= "TUNNEL_EXT";
	missionfullname[12]	= "TUNNEL extermination";
	missiondirectory[12]= "data\\map12\\";
	missiontxt[12]		= "ext";
	missionname[13]		= "URBAN_DEF";
	missionfullname[13]	= "URBAN defend target";
	missiondirectory[13]= "data\\map8\\";
	missiontxt[13]		= "def";
	missionname[14]		= "DTOWN_EXT";
	missionfullname[14]	= "DESERT TOWN extermination";
	missiondirectory[14]= "data\\map3\\";
	missiontxt[14]		= "ext";
	missionname[15]		= "URBAN_DEF2";
	missionfullname[15]	= "URBAN defend target2";
	missiondirectory[15]= "data\\map8\\";
	missiontxt[15]		= "def2";
	missionname[16]		= "WAREHOUSE_KT";
	missionfullname[16]	= "WAREHOUSE AREA kill the target";
	missiondirectory[16]= "data\\map7\\";
	missiontxt[16]		= "kt";
	missionname[17]		= "RUINS_RE";
	missionfullname[17]	= "RUINS release";
	missiondirectory[17]= "data\\map14\\";
	missiontxt[17]		= "re";
	missionname[18]		= "RELIC_CAP";
	missionfullname[18]	= "RELIC capture";
	missiondirectory[18]= "data\\map16\\";
	missiontxt[18]		= "cap";
	missionname[19]		= "MBASE_DE";
	missionfullname[19]	= "MILITARY BASE destroy";
	missiondirectory[19]= "data\\map4\\";
	missiontxt[19]		= "de";
	missionname[20]		= "RUINS_CAP";
	missionfullname[20]	= "RUINS capture";
	missiondirectory[20]= "data\\map14\\";
	missiontxt[20]		= "cap";
	missionname[21]		= "DTOWN_KT";
	missionfullname[21]	= "DESERT TOWN kill the target";
	missiondirectory[21]= "data\\map3\\";
	missiontxt[21]		= "kt";
	missionname[22]		= "SNOW BASE_RE";
	missionfullname[22]	= "SNOW BASE release";
	missiondirectory[22]= "data\\map2\\";
	missiontxt[22]		= "re";
	missionname[23]		= "UNDERGROUND_DEF";
	missionfullname[23]	= "UNDERGROUND defend target";
	missiondirectory[23]= "data\\map5\\";
	missiontxt[23]		= "def";
	missionname[24]		= "TUNNEL_EXT2";
	missionfullname[24]	= "TUNNEL extermination2";
	missiondirectory[24]= "data\\map12\\";
	missiontxt[24]		= "ext2";
	missionname[25]		= "RELIC_DEF";
	missionfullname[25]	= "RELIC defend target";
	missiondirectory[25]= "data\\map16\\";
	missiontxt[25]		= "def";
	missionname[26]		= "URBAN_KT";
	missionfullname[26]	= "URBAN kill the target";
	missiondirectory[26]= "data\\map8\\";
	missiontxt[26]		= "kt";
	missionname[27]		= "ALLEY_KT";
	missionfullname[27]	= "ALLEY kill the target";
	missiondirectory[27]= "data\\map10\\";
	missiontxt[27]		= "kt";
	missionname[28]		= "STATION_KT";
	missionfullname[28]	= "SUBWAY STATION kill the target";
	missiondirectory[28]= "data\\map6\\";
	missiontxt[28]		= "kt";
	missionname[29]		= "WAREHOUSE_DEF";
	missionfullname[29]	= "WAREHOUSE AREA defend target";
	missiondirectory[29]= "data\\map7\\";
	missiontxt[29]		= "def";
	missionname[30]		= "URBAN_KT2";
	missionfullname[30]	= "URBAN kill the target2";
	missiondirectory[30]= "data\\map8\\";
	missiontxt[30]		= "kt2";
	missionname[31]		= "OFFICE_DEF";
	missionfullname[31]	= "OFFICE defend target";
	missiondirectory[31]= "data\\map15\\";
	missiontxt[31]		= "def";
	missionname[32]		= "URBAN_CAP";
	missionfullname[32]	= "URBAN capture";
	missiondirectory[32]= "data\\map8\\";
	missiontxt[32]		= "cap";
	missionname[33]		= "ALLEY_EXT";
	missionfullname[33]	= "ALLEY extermination";
	missiondirectory[33]= "data\\map10\\";
	missiontxt[33]		= "ext";
	missionname[34]		= "TUNNEL_ESC";
	missionfullname[34]	= "TUNNEL escape";
	missiondirectory[34]= "data\\map12\\";
	missiontxt[34]		= "esc";
	missionname[35]		= "MAZE_CAP";
	missionfullname[35]	= "MAZE capture";
	missiondirectory[35]= "data\\map13\\";
	missiontxt[35]		= "cap";
	missionname[36]		= "WAREHOUSE_DEF2";
	missionfullname[36]	= "WAREHOUSE AREA defend target2";
	missiondirectory[36]= "data\\map7\\";
	missiontxt[36]		= "def2";
	missionname[37]		= "RUINS_DE";
	missionfullname[37]	= "RUINS destroy";
	missiondirectory[37]= "data\\map14\\";
	missiontxt[37]		= "de";
	missionname[38]		= "URBAN_KT3";
	missionfullname[38]	= "URBAN kill the target3";
	missiondirectory[38]= "data\\map8\\";
	missiontxt[38]		= "kt3";
	missionname[39]		= "DUEL_EXT2";
	missionfullname[39]	= "DUEL extermination2";
	missiondirectory[39]= "data\\map9\\";
	missiontxt[39]		= "ext2";
	missionname[40]		= "RELIC_EXT";
	missionfullname[40]	= "RELIC extermination";
	missiondirectory[40]= "data\\map16\\";
	missiontxt[40]		= "ext";
	missionname[41]		= "SNOW BASE_DEF";
	missionfullname[41]	= "SNOW BASE defend target";
	missiondirectory[41]= "data\\map2\\";
	missiontxt[41]		= "def";
	missionname[42]		= "BUILDING_KT";
	missionfullname[42]	= "BUILDING kill the target";
	missiondirectory[42]= "data\\map1\\";
	missiontxt[42]		= "kt";
	missionname[43]		= "MAZE_ESC";
	missionfullname[43]	= "MAZE escape";
	missiondirectory[43]= "data\\map13\\";
	missiontxt[43]		= "esc";
	missionname[44]		= "ALLEY_EXT2";
	missionfullname[44]	= "ALLEY extermination2";
	missiondirectory[44]= "data\\map10\\";
	missiontxt[44]		= "ext2";
	missionname[45]		= "MBASE_ESC";
	missionfullname[45]	= "MILITARY BASE escape";
	missiondirectory[45]= "data\\map4\\";
	missiontxt[45]		= "esc";
	missionname[46]		= "DTOWN_DEF";
	missionfullname[46]	= "DESERT TOWN defend target";
	missiondirectory[46]= "data\\map3\\";
	missiontxt[46]		= "def";
	missionname[47]		= "OFFICE_KT";
	missionfullname[47]	= "OFFICE kill the target";
	missiondirectory[47]= "data\\map15\\";
	missiontxt[47]		= "kt";
	missionname[48]		= "BUILDING_DEF";
	missionfullname[48]	= "BUILDING defend target";
	missiondirectory[48]= "data\\map1\\";
	missiontxt[48]		= "def";
	missionname[49]		= "OFFICE_KT2";
	missionfullname[49]	= "OFFICE kill the target2";
	missiondirectory[49]= "data\\map15\\";
	missiontxt[49]		= "kt2";
	missionname[50]		= "MBASE_CAP";
	missionfullname[50]	= "MILITARY BASE capture";
	missiondirectory[50]= "data\\map4\\";
	missiontxt[50]		= "cap";
	missionname[51]		= "TUNNEL_KT";
	missionfullname[51]	= "TUNNEL kill the target";
	missiondirectory[51]= "data\\map12\\";
	missiontxt[51]		= "kt";
	missionname[52]		= "OFFICE_RE";
	missionfullname[52]	= "OFFICE release";
	missiondirectory[52]= "data\\map15\\";
	missiontxt[52]		= "re";
	missionname[53]		= "URBAN_KT4";
	missionfullname[53]	= "URBAN kill the target4";
	missiondirectory[53]= "data\\map8\\";
	missiontxt[53]		= "kt4";
	missionname[54]		= "UNDERGROUND_EXT3";
	missionfullname[54]	= "UNDERGROUND extermination3";
	missiondirectory[54]= "data\\map5\\";
	missiontxt[54]		= "ext3";
	missionname[55]		= "SCHOOL_EXT";
	missionfullname[55]	= "SCHOOL extermination";
	missiondirectory[55]= "data\\map11\\";
	missiontxt[55]		= "ext";
	missionname[56]		= "SCHOOL_EXT2";
	missionfullname[56]	= "SCHOOL extermination2";
	missiondirectory[56]= "data\\map11\\";
	missiontxt[56]		= "ext2";
	missionname[57]		= "SCHOOL_DE";
	missionfullname[57]	= "SCHOOL destroy";
	missiondirectory[57]= "data\\map11\\";
	missiontxt[57]		= "de";


	AIlevel[0].aiming = 1;
	AIlevel[0].attack = 99;
	AIlevel[0].search = 1;
	AIlevel[0].limitserror = 0;
	AIlevel[1].aiming = 1;
	AIlevel[1].attack = 18;
	AIlevel[1].search = 2;
	AIlevel[1].limitserror = 2;
	AIlevel[2].aiming = 2;
	AIlevel[2].attack = 16;
	AIlevel[2].search = 3;
	AIlevel[2].limitserror = 0;
	AIlevel[3].aiming = 3;
	AIlevel[3].attack = 14;
	AIlevel[3].search = 4;
	AIlevel[3].limitserror = -1;
	AIlevel[4].aiming = 4;
	AIlevel[4].attack = 12;
	AIlevel[4].search = 5;
	AIlevel[4].limitserror = -2;
	AIlevel[5].aiming = 5;
	AIlevel[5].attack = 10;
	AIlevel[5].search = 6;
	AIlevel[5].limitserror = -3;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
}

//! @brief 解放
//! @attention 本関数は自動的に呼び出されますが、明示的に呼び出すことも可能です。
void ParameterInfo::DestroyInfo()
{
	//CleanupModelTextureWeapon();
	//CleanupModelTextureSmallObject();

	if( (Human == NULL)&&(Weapon == NULL)&&(SmallObject == NULL)&&(AIlevel == NULL) ){ return; }

	if( Human != NULL ){
		delete [] Human;
		Human = NULL;
	}
	if( Weapon != NULL ){
		delete [] Weapon;
		Weapon = NULL;
	}
	if( SmallObject != NULL ){
		delete [] SmallObject;
		SmallObject = NULL;
	}
	if( Bullet != NULL ){
		delete [] Bullet;
		Bullet = NULL;
	}
	if( AIlevel != NULL ){
		delete [] AIlevel;
		AIlevel = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "設定値", "");
#endif
}

//! @brief 人の設定を取得
//! @param id 番号
//! @param out_data 受け取るHumanParameter型ポインタ
//! @return 成功：0　失敗：1
int ParameterInfo::GetHuman(int id, HumanParameter *out_data)
{
	if( (id < 0)||((TOTAL_PARAMETERINFO_HUMAN -1) < id ) ){ return 1; }

	*out_data = Human[id];
	return 0;
}

//! @brief 人のテクスチャファイルのパスを取得
//! @param id 番号
//! @param *out_str 受け取るポインタ
//! @return 成功：0　失敗：1
int ParameterInfo::GetHumanTexturePath(int id, char *out_str)
{
	if( (id < 0)||((TOTAL_HUMANTEXTURE -1) < id ) ){ return 1; }

	strcpy(out_str, HumanTexturePath[id]);
	return 0;
}

//! @brief 武器の設定を取得
//! @param id 番号
//! @param out_data 受け取るWeaponParameter型ポインタ
//! @return 成功：0　失敗：1
int ParameterInfo::GetWeapon(int id, WeaponParameter *out_data)
{
	if( (id < 0)||((TOTAL_PARAMETERINFO_WEAPON -1) < id ) ){ return 1; }

	*out_data = Weapon[id];
	return 0;
}

//! @brief 小物の設定を取得
//! @param id 番号
//! @param out_data 受け取るSmallObjectParameter型ポインタ
//! @return 成功：0　失敗：1
//! @warning 追加小物の情報は取得できません。追加小物の情報は MIFInterfaceクラス から取得してください。
int ParameterInfo::GetSmallObject(int id, SmallObjectParameter *out_data)
{
	if( (id < 0)||((TOTAL_PARAMETERINFO_SMALLOBJECT -1) < id ) ){ return 1; }

	*out_data = SmallObject[id];
	return 0;
}

//! @brief 銃弾オブジェクトの設定を取得
//! @param id 番号
//! @param out_data 受け取るBulletParameter型ポインタ
//! @return 成功：0　失敗：1
int ParameterInfo::GetBullet(int id, BulletParameter *out_data)
{
	if( (id < 0)||((TOTAL_PARAMETERINFO_BULLET -1) < id ) ){ return 1; }

	*out_data = Bullet[id];
	return 0;
}

//! @brief 標準ミッションを取得
//! @param id 番号
//! @param name ミッション識別名を受け取るポインタ　（NULL可）
//! @param fullname ミッション正式名称を受け取るポインタ　（NULL可）
//! @param directory データの格納先を受け取るポインタ　（NULL可）
//! @param txt ファイル名を受け取るポインタ　（NULL可）
//! @return 成功：0　失敗：1
//! @note ファイル名・・・ミッション情報ファイル（.txt）とポイントデータファイル（.pd1）に使われる名前
int ParameterInfo::GetOfficialMission(int id, char *name, char *fullname, char* directory, char *txt)
{
	if( (id < 0)||((TOTAL_OFFICIALMISSION -1) < id ) ){ return 1; }

	if( name != NULL ){ strcpy(name, missionname[id]); }
	if( fullname != NULL ){ strcpy(fullname, missionfullname[id]); }
	if( directory != NULL ){ strcpy(directory, missiondirectory[id]); }
	if( txt != NULL ){ strcpy(txt, missiontxt[id]); }
	return 0;
}

//! @brief AIレベルの設定（性能値）を取得
//! @param level AIレベル
//! @param out_AIlevel 受け取るAIParameter型ポインタのポインタ（2重ポインタ）
//! @return 成功：0　失敗：1
int ParameterInfo::GetAIlevel(int level, AIParameter **out_AIlevel)
{
	if( (level < 0)||((TOTAL_PARAMETERINFO_AILEVEL -1) < level ) ){ return 1; }
	*out_AIlevel = &(AIlevel[level]);
	return 0;
}