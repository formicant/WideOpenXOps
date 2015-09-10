//! @file d3dgraphics-directx.cpp
//! @brief D3DGraphicsクラスの定義（DirectX版）

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

#include "d3dgraphics.h"

#ifndef GRAPHICS_OPENGL

//! @brief コンストラクタ
D3DGraphics::D3DGraphics()
{
	pD3D = NULL;
	pd3dDevice = NULL;
	aspect = 1.0f;
	fullscreenflag = false;
	for(int i=0; i<MAX_MODEL; i++){
		pmesh[i] = NULL;
	}
	for(int i=0; i<MAX_TEXTURE; i++){
		ptextures[i] = NULL;
	}

	blockdata = NULL;
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		mapTextureID[i] = -1;
	}

	StartRenderFlag = false;

	//ptextsprite = NULL;
	pxmsfont = NULL;
	TextureFont = -1;
}

//! @brief ディストラクタ
D3DGraphics::~D3DGraphics()
{
	//リソース解放
	CleanupD3Dresource();

	if( pd3dDevice != NULL ) pd3dDevice->Release();
	if( pD3D != NULL ) pD3D->Release();

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "グラフィック", "DirectX");
#endif
}

//! @brief 初期化@n
//! （DirectX 9）
//! @param WindowCtrl WindowControlクラスのポインタ
//! @param TextureFontFilename 使用するテクスチャフォントのファイル名
//! @param fullscreen false：ウィンドウ表示　true：フルスクリーン用表示
//! @return 成功：0　失敗：1
int D3DGraphics::InitD3D(WindowControl *WindowCtrl, char *TextureFontFilename, bool fullscreen)
{
#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_INIT, "グラフィック", "DirectX");
#endif

	D3DPRESENT_PARAMETERS d3dpp;
	RECT rec;

	GetClientRect(WindowCtrl->GethWnd(), &rec);

	fullscreenflag = fullscreen;

	//D3D9の作成
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( pD3D == NULL ){
		return 1;
	}

	//D3Dデバイスの作成
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	if( fullscreenflag == false ){
		d3dpp.Windowed = TRUE;
		d3dpp.BackBufferWidth = rec.right;
		d3dpp.BackBufferHeight = rec.bottom;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.FullScreen_RefreshRateInHz = 0;
	}
	else{
		D3DDISPLAYMODE dispmode;
		pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispmode);

		d3dpp.Windowed = FALSE;
		d3dpp.BackBufferWidth = rec.right;
		d3dpp.BackBufferHeight = rec.bottom;
		d3dpp.BackBufferFormat = dispmode.Format;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.FullScreen_RefreshRateInHz = dispmode.RefreshRate;
	}

	if( FAILED( pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, WindowCtrl->GethWnd(), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice) ) ){
		if( FAILED( pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, WindowCtrl->GethWnd(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice) ) ){
			return 1;
		}
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//テクスチャフォント用画像のファイル名を設定
	strcpy(TextureFontFname, TextureFontFilename);

	//描画関係の詳細な設定
	if( InitSubset() != 0){
		return 1;
	}
	

	//アスペクト比を設定
	aspect = (float)rec.right / (float)rec.bottom;

	//マウスカーソルを消す
	//ShowCursor(FALSE);


	float aspecth, prx, pry, r;
	aspecth = (float)SCREEN_WIDTH/SCREEN_HEIGHT;

	//HUD_myweapon [奥行き, 縦, 横]

	//HUD_A　現在持っている武器を表示する座標
	prx = DegreeToRadian(-39) * aspecth /2;
	pry = DegreeToRadian(-55) /2;
	r = 7.5f;
	HUD_myweapon_x[0] = cos(pry)*r;
	HUD_myweapon_y[0] = sin(pry)*r;
	HUD_myweapon_z[0] = sin(prx)*r;

	//HUD_A　予備の武器を表示する座標
	prx = DegreeToRadian(-52) * aspecth /2;
	pry = DegreeToRadian(-60) /2;
	r = 16.0f;
	HUD_myweapon_x[1] = cos(pry)*r;
	HUD_myweapon_y[1] = sin(pry)*r;
	HUD_myweapon_z[1] = sin(prx)*r;

	return 0;
}

//! @brief リセット@n
//! （ウィンドウ最小化からの復帰　など）
//! @param WindowCtrl WindowControlクラスのポインタ
//! @return 成功：0　待ち：1　失敗：2
int D3DGraphics::ResetD3D(WindowControl *WindowCtrl)
{
	//フォーカスを失っているなら待たせる
	if( pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICELOST ){
		return 1;
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_INIT, "グラフィック", "DirectX（リセット）");
#endif

	//リソース解放
	CleanupD3Dresource();

	D3DPRESENT_PARAMETERS d3dpp;
	RECT rec;

	GetClientRect(WindowCtrl->GethWnd(), &rec);

	//D3Dデバイスの作成
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	if( fullscreenflag == false ){
		d3dpp.Windowed = TRUE;
		d3dpp.BackBufferWidth = rec.right;
		d3dpp.BackBufferHeight = rec.bottom;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.FullScreen_RefreshRateInHz = 0;
	}
	else{
		D3DDISPLAYMODE dispmode;
		pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispmode);

		d3dpp.Windowed = FALSE;
		d3dpp.BackBufferWidth = rec.right;
		d3dpp.BackBufferHeight = rec.bottom;
		d3dpp.BackBufferFormat = dispmode.Format;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.FullScreen_RefreshRateInHz = dispmode.RefreshRate;
	}

	if( FAILED( pd3dDevice->Reset(&d3dpp) ) ){
		return 2;
	}

	//描画関係の詳細な設定
	if( InitSubset() != 0){
		return 2;
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief 描画関係の細部設定
//! @attention 初期化時に1度だけ実行してください。
int D3DGraphics::InitSubset()
{
	//ライト
	//pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(0,255,255,255) );
	pd3dDevice->LightEnable(0, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//フォグ
	float fog_st = 100;
	float fog_end = 800;
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_RGBA(0, 0, 0, 0));
	pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	pd3dDevice->SetRenderState(D3DRS_FOGSTART,*(DWORD*)(&fog_st));
	pd3dDevice->SetRenderState(D3DRS_FOGEND,  *(DWORD*)(&fog_end));

	// テクスチャフィルタを使う
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//ミップマップの詳細レベル (LOD) バイアスを指定する。
	float LODBias = -0.2f;
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&LODBias)) );

	//アルファ・ブレンディングを行う
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//透過処理を行う
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//アルファテストに対応しているかチェック
	D3DCAPS9 Caps;
	pd3dDevice->GetDeviceCaps(&Caps);
	if( Caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL ){
		//アルファテスト設定
		//　完全に透明なピクセルは描画しない
		pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
		pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}

	//深度バッファ比較関数
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	//ポリゴンの裏・表
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


	//テキストスプライト初期化
	if( FAILED( D3DXCreateSprite( pd3dDevice, &ptextsprite ) ) ){
		return 1;
	}
	//フォント名：ＭＳ ゴシック　サイズ：18
	HRESULT hr = D3DXCreateFont( pd3dDevice, -18, 0, FW_NORMAL, 1, FALSE, SHIFTJIS_CHARSET , OUT_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "ＭＳ ゴシック", &pxmsfont);
	if( FAILED(hr) ) return 1;

	//テクスチャフォント用画像を取得
	TextureFont = LoadTexture(TextureFontFname, true, false);
	return 0;
}

//! @brief デバイスのリソースを解放
void D3DGraphics::CleanupD3Dresource()
{
	if( TextureFont != -1 ){ CleanupTexture(TextureFont); }
	if( pxmsfont != NULL ){
		pxmsfont->Release();
		pxmsfont = NULL;
	}
	if( ptextsprite != NULL ){ ptextsprite->Release(); }

	CleanupMapdata();

	for(int i=0; i<MAX_MODEL; i++){
		CleanupModel(i);
	}
	for(int i=0; i<MAX_TEXTURE; i++){
		CleanupTexture(i);
	}
}

//! @brief モデルファイルを読み込む（.x）
//! @param filename ファイル名
//! @return 成功：モデル認識番号（0以上）　失敗：-1
int D3DGraphics::LoadModel(char* filename)
{
#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "モデル", filename);
#endif

	int id = -1;

	//空いている要素を探す
	for(int i=0; i<MAX_MODEL; i++){
		if( pmesh[i] == NULL ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	LPD3DXBUFFER pD3DXMtrlBuffer;

#ifdef PATH_DELIMITER_SLASH
	//パス区切り文字を変換
	filename = ChangePathDelimiter(filename);
#endif

	//.xファイルをバッファーに読み込む
	if( FAILED( D3DXLoadMeshFromX( filename, D3DXMESH_SYSTEMMEM, pd3dDevice, NULL, 
				&pD3DXMtrlBuffer, NULL, &nummaterials[id], &pmesh[id] ) ) ) {
		return -1;
	}

	//マテリアル情報を取得
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	int num = nummaterials[id];
	pmaterials[id] = new D3DMATERIAL9[num];
	if( pmaterials[id]  == NULL ) return -1;

	//構造体に代入
	for( int i=0; i<num; i=i+1 ){
		pmaterials[id][i] = d3dxMaterials[i].MatD3D;
		pmaterials[id][i].Ambient = pmaterials[id][i].Diffuse;
	}

	//バッファを開放
	pD3DXMtrlBuffer->Release();

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif
	return id;
}

//! @brief モデルファイルの中間データを作成（モーフィング）
//! @param idA モデルAの認識番号
//! @param idB モデルBの認識番号
//! @return 成功：新しいモデル認識番号（0以上）　失敗：-1
//! @attention モデルAとモデルBは、頂点数・ポリゴン数・インデックスが同じである必要があります。
//! @attention それぞれのモデルデータが正しくないか 頂点数が異なる場合、実行に失敗します。
int D3DGraphics::MorphingModel(int idA, int idB)
{
#ifdef ENABLE_DEBUGLOG
	char str[128];
	sprintf(str, "中間データ作成　　ID：%d and %d", idA, idB);

	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "モデル", str);
#endif

	//データが正しいか調べる
	if( (idA < 0)||((MAX_MODEL -1) < idA) ){ return -1; }
	if( pmesh[idA] == NULL ){ return -1; }
	if( (idB < 0)||((MAX_MODEL -1) < idB) ){ return -1; }
	if( pmesh[idB] == NULL ){ return -1; }

	int idN = -1;
	int numvA, numvB;
	LPDIRECT3DVERTEXBUFFER9 pvbA, pvbB, pvbN;
	D3DXVECTOR3 *pVerticesA, *pVerticesB, *pVerticesN;
	int FVFsize;

	//空いている要素を探す
	for(int i=0; i<MAX_MODEL; i++){
		if( pmesh[i] == NULL ){
			idN = i;
			break;
		}
	}
	if( idN == -1 ){ return -1; }

	//頂点数を取得
	numvA = pmesh[idA]->GetNumVertices();
	numvB = pmesh[idB]->GetNumVertices();

	//頂点数が同じかどうか調べる
	if( numvA != numvB ){ return -1; }

	//頂点データをコピー（実質的に領域確保用のダミー）
	if( pmesh[idA]->CloneMeshFVF(pmesh[idA]->GetOptions(), pmesh[idA]->GetFVF(), pd3dDevice, &pmesh[idN]) != D3D_OK ){
		return -1;
	}

	//マテリアル情報をコピー
	int num = nummaterials[idA];
	nummaterials[idN] = nummaterials[idA];
	pmaterials[idN] = new D3DMATERIAL9[num];
	if( pmaterials[idN]  == NULL ) return -1;
	for( int i=0; i<num; i=i+1 ){
		pmaterials[idN][i] = pmaterials[idA][i];
	}

	//バッファーを取得
	pmesh[idA]->GetVertexBuffer(&pvbA);
	pmesh[idB]->GetVertexBuffer(&pvbB);
	pmesh[idN]->GetVertexBuffer(&pvbN);

	//1頂点当たりのバイト数取得
	FVFsize = D3DXGetFVFVertexSize(pmesh[idN]->GetFVF());

	//各頂点を読み出し計算
	for(int i=0; i<numvA; i++){
		pvbA->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesA, D3DLOCK_READONLY);
		pvbB->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesB, D3DLOCK_READONLY);
		pvbN->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesN, 0);

		//平均化
		pVerticesN->x = (pVerticesA->x + pVerticesB->x)/2;
		pVerticesN->y = (pVerticesA->y + pVerticesB->y)/2;
		pVerticesN->z = (pVerticesA->z + pVerticesB->z)/2;

		pvbA->Unlock();
		pvbB->Unlock();
		pvbN->Unlock();
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", idN);
#endif
	return idN;
}

//! @brief モデルファイルを解放
//! @param id モデル認識番号
void D3DGraphics::CleanupModel(int id)
{
	if( (id < 0)||((MAX_MODEL -1) < id) ){ return; }
	if( pmesh[id] != NULL ){
		delete [] pmaterials[id];

		pmesh[id]->Release();
		pmesh[id] = NULL;

#ifdef ENABLE_DEBUGLOG
		//ログに出力
		OutputLog.WriteLog(LOG_CLEANUP, "モデル", id);
#endif
	}
}

//! @brief テクスチャを読み込む
//! @param filename ファイル名
//! @param texturefont テクスチャフォントフラグ
//! @param BlackTransparent 黒を透過する
//! @return 成功：テクスチャ認識番号（0以上）　失敗：-1
int D3DGraphics::LoadTexture(char* filename, bool texturefont, bool BlackTransparent)
{
	int id = -1;
	D3DXIMAGE_INFO info;
	int MipLevels;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "テクスチャ", filename);
#endif

	//空いている認識番号を探す
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i] == NULL ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

#ifdef PATH_DELIMITER_SLASH
	//パス区切り文字を変換
	filename = ChangePathDelimiter(filename);
#endif

	//ファイル情報を取得
	if( D3DXGetImageInfoFromFile(filename, &info) != D3D_OK ){ return -1; }

	//ミップマップレベルを設定
	if( texturefont == true ){
		MipLevels = 1;
	}
	else{
		MipLevels = 4;//D3DX_DEFAULT;
	}

	//テクスチャを読み込む
	if( BlackTransparent == false ){
		if( FAILED( D3DXCreateTextureFromFileEx(pd3dDevice, filename, info.Width, info.Height, MipLevels, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0x00000000, NULL, NULL, &ptextures[id]) ) ) {
			return -1;
		}
	}
	else{
		if( FAILED( D3DXCreateTextureFromFileEx(pd3dDevice, filename, info.Width, info.Height, MipLevels, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_ARGB(255, 0, 0, 0), NULL, NULL, &ptextures[id]) ) ) {
			return -1;
		}
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif
	return id;
}

//! @brief テクスチャのサイズを取得
//! @param id テクスチャ認識番号
//! @param width 幅を受け取るポインタ
//! @param height 高さを受け取るポインタ
//! @return 成功：0　失敗：1
//! @attention サーフェイスのサイズを取得します。GPUにロードされたサイズであり、テクスチャ（現物）と異なる場合があります。
int D3DGraphics::GetTextureSize(int id, int *width, int *height)
{
	//無効な認識番号が指定されていたら、処理せず返す。
	if( id == -1 ){ return 1; }
	if( ptextures[id] == NULL ){ return 1; }

	IDirect3DSurface9 *surface;
	D3DSURFACE_DESC desc;

	//サーフェイスを取得
	ptextures[id]->GetSurfaceLevel(0, &surface);

	//幅と高さを取得
	surface->GetDesc(&desc);
	*width = desc.Width;
	*height = desc.Height;

	//サーフェイスを開放
	surface->Release();

	return 0;
}

//! @brief テクスチャを解放
//! @param id テクスチャ認識番号
void D3DGraphics::CleanupTexture(int id)
{
	if( (id < 0)||((MAX_TEXTURE -1) < id) ){ return; }
	if( ptextures[id] != NULL ){
		ptextures[id]->Release();
		ptextures[id] = NULL;

#ifdef ENABLE_DEBUGLOG
		//ログに出力
		OutputLog.WriteLog(LOG_CLEANUP, "テクスチャ", id);
#endif
	}
}

//! @brief 全ての描画処理を開始
//! @return 成功：0　失敗：1
//! @attention 描画処理の最初に呼び出す必要があります。
int D3DGraphics::StartRender()
{
	if( StartRenderFlag == true ){ return 1; }

	//領域を初期化
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

	if( SUCCEEDED( pd3dDevice->BeginScene() ) ){
		//Zバッファ初期化
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		//座標ゼロ地点にワールド変換行列
		ResetWorldTransform();

		//描画中のフラグを立てる
		StartRenderFlag = true;
		return 0;
	}

	return 1;
}

//! @brief 全ての描画処理を終了
//! @return 成功：false　失敗：true
//! @attention 描画処理の最後に呼び出す必要があります。
bool D3DGraphics::EndRender()
{
	//描画中なら終了
	if( StartRenderFlag == true ){
		pd3dDevice->EndScene();
	}

	HRESULT hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);

	//フラグを false に
	StartRenderFlag = false;

	if( hr == D3DERR_DEVICELOST ){
		return true;
	}
	return false;
}

//! @brief Zバッファをリセット
void D3DGraphics::ResetZbuffer()
{
	//Zバッファを一度無効にし、初期化後、再度有効に
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

//! @brief ワールド空間を原点（0,0,0）に戻す　など
void D3DGraphics::ResetWorldTransform()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

//! @brief ワールド空間の座標・角度・拡大率を設定
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param rx 横軸角度
//! @param ry 縦軸角度
//! @param size 拡大率
void D3DGraphics::SetWorldTransform(float x, float y, float z, float rx, float ry, float size)
{
	SetWorldTransform(x, y, z, rx, ry, 0.0f, size);
}

//! @brief ワールド空間の座標・角度・拡大率を設定
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param rx 横軸角度
//! @param ry1 縦軸角度
//! @param ry2 縦軸角度
//! @param size 拡大率
void D3DGraphics::SetWorldTransform(float x, float y, float z, float rx, float ry1, float ry2, float size)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//行列を作成
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationX(&matWorld3, ry1);
	D3DXMatrixRotationZ(&matWorld4, ry2);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//計算
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//適用
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//! @brief ワールド空間の座標・角度・拡大率を設定（エフェクト用）
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param rx 横軸角度
//! @param ry 縦軸角度
//! @param rt 回転角度
//! @param size 拡大率
void D3DGraphics::SetWorldTransformEffect(float x, float y, float z, float rx, float ry, float rt, float size)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//行列を作成
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationZ(&matWorld3, ry);
	D3DXMatrixRotationX(&matWorld4, rt);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//計算
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//適用
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//! @brief ワールド空間を人が武器を持つ場所に設定
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param mx 手元を原点にした モデルのX座標
//! @param my 手元を原点にした モデルのY座標
//! @param mz 手元を原点にした モデルのZ座標
//! @param rx 横軸角度
//! @param ry 縦軸角度
//! @param size 拡大率
void D3DGraphics::SetWorldTransformHumanWeapon(float x, float y, float z, float mx, float my, float mz, float rx, float ry, float size)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//行列を作成
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationX(&matWorld3, ry);
	D3DXMatrixTranslation(&matWorld4, mx, my, mz);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//計算
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//適用
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//! @brief ワールド空間を所持している武器を表示する場所に設定
//! @param rotation 武器を回転させる
//! @param camera_x カメラのX座標
//! @param camera_y カメラのY座標
//! @param camera_z カメラのZ座標
//! @param camera_rx カメラの横軸角度
//! @param camera_ry カメラの縦軸角度
//! @param rx 武器のの縦軸角度
//! @param size 表示サイズ
//! @note rotation・・　true：現在持っている武器です。　false：予備の武器です。（rx は無視されます）
//! @todo 位置やサイズの微調整
void D3DGraphics::SetWorldTransformPlayerWeapon(bool rotation, float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float rx, float size)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5, matWorld6;

	size = size * 0.3f;

	//行列を作成
	D3DXMatrixTranslation(&matWorld1, camera_x, camera_y, camera_z);
	D3DXMatrixRotationY(&matWorld2, camera_rx *-1);
	D3DXMatrixRotationZ(&matWorld3, camera_ry);
	// matWorld4 = [奥行き, 縦, 横]
	if( rotation == true ){
		D3DXMatrixTranslation(&matWorld4, HUD_myweapon_x[0], HUD_myweapon_y[0], HUD_myweapon_z[0]);
		D3DXMatrixRotationY(&matWorld5, rx);
	}
	else{
		D3DXMatrixTranslation(&matWorld4, HUD_myweapon_x[1], HUD_myweapon_y[1], HUD_myweapon_z[1]);
		D3DXMatrixRotationY(&matWorld5, D3DX_PI);
	}
	D3DXMatrixScaling(&matWorld6, size, size, size);

	//計算
	matWorld = matWorld6 * matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//適用
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//! @brief ワールド空間の座標を取得
//! @param *x x軸を受け取るポインタ
//! @param *y y軸を受け取るポインタ
//! @param *z z軸を受け取るポインタ
void D3DGraphics::GetWorldTransformPos(float *x, float *y, float *z)
{
	D3DXMATRIX matWorld;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
	*x = matWorld._41;
	*y = matWorld._42;
	*z = matWorld._43;
}

//! @brief フォグを設定
//! @param skynumber 空の番号
void D3DGraphics::SetFog(int skynumber)
{
	D3DCOLOR skycolor;

	//空の番号により色を決定
	switch(skynumber){
		case 1: skycolor = D3DCOLOR_RGBA(64, 64+16, 64, 0); break;
		case 2: skycolor = D3DCOLOR_RGBA(16, 16, 16, 0); break;
		case 3: skycolor = D3DCOLOR_RGBA(0, 16, 32, 0); break;
		case 4: skycolor = D3DCOLOR_RGBA(32, 16, 16, 0); break;
		case 5: skycolor = D3DCOLOR_RGBA(64, 32, 32, 0); break;
		default: skycolor = D3DCOLOR_RGBA(0, 0, 0, 0); break;
	}

	//フォグを設定
	pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, skycolor);
}

//! @brief カメラ（視点）を設定
//! @param camera_x カメラのX座標
//! @param camera_y カメラのY座標
//! @param camera_z カメラのZ座標
//! @param camera_rx カメラの横軸角度
//! @param camera_ry カメラの縦軸角度
//! @param viewangle 視野角
void D3DGraphics::SetCamera(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float viewangle)
{
	float vUpVecF;
	D3DXMATRIX matWorld;
	D3DXMATRIXA16 matView;

	//camera_ryを -PI～PI の間に正規化
	for(; camera_ry>D3DX_PI; camera_ry -= D3DX_PI*2){}
	for(; camera_ry<D3DX_PI*-1; camera_ry += D3DX_PI*2){}

	//カメラの向きを決定
	if( fabs(camera_ry) < D3DX_PI/2 ){
		vUpVecF = 1.0f;
	}
	else{
		vUpVecF = -1.0f;
	}

	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	//カメラ座標
	D3DXVECTOR3 vEyePt( camera_x, camera_y, camera_z );
	D3DXVECTOR3 vLookatPt( cos(camera_rx)*cos(camera_ry) + camera_x, sin(camera_ry) + camera_y, sin(camera_rx)*cos(camera_ry) + camera_z );
	D3DXVECTOR3 vUpVec( 0.0f, vUpVecF, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//カメラ設定（射影変換行列）viewangle
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, viewangle, aspect, CLIPPINGPLANE_NEAR, CLIPPINGPLANE_FAR);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

//! @brief マップデータを取り込む
//! @param in_blockdata ブロックデータ
//! @param directory ブロックデータが存在するディレクトリ
void D3DGraphics::LoadMapdata(BlockDataInterface* in_blockdata, char *directory)
{
	//ブロックデータが指定されていなければ、処理しない。
	if( in_blockdata == NULL ){ return; }

	char fname[MAX_PATH];
	char fnamefull[MAX_PATH];
	//int bs;
	struct blockdata data;
	int vID[4];
	int uvID[4];

	//クラスを設定
	blockdata = in_blockdata;

	//テクスチャ読み込み
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		//テクスチャ名を取得
		blockdata->GetTexture(fname, i);

		if( strcmp(fname, "") == 0 ){	//指定されていなければ、処理しない
			mapTextureID[i] = -1;
		}
		else{
			//「ディレクトリ＋ファイル名」を生成し、読み込む
			strcpy(fnamefull, directory);
			strcat(fnamefull, fname);
			mapTextureID[i] = LoadTexture(fnamefull, false, false);
		}
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "マップ", "（頂点データ）");
#endif

#ifdef BLOCKDATA_GPUMEMORY
	VERTEXTXTA* pVertices;

	//ブロック数を取得
	bs = blockdata->GetTotaldatas();

	//ブロック数分のバッファーを作成
	pd3dDevice->CreateVertexBuffer(bs*6*4*sizeof(VERTEXTXTA),0,D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,D3DPOOL_DEFAULT,&g_pVB,NULL);

	for(int i=0; i<bs; i++){
		//データを取得
		blockdata->Getdata(&data, i);

		for(int j=0; j<6; j++){
			//面の頂点データの関連付けを取得
			blockdataface(j, &vID[0], &uvID[0]);

			//GPUをロック（1面分）
			g_pVB->Lock((i*6+j)*4*sizeof(VERTEXTXTA), 4*sizeof(VERTEXTXTA), (void**)&pVertices, 0);

			//頂点座標・UV座標・色を設定
			pVertices[0].position = D3DXVECTOR3( data.x[ vID[1] ], data.y[ vID[1] ], data.z[ vID[1] ] );
			pVertices[0].tu       = data.material[j].u[ uvID[1] ];
			pVertices[0].tv       = data.material[j].v[ uvID[1] ];
			pVertices[1].position = D3DXVECTOR3( data.x[ vID[2] ], data.y[ vID[2] ], data.z[ vID[2] ] );
			pVertices[1].tu       = data.material[j].u[ uvID[2] ];
			pVertices[1].tv       = data.material[j].v[ uvID[2] ];
			pVertices[2].position = D3DXVECTOR3( data.x[ vID[0] ], data.y[ vID[0] ], data.z[ vID[0] ] );
			pVertices[2].tu       = data.material[j].u[ uvID[0] ];
			pVertices[2].tv       = data.material[j].v[ uvID[0] ];
			pVertices[3].position = D3DXVECTOR3( data.x[ vID[3] ], data.y[ vID[3] ], data.z[ vID[3] ] );
			pVertices[3].tu       = data.material[j].u[ uvID[3] ];
			pVertices[3].tv       = data.material[j].v[ uvID[3] ];
			for(int k=0; k<4; k++){
				pVertices[k].color = D3DCOLOR_COLORVALUE(data.material[j].shadow, data.material[j].shadow, data.material[j].shadow, 1.0f);
			}

			//GPUのロックを解除
			g_pVB->Unlock();
		}
	}
#else
	//ブロック数を取得
	bs = blockdata->GetTotaldatas();

	for(int i=0; i<bs; i++){
		//データを取得
		blockdata->Getdata(&data, i);

		for(int j=0; j<6; j++){
			//面の頂点データの関連付けを取得
			blockdataface(j, vID, uvID);

			//頂点座標・UV座標・色を設定
			g_pVertices[i][j][0].position = D3DXVECTOR3( data.x[ vID[1] ], data.y[ vID[1] ], data.z[ vID[1] ] );
			g_pVertices[i][j][0].tu       = data.material[j].u[ uvID[1] ];
			g_pVertices[i][j][0].tv       = data.material[j].v[ uvID[1] ];
			g_pVertices[i][j][1].position = D3DXVECTOR3( data.x[ vID[2] ], data.y[ vID[2] ], data.z[ vID[2] ] );
			g_pVertices[i][j][1].tu       = data.material[j].u[ uvID[2] ];
			g_pVertices[i][j][1].tv       = data.material[j].v[ uvID[2] ];
			g_pVertices[i][j][2].position = D3DXVECTOR3( data.x[ vID[0] ], data.y[ vID[0] ], data.z[ vID[0] ] );
			g_pVertices[i][j][2].tu       = data.material[j].u[ uvID[0] ];
			g_pVertices[i][j][2].tv       = data.material[j].v[ uvID[0] ];
			g_pVertices[i][j][3].position = D3DXVECTOR3( data.x[ vID[3] ], data.y[ vID[3] ], data.z[ vID[3] ] );
			g_pVertices[i][j][3].tu       = data.material[j].u[ uvID[3] ];
			g_pVertices[i][j][3].tv       = data.material[j].v[ uvID[3] ];
			for(int k=0; k<4; k++){
				g_pVertices[i][j][k].color = D3DCOLOR_COLORVALUE(data.material[j].shadow, data.material[j].shadow, data.material[j].shadow, 1.0f);
			}
		}
	}
#endif

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
}

//! @brief マップデータを描画
//! @param wireframe ワイヤーフレーム表示
void D3DGraphics::DrawMapdata(bool wireframe)
{
	//ブロックデータが読み込まれていなければ、処理しない。
	if( blockdata == NULL ){ return; }

	struct blockdata data;
	int textureID;

	if( wireframe == true ){
		//ワイヤーフレーム表示
		for(int i=0; i<bs; i++){
			blockdata->Getdata(&data, i);
			Drawline(data.x[0], data.y[0], data.z[0], data.x[1], data.y[1], data.z[1]);
			Drawline(data.x[1], data.y[1], data.z[1], data.x[2], data.y[2], data.z[2]);
			Drawline(data.x[2], data.y[2], data.z[2], data.x[3], data.y[3], data.z[3]);
			Drawline(data.x[3], data.y[3], data.z[3], data.x[0], data.y[0], data.z[0]);
			Drawline(data.x[4], data.y[4], data.z[4], data.x[5], data.y[5], data.z[5]);
			Drawline(data.x[5], data.y[5], data.z[5], data.x[6], data.y[6], data.z[6]);
			Drawline(data.x[6], data.y[6], data.z[6], data.x[7], data.y[7], data.z[7]);
			Drawline(data.x[7], data.y[7], data.z[7], data.x[4], data.y[4], data.z[4]);
			Drawline(data.x[0], data.y[0], data.z[0], data.x[4], data.y[4], data.z[4]);
			Drawline(data.x[1], data.y[1], data.z[1], data.x[5], data.y[5], data.z[5]);
			Drawline(data.x[2], data.y[2], data.z[2], data.x[6], data.y[6], data.z[6]);
			Drawline(data.x[3], data.y[3], data.z[3], data.x[7], data.y[7], data.z[7]);
		}
		return;
	}

	//深度バッファ比較関数を設定
	//pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);


#ifdef BLOCKDATA_GPUMEMORY
	//データ設定
	pd3dDevice->SetStreamSource(0,g_pVB,0,sizeof(VERTEXTXTA));

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		//テクスチャが正常に読み込めていなければ設定
		if( mapTextureID[textureID] == -1 ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pd3dDevice->SetTexture(0, NULL);
		}
		else if( ptextures[ mapTextureID[textureID] ] == NULL ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pd3dDevice->SetTexture(0, NULL);
		}
		else{
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
			pd3dDevice->SetTexture(0, ptextures[mapTextureID[textureID]] );
		}

		for(int i=0; i<bs; i++){
			//データ取得
			blockdata->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//テクスチャ認識番号を取得
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//面を描画
					pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (i*6+j)*4, 2);
				}
			}
		}
	}
#else
	//データを設定
	pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		//テクスチャが正常に読み込めていなければ設定
		if( mapTextureID[textureID] == -1 ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pd3dDevice->SetTexture(0, NULL);
		}
		else if( ptextures[ mapTextureID[textureID] ] == NULL ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pd3dDevice->SetTexture(0, NULL);
		}
		else{
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
			pd3dDevice->SetTexture(0, ptextures[mapTextureID[textureID]] );
		}

		for(int i=0; i<bs; i++){
			//データ取得
			blockdata->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//テクスチャ認識番号を取得
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//面を描画
					pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_pVertices[i][j], sizeof(VERTEXTXTA));
				}
			}
		}
	}
#endif

	//深度バッファ比較関数を元に戻す
	//pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

//! @brief マップテクスチャを取得
//! @param id テクスチャ番号
//! @return テクスチャ認識番号（失敗：-1）
int D3DGraphics::GetMapTextureID(int id)
{
	if( (id < 0)||((TOTAL_BLOCKTEXTURE -1) < id ) ){ return -1; }
	return mapTextureID[id];
}

//! @brief マップデータを解放
void D3DGraphics::CleanupMapdata()
{
	//テクスチャを開放
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		CleanupTexture(mapTextureID[i]);
	}

#ifdef BLOCKDATA_GPUMEMORY
	//頂点データ解放
	if( g_pVB != NULL ){
		g_pVB->Release();
		g_pVB = NULL;
	}
#endif
	bs = 0;

	blockdata = NULL;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "マップ", "（頂点データ）");
#endif
}

//! @brief モデルファイルを描画
//! @param id_model モデル認識番号
//! @param id_texture テクスチャ認識番号
void D3DGraphics::RenderModel(int id_model, int id_texture)
{
	//無効な引数が設定されていれば失敗
	if( id_model == -1 ){ return; }
	//if( id_texture == -1 ){ return; }

	//指定したモデルが初期化されていなければ失敗
	if( pmesh[id_model] == NULL) return;

	//描画
	for(int i=0; i<(signed)nummaterials[id_model]; i=i+1){
		pd3dDevice->SetMaterial( &pmaterials[id_model][i] );
		if( id_texture == -1 ){
			pd3dDevice->SetTexture(0, NULL);
		}
		else if( ptextures[id_texture] == NULL ){
			pd3dDevice->SetTexture(0, NULL);
		}
		else{
			pd3dDevice->SetTexture( 0, ptextures[id_texture] );
		}
		pmesh[id_model]->DrawSubset(i);
	}
}

//! @brief 板を描画
//! @param id_texture テクスチャ認識番号
//! @param alpha 透明度　（0.0～1.0　0.0：完全透明）
void D3DGraphics::RenderBoard(int id_texture, float alpha)
{
	//テクスチャが設定されていなければ、処理しない。
	if( id_texture == -1 ){ return; }

	VERTEXTXTA BoardVertices[4];

	//頂点座標・UV座標・色/透明度を設定
	BoardVertices[0].position = D3DXVECTOR3(0.0f, 0.5f, -0.5f);
	BoardVertices[0].tu       = 1.0f;
	BoardVertices[0].tv       = 0.0f;
	BoardVertices[1].position = D3DXVECTOR3(0.0f, -0.5f, -0.5f);
	BoardVertices[1].tu       = 1.0f;
	BoardVertices[1].tv       = 1.0f;
	BoardVertices[2].position = D3DXVECTOR3(0.0f, 0.5f, 0.5f);
	BoardVertices[2].tu       = 0.0f;
	BoardVertices[2].tv       = 0.0f;
	BoardVertices[3].position = D3DXVECTOR3(0.0f, -0.5f, 0.5f);
	BoardVertices[3].tu       = 0.0f;
	BoardVertices[3].tv       = 1.0f;
	for(int i=0; i<4; i++){
		BoardVertices[i].color = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, alpha);
	}

	//アルファブレンドを設定
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//テクスチャとデータ形式を設定し描画
	pd3dDevice->SetTexture(0, ptextures[id_texture]);
	pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, BoardVertices, sizeof(VERTEXTXTA));

	//アルファブレンドを元に戻す
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

//! @brief 画面の明るさを設定
//! @param Width 幅
//! @param Height 高さ
//! @param Brightness 画面の明るさ　（0 で不変、1 以上で明るさの度合い）
void D3DGraphics::ScreenBrightness(int Width, int Height, int Brightness)
{
	//明るさ不変なら処理しない（軽量化）
	if( Brightness == 0 ){ return; }

	//透明度を設定し、描画
	float alpha = 0.02f * Brightness;
	Draw2DBox(0, 0, Width, Height, D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,alpha));
}

//! @brief 【デバック用】中心線描画
void D3DGraphics::Centerline()
{
	ResetWorldTransform();
	Drawline(100.0f, 0.0f, 0.0f, -100.0f, 0.0f, 0.0f);
	Drawline(0.0f, 100.0f, 0.0f, 0.0f, -100.0f, 0.0f);
	Drawline(0.0f, 0.0f, 100.0f, 0.0f, 0.0f, -100.0f);
}

//! @brief 【デバック用】緑線描画
void D3DGraphics::Drawline(float x1, float y1, float z1, float x2, float y2, float z2)
{
	VERTEXTXTA mv[2];

	mv[0].position = D3DXVECTOR3(x1, y1, z1);
	mv[1].position = D3DXVECTOR3(x2, y2, z2);
	for(int i=0; i<2; i++){
		mv[i].color = 0xFF00FF00;
		mv[i].tu = 0.0f;
		mv[i].tv = 0.0f;
	}

	pd3dDevice->SetTexture(0, NULL);
	pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, mv, sizeof(VERTEXTXTA));
}

//! @brief 2D システムフォントによるテキスト描画を開始
//! @attention DirectXの ID3DXSprite を初期化しています。
void D3DGraphics::Start2DMSFontTextRender()
{
	ptextsprite->Begin(D3DXSPRITE_ALPHABLEND);
}

//! @brief 文字を描画（システムフォント使用）
//! @param x x座標
//! @param y y座標
//! @param str 文字列　（改行コード：可）
//! @param color 色
//! @warning <b>描画は非常に低速です。</b>画面内で何度も呼び出すとパフォーマンスに影響します。
//! @warning 「改行コードを活用し一度に描画する」「日本語が必要ない文字はテクスチャフォントを活用する」などの対応を講じてください。
//! @attention DirectXの ID3DXSprite を使用し、システムフォントで描画しています。
//! @attention フォントの種類やサイズは固定です。　文字を二重に重ねて立体感を出さないと見にくくなります。
void D3DGraphics::Draw2DMSFontText(int x, int y, char *str, int color)
{
	//if( ptextsprite == NULL ){ return; }

	//テキストスプライト初期化
	Start2DMSFontTextRender();

	//基準座標を設定
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	//文字を描画
	RECT rc = {x, y, 0, 0};
	pxmsfont->DrawText(ptextsprite, str, -1, &rc, DT_NOCLIP, color);

	//テキストスプライト解放
	End2DMSFontTextRender();
}

//! @brief 文字を中央揃えで描画（システムフォント使用）
//! @param x x座標
//! @param y y座標
//! @param w 横の大きさ
//! @param h 縦の大きさ
//! @param str 文字列　（改行コード：可）
//! @param color 色
//! @warning <b>描画は非常に低速です。</b>画面内で何度も呼び出すとパフォーマンスに影響します。
//! @warning 「改行コードを活用し一度に描画する」「日本語が必要ない文字はテクスチャフォントを活用する」などの対応を講じてください。
//! @attention DirectXの ID3DXSprite を使用し、システムフォントで描画しています。
//! @attention フォントの種類やサイズは固定です。　文字を二重に重ねて立体感を出さないと見にくくなります。
void D3DGraphics::Draw2DMSFontTextCenter(int x, int y, int w, int h, char *str, int color)
{
	//if( ptextsprite == NULL ){ return; }

	//テキストスプライト初期化
	Start2DMSFontTextRender();

	//基準座標を設定
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	//文字を描画
	RECT rc = {x, y, x+w, y+h};
	pxmsfont->DrawText(ptextsprite, str, -1, &rc, DT_CENTER, color);

	//テキストスプライト解放
	End2DMSFontTextRender();
}

//! @brief 2D システムフォントによるテキスト描画を終了
//! @attention DirectXの ID3DXSprite を解放しています。
void D3DGraphics::End2DMSFontTextRender()
{
	ptextsprite->End();
}

//! @brief 2D描画用設定
void D3DGraphics::Start2DRender()
{
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//深度バッファ比較関数を設定
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
}

//! @brief 文字を描画（テクスチャフォント使用）
//! @param x x座標
//! @param y y座標
//! @param str 文字列　（改行コード：<b>不可</b>）
//! @param color 色
//! @param fontwidth 一文字の幅
//! @param fontheight 一文字の高さ
//! @attention 文字を二重に重ねて立体感を出さないと見にくくなります。
void D3DGraphics::Draw2DTextureFontText(int x, int y, char *str, int color, int fontwidth, int fontheight)
{
	//テクスチャフォントの取得に失敗していれば、処理しない
	if( TextureFont == -1 ){ return; }

	//2D描画用設定を適用
	Start2DRender();

	int w;
	float font_u, font_v;
	float t_u, t_v;
	TLVERTX pBoxVertices[4];

	//1文字のUV座標を計算
	font_u = 1.0f / 16;
	font_v = 1.0f / 16;

	//ワールド座標を原点に戻す
	ResetWorldTransform();

	//テクスチャをフォントテクスチャに設定
	pd3dDevice->SetTexture( 0, ptextures[TextureFont] );

	//データ形式を設定
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	// 与えられた文字数分ループ
	for(int i=0; i<(int)strlen(str); i++){
		//UV座標を計算
		w = str[i];
		if( w < 0 ){ w += 256; }
		t_u = (w % 16) * font_u;
		t_v = (w / 16) * font_v;

		//頂点座標・UV座標・色を設定
		pBoxVertices[0].x = (float)x + i*fontwidth;
		pBoxVertices[0].y = (float)y;
		pBoxVertices[0].tu = t_u;
		pBoxVertices[0].tv = t_v;
		pBoxVertices[1].x = (float)x + fontwidth + i*fontwidth;
		pBoxVertices[1].y = (float)y;
		pBoxVertices[1].tu = t_u + font_u;
		pBoxVertices[1].tv = t_v;
		pBoxVertices[2].x = (float)x + i*fontwidth;
		pBoxVertices[2].y = (float)y + fontheight;
		pBoxVertices[2].tu = t_u;
		pBoxVertices[2].tv = t_v + font_v;
		pBoxVertices[3].x = (float)x + fontwidth + i*fontwidth;
		pBoxVertices[3].y = (float)y + fontheight;
		pBoxVertices[3].tu = t_u + font_u;
		pBoxVertices[3].tv = t_v + font_v;
		for(int j=0; j<4; j++){
			pBoxVertices[j].z = 0.0f;
			pBoxVertices[j].rhw = 1.0f;
			pBoxVertices[j].color = color;
		}

		//描画
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));
	}

	//2D描画用設定を解除
	End2DRender();
}

//! @brief 線を描画
//! @param x1 始点の x座標
//! @param y1 始点の y座標
//! @param x2 終点の x座標
//! @param y2 終点の y座標
//! @param color 色
void D3DGraphics::Draw2DLine(int x1, int y1, int x2, int y2, int color)
{
	TLVERTX pLineVertices[2];

	//2D描画用設定を適用
	Start2DRender();

	//ワールド座標を原点に戻す
	ResetWorldTransform();

	//頂点座標と色などを設定
	pLineVertices[0].x = (float)x1;
	pLineVertices[0].y = (float)y1;
	pLineVertices[1].x = (float)x2;
	pLineVertices[1].y = (float)y2;
	for(int i=0; i<2; i++){
		pLineVertices[i].z = 0.0f;
		pLineVertices[i].rhw = 1.0f;
		pLineVertices[i].color = color;
		pLineVertices[i].tu = 0.0f;
		pLineVertices[i].tv = 0.0f;
	}

	pd3dDevice->SetTexture(0, NULL);

	//データ形式を設定し、描画。
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, pLineVertices, sizeof(TLVERTX));

	//2D描画用設定を解除
	End2DRender();
}

//! @brief 円（16角形）を描画
//! @param x 中心の x座標
//! @param y 中心の y座標
//! @param r 半径
//! @param color 色
void D3DGraphics::Draw2DCycle(int x, int y, int r, int color)
{
	TLVERTX pLineVertices[16+1];

	//2D描画用設定を適用
	Start2DRender();

	//ワールド座標を原点に戻す
	ResetWorldTransform();

	//頂点座標と色などを設定
	for(int i=0; i<16+1; i++){
		pLineVertices[i].x = (float)x + cos(DegreeToRadian((360.0f/16.0f)) * i) * r;
		pLineVertices[i].y = (float)y + sin(DegreeToRadian((360.0f/16.0f)) * i) * r;

		pLineVertices[i].z = 0.0f;
		pLineVertices[i].rhw = 1.0f;
		pLineVertices[i].color = color;
		pLineVertices[i].tu = 0.0f;
		pLineVertices[i].tv = 0.0f;
	}

	pd3dDevice->SetTexture(0, NULL);

	//データ形式を設定し、描画。
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, pLineVertices, sizeof(TLVERTX));

	//2D描画用設定を解除
	End2DRender();
}

//! @brief 四角形を描画
//! @param x1 左上の x座標
//! @param y1 左上の y座標
//! @param x2 右下の x座標
//! @param y2 右下の y座標
//! @param color 色
void D3DGraphics::Draw2DBox(int x1, int y1, int x2, int y2, int color)
{
	TLVERTX pBoxVertices[4];

	//2D描画用設定を適用
	Start2DRender();

	//ワールド座標を原点に戻す
	ResetWorldTransform();

	//頂点座標と色などを設定
	pBoxVertices[0].x = (float)x1;
	pBoxVertices[0].y = (float)y1;
	pBoxVertices[1].x = (float)x2;
	pBoxVertices[1].y = (float)y1;
	pBoxVertices[2].x = (float)x1;
	pBoxVertices[2].y = (float)y2;
	pBoxVertices[3].x = (float)x2;
	pBoxVertices[3].y = (float)y2;
	for(int i=0; i<4; i++){
		pBoxVertices[i].z = 0.0f;
		pBoxVertices[i].rhw = 1.0f;
		pBoxVertices[i].color = color;
		pBoxVertices[i].tu = 0.0f;
		pBoxVertices[i].tv = 0.0f;
	}

	pd3dDevice->SetTexture(0, NULL);

	//データ形式を設定し、描画。
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));

	//2D描画用設定を解除
	End2DRender();
}

//! @brief 画像を描画
//! @param x x座標
//! @param y y座標
//! @param id テクスチャ認識番号
//! @param width 幅
//! @param height 高さ
//! @param alpha 透明度（0.0～1.0）
void D3DGraphics::Draw2DTexture(int x, int y, int id, int width, int height, float alpha)
{
	//無効なテクスチャ番号を指定されていれば処理しない
	if( id == -1 ){ return; }

	TLVERTX pBoxVertices[4];

	//2D描画用設定を適用
	Start2DRender();

	//ワールド座標を原点に戻す
	ResetWorldTransform();

	//頂点座標・UV座標・色を設定
	pBoxVertices[0].x = (float)x;
	pBoxVertices[0].y = (float)y;
	pBoxVertices[0].tu = 0.0f;
	pBoxVertices[0].tv = 0.0f;
	pBoxVertices[1].x = (float)x + width;
	pBoxVertices[1].y = (float)y;
	pBoxVertices[1].tu = 1.0f;
	pBoxVertices[1].tv = 0.0f;
	pBoxVertices[2].x = (float)x;
	pBoxVertices[2].y = (float)y + height;
	pBoxVertices[2].tu = 0.0f;
	pBoxVertices[2].tv = 1.0f;
	pBoxVertices[3].x = (float)x + width;
	pBoxVertices[3].y = (float)y + height;
	pBoxVertices[3].tu = 1.0f;
	pBoxVertices[3].tv = 1.0f;
	for(int i=0; i<4; i++){
		pBoxVertices[i].z = 0.0f;
		pBoxVertices[i].rhw = 1.0f;
		pBoxVertices[i].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,alpha);
	}

	//テクスチャとデータ形式を設定し、描画
	pd3dDevice->SetTexture( 0, ptextures[id] );
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));

	//2D描画用設定を解除
	End2DRender();
}

//! @brief 2D描画用設定を解除
void D3DGraphics::End2DRender()
{
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	//深度バッファ比較関数を元に戻す
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

//! @brief 画面のスクリーンショットを保存
//! @param filename ファイル名
//! @return 成功：true　失敗：false
bool D3DGraphics::SaveScreenShot(char* filename)
{
	LPDIRECT3DSURFACE9 pSurface = NULL;
	HRESULT hr;

	//サーフェースを作成し、画面を取得
	pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);

	//サーフェイスを画像に出力
	hr = D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, pSurface, NULL, NULL);

	//解放
	pSurface->Release();

	if( hr == D3D_OK ){
		return true;
	}
	return false;
}

//! @brief カラーコードを取得
//! @param red 赤（0.0f～1.0f）
//! @param green 緑（0.0f～1.0f）
//! @param blue 青（0.0f～1.0f）
//! @param alpha 透明度（0.0f～1.0f）
//! @return カラーコード
int D3DGraphics::GetColorCode(float red, float green, float blue, float alpha)
{
	return D3DCOLOR_COLORVALUE(red, green, blue, alpha);
}

#endif	//GRAPHICS_OPENGL