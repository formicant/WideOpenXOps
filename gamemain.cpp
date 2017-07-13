//! @file gamemain.cpp
//! @brief ゲームメイン処理

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

#include "gamemain.h"

D3DGraphics d3dg;			//!< 描画クラス
InputControl inputCtrl;		//!< 入力取得クラス
SoundControl SoundCtrl;		//!< サウンド再生クラス

//! ゲームの設定値
ParameterInfo GameParamInfo;

//! リソース管理
ResourceManager Resource;

SoundManager GameSound;		//!< ゲーム効果音再生クラス

BlockDataInterface BlockData;		//!< ブロックデータ管理クラス
PointDataInterface PointData;		//!< ポイントデータ管理クラス
MIFInterface MIFdata;				//!< MIFコントロール
AddonList GameAddon;				//!< addonのリスト
Collision CollD;					//!< 当たり判定管理クラス
ObjectManager ObjMgr;				//!< オブジェクト管理クラス
AIcontrol HumanAI[MAX_HUMAN];		//!< AI管理クラス

GameInfo GameInfoData;				//!< ゲームの状態

EventControl Event[TOTAL_EVENTLINE];	//!< イベント制御クラス


//! @brief 基本的な初期化処理
int InitGame(WindowControl *WindowCtrl)
{
	//D3DGraphicsクラス初期化
	if( d3dg.InitD3D(WindowCtrl, "data\\char.dds", GameConfig.GetFullscreenFlag()) ){
		WindowCtrl->ErrorInfo("Direct3Dの作成に失敗しました");
		return 1;
	}

	//InputControlクラス初期化
	if( inputCtrl.InitInput(WindowCtrl) ){
		WindowCtrl->ErrorInfo("Input initialization error");
		return 1;
	}

	//SoundControlクラス初期化
	if( SoundCtrl.InitSound(WindowCtrl) ){
		WindowCtrl->ErrorInfo("dll open failed");
		return 1;
	}

	//設定値を初期化
	GameParamInfo.InitInfo();

	//リソースの初期設定
	Resource.SetParameterInfo(&GameParamInfo);
	Resource.SetD3DGraphics(&d3dg);
	Resource.SetSoundControl(&SoundCtrl);

	//リソースを初期化
	Resource.LoadHumanModel();
	Resource.LoadWeaponModelTexture();
	Resource.LoadWeaponSound();
	Resource.LoadSmallObjectModelTexture();
	Resource.LoadSmallObjectSound();
	Resource.LoadScopeTexture();
	Resource.LoadBulletModelTexture();
	Resource.LoadBulletSound();
	Resource.LoadEffectTexture();

	//効果音初期化
	float volume;
	if( GameConfig.GetSoundFlag() == false ){
		volume = 0.0f;
	}
	else{
		volume = 1.0f;
	}
	GameSound.SetClass(&SoundCtrl, &Resource, &GameParamInfo);
	SoundCtrl.SetVolume(volume);

	//オブジェクトマネージャー初期化
	ObjMgr.SetClass(&GameParamInfo, &d3dg, &Resource, &BlockData, &PointData, &CollD, &GameSound, &MIFdata);

	//addonリスト作成
	GameAddon.LoadFiledata("addon\\");

	GameInfoData.selectaddon = false;

	return 0;
}

//! @brief リソースをリセットする
//! @return 成功：0　失敗：-1　待機：1
//! @attention 通常は、描画処理に失敗した場合に限り呼び出してください。
int ResetGame(WindowControl *WindowCtrl)
{
	//リストを正しく解放するため、予め呼ぶ。
	Resource.CleanupHumanTexture();

	int rtn = d3dg.ResetD3D(WindowCtrl);

	if( rtn == 0 ){
		//リソースを初期化
		Resource.LoadHumanModel();
		Resource.LoadWeaponModelTexture();
		Resource.LoadSmallObjectModelTexture();
		Resource.LoadScopeTexture();
		Resource.LoadBulletModelTexture();
		Resource.LoadEffectTexture();

		//WindowCtrl->ErrorInfo("Recovery...");
		return 0;
	}
	if( rtn == 1 ){
		return 1;
	}
	//if( rtn == 2 ){
		//WindowCtrl->ErrorInfo("Resetに失敗しました");
		//WindowCtrl->CloseWindow();
		return -1;
	//}
}

//! @brief 基本的な解放処理
void CleanupGame()
{
	//リソースを解放
	Resource.DestroyResource();

	//設定値を解放
	GameParamInfo.DestroyInfo();


	//SoundControlクラス解放
	SoundCtrl.DestroySound();

	//InputControlクラス解放
	inputCtrl.DestroyInput();

	//D3DGraphicsクラス解放
	d3dg.DestroyD3D();
}

//! @brief コンストラクタ
opening::opening()
{
	add_camera_x = 0.0f;
	add_camera_y = 0.0f;
	add_camera_z = 0.0f;
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;
}

//! @brief ディストラクタ
opening::~opening()
{}

int opening::Create()
{
	int blockflag, pointflag;

	//ブロックデータ読み込み
	blockflag = BlockData.LoadFiledata("data\\map10\\temp.bd1");

	//ポイントデータ読み込み
	pointflag = PointData.LoadFiledata("data\\map10\\op.pd1");

	//ファイル読み込みでエラーがあったら中断
	if( (blockflag != 0)||(pointflag != 0) ){
		//2bit : point data open failed
		//1bit : block data open failed
		return pointflag*2 + blockflag;
	}

	//ブロックデータ初期化
	BlockData.CalculationBlockdata(false);
	d3dg->LoadMapdata(&BlockData, "data\\map10\\");
	CollD.InitCollision(&BlockData);

	//ポイントデータ初期化
	ObjMgr.LoadPointData();
	ObjMgr.SetPlayerID(MAX_HUMAN-1);	//実在しない人をプレイヤーに（銃声のサウンド再生対策）

	//追加の当たり判定設定
	ObjMgr.SetAddCollisionFlag(false);

	//AI設定
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
	}

	//背景空読み込み
	Resource.LoadSkyModelTexture(1);

	//opening_banner = d3dg->LoadTexture("banner.png", true, false);

	//サウンド初期化
	GameSound->InitWorldSound();

	//マウスカーソルを中央へ移動
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	camera_x = -5.0f;
	camera_y = 58.0f;
	camera_z = 29.0f;
	camera_rx = DegreeToRadian(206);
	camera_ry = DegreeToRadian(12);
	add_camera_x = 0.0f;
	add_camera_y = 0.0f;
	add_camera_z = 0.0f;
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;

	GameState->NextState();
	return 0;
}

int opening::Recovery()
{
	//ブロックデータ初期化
	d3dg->LoadMapdata(&BlockData, "data\\map10\\");

	//ポイントデータ初期化
	ObjMgr.Recovery();

	//背景空読み込み
	Resource.LoadSkyModelTexture(1);

	//opening_banner = d3dg->LoadTexture("banner.png", true, false);

	return 0;
}

void opening::Input()
{
	inputCtrl->GetInputState(false);

	if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){
		GameState->PushBackSpaceKey();
	}
	else if( inputCtrl->CheckMouseButtonUpL() ){
		GameState->PushMouseButton();
	}
	else if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F)) ){		// [ENTER]
		//[ENTER]を押しても、マウスをクリックしたことにする。
		GameState->PushMouseButton();
	}
}

//! @todo カメラの移動を滑らかにする
void opening::Process()
{
	//オブジェクトマネージャーを実行
	ObjMgr.Process(-1, false, camera_rx, camera_ry, false);

	//AIを実行
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].Process();
	}

	//カメラワークを求める（座標）
	if( framecnt < (int)(4.0f*GAMEFPS) ){
		add_camera_z = 0.0f;
		add_camera_y = 0.0f;
	}
	else if( framecnt < (int)(5.0f*GAMEFPS) ){
		add_camera_z += (0.08f - add_camera_z) / 5.0f;
		add_camera_y += (-0.05f - add_camera_y) / 5.0f;
	}
	else{
		add_camera_z = 0.08f;
		add_camera_y = -0.05f;
	}
	camera_x += add_camera_x;
	camera_y += add_camera_y;
	camera_z += add_camera_z;

	//カメラワークを求める（回転）
	if( framecnt < (int)(2.6f*GAMEFPS) ){
		add_camera_rx = 0.0f;
		add_camera_ry = 0.0f;
	}
	else if( framecnt < (int)(3.6f*GAMEFPS) ){
		add_camera_rx += (DegreeToRadian(0.9f) - add_camera_rx) / 5.0f;
		add_camera_ry += (DegreeToRadian(-0.6f) - add_camera_ry) / 5.0f;
	}
	else if( framecnt < (int)(5.0f*GAMEFPS) ){
		add_camera_rx = DegreeToRadian(0.9f);
		add_camera_ry = DegreeToRadian(-0.6f);
	}
	else{
		add_camera_rx *= 0.8f;
		add_camera_ry *= 0.8f;
	}
	camera_rx += add_camera_rx;
	camera_ry += add_camera_ry;

	//16秒経ったら終了
	if( framecnt >= 16*((int)GAMEFPS) ){
		GameState->PushMouseButton();
	}

	framecnt += 1;
}

void opening::Render3D()
{
	int skymodel, skytexture;

	//フォグとカメラを設定
	d3dg->SetFog(1);
	d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);

	//カメラ座標に背景空を描画
	d3dg->SetWorldTransform(camera_x, camera_y, camera_z, 0.0f, 0.0f, 2.0f);
	Resource.GetSkyModelTexture(&skymodel, &skytexture);
	d3dg->RenderModel(skymodel, skytexture, false);

	//Zバッファを初期化
	d3dg->ResetZbuffer();

	//マップを描画
	d3dg->ResetWorldTransform();
	d3dg->DrawMapdata(false);

	//オブジェクトを描画
	ObjMgr.Render(camera_x, camera_y, camera_z, 0);
}

void opening::Render2D()
{
	float effect = 0.0f;

	//ブラックアウト設定
	if( framecnt < (int)(1.0f*GAMEFPS) ){
		effect = GetEffectAlpha(framecnt, 1.0f, 1.0f, 0.0f, true);
	}
	if( ((int)(1.0f*GAMEFPS) <= framecnt)&&(framecnt < (int)(13.0f*GAMEFPS)) ){
		effect = 0.0f;
	}
	if( ((int)(11.0f*GAMEFPS) <= framecnt)&&(framecnt < (int)(15.0f*GAMEFPS)) ){
		effect =  GetEffectAlpha(framecnt, 1.0f, 4.0f, 11.0f, false);
	}
	if( (int)(15.0f*GAMEFPS) <= framecnt ){
		effect = 1.0f;
	}
	d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,effect));

	//上下の黒縁描画
	d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, 40, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DBox(0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));

	//プロジェクト名
	if( ((int)(0.5f*GAMEFPS) < framecnt)&&(framecnt < (int)(4.0f*GAMEFPS)) ){
		char str[32];
		float effectA = 1.0f;
		sprintf(str, "%s project", GAMENAME);
		if( framecnt < (int)(1.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 0.5f, false); }
		if( framecnt > (int)(3.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 3.0f, true); }
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(str)*22/2, SCREEN_HEIGHT - 140, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 22, 22);
	}

	//スタッフ名・その１
	if( ((int)(4.5f*GAMEFPS) < framecnt)&&(framecnt < (int)(8.5f*GAMEFPS)) ){
		float effectA = 1.0f;
		if( framecnt < (int)(5.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 4.5f, false); }
		if( framecnt > (int)(7.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 7.5f, true); }
		d3dg->Draw2DTextureFontText(60, 150, "ORIGINAL", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
	}
	if( ((int)(5.0f*GAMEFPS) < framecnt)&&(framecnt < (int)(9.0f*GAMEFPS)) ){
		float effectA = 1.0f;
		if( framecnt < (int)(6.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 5.0f, false); }
		if( framecnt > (int)(8.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 8.0f, true); }
		d3dg->Draw2DTextureFontText(100, 180, "nine-two", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
		d3dg->Draw2DTextureFontText(100, 210, "TENNKUU", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
	}

	//スタッフ名・その２
	if( ((int)(7.0f*GAMEFPS) < framecnt)&&(framecnt < (int)(11.0f*GAMEFPS)) ){
		float effectA = 1.0f;
		if( framecnt < (int)(8.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 7.0f, false); }
		if( framecnt > (int)(10.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 10.0f, true); }
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH - 310, 300, "REMAKE", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
	}
	if( ((int)(7.5f*GAMEFPS) < framecnt)&&(framecnt < (int)(11.5f*GAMEFPS)) ){
		float effectA = 1.0f;
		if( framecnt < (int)(8.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 7.5f, false); }
		if( framecnt > (int)(10.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 10.5f, true); }
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH - 270, 330, "[-_-;](mikan)", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
		//d3dg->Draw2DTexture(410, 360, opening_banner, 200, 40, effectA);
	}

	//ゲーム名
	if( (int)(12.0f*GAMEFPS) <= framecnt ){	//framecnt < (int)(16.0f*GAMEFPS)
		char str[32];
		float effectA = 1.0f;
		sprintf(str, GAMENAME);
		if( framecnt < (int)(13.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 12.0f, false); }
		if( ((int)(15.0f*GAMEFPS) < framecnt)&&(framecnt < (int)(16.0f*GAMEFPS)) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 15.0f, true); }
		if( framecnt >= (int)(16.0f*GAMEFPS) ){ effectA = 0.0f; }
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(str)*22/2, (SCREEN_HEIGHT-11)/2, str, d3dg->GetColorCode(1.0f,0.0f,0.0f,effectA), 22, 22);
	}
}

void opening::Destroy()
{
	//ブロックデータ解放
	d3dg->CleanupMapdata();

	//オブジェクトマネージャー解放
	ObjMgr.Cleanup();

	//背景空解放
	Resource.CleanupSkyModelTexture();

	//d3dg->CleanupTexture(opening_banner);

	GameState->NextState();
}

//! @brief コンストラクタ
mainmenu::mainmenu()
{
	demopath[0] = '\0';
	mainmenu_scrollitems_official = 0;
	mainmenu_scrollitems_addon = 0;
}

//! @brief ディストラクタ
mainmenu::~mainmenu()
{}

int mainmenu::Create()
{
	char bdata[MAX_PATH];
	char pdata[MAX_PATH];
	int blockflag, pointflag;

	//デモを決定し読み込む
	switch( GetRand(6) ){
		case 0:
			strcpy(demopath, "data\\map2\\");
			break;
		case 1:
			strcpy(demopath, "data\\map4\\");
			break;
		case 2:
			strcpy(demopath, "data\\map5\\");
			break;
		case 3:
			strcpy(demopath, "data\\map7\\");
			break;
		case 4:
			strcpy(demopath, "data\\map8\\");
			break;
		case 5:
			strcpy(demopath, "data\\map16\\");
			break;
	}
	strcpy(bdata, demopath);
	strcat(bdata, "temp.bd1");
	strcpy(pdata, demopath);
	strcat(pdata, "demo.pd1");

	//ブロックデータ読み込み
	blockflag = BlockData.LoadFiledata(bdata);

	//ポイントデータ読み込み
	pointflag = PointData.LoadFiledata(pdata);

	//ファイル読み込みでエラーがあったら中断
	if( (blockflag != 0)||(pointflag != 0) ){
		//2bit : point data open failed
		//1bit : block data open failed
		return pointflag*2 + blockflag;
	}

	//ブロックデータ初期化
	BlockData.CalculationBlockdata(false);
	d3dg->LoadMapdata(&BlockData, demopath);
	CollD.InitCollision(&BlockData);

	//ポイントデータ初期化
	ObjMgr.LoadPointData();

	//追加の当たり判定設定
	ObjMgr.SetAddCollisionFlag(false);

	//AI設定
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
	}

	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	//サウンド初期化
	GameSound->InitWorldSound();

	mainmenu_mouseX = SCREEN_WIDTH/2;
	mainmenu_mouseY = SCREEN_HEIGHT/2;

	//標準ミッションのスクロールバーの設定
#if TOTAL_OFFICIALMISSION > TOTAL_MENUITEMS
	//if( TOTAL_OFFICIALMISSION > TOTAL_MENUITEMS ){
		mainmenu_scrollbar_official_height = (float)(MAINMENU_H-25) / TOTAL_OFFICIALMISSION * TOTAL_MENUITEMS;
		mainmenu_scrollbar_official_scale = ((float)(MAINMENU_H-25) - mainmenu_scrollbar_official_height) / (TOTAL_OFFICIALMISSION - TOTAL_MENUITEMS);
	//}
#else
	//else{
		mainmenu_scrollbar_official_height = 0.0f;
		mainmenu_scrollbar_official_scale = 0.0f;
	//}
#endif

	//addonのスクロールバーの設定
	if( GameAddon.GetTotaldatas() > TOTAL_MENUITEMS ){
		mainmenu_scrollbar_addon_height = (float)(MAINMENU_H-25) / GameAddon.GetTotaldatas() * TOTAL_MENUITEMS;
		mainmenu_scrollbar_addon_scale = ((float)(MAINMENU_H-25) - mainmenu_scrollbar_addon_height) / (GameAddon.GetTotaldatas() - TOTAL_MENUITEMS);
	}
	else{
		mainmenu_scrollbar_addon_height = 0.0f;
		mainmenu_scrollbar_addon_scale = 0.0f;
	}

	mainmenu_scrollbar_official_y = MAINMENU_Y+1 + (int)(mainmenu_scrollbar_official_scale*mainmenu_scrollitems_official);
	mainmenu_scrollbar_addon_y = MAINMENU_Y+1 + (int)(mainmenu_scrollbar_addon_scale*mainmenu_scrollitems_addon);
	mainmenu_scrollbar_flag = false;
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	GameState->NextState();
	return 0;
}

int mainmenu::Recovery()
{
	//ブロックデータ初期化
	d3dg->LoadMapdata(&BlockData, demopath);

	//ポイントデータ初期化
	ObjMgr.Recovery();

	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	return 0;
}

void mainmenu::Input()
{
	inputCtrl->GetInputState(false);

	//スクロールバーの情報などを取得
	int scrollitems;
	float scrollbar_height;
	float scrollbar_scale;
	int scrollbar_y;
	int totalmission;
	if( GameInfoData.selectaddon == false ){
		scrollitems = mainmenu_scrollitems_official;
		scrollbar_height = mainmenu_scrollbar_official_height;
		scrollbar_scale = mainmenu_scrollbar_official_scale;
		scrollbar_y = mainmenu_scrollbar_official_y;
		totalmission = TOTAL_OFFICIALMISSION;
	}
	else{
		scrollitems = mainmenu_scrollitems_addon;
		scrollbar_height = mainmenu_scrollbar_addon_height;
		scrollbar_scale = mainmenu_scrollbar_addon_scale;
		scrollbar_y = mainmenu_scrollbar_addon_y;
		totalmission = GameAddon.GetTotaldatas();
	}

	//マウス座標を取得
	int y = mainmenu_mouseY;
	inputCtrl->GetMouseMovement(&mainmenu_mouseX, &mainmenu_mouseY);
	if( mainmenu_mouseX < 0 ){ mainmenu_mouseX = 0; }
	if( mainmenu_mouseX > SCREEN_WIDTH-1 ){ mainmenu_mouseX = SCREEN_WIDTH-1; }
	if( mainmenu_mouseY < 0 ){ mainmenu_mouseY = 0; }
	if( mainmenu_mouseY > SCREEN_HEIGHT-1 ){ mainmenu_mouseY = SCREEN_HEIGHT-1; }

	if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){		//ESCキーを処理
		GameState->PushBackSpaceKey();
	}
	else if( inputCtrl->CheckMouseButtonUpL() ){		//ミッション選択
		for(int i=0; i<TOTAL_MENUITEMS; i++){
			char name[32];
			strcpy(name, "");
			if( GameInfoData.selectaddon == false ){
				GameParamInfo.GetOfficialMission(scrollitems + i, name, NULL, NULL, NULL, NULL, NULL);
			}
			else{
				strcpy(name, GameAddon.GetMissionName(scrollitems + i));
			}

			if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+(signed)strlen(name)*20))&&(MAINMENU_Y+30 + i*30 < mainmenu_mouseY)&&(mainmenu_mouseY < MAINMENU_Y+30 + i*30 + 26) ){
				GameInfoData.selectmission_id = scrollitems + i;
				GameState->PushMouseButton();
			}
		}
	}

	//スクロールバーを押したか判定
	if( inputCtrl->CheckMouseButtonDownL() ){
		if( ((MAINMENU_X+341) < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+360))&&(MAINMENU_Y+1 + (int)(scrollbar_scale*scrollitems) < mainmenu_mouseY)
			&&(mainmenu_mouseY < MAINMENU_Y+1 + (int)(scrollbar_scale*scrollitems + scrollbar_height))
		){
			mainmenu_scrollbar_flag = true;
		}
	}

	if( inputCtrl->CheckMouseButtonUpL() ){
		mainmenu_scrollbar_flag = false;

		// UP
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&(MAINMENU_Y < mainmenu_mouseY)&&(mainmenu_mouseY < MAINMENU_Y+30) ){
			if( scrollitems > 0 ){ scrollitems -= 1; }
		}

		// DOWN
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&((MAINMENU_Y+MAINMENU_H-55) < mainmenu_mouseY)&&(mainmenu_mouseY < (MAINMENU_Y+MAINMENU_H-55+30)) ){
			if( scrollitems < (totalmission - TOTAL_MENUITEMS) ){ scrollitems += 1; }
		}
	}

	//スクロールバーの移動
	if( mainmenu_scrollbar_flag == true ){
		scrollbar_y += mainmenu_mouseY - y;
		if( scrollbar_y < MAINMENU_Y+1 ){ scrollbar_y = MAINMENU_Y+1; }
		if( scrollbar_y > MAINMENU_Y+MAINMENU_H-24 - (int)(scrollbar_height) ){ scrollbar_y = MAINMENU_Y+MAINMENU_H-24 - (int)(scrollbar_height); }

		scrollitems = (scrollbar_y - (MAINMENU_Y+1)) / (int)(scrollbar_scale);
		if( scrollitems < 0 ){
			scrollitems = 0;
		}
		if( scrollitems > (totalmission - TOTAL_MENUITEMS) ){
			scrollitems = (totalmission - TOTAL_MENUITEMS);
		}
	}
	else{
		scrollbar_y = MAINMENU_Y+1 + (int)(scrollbar_scale*scrollitems);
	}

	//スクロールバーの情報などを反映
	if( GameInfoData.selectaddon == false ){
		mainmenu_scrollitems_official = scrollitems;
		mainmenu_scrollbar_official_y = scrollbar_y;
	}
	else{
		mainmenu_scrollitems_addon = scrollitems;
		mainmenu_scrollbar_addon_y = scrollbar_y;
	}

	//標準ミッションとアドオンリストの切り替え
	if( inputCtrl->CheckMouseButtonUpL() ){
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&((MAINMENU_Y+MAINMENU_H-25) < mainmenu_mouseY)&&(mainmenu_mouseY < (MAINMENU_Y+MAINMENU_H-2)) ){
			if( GameInfoData.selectaddon == false ){
				if( GameAddon.GetTotaldatas() > 0 ){
					GameInfoData.selectaddon = true;
				}
			}
			else{
				GameInfoData.selectaddon = false;
			}
		}
	}
}

void mainmenu::Process()
{
	//オブジェクトマネージャーを実行
	ObjMgr.Process(-1, true, camera_rx, camera_ry, false);

	//AIを実行
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].Process();
	}

	//カメラ位置を計算
	human *myHuman = ObjMgr.GetPlayerHumanObject();
	myHuman->GetPosData(&camera_x, &camera_y, &camera_z, NULL);
	camera_x -= 4.0f;
	camera_y += 22.0f;
	camera_z -= 12.0f;
	camera_rx = DegreeToRadian(45);
	camera_ry = DegreeToRadian(-25);

	framecnt += 1;
}

void mainmenu::Render3D()
{
	//フォグとカメラを設定
	d3dg->SetFog(0);
	d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);

	//Zバッファを初期化
	d3dg->ResetZbuffer();

	//マップを描画
	d3dg->ResetWorldTransform();
	d3dg->DrawMapdata(false);

	//オブジェクトを描画
	ObjMgr.Render(camera_x, camera_y, camera_z, 0);
}

void mainmenu::Render2D()
{
	int color, color2;
	float effect;

	//スクロールバーの情報などを取得
	int scrollitems;
	float scrollbar_height;
	int scrollbar_y;
	int totalmission;
	if( GameInfoData.selectaddon == false ){
		scrollitems = mainmenu_scrollitems_official;
		scrollbar_height = mainmenu_scrollbar_official_height;
		scrollbar_y = mainmenu_scrollbar_official_y;
		totalmission = TOTAL_OFFICIALMISSION;
	}
	else{
		scrollitems = mainmenu_scrollitems_addon;
		scrollbar_height = mainmenu_scrollbar_addon_height;
		scrollbar_y = mainmenu_scrollbar_addon_y;
		totalmission = GameAddon.GetTotaldatas();
	}

	//ゲームのバージョン情報表示
	d3dg->Draw2DTextureFontText(522+1, 75+1, GAMEVERSION, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 18, 22);
	d3dg->Draw2DTextureFontText(522, 75, GAMEVERSION, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 18, 22);

	//メニューエリア描画
	if( GameAddon.GetTotaldatas() > 0 ){	//addonがあれば
		d3dg->Draw2DBox(MAINMENU_X-1, MAINMENU_Y, MAINMENU_X+360, MAINMENU_Y+MAINMENU_H+1, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
	}
	else{
		d3dg->Draw2DBox(MAINMENU_X-1, MAINMENU_Y, MAINMENU_X+360, MAINMENU_Y+MAINMENU_H-24, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
	}
	d3dg->Draw2DBox(MAINMENU_X+341, MAINMENU_Y+1, MAINMENU_X+360, MAINMENU_Y+MAINMENU_H-24, d3dg->GetColorCode(0.5f,0.5f,0.5f,0.5f));

	//スクロールバー描画
	if( totalmission > TOTAL_MENUITEMS ){
		//色を設定
		if( mainmenu_scrollbar_flag == true ){
			color = d3dg->GetColorCode(0.6f,0.3f,0.25f,1.0f);
			color2 = d3dg->GetColorCode(0.8f,0.3f,0.25f,1.0f);
		}
		else if( ((MAINMENU_X+341) < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+360))&&(scrollbar_y < mainmenu_mouseY)&&(mainmenu_mouseY < scrollbar_y + (int)scrollbar_height) ){
			color = d3dg->GetColorCode(0.4f,0.67f,0.57f,1.0f);
			color2 = d3dg->GetColorCode(0.38f,0.77f,0.64f,1.0f);
		}
		else{
			color = d3dg->GetColorCode(0.6f,0.6f,0.25f,1.0f);
			color2 = d3dg->GetColorCode(0.8f,0.8f,0.25f,1.0f);
		}

		//描画
		d3dg->Draw2DBox(MAINMENU_X+341, scrollbar_y, MAINMENU_X+360, scrollbar_y + (int)scrollbar_height, color);
		d3dg->Draw2DBox(MAINMENU_X+341+3, scrollbar_y +3, MAINMENU_X+360-3, scrollbar_y + (int)scrollbar_height -3, color2);
	}

	//'< UP >'表示
	if( scrollitems > 0 ){
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+1, "<  UP  >", d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 25, 26);

		//文字の色を設定
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&(MAINMENU_Y < mainmenu_mouseY)&&(mainmenu_mouseY < MAINMENU_Y+30) ){
			color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
		}
		else{
			color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
		}

		//文字を表示
		d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y, "<  UP  >", color, 25, 26);
	}
	else{
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+1, "<  UP  >", d3dg->GetColorCode(0.6f,0.6f,0.6f,1.0f), 25, 26);
	}

	//'< DOWN >'表示
	if( scrollitems < (totalmission - TOTAL_MENUITEMS) ){
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+MAINMENU_H-55+1, "< DOWN >", d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 25, 26);

		//文字の色を設定
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&((MAINMENU_Y+MAINMENU_H-55) < mainmenu_mouseY)&&(mainmenu_mouseY < (MAINMENU_Y+MAINMENU_H-55+30)) ){
			color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
		}
		else{
			color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
		}

		//文字を表示
		d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y+MAINMENU_H-55, "< DOWN >", color, 25, 26);
	}
	else{
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+MAINMENU_H-55+1, "< DOWN >", d3dg->GetColorCode(0.6f,0.6f,0.6f,1.0f), 25, 26);
	}

	//標準ミッションとaddon切り替え
	if( GameInfoData.selectaddon == false ){
		//addonがあれば
		if( GameAddon.GetTotaldatas() > 0 ){
			//文字の色を設定
			if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&((MAINMENU_Y+MAINMENU_H-25) < mainmenu_mouseY)&&(mainmenu_mouseY < (MAINMENU_Y+MAINMENU_H-2)) ){
				color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
			}
			else{
				color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
			}

			//文字を表示
			d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+MAINMENU_H-25+1, "ADD-ON MISSIONS >>", d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 17, 22);
			d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y+MAINMENU_H-25, "ADD-ON MISSIONS >>", color, 17, 22);
		}
	}
	else{
		//addonがあれば
		//if( GameAddon.GetTotaldatas() > 0 ){
			//文字の色を設定
			if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&((MAINMENU_Y+MAINMENU_H-25) < mainmenu_mouseY)&&(mainmenu_mouseY < (MAINMENU_Y+MAINMENU_H-2)) ){
				color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
			}
			else{
				color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
			}

			//文字を表示
			d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+MAINMENU_H-25+1, "<< STANDARD MISSIONS", d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 17, 22);
			d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y+MAINMENU_H-25, "<< STANDARD MISSIONS", color, 17, 22);
		//}
	}

	//ミッション名を表示
	for(int i=0; i<TOTAL_MENUITEMS; i++){
		char name[32];
		strcpy(name, "");

		//ミッション名を取得
		if( GameInfoData.selectaddon == false ){
			GameParamInfo.GetOfficialMission(scrollitems + i, name, NULL, NULL, NULL, NULL, NULL);
		}
		else{
			strcpy(name, GameAddon.GetMissionName(scrollitems + i));
		}

		//文字の色を設定
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+(signed)strlen(name)*20))&&(MAINMENU_Y+30 + i*30 < mainmenu_mouseY)&&(mainmenu_mouseY < MAINMENU_Y+30 + i*30 + 26) ){
			color = d3dg->GetColorCode(1.0f,0.6f,0.6f,1.0f);
		}
		else{
			color = d3dg->GetColorCode(0.6f,0.6f,1.0f,1.0f);
		}

		//文字を表示
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+30+1 + i*30, name, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 20, 26);
		d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y+30 + i*30, name, color, 20, 26);
	}

	//マウスカーソル表示（赤線）
	d3dg->Draw2DBox(0, mainmenu_mouseY-1, SCREEN_WIDTH, mainmenu_mouseY+1, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
	d3dg->Draw2DBox(mainmenu_mouseX-1, 0, mainmenu_mouseX+1, SCREEN_HEIGHT, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
	d3dg->Draw2DLine(0, mainmenu_mouseY, SCREEN_WIDTH, mainmenu_mouseY, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DLine(mainmenu_mouseX, 0, mainmenu_mouseX, SCREEN_HEIGHT, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));

	//ゲームのロゴマーク描画
	d3dg->Draw2DTexture(20, 25, gametitle, 480, 80, 1.0f);

	//ブラックアウト設定
	if( framecnt < (int)(2.0f*GAMEFPS) ){
		effect = GetEffectAlpha(framecnt, 1.0f, 2.0f, 0.0f, true);
	}
	else{
		effect = 0.0f;
	}
	d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,effect));
}

void mainmenu::Destroy()
{
	//ブロックデータ解放
	d3dg->CleanupMapdata();

	//オブジェクトマネージャー解放
	ObjMgr.Cleanup();

	//背景空解放
	Resource.CleanupSkyModelTexture();

	d3dg->CleanupTexture(gametitle);

	GameState->NextState();
}

//! @brief コンストラクタ
briefing::briefing()
{}

//! @brief ディストラクタ
briefing::~briefing()
{}

int briefing::Create()
{
	char path[MAX_PATH];
	char pdata[MAX_PATH];
	char PictureA[MAX_PATH];
	char PictureB[MAX_PATH];

	//背景画像を取得
	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	//mifファイルのファイルパス取得
	if( GameInfoData.selectaddon == false ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, NULL, path, pdata, NULL, NULL);
		strcat(path, pdata);
		strcat(path, ".txt");
	}
	else{
		strcpy(path, "addon\\");
		strcat(path, GameAddon.GetFileName(GameInfoData.selectmission_id));
	}

	//mifファイルを読み込み
	if( MIFdata.LoadFiledata(path) != 0 ){
		//briefing data open failed
		return 1;
	}

	//ブリーフィング画像のファイルパス取得
	MIFdata.GetPicturefilePath(PictureA, PictureB);

	//ブリーフィング画像読み込み
	if( strcmp(PictureB, "!") == 0 ){
		TwoTexture = false;
		TextureA = d3dg->LoadTexture(PictureA, true, false);
		TextureB = -1;
	}
	else{
		TwoTexture = true;
		TextureA = d3dg->LoadTexture(PictureA, true, false);
		TextureB = d3dg->LoadTexture(PictureB, true, false);
	}

	//マウスカーソルを中央へ移動
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	GameState->NextState();
	return 0;
}

int briefing::Recovery()
{
	char PictureA[MAX_PATH];
	char PictureB[MAX_PATH];

	//背景画像を取得
	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	//ブリーフィング画像のファイルパス取得
	MIFdata.GetPicturefilePath(PictureA, PictureB);

	//ブリーフィング画像読み込み
	if( strcmp(PictureB, "!") == 0 ){
		TwoTexture = false;
		TextureA = d3dg->LoadTexture(PictureA, true, false);
		TextureB = -1;
	}
	else{
		TwoTexture = true;
		TextureA = d3dg->LoadTexture(PictureA, true, false);
		TextureB = d3dg->LoadTexture(PictureB, true, false);
	}

	return 0;
}

void briefing::Render2D()
{
	float effectA = GetEffectAlphaLoop(framecnt, 0.8f, 0.7f, true);
	float effectB = GetEffectAlphaLoop(framecnt, 0.8f, 1.0f, true);
	int effectB_sizeW = (int)( (float)(framecnt%((int)(GAMEFPS*1.0f))) * 0.2f + 18 );
	int effectB_sizeH = (int)( (float)(framecnt%((int)(GAMEFPS*1.0f))) * 1.0f + 26 );

	//メモ：背景画像の描画は、自動的に行われる。

	//固定文字表示
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 60*4, 30, "BRIEFING", d3dg->GetColorCode(1.0f,1.0f,0.0f,effectA), 60, 42);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH - 210 - effectB_sizeW*20/2, SCREEN_HEIGHT - 37 - effectB_sizeH/2,
								"LEFT CLICK TO BEGIN", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectB), effectB_sizeW, effectB_sizeH);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH - 210 - 18*20/2, SCREEN_HEIGHT - 37 - 26/2, "LEFT CLICK TO BEGIN", d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 18, 26);

	//ブリーフィング画像描画
	if( TwoTexture == false ){
		if( TextureA == -1 ){ d3dg->Draw2DBox(40, 180, 40+160, 180+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f)); }
		else{ d3dg->Draw2DTexture(40, 180, TextureA, 160, 150, 1.0f); }
	}
	else{
		if( TextureA == -1 ){ d3dg->Draw2DBox(40, 130, 40+160, 130+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f)); }
		else{ d3dg->Draw2DTexture(40, 130, TextureA, 160, 150, 1.0f); }

		if( TextureB == -1 ){ d3dg->Draw2DBox(40, 300, 40+160, 300+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f)); }
		else{ d3dg->Draw2DTexture(40, 300, TextureB, 160, 150, 1.0f); }
	}

	//ミッション名を取得・表示
	char mname[64];
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, mname, NULL, NULL, NULL, NULL);
	}
	else{
		strcpy(mname, MIFdata.GetMissionFullname());
	}
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(mname)*18/2, 90, mname, d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f), 18, 25);

	//ミッション説明を表示
	d3dg->Draw2DMSFontText(230, 180, MIFdata.GetBriefingText(), d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
}

void briefing::Destroy()
{
	//ブリーフィング画像を開放
	d3dg->CleanupTexture(TextureA);
	d3dg->CleanupTexture(TextureB);

	//背景画像を開放
	d3dg->CleanupTexture(gametitle);

	GameState->NextState();
}

//! @brief コンストラクタ
maingame::maingame()
{
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;
	ShowInfo_Debugmode = false;
	Camera_F2mode = 0;
	Camera_HOMEmode = false;
	time_input = 0;
	time_process_object = 0;
	time_process_ai = 0;
	time_process_event = 0;
	time_sound = 0;
	time_render = 0;
}

//! @brief ディストラクタ
maingame::~maingame()
{}

//! @brief ゲームの実行速度を取得
int maingame::GetGameSpeed()
{
	return GameSpeed;
}

int maingame::Create()
{
	MainGameInfo = GameInfoData;
	char path[MAX_PATH];
	char bdata[MAX_PATH];
	char pdata[MAX_PATH];
	char pdata2[MAX_PATH];
	int blockflag, pointflag;
	bool collisionflag;

	//.bd1と.pd1のファイルパスを求める
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(MainGameInfo.selectmission_id, NULL, NULL, path, pdata2, &collisionflag, &DarkScreenFlag);

		strcpy(bdata, path);
		strcat(bdata, OFFICIALMISSION_BD1);
		strcpy(pdata, path);
		strcat(pdata, pdata2);
		strcat(pdata, ".pd1");
	}
	else{
		MIFdata.GetDatafilePath(bdata, pdata);
		collisionflag = MIFdata.GetCollisionFlag();
		DarkScreenFlag = MIFdata.GetScreenFlag();

		strcpy(path, bdata);
		for(int i=strlen(path)-1; i>0; i--){
			if( path[i] == '\\' ){
				path[i+1] = '\0';
				break;
			}
		}
	}

	//追加小物を読み込む
	Resource.LoadAddSmallObject(MIFdata.GetAddSmallobjectModelPath(), MIFdata.GetAddSmallobjectTexturePath(), MIFdata.GetAddSmallobjectSoundPath());

	//ブロックデータ読み込み
	blockflag = BlockData.LoadFiledata(bdata);

	//ポイントデータ読み込み
	pointflag = PointData.LoadFiledata(pdata);

	//ファイル読み込みでエラーがあったら中断
	if( (blockflag != 0)||(pointflag != 0) ){
		//2bit : point data open failed
		//1bit : block data open failed
		return pointflag*2 + blockflag;
	}

	//ブロックデータ初期化
	BlockData.CalculationBlockdata(DarkScreenFlag);
	d3dg->LoadMapdata(&BlockData, path);
	CollD.InitCollision(&BlockData);

	//ポイントデータ初期化
	ObjMgr.LoadPointData();

	//追加の当たり判定設定
	ObjMgr.SetAddCollisionFlag(collisionflag);

	//AI設定
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
	}


	//背景空読み込み
	SkyNumber = MIFdata.GetSkynumber();
	Resource.LoadSkyModelTexture(SkyNumber);

	//サウンド初期化
	GameSound->InitWorldSound();

	//イベント初期化
	for(int i=0; i<TOTAL_EVENTLINE; i++){
		Event[i].SetClass(&PointData, &ObjMgr);
	}
	Event[0].Reset(TOTAL_EVENTENTRYPOINT_0);
	Event[1].Reset(TOTAL_EVENTENTRYPOINT_1);
	Event[2].Reset(TOTAL_EVENTENTRYPOINT_2);

	//プレイヤーの向きを取得
	ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

	view_rx = 0.0f;
	view_ry = 0.0f;
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;
	Camera_Debugmode = false;
	tag = false;
	radar = false;
	wireframe = false;
	CenterLine = false;
	Camera_Blind = true;
	Camera_F1mode = false;
	InvincibleID = -1;
	PlayerAI = false;
	AIstop = false;
	AINoFight = false;
	AIdebuginfoID = -1;
	framecnt = 0;
	start_framecnt = 0;
	end_framecnt = 0;
	EventStop = false;
	GameSpeed = 1;
	message_id = -1;
	message_cnt = 0;
	redflash_flag = false;
	MainGameInfo.missioncomplete = false;
	MainGameInfo.fire = 0;
	MainGameInfo.ontarget = 0;
	MainGameInfo.kill = 0;
	MainGameInfo.headshot = 0;

#ifdef ENABLE_DEBUGCONSOLE
	Show_Console = false;
	ScreenShot = 0;

	//コンソール用初期化
	InfoConsoleData = new ConsoleData [MAX_CONSOLELINES];
	InputConsoleData = new ConsoleData;
	for(int i=0; i<MAX_CONSOLELINES; i++){
		InfoConsoleData[i].color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
		InfoConsoleData[i].textdata[0] = '\0';
	}
	InputConsoleData->color = d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f);
	InputConsoleData->textdata[0] = '\0';
	AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Game Debug Console.");
	AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "        Command list >help");
#endif

	//マウスカーソルを中央へ移動
	inputCtrl->MoveMouseCenter();

	GameState->NextState();
	return 0;
}

int maingame::Recovery()
{
	char path[MAX_PATH];
	char bdata[MAX_PATH];	//ダミー
	char pdata[MAX_PATH];	//ダミー

	//.bd1と.pd1のファイルパスを求める
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(MainGameInfo.selectmission_id, NULL, NULL, path, NULL, NULL, NULL);
	}
	else{
		MIFdata.GetDatafilePath(bdata, pdata);

		strcpy(path, bdata);
		for(int i=strlen(path)-1; i>0; i--){
			if( path[i] == '\\' ){
				path[i+1] = '\0';
				break;
			}
		}
	}

	//追加小物を読み込む
	Resource.LoadAddSmallObject(MIFdata.GetAddSmallobjectModelPath(), MIFdata.GetAddSmallobjectTexturePath(), MIFdata.GetAddSmallobjectSoundPath());

	//ブロックデータ初期化
	d3dg->LoadMapdata(&BlockData, path);

	//ポイントデータ初期化
	ObjMgr.Recovery();

	//背景空読み込み
	Resource.LoadSkyModelTexture(SkyNumber);

	return 0;
}

//! @brief 特定操作の入力をチェック
bool maingame::CheckInputControl(int CheckKey, int mode)
{
	int KeyCode = OriginalkeycodeToDinputdef(GameConfig.GetKeycode(CheckKey));
	if( KeyCode == -1 ){
		if( mode == 0 ){
			return inputCtrl->CheckMouseButtonNowL();
		}
		if( mode == 1 ){
			return inputCtrl->CheckMouseButtonDownL();
		}
		//if( mode == 2 ){
			return inputCtrl->CheckMouseButtonUpL();
		//}
	}
	if( KeyCode == -2 ){
		if( mode == 0 ){
			return inputCtrl->CheckMouseButtonNowR();
		}
		if( mode == 1 ){
			return inputCtrl->CheckMouseButtonDownR();
		}
		//if( mode == 2 ){
			return inputCtrl->CheckMouseButtonUpR();
		//}
	}
	if( KeyCode == -3 ){
		int CodeL, CodeR;
		GetDoubleKeyCode(0, &CodeL, &CodeR);
		if( mode == 0 ){
			if( inputCtrl->CheckKeyNow(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyNow(CodeR);
		}
		if( mode == 1 ){
			if( inputCtrl->CheckKeyDown(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyDown(CodeR);
		}
		//if( mode == 2 ){
			if( inputCtrl->CheckKeyUp(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyUp(CodeR);
		//}
	}
	if( KeyCode == -4 ){
		int CodeL, CodeR;
		GetDoubleKeyCode(1, &CodeL, &CodeR);
		if( mode == 0 ){
			if( inputCtrl->CheckKeyNow(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyNow(CodeR);
		}
		if( mode == 1 ){
			if( inputCtrl->CheckKeyDown(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyDown(CodeR);
		}
		//if( mode == 2 ){
			if( inputCtrl->CheckKeyUp(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyUp(CodeR);
		//}
	}

	if( mode == 0 ){
		return inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(GameConfig.GetKeycode(CheckKey)) );
	}
	if( mode == 1 ){
		return inputCtrl->CheckKeyDown( OriginalkeycodeToDinputdef(GameConfig.GetKeycode(CheckKey)) );
	}
	//if( mode == 2 ){
		return inputCtrl->CheckKeyUp( OriginalkeycodeToDinputdef(GameConfig.GetKeycode(CheckKey)) );
	//}
}

void maingame::Input()
{
	time = GetTimeMS();

	//プレイヤーのクラスを取得
	human *myHuman = ObjMgr.GetPlayerHumanObject();

	//キー入力を取得
	inputCtrl->GetInputState(true);
	inputCtrl->MoveMouseCenter();

	//マウスの移動量取得
	int x, y;
	float MouseSensitivity;
	inputCtrl->GetMouseMovement(&x, &y);

	//視点の移動量計算
	float mang = 0.0f;
	if( myHuman->GetScopeMode() == 0 ){ mang = 0.01f; }
	if( myHuman->GetScopeMode() == 1 ){ mang = 0.0032f; }
	if( myHuman->GetScopeMode() == 2 ){ mang = 0.0060f; }
	MouseSensitivity = DegreeToRadian(1) * mang * GameConfig.GetMouseSensitivity();

	//マウス反転（オプション設定）が有効ならば、反転する。
	if( GameConfig.GetInvertMouseFlag() == true ){
		y *= -1;
	}

	if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){					//ゲーム終了操作かチェック
		GameState->PushBackSpaceKey();
		GameSpeed = 1;
	}
	else if( inputCtrl->CheckKeyDown( GetFunctionKeycode(12) ) ){	//リセット操作かチェック
		GameState->PushF12Key();
		GameSpeed = 1;
	}

	//画面のUI変更操作かチェック
	if( inputCtrl->CheckKeyDown( GetFunctionKeycode(2) ) ){
		if( Camera_F2mode == 2 ){
			Camera_F2mode = 0;
		}
		else{
			Camera_F2mode += 1;
		}
	}

#ifdef ENABLE_DEBUGCONSOLE
	if( Show_Console == false ){
#endif

		if( Camera_Debugmode == false ){	//通常モードならば
			if( PlayerAI == false ){
				InputPlayer(myHuman, x, y, MouseSensitivity);
			}
		}
		else{								//デバックモードならば
			InputViewCamera(x, y, MouseSensitivity);
		}

#ifdef ENABLE_DEBUGCONSOLE
	}

	//デバック用コンソールの表示操作かチェック
	if( inputCtrl->CheckKeyDown( GetFunctionKeycode(11) ) ){
		if( Show_Console == false ){
			Show_Console = true;

			strcpy(InputConsoleData->textdata, CONSOLE_PROMPT);
		}
		else{
			Show_Console = false;
		}
	}

	if( Show_Console == true ){
		InputConsole();
	}
#endif

	time_input = GetTimeMS() - time;
}

//! @brief プレイヤー操作系の入力処理
//! @param myHuman プレイヤーのクラス
//! @param mouse_x マウスのX座標
//! @param mouse_y マウスのY座標 
//! @param MouseSensitivity 視点の移動量計算
void maingame::InputPlayer(human *myHuman, int mouse_x, int mouse_y, float MouseSensitivity)
{
	int PlayerID = ObjMgr.GetPlayerID();

	if( myHuman->GetHP() > 0 ){

		//マウスによる向きを計算
		mouse_rx += mouse_x * MouseSensitivity;
		mouse_ry -= mouse_y * MouseSensitivity;

		//キー操作による向きを計算
		if(      (CheckInputControl(KEY_TURNUP, 0) == true)&&(CheckInputControl(KEY_TURNDOWN, 0) == false) ){ add_camera_ry += (INPUT_ARROWKEYS_ANGLE - add_camera_ry)*0.2f; }			// 標準：[↑]
		else if( (CheckInputControl(KEY_TURNDOWN, 0) == true)&&(CheckInputControl(KEY_TURNUP, 0) == false) ){ add_camera_ry += (INPUT_ARROWKEYS_ANGLE*-1 - add_camera_ry)*0.2f; }		// 標準：[↓]
		else { add_camera_ry = 0.0f; }
		if(      (CheckInputControl(KEY_TURNLEFT, 0) == true)&&(CheckInputControl(KEY_TURNRIGHT, 0) == false) ){ add_camera_rx += (INPUT_ARROWKEYS_ANGLE*-1 - add_camera_rx)*0.2f; }	// 標準：[←]
		else if( (CheckInputControl(KEY_TURNRIGHT, 0) == true)&&(CheckInputControl(KEY_TURNLEFT, 0) == false) ){ add_camera_rx += (INPUT_ARROWKEYS_ANGLE - add_camera_rx)*0.2f; }		// 標準：[→]
		else { add_camera_rx = 0.0f; }
		mouse_rx += add_camera_rx;
		mouse_ry += add_camera_ry;

		if( mouse_ry > DegreeToRadian(70) ) mouse_ry = DegreeToRadian(70);
		if( mouse_ry < DegreeToRadian(-70) ) mouse_ry = DegreeToRadian(-70);


		//プレイヤー（オブジェクト）の向きを設定
		myHuman->SetRxRy(mouse_rx, mouse_ry);


		//前後左右の移動（走り）操作かチェック
		if( CheckInputControl(KEY_MOVEFORWARD, 0) ){
			ObjMgr.MoveForward(PlayerID);
		}
		else{
			if( CheckInputControl(KEY_MOVEBACKWARD, 0) ){
				ObjMgr.MoveBack(PlayerID);
			}
		}
		if( CheckInputControl(KEY_MOVELEFT, 0) ){
			ObjMgr.MoveLeft(PlayerID);
		}
		else{
			if( CheckInputControl(KEY_MOVERIGHT, 0) ){
				ObjMgr.MoveRight(PlayerID);
			}
		}

		//歩き操作かチェック
		if( CheckInputControl(KEY_WALK, 0) ){
			ObjMgr.MoveWalk(PlayerID);
		}

		//ジャンプ操作かチェック
		if( CheckInputControl(KEY_JUMP, 1) ){
			ObjMgr.MoveJump(PlayerID);
		}

		HumanParameter humandata;
		int id_param;
		bool zombie;
		int keymode;

		//ゾンビかどうか判定
		myHuman->GetParamData(&id_param, NULL, NULL, NULL);
		GameParamInfo.GetHuman(id_param, &humandata);
		if( humandata.type == 2 ){
			zombie = true;
		}
		else{
			zombie = false;
		}

		//連射モードを取得
		if( zombie == true ){
			keymode = 1;
		}
		else if( myHuman->GetWeaponBlazingmode() == false ){
			keymode = 1;
		}
		else{
			keymode = 0;
		}

		//発砲操作かチェック
		if( CheckInputControl(KEY_Shot, keymode) ){

			if( zombie == false ){
				//弾の発射に成功すれば
				if( ObjMgr.ShotWeapon(PlayerID) == 1 ){
					//スコアに加算
					MainGameInfo.fire += 1;

					//プレイヤーの向きを取得
					ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);
				}
			}
			else{
				for(int i=0; i<MAX_HUMAN; i++){
					human *EnemyHuman = ObjMgr.GeHumanObject(i);
					if( ObjMgr.CheckZombieAttack(myHuman, EnemyHuman) == true ){
						ObjMgr.HitZombieAttack(myHuman, EnemyHuman);
					}
				}
			}

		}

		//リロード操作かチェック
		if( CheckInputControl(KEY_RELOAD, 1) ){
			ObjMgr.ReloadWeapon(PlayerID);
		}

		//武器の切り替え操作かチェック
		if( CheckInputControl(KEY_SWITCHWEAPON, 1) ){
			ObjMgr.ChangeWeapon(PlayerID, -1);
		}
		if( CheckInputControl(KEY_WEAPON1, 1) ){
			ObjMgr.ChangeWeapon(PlayerID, 0);
		}
		if( CheckInputControl(KEY_WEAPON2, 1) ){
			ObjMgr.ChangeWeapon(PlayerID, 1);
		}

		//武器の廃棄操作かチェック
		if( CheckInputControl(KEY_DROPWEAPON, 1) ){
			ObjMgr.DumpWeapon(PlayerID);
		}

		//スコープ操作かチェック
		if( CheckInputControl(KEY_ZOOMSCOPE, 1) ){
			ObjMgr.ChangeScopeMode(PlayerID);
		}

		//連射モード変更操作かチェック
		if( CheckInputControl(KEY_ShotMODE, 1) ){
			ObjMgr.ChangeShotMode(PlayerID);
		}

		//カメラ表示モード変更操作かチェック
		if( inputCtrl->CheckKeyDown( GetFunctionKeycode(1) ) ){
			if( Camera_F1mode == false ){
				Camera_F1mode = true;
				view_rx = 0.0f;
				view_ry = VIEW_F1MODE_ANGLE;
			}
			else{
				Camera_F1mode = false;
				view_rx = 0.0f;
				view_ry = 0.0f;
			}
		}

		//カメラ操作
		if( Camera_F1mode == true ){
			if( inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(0x0C) ) ){	//NUM8
				view_ry -= INPUT_F1NUMKEYS_ANGLE;
			}
			if( inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(0x09) ) ){	//NUM5
				view_ry += INPUT_F1NUMKEYS_ANGLE;
			}
			if( inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(0x08) ) ){	//NUM4
				view_rx -= INPUT_F1NUMKEYS_ANGLE;
			}
			if( inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(0x0A) ) ){	//NUM6
				view_rx += INPUT_F1NUMKEYS_ANGLE;
			}
		}

	}

	//　ここまで通常操作系
	//
	//　ここから裏技系

	if( myHuman->GetHP() > 0 ){

		//裏技・上昇の操作かチェック
		if( (inputCtrl->CheckKeyNow( GetFunctionKeycode(5) ))&&(inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x0F))) ){	// F5 + [ENTER]
			Cmd_F5 = true;
		}
		else{
			Cmd_F5 = false;
		}

		//裏技・弾追加の操作かチェック
		if( inputCtrl->CheckKeyNow( GetFunctionKeycode(6) ) ){
			if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F)) ){		// [ENTER]
				ObjMgr.CheatAddBullet(PlayerID);
			}
		}

		//裏技・武器変更の操作かチェック
		if( inputCtrl->CheckKeyNow( GetFunctionKeycode(7) ) ){
			if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x02)) ){		// [←]
				int id_param = myHuman->GetMainWeaponTypeNO();

				//次の武器番号を計算
				if( id_param >= TOTAL_PARAMETERINFO_WEAPON-1 ){ id_param = 0; }
				else{ id_param += 1; }

				ObjMgr.CheatNewWeapon(PlayerID, id_param);
			}
			if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x03)) ){		// [→]
				int id_param = myHuman->GetMainWeaponTypeNO();

				//次の武器番号を計算
				if( id_param <= 0 ){ id_param = TOTAL_PARAMETERINFO_WEAPON-1; }
				else{ id_param -= 1; }

				ObjMgr.CheatNewWeapon(PlayerID, id_param);
			}
		}

	}

	//裏技・人変更の操作かチェック
	if( inputCtrl->CheckKeyNow( GetFunctionKeycode(8) ) ){
		int Player_HumanID;

		if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x02)) ){		// [←]
			//現在のプレイヤー番号を取得
			Player_HumanID = ObjMgr.GetPlayerID();

			//次の人を計算
			Player_HumanID += 1;
			if( Player_HumanID >= MAX_HUMAN ){ Player_HumanID = 0; }

			//対象プレイヤー番号を適用
			ObjMgr.SetPlayerID(Player_HumanID);

			//プレイヤーの向きを取得
			ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

			//F1モード時にカメラの向きを再設定
			if( Camera_F1mode == true ){
				camera_rx = DegreeToRadian(90);
			}
		}
		if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x03)) ){		// [→]
			//現在のプレイヤー番号を取得
			Player_HumanID = ObjMgr.GetPlayerID();

			//次の人を計算
			Player_HumanID -= 1;
			if( Player_HumanID < 0 ){ Player_HumanID = MAX_HUMAN-1; }

			//対象プレイヤー番号を適用
			ObjMgr.SetPlayerID(Player_HumanID);

			//プレイヤーの向きを取得
			ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

			//F1モード時にカメラの向きを再設定
			if( Camera_F1mode == true ){
				camera_rx = DegreeToRadian(90);
			}
		}
	}

	if( myHuman->GetHP() > 0 ){

		//裏技・人追加の操作かチェック
		if( inputCtrl->CheckKeyNow( GetFunctionKeycode(9) ) ){
			if( (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x00)))||(inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x01))) ){		// [↑]・[↓]
				float x, y, z, r;
				int param, dataid, team; 
				int selectweapon;
				weapon *weapon[TOTAL_HAVEWEAPON];
				int weapon_paramid[TOTAL_HAVEWEAPON];
				for(int i=0; i<TOTAL_HAVEWEAPON; i++){
					weapon[i] = NULL;
					weapon_paramid[i] = 0;
				}
				int id;

				//プレイヤーの座標や武器を取得
				myHuman->GetPosData(&x, &y, &z, &r);
				myHuman->GetParamData(&param, &dataid, NULL, &team);
				myHuman->GetWeapon(&selectweapon, weapon);
				for(int i=0; i<TOTAL_HAVEWEAPON; i++){
					if( weapon[i] != NULL ){
						weapon[i]->GetParamData(&weapon_paramid[i], NULL, NULL);
					}
				}

				//プレイヤーの目の前の座標を取得
				x += cos(r*-1 + (float)M_PI/2)*10.0f;
				y += 5.0f;
				z += sin(r*-1 + (float)M_PI/2)*10.0f;

				//人を追加
				id = ObjMgr.AddHumanIndex(x, y, z, r, param, team, weapon_paramid);
				if( id >= 0 ){
					ObjMgr.ChangeWeapon(id, selectweapon);

					//AIを設定
					HumanAI[id].Init();
					if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x00)) ){		// [↑]
						HumanAI[id].SetHoldTracking(PlayerID);
					}
					if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x01)) ){		// [↓]
						HumanAI[id].SetHoldWait(x, z, r);
					}
				}
			}
		}

		//裏技・腕描画の操作かチェック
		if( inputCtrl->CheckKeyDown( GetHomeKeycode() ) ){
			if( Camera_HOMEmode == false ){
				Camera_HOMEmode = true;
			}
			else{
				Camera_HOMEmode = false;
			}
		}
	}
}

//! @brief フリーカメラ操作系の入力処理
//! @param mouse_x マウスのX座標
//! @param mouse_y マウスのY座標 
//! @param MouseSensitivity 視点の移動量計算
void maingame::InputViewCamera(int mouse_x, int mouse_y, float MouseSensitivity)
{
	//マウス移動をカメラの向きとして適用
	camera_rx -= mouse_x * MouseSensitivity;
	camera_ry -= mouse_y * MouseSensitivity;

	//キー操作による向きを計算
	if( CheckInputControl(KEY_TURNUP, 0) ){ camera_ry += INPUT_ARROWKEYS_ANGLE; }			// 標準：[↑]
	if( CheckInputControl(KEY_TURNDOWN, 0) ){ camera_ry -= INPUT_ARROWKEYS_ANGLE; }			// 標準：[↓]
	if( CheckInputControl(KEY_TURNLEFT, 0) ){ camera_rx += INPUT_ARROWKEYS_ANGLE; }			// 標準：[←]
	if( CheckInputControl(KEY_TURNRIGHT, 0) ){ camera_rx -= INPUT_ARROWKEYS_ANGLE; }		// 標準：[→]

	if( camera_ry > DegreeToRadian(70) ) camera_ry = DegreeToRadian(70);
	if( camera_ry < DegreeToRadian(-70) ) camera_ry = DegreeToRadian(-70);

	//移動量決定
	float dist = VIEW_FREECAMERA_SCALE;
	if( CheckInputControl(KEY_Shot, 0) ){
		dist *= 2;
	}

	//キー操作によりカメラ座標を計算
	if( CheckInputControl(KEY_MOVEFORWARD, 0) ){
		camera_x += cos(camera_rx)*cos(camera_ry)*dist;
		camera_y += sin(camera_ry)*dist;
		camera_z += sin(camera_rx)*cos(camera_ry)*dist;
	}
	if( CheckInputControl(KEY_MOVEBACKWARD, 0) ){
		camera_x -= cos(camera_rx)*cos(camera_ry)*dist;
		camera_y -= sin(camera_ry)*dist;
		camera_z -= sin(camera_rx)*cos(camera_ry)*dist;
	}
	if( CheckInputControl(KEY_MOVELEFT, 0) ){
		camera_x += cos(camera_rx + (float)M_PI/2)*dist;
		camera_z += sin(camera_rx + (float)M_PI/2)*dist;
	}
	if( CheckInputControl(KEY_MOVERIGHT, 0) ){
		camera_x += cos(camera_rx - (float)M_PI/2)*dist;
		camera_z += sin(camera_rx - (float)M_PI/2)*dist;
	}
}

void maingame::Process()
{
	//プレイヤーのクラスを取得
	human *myHuman = ObjMgr.GetPlayerHumanObject();

	int ontarget, kill, headshot;

	//-----------------------------------

	time = GetTimeMS();

	//武器の持ち方を取得
	int weaponid;
	WeaponParameter data;
	weaponid = myHuman->GetMainWeaponTypeNO();
	GameParamInfo.GetWeapon(weaponid, &data);

	//オブジェクトマネージャーを実行
	int cmdF5id;
	if( Cmd_F5 == true ){
		cmdF5id = ObjMgr.GetPlayerID();
	}
	else{
		cmdF5id = -1;
	}
	ObjMgr.Process(cmdF5id, false, camera_rx, camera_ry, DarkScreenFlag);

	//プレイヤーの戦歴を加算
	ObjMgr.GetHumanShotInfo( ObjMgr.GetPlayerID(), &ontarget, &kill, &headshot);
	MainGameInfo.ontarget += ontarget;
	MainGameInfo.kill += kill;
	MainGameInfo.headshot += headshot;

	time_process_object = GetTimeMS() - time;

	//-----------------------------------

	time = GetTimeMS();
	if( AIstop == false ){
		int PlayerID = ObjMgr.GetPlayerID();
		for(int i=0; i<MAX_HUMAN; i++){
			if( (PlayerAI == false)&&(i == PlayerID) ){ continue; }
			//AIを実行
			HumanAI[i].Process();
		}
	}
	time_process_ai = GetTimeMS() - time;

	//-----------------------------------

	//ミッションが終了していなければ、
	if( end_framecnt == 0 ){
		int check = ObjMgr.CheckGameOverorComplete();

		//ゲームクリアー判定
		if( check == 1 ){
			end_framecnt += 1;
			MainGameInfo.missioncomplete = true;
		}

		//ゲームオーバー判定
		if( check == 2 ){
			end_framecnt += 1;
			MainGameInfo.missioncomplete = false;
		}
	}

	//-----------------------------------

	time = GetTimeMS();
	bool SetMessageID;
	if( end_framecnt == 0 ){
		if( EventStop == false ){
			//イベント実行
			for(int i=0; i<TOTAL_EVENTLINE; i++){
				SetMessageID = false;
				Event[i].Execution(&end_framecnt, &MainGameInfo.missioncomplete, &message_id, &SetMessageID);

				//イベントメッセージが再セットされていたら、カウントを戻す。
				if( SetMessageID == true ){
					message_cnt = 0;
				}
			}
		}
	}
	if( (message_id != -1)&&(message_cnt < (int)(TOTAL_EVENTENT_SHOWMESSEC*GAMEFPS)) ){
		message_cnt += 1;
	}
	else{
		message_id = -1;
		message_cnt = 0;
	}
	time_process_event = GetTimeMS() - time;

	//-----------------------------------

	float x, y, z;
	myHuman->GetPosData(&x, &y, &z, NULL);

	if( PlayerAI == true ){
		//プレイヤーの向きを取得
		ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);
	}

	//カメラワークを求める
	if( Camera_Debugmode == true ){
		//
	}
	else if( myHuman->GetDeadFlag() == true ){
		float camera_ry2 = camera_ry;

		//camera_ry2を -PI～PI の間に正規化
		for(; camera_ry2>(float)M_PI; camera_ry2 -= (float)M_PI*2){}
		for(; camera_ry2<(float)M_PI*-1; camera_ry2 += (float)M_PI*2){}

		float ry = camera_ry2*0.95f + DegreeToRadian(-89.0f)*0.05f;						// 19/20 + 1/20
		float r = 3.12f;

		camera_rx += DegreeToRadian(1.0f);
		camera_ry = ry;
		camera_x = x + cos(camera_rx)*r;
		camera_y = y + 33.3f;
		camera_z = z + sin(camera_rx)*r;
	}
	else if( Camera_F1mode == true ){
		float crx, cry;
		float ccx, ccy, ccz;

		//カメラの注視点を計算
		crx = camera_rx*0.8f + (view_rx + mouse_rx*-1 + (float)M_PI/2)*0.2f;	// 8 : 2
		cry = camera_ry*0.8f + (view_ry + mouse_ry)*0.2f - (float)M_PI/2;		// 8 : 2
		ccx = x - cos(crx)*cos(cry)*3.0f;
		ccy = y + HUMAN_HEIGHT-0.5f + sin(cry*-1)*2.5f;
		ccz = z - sin(crx)*cos(cry)*3.0f;

		//注視点からカメラまでの当たり判定
		cry += (float)M_PI/2;
		float dist;
		if( CollD.CheckALLBlockIntersectRay(ccx, ccy, ccz, cos(crx)*cos(cry)*-1, sin(cry*-1), sin(crx)*cos(cry)*-1, NULL, NULL, &dist, VIEW_F1MODE_DIST) == true ){
			dist -= 1.0f;
		}
		else{
			dist = VIEW_F1MODE_DIST;
		}

		//カメラ座標を再計算
		camera_x = ccx - cos(crx)*cos(cry)*(dist);
		camera_y = ccy + sin(cry*-1)*dist;
		camera_z = ccz - sin(crx)*cos(cry)*(dist);
		camera_rx = crx;
		camera_ry = cry;
	}
	else{
		float crx = view_rx + mouse_rx*-1 + (float)M_PI/2;
		float cry = view_ry + mouse_ry;

		camera_x = x + cos(crx)*cos(cry)*VIEW_DIST;
		camera_y = y + VIEW_HEIGHT + sin(cry)*VIEW_DIST;
		camera_z = z + sin(crx)*cos(cry)*VIEW_DIST;
		camera_rx = crx;
		camera_ry = cry;
	}

	//ダメージを受けていれば、レッドフラッシュを描画する
	redflash_flag = myHuman->CheckHit(NULL);

	//-----------------------------------

#ifdef ENABLE_DEBUGCONSOLE
	//デバック用コンソールを表示しており、かつミッション終了のカウントが始まっていなければ～
	if( (Show_Console == true)&&(end_framecnt == 0) ){
		//デバック用コンソールのメイン処理
		ProcessConsole();
	}
#endif

	//-----------------------------------

	framecnt += 1;
	if( start_framecnt < (int)(1.0f*GAMEFPS) ){	//ミッション開始中なら
		start_framecnt += 1;
	}
	if( end_framecnt == 1 ){					//ミッション終了直後ならば
		MainGameInfo.framecnt = framecnt;
		GameInfoData = MainGameInfo;	//全てコピー
		end_framecnt += 1;
	}
	else if( end_framecnt > 0 ){				//ミッション終了中ならば
		if( end_framecnt < (int)(5.0f*GAMEFPS) ){
			end_framecnt += 1;
		}
		else{
			GameInfoData.fire = MainGameInfo.fire;			//射撃回数
			GameInfoData.ontarget = MainGameInfo.ontarget;	//命中数
			GameInfoData.kill = MainGameInfo.kill;			//倒した敵の数
			GameInfoData.headshot = MainGameInfo.headshot;	//敵の頭部に命中した数

			GameState->PushMouseButton();
			GameSpeed = 1;
		}
	}
}

void maingame::Sound()
{
	time = GetTimeMS();

	//プレイヤーのチーム番号取得
	int teamid;
	human *myHuman = ObjMgr.GetPlayerHumanObject();
	myHuman->GetParamData(NULL, NULL, NULL, &teamid);

	//サウンドを再生
	GameSound->PlayWorldSound(camera_x, camera_y, camera_z, camera_rx, teamid);

	time_sound = GetTimeMS() - time;
}

void maingame::Render3D()
{
	time = GetTimeMS();


	int skymodel, skytexture;
	human *myHuman = ObjMgr.GetPlayerHumanObject();

	//フォグとカメラを設定
	d3dg->SetFog(SkyNumber);
	if( Camera_F1mode == false ){
		int scopemode = myHuman->GetScopeMode();
		float viewangle = 0.0f;

		//スコープによる視野角を決定
		if( scopemode == 0 ){ viewangle = VIEWANGLE_NORMAL; }
		if( scopemode == 1 ){ viewangle = VIEWANGLE_SCOPE_1; }
		if( scopemode == 2 ){ viewangle = VIEWANGLE_SCOPE_2; }

		d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, viewangle);
	}
	else{
		d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);
	}

	//カメラ座標に背景空を描画
	d3dg->SetWorldTransform(camera_x, camera_y, camera_z, 0.0f, 0.0f, 2.0f);
	Resource.GetSkyModelTexture(&skymodel, &skytexture);
	d3dg->RenderModel(skymodel, skytexture, DarkScreenFlag);

	//Zバッファを初期化
	d3dg->ResetZbuffer();

	if( CenterLine == true ){
		//中心線表示（デバック用）
		d3dg->Centerline();
	}

	//マップを描画
	d3dg->ResetWorldTransform();
	d3dg->DrawMapdata(wireframe);

	//プレイヤーの描画有無の決定
	int DrawPlayer = 0;
	if( (Camera_F1mode == false)&&(Camera_Debugmode == false)&&(myHuman->GetHP() > 0) ){
		if( Camera_HOMEmode == false ){
			DrawPlayer = 1;
		}
		else{
			DrawPlayer = 2;
		}
	}
	//オブジェクトを描画
	ObjMgr.Render(camera_x, camera_y, camera_z, DrawPlayer);

	//AIデバック情報表示
	if( AIdebuginfoID != -1 ){
		if( (0 <= AIdebuginfoID)&&(AIdebuginfoID < MAX_HUMAN) ){
			float posx, posy, posz, rx;
			int EnemyID;
			float mposx, mposz;
			int movemode;
			ObjMgr.GeHumanObject(AIdebuginfoID)->GetPosData(&posx, &posy, &posz, &rx);
			EnemyID = HumanAI[AIdebuginfoID].GetEnemyHumanID();
			HumanAI[AIdebuginfoID].GetMoveTargetPos(&mposx, &mposz, &movemode);

			d3dg->ResetWorldTransform();

			//本人
			d3dg->Drawline(posx+10.0f, posy, posz, posx-10.0f, posy, posz, d3dg->GetColorCode(0.0f,0.0f,1.0f,1.0f));
			d3dg->Drawline(posx, posy+10.0f, posz, posx, posy-10.0f, posz, d3dg->GetColorCode(0.0f,0.0f,1.0f,1.0f));
			d3dg->Drawline(posx, posy, posz+10.0f, posx, posy, posz-10.0f, d3dg->GetColorCode(0.0f,0.0f,1.0f,1.0f));

			//移動先
			d3dg->Drawline(mposx+10.0f, posy, mposz, mposx-10.0f, posy, mposz, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			d3dg->Drawline(mposx, 5000.0f, mposz, mposx, -500.0f,mposz, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			d3dg->Drawline(mposx, posy, mposz+10.0f, mposx, posy, mposz-10.0f, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));

			if( EnemyID != -1 ){
				ObjMgr.GeHumanObject(EnemyID)->GetPosData(&posx, &posy, &posz, &rx);

				//攻撃対象
				d3dg->Drawline(posx+3.0f, posy, posz+3.0f, posx+3.0f, posy, posz-3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx+3.0f, posy, posz-3.0f, posx-3.0f, posy, posz-3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx-3.0f, posy, posz-3.0f, posx-3.0f, posy, posz+3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx-3.0f, posy, posz+3.0f, posx+3.0f, posy, posz+3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx+3.0f, posy+HUMAN_HEIGHT, posz+3.0f, posx+3.0f, posy+HUMAN_HEIGHT, posz-3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx+3.0f, posy+HUMAN_HEIGHT, posz-3.0f, posx-3.0f, posy+HUMAN_HEIGHT, posz-3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx-3.0f, posy+HUMAN_HEIGHT, posz-3.0f, posx-3.0f, posy+HUMAN_HEIGHT, posz+3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx-3.0f, posy+HUMAN_HEIGHT, posz+3.0f, posx+3.0f, posy+HUMAN_HEIGHT, posz+3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx+3.0f, posy, posz+3.0f, posx+3.0f, posy+HUMAN_HEIGHT, posz+3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx+3.0f, posy, posz-3.0f, posx+3.0f, posy+HUMAN_HEIGHT, posz-3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx-3.0f, posy, posz-3.0f, posx-3.0f, posy+HUMAN_HEIGHT, posz-3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
				d3dg->Drawline(posx-3.0f, posy, posz+3.0f, posx-3.0f, posy+HUMAN_HEIGHT, posz+3.0f, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
			}
		}
	}
}

void maingame::Render2D()
{
	char str[256];
	unsigned char stru[256];
	float fps = GetFps(10);
	float effect;

	human *myHuman = ObjMgr.GetPlayerHumanObject();

	int selectweapon;
	weapon *weapon[TOTAL_HAVEWEAPON];
	int weapon_paramid[TOTAL_HAVEWEAPON];
	WeaponParameter weapon_paramdata;
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
		weapon_paramid[i] = 0;
	}
	int lnbs = 0;
	int nbs = 0;
	int reloadcnt = 0;
	int selectweaponcnt = 0;
	int weaponmodel, weapontexture;
	char weaponname[16];
	int hp;
	int param_scopemode;
	int param_WeaponP;
	int ErrorRange;

	//各種設定やゲーム情報を取得
	myHuman->GetWeapon(&selectweapon, weapon);
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		if( weapon[i] != NULL ){
			if( selectweapon == i ){
				weapon[i]->GetParamData(&weapon_paramid[i], &lnbs, &nbs);
				reloadcnt = weapon[i]->GetReloadCnt();
			}
			else{
				weapon[i]->GetParamData(&weapon_paramid[i], NULL, NULL);
			}
		}
	}
	selectweaponcnt = myHuman->GetChangeWeaponCnt();
	GameParamInfo.GetWeapon(weapon_paramid[selectweapon], &weapon_paramdata);
	strcpy(weaponname, weapon_paramdata.name);
	hp = myHuman->GetHP();
	param_scopemode = weapon_paramdata.scopemode;
	param_WeaponP = weapon_paramdata.WeaponP;
	ErrorRange = myHuman->GetGunsightErrorRange();

	float human_x, human_y, human_z, human_rx;
	myHuman->GetPosData(&human_x, &human_y, &human_z, &human_rx);

	//レッドフラッシュ描画
	if( redflash_flag == true ){
		d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
		redflash_flag = false;
	}

	//スコープ描画
	if( (Camera_F1mode == false)&&(myHuman->GetScopeMode() == 1) ){
		d3dg->Draw2DTexture(0, 0, Resource.GetScopeTexture(), SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
		d3dg->Draw2DLine(SCREEN_WIDTH/2-49, SCREEN_HEIGHT/2, SCREEN_WIDTH/2-4, SCREEN_HEIGHT/2, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH/2+4, SCREEN_HEIGHT/2, SCREEN_WIDTH/2+49, SCREEN_HEIGHT/2, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2-49, SCREEN_WIDTH/2, SCREEN_HEIGHT/2-4, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2+4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2+49, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DBox(SCREEN_WIDTH/2-50, SCREEN_HEIGHT/2-1, SCREEN_WIDTH/2+50, SCREEN_HEIGHT/2+1, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
		d3dg->Draw2DBox(SCREEN_WIDTH/2-1, SCREEN_HEIGHT/2-50, SCREEN_WIDTH/2+1, SCREEN_HEIGHT/2+50, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
	}
	if( (Camera_F1mode == false)&&(myHuman->GetScopeMode() == 2) ){
		d3dg->Draw2DTexture(0, 0, Resource.GetScopeTexture(), SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
		d3dg->Draw2DLine(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DBox(0, SCREEN_HEIGHT/2-1, SCREEN_WIDTH, SCREEN_HEIGHT/2+1, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
		d3dg->Draw2DBox(SCREEN_WIDTH/2-1, 0, SCREEN_WIDTH/2+1, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
	}

	//目隠し描画
	if( (Camera_Blind == true)&&(Camera_Debugmode == false)&&(hp > 0) ){

		int scopemode = myHuman->GetScopeMode();
		float addang;
		float adddist = 1.2f;

		//スコープによる視野角を決定
		if( scopemode == 0 ){ addang = VIEWANGLE_NORMAL / 4; }
		if( scopemode == 1 ){ addang = VIEWANGLE_SCOPE_1 / 4; }
		if( scopemode == 2 ){ addang = VIEWANGLE_SCOPE_2 / 4; }

		//上
		if( CollD.CheckALLBlockInside(camera_x + cos(camera_rx)*cos(camera_ry + addang) * adddist, camera_y + sin(camera_ry + addang) * adddist, camera_z + sin(camera_rx)*cos(camera_ry + addang) * adddist) == true ){
			d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/2, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		}

		//下
		if( CollD.CheckALLBlockInside(camera_x + cos(camera_rx)*cos(camera_ry - addang) * adddist, camera_y + sin(camera_ry - addang) * adddist, camera_z + sin(camera_rx)*cos(camera_ry - addang) * adddist) == true ){
			d3dg->Draw2DBox(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		}

		//左
		if( CollD.CheckALLBlockInside(camera_x + cos(camera_rx + addang)*cos(camera_ry) * adddist, camera_y + sin(camera_ry) * adddist, camera_z + sin(camera_rx + addang)*cos(camera_ry) * adddist) == true ){
			d3dg->Draw2DBox(0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		}

		//右
		if( CollD.CheckALLBlockInside(camera_x + cos(camera_rx - addang)*cos(camera_ry) * adddist, camera_y + sin(camera_ry) * adddist, camera_z + sin(camera_rx - addang)*cos(camera_ry) * adddist) == true ){
			d3dg->Draw2DBox(SCREEN_WIDTH/2, 0, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		}
	}

#ifdef ENABLE_DEBUGCONSOLE
	//デバック用・ゲーム情報の表示
	if( (ShowInfo_Debugmode == true)||(Camera_Debugmode == true) ){
		float move_x, move_y, move_z;
		myHuman->GetMovePos(&move_x, &move_y, &move_z);

		//テクスチャフォントによる表示（半角英数字と記号のみ）
		sprintf(str, "frame:%d   time %02d:%02d", framecnt, framecnt/(int)GAMEFPS/60, framecnt/(int)GAMEFPS%60);
		d3dg->Draw2DTextureDebugFontText(10+1, 10+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 10, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
		sprintf(str, "camera x:%.2f y:%.2f z:%.2f rx:%.2f ry:%.2f", camera_x, camera_y, camera_z, camera_rx, camera_ry);
		d3dg->Draw2DTextureDebugFontText(10+1, 30+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 30, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
		sprintf(str, "human[%d] x:%.2f y:%.2f z:%.2f rx:%.2f HP:%d", ObjMgr.GetPlayerID(), human_x, human_y, human_z, human_rx, hp);
		d3dg->Draw2DTextureDebugFontText(10+1, 50+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 50, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
		sprintf(str, "          move_x:%.2f move_y:%.2f move_z:%.2f", move_x, move_y, move_z);
		d3dg->Draw2DTextureDebugFontText(10+1, 70+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 70, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
		sprintf(str, "Input:%02dms Object:%02dms AI:%02dms Event:%02dms Sound:%02dms Render:%02dms", time_input, time_process_object, time_process_ai, time_process_event, time_sound, time_render);
		d3dg->Draw2DTextureDebugFontText(10+1, 90+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 90, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
	}
#endif

	//ゲーム実行速度の表示
	//int speed = (int)(fps / (1000.0f/GAMEFRAMEMS) * 100);
	//sprintf(str, "PROCESSING SPEED %d%%", speed);
	sprintf(str, "fps:%.2f", fps);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH - strlen(str)*14 - 14 +1, 10+1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 14, 18);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH - strlen(str)*14 - 14, 10, str, d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f), 14, 18);

	//HUD表示・モードA
	if( Camera_F2mode == 0 ){
		//左下エリア描画
		//"ｳｴｴｴｴｴｴｵ"
		stru[0] = 0xB3;		stru[1] = 0xB4;		stru[2] = 0xB4;		stru[3] = 0xB4;		stru[4] = 0xB4;
		stru[5] = 0xB4;		stru[6] = 0xB4;		stru[7] = 0xB5;		stru[8] = '\0';
		d3dg->Draw2DTextureFontText(15, SCREEN_HEIGHT - 105, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		//"ﾃﾄﾄﾄﾄﾄﾄﾅ"
		for(int i=0; stru[i] != 0x00; i++){ stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(15, SCREEN_HEIGHT - 105 +32, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		//"ｳｴｴｶｷｷｷｸｹ"
		stru[0] = 0xB3;		stru[1] = 0xB4;		stru[2] = 0xB4;		stru[3] = 0xB6;		stru[4] = 0xB7;
		stru[5] = 0xB7;		stru[6] = 0xB7;		stru[7] = 0xB8;		stru[8] = 0xB9;		stru[9] = '\0';
		d3dg->Draw2DTextureFontText(15, SCREEN_HEIGHT - 55, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		//"ﾃﾄﾄﾆﾇﾇﾇﾈﾉ"
		for(int i=0; stru[i] != 0x00; i++){ stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(15, SCREEN_HEIGHT - 55 +32, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);

		//右下エリア用文字コード設定
		stru[0] = 0xB0;//'ｰ';
		for(int i=1; i<HUDA_WEAPON_SIZEW-1; i++){
			stru[i] = 0xB1;//'ｱ';
		}
		stru[HUDA_WEAPON_SIZEW-1] = 0xB2;//'ｲ';
		stru[HUDA_WEAPON_SIZEW] = '\0';

		//右下エリア描画
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		for(int i=0; i<HUDA_WEAPON_SIZEW; i++){ stru[i] += 0x10; }
		for(int i=1; i<HUDA_WEAPON_SIZEH-1; i++){
			d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY + 32*i, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		}
		for(int i=0; i<HUDA_WEAPON_SIZEW; i++){ stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY + 32*(HUDA_WEAPON_SIZEH-1), (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);

		//武器の弾数表示
		sprintf((char*)stru, "A%d B%d", lnbs, (nbs - lnbs));
		for(int i=0; i<(int)strlen((char*)stru); i++){
			if( stru[i] == 'A' ){ stru[i] = 0xBB; }	//'ｻ'
			if( stru[i] == 'B' ){ stru[i] = 0xBA; }	//'ｺ'
		}
		d3dg->Draw2DTextureFontText(25, SCREEN_HEIGHT - 96, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 23, 24);

		//HPによる色の決定
		int statecolor;
		if( hp >= 100 ){
			statecolor = d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f);
		}
		else if( hp >= 50 ){
			statecolor = d3dg->GetColorCode(1.0f/50*(100-hp),1.0f,0.0f,1.0f);
		}
		else if( hp > 0 ){
			statecolor = d3dg->GetColorCode(1.0f,1.0f/50*hp,0.0f,1.0f);
		}
		else{
			statecolor = d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f);
		}

		//HP表示
		if( hp >= 80 ){
			d3dg->Draw2DTextureFontText(23, SCREEN_HEIGHT - 45, "STATE", statecolor, 18, 24);
			d3dg->Draw2DTextureFontText(155, SCREEN_HEIGHT - 45, "FINE", statecolor, 18, 24);
		}
		else if( hp >= 40 ){
			d3dg->Draw2DTextureFontText(23, SCREEN_HEIGHT - 45, "STATE", statecolor, 18, 24);
			d3dg->Draw2DTextureFontText(135, SCREEN_HEIGHT - 45, "CAUTION", statecolor, 18, 24);
		}
		else if( hp > 0 ){
			d3dg->Draw2DTextureFontText(23, SCREEN_HEIGHT - 45, "STATE", statecolor, 18, 24);
			d3dg->Draw2DTextureFontText(140, SCREEN_HEIGHT - 45, "DANGER", statecolor, 18, 24);
		}
		else{
			d3dg->Draw2DTextureFontText(23, SCREEN_HEIGHT - 45, "STATE", statecolor, 18, 24);
			d3dg->Draw2DTextureFontText(155, SCREEN_HEIGHT - 45, "DEAD", statecolor, 18, 24);
		}

		//武器名表示
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX + 9, HUDA_WEAPON_POSY + 4, weaponname, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 16, 20);
	}

	//HUD表示・モードB
	if( Camera_F2mode == 1 ){
		//画面周りの線
		d3dg->Draw2DLine(0, 0, SCREEN_WIDTH-1, 0, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH-1, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));
		d3dg->Draw2DLine(0, 0, 0, SCREEN_HEIGHT-1, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));
		d3dg->Draw2DLine(0, SCREEN_HEIGHT-1, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));

		//武器名表示
		d3dg->Draw2DBox(8, SCREEN_HEIGHT - 32, 227, SCREEN_HEIGHT - 7, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.3f));
		d3dg->Draw2DTextureFontText(10, SCREEN_HEIGHT - 30, weaponname, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 16, 20);
	}

	//レーダー描画
	if( radar == true ){
		RenderRadar();
	}

	//イベントメッセージ表示
	if( (message_id != -1)&&(message_cnt < (int)(TOTAL_EVENTENT_SHOWMESSEC*GAMEFPS)) ){
		char messtr[MAX_POINTMESSAGEBYTE];
		PointData.GetMessageText(messtr, message_id);
		float effectA = 1.0f;
		if( message_cnt < (int)(0.2f*GAMEFPS) ){ effectA = GetEffectAlpha(message_cnt, 1.0f, 0.2f, 0.0f, false); }
		if( (int)((TOTAL_EVENTENT_SHOWMESSEC-0.2f)*GAMEFPS) < message_cnt ){ effectA = GetEffectAlpha(message_cnt, 1.0f, 0.2f, (TOTAL_EVENTENT_SHOWMESSEC - 0.2f), true); }
		d3dg->Draw2DMSFontTextCenter(0 +1, SCREEN_HEIGHT - 140 +1, SCREEN_WIDTH, 140, messtr, d3dg->GetColorCode(0.1f,0.1f,0.1f,effectA));
		d3dg->Draw2DMSFontTextCenter(0, SCREEN_HEIGHT - 140, SCREEN_WIDTH, 140, messtr, d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA));
	}

#ifdef ENABLE_DEBUGCONSOLE
	//デバック用・ゲーム情報の表示
	if( ShowInfo_Debugmode == true ){
		ObjMgr.RenderLog(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY-60);
	}
#endif

	//リロード表示
	if( reloadcnt > 0 ){
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 145 +3, SCREEN_HEIGHT - 180+3, "RELOADING", d3dg->GetColorCode(0.2f,0.2f,0.2f,1.0f), 32, 34);
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 145, SCREEN_HEIGHT - 180, "RELOADING", d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 32, 34);
	}

	//武器切り替え表示
	if( selectweaponcnt > 0 ){
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 130 +3, SCREEN_HEIGHT - 180+3, "CHANGING", d3dg->GetColorCode(0.2f,0.2f,0.2f,1.0f), 32, 34);
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT - 180, "CHANGING", d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 32, 34);
	}

	//照準表示
	if( (Camera_F1mode == false)&&(param_WeaponP != 2) ){
		if( (weapon[selectweapon] != NULL)&&(myHuman->GetScopeMode() == 0)&&(param_scopemode != 2) ){
			if( (end_framecnt == 0)||(GameInfoData.missioncomplete == true) ){
				if( GameConfig.GetAnotherGunsightFlag() ){	//オプション型
					//照準の透明度
					float alpha = 1.0f - (float)ErrorRange/40.0f;
					if( alpha < 0.0f ){ alpha = 0.0f; }

					d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2+4, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2-15, SCREEN_HEIGHT/2+15, SCREEN_WIDTH/2-19, SCREEN_HEIGHT/2+19, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2+15, SCREEN_HEIGHT/2+15, SCREEN_WIDTH/2+19, SCREEN_HEIGHT/2+19, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2-4, SCREEN_HEIGHT/2+4, SCREEN_WIDTH/2+4, SCREEN_HEIGHT/2+4, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));

					d3dg->Draw2DLine(SCREEN_WIDTH/2-4 - ErrorRange, SCREEN_HEIGHT/2-4 - ErrorRange/2, SCREEN_WIDTH/2-4 - ErrorRange, SCREEN_HEIGHT/2+4 + ErrorRange/2, d3dg->GetColorCode(1.0f,0.0f,0.0f,alpha));
					d3dg->Draw2DLine(SCREEN_WIDTH/2+4 + ErrorRange, SCREEN_HEIGHT/2-4 - ErrorRange/2, SCREEN_WIDTH/2+4 + ErrorRange, SCREEN_HEIGHT/2+4 + ErrorRange/2, d3dg->GetColorCode(1.0f,0.0f,0.0f,alpha));
				}
				else{										//標準型
					d3dg->Draw2DLine(SCREEN_WIDTH/2-13, SCREEN_HEIGHT/2, SCREEN_WIDTH/2-3, SCREEN_HEIGHT/2, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2+13, SCREEN_HEIGHT/2, SCREEN_WIDTH/2+3, SCREEN_HEIGHT/2, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2-13, SCREEN_WIDTH/2, SCREEN_HEIGHT/2-3, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2+13, SCREEN_WIDTH/2, SCREEN_HEIGHT/2+3, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));

					stru[0] = 0xBD;		stru[1] = '\0';	//"ｽ"
					d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 16 - ErrorRange, SCREEN_HEIGHT/2 - 16, (char*)stru, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f), 32, 32);
					stru[0] = 0xBE;		stru[1] = '\0';	//"ｾ"
					d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 16 + ErrorRange, SCREEN_HEIGHT/2 - 16, (char*)stru, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f), 32, 32);
				}
			}
		}
	}

	if( tag == true ){
		int color;
		if( ObjMgr.GetObjectInfoTag(camera_x, camera_y, camera_z, camera_rx, camera_ry, &color, str) == true ){
			d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(str)*14/2 + 1, SCREEN_HEIGHT/2 + 30 +1 , str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 14, 18);
			d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(str)*14/2, SCREEN_HEIGHT/2 + 30, str, color, 14, 18);
		}
	}

#ifdef ENABLE_DEBUGCONSOLE
	//AIデバック情報表示
	if( AIdebuginfoID != -1 ){
		if( (0 <= AIdebuginfoID)&&(AIdebuginfoID < MAX_HUMAN) ){
			float posx, posy, posz, rx;
			int hp;
			char modestr[32];
			int EnemyID;
			float mposx, mposz;
			int movemode;
			pointdata ppdata;
			ObjMgr.GeHumanObject(AIdebuginfoID)->GetPosData(&posx, &posy, &posz, &rx);
			hp = ObjMgr.GeHumanObject(AIdebuginfoID)->GetHP();
			HumanAI[AIdebuginfoID].GetBattleMode(NULL, modestr);
			EnemyID = HumanAI[AIdebuginfoID].GetEnemyHumanID();
			HumanAI[AIdebuginfoID].GetMoveTargetPos(&mposx, &mposz, &movemode);
			HumanAI[AIdebuginfoID].GetPathPointData(&ppdata);

			sprintf(str, "AI debug info [ID:%d]", AIdebuginfoID);
			d3dg->Draw2DTextureDebugFontText(20 +1, 130 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 130, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			sprintf(str, "(X:%.2f Y:%.2f Z:%.2f RX:%.2f HP:%d)", posx, posy, posz, rx, hp);
			d3dg->Draw2DTextureDebugFontText(20 +1, 150 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 150, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			sprintf(str, "Mode:%s  TargetEnemyID:%d", modestr, EnemyID);
			d3dg->Draw2DTextureDebugFontText(20 +1, 170 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 170, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			sprintf(str, "PointPath:[%d][%d][%d][%d]", ppdata.p1, ppdata.p2, ppdata.p3, ppdata.p4);
			d3dg->Draw2DTextureDebugFontText(20 +1, 190 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 190, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			sprintf(str, "MovePosX:%.2f  MovePosZ:%.2f", mposx, mposz);
			d3dg->Draw2DTextureDebugFontText(20 +1, 210 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 210, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
		}
	}
#endif


	//-----------------------------------

	//Zバッファを初期化
	d3dg->ResetZbuffer();


	//HUD表示・モードA
	if( Camera_F2mode == 0 ){
		int notselectweapon = selectweapon + 1;
		if( notselectweapon == TOTAL_HAVEWEAPON ){ notselectweapon = 0; }

		//（3D描画）カメラ座標を暫定設定
		d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);

		//（3D描画）所持している武器モデルの描画・メイン武器
		GameParamInfo.GetWeapon(weapon_paramid[selectweapon], &weapon_paramdata);
		Resource.GetWeaponModelTexture(weapon_paramid[selectweapon], &weaponmodel, &weapontexture);
		d3dg->SetWorldTransformPlayerWeapon(true, camera_rx, camera_ry, DegreeToRadian(framecnt*2), weapon_paramdata.size);
		d3dg->RenderModel(weaponmodel, weapontexture, false);

		//（3D描画）所持している武器モデルの描画・サブ武器
		GameParamInfo.GetWeapon(weapon_paramid[notselectweapon], &weapon_paramdata);
		Resource.GetWeaponModelTexture(weapon_paramid[notselectweapon], &weaponmodel, &weapontexture);
		d3dg->SetWorldTransformPlayerWeapon(false, camera_rx, camera_ry, 0.0f, weapon_paramdata.size);
		d3dg->RenderModel(weaponmodel, weapontexture, false);
	}

	//-----------------------------------

#ifdef ENABLE_DEBUGCONSOLE
	if( Show_Console == true ){
		if( ScreenShot == 1 ){
			ScreenShot = 2;
		}
		else{
			RenderConsole();
		}
	}
#endif

	//-----------------------------------


	//スタート時と終了時のブラックアウト設定
	if( start_framecnt < (int)(1.0f*GAMEFPS) ){
		effect = GetEffectAlpha(start_framecnt, 1.0f, 1.0f, 0.0f, true);
	}
	else if( end_framecnt > 0 ){
		effect = GetEffectAlpha(end_framecnt, 1.0f, 4.0f, 0.0f, false);
	}
	else{
		effect = 0.0f;
	}
	d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,effect));

	//終了時の文字表示
	if( end_framecnt > 0 ){
		if( end_framecnt < (int)(1.0f*GAMEFPS) ){
			effect = GetEffectAlpha(end_framecnt, 1.0f, 1.0f, 0.0f, false);
		}
		else if( end_framecnt > (int)(4.0f*GAMEFPS) ){
			effect = GetEffectAlpha(end_framecnt, 1.0f, 1.0f, 4.0f, true);
		}
		else{
			effect = 1.0f;
		}

		if( GameInfoData.missioncomplete == true ){
			d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 252, SCREEN_HEIGHT/2 + 10, "objective complete", d3dg->GetColorCode(1.0f,0.5f,0.0f,effect), 28, 32);
		}
		else{
			d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 210, SCREEN_HEIGHT/2 + 10, "mission failure", d3dg->GetColorCode(1.0f,0.0f,0.0f,effect), 28, 32);
		}
	}


	time_render = GetTimeMS() - time;
}

//! @brief レーダーに描画する座標に変換
//! @param in_x 空間 X座標
//! @param in_y 空間 Y座標
//! @param in_z 空間 Z座標
//! @param RadarPosX レーダーの描画 X座標（左上基準）
//! @param RadarPosY レーダーの描画 Y座標（左上基準）
//! @param RadarSize レーダーの描画サイズ
//! @param RadarWorldR レーダーにポイントする距離
//! @param out_x 2D X座標 を受け取るポインタ
//! @param out_y 2D X座標 を受け取るポインタ
//! @param local_y ローカル Y座標 を受け取るポインタ（NULL可）
//! @param check 計算の省略
//! @return 成功：true　失敗：false
//! @attention checkフラグを有効にすると、レーダーから外れることが明らかになった時点で計算を終了し、falseを返します。
bool maingame::GetRadarPos(float in_x, float in_y, float in_z, int RadarPosX, int RadarPosY, int RadarSize, float RadarWorldR, int *out_x, int *out_y, float *local_y, bool check)
{
	bool outf = false;
	float x, y, z, r, rx;
	float x2, z2, r2, rx2;

	//カメラとの距離を計算
	x = in_x - camera_x;
	y = in_y - camera_y;
	z = in_z - camera_z;

	//近ければ処理する
	if( (check == false) || ((fabs(x) < RadarWorldR*2)&&(fabs(z) < RadarWorldR*2)&&(fabs(y) < 80.0f)) ){
		//角度を距離を計算
		rx = atan2(z, x);
		r = sqrt(x*x + z*z);

		//カメラ基準の座標を再計算
		rx2 = (rx - camera_rx)*-1 - (float)M_PI/2;
		x2 = cos(rx2) * r;
		z2 = sin(rx2) * r;

		//収まるか判定
		if( (check == false) || ((fabs(x2) < RadarWorldR)&&(fabs(z2) < RadarWorldR)) ){
			//描画座標を計算
			r2 = r / RadarWorldR * (RadarSize/2);
			*out_x = (int)(RadarPosX+RadarSize/2 + cos(rx2) * r2);
			*out_y = (int)(RadarPosY+RadarSize/2 + sin(rx2) * r2);
			if( local_y != NULL ){ *local_y = y; }
			outf = true;
		}
	}

	return outf;
}

//! @brief 簡易レーダー表示
void maingame::RenderRadar()
{
	int RadarSize = 200;							//レーダーの描画サイズ
	int RadarPosX = SCREEN_WIDTH - RadarSize - 10;	//レーダーの描画 X座標（左上基準）
	int RadarPosY = 110;							//レーダーの描画 Y座標（左上基準）
	float RadarWorldR = 300.0f;						//レーダーにポイントする距離

	float ecr = DISTANCE_CHECKPOINT / RadarWorldR * (RadarSize/2);

	//下地と囲い
	d3dg->Draw2DBox(RadarPosX, RadarPosY, RadarPosX+RadarSize, RadarPosY+RadarSize, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.6f));
	d3dg->Draw2DLine(RadarPosX, RadarPosY, RadarPosX+RadarSize, RadarPosY, d3dg->GetColorCode(0.0f,0.8f,0.0f,1.0f));
	d3dg->Draw2DLine(RadarPosX+RadarSize, RadarPosY, RadarPosX+RadarSize, RadarPosY+RadarSize, d3dg->GetColorCode(0.0f,0.8f,0.0f,1.0f));
	d3dg->Draw2DLine(RadarPosX+RadarSize, RadarPosY+RadarSize, RadarPosX, RadarPosY+RadarSize, d3dg->GetColorCode(0.0f,0.8f,0.0f,1.0f));
	d3dg->Draw2DLine(RadarPosX, RadarPosY+RadarSize, RadarPosX, RadarPosY, d3dg->GetColorCode(0.0f,0.8f,0.0f,1.0f));

	//マップを描画
	int bs = BlockData.GetTotaldatas();
	for(int i=0; i< bs; i++){
		blockdata bdata;
		float x_min, y_min, z_min, x_max, y_max, z_max;
		int vid[4];
		int bvx[4], bvy[4];

		//ブロックのデータを取得
		BlockData.Getdata(&bdata, i);

		//描画候補のブロックを検出（荒削り）
		CollD.GetBlockPosMINMAX(i, &x_min, &y_min, &z_min, &x_max, &y_max, &z_max);
		if( CollideBoxAABB(x_min, y_min, z_min, x_max, y_max, z_max, camera_x-RadarWorldR*2, camera_y-1.0f, camera_z-RadarWorldR*2, camera_x+RadarWorldR*2, camera_y+1.0f, camera_z+RadarWorldR*2) == true ){

			//各面ごとに処理する
			for(int j=0; j<6; j++){
				//登れない斜面か判定　※地面や階段などの傾斜を除外する
				float angle = acos(bdata.material[j].vy);
				if( (HUMAN_MAPCOLLISION_SLOPEANGLE < angle)&&(angle < DegreeToRadian(120)) ){

					//ブロック頂点データの関連付けを取得
					blockdataface(j, &(vid[0]), NULL);
					
					//4頂点を計算
					for(int k=0; k<4; k++){
						GetRadarPos(bdata.x[ vid[k] ], bdata.y[ vid[k] ], bdata.z[ vid[k] ], RadarPosX, RadarPosY, RadarSize, RadarWorldR, &(bvx[k]), &(bvy[k]), NULL, false);
					}

					int line_x1, line_y1, line_x2, line_y2;

					//レーダーの四角形に収まるように描画する
					if( Get2DLineInBox(bvx[0], bvy[0], bvx[1], bvy[1], RadarPosX, RadarPosY, RadarPosX+RadarSize, RadarPosY+RadarSize, &line_x1, &line_y1, &line_x2, &line_y2) == true ){
						d3dg->Draw2DLine(line_x1, line_y1, line_x2, line_y2, d3dg->GetColorCode(0.8f,0.8f,0.8f,1.0f));
					}
					if( Get2DLineInBox(bvx[1], bvy[1], bvx[2], bvy[2], RadarPosX, RadarPosY, RadarPosX+RadarSize, RadarPosY+RadarSize, &line_x1, &line_y1, &line_x2, &line_y2) == true ){
						d3dg->Draw2DLine(line_x1, line_y1, line_x2, line_y2, d3dg->GetColorCode(0.8f,0.8f,0.8f,1.0f));
					}
					if( Get2DLineInBox(bvx[2], bvy[2], bvx[3], bvy[3], RadarPosX, RadarPosY, RadarPosX+RadarSize, RadarPosY+RadarSize, &line_x1, &line_y1, &line_x2, &line_y2) == true ){
						d3dg->Draw2DLine(line_x1, line_y1, line_x2, line_y2, d3dg->GetColorCode(0.8f,0.8f,0.8f,1.0f));
					}
					if( Get2DLineInBox(bvx[3], bvy[3], bvx[0], bvy[0], RadarPosX, RadarPosY, RadarPosX+RadarSize, RadarPosY+RadarSize, &line_x1, &line_y1, &line_x2, &line_y2) == true ){
						d3dg->Draw2DLine(line_x1, line_y1, line_x2, line_y2, d3dg->GetColorCode(0.8f,0.8f,0.8f,1.0f));
					}
				}
			}
		}
	}

	//イベントの到着ポイントを表示
	if( EventStop == false ){
		for(int i=0; i<TOTAL_EVENTLINE; i++){
			signed char p4 = Event[i].GetNextP4();
			pointdata data;
			if( PointData.SearchPointdata(&data, 0x08, 0, 0, 0, p4, 0) != 0 ){
				float y;
				int x_2d, y_2d;
				float alpha;

				if( (data.p1 == 13)||(data.p1 == 16) ){
					data.y += VIEW_HEIGHT;

					if( GetRadarPos(data.x, data.y, data.z, RadarPosX, RadarPosY, RadarSize, RadarWorldR, &x_2d, &y_2d, &y, true) == true ){
						//高さによる透明度
						if( (fabs(y) < 40.0f) ){
							alpha = 1.0f;
						}
						else{
							alpha = 0.5f;
						}

						//マーカー描画
						d3dg->Draw2DCycle(x_2d, y_2d, (int)ecr, d3dg->GetColorCode(1.0f,0.5f,0.0f,alpha));
					}
				}
			}
		}
	}

	//プレイヤーの情報を取得
	int PlayerID = ObjMgr.GetPlayerID();
	int myteamid;
	ObjMgr.GeHumanObject(PlayerID)->GetParamData(NULL, NULL, NULL, &myteamid);

	//人を描画
	for(int i=0; i<MAX_HUMAN; i++){
		human* thuman;
		float tx, ty, tz;
		int tteamid;
		float y;
		int x_2d, y_2d;
		float alpha;
		int color;

		//人のオブジェクトを取得
		thuman = ObjMgr.GeHumanObject(i);

		//使われていない人や死体は無視する
		if( thuman->GetEnableFlag() == false ){ continue; }
		if( thuman->GetDeadFlag() == true ){ continue; }

		//人の情報を取得
		thuman->GetPosData(&tx, &ty, &tz, NULL);
		thuman->GetParamData(NULL, NULL, NULL, &tteamid);
		ty += VIEW_HEIGHT;

		if( GetRadarPos(tx, ty, tz, RadarPosX, RadarPosY, RadarSize, RadarWorldR, &x_2d, &y_2d, &y, true) == true ){
			//高さによる透明度
			if( (fabs(y) < 40.0f) ){
				alpha = 1.0f;
			}
			else{
				alpha = 0.5f;
			}

			//マーカーの色を決定
			if( PlayerID == i ){ color = d3dg->GetColorCode(1.0f,1.0f,0.0f,alpha); }				//プレイヤー自身
			else if( tteamid == myteamid ){ color = d3dg->GetColorCode(0.0f,0.5f,1.0f,alpha); }	//味方
			else{ color = d3dg->GetColorCode(1.0f,0.0f,0.5f,alpha); }								//敵

			//マーカー描画
			d3dg->Draw2DBox(x_2d-3, y_2d-3, x_2d+3, y_2d+3, color);
		}
	}
}

#ifdef ENABLE_DEBUGCONSOLE

//! @brief デバック用コンソールに新たな文字列を追加
//! @param color 文字の色
//! @param str 追加する文字列のポインタ
//! @attention 新しい文字列は常に下から追加されます。
//! @attention 表示可能行数（定数：MAX_CONSOLELINES）を上回る場合、最初の行（1行目）を削除し1行ずつずらした上で、一番下の行に追加します。
void maingame::AddInfoConsole(int color, char *str)
{
	for(int i=0; i<MAX_CONSOLELINES; i++){
		if( InfoConsoleData[i].textdata[0] == NULL ){
			InfoConsoleData[i].color = color;
			strcpy(InfoConsoleData[i].textdata, str);
			return;
		}
	}

	for(int i=1; i<MAX_CONSOLELINES; i++){
		memcpy(&(InfoConsoleData[i-1]), &(InfoConsoleData[i]), sizeof(ConsoleData));
	}
	InfoConsoleData[MAX_CONSOLELINES-1].color = color;
	strcpy(InfoConsoleData[MAX_CONSOLELINES-1].textdata, str);
}

//! @brief 入力用コンソールに文字を一文字追加
//! @param inchar 追加する文字
void maingame::ConsoleInputText(char inchar)
{
	for(int i=0; i<MAX_CONSOLELEN; i++){
		if( InputConsoleData->textdata[i] == '\0' ){
			InputConsoleData->textdata[i] = inchar;
			InputConsoleData->textdata[i+1] = '\0';
			return;
		}
	}
}

//! @brief 入力用コンソールの文字を一文字削除
void maingame::ConsoleDeleteText()
{
	int s = strlen(InputConsoleData->textdata);
	if( s == (int)strlen(CONSOLE_PROMPT) ){ return; }
	InputConsoleData->textdata[ s-1 ] = '\0';
}

//! @brief コマンドの判定および引数（整数値）を取得
//! @param cmd 判定するコマンド文字のポインタ
//! @param num 与えられた引数を受け取るポインタ
//! @return 取得：true　判定外：false
bool maingame::GetCommandNum(char *cmd, int *num)
{
	char str[MAX_CONSOLELEN];

	//コマンド名を調べる
	strcpy(str, NewCommand);
	str[ strlen(cmd) ] = '\0';
	if( strcmp(str, cmd) != 0 ){ return false; }

	//「コマンド名_X」分の文字数に達しているかどうか
	if( strlen(cmd)+2 > strlen(NewCommand) ){ return false; }

	//数字が与えられているか
	for(int i=strlen(cmd)+1; NewCommand[i] != '\0'; i++){
		if( ((NewCommand[i] < '0')||('9' < NewCommand[i]))&&(NewCommand[i] != '+')&&(NewCommand[i] != '-') ){ return false; }
	}

	//与えられた数字を調べる
	*num = atoi(&(NewCommand[ strlen(cmd)+1 ]));
	return true;
}

//! @brief デバック用コンソールの入力処理
void maingame::InputConsole()
{
	NewCommand[0] = '\0';

	if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F)) ){		// [ENTER]
		//何か入力されていれば～
		if( strcmp(InputConsoleData->textdata, CONSOLE_PROMPT) != 0 ){
			//コンソールに追加し、未処理コマンドとして登録
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), InputConsoleData->textdata);
			strcpy(NewCommand, &(InputConsoleData->textdata[strlen(CONSOLE_PROMPT)]));
		}

		//入力エリアを初期化
		strcpy(InputConsoleData->textdata, CONSOLE_PROMPT);
	}
	else{
		//数字
		for(char key=0x16; key<=0x1F; key++){
			if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
				ConsoleInputText(key + 0x1A);
			}
		}
		for(char key=0x04; key<=0x0D; key++){
			if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
				ConsoleInputText(key + 0x2C);
			}
		}

		//アルファベット小文字
		for(char key=0x20; key<=0x39; key++){
			if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
				ConsoleInputText(key + 0x41);
			}
		}

		//スペース
		if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x11)) ){		// [SPACE]
			ConsoleInputText(' ');
		}

		//一文字削除
		if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0E)) ){		// [BACK]
			ConsoleDeleteText();
		}
	}
}

//! @brief デバック用コンソールのメイン処理
void maingame::ProcessConsole()
{
	char str[MAX_CONSOLELEN];
	int id;

	//コマンドリスト
	if( strcmp(NewCommand, "help") == 0 ){
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "help          human         result         event        ver");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "mif           bd1           pd1");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "info          view          center         map          aiinfo <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "tag           radar         inmap          sky <NUM>    dark");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "ff            revive        kill <NUM>     treat <NUM>  nodamage <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "break <NUM>   newobj <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "bot           nofight       caution        stop         estop      speed");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "window        ss            clear          exit");
	}

	//人の統計情報
	if( strcmp(NewCommand, "human") == 0 ){
		int alivemyfriend = 0;
		int myfriend = 0;
		int aliveenemy = 0;
		int enemy = 0;
		int myteamid;

		//プレイヤーのチーム番号を取得
		ObjMgr.GetPlayerHumanObject()->GetParamData(NULL, NULL, NULL, &myteamid);

		for(int i=0; i<MAX_HUMAN; i++){
			int teamid;
			bool deadflag;
			human *thuman = ObjMgr.GeHumanObject(i);
			if( thuman->GetEnableFlag() == true ){
				//死亡状態とチーム番号を取得
				deadflag = thuman->GetDeadFlag();
				thuman->GetParamData(NULL, NULL, NULL, &teamid);

				//カウント
				if( teamid == myteamid ){
					myfriend += 1;
					if( deadflag == false ){ alivemyfriend += 1; }
				}
				else{
					enemy += 1;
					if( deadflag == false ){ aliveenemy += 1; }
				}
			}
		}

		sprintf(str, "Friend:%d/%d  Enemy:%d/%d  Total:%d/%d",
			alivemyfriend, myfriend, aliveenemy, enemy, alivemyfriend + aliveenemy, myfriend + enemy);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//暫定リザルト表示
	if( strcmp(NewCommand, "result") == 0 ){
		float rate;
		if( MainGameInfo.fire == 0 ){
			rate = 0.0f;
		}
		else{
			rate = (float)MainGameInfo.ontarget / MainGameInfo.fire * 100;
		}

		sprintf(str, "Time %02d:%02d  /  Fired %d  /  On target %d", framecnt/(int)GAMEFPS/60, framecnt/(int)GAMEFPS%60, MainGameInfo.fire, MainGameInfo.ontarget);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, "AR rate %.1f%%  /  Kill %d  /  HS %d", rate, MainGameInfo.kill, MainGameInfo.headshot);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//イベントタスク表示
	if( strcmp(NewCommand, "event") == 0 ){
		for(int i=0; i<TOTAL_EVENTLINE; i++){
			signed char p4 = Event[i].GetNextP4();
			pointdata data;
			if( PointData.SearchPointdata(&data, 0x08, 0, 0, 0, p4, 0) == 0 ){
				sprintf(str, "Event %d   No task.", i);
			}
			else{
				sprintf(str, "Event %d   [%d][%d][%d][%d]", i, data.p1, data.p2, data.p3, data.p4);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//MIFの情報表示
	if( strcmp(NewCommand, "mif") == 0 ){
		bool AddonFlag = GameInfoData.selectaddon;
		int MissionID = GameInfoData.selectmission_id;
		char str2[MAX_PATH];
		char str3[MAX_PATH];
		bool collisionflag, screenflag;

		//ヘッダー
		if( AddonFlag == true ){ sprintf(str, "[Addon Mission]   (MissionID:%d)", MissionID); }
		else{ sprintf(str, "[Standard Mission]   (MissionID:%d)", MissionID); }
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//ミッション識別名
		strcpy(str, "Name : ");
		if( AddonFlag == true ){ strcpy(str2, MIFdata.GetMissionName()); }
		else{ GameParamInfo.GetOfficialMission(MissionID, str2, NULL, NULL, NULL, NULL, NULL); }
		str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
		strcat(str, str2);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//ミッション正式名称
		strcpy(str, "FullName : ");
		if( AddonFlag == true ){ strcpy(str2, MIFdata.GetMissionFullname()); }
		else{ GameParamInfo.GetOfficialMission(MissionID, NULL, str2, NULL, NULL, NULL, NULL); }
		str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
		strcat(str, str2);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//ブロックデータファイル
		strcpy(str, "BD1file : ");
		if( AddonFlag == true ){
			MIFdata.GetDatafilePath(str2, str3);
			str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
			strcat(str, str2);
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, str2, NULL, NULL, NULL);
			str2[(MAX_CONSOLELEN - strlen(str) - 8 - 1)] = '\0';
			strcat(str, str2);
			strcat(str, OFFICIALMISSION_BD1);
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//ポイントデータファイル
		strcpy(str, "PD1file : ");
		if( AddonFlag == true ){
			MIFdata.GetDatafilePath(str3, str2);
			str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
			strcat(str, str2);
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, str2, str3, NULL, NULL);
			strcat(str2, str3);
			str2[(MAX_CONSOLELEN - strlen(str) - 4 - 1)] = '\0';
			strcat(str, str2);
			strcat(str, ".pd1");
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//追加小物情報ファイル
		strcpy(str, "AddOBJfile : ");
		strcpy(str2, MIFdata.GetAddSmallobjectFile());
		str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
		strcat(str, str2);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//各設定値とFlag
		if( AddonFlag == true ){
			collisionflag = MIFdata.GetCollisionFlag();
			screenflag = MIFdata.GetScreenFlag();
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, NULL, NULL, &collisionflag, &screenflag);
		}
		sprintf(str, "Sky:%d    CollisionFlag:%d    DarkScreenFlag:%d", MIFdata.GetSkynumber(), (int)collisionflag, (int)screenflag);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//ブロックデータの情報
	if( strcmp(NewCommand, "bd1") == 0 ){
		bool AddonFlag = GameInfoData.selectaddon;
		int MissionID = GameInfoData.selectmission_id;
		char str2[MAX_PATH];
		char str3[MAX_PATH];
		char fname1[_MAX_PATH];
		char fname2[_MAX_PATH];
		char flagstr1[4];
		char flagstr2[4];

		//ファイル名表示
		strcpy(str, "Filename : ");
		if( AddonFlag == true ){
			MIFdata.GetDatafilePath(str2, str3);
			str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
			strcat(str, str2);
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, str2, NULL, NULL, NULL);
			str2[(MAX_CONSOLELEN - strlen(str) - 8 - 1)] = '\0';
			strcat(str, str2); 
			strcat(str, OFFICIALMISSION_BD1);
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//合計ブロック数表示
		sprintf(str, "TotalBlocks : %d", BlockData.GetTotaldatas());
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//マップテクスチャの読み込み状況表示
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Texture : ");
		for(int i=0; i<(TOTAL_BLOCKTEXTURE/2); i++){
			//テクスチャIDを取得
			BlockData.GetTexture(fname1, i);
			BlockData.GetTexture(fname2, i + (TOTAL_BLOCKTEXTURE/2));
			
			//テクスチャが読み込まれているか判定
			if( d3dg->GetMapTextureID(i) == -1 ){ strcpy(flagstr1, "NG"); }
			else{ strcpy(flagstr1, "OK"); }
			if( d3dg->GetMapTextureID(i + (TOTAL_BLOCKTEXTURE/2)) == -1 ){ strcpy(flagstr2, "NG"); }
			else{ strcpy(flagstr2, "OK"); }

			//表示
			sprintf(str, " %02d_%s %-31s %02d_%s %s", i, flagstr1, fname1, i + (TOTAL_BLOCKTEXTURE/2), flagstr2, fname2);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//ポイントデータの情報
	if( strcmp(NewCommand, "pd1") == 0 ){
		bool AddonFlag = GameInfoData.selectaddon;
		int MissionID = GameInfoData.selectmission_id;
		char str2[MAX_PATH];
		char str3[MAX_PATH];
		pointdata pdata;
		int TotalPoints = PointData.GetTotaldatas();
		int HumanPoints = 0;
		int WeaponPoints = 0;
		int OjectPoints = 0;
		int HumaninfoPoints = 0;
		int PathPoints = 0;
		int EventPoints = 0;

		//ファイル名表示
		strcpy(str, "Filename : ");
		if( AddonFlag == true ){
			MIFdata.GetDatafilePath(str3, str2);
			str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
			strcat(str, str2);
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, str2, str3, NULL, NULL);
			strcat(str2, str3);
			str2[(MAX_CONSOLELEN - strlen(str) - 4 - 1)] = '\0';
			strcat(str, str2);
			strcat(str, ".pd1");
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//合計ポイント数表示
		sprintf(str, "TotalPoints : %d", TotalPoints);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//各種ポイント数を数えて表示
		for(int i=0; i<TotalPoints; i++){
			if( PointData.Getdata(&pdata, i) != 0 ){ continue; }

			if( (pdata.p1 == 1)||(pdata.p1 == 6) ){ HumanPoints += 1; }
			if( (pdata.p1 == 2)||(pdata.p1 == 7) ){ WeaponPoints += 1; }
			if( pdata.p1 == 5 ){ OjectPoints += 1; }
			if( pdata.p1 == 4 ){ HumaninfoPoints += 1; }
			if( (pdata.p1 == 3)||(pdata.p1 == 8) ){ PathPoints += 1; }
			if( (10 <= pdata.p1)&&(pdata.p1 <= 19) ){ EventPoints += 1; }
		}
		sprintf(str, "HumanPoints : %-3d       WeaponPoints : %-3d    SmallOjectPoints : %-3d", HumanPoints, WeaponPoints, OjectPoints);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, "HumaninfoPoints : %-3d   AIpathPoints : %-3d    EventPoints : %-3d", HumaninfoPoints, PathPoints, EventPoints);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//無効なポイント数を逆算で求める
		sprintf(str, "InvalidPoints : %d", TotalPoints - (HumanPoints + WeaponPoints + OjectPoints + HumaninfoPoints + PathPoints + EventPoints));
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//人のテクスチャ数を取得
		sprintf(str, "HumanTextures : %d/%d", Resource.GetHumanTextures(), MAX_LOADHUMANTEXTURE);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//デバック用文字の表示
	if( strcmp(NewCommand, "info") == 0 ){
		if( ShowInfo_Debugmode == false ){
			ShowInfo_Debugmode = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Show Debug information.");
		}
		else{
			ShowInfo_Debugmode = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Hide Debug information.");
		}
	}

	//フリーカメラ切り替え
	if( strcmp(NewCommand, "view") == 0 ){
		if( Camera_Debugmode == false ){
			Camera_Debugmode = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Change FreeView mode.");
		}
		else{
			Camera_Debugmode = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Change PlayerView mode.");
		}

		//デバックモード開始時のカメラ設定
		if( Camera_Debugmode == true ){
			camera_x = 100.0f;
			camera_y = 100.0f;
			camera_z = -100.0f;
			camera_rx = DegreeToRadian(135);
			camera_ry = DegreeToRadian(-40);
		}
	}

	//3Dの中心線表示
	if( strcmp(NewCommand, "center") == 0 ){
		if( CenterLine == false ){
			CenterLine = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Show World center position.");
		}
		else{
			CenterLine = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Hide World center position.");
		}
	}

	//マップをワイヤーフレーム化
	if( strcmp(NewCommand, "map") == 0 ){
		if( wireframe == false ){
			wireframe = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Draw map on the Wire frame.");
		}
		else{
			wireframe = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Draw map on the Normal.");
		}
	}

	//AI情報表示
	if( GetCommandNum("aiinfo", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			if( AIdebuginfoID == id ){
				//同じ番号が指定されたらなら、無効化
				AIdebuginfoID = -1;
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable AI Debug information.");
			}
			else{
				//新たに人を指定
				AIdebuginfoID = id;
				sprintf(str, "Enable AI Debug information. Human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//オブジェクトのタグを表示
	if( strcmp(NewCommand, "tag") == 0 ){
		if( tag == false ){
			tag = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Show object information.");
		}
		else{
			tag = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Hide object information.");
		}
	}

	//レーダーを描画
	if( strcmp(NewCommand, "radar") == 0 ){
		if( radar == false ){
			radar = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Show Radar.");
		}
		else{
			radar = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Hide Radar.");
		}
	}

	//目隠しを描画
	if( strcmp(NewCommand, "inmap") == 0 ){
		if( Camera_Blind == false ){
			Camera_Blind = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Blindfold in map.");
		}
		else{
			Camera_Blind = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Not blindfold in map.");
		}
	}

	//背景空の変更
	if( GetCommandNum("sky", &id) == true ){
		if( (0 <= id)&&(id <= 5) ){
			SkyNumber = id;

			//リソース再構築
			Resource.CleanupSkyModelTexture();
			Resource.LoadSkyModelTexture(SkyNumber);

			sprintf(str, "Select SkyNumber %d.", id);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//画面を暗く
	if( strcmp(NewCommand, "dark") == 0 ){
		char path[MAX_PATH];
		char bdata[MAX_PATH];
		char pdata[MAX_PATH];

		//フラグ切り替え
		if( DarkScreenFlag == false ){
			DarkScreenFlag = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Enable DarkScreen Flag.");
		}
		else{
			DarkScreenFlag = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable DarkScreen Flag.");
		}

		//.bd1のファイルパスを求める
		if( MIFdata.GetFiletype() == false ){
			GameParamInfo.GetOfficialMission(MainGameInfo.selectmission_id, NULL, NULL, path, NULL, NULL, NULL);
		}
		else{
			MIFdata.GetDatafilePath(bdata, pdata);

			strcpy(path, bdata);
			for(int i=strlen(path)-1; i>0; i--){
				if( path[i] == '\\' ){
					path[i+1] = '\0';
					break;
				}
			}
		}

		//ブロックデータ初期化
		BlockData.CalculationBlockdata(DarkScreenFlag);
		d3dg->CleanupMapdata();
		d3dg->LoadMapdata(&BlockData, path);
	}

	//全ての死者を蘇生する
	if( strcmp(NewCommand, "revive") == 0 ){
		for(int i=0; i<MAX_HUMAN; i++){
			ObjMgr.HumanResuscitation(i);
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "All human has resuscitation.");
	}

	//回復する
	if( GetCommandNum("treat", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			int param, hp;
			HumanParameter data;
			human *thuman = ObjMgr.GeHumanObject(id);
			
			//初期時のHPを取得
			thuman->GetParamData(&param, NULL, NULL, NULL);
			GameParamInfo.GetHuman(param, &data);
			hp = data.hp;

			//回復
			if( thuman->SetHP(hp) == true ){
				sprintf(str, "Set the HP:%d to Human[%d].", data.hp, id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//無敵化
	if( GetCommandNum("nodamage", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			if( InvincibleID == id ){
				//同じ番号が指定されたらなら、無効化
				InvincibleID = -1;
				ObjMgr.GeHumanObject(id)->SetInvincibleFlag(false);
				sprintf(str, "Not invincible Human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
			else{
				//既に誰かが指定されていたら、既に指定されている人を無効化
				if( InvincibleID != -1 ){
					ObjMgr.GeHumanObject(InvincibleID)->SetInvincibleFlag(false);
					sprintf(str, "Not invincible Human[%d].", InvincibleID);
					AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
				}

				//新たに無敵状態の人を指定
				InvincibleID = id;
				ObjMgr.GeHumanObject(id)->SetInvincibleFlag(true);
				sprintf(str, "Invincible Human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//殺害
	if( GetCommandNum("kill", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			human *thuman = ObjMgr.GeHumanObject(id);
			if( thuman->GetEnableFlag() == true ){
				if( thuman->SetHP(0) == true ){
					sprintf(str, "Killed Human[%d].", id);
					AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
				}
			}
		}
	}

	//小物を破壊
	if( GetCommandNum("break", &id) == true ){
		if( (0 <= id)&&(id < MAX_SMALLOBJECT) ){
			smallobject *tsmallobject = ObjMgr.GetSmallObject(id);
			if( tsmallobject->GetEnableFlag() == true ){
				tsmallobject->Destruction();
				sprintf(str, "Broke SmallObject[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//小物を新規配置
	if( GetCommandNum("newobj", &id) == true ){
		if( (0 <= id)&&(id < TOTAL_PARAMETERINFO_SMALLOBJECT) ){
			int dataid = ObjMgr.AddSmallObjectIndex(camera_x + cos(camera_rx)*20.0f, camera_y, camera_z + sin(camera_rx)*20.0f, camera_rx*-1, id, true);
			if( dataid != -1 ){
				sprintf(str, "Add SmallObject[%d].", dataid);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//FF（同士討ち）有効化
	if( strcmp(NewCommand, "ff") == 0 ){
		if( ObjMgr.GetFriendlyFireFlag() == false ){
			ObjMgr.SetFriendlyFireFlag(true);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Enable Friendly Fire.");
		}
		else{
			ObjMgr.SetFriendlyFireFlag(false);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable Friendly Fire.");
		}
	}

	//プレイヤー操作をAI化
	if( strcmp(NewCommand, "bot") == 0 ){
		if( PlayerAI == false ){
			PlayerAI = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Player is AI control. (bot)");
		}
		else{
			PlayerAI = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Player is User control.");
		}
	}

	//AIを非戦闘化させる
	if( strcmp(NewCommand, "nofight") == 0 ){
		if( AINoFight == false ){
			AINoFight = true;
			for(int i=0; i<MAX_HUMAN; i++){
				HumanAI[i].SetNoFightFlag(true);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "AI had no Fight.");
		}
		else{
			AINoFight = false;
			for(int i=0; i<MAX_HUMAN; i++){
				HumanAI[i].SetNoFightFlag(false);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "AI had Fight.");
		}
	}

	//AIを警戒させる
	if( strcmp(NewCommand, "caution") == 0 ){
		for(int i=0; i<MAX_HUMAN; i++){
			HumanAI[i].SetCautionMode();
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Set cautious AI.");
	}

	//AIの処理を停止
	if( strcmp(NewCommand, "stop") == 0 ){
		if( AIstop == false ){
			AIstop = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Stopped AI control.");
		}
		else{
			AIstop = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Started AI control.");
		}
	}

	/*
	//ミッション達成
	if( strcmp(NewCommand, "comp") == 0 ){
		end_framecnt += 1;
		MainGameInfo.missioncomplete = true;
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Mission complete.");
	}

	//ミッション失敗
	if( strcmp(NewCommand, "fail") == 0 ){
		end_framecnt += 1;
		MainGameInfo.missioncomplete = false;
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Mission fail.");
	}
	*/

	//イベントポイントの処理を停止
	if( strcmp(NewCommand, "estop") == 0 ){
		if( EventStop == false ){
			EventStop = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Stopped Event chains.");
		}
		else{
			EventStop = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Started Event chains.");
		}
	}

	//バージョン情報取得
	if( strcmp(NewCommand, "ver") == 0 ){
		sprintf(str, "%s   Version:%s", GAMENAME, GAMEVERSION);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, " (Graphics:%s   Sound:%s)", GRAPHICS_CORE, SOUND_CORE);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//ゲームの実行速度
	if( strcmp(NewCommand, "speed") == 0 ){
		if( GameSpeed == 1 ){ GameSpeed = 2; }
		else if( GameSpeed == 2 ){ GameSpeed = 4; }
		else{ GameSpeed = 1; }

		sprintf(str, "Set GameSpeed x%d", GameSpeed);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//ウィンドウ・フルスクリーン切り替え
	if( strcmp(NewCommand, "window") == 0 ){
		bool ErrorFlag = false;

		//現在の表示モード取得
		bool flag = d3dg->GetFullScreenFlag();

		if( flag == false ){ flag = true; }
		else{ flag = false; }

		//切り替え処理（初回）
		WindowCtrl->ChangeWindowMode(flag);
		d3dg->SetFullScreenFlag(flag);
		if( ResetGame(WindowCtrl) != 0 ){
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Error] Change failed.");

			if( flag == false ){
				//フルスクリーン⇒ウィンドウが失敗したら、エラーとして終了。
				WindowCtrl->ErrorInfo("Resetに失敗しました");
				WindowCtrl->CloseWindow();
				ErrorFlag = true;
			}
			else{
				//ウィンドウ⇒フルスクリーンが失敗したら、ウィンドウモードへ戻してゲーム続行を試みる。
				//　（GPUが指定解像度のフルスクリーンに対応してないとか・・？）
				flag = false;

				WindowCtrl->ChangeWindowMode(flag);
				d3dg->SetFullScreenFlag(flag);
				if( ResetGame(WindowCtrl) != 0 ){
					//戻しても失敗するなら、エラーとして終了。
					WindowCtrl->ErrorInfo("Resetに失敗しました");
					WindowCtrl->CloseWindow();
					ErrorFlag = true;
				}
			}
		}
		else{
			//初回で切り替えに成功したら、成功メッセージを表示。
			if( flag == true ){
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Changed FullScreen mode.");
			}
			else{
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Changed Window mode.");
			}
		}

		//切り替えに成功したら、回復などの後処理。
		if( ErrorFlag == false ){
			Recovery();

			//キー入力を取得
			//　※ディスプレイ解像度の変化によるマウスの移動分を捨てる
			inputCtrl->GetInputState(true);
			inputCtrl->MoveMouseCenter();
		}
	}

	//スクリーンショットを撮影
	//　※コンソール画面を削除するため、撮影を1フレーム遅らせる。
	if( ScreenShot == 2 ){
		char fname[256];

		//ファイル名を決定
		GetTimeName(fname);
		strcat(fname, ".bmp");

		//撮影・保存
		if( d3dg->SaveScreenShot(fname) == true ){
			sprintf(str, "Saved Screenshot  (File:%s)", fname);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
		else{
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Error] Save failed.");
		}

		ScreenShot = 0;
	}
	else{
		if( strcmp(NewCommand, "ss") == 0 ){
			ScreenShot = 1;
		}
	}

	//コンソールをクリア
	if( strcmp(NewCommand, "clear") == 0 ){
		for(int i=0; i<MAX_CONSOLELINES; i++){
			InfoConsoleData[i].color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
			InfoConsoleData[i].textdata[0] = '\0';
		}
	}

	//コンソールを閉じる
	if( strcmp(NewCommand, "exit") == 0 ){
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Closed debug console.");
		Show_Console = false;
	}

#ifdef _DEBUG
	//リセット操作
	if( strcmp(NewCommand, "f12") == 0 ){
		GameState->PushF12Key();
		GameSpeed = 1;
	}
#endif
}

//! @brief デバック用コンソールの表示処理
void maingame::RenderConsole()
{
	//下地
	d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, (MAX_CONSOLELINES+1)*17 + 5 + 5, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.75f));

	//表示中の文字
	for(int i=0; i<MAX_CONSOLELINES; i++){
		if( InfoConsoleData[i].textdata[0] != NULL ){
			d3dg->Draw2DTextureDebugFontText(5+1, i*17+5+1, InfoConsoleData[i].textdata, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(5, i*17+5, InfoConsoleData[i].textdata, InfoConsoleData[i].color);
		}
	}

	//入力中の文字
	int cnt;
	for(cnt=0; cnt<MAX_CONSOLELINES; cnt++){
		if( InfoConsoleData[cnt].textdata[0] == NULL ){ break; }
	}
	d3dg->Draw2DTextureDebugFontText(5+1, cnt*17+5+1, InputConsoleData->textdata, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DTextureDebugFontText(5, cnt*17+5, InputConsoleData->textdata, InputConsoleData->color);
}

#endif

void maingame::Destroy()
{
	//ブロックデータ解放
	d3dg->CleanupMapdata();

	//オブジェクトマネージャー解放
	ObjMgr.Cleanup();

	//背景空解放
	Resource.CleanupSkyModelTexture();

#ifdef ENABLE_DEBUGCONSOLE
	//コンソール用データを解放
	if( InfoConsoleData != NULL ){ delete [] InfoConsoleData; }
	if( InputConsoleData != NULL ){ delete InputConsoleData; }
#endif

	GameState->NextState();
}

//! @brief コンストラクタ
result::result()
{}

//! @brief ディストラクタ
result::~result()
{}

//! @brief リザルト画面の2D描画部分
void result::Render2D()
{
	char mname[64];
	char str[32];
	float effectA = GetEffectAlphaLoop(framecnt, 1.0f, 0.8f, true);
	float rate;
	if( GameInfoData.fire == 0 ){
		rate = 0.0f;
	}
	else{
		rate = (float)GameInfoData.ontarget / GameInfoData.fire * 100;
	}

	//メモ：背景画像の描画は、自動的に行われる。

	//固定文字表示
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 50*3, 40, "RESULT", d3dg->GetColorCode(1.0f,0.0f,1.0f,effectA), 50, 42);

	//ミッション名を取得し表示
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, mname, NULL, NULL, NULL, NULL);
	}
	else{
		strcpy(mname, MIFdata.GetMissionFullname());
	}
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(mname)*18/2, 100, mname, d3dg->GetColorCode(0.5f,0.5f,1.0f,1.0f), 18, 25);

	//ミッションクリアーの有無
	if( GameInfoData.missioncomplete == true ){
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 216, 150, "mission successful", d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f), 24, 32);
	}
	else{
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 180, 150, "mission failure", d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f), 24, 32);
	}

	//結果表示
	sprintf(str, "Time  %dmin %dsec", GameInfoData.framecnt/(int)GAMEFPS/60, GameInfoData.framecnt/(int)GAMEFPS%60);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2-strlen(str)*20/2, 210, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
	sprintf(str, "Rounds fired  %d", GameInfoData.fire);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2-strlen(str)*20/2, 260, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
	sprintf(str, "Rounds on target  %d", GameInfoData.ontarget);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2-strlen(str)*20/2, 310, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
	sprintf(str, "Accuracy rate  %.1f%%", rate);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2-strlen(str)*20/2, 360, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
	sprintf(str, "Kill  %d / HeadShot  %d", GameInfoData.kill, GameInfoData.headshot);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2-strlen(str)*20/2, 410, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
}

//! @brief screen派生クラスの初期化（クラスの設定）
void InitScreen(WindowControl *WindowCtrl, opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result)
{
	Opening->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl, &GameSound);
	MainMenu->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl, &GameSound);
	Briefing->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl);
	MainGame->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl, &GameSound);
	Result->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl);
}

//! @brief screen派生クラスの実行
void ProcessScreen(WindowControl *WindowCtrl, opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result, unsigned int framecnt)
{
	int error;

	switch(GameState.GetState()){
		//オープニング初期化
		case STATE_CREATE_OPENING:
			error = Opening->Create();
			if( error != 0 ){
				if( (error&0x0001) != 0 ){
					WindowCtrl->ErrorInfo("block data open failed");
				}
				if( (error&0x0002) != 0 ){
					WindowCtrl->ErrorInfo("point data open failed");
				}
				//WindowCtrl->CloseWindow();
				GameState.PushBackSpaceKey();
			}
			break;

		//オープニング実行
		case STATE_NOW_OPENING:
			Opening->Input();
			Opening->Process();
			Opening->Sound();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( Opening->RenderMain() == true ){
					if( ResetGame(WindowCtrl) != 0 ){
						WindowCtrl->ErrorInfo("Resetに失敗しました");
						WindowCtrl->CloseWindow();
					}
					else{
						Opening->Recovery();

						//現在の画面を再スタートさせる
						//GameState.PushF12Key();
					}
				}
			}
			break;

		//オープニング廃棄
		case STATE_DESTROY_OPENING:
			Opening->Destroy();
			break;

		//メニュー初期化
		case STATE_CREATE_MENU:
			error = MainMenu->Create();
			if( error != 0 ){
				if( (error&0x0001) != 0 ){
					WindowCtrl->ErrorInfo("block data open failed");
				}
				if( (error&0x0002) != 0 ){
					WindowCtrl->ErrorInfo("point data open failed");
				}
				WindowCtrl->CloseWindow();
			}
			break;

		//メニュー実行
		case STATE_NOW_MENU:
			MainMenu->Input();
			MainMenu->Process();
			MainMenu->Sound();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( MainMenu->RenderMain() == true ){
					if( ResetGame(WindowCtrl) != 0 ){
						WindowCtrl->ErrorInfo("Resetに失敗しました");
						WindowCtrl->CloseWindow();
					}
					else{
						MainMenu->Recovery();

						//現在の画面を再スタートさせる
						//GameState.PushF12Key();
					}
				}
			}
			break;

		//メニュー廃棄
		case STATE_DESTROY_MENU:
			MainMenu->Destroy();
			break;

		//ブリーフィング初期化
		case STATE_CREATE_BRIEFING:
			error = Briefing->Create();
			if( error == 1 ){
				WindowCtrl->ErrorInfo("briefing data open failed");
				WindowCtrl->CloseWindow();
			}
			break;

		//ブリーフィング実行
		case STATE_NOW_BRIEFING:
			Briefing->Input();
			Briefing->Process();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( Briefing->RenderMain() == true ){
					if( ResetGame(WindowCtrl) != 0 ){
						WindowCtrl->ErrorInfo("Resetに失敗しました");
						WindowCtrl->CloseWindow();
					}
					else{
						Briefing->Recovery();

						//現在の画面を再スタートさせる
						//GameState.PushF12Key();
					}
				}
			}
			break;

		//ブリーフィング廃棄
		case STATE_DESTROY_BRIEFING:
			Briefing->Destroy();
			break;

		//メインゲーム初期化
		case STATE_CREATE_MAINGAME:
			error = MainGame->Create();
			if( error != 0 ){
				if( (error&0x0001) != 0 ){
					WindowCtrl->ErrorInfo("block data open failed");
				}
				if( (error&0x0002) != 0 ){
					WindowCtrl->ErrorInfo("point data open failed");
				}
				WindowCtrl->CloseWindow();
			}
			break;

		//メインゲーム実行
		case STATE_NOW_MAINGAME:
			for(int i=0; i<MainGame->GetGameSpeed(); i++){
				MainGame->Input();
				MainGame->Process();
				MainGame->Sound();
			}
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( MainGame->RenderMain() == true ){
					if( ResetGame(WindowCtrl) != 0 ){
						WindowCtrl->ErrorInfo("Resetに失敗しました");
						WindowCtrl->CloseWindow();
					}
					else{
						MainGame->Recovery();

						//現在の画面を再スタートさせる
						//GameState.PushF12Key();
					}
				}
			}
			break;

		//メインゲーム廃棄
		case STATE_DESTROY_MAINGAME:
			MainGame->Destroy();
			break;

		//リザルト初期化
		case STATE_CREATE_RESULT:
			error = Result->Create();
			break;

		//リザルト実行
		case STATE_NOW_RESULT:
			Result->Input();
			Result->Process();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( Result->RenderMain() == true ){
					if( ResetGame(WindowCtrl) != 0 ){
						WindowCtrl->ErrorInfo("Resetに失敗しました");
						WindowCtrl->CloseWindow();
					}
					else{
						Result->Recovery();

						//現在の画面を再スタートさせる
						//GameState.PushF12Key();
					}
				}
			}
			break;

		//リザルト廃棄
		case STATE_DESTROY_RESULT:
			Result->Destroy();
			break;

		//ゲーム終了
		case STATE_EXIT:
			WindowCtrl->CloseWindow();
			break;

		default:
			break;
	}
}