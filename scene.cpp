//! @file scene.cpp
//! @brief 各画面を管理するクラスの定義

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

#include "scene.h"

//! コンストラクタ
scene::scene()
{
	d3dg = NULL;
	inputCtrl = NULL;
	framecnt = 0;
}

//! ディストラクタ
scene::~scene()
{}

//! クラスを設定
void scene::SetClass(StateMachine *in_GameState, D3DGraphics *in_d3dg, InputControl *in_inputCtrl)
{
	GameState = in_GameState;
	d3dg = in_d3dg;
	inputCtrl = in_inputCtrl;
}

//! シーン作成
int scene::Create()
{
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	GameState->NextState();
	return 0;
}

//! シーン入力処理
void scene::Input()
{
	inputCtrl->GetInputState(false);

	if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){
		GameState->PushBackSpaceKey();
	}

	if( inputCtrl->CheckMouseButtonUpL() ){
		GameState->PushMouseButton();
	}
}

//! シーン計算処理
void scene::Process()
{
	framecnt += 1;
}

//! シーン描画処理
void scene::RenderMain()
{
	if( d3dg->StartRender() ){ return; }

	d3dg->Draw2DBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH, D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DTextureFontText(10, 10, "hello world !", D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f), 20, 32);

	d3dg->ScreenBrightness(SCREEN_WIDTH, SCREEN_HEIGTH, GameConfig.GetBrightness());

	d3dg->EndRender();
}

//! シーン解放処理
void scene::Destroy()
{
	GameState->NextState();
}

//! コンストラクタ
D2Dscene::D2Dscene()
{}

//! ディストラクタ
D2Dscene::~D2Dscene()
{}

//! 2Dシーン初期化
int D2Dscene::Create()
{
	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);
	GameState->NextState();
	if( gametitle == -1 ){ return 1; }
	return 0;
}

//! 2Dシーン描画処理（2D）
void D2Dscene::Render2D()
{
	d3dg->Draw2DTextureFontText(10, 10, "hello world !", D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f), 20, 32);
}

//! 2Dシーン描画処理（メイン）
void D2Dscene::RenderMain()
{
	if( d3dg->StartRender() ){ return; }

	d3dg->Draw2DTexture(0, 0, gametitle, SCREEN_WIDTH, SCREEN_HEIGTH, 0.4f);
	d3dg->Draw2DBox(11, 11, SCREEN_WIDTH - 10, SCREEN_HEIGTH - 10, D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,0.75f));

	Render2D();

	d3dg->ScreenBrightness(SCREEN_WIDTH, SCREEN_HEIGTH, GameConfig.GetBrightness());

	d3dg->EndRender();
}

//! 2Dシーン解放
void D2Dscene::Destroy()
{
	d3dg->CleanupTexture(gametitle);
	GameState->NextState();
}


//! コンストラクタ
D3Dscene::D3Dscene()
{}

//! ディストラクタ
D3Dscene::~D3Dscene()
{}

//! 3Dシーンクラスを設定
void D3Dscene::SetClass(StateMachine *in_GameState, D3DGraphics *in_d3dg, InputControl *in_inputCtrl, SoundManager *in_GameSound)
{
	GameState = in_GameState;
	d3dg = in_d3dg;
	inputCtrl = in_inputCtrl;
	GameSound = in_GameSound;
}

//! 3Dシーン主計算処理
void D3Dscene::Process()
{
	GameSound->PlayWorldSound(camera_x, camera_y, camera_z, camera_rx);
}

//! 3Dシーンサウンド再生処理
void D3Dscene::Sound()
{
	//サウンドを再生
	GameSound->PlayWorldSound(camera_x, camera_y, camera_z, camera_rx);
}

//! 3Dシーン描画処理（3D）
void D3Dscene::Render3D()
{}

//! 3Dシーン描画処理（2D）
void D3Dscene::Render2D()
{}

//! 3Dシーン描画処理（メイン）
void D3Dscene::RenderMain()
{
	if( d3dg->StartRender() ){ return; }

	Render3D();
	Render2D();

	d3dg->ScreenBrightness(SCREEN_WIDTH, SCREEN_HEIGTH, GameConfig.GetBrightness());

	d3dg->EndRender();
}