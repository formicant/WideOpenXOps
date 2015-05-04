//! @file gamemain.cpp
//! @brief �Q�[�����C������

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
// * Neither the name of the OpenXOPS Project nor the�@names of its contributors 
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

D3DGraphics d3dg;			//!< �`��N���X
InputControl inputCtrl;		//!< ���͎擾�N���X
SoundControl SoundCtrl;		//!< �T�E���h�Đ��N���X

//! �Q�[���̐ݒ�l
ParameterInfo GameParamInfo;

//! ���\�[�X�Ǘ�
ResourceManager Resource;

SoundManager GameSound;		//!< �Q�[�����ʉ��Đ��N���X

BlockDataInterface BlockData;		//!< �u���b�N�f�[�^�Ǘ��N���X
PointDataInterface PointData;		//!< �|�C���g�f�[�^�Ǘ��N���X
MIFInterface MIFdata;				//!< MIF�R���g���[��
AddonList GameAddon;				//!< addon�̃��X�g
Collision CollD;					//!< �����蔻��Ǘ��N���X
ObjectManager ObjMgr;				//!< �I�u�W�F�N�g�Ǘ��N���X
AIcontrol HumanAI[MAX_HUMAN];		//!< AI�Ǘ��N���X

GameInfo GameInfoData;				//!< �Q�[���̏��

EventControl Event[TOTAL_EVENTLINE];	//!< �C�x���g����N���X


//! @brief ��{�I�ȏ���������
int InitGame(WindowControl *WindowCtrl)
{
	//DirectX������
	if( d3dg.InitD3D(WindowCtrl, "data\\char.dds", GameConfig.GetFullscreenFlag()) ){
		WindowCtrl->ErrorInfo("Direct3D�̍쐬�Ɏ��s���܂���");
		return 1;
	}

	//Directinput�̏�����
	if( inputCtrl.InitD3Dinput(WindowCtrl) ){
		WindowCtrl->ErrorInfo("Input initialization error");
		return 1;
	}

	//EASY DIRECT SOUND ������
	if( SoundCtrl.InitSound(WindowCtrl) ){
		WindowCtrl->ErrorInfo("DLL open failed");
		return 1;
	}

	//�ݒ�l��������
	GameParamInfo.InitInfo();

	//���\�[�X�̏����ݒ�
	Resource.SetParameterInfo(&GameParamInfo);
	Resource.SetD3DGraphics(&d3dg);
	Resource.SetSoundControl(&SoundCtrl);

	//���\�[�X��������
	Resource.LoadHumanModel();
	Resource.LoadWeaponModelTexture();
	Resource.LoadWeaponSound();
	Resource.LoadSmallObjectModelTexture();
	Resource.LoadSmallObjectSound();
	Resource.LoadScopeTexture();
	Resource.LoadBulletModelTexture();
	Resource.LoadBulletSound();
	Resource.LoadEffectTexture();

	//���ʉ�������
	float volume;
	if( GameConfig.GetSoundFlag() == false ){
		volume = 0.0f;
	}
	else{
		volume = 1.0f;
	}
	GameSound.SetClass(&SoundCtrl, &Resource, &GameParamInfo);
	SoundCtrl.SetVolume(volume);

	//�I�u�W�F�N�g�}�l�[�W���[������
	ObjMgr.SetClass(&GameParamInfo, &d3dg, &Resource, &BlockData, &PointData, &CollD, &GameSound, &MIFdata);

	//addon���X�g�쐬
	GameAddon.LoadFiledata("addon\\");

	GameInfoData.selectaddon = false;

	return 0;
}

//! @brief DirectX�����Z�b�g����
//! @return ���s�F1�@����ȊO�F0
//! @attention �ʏ�́A�`�揈���Ɏ��s�����ꍇ�Ɍ���Ăяo���Ă��������B
int ResetGame(WindowControl *WindowCtrl)
{
	int rtn = d3dg.ResetD3D(WindowCtrl);

	if( rtn == 0 ){
		//���\�[�X��������
		Resource.LoadHumanModel();
		Resource.LoadWeaponModelTexture();
		Resource.LoadSmallObjectModelTexture();
		Resource.LoadScopeTexture();
		Resource.LoadBulletModelTexture();
		Resource.LoadEffectTexture();

		//���݂̉�ʂ��ăX�^�[�g������
		GameState.PushF12Key();
	}
	if( rtn == 1 ){
		//
	}
	if( rtn == 2 ){
		WindowCtrl->ErrorInfo("Reset�Ɏ��s���܂���");
		WindowCtrl->CloseWindow();
		return 1;
	}
	return 0;
}

//! @brief �R���X�g���N�^
opening::opening()
{}

//! @brief �f�B�X�g���N�^
opening::~opening()
{}

int opening::Create()
{
	//�u���b�N�f�[�^�ǂݍ���
	if( BlockData.LoadFiledata("data\\map10\\temp.bd1") ){
		//block data open failed
		return 1;
	}
	BlockData.CalculationBlockdata(false);
	d3dg->LoadMapdata(&BlockData, "data\\map10\\");
	CollD.InitCollision(&BlockData);

	//�|�C���g�f�[�^�ǂݍ���
	if( PointData.LoadFiledata("data\\map10\\op.pd1") ){
		//point data open failed
		return 2;
	}
	ObjMgr.LoadPointData();
	ObjMgr.SetPlayerID(MAX_HUMAN-1);	//���݂��Ȃ��l���v���C���[�Ɂi�e���̃T�E���h�Đ��΍�j

	//AI�ݒ�
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
	}

	//�w�i��ǂݍ���
	Resource.LoadSkyModelTexture(1);

	//opening_banner = d3dg->LoadTexture("banner.png", true, false);

	//�T�E���h������
	GameSound->InitWorldSound();

	//�}�E�X�J�[�\���𒆉��ֈړ�
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	GameState->NextState();
	return 0;
}

void opening::Input()
{
	inputCtrl->GetInputState(false);

	if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){
		GameState->PushBackSpaceKey();
	}

	if( inputCtrl->CheckMouseButtonUpL() ){
		GameState->PushMouseButton();
	}

	if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F)) ){		// [ENTER]
		//[ENTER]�������Ă��A�}�E�X���N���b�N�������Ƃɂ���B
		GameState->PushMouseButton();
	}
}

//! @todo �J�����̈ړ������炩�ɂ���
void opening::Process()
{
	//�I�u�W�F�N�g�}�l�[�W���[�����s
	ObjMgr.Process(-1, camera_rx, camera_ry);

	//AI�����s
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].Process();
	}

	//�J�������[�N�����߂�
	if( framecnt < 3*((int)GAMEFPS) ){
		camera_x = -5.0f;
		camera_y = 58.0f;
		camera_z = 29.0f;
		camera_rx = DegreeToRadian(206);
		camera_ry = DegreeToRadian(12);
	}
	else if( framecnt < 5*((int)GAMEFPS) ){
		camera_rx += DegreeToRadian(1.1f);
		camera_ry -= DegreeToRadian(0.7f);
	}
	else if( framecnt < 17*((int)GAMEFPS) ){
		camera_z += 0.08f;
		camera_y -= 0.05f;
	}
	else {
		GameState->PushMouseButton();
	}

	framecnt += 1;
}

void opening::Render3D()
{
	int skymodel, skytexture;

	//�t�H�O�ƃJ������ݒ�
	d3dg->SetFog(1);
	d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);

	//�J�������W�ɔw�i���`��
	d3dg->SetWorldTransform(camera_x, camera_y, camera_z, 0.0f, 0.0f, 2.0f);
	Resource.GetSkyModelTexture(&skymodel, &skytexture);
	d3dg->RenderModel(skymodel, skytexture);

	//Z�o�b�t�@��������
	d3dg->ResetZbuffer();

	//�}�b�v��`��
	d3dg->ResetWorldTransform();
	d3dg->DrawMapdata(false);

	//�I�u�W�F�N�g��`��
	ObjMgr.Render(camera_x, camera_y, camera_z, 0);
}

void opening::Render2D()
{
	float effect = 0.0f;

	//�u���b�N�A�E�g�ݒ�
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

	//�㉺�̍����`��
	d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, 40, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DBox(0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));

	//�v���W�F�N�g��
	if( ((int)(0.5f*GAMEFPS) < framecnt)&&(framecnt < (int)(4.0f*GAMEFPS)) ){
		char str[32];
		float effectA = 1.0f;
		sprintf(str, GAMENAME" project", 0, 0);
		if( framecnt < (int)(1.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 0.5f, false); }
		if( framecnt > (int)(3.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 3.0f, true); }
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(str)*22/2, SCREEN_HEIGHT - 140, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 22, 22);
	}

	//�X�^�b�t���E���̂P
	if( ((int)(4.0f*GAMEFPS) < framecnt)&&(framecnt < (int)(8.0f*GAMEFPS)) ){
		float effectA = 1.0f;
		if( framecnt < (int)(5.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 4.0f, false); }
		if( framecnt > (int)(7.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 7.0f, true); }
		d3dg->Draw2DTextureFontText(60, 150, "ORIGINAL", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
	}
	if( ((int)(4.5f*GAMEFPS) < framecnt)&&(framecnt < (int)(8.5f*GAMEFPS)) ){
		float effectA = 1.0f;
		if( framecnt < (int)(5.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 4.5f, false); }
		if( framecnt > (int)(7.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 7.5f, true); }
		d3dg->Draw2DTextureFontText(100, 180, "nine-two", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
		d3dg->Draw2DTextureFontText(100, 210, "TENNKUU", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
	}

	//�X�^�b�t���E���̂Q
	if( ((int)(7.0f*GAMEFPS) < framecnt)&&(framecnt < (int)(11.0f*GAMEFPS)) ){
		float effectA = 1.0f;
		if( framecnt < (int)(8.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 7.0f, false); }
		if( framecnt > (int)(10.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 10.0f, true); }
		d3dg->Draw2DTextureFontText(330, 300, "REMAKE", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
	}
	if( ((int)(7.5f*GAMEFPS) < framecnt)&&(framecnt < (int)(11.5f*GAMEFPS)) ){
		float effectA = 1.0f;
		if( framecnt < (int)(8.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 7.5f, false); }
		if( framecnt > (int)(10.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 10.5f, true); }
		d3dg->Draw2DTextureFontText(370, 330, "[-_-;](mikan)", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 20, 20);
		//d3dg->Draw2DTexture(410, 360, opening_banner, 200, 40, effectA);
	}

	//�Q�[����
	if( (int)(12.0f*GAMEFPS) <= framecnt ){	//framecnt < (int)(17.0f*GAMEFPS)
		char str[32];
		float effectA = 1.0f;
		sprintf(str, GAMENAME, 0, 0);
		if( framecnt < (int)(13.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 12.0f, false); }
		if( ((int)(16.0f*GAMEFPS) < framecnt)&&(framecnt < (int)(17.0f*GAMEFPS)) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 16.0f, true); }
		if( framecnt >= (int)(17.0f*GAMEFPS) ){ effectA = 0.0f; }
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(str)*22/2, (SCREEN_HEIGHT-11)/2, str, d3dg->GetColorCode(1.0f,0.0f,0.0f,effectA), 22, 22);
	}
}

void opening::Destroy()
{
	//�u���b�N�f�[�^���
	d3dg->CleanupMapdata();

	//�I�u�W�F�N�g�}�l�[�W���[���
	ObjMgr.Cleanup();

	//�w�i����
	Resource.CleanupSkyModelTexture();

	//d3dg->CleanupTexture(opening_banner);

	GameState->NextState();
}

//! @brief �R���X�g���N�^
mainmenu::mainmenu()
{
	mainmenu_scrollitems_official = 0;
	mainmenu_scrollitems_addon = 0;
}

//! @brief �f�B�X�g���N�^
mainmenu::~mainmenu()
{}

int mainmenu::Create()
{
	char path[MAX_PATH];
	char bdata[MAX_PATH];
	char pdata[MAX_PATH];

	//�f�������肵�ǂݍ���
	switch( GetRand(6) ){
		case 0:
			strcpy(path, "data\\map2\\");
			break;
		case 1:
			strcpy(path, "data\\map4\\");
			break;
		case 2:
			strcpy(path, "data\\map5\\");
			break;
		case 3:
			strcpy(path, "data\\map7\\");
			break;
		case 4:
			strcpy(path, "data\\map8\\");
			break;
		case 5:
			strcpy(path, "data\\map16\\");
			break;
	}
	strcpy(bdata, path);
	strcat(bdata, "temp.bd1");
	strcpy(pdata, path);
	strcat(pdata, "demo.pd1");

	//�u���b�N�f�[�^�ǂݍ���
	if( BlockData.LoadFiledata(bdata) ){
		//block data open failed
		return 1;
	}
	BlockData.CalculationBlockdata(false);
	d3dg->LoadMapdata(&BlockData, path);
	CollD.InitCollision(&BlockData);

	//�|�C���g�f�[�^�ǂݍ���
	if( PointData.LoadFiledata(pdata) ){
		//point data open failed
		return 2;
	}
	ObjMgr.LoadPointData();

	//AI�ݒ�
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
	}

	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	//�T�E���h������
	GameSound->InitWorldSound();

	mainmenu_mouseX = SCREEN_WIDTH/2;
	mainmenu_mouseY = SCREEN_HEIGHT/2;

	//�W���~�b�V�����̃X�N���[���o�[�̐ݒ�
	if( TOTAL_OFFICIALMISSION > TOTAL_MENUITEMS ){
		mainmenu_scrollbar_official_height = (float)(MAINMENU_H-25) / TOTAL_OFFICIALMISSION * TOTAL_MENUITEMS;
		mainmenu_scrollbar_official_scale = ((float)(MAINMENU_H-25) - mainmenu_scrollbar_official_height) / (TOTAL_OFFICIALMISSION - TOTAL_MENUITEMS);
	}
	else{
		mainmenu_scrollbar_official_height = 0.0f;
		mainmenu_scrollbar_official_scale = 0.0f;
	}

	//addon�̃X�N���[���o�[�̐ݒ�
	if( GameAddon.GetTotaldatas() > TOTAL_MENUITEMS ){
		mainmenu_scrollbar_addon_height = (float)(MAINMENU_H-25) / GameAddon.GetTotaldatas() * TOTAL_MENUITEMS;
		mainmenu_scrollbar_addon_scale = ((float)(MAINMENU_H-25) - mainmenu_scrollbar_addon_height) / (GameAddon.GetTotaldatas() - TOTAL_MENUITEMS);
	}
	else{
		mainmenu_scrollbar_addon_height = 0.0f;
		mainmenu_scrollbar_addon_scale = 0.0f;
	}

	mainmenu_scrollbar_official_y = 141;
	mainmenu_scrollbar_addon_y = 141;
	mainmenu_scrollbar_flag = false;
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	GameState->NextState();
	return 0;
}

void mainmenu::Input()
{
	inputCtrl->GetInputState(false);

	//�X�N���[���o�[�̏��Ȃǂ��擾
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

	//�}�E�X���W���擾
	int y = mainmenu_mouseY;
	inputCtrl->GetMouseMovement(&mainmenu_mouseX, &mainmenu_mouseY);
	if( mainmenu_mouseX < 0 ){ mainmenu_mouseX = 0; }
	if( mainmenu_mouseX > SCREEN_WIDTH-1 ){ mainmenu_mouseX = SCREEN_WIDTH-1; }
	if( mainmenu_mouseY < 0 ){ mainmenu_mouseY = 0; }
	if( mainmenu_mouseY > SCREEN_HEIGHT-1 ){ mainmenu_mouseY = SCREEN_HEIGHT-1; }

	//ESC�L�[������
	if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){
		GameState->PushBackSpaceKey();
	}

	//�X�N���[���o�[��������������
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

		//�~�b�V�����I��
		for(int i=0; i<TOTAL_MENUITEMS; i++){
			char name[32];
			if( GameInfoData.selectaddon == false ){
				GameParamInfo.GetOfficialMission(scrollitems + i, name, NULL, NULL, NULL);
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

	//�X�N���[���o�[�̈ړ�
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

	//�X�N���[���o�[�̏��Ȃǂ𔽉f
	if( GameInfoData.selectaddon == false ){
		mainmenu_scrollitems_official = scrollitems;
		mainmenu_scrollbar_official_y = scrollbar_y;
	}
	else{
		mainmenu_scrollitems_addon = scrollitems;
		mainmenu_scrollbar_addon_y = scrollbar_y;
	}

	//�W���~�b�V�����ƃA�h�I�����X�g�̐؂�ւ�
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
	//�I�u�W�F�N�g�}�l�[�W���[�����s
	ObjMgr.Process(-1, camera_rx, camera_ry);

	//AI�����s
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].Process();
	}

	//�J�����ʒu���v�Z
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
	//�t�H�O�ƃJ������ݒ�
	d3dg->SetFog(0);
	d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);

	//Z�o�b�t�@��������
	d3dg->ResetZbuffer();

	//�}�b�v��`��
	d3dg->ResetWorldTransform();
	d3dg->DrawMapdata(false);

	//�I�u�W�F�N�g��`��
	ObjMgr.Render(camera_x, camera_y, camera_z, 0);
}

void mainmenu::Render2D()
{
	int color, color2;
	float effect;

	//�X�N���[���o�[�̏��Ȃǂ��擾
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

	//�Q�[���̃o�[�W�������`��
	d3dg->Draw2DTextureFontText(522+1, 75+1, GAMEVERSION, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 18, 22);
	d3dg->Draw2DTextureFontText(522, 75, GAMEVERSION, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 18, 22);

	//���j���[�G���A�`��
	if( GameAddon.GetTotaldatas() > 0 ){	//addon�������
		d3dg->Draw2DBox(MAINMENU_X-1, MAINMENU_Y, MAINMENU_X+360, MAINMENU_Y+MAINMENU_H+1, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
	}
	else{
		d3dg->Draw2DBox(MAINMENU_X-1, MAINMENU_Y, MAINMENU_X+360, MAINMENU_Y+MAINMENU_H-24, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
	}
	d3dg->Draw2DBox(MAINMENU_X+341, MAINMENU_Y+1, MAINMENU_X+360, MAINMENU_Y+MAINMENU_H-24, d3dg->GetColorCode(0.5f,0.5f,0.5f,0.5f));

	//�X�N���[���o�[�`��
	if( totalmission > TOTAL_MENUITEMS ){
		//�F��ݒ�
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

		//�`��
		d3dg->Draw2DBox(MAINMENU_X+341, scrollbar_y, MAINMENU_X+360, scrollbar_y + (int)scrollbar_height, color);
		d3dg->Draw2DBox(MAINMENU_X+341+3, scrollbar_y +3, MAINMENU_X+360-3, scrollbar_y + (int)scrollbar_height -3, color2);
	}

	//'< UP >'�`��
	if( scrollitems > 0 ){
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+1, "<  UP  >", d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 25, 26);

		//�����̐F��ݒ�
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&(MAINMENU_Y < mainmenu_mouseY)&&(mainmenu_mouseY < MAINMENU_Y+30) ){
			color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
		}
		else{
			color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
		}

		//������`��
		d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y, "<  UP  >", color, 25, 26);
	}
	else{
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+1, "<  UP  >", d3dg->GetColorCode(0.6f,0.6f,0.6f,1.0f), 25, 26);
	}

	//'< DOWN >'�`��
	if( scrollitems < (totalmission - TOTAL_MENUITEMS) ){
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+MAINMENU_H-55+1, "< DOWN >", d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 25, 26);

		//�����̐F��ݒ�
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&((MAINMENU_Y+MAINMENU_H-55) < mainmenu_mouseY)&&(mainmenu_mouseY < (MAINMENU_Y+MAINMENU_H-55+30)) ){
			color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
		}
		else{
			color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
		}

		//������`��
		d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y+MAINMENU_H-55, "< DOWN >", color, 25, 26);
	}
	else{
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+MAINMENU_H-55+1, "< DOWN >", d3dg->GetColorCode(0.6f,0.6f,0.6f,1.0f), 25, 26);
	}

	//�W���~�b�V������addon�؂�ւ�
	if( GameInfoData.selectaddon == false ){
		//addon�������
		if( GameAddon.GetTotaldatas() > 0 ){
			//�����̐F��ݒ�
			if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&((MAINMENU_Y+MAINMENU_H-25) < mainmenu_mouseY)&&(mainmenu_mouseY < (MAINMENU_Y+MAINMENU_H-2)) ){
				color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
			}
			else{
				color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
			}

			//������`��
			d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+MAINMENU_H-25+1, "ADD-ON MISSIONS >>", d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 17, 22);
			d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y+MAINMENU_H-25, "ADD-ON MISSIONS >>", color, 17, 22);
		}
	}
	else{
		//addon�������
		//if( GameAddon.GetTotaldatas() > 0 ){
			//�����̐F��ݒ�
			if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+340))&&((MAINMENU_Y+MAINMENU_H-25) < mainmenu_mouseY)&&(mainmenu_mouseY < (MAINMENU_Y+MAINMENU_H-2)) ){
				color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
			}
			else{
				color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
			}

			//������`��
			d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+MAINMENU_H-25+1, "<< STANDARD MISSIONS", d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 17, 22);
			d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y+MAINMENU_H-25, "<< STANDARD MISSIONS", color, 17, 22);
		//}
	}

	//�~�b�V��������`��
	for(int i=0; i<TOTAL_MENUITEMS; i++){
		char name[32];
		strcpy(name, "");

		//�~�b�V���������擾
		if( GameInfoData.selectaddon == false ){
			GameParamInfo.GetOfficialMission(scrollitems + i, name, NULL, NULL, NULL);
		}
		else{
			strcpy(name, GameAddon.GetMissionName(scrollitems + i));
		}

		//�����̐F��ݒ�
		if( (MAINMENU_X < mainmenu_mouseX)&&(mainmenu_mouseX < (MAINMENU_X+(signed)strlen(name)*20))&&(MAINMENU_Y+30 + i*30 < mainmenu_mouseY)&&(mainmenu_mouseY < MAINMENU_Y+30 + i*30 + 26) ){
			color = d3dg->GetColorCode(1.0f,0.6f,0.6f,1.0f);
		}
		else{
			color = d3dg->GetColorCode(0.6f,0.6f,1.0f,1.0f);
		}

		//������`��
		d3dg->Draw2DTextureFontText(MAINMENU_X+1, MAINMENU_Y+30+1 + i*30, name, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 20, 26);
		d3dg->Draw2DTextureFontText(MAINMENU_X, MAINMENU_Y+30 + i*30, name, color, 20, 26);
	}

	//�}�E�X�J�[�\���`��i�Ԑ��j
	d3dg->Draw2DBox(0, mainmenu_mouseY-1, SCREEN_WIDTH, mainmenu_mouseY+1, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
	d3dg->Draw2DBox(mainmenu_mouseX-1, 0, mainmenu_mouseX+1, SCREEN_HEIGHT, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
	d3dg->Draw2DLine(0, mainmenu_mouseY, SCREEN_WIDTH, mainmenu_mouseY, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DLine(mainmenu_mouseX, 0, mainmenu_mouseX, SCREEN_HEIGHT, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));

	//�Q�[���̃��S�}�[�N�`��
	d3dg->Draw2DTexture(20, 25, gametitle, 480, 80, 1.0f);

	//�u���b�N�A�E�g�ݒ�
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
	//�u���b�N�f�[�^���
	d3dg->CleanupMapdata();

	//�I�u�W�F�N�g�}�l�[�W���[���
	ObjMgr.Cleanup();

	//�w�i����
	Resource.CleanupSkyModelTexture();

	d3dg->CleanupTexture(gametitle);

	GameState->NextState();
}

//! @brief �R���X�g���N�^
briefing::briefing()
{}

//! @brief �f�B�X�g���N�^
briefing::~briefing()
{}

int briefing::Create()
{
	char path[MAX_PATH];
	char pdata[MAX_PATH];
	char PictureA[MAX_PATH];
	char PictureB[MAX_PATH];

	//�w�i�摜���擾
	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	//mif�t�@�C���̃t�@�C���p�X�擾
	if( GameInfoData.selectaddon == false ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, NULL, path, pdata);
		strcat(path, pdata);
		strcat(path, ".txt");
	}
	else{
		strcpy(path, "addon\\");
		strcat(path, GameAddon.GetFileName(GameInfoData.selectmission_id));
	}

	//mif�t�@�C����ǂݍ���
	if( MIFdata.LoadFiledata(path) != 0 ){
		//briefing data open failed
		return 1;
	}

	//�u���[�t�B���O�摜�̃t�@�C���p�X�擾
	MIFdata.GetPicturefilePath(PictureA, PictureB);

	//�u���[�t�B���O�摜�ǂݍ���
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

	//�}�E�X�J�[�\���𒆉��ֈړ�
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	GameState->NextState();
	return 0;
}

void briefing::Render2D()
{
	float effectA = GetEffectAlphaLoop(framecnt, 0.8f, 0.7f, true);
	float effectB = GetEffectAlphaLoop(framecnt, 0.8f, 1.0f, true);
	int effectB_sizeW = (int)( (float)(framecnt%((int)(GAMEFPS*1.0f))) * 0.2f + 18 );
	int effectB_sizeH = (int)( (float)(framecnt%((int)(GAMEFPS*1.0f))) * 1.0f + 26 );

	//�����F�w�i�摜�̕`��́A�����I�ɍs����B

	//�Œ蕶���`��
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 60*4, 30, "BRIEFING", d3dg->GetColorCode(1.0f,1.0f,0.0f,effectA), 60, 42);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH - 210 - effectB_sizeW*20/2, SCREEN_HEIGHT - 37 - effectB_sizeH/2,
								"LEFT CLICK TO BEGIN", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectB), effectB_sizeW, effectB_sizeH);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH - 210 - 18*20/2, SCREEN_HEIGHT - 37 - 26/2, "LEFT CLICK TO BEGIN", d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 18, 26);

	//�u���[�t�B���O�摜�`��
	if( TwoTexture == false ){
		d3dg->Draw2DBox(40, 180, 40+160, 180+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
		d3dg->Draw2DTexture(40, 180, TextureA, 160, 150, 1.0f);
	}
	else{
		d3dg->Draw2DBox(40, 130, 40+160, 130+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
		d3dg->Draw2DTexture(40, 130, TextureA, 160, 150, 1.0f);

		d3dg->Draw2DBox(40, 300, 40+160, 300+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
		d3dg->Draw2DTexture(40, 300, TextureB, 160, 150, 1.0f);
	}

	//�~�b�V���������擾�E�`��
	char mname[64];
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, mname, NULL, NULL);
	}
	else{
		strcpy(mname, MIFdata.GetMissionFullname());
	}
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(mname)*18/2, 90, mname, d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f), 18, 25);

	//�~�b�V����������`��
	d3dg->Draw2DMSFontText(230, 180, MIFdata.GetBriefingText(), d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
}

void briefing::Destroy(){
	//�u���[�t�B���O�摜���J��
	d3dg->CleanupTexture(TextureA);
	d3dg->CleanupTexture(TextureB);

	//�w�i�摜���J��
	d3dg->CleanupTexture(gametitle);

	GameState->NextState();
}

//! @brief �R���X�g���N�^
maingame::maingame()
{
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

//! @brief �f�B�X�g���N�^
maingame::~maingame()
{}

int maingame::Create()
{
	MainGameInfo = GameInfoData;
	char path[MAX_PATH];
	char bdata[MAX_PATH];
	char pdata[MAX_PATH];
	char pdata2[MAX_PATH];

	//.bd1��.pd1�̃t�@�C���p�X�����߂�
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(MainGameInfo.selectmission_id, NULL, NULL, path, pdata2);

		strcpy(bdata, path);
		strcat(bdata, "temp.bd1");
		strcpy(pdata, path);
		strcat(pdata, pdata2);
		strcat(pdata, ".pd1");
	}
	else{
		MIFdata.GetDatafilePath(bdata, pdata);

		strcpy(path, bdata);
		for(int i=strlen(path)-1; i>0; i--){
			if( path[i] == '\\' ){
				path[i+1] = 0x00;
				break;
			}
		}
	}

	//�ǉ�������ǂݍ���
	Resource.LoadAddSmallObject(MIFdata.GetAddSmallobjectModelPath(), MIFdata.GetAddSmallobjectTexturePath(), MIFdata.GetAddSmallobjectSoundPath());

	//�u���b�N�f�[�^�ǂݍ���
	if( BlockData.LoadFiledata(bdata) ){
		//block data open failed
		return 1;
	}
	BlockData.CalculationBlockdata(MIFdata.GetScreenFlag());
	d3dg->LoadMapdata(&BlockData, path);
	CollD.InitCollision(&BlockData);

	//�|�C���g�f�[�^�ǂݍ���
	if( PointData.LoadFiledata(pdata) ){
		//point data open failed
		return 2;
	}
	ObjMgr.LoadPointData();

	//AI�ݒ�
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
	}


	//�w�i��ǂݍ���
	Resource.LoadSkyModelTexture(MIFdata.GetSkynumber());

	//�T�E���h������
	GameSound->InitWorldSound();

	//�C�x���g������
	for(int i=0; i<TOTAL_EVENTLINE; i++){
		Event[i].SetClass(&PointData, &ObjMgr);
	}
	Event[0].Reset(TOTAL_EVENTENTRYPOINT_0);
	Event[1].Reset(TOTAL_EVENTENTRYPOINT_1);
	Event[2].Reset(TOTAL_EVENTENTRYPOINT_2);

	//�v���C���[�̌������擾
	ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

	view_rx = 0.0f;
	view_ry = 0.0f;
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
	framecnt = 0;
	start_framecnt = 0;
	end_framecnt = 0;
	EventStop = false;
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

	//�R���\�[���p������
	InfoConsoleData = new ConsoleData [MAX_CONSOLELINES];
	InputConsoleData = new ConsoleData;
	for(int i=0; i<MAX_CONSOLELINES; i++){
		InfoConsoleData[i].color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
		InfoConsoleData[i].textdata[0] = NULL;
	}
	InputConsoleData->color = d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f);
	InputConsoleData->textdata[0] = NULL;
	AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Game Debug Console.");
	AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "        Command list >help");
#endif

	GameState->NextState();
	return 0;
}

//! @brief ���葀��̓��͂��`�F�b�N
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
	static unsigned int bullettime = 0;

	//�v���C���[�̃N���X���擾
	human *myHuman = ObjMgr.GetPlayerHumanObject();
	int PlayerID = ObjMgr.GetPlayerID();

	//�L�[���͂��擾
	inputCtrl->GetInputState(true);
	inputCtrl->MoveMouseCenter();

	//�}�E�X�̈ړ��ʎ擾
	int x, y;
	float MouseSensitivity;
	inputCtrl->GetMouseMovement(&x, &y);

	//���_�̈ړ��ʌv�Z
	float mang = 0.0f;
	if( myHuman->GetScopeMode() == 0 ){ mang = 0.01f; }
	if( myHuman->GetScopeMode() == 1 ){ mang = 0.0032f; }
	if( myHuman->GetScopeMode() == 2 ){ mang = 0.0060f; }
	MouseSensitivity = DegreeToRadian(1) * mang * GameConfig.GetMouseSensitivity();

	//�}�E�X���]�i�I�v�V�����ݒ�j���L���Ȃ�΁A���]����B
	if( GameConfig.GetInvertMouseFlag() == true ){
		y *= -1;
	}

	if( Camera_Debugmode == false ){	//�ʏ탂�[�h�Ȃ��
		if( myHuman->GetHP() > 0 ){
			//�}�E�X�ɂ��������v�Z
			mouse_rx += x * MouseSensitivity;
			mouse_ry -= y * MouseSensitivity;

			//�L�[����ɂ��������v�Z
			if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x00)) ){ mouse_ry += DegreeToRadian(3); }		// [��]
			if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x01)) ){ mouse_ry -= DegreeToRadian(3); }		// [��]
			if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x02)) ){ mouse_rx -= DegreeToRadian(3); }		// [��]
			if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x03)) ){ mouse_rx += DegreeToRadian(3); }		// [��]

			if( mouse_ry > DegreeToRadian(70) ) mouse_ry = DegreeToRadian(70);
			if( mouse_ry < DegreeToRadian(-70) ) mouse_ry = DegreeToRadian(-70);
		}
	}
	else{								//�f�o�b�N���[�h�Ȃ��
		//�L�[����ɂ��J�������W���v�Z
		if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x00)) ){ camera_x += 2.0f; }		// [��]
		if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x01)) ){ camera_x -= 2.0f; }		// [��]
		if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x02)) ){ camera_z += 2.0f; }		// [��]
		if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x03)) ){ camera_z -= 2.0f; }		// [��]
		if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x48)) ){ camera_y += 2.0f; }		//�mNUM +�n
		if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x49)) ){ camera_y -= 2.0f; }		//�mNUM -�n

		//�}�E�X�ړ����J�����̌����Ƃ��ēK�p
		//camera_rx -= x*0.0025f;
		//camera_ry -= y*0.0025f;
		camera_rx -= x * MouseSensitivity;
		camera_ry -= y * MouseSensitivity;
		if( camera_ry > DegreeToRadian(70) ) camera_ry = DegreeToRadian(70);
		if( camera_ry < DegreeToRadian(-70) ) camera_ry = DegreeToRadian(-70);
	}

	//�Q�[���I�����삩�`�F�b�N
	if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){
		GameState->PushBackSpaceKey();
	}

	//�J�����\�����[�h�ύX���삩�`�F�b�N
	if( inputCtrl->CheckKeyDown( GetFunctionKeycode(1) ) ){
		if( Camera_F1mode == false ){
			Camera_F1mode = true;
			view_rx = 0.0f;
			view_ry = DegreeToRadian(-22.5f);
		}
		else{
			Camera_F1mode = false;
			view_rx = 0.0f;
			view_ry = 0.0f;
		}
	}

	//��ʂ�UI�ύX���삩�`�F�b�N
	if( inputCtrl->CheckKeyDown( GetFunctionKeycode(2) ) ){
		if( Camera_F2mode == 2 ){
			Camera_F2mode = 0;
		}
		else{
			Camera_F2mode += 1;
		}
	}

	//���Z�b�g���삩�`�F�b�N
	if( inputCtrl->CheckKeyDown( GetFunctionKeycode(12) ) ){
		GameState->PushF12Key();
	}

#ifdef ENABLE_DEBUGCONSOLE
	//�f�o�b�N�p�R���\�[���̕\�����삩�`�F�b�N
	if( inputCtrl->CheckKeyDown( GetFunctionKeycode(11) ) ){
		if( Show_Console == false ){
			Show_Console = true;

			strcpy(InputConsoleData->textdata, CONSOLE_PROMPT);
		}
		else{
			Show_Console = false;
		}
	}

	if( Show_Console == false ){
#endif

		if( PlayerAI == false ){

			//�O�㍶�E�̈ړ��i����j���삩�`�F�b�N
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

			//�������삩�`�F�b�N
			if( CheckInputControl(KEY_WALK, 0) ){
				ObjMgr.MoveWalk(PlayerID);
			}

			//�W�����v���삩�`�F�b�N
			if( CheckInputControl(KEY_JUMP, 1) ){
				ObjMgr.MoveJump(PlayerID);
			}

			if( Camera_Debugmode == true ){		//�f�o�b�N���[�h�Ȃ��
				//���C���삩�`�F�b�N
				if( CheckInputControl(KEY_Shot, 0) ){
					//�O��̔��˂��A4�t���[�����������Ԃ��o���Ă����
					if( bullettime + 4*((int)GAMEFPS) < GetTimeMS() ){
						float x, y, z, rx, ry;
						x = camera_x;
						y = camera_y;
						z = camera_z;
						rx = camera_rx;
						ry = camera_ry;

						//���g�p�̒e�I�u�W�F�N�g���擾
						bullet* newbullet = ObjMgr.GetNewBulletObject();
						if( newbullet != NULL ){
							//�e�I�u�W�F�N�g��ݒ�
							newbullet->SetPosData(x, y, z, rx, ry);
							newbullet->SetParamData(40, 5, 10, 1024, ObjMgr.GetPlayerID(), true);
							newbullet->SetEnableFlag(true);
							GameSound->ShotWeapon(x, y, z, 0, 1024, true);

							//�X�R�A�ɉ��Z
							MainGameInfo.fire += 1;

							//���ˎ��Ԃ��L���i�A�ˊԊu����p�j
							bullettime = GetTimeMS();
						}
					}
				}
			}
			else{		//�f�o�b�N���[�h�łȂ����
				HumanParameter humandata;
				int id_param;
				bool zombie;
				int keymode;

				//�]���r���ǂ�������
				myHuman->GetParamData(&id_param, NULL, NULL, NULL);
				GameParamInfo.GetHuman(id_param, &humandata);
				if( humandata.type == 2 ){
					zombie = true;
				}
				else{
					zombie = false;
				}

				//�A�˃��[�h���擾
				if( zombie == true ){
					keymode = 1;
				}
				else if( myHuman->GetWeaponBlazingmode() == false ){
					keymode = 1;
				}
				else{
					keymode = 0;
				}

				//���C���삩�`�F�b�N
				if( CheckInputControl(KEY_Shot, keymode) ){

					if( zombie == false ){
						//�e�̔��˂ɐ��������
						if( ObjMgr.ShotWeapon(PlayerID) == 1 ){
							//�X�R�A�ɉ��Z
							MainGameInfo.fire += 1;

							//�v���C���[�̌������擾
							ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);
						}
					}
					else{
						for(int i=0; i<MAX_HUMAN; i++){
							human *EnemyHuman = ObjMgr.GeHumanObject(i);
							if( ObjMgr.CheckZombieAttack(myHuman, EnemyHuman) == true ){
								ObjMgr.HitZombieAttack(EnemyHuman);
							}
						}
					}

				}
			}

			//�����[�h���삩�`�F�b�N
			if( CheckInputControl(KEY_RELOAD, 1) ){
				ObjMgr.ReloadWeapon(PlayerID);
			}

			//����̐؂�ւ����삩�`�F�b�N
			if( CheckInputControl(KEY_SWITCHWEAPON, 1) ){
				ObjMgr.ChangeWeapon(PlayerID, -1);
			}
			if( CheckInputControl(KEY_WEAPON1, 1) ){
				ObjMgr.ChangeWeapon(PlayerID, 0);
			}
			if( CheckInputControl(KEY_WEAPON2, 1) ){
				ObjMgr.ChangeWeapon(PlayerID, 1);
			}

			//����̔p�����삩�`�F�b�N
			if( CheckInputControl(KEY_DROPWEAPON, 1) ){
				ObjMgr.DumpWeapon(PlayerID);
			}

			//�X�R�[�v���삩�`�F�b�N
			if( CheckInputControl(KEY_ZOOM, 1) ){
				ObjMgr.ChangeScopeMode(PlayerID);
			}

			//�A�˃��[�h�ύX���삩�`�F�b�N
			if( CheckInputControl(KEY_ShotMODE, 1) ){
				ObjMgr.ChangeShotMode(PlayerID);
			}

			//�J��������
			if( Camera_F1mode == true ){
				if( inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(0x0C) ) ){	//NUM8
					view_ry -= DegreeToRadian(2);
				}
				if( inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(0x09) ) ){	//NUM5
					view_ry += DegreeToRadian(2);
				}
				if( inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(0x08) ) ){	//NUM4
					view_rx -= DegreeToRadian(2);
				}
				if( inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(0x0A) ) ){	//NUM6
					view_rx += DegreeToRadian(2);
				}
			}

			//���Z�E�㏸�̑��삩�`�F�b�N
			if( (inputCtrl->CheckKeyNow( GetFunctionKeycode(5) ))&&(inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x0F))) ){	// F5 + [ENTER]
				Cmd_F5 = true;
			}
			else{
				Cmd_F5 = false;
			}

			//���Z�E�e�ǉ��̑��삩�`�F�b�N
			if( inputCtrl->CheckKeyNow( GetFunctionKeycode(6) ) ){
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F)) ){		// [ENTER]
					ObjMgr.CheatAddBullet(PlayerID);
				}
			}

			//���Z�E����ύX�̑��삩�`�F�b�N
			if( inputCtrl->CheckKeyNow( GetFunctionKeycode(7) ) ){
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x02)) ){		// [��]
					int id_param = myHuman->GetMainWeaponTypeNO();

					//���̕���ԍ����v�Z
					if( id_param >= TOTAL_PARAMETERINFO_WEAPON-1 ){ id_param = 0; }
					else{ id_param += 1; }

					ObjMgr.CheatNewWeapon(PlayerID, id_param);
				}
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x03)) ){		// [��]
					int id_param = myHuman->GetMainWeaponTypeNO();

					//���̕���ԍ����v�Z
					if( id_param <= 0 ){ id_param = TOTAL_PARAMETERINFO_WEAPON-1; }
					else{ id_param -= 1; }

					ObjMgr.CheatNewWeapon(PlayerID, id_param);
				}
			}

			//���Z�E�l�ύX�̑��삩�`�F�b�N
			if( inputCtrl->CheckKeyNow( GetFunctionKeycode(8) ) ){
				int Player_HumanID;

				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x02)) ){		// [��]
					//���݂̃v���C���[�ԍ����擾
					Player_HumanID = ObjMgr.GetPlayerID();

					//���̐l���v�Z
					Player_HumanID += 1;
					if( Player_HumanID >= MAX_HUMAN ){ Player_HumanID = 0; }

					//�Ώۃv���C���[�ԍ���K�p
					ObjMgr.SetPlayerID(Player_HumanID);

					//�v���C���[�̌������擾
					ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

					//F1���[�h���ɃJ�����̌������Đݒ�
					if( Camera_F1mode == true ){
						camera_rx = DegreeToRadian(90);
					}
				}
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x03)) ){		// [��]
					//���݂̃v���C���[�ԍ����擾
					Player_HumanID = ObjMgr.GetPlayerID();

					//���̐l���v�Z
					Player_HumanID -= 1;
					if( Player_HumanID < 0 ){ Player_HumanID = MAX_HUMAN-1; }

					//�Ώۃv���C���[�ԍ���K�p
					ObjMgr.SetPlayerID(Player_HumanID);

					//�v���C���[�̌������擾
					ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

					//F1���[�h���ɃJ�����̌������Đݒ�
					if( Camera_F1mode == true ){
						camera_rx = DegreeToRadian(90);
					}
				}
			}

			//���Z�E�l�ǉ��̑��삩�`�F�b�N
			if( inputCtrl->CheckKeyNow( GetFunctionKeycode(9) ) ){
				if( (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x00)))||(inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x01))) ){		// [��]�E[��]
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

					//�v���C���[�̍��W�═����擾
					myHuman->GetPosData(&x, &y, &z, &r);
					myHuman->GetParamData(&param, &dataid, NULL, &team);
					myHuman->GetWeapon(&selectweapon, weapon);
					for(int i=0; i<TOTAL_HAVEWEAPON; i++){
						if( weapon[i] != NULL ){
							weapon[i]->GetParamData(&weapon_paramid[i], NULL, NULL);
						}
					}

					//�v���C���[�̖ڂ̑O�̍��W���擾
					x += cos(r*-1 + (float)M_PI/2)*10.0f;
					y += 5.0f;
					z += sin(r*-1 + (float)M_PI/2)*10.0f;

					//�l��ǉ�
					id = ObjMgr.AddHumanIndex(x, y, z, r, param, team, weapon_paramid);
					if( id >= 0 ){
						ObjMgr.ChangeWeapon(id, selectweapon);

						//AI��ݒ�
						HumanAI[id].Init();
						if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x00)) ){		// [��]
							HumanAI[id].SetHoldTracking(dataid);
						}
						if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x01)) ){		// [��]
							HumanAI[id].SetHoldWait(x, z, r);
						}
					}
				}
			}

			//���Z�E�r�\���̑��삩�`�F�b�N
			if( inputCtrl->CheckKeyDown( GetHomeKeycode() ) ){
				if( Camera_HOMEmode == false ){
					Camera_HOMEmode = true;
				}
				else{
					Camera_HOMEmode = false;
				}
			}
		}

#ifdef ENABLE_DEBUGCONSOLE
	}
	if( Show_Console == true ){
		InputConsole();
	}
#endif

	time_input = GetTimeMS() - time;
}

void maingame::Process()
{
	//�v���C���[�̃N���X���擾
	human *myHuman = ObjMgr.GetPlayerHumanObject();

	int ontarget, kill, headshot;

	//-----------------------------------

	time = GetTimeMS();

	//����̎��������擾
	int weaponid;
	WeaponParameter data;
	weaponid = myHuman->GetMainWeaponTypeNO();
	GameParamInfo.GetWeapon(weaponid, &data);

	//�v���C���[�i�I�u�W�F�N�g�j�̌�����ݒ�
	if( (PlayerAI == false)&&(myHuman->GetHP() > 0) ){
		myHuman->SetRxRy(mouse_rx, mouse_ry);
	}

	//�I�u�W�F�N�g�}�l�[�W���[�����s
	if( Cmd_F5 == true ){
		ObjMgr.Process( ObjMgr.GetPlayerID() , camera_rx, camera_ry);
	}
	else{
		ObjMgr.Process(-1, camera_rx, camera_ry);
	}

	//�v���C���[�̐�������Z
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
			//AI�����s
			HumanAI[i].Process();
		}
	}
	time_process_ai = GetTimeMS() - time;

	//-----------------------------------

	//�~�b�V�������I�����Ă��Ȃ���΁A
	if( end_framecnt == 0 ){
		int check = ObjMgr.CheckGameOverorComplete();

		//�Q�[���N���A�[����
		if( check == 1 ){
			end_framecnt += 1;
			MainGameInfo.missioncomplete = true;
		}

		//�Q�[���I�[�o�[����
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
			//�C�x���g���s
			for(int i=0; i<TOTAL_EVENTLINE; i++){
				SetMessageID = false;
				Event[i].Execution(&end_framecnt, &MainGameInfo.missioncomplete, &message_id, &SetMessageID);

				//�C�x���g���b�Z�[�W���ăZ�b�g����Ă�����A�J�E���g��߂��B
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
		//�v���C���[�̌������擾
		ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);
	}

	//�J�������[�N�����߂�
	if( Camera_Debugmode == true ){
		//
	}
	else if( myHuman->GetHP() <= 0 ){
		float rx = DegreeToRadian(end_framecnt);
		float ry = DegreeToRadian(-85);
		float r = 25.0f;

		camera_x = x + cos(rx)*cos(ry)*r;
		camera_y = y - sin(ry)*r;
		camera_z = z + sin(rx)*cos(ry)*r;
		camera_rx = rx;
		camera_ry = ry;
	}
	else if( Camera_F1mode == true ){
		float crx = camera_rx*0.6f + (view_rx + mouse_rx*-1 + (float)M_PI/2)*0.4f;		// 3/5 + 2/5
		float cry = camera_ry*0.6f + (view_ry + mouse_ry)*0.4f;							// 3/5 + 2/5
		float dist;
		if( CollD.CheckALLBlockIntersectRay(x, y + HUMAN_HEIGTH, z, cos(crx)*cos(cry)*-1, sin(cry*-1), sin(crx)*cos(cry)*-1, NULL, NULL, &dist, 13.0f) == true ){
			dist -= 1.0f;
		}
		else{
			dist = 13.0f;
		}
		camera_x = x - cos(crx)*cos(cry)*dist;
		camera_y = y + HUMAN_HEIGTH + sin(cry*-1)*dist;
		camera_z = z - sin(crx)*cos(cry)*dist;
		camera_rx = crx;
		camera_ry = cry;
	}
	else{
		camera_x = x;
		camera_y = y + VIEW_HEIGHT;
		camera_z = z;
		camera_rx = view_rx + mouse_rx*-1 + (float)M_PI/2;
		camera_ry = view_ry + mouse_ry;
	}

	//�_���[�W���󂯂Ă���΁A���b�h�t���b�V����`�悷��
	redflash_flag = myHuman->CheckHit();

	//-----------------------------------

#ifdef ENABLE_DEBUGCONSOLE
	//�f�o�b�N�p�R���\�[����\�����Ă���A���~�b�V�����I���̃J�E���g���n�܂��Ă��Ȃ���΁`
	if( (Show_Console == true)&&(end_framecnt == 0) ){
		//�f�o�b�N�p�R���\�[���̃��C������
		ProcessConsole();
	}
#endif

	//-----------------------------------

	framecnt += 1;
	if( start_framecnt < (int)(1.0f*GAMEFPS) ){	//�~�b�V�����J�n���Ȃ�
		start_framecnt += 1;
	}
	if( end_framecnt == 1 ){					//�~�b�V�����I������Ȃ��
		MainGameInfo.framecnt = framecnt;
		GameInfoData = MainGameInfo;	//�S�ăR�s�[
		end_framecnt += 1;
	}
	else if( end_framecnt > 0 ){				//�~�b�V�����I�����Ȃ��
		if( end_framecnt < (int)(5.0f*GAMEFPS) ){
			end_framecnt += 1;
		}
		else{
			GameInfoData.fire = MainGameInfo.fire;			//�ˌ���
			GameInfoData.ontarget = MainGameInfo.ontarget;	//������
			GameInfoData.kill = MainGameInfo.kill;			//�|�����G�̐�
			GameInfoData.headshot = MainGameInfo.headshot;	//�G�̓����ɖ���������

			GameState->PushMouseButton();
		}
	}
}

void maingame::Sound()
{
	time = GetTimeMS();

	//�T�E���h���Đ�
	GameSound->PlayWorldSound(camera_x, camera_y, camera_z, camera_rx);

	time_sound = GetTimeMS() - time;
}

void maingame::Render3D()
{
	time = GetTimeMS();


	int skymodel, skytexture;
	human *myHuman = ObjMgr.GetPlayerHumanObject();

	//�t�H�O�ƃJ������ݒ�
	d3dg->SetFog(MIFdata.GetSkynumber());
	if( Camera_F1mode == false ){
		int scopemode = myHuman->GetScopeMode();
		float viewangle = 0.0f;

		//�X�R�[�v�ɂ�鎋��p������
		if( scopemode == 0 ){ viewangle = VIEWANGLE_NORMAL; }
		if( scopemode == 1 ){ viewangle = VIEWANGLE_SCOPE_1; }
		if( scopemode == 2 ){ viewangle = VIEWANGLE_SCOPE_2; }

		d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, viewangle);
	}
	else{
		d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);
	}

	//�J�������W�ɔw�i���`��
	d3dg->SetWorldTransform(camera_x, camera_y, camera_z, 0.0f, 0.0f, 2.0f);
	Resource.GetSkyModelTexture(&skymodel, &skytexture);
	d3dg->RenderModel(skymodel, skytexture);

	//Z�o�b�t�@��������
	d3dg->ResetZbuffer();

	if( CenterLine == true ){
		//���S���`��i�f�o�b�N�p�j
		d3dg->Centerline();
	}

	//�}�b�v��`��
	d3dg->ResetWorldTransform();
	d3dg->DrawMapdata(wireframe);

	//�v���C���[�̕`��L���̌���
	int DrawPlayer = 0;
	if( (Camera_F1mode == false)&&(Camera_Debugmode == false)&&(myHuman->GetHP() > 0) ){
		if( Camera_HOMEmode == false ){
			DrawPlayer = 1;
		}
		else{
			DrawPlayer = 2;
		}
	}
	//�I�u�W�F�N�g��`��
	ObjMgr.Render(camera_x, camera_y, camera_z, DrawPlayer);
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

	//�e��ݒ��Q�[�������擾
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

	//���b�h�t���b�V���`��
	if( redflash_flag == true ){
		d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
		redflash_flag = false;
	}

	//�X�R�[�v�\��
	if( (Camera_F1mode == false)&&(myHuman->GetScopeMode() == 1) ){
		d3dg->Draw2DTexture(0, 0, Resource.GetScopeTexture(), SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
		d3dg->Draw2DLine(SCREEN_WIDTH/2-49, SCREEN_HEIGHT/2, SCREEN_WIDTH/2-4, SCREEN_HEIGHT/2, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH/2+4, SCREEN_HEIGHT/2, SCREEN_WIDTH/2+49, SCREEN_HEIGHT/2, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2-49, SCREEN_WIDTH/2, SCREEN_HEIGHT/2-4, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2+4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2+49, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DBox(SCREEN_WIDTH/2-50, SCREEN_HEIGHT/2-1, SCREEN_WIDTH/20+50, SCREEN_HEIGHT/2+1, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
		d3dg->Draw2DBox(SCREEN_WIDTH/2-1, SCREEN_HEIGHT/2-50, SCREEN_WIDTH/2+1, SCREEN_HEIGHT/2+50, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
	}
	if( (Camera_F1mode == false)&&(myHuman->GetScopeMode() == 2) ){
		d3dg->Draw2DTexture(0, 0, Resource.GetScopeTexture(), SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
		d3dg->Draw2DLine(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		d3dg->Draw2DBox(0, SCREEN_HEIGHT/2-1, SCREEN_WIDTH, SCREEN_HEIGHT/2+1, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
		d3dg->Draw2DBox(SCREEN_WIDTH/2-1, 0, SCREEN_WIDTH/2+1, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));
	}

	//�ډB���\��
	if( (Camera_Blind == true)&&(Camera_Debugmode == false)&&(hp > 0) ){

		int scopemode = myHuman->GetScopeMode();
		float addang;
		float adddist = 1.2f;

		//�X�R�[�v�ɂ�鎋��p������
		if( scopemode == 0 ){ addang = VIEWANGLE_NORMAL / 4; }
		if( scopemode == 1 ){ addang = VIEWANGLE_SCOPE_1 / 4; }
		if( scopemode == 2 ){ addang = VIEWANGLE_SCOPE_2 / 4; }

		//��
		if( CollD.CheckALLBlockInside(camera_x + cos(camera_rx)*cos(camera_ry + addang) * adddist, camera_y + sin(camera_ry + addang) * adddist, camera_z + sin(camera_rx)*cos(camera_ry + addang) * adddist) == true ){
			d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/2, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		}

		//��
		if( CollD.CheckALLBlockInside(camera_x + cos(camera_rx)*cos(camera_ry - addang) * adddist, camera_y + sin(camera_ry - addang) * adddist, camera_z + sin(camera_rx)*cos(camera_ry - addang) * adddist) == true ){
			d3dg->Draw2DBox(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		}

		//��
		if( CollD.CheckALLBlockInside(camera_x + cos(camera_rx + addang)*cos(camera_ry) * adddist, camera_y + sin(camera_ry) * adddist, camera_z + sin(camera_rx + addang)*cos(camera_ry) * adddist) == true ){
			d3dg->Draw2DBox(0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		}

		//�E
		if( CollD.CheckALLBlockInside(camera_x + cos(camera_rx - addang)*cos(camera_ry) * adddist, camera_y + sin(camera_ry) * adddist, camera_z + sin(camera_rx - addang)*cos(camera_ry) * adddist) == true ){
			d3dg->Draw2DBox(SCREEN_WIDTH/2, 0, SCREEN_WIDTH, SCREEN_HEIGHT, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
		}
	}

	//�f�o�b�N�p�E�Q�[�����̕\��
	if( (ShowInfo_Debugmode == true)||(Camera_Debugmode == true) ){
		//�V�X�e���t�H���g�ɂ��\���@���₷���E�d��
		/*
		sprintf(str, "OpenXOPS �e�X�g\ncamera x:%.2f y:%.2f z:%.2f rx:%.2f ry:%.2f\n"
			"human[%d]�Fx:%.2f y:%.2f z:%.2f rx:%.2f\n"
			"I:%02dms PO:%02dms PA:%02dms PE:%02dms R:%02dms",
			camera_x, camera_y, camera_z, camera_rx, camera_ry,
			ObjMgr.GetPlayerID(), human_x, human_y, human_z, human_rx,
			time_input, time_process_object, time_process_ai, time_process_event, time_render);
		d3dg->Draw2DMSFontText(10+1, 10+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f));
		d3dg->Draw2DMSFontText(10, 10, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f));
		*/

		//�e�N�X�`���t�H���g�ɂ��\���@�y���E���ɂ���
		sprintf(str, "frame:%d   time %02d:%02d", framecnt, framecnt/(int)GAMEFPS/60, framecnt/(int)GAMEFPS%60);
		d3dg->Draw2DTextureFontText(10+1, 10+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f), 10, 14);
		d3dg->Draw2DTextureFontText(10, 10, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 10, 14);
		sprintf(str, "camera x:%.2f y:%.2f z:%.2f rx:%.2f ry:%.2f", camera_x, camera_y, camera_z, camera_rx, camera_ry);
		d3dg->Draw2DTextureFontText(10+1, 30+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f), 10, 14);
		d3dg->Draw2DTextureFontText(10, 30, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 10, 14);
		sprintf(str, "human[%d] x:%.2f y:%.2f z:%.2f rx:%.2f HP:%d", ObjMgr.GetPlayerID(), human_x, human_y, human_z, human_rx, hp);
		d3dg->Draw2DTextureFontText(10+1, 50+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f), 10, 14);
		d3dg->Draw2DTextureFontText(10, 50, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 10, 14);
		sprintf(str, "Input:%02dms Object:%02dms AI:%02dms Event:%02dms Sound:%02dms", time_input, time_process_object, time_process_ai, time_process_event, time_sound);
		d3dg->Draw2DTextureFontText(10+1, 70+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f), 10, 14);
		d3dg->Draw2DTextureFontText(10, 70, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 10, 14);
		sprintf(str, "Render:%02dms", time_render);
		d3dg->Draw2DTextureFontText(10+1, 90+1, str, d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f), 10, 14);
		d3dg->Draw2DTextureFontText(10, 90, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 10, 14);
	}

	//�Q�[�����s���x�̕\��
	if( 1 ){
		sprintf(str, "fps:%.2f", fps);
	}
	else{
		int speed = (int)(fps / (1000.0f/GAMEFRAMEMS) * 100);
		sprintf(str, "PROCESSING SPEED %d%%", speed);
	}
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH - strlen(str)*14 - 14 +1, 10+1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 14, 18);
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH - strlen(str)*14 - 14, 10, str, d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f), 14, 18);

	//HUD�\���E���[�hA
	if( Camera_F2mode == 0 ){
		//�����G���A�`��
		//"��������"
		stru[0] = 0xB3;		stru[1] = 0xB4;		stru[2] = 0xB4;		stru[3] = 0xB4;		stru[4] = 0xB4;
		stru[5] = 0xB4;		stru[6] = 0xB4;		stru[7] = 0xB5;		stru[8] = 0x00;
		d3dg->Draw2DTextureFontText(15, SCREEN_HEIGHT - 105, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		//"��������"
		for(int i=0; stru[i] != 0x00; i++){ stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(15, SCREEN_HEIGHT - 105 +32, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		//"���������"
		stru[0] = 0xB3;		stru[1] = 0xB4;		stru[2] = 0xB4;		stru[3] = 0xB6;		stru[4] = 0xB7;
		stru[5] = 0xB7;		stru[6] = 0xB7;		stru[7] = 0xB8;		stru[8] = 0xB9;		stru[9] = 0x00;
		d3dg->Draw2DTextureFontText(15, SCREEN_HEIGHT - 55, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		//"���������"
		for(int i=0; stru[i] != 0x00; i++){ stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(15, SCREEN_HEIGHT - 55 +32, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);

		//�E���G���A�p�����R�[�h�ݒ�
		stru[0] = 0xB0;//'�';
		for(int i=1; i<HUDA_WEAPON_SIZEW-1; i++){
			stru[i] = 0xB1;//'�';
		}
		stru[HUDA_WEAPON_SIZEW-1] = 0xB2;//'�';
		stru[HUDA_WEAPON_SIZEW] = 0x00;

		//�E���G���A�`��
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		for(int i=0; i<HUDA_WEAPON_SIZEW; i++){ stru[i] += 0x10; }
		for(int i=1; i<HUDA_WEAPON_SIZEH-1; i++){
			d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY + 32*i, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);
		}
		for(int i=0; i<HUDA_WEAPON_SIZEW; i++){ stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY + 32*(HUDA_WEAPON_SIZEH-1), (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,0.5f), 32, 32);

		//����̒e���\��
		sprintf((char*)stru, "A%d B%d", lnbs, (nbs - lnbs));
		for(int i=0; i<(int)strlen((char*)stru); i++){
			if( stru[i] == 'A' ){ stru[i] = 0xBB; }	//'�'
			if( stru[i] == 'B' ){ stru[i] = 0xBA; }	//'�'
		}
		d3dg->Draw2DTextureFontText(25, SCREEN_HEIGHT - 96, (char*)stru, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 23, 24);

		//HP�ɂ��F�̌���
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

		//HP�\��
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

		//���햼�\��
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX + 9, HUDA_WEAPON_POSY + 4, weaponname, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 16, 20);
	}

	//HUD�\���E���[�hB
	if( Camera_F2mode == 1 ){
		//��ʎ���̐�
		d3dg->Draw2DLine(0, 0, SCREEN_WIDTH-1, 0, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));
		d3dg->Draw2DLine(SCREEN_WIDTH-1, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));
		d3dg->Draw2DLine(0, 0, 0, SCREEN_HEIGHT-1, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));
		d3dg->Draw2DLine(0, SCREEN_HEIGHT-1, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));

		//���햼�\��
		d3dg->Draw2DBox(8, SCREEN_HEIGHT - 32, 227, SCREEN_HEIGHT - 7, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.3f));
		d3dg->Draw2DTextureFontText(10, SCREEN_HEIGHT - 30, weaponname, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 16, 20);
	}

	//���[�_�[�`��
	if( radar == true ){
		RenderRadar();
	}

	//�C�x���g���b�Z�[�W�\��
	if( (message_id != -1)&&(message_cnt < (int)(TOTAL_EVENTENT_SHOWMESSEC*GAMEFPS)) ){
		char messtr[MAX_POINTMESSAGEBYTE];
		PointData.GetMessageText(messtr, message_id);
		float effectA = 1.0f;
		if( message_cnt < (int)(0.2f*GAMEFPS) ){ effectA = GetEffectAlpha(message_cnt, 1.0f, 0.2f, 0.0f, false); }
		if( (int)((TOTAL_EVENTENT_SHOWMESSEC-0.2f)*GAMEFPS) < message_cnt ){ effectA = GetEffectAlpha(message_cnt, 1.0f, 0.2f, (TOTAL_EVENTENT_SHOWMESSEC - 0.2f), true); }
		d3dg->Draw2DMSFontTextCenter(0 +1, SCREEN_HEIGHT - 140 +1, SCREEN_WIDTH, 140, messtr, d3dg->GetColorCode(0.1f,0.1f,0.1f,effectA));
		d3dg->Draw2DMSFontTextCenter(0, SCREEN_HEIGHT - 140, SCREEN_WIDTH, 140, messtr, d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA));
	}

	//�����[�h�\��
	if( reloadcnt > 0 ){
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 145 +3, SCREEN_HEIGHT - 180+3, "RELOADING", d3dg->GetColorCode(0.2f,0.2f,0.2f,1.0f), 32, 34);
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 145, SCREEN_HEIGHT - 180, "RELOADING", d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 32, 34);
	}

	//����؂�ւ��\��
	if( selectweaponcnt > 0 ){
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 130 +3, SCREEN_HEIGHT - 180+3, "CHANGING", d3dg->GetColorCode(0.2f,0.2f,0.2f,1.0f), 32, 34);
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT - 180, "CHANGING", d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 32, 34);
	}

	//�Ə��\��
	if( (Camera_F1mode == false)&&(param_WeaponP != 2) ){
		if( (weapon[selectweapon] != NULL)&&(myHuman->GetScopeMode() == 0)&&(param_scopemode != 2) ){
			if( (end_framecnt == 0)||(GameInfoData.missioncomplete == true) ){
				if( GameConfig.GetAnotherGunsightFlag() ){	//�I�v�V�����^
					//�Ə��̓����x
					float alpha = 1.0f - (float)ErrorRange/40.0f;
					if( alpha < 0.0f ){ alpha = 0.0f; }

					d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2+4, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2-15, SCREEN_HEIGHT/2+15, SCREEN_WIDTH/2-19, SCREEN_HEIGHT/2+19, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2+15, SCREEN_HEIGHT/2+15, SCREEN_WIDTH/2+19, SCREEN_HEIGHT/2+19, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2-4, SCREEN_HEIGHT/2+4, SCREEN_WIDTH/2+4, SCREEN_HEIGHT/2+4, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));

					d3dg->Draw2DLine(SCREEN_WIDTH/2-4 - ErrorRange, SCREEN_HEIGHT/2-4 - ErrorRange/2, SCREEN_WIDTH/2-4 - ErrorRange, SCREEN_HEIGHT/2+4 + ErrorRange/2, d3dg->GetColorCode(1.0f,0.0f,0.0f,alpha));
					d3dg->Draw2DLine(SCREEN_WIDTH/2+4 + ErrorRange, SCREEN_HEIGHT/2-4 - ErrorRange/2, SCREEN_WIDTH/2+4 + ErrorRange, SCREEN_HEIGHT/2+4 + ErrorRange/2, d3dg->GetColorCode(1.0f,0.0f,0.0f,alpha));
				}
				else{										//�W���^
					d3dg->Draw2DLine(SCREEN_WIDTH/2-13, SCREEN_HEIGHT/2, SCREEN_WIDTH/2-3, SCREEN_HEIGHT/2, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2+13, SCREEN_HEIGHT/2, SCREEN_WIDTH/2+3, SCREEN_HEIGHT/2, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2-13, SCREEN_WIDTH/2, SCREEN_HEIGHT/2-3, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
					d3dg->Draw2DLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2+13, SCREEN_WIDTH/2, SCREEN_HEIGHT/2+3, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));

					stru[0] = 0xBD;		stru[1] = 0x00;	//"�"
					d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 16 - ErrorRange, SCREEN_HEIGHT/2 - 16, (char*)stru, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f), 32, 32);
					stru[0] = 0xBE;		stru[1] = 0x00;	//"�"
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


	//-----------------------------------

	//Z�o�b�t�@��������
	d3dg->ResetZbuffer();


	//HUD�\���E���[�hA
	if( Camera_F2mode == 0 ){
		int notselectweapon = selectweapon + 1;
		if( notselectweapon == TOTAL_HAVEWEAPON ){ notselectweapon = 0; }

		//�i3D�`��j�J�������W���b��ݒ�
		d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);

		//�i3D�`��j�������Ă��镐�탂�f���̕`��E���C������
		GameParamInfo.GetWeapon(weapon_paramid[selectweapon], &weapon_paramdata);
		Resource.GetWeaponModelTexture(weapon_paramid[selectweapon], &weaponmodel, &weapontexture);
		d3dg->SetWorldTransformPlayerWeapon(true, camera_x, camera_y, camera_z, camera_rx, camera_ry, DegreeToRadian(framecnt*2), weapon_paramdata.size);
		d3dg->RenderModel(weaponmodel, weapontexture);

		//�i3D�`��j�������Ă��镐�탂�f���̕`��E�T�u����
		GameParamInfo.GetWeapon(weapon_paramid[notselectweapon], &weapon_paramdata);
		Resource.GetWeaponModelTexture(weapon_paramid[notselectweapon], &weaponmodel, &weapontexture);
		d3dg->SetWorldTransformPlayerWeapon(false, camera_x, camera_y, camera_z, camera_rx, camera_ry, 0.0f, weapon_paramdata.size);
		d3dg->RenderModel(weaponmodel, weapontexture);
	}

	//-----------------------------------

#ifdef ENABLE_DEBUGCONSOLE
	if( Show_Console == true ){
		if( ScreenShot == false ){
			RenderConsole();
		}
	}
#endif

	//-----------------------------------


	//�X�^�[�g���ƏI�����̃u���b�N�A�E�g�ݒ�
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

	//�I�����̕����\��
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

//! @brief ���[�_�[�ɕ\��������W�ɕϊ�
//! @param in_x ��� X���W
//! @param in_y ��� Y���W
//! @param in_z ��� Z���W
//! @param RadarPosX ���[�_�[�̕`�� X���W�i�����j
//! @param RadarPosY ���[�_�[�̕`�� Y���W�i�����j
//! @param RadarSize ���[�_�[�̕`��T�C�Y
//! @param RadarWorldR ���[�_�[�Ƀ|�C���g���鋗��
//! @param out_x 2D X���W ���󂯎��|�C���^
//! @param out_y 2D X���W ���󂯎��|�C���^
//! @param local_y ���[�J�� Y���W ���󂯎��|�C���^�iNULL�j
//! @param check �v�Z�̏ȗ�
//! @return �����Ftrue�@���s�Ffalse
//! @attention check�t���O��L���ɂ���ƁA���[�_�[����O��邱�Ƃ����炩�ɂȂ������_�Ōv�Z���I�����Afalse��Ԃ��܂��B
bool maingame::GetRadarPos(float in_x, float in_y, float in_z, int RadarPosX, int RadarPosY, int RadarSize, float RadarWorldR, int *out_x, int *out_y, float *local_y, bool check)
{
	bool outf = false;
	float x, y, z, r, rx;
	float x2, z2, r2, rx2;

	//�J�����Ƃ̋������v�Z
	x = in_x - camera_x;
	y = in_y - camera_y;
	z = in_z - camera_z;

	//�߂���Ώ�������
	if( (check == false) || ((fabs(x) < RadarWorldR*2)&&(fabs(z) < RadarWorldR*2)&&(fabs(y) < 80.0f)) ){
		//�p�x���������v�Z
		rx = atan2(z, x);
		r = sqrt(x*x + z*z);

		//�J������̍��W���Čv�Z
		rx2 = (rx - camera_rx)*-1 - (float)M_PI/2;
		x2 = cos(rx2) * r;
		z2 = sin(rx2) * r;

		//���܂邩����
		if( (check == false) || ((fabs(x2) < RadarWorldR)&&(fabs(z2) < RadarWorldR)) ){
			//�`����W���v�Z
			r2 = r / RadarWorldR * (RadarSize/2);
			*out_x = (int)(RadarPosX+RadarSize/2 + cos(rx2) * r2);
			*out_y = (int)(RadarPosY+RadarSize/2 + sin(rx2) * r2);
			if( local_y != NULL ){ *local_y = y; }
			outf = true;
		}
	}

	return outf;
}

//! @brief �ȈՃ��[�_�[�\��
void maingame::RenderRadar()
{
	int RadarPosX = 10;				//���[�_�[�̕`�� X���W�i�����j
	int RadarPosY = 130;			//���[�_�[�̕`�� Y���W�i�����j
	int RadarSize = 200;			//���[�_�[�̕`��T�C�Y
	float RadarWorldR = 300.0f;		//���[�_�[�Ƀ|�C���g���鋗��

	float ecr = DISTANCE_CHECKPOINT / RadarWorldR * (RadarSize/2);

	//���n�ƈ͂�
	d3dg->Draw2DBox(RadarPosX, RadarPosY, RadarPosX+RadarSize, RadarPosY+RadarSize, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.6f));
	d3dg->Draw2DLine(RadarPosX, RadarPosY, RadarPosX+RadarSize, RadarPosY, d3dg->GetColorCode(0.0f,0.8f,0.0f,1.0f));
	d3dg->Draw2DLine(RadarPosX+RadarSize, RadarPosY, RadarPosX+RadarSize, RadarPosY+RadarSize, d3dg->GetColorCode(0.0f,0.8f,0.0f,1.0f));
	d3dg->Draw2DLine(RadarPosX+RadarSize, RadarPosY+RadarSize, RadarPosX, RadarPosY+RadarSize, d3dg->GetColorCode(0.0f,0.8f,0.0f,1.0f));
	d3dg->Draw2DLine(RadarPosX, RadarPosY+RadarSize, RadarPosX, RadarPosY, d3dg->GetColorCode(0.0f,0.8f,0.0f,1.0f));

	//�}�b�v��`��
	int bs = BlockData.GetTotaldatas();
	for(int i=0; i< bs; i++){
		blockdata bdata;
		float x_min, y_min, z_min, x_max, y_max, z_max;
		int vid[4];
		int bvx[4], bvy[4];

		//�u���b�N�̃f�[�^���擾
		BlockData.Getdata(&bdata, i);

		//�\�����̃u���b�N�����o�i�r���j
		CollD.GetBlockPosMINMAX(i, &x_min, &y_min, &z_min, &x_max, &y_max, &z_max);
		if( CollideBoxAABB(x_min, y_min, z_min, x_max, y_max, z_max, camera_x-RadarWorldR*2, camera_y-1.0f, camera_z-RadarWorldR*2, camera_x+RadarWorldR*2, camera_y+1.0f, camera_z+RadarWorldR*2) == true ){

			//�e�ʂ��Ƃɏ�������
			for(int j=0; j<6; j++){
				//�o��Ȃ��Ζʂ�����@���n�ʂ�K�i�Ȃǂ̌X�΂����O����
				float angle = acos(bdata.material[j].vy);
				if( (HUMAN_MAPCOLLISION_SLOPEANGLE < angle)&&(angle < DegreeToRadian(120)) ){

					//�u���b�N���_�f�[�^�̊֘A�t�����擾
					blockdataface(j, &(vid[0]), NULL);
					
					//4���_���v�Z
					for(int k=0; k<4; k++){
						GetRadarPos(bdata.x[ vid[k] ], bdata.y[ vid[k] ], bdata.z[ vid[k] ], RadarPosX, RadarPosY, RadarSize, RadarWorldR, &(bvx[k]), &(bvy[k]), NULL, false);
					}

					int line_x1, line_y1, line_x2, line_y2;

					//���[�_�[�̎l�p�`�Ɏ��܂�悤�ɕ`�悷��
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

	//�C�x���g�̓����|�C���g��`��
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
						//�����ɂ�铧���x
						if( (fabs(y) < 40.0f) ){
							alpha = 1.0f;
						}
						else{
							alpha = 0.5f;
						}

						//�}�[�J�[�`��
						d3dg->Draw2DCycle(x_2d, y_2d, (int)ecr, d3dg->GetColorCode(1.0f,0.5f,0.0f,alpha));
					}
				}
			}
		}
	}

	//�v���C���[�̏����擾
	int PlayerID = ObjMgr.GetPlayerID();
	int myteamid;
	ObjMgr.GeHumanObject(PlayerID)->GetParamData(NULL, NULL, NULL, &myteamid);

	//�l��`��
	for(int i=0; i<MAX_HUMAN; i++){
		human* thuman;
		float tx, ty, tz;
		int tteamid;
		float y;
		int x_2d, y_2d;
		float alpha;
		int color;

		//�l�̃I�u�W�F�N�g���擾
		thuman = ObjMgr.GeHumanObject(i);

		//�g���Ă��Ȃ��l�⎀�͖̂�������
		if( thuman->GetEnableFlag() == false ){ continue; }
		if( thuman->GetDeadFlag() == true ){ continue; }

		//�l�̏����擾
		thuman->GetPosData(&tx, &ty, &tz, NULL);
		thuman->GetParamData(NULL, NULL, NULL, &tteamid);
		ty += VIEW_HEIGHT;

		if( GetRadarPos(tx, ty, tz, RadarPosX, RadarPosY, RadarSize, RadarWorldR, &x_2d, &y_2d, &y, true) == true ){
			//�����ɂ�铧���x
			if( (fabs(y) < 40.0f) ){
				alpha = 1.0f;
			}
			else{
				alpha = 0.5f;
			}

			//�}�[�J�[�̐F������
			if( PlayerID == i ){ color = d3dg->GetColorCode(1.0f,1.0f,0.0f,alpha); }				//�v���C���[���g
			else if( tteamid == myteamid ){ color = d3dg->GetColorCode(0.0f,0.5f,1.0f,alpha); }	//����
			else{ color = d3dg->GetColorCode(1.0f,0.0f,0.5f,alpha); }								//�G

			//�}�[�J�[�`��
			d3dg->Draw2DBox(x_2d-3, y_2d-3, x_2d+3, y_2d+3, color);
		}
	}
}

#ifdef ENABLE_DEBUGCONSOLE

//! @brief �f�o�b�N�p�R���\�[���ɐV���ȕ������ǉ�
//! @param color �����̐F
//! @param str �ǉ����镶����̃|�C���^
//! @attention �V����������͏�ɉ�����ǉ�����܂��B
//! @attention �\���\�s���i�萔�FMAX_CONSOLELINES�j������ꍇ�A�ŏ��̍s�i1�s�ځj���폜��1�s�����炵����ŁA��ԉ��̍s�ɒǉ����܂��B
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

//! @brief ���͗p�R���\�[���ɕ������ꕶ���ǉ�
//! @param inchar �ǉ����镶��
void maingame::ConsoleInputText(char inchar)
{
	for(int i=0; i<MAX_CONSOLELEN; i++){
		if( InputConsoleData->textdata[i] == NULL ){
			InputConsoleData->textdata[i] = inchar;
			InputConsoleData->textdata[i+1] = NULL;
			return;
		}
	}
}

//! @brief ���͗p�R���\�[���̕������ꕶ���폜
void maingame::ConsoleDeleteText()
{
	int s = strlen(InputConsoleData->textdata);
	if( s == (int)strlen(CONSOLE_PROMPT) ){ return; }
	InputConsoleData->textdata[ s-1 ] = NULL;
}

//! @brief �R�}���h�̔��肨��ш����i�����l�j���擾
//! @param cmd ���肷��R�}���h�����̃|�C���^
//! @param num �^����ꂽ�������󂯎��|�C���^
//! @return �擾�Ftrue�@����O�Ffalse
bool maingame::GetCommandNum(char *cmd, int *num)
{
	char str[MAX_CONSOLELEN];

	//�R�}���h���𒲂ׂ�
	strcpy(str, NewCommand);
	str[ strlen(cmd) ] = NULL;
	if( strcmp(str, cmd) != 0 ){ return false; }

	//�u�R�}���h��_X�v���̕������ɒB���Ă��邩�ǂ���
	if( strlen(cmd)+2 > strlen(NewCommand) ){ return false; }

	//�^����ꂽ�����𒲂ׂ�
	*num = atoi(&(NewCommand[ strlen(cmd)+1 ]));
	return true;
}

//! @brief �f�o�b�N�p�R���\�[���̓��͏���
void maingame::InputConsole()
{
	NewCommand[0] = NULL;

	if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F)) ){		// [ENTER]
		//�������͂���Ă���΁`
		if( strcmp(InputConsoleData->textdata, CONSOLE_PROMPT) != 0 ){
			//�R���\�[���ɒǉ����A�������R�}���h�Ƃ��ēo�^
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), InputConsoleData->textdata);
			strcpy(NewCommand, &(InputConsoleData->textdata[strlen(CONSOLE_PROMPT)]));
		}

		//���̓G���A��������
		strcpy(InputConsoleData->textdata, CONSOLE_PROMPT);
	}
	else{
		//����
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

		//�A���t�@�x�b�g������
		for(char key=0x20; key<=0x39; key++){
			if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
				ConsoleInputText(key + 0x41);
			}
		}

		//�X�y�[�X
		if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x11)) ){		// [SPACE]
			ConsoleInputText(' ');
		}

		//�ꕶ���폜
		if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0E)) ){		// [BACK]
			ConsoleDeleteText();
		}
	}
}

//! @brief �f�o�b�N�p�R���\�[���̃��C������
void maingame::ProcessConsole()
{
	char str[MAX_CONSOLELEN];
	int id;

	//�R�}���h���X�g
	if( strcmp(NewCommand, "help") == 0 ){
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "help        human        result      event");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "ver");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "info        view         center      map");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "tag         radar        inmap");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "revive      treat <NUM>  nodamage <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "kill <NUM>  break <NUM>  newobj <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "bot         nofight      caution     stop");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "estop       ss           clear");
	}

	//�l�̓��v���
	if( strcmp(NewCommand, "human") == 0 ){
		int alivemyfriend = 0;
		int myfriend = 0;
		int aliveenemy = 0;
		int enemy = 0;
		int myteamid;

		//�v���C���[�̃`�[���ԍ����擾
		ObjMgr.GetPlayerHumanObject()->GetParamData(NULL, NULL, NULL, &myteamid);

		for(int i=0; i<MAX_HUMAN; i++){
			int teamid;
			bool deadflag;
			human *thuman = ObjMgr.GeHumanObject(i);
			if( thuman->GetEnableFlag() == true ){
				//���S��Ԃƃ`�[���ԍ����擾
				deadflag = thuman->GetDeadFlag();
				thuman->GetParamData(NULL, NULL, NULL, &teamid);

				//�J�E���g
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

	//�b�胊�U���g�\��
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

	//�C�x���g�^�X�N�\��
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

	//�f�o�b�N�p�����̕\��
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

	//�t���[�J�����؂�ւ�
	if( strcmp(NewCommand, "view") == 0 ){
		if( Camera_Debugmode == false ){
			Camera_Debugmode = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Change FreeView mode.");
		}
		else{
			Camera_Debugmode = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Change PlayerView mode.");
		}

		//�f�o�b�N���[�h�J�n���̃J�����ݒ�
		if( Camera_Debugmode == true ){
			camera_x = 100.0f;
			camera_y = 100.0f;
			camera_z = -100.0f;
			camera_rx = DegreeToRadian(135);
			camera_ry = DegreeToRadian(-40);
		}
	}

	//3D�̒��S���\��
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

	//�}�b�v�����C���[�t���[����
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

	//�I�u�W�F�N�g�̃^�O��\��
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

	//���[�_�[��\��
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

	//�ډB����\��
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

	//�S�Ă̎��҂�h������
	if( strcmp(NewCommand, "revive") == 0 ){
		for(int i=0; i<MAX_HUMAN; i++){
			ObjMgr.HumanResuscitation(i);
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "All human has resuscitation.");
	}

	//�񕜂���
	if( GetCommandNum("treat", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			int param, hp;
			HumanParameter data;
			human *thuman = ObjMgr.GeHumanObject(id);
			
			//��������HP���擾
			thuman->GetParamData(&param, NULL, NULL, NULL);
			GameParamInfo.GetHuman(param, &data);
			hp = data.hp;

			//��
			if( thuman->SetHP(hp) == true ){
				sprintf(str, "Set the HP:%d to Human[%d].", data.hp, id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//���G��
	if( GetCommandNum("nodamage", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			if( InvincibleID == id ){
				//�����ԍ����w�肳�ꂽ��Ȃ�A������
				InvincibleID = -1;
				ObjMgr.GeHumanObject(id)->SetInvincibleFlag(false);
				sprintf(str, "Not invincible Human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
			else{
				//���ɒN�����w�肳��Ă�����A���Ɏw�肳��Ă���l�𖳌���
				if( InvincibleID != -1 ){
					ObjMgr.GeHumanObject(InvincibleID)->SetInvincibleFlag(false);
					sprintf(str, "Not invincible Human[%d].", InvincibleID);
					AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
				}

				//�V���ɖ��G��Ԃ̐l���w��
				InvincibleID = id;
				ObjMgr.GeHumanObject(id)->SetInvincibleFlag(true);
				sprintf(str, "Invincible Human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//�E�Q
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

	//������j��
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

	//������V�K�z�u
	if( GetCommandNum("newobj", &id) == true ){
		if( (0 <= id)&&(id < TOTAL_PARAMETERINFO_SMALLOBJECT) ){
			int dataid = ObjMgr.AddSmallObjectIndex(camera_x + cos(camera_rx)*20.0f, camera_y, camera_z + sin(camera_rx)*20.0f, camera_rx*-1, id, true);
			if( dataid != -1 ){
				sprintf(str, "Add SmallObject[%d].", dataid);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//�v���C���[�����AI��
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

	//AI���퓬��������
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

	//AI���x��������
	if( strcmp(NewCommand, "caution") == 0 ){
		for(int i=0; i<MAX_HUMAN; i++){
			HumanAI[i].SetCautionMode();
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Set cautious AI.");
	}

	//AI�̏������~
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
	//�~�b�V�����B��
	if( strcmp(NewCommand, "comp") == 0 ){
		end_framecnt += 1;
		MainGameInfo.missioncomplete = true;
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Mission complete.");
	}

	//�~�b�V�������s
	if( strcmp(NewCommand, "fail") == 0 ){
		end_framecnt += 1;
		MainGameInfo.missioncomplete = false;
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Mission fail.");
	}
	*/

	//�C�x���g�|�C���g�̏������~
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

	//�o�[�W�������擾
	if( strcmp(NewCommand, "ver") == 0 ){
		sprintf(str, "%s   Version:%s", GAMENAME, GAMEVERSION);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), " (Graphics:"GRAPHICS_CORE"   Sound:"SOUND_CORE")");
	}

	//�X�N���[���V���b�g���B�e
	//�@���R���\�[����ʂ��폜���邽�߁A�B�e��1�t���[���x�点��B
	if( ScreenShot == true ){
		char fname[256];

		//�t�@�C����������
		GetTimeName(fname);
		strcat(fname, ".bmp");

		//�B�e�E�ۑ�
		if( d3dg->SaveScreenShot(fname) == true ){
			sprintf(str, "Saved Screenshot  (File:%s)", fname);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
		else{
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Error] Save failed.");
		}
	}
	if( strcmp(NewCommand, "ss") == 0 ){
		ScreenShot = true;
	}
	else{
		ScreenShot = false;
	}

	//�R���\�[�����N���A
	if( strcmp(NewCommand, "clear") == 0 ){
		for(int i=0; i<MAX_CONSOLELINES; i++){
			InfoConsoleData[i].color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
			InfoConsoleData[i].textdata[0] = NULL;
		}
	}
}

//! @brief �f�o�b�N�p�R���\�[���̕`�揈��
void maingame::RenderConsole()
{
	//���n
	d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, (MAX_CONSOLELINES+1)*18 + 5 + 5, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.75f));

	//�\�����̕���
	for(int i=0; i<MAX_CONSOLELINES; i++){
		if( InfoConsoleData[i].textdata[0] != NULL ){
			d3dg->Draw2DTextureFontText(5+1, i*18+5+1, InfoConsoleData[i].textdata, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 14, 17);
			d3dg->Draw2DTextureFontText(5, i*18+5, InfoConsoleData[i].textdata, InfoConsoleData[i].color, 14, 17);
		}
	}

	//���͒��̕���
	int cnt;
	for(cnt=0; cnt<MAX_CONSOLELINES; cnt++){
		if( InfoConsoleData[cnt].textdata[0] == NULL ){ break; }
	}
	d3dg->Draw2DTextureFontText(5+1, cnt*18+5+1, InputConsoleData->textdata, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 14, 17);
	d3dg->Draw2DTextureFontText(5, cnt*18+5, InputConsoleData->textdata, InputConsoleData->color, 14, 17);
}

#endif

void maingame::Destroy()
{
	//�u���b�N�f�[�^���
	d3dg->CleanupMapdata();

	//�I�u�W�F�N�g�}�l�[�W���[���
	ObjMgr.Cleanup();

	//�w�i����
	Resource.CleanupSkyModelTexture();

#ifdef ENABLE_DEBUGCONSOLE
	//�R���\�[���p�f�[�^�����
	if( InfoConsoleData != NULL ){ delete [] InfoConsoleData; }
	if( InputConsoleData != NULL ){ delete InputConsoleData; }
#endif

	GameState->NextState();
}

//! @brief �R���X�g���N�^
result::result()
{}

//! @brief �f�B�X�g���N�^
result::~result()
{}

//! @brief ���U���g��ʂ�2D�`�敔��
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

	//�����F�w�i�摜�̕`��́A�����I�ɍs����B

	//�Œ蕶���`��
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 50*3, 40, "RESULT", d3dg->GetColorCode(1.0f,0.0f,1.0f,effectA), 50, 42);

	//�~�b�V���������擾���`��
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, mname, NULL, NULL);
	}
	else{
		strcpy(mname, MIFdata.GetMissionFullname());
	}
	d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - strlen(mname)*18/2, 100, mname, d3dg->GetColorCode(0.5f,0.5f,1.0f,1.0f), 18, 25);

	//�~�b�V�����N���A�[�̗L��
	if( GameInfoData.missioncomplete == true ){
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 216, 150, "mission successful", d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f), 24, 32);
	}
	else{
		d3dg->Draw2DTextureFontText(SCREEN_WIDTH/2 - 180, 150, "mission failure", d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f), 24, 32);
	}

	//���ʕ\��
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

//! @brief screen�h���N���X�̏������i�N���X�̐ݒ�j
void InitScreen(opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result)
{
	Opening->SetClass(&GameState, &d3dg, &inputCtrl, &GameSound);
	MainMenu->SetClass(&GameState, &d3dg, &inputCtrl, &GameSound);
	Briefing->SetClass(&GameState, &d3dg, &inputCtrl);
	MainGame->SetClass(&GameState, &d3dg, &inputCtrl, &GameSound);
	Result->SetClass(&GameState, &d3dg, &inputCtrl);
}

//! @brief screen�h���N���X�̎��s
void ProcessScreen(WindowControl *WindowCtrl, opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result, unsigned int framecnt)
{
	int error;

	switch(GameState.GetState()){
		//�I�[�v�j���O������
		case STATE_CREATE_OPENING:
			error = Opening->Create();
			if( error == 1 ){
				WindowCtrl->ErrorInfo("block data open failed");
				WindowCtrl->CloseWindow();
			}
			if( error == 2 ){
				WindowCtrl->ErrorInfo("point data open failed");
				WindowCtrl->CloseWindow();
			}
			break;

		//�I�[�v�j���O���s
		case STATE_NOW_OPENING:
			Opening->Input();
			Opening->Process();
			Opening->Sound();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( Opening->RenderMain() == true ){
					ResetGame(WindowCtrl);
				}
			}
			break;

		//�I�[�v�j���O�p��
		case STATE_DESTROY_OPENING:
			Opening->Destroy();
			break;

		//���j���[������
		case STATE_CREATE_MENU:
			error = MainMenu->Create();
			if( error == 1 ){
				WindowCtrl->ErrorInfo("block data open failed");
				WindowCtrl->CloseWindow();
			}
			if( error == 2 ){
				WindowCtrl->ErrorInfo("point data open failed");
				WindowCtrl->CloseWindow();
			}
			break;

		//���j���[���s
		case STATE_NOW_MENU:
			MainMenu->Input();
			MainMenu->Process();
			MainMenu->Sound();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( MainMenu->RenderMain() == true ){
					ResetGame(WindowCtrl);
				}
			}
			break;

		//���j���[�p��
		case STATE_DESTROY_MENU:
			MainMenu->Destroy();
			break;

		//�u���[�t�B���O������
		case STATE_CREATE_BRIEFING:
			error = Briefing->Create();
			if( error == 1 ){
				WindowCtrl->ErrorInfo("briefing data open failed");
				WindowCtrl->CloseWindow();
			}
			break;

		//�u���[�t�B���O���s
		case STATE_NOW_BRIEFING:
			Briefing->Input();
			Briefing->Process();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( Briefing->RenderMain() == true ){
					ResetGame(WindowCtrl);
				}
			}
			break;

		//�u���[�t�B���O�p��
		case STATE_DESTROY_BRIEFING:
			Briefing->Destroy();
			break;

		//���C���Q�[��������
		case STATE_CREATE_MAINGAME:
			error = MainGame->Create();
			if( error == 1 ){
				WindowCtrl->ErrorInfo("block data open failed");
				WindowCtrl->CloseWindow();
			}
			if( error == 2 ){
				WindowCtrl->ErrorInfo("point data open failed");
				WindowCtrl->CloseWindow();
			}
			break;

		//���C���Q�[�����s
		case STATE_NOW_MAINGAME:
			MainGame->Input();
			MainGame->Process();
			MainGame->Sound();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( MainGame->RenderMain() == true ){
					ResetGame(WindowCtrl);
				}
			}
			break;

		//���C���Q�[���p��
		case STATE_DESTROY_MAINGAME:
			MainGame->Destroy();
			break;

		//���U���g������
		case STATE_CREATE_RESULT:
			error = Result->Create();
			break;

		//���U���g���s
		case STATE_NOW_RESULT:
			Result->Input();
			Result->Process();
			if( (GameConfig.GetFrameskipFlag() == false)||(framecnt%2 == 0) ){
				if( Result->RenderMain() == true ){
					ResetGame(WindowCtrl);
				}
			}
			break;

		//���U���g�p��
		case STATE_DESTROY_RESULT:
			Result->Destroy();
			break;

		//�Q�[���I��
		case STATE_EXIT:
			WindowCtrl->CloseWindow();
			break;

		default:
			break;
	}
}