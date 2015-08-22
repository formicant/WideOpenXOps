//! @file d3dgraphics-directx.cpp
//! @brief D3DGraphics�N���X�̒�`�iDirectX�Łj

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

#include "d3dgraphics.h"

#ifndef GRAPHICS_OPENGL

//! @brief �R���X�g���N�^
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

//! @brief �f�B�X�g���N�^
D3DGraphics::~D3DGraphics()
{
	//���\�[�X���
	CleanupD3Dresource();

	if( pd3dDevice != NULL ) pd3dDevice->Release();
	if( pD3D != NULL ) pD3D->Release();

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "�O���t�B�b�N", "DirectX");
#endif
}

//! @brief ������@n
//! �iDirectX 9�j
//! @param WindowCtrl WindowControl�N���X�̃|�C���^
//! @param TextureFontFilename �g�p����e�N�X�`���t�H���g�̃t�@�C����
//! @param fullscreen false�F�E�B���h�E�\���@true�F�t���X�N���[���p�\��
//! @return �����F0�@���s�F1
int D3DGraphics::InitD3D(WindowControl *WindowCtrl, char *TextureFontFilename, bool fullscreen)
{
#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_INIT, "�O���t�B�b�N", "DirectX");
#endif

	D3DPRESENT_PARAMETERS d3dpp;
	RECT rec;

	GetClientRect(WindowCtrl->GethWnd(), &rec);

	fullscreenflag = fullscreen;

	//D3D9�̍쐬
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( pD3D == NULL ){
		return 1;
	}

	//D3D�f�o�C�X�̍쐬
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
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//�e�N�X�`���t�H���g�p�摜�̃t�@�C������ݒ�
	strcpy(TextureFontFname, TextureFontFilename);

	//�`��֌W�̏ڍׂȐݒ�
	if( InitSubset() != 0){
		return 1;
	}
	

	//�A�X�y�N�g���ݒ�
	aspect = (float)rec.right / (float)rec.bottom;

	//�}�E�X�J�[�\��������
	//ShowCursor(FALSE);


	float aspecth, prx, pry, r;
	aspecth = (float)SCREEN_WIDTH/SCREEN_HEIGHT;

	//HUD_myweapon [���s��, �c, ��]

	//HUD_A�@���ݎ����Ă��镐���\��������W
	prx = DegreeToRadian(-39) * aspecth /2;
	pry = DegreeToRadian(-55) /2;
	r = 7.5f;
	HUD_myweapon_x[0] = cos(pry)*r;
	HUD_myweapon_y[0] = sin(pry)*r;
	HUD_myweapon_z[0] = sin(prx)*r;

	//HUD_A�@�\���̕����\��������W
	prx = DegreeToRadian(-52) * aspecth /2;
	pry = DegreeToRadian(-60) /2;
	r = 16.0f;
	HUD_myweapon_x[1] = cos(pry)*r;
	HUD_myweapon_y[1] = sin(pry)*r;
	HUD_myweapon_z[1] = sin(prx)*r;

	return 0;
}

//! @brief ���Z�b�g@n
//! �i�E�B���h�E�ŏ�������̕��A�@�Ȃǁj
//! @param WindowCtrl WindowControl�N���X�̃|�C���^
//! @return �����F0�@�҂��F1�@���s�F2
int D3DGraphics::ResetD3D(WindowControl *WindowCtrl)
{
	//�t�H�[�J�X�������Ă���Ȃ�҂�����
	if( pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICELOST ){
		return 1;
	}

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_INIT, "�O���t�B�b�N", "DirectX�i���Z�b�g�j");
#endif

	//���\�[�X���
	CleanupD3Dresource();

	D3DPRESENT_PARAMETERS d3dpp;
	RECT rec;

	GetClientRect(WindowCtrl->GethWnd(), &rec);

	//D3D�f�o�C�X�̍쐬
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

	//�`��֌W�̏ڍׂȐݒ�
	if( InitSubset() != 0){
		return 2;
	}

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief �`��֌W�̍ו��ݒ�
//! @attention ����������1�x�������s���Ă��������B
int D3DGraphics::InitSubset()
{
	//���C�g
	//pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(0,255,255,255) );
	pd3dDevice->LightEnable(0, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//�t�H�O
	float fog_st = 100;
	float fog_end = 800;
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_RGBA(0, 0, 0, 0));
	pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	pd3dDevice->SetRenderState(D3DRS_FOGSTART,*(DWORD*)(&fog_st));
	pd3dDevice->SetRenderState(D3DRS_FOGEND,  *(DWORD*)(&fog_end));

	// �e�N�X�`���t�B���^���g��
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//�~�b�v�}�b�v�̏ڍ׃��x�� (LOD) �o�C�A�X���w�肷��B
	float LODBias = -0.2f;
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&LODBias)) );

	//�A���t�@�E�u�����f�B���O���s��
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//���ߏ������s��
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//�A���t�@�e�X�g�ɑΉ����Ă��邩�`�F�b�N
	D3DCAPS9 Caps;
	pd3dDevice->GetDeviceCaps(&Caps);
	if( Caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL ){
		//�A���t�@�e�X�g�ݒ�
		//�@���S�ɓ����ȃs�N�Z���͕`�悵�Ȃ�
		pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
		pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}

	//�[�x�o�b�t�@��r�֐�
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	//�|���S���̗��E�\
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


	//�e�L�X�g�X�v���C�g������
	if( FAILED( D3DXCreateSprite( pd3dDevice, &ptextsprite ) ) ){
		return 1;
	}
	//�t�H���g���F�l�r �S�V�b�N�@�T�C�Y�F18
	HRESULT hr = D3DXCreateFont( pd3dDevice, -18, 0, FW_NORMAL, 1, FALSE, SHIFTJIS_CHARSET , OUT_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "�l�r �S�V�b�N", &pxmsfont);
	if( FAILED(hr) ) return 1;

	//�e�N�X�`���t�H���g�p�摜���擾
	TextureFont = LoadTexture(TextureFontFname, true, false);
	return 0;
}

//! @brief �f�o�C�X�̃��\�[�X�����
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

//! @brief ���f���t�@�C����ǂݍ��ށi.x�j
//! @param filename �t�@�C����
//! @return �����F���f���F���ԍ��i0�ȏ�j�@���s�F-1
int D3DGraphics::LoadModel(char* filename)
{
#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "���f��", filename);
#endif

	int id = -1;

	//�󂢂Ă���v�f��T��
	for(int i=0; i<MAX_MODEL; i++){
		if( pmesh[i] == NULL ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	LPD3DXBUFFER pD3DXMtrlBuffer;

#ifdef PATH_DELIMITER_SLASH
	//�p�X��؂蕶����ϊ�
	filename = ChangePathDelimiter(filename);
#endif

	//.x�t�@�C�����o�b�t�@�[�ɓǂݍ���
	if( FAILED( D3DXLoadMeshFromX( filename, D3DXMESH_SYSTEMMEM, pd3dDevice, NULL, 
				&pD3DXMtrlBuffer, NULL, &nummaterials[id], &pmesh[id] ) ) ) {
		return -1;
	}

	//�}�e���A�������擾
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	int num = nummaterials[id];
	pmaterials[id] = new D3DMATERIAL9[num];
	if( pmaterials[id]  == NULL ) return -1;

	//�\���̂ɑ��
	for( int i=0; i<num; i=i+1 ){
		pmaterials[id][i] = d3dxMaterials[i].MatD3D;
		pmaterials[id][i].Ambient = pmaterials[id][i].Diffuse;
	}

	//�o�b�t�@���J��
	pD3DXMtrlBuffer->Release();

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif
	return id;
}

//! @brief ���f���t�@�C���̒��ԃf�[�^���쐬�i���[�t�B���O�j
//! @param idA ���f��A�̔F���ԍ�
//! @param idB ���f��B�̔F���ԍ�
//! @return �����F�V�������f���F���ԍ��i0�ȏ�j�@���s�F-1
//! @attention ���f��A�ƃ��f��B�́A���_���E�|���S�����E�C���f�b�N�X�������ł���K�v������܂��B
//! @attention ���ꂼ��̃��f���f�[�^���������Ȃ��� ���_�����قȂ�ꍇ�A���s�Ɏ��s���܂��B
int D3DGraphics::MorphingModel(int idA, int idB)
{
#ifdef ENABLE_DEBUGLOG
	char str[128];
	sprintf(str, "���ԃf�[�^�쐬�@�@ID�F%d and %d", idA, idB);

	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "���f��", str);
#endif

	//�f�[�^�������������ׂ�
	if( (idA < 0)||((MAX_MODEL -1) < idA) ){ return -1; }
	if( pmesh[idA] == NULL ){ return -1; }
	if( (idB < 0)||((MAX_MODEL -1) < idB) ){ return -1; }
	if( pmesh[idB] == NULL ){ return -1; }

	int idN = -1;
	int numvA, numvB;
	LPDIRECT3DVERTEXBUFFER9 pvbA, pvbB, pvbN;
	D3DXVECTOR3 *pVerticesA, *pVerticesB, *pVerticesN;
	int FVFsize;

	//�󂢂Ă���v�f��T��
	for(int i=0; i<MAX_MODEL; i++){
		if( pmesh[i] == NULL ){
			idN = i;
			break;
		}
	}
	if( idN == -1 ){ return -1; }

	//���_�����擾
	numvA = pmesh[idA]->GetNumVertices();
	numvB = pmesh[idB]->GetNumVertices();

	//���_�����������ǂ������ׂ�
	if( numvA != numvB ){ return -1; }

	//���_�f�[�^���R�s�[�i�����I�ɗ̈�m�ۗp�̃_�~�[�j
	if( pmesh[idA]->CloneMeshFVF(pmesh[idA]->GetOptions(), pmesh[idA]->GetFVF(), pd3dDevice, &pmesh[idN]) != D3D_OK ){
		return -1;
	}

	//�}�e���A�������R�s�[
	int num = nummaterials[idA];
	nummaterials[idN] = nummaterials[idA];
	pmaterials[idN] = new D3DMATERIAL9[num];
	if( pmaterials[idN]  == NULL ) return -1;
	for( int i=0; i<num; i=i+1 ){
		pmaterials[idN][i] = pmaterials[idA][i];
	}

	//�o�b�t�@�[���擾
	pmesh[idA]->GetVertexBuffer(&pvbA);
	pmesh[idB]->GetVertexBuffer(&pvbB);
	pmesh[idN]->GetVertexBuffer(&pvbN);

	//1���_������̃o�C�g���擾
	FVFsize = D3DXGetFVFVertexSize(pmesh[idN]->GetFVF());

	//�e���_��ǂݏo���v�Z
	for(int i=0; i<numvA; i++){
		pvbA->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesA, D3DLOCK_READONLY);
		pvbB->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesB, D3DLOCK_READONLY);
		pvbN->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesN, 0);

		//���ω�
		pVerticesN->x = (pVerticesA->x + pVerticesB->x)/2;
		pVerticesN->y = (pVerticesA->y + pVerticesB->y)/2;
		pVerticesN->z = (pVerticesA->z + pVerticesB->z)/2;

		pvbA->Unlock();
		pvbB->Unlock();
		pvbN->Unlock();
	}

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", idN);
#endif
	return idN;
}

//! @brief ���f���t�@�C�������
//! @param id ���f���F���ԍ�
void D3DGraphics::CleanupModel(int id)
{
	if( (id < 0)||((MAX_MODEL -1) < id) ){ return; }
	if( pmesh[id] != NULL ){
		delete [] pmaterials[id];

		pmesh[id]->Release();
		pmesh[id] = NULL;

#ifdef ENABLE_DEBUGLOG
		//���O�ɏo��
		OutputLog.WriteLog(LOG_CLEANUP, "���f��", id);
#endif
	}
}

//! @brief �e�N�X�`����ǂݍ���
//! @param filename �t�@�C����
//! @param texturefont �e�N�X�`���t�H���g�t���O
//! @param BlackTransparent ���𓧉߂���
//! @return �����F�e�N�X�`���F���ԍ��i0�ȏ�j�@���s�F-1
int D3DGraphics::LoadTexture(char* filename, bool texturefont, bool BlackTransparent)
{
	int id = -1;
	D3DXIMAGE_INFO info;
	int MipLevels;

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "�e�N�X�`��", filename);
#endif

	//�󂢂Ă���F���ԍ���T��
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i] == NULL ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

#ifdef PATH_DELIMITER_SLASH
	//�p�X��؂蕶����ϊ�
	filename = ChangePathDelimiter(filename);
#endif

	//�t�@�C�������擾
	if( D3DXGetImageInfoFromFile(filename, &info) != D3D_OK ){ return -1; }

	//�~�b�v�}�b�v���x����ݒ�
	if( texturefont == true ){
		MipLevels = 1;
	}
	else{
		MipLevels = 4;//D3DX_DEFAULT;
	}

	//�e�N�X�`����ǂݍ���
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
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif
	return id;
}

//! @brief �e�N�X�`���̃T�C�Y���擾
//! @param id �e�N�X�`���F���ԍ�
//! @param width �����󂯎��|�C���^
//! @param height �������󂯎��|�C���^
//! @return �����F0�@���s�F1
//! @attention �T�[�t�F�C�X�̃T�C�Y���擾���܂��BGPU�Ƀ��[�h���ꂽ�T�C�Y�ł���A�e�N�X�`���i�����j�ƈقȂ�ꍇ������܂��B
int D3DGraphics::GetTextureSize(int id, int *width, int *height)
{
	//�����ȔF���ԍ����w�肳��Ă�����A���������Ԃ��B
	if( id == -1 ){ return 1; }
	if( ptextures[id] == NULL ){ return 1; }

	IDirect3DSurface9 *surface;
	D3DSURFACE_DESC desc;

	//�T�[�t�F�C�X���擾
	ptextures[id]->GetSurfaceLevel(0, &surface);

	//���ƍ������擾
	surface->GetDesc(&desc);
	*width = desc.Width;
	*height = desc.Height;

	//�T�[�t�F�C�X���J��
	surface->Release();

	return 0;
}

//! @brief �e�N�X�`�������
//! @param id �e�N�X�`���F���ԍ�
void D3DGraphics::CleanupTexture(int id)
{
	if( (id < 0)||((MAX_TEXTURE -1) < id) ){ return; }
	if( ptextures[id] != NULL ){
		ptextures[id]->Release();
		ptextures[id] = NULL;

#ifdef ENABLE_DEBUGLOG
		//���O�ɏo��
		OutputLog.WriteLog(LOG_CLEANUP, "�e�N�X�`��", id);
#endif
	}
}

//! @brief �S�Ă̕`�揈�����J�n
//! @return �����F0�@���s�F1
//! @attention �`�揈���̍ŏ��ɌĂяo���K�v������܂��B
int D3DGraphics::StartRender()
{
	if( StartRenderFlag == true ){ return 1; }

	//�̈��������
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

	if( SUCCEEDED( pd3dDevice->BeginScene() ) ){
		//Z�o�b�t�@������
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		//���W�[���n�_�Ƀ��[���h�ϊ��s��
		ResetWorldTransform();

		//�`�撆�̃t���O�𗧂Ă�
		StartRenderFlag = true;
		return 0;
	}

	return 1;
}

//! @brief �S�Ă̕`�揈�����I��
//! @return �����Ffalse�@���s�Ftrue
//! @attention �`�揈���̍Ō�ɌĂяo���K�v������܂��B
bool D3DGraphics::EndRender()
{
	//�`�撆�Ȃ�I��
	if( StartRenderFlag == true ){
		pd3dDevice->EndScene();
	}

	HRESULT hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);

	//�t���O�� false ��
	StartRenderFlag = false;

	if( hr == D3DERR_DEVICELOST ){
		return true;
	}
	return false;
}

//! @brief Z�o�b�t�@�����Z�b�g
void D3DGraphics::ResetZbuffer()
{
	//Z�o�b�t�@����x�����ɂ��A��������A�ēx�L����
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

//! @brief ���[���h��Ԃ����_�i0,0,0�j�ɖ߂��@�Ȃ�
void D3DGraphics::ResetWorldTransform()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

//! @brief ���[���h��Ԃ̍��W�E�p�x�E�g�嗦��ݒ�
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param rx �����p�x
//! @param ry �c���p�x
//! @param size �g�嗦
void D3DGraphics::SetWorldTransform(float x, float y, float z, float rx, float ry, float size)
{
	SetWorldTransform(x, y, z, rx, ry, 0.0f, size);
}

//! @brief ���[���h��Ԃ̍��W�E�p�x�E�g�嗦��ݒ�
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param rx �����p�x
//! @param ry1 �c���p�x
//! @param ry2 �c���p�x
//! @param size �g�嗦
void D3DGraphics::SetWorldTransform(float x, float y, float z, float rx, float ry1, float ry2, float size)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//�s����쐬
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationX(&matWorld3, ry1);
	D3DXMatrixRotationZ(&matWorld4, ry2);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//�v�Z
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//�K�p
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//! @brief ���[���h��Ԃ̍��W�E�p�x�E�g�嗦��ݒ�i�G�t�F�N�g�p�j
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param rx �����p�x
//! @param ry �c���p�x
//! @param rt ��]�p�x
//! @param size �g�嗦
void D3DGraphics::SetWorldTransformEffect(float x, float y, float z, float rx, float ry, float rt, float size)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//�s����쐬
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationZ(&matWorld3, ry);
	D3DXMatrixRotationX(&matWorld4, rt);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//�v�Z
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//�K�p
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//! @brief ���[���h��Ԃ�l����������ꏊ�ɐݒ�
//! @param x X���W
//! @param y Y���W
//! @param z Z���W
//! @param mx �茳�����_�ɂ��� ���f����X���W
//! @param my �茳�����_�ɂ��� ���f����Y���W
//! @param mz �茳�����_�ɂ��� ���f����Z���W
//! @param rx �����p�x
//! @param ry �c���p�x
//! @param size �g�嗦
void D3DGraphics::SetWorldTransformHumanWeapon(float x, float y, float z, float mx, float my, float mz, float rx, float ry, float size)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//�s����쐬
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationX(&matWorld3, ry);
	D3DXMatrixTranslation(&matWorld4, mx, my, mz);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//�v�Z
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//�K�p
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//! @brief ���[���h��Ԃ��������Ă��镐���\������ꏊ�ɐݒ�
//! @param rotation �������]������
//! @param camera_x �J������X���W
//! @param camera_y �J������Y���W
//! @param camera_z �J������Z���W
//! @param camera_rx �J�����̉����p�x
//! @param camera_ry �J�����̏c���p�x
//! @param rx ����̂̏c���p�x
//! @param size �\���T�C�Y
//! @note rotation�E�E�@true�F���ݎ����Ă��镐��ł��B�@false�F�\���̕���ł��B�irx �͖�������܂��j
//! @todo �ʒu��T�C�Y�̔�����
void D3DGraphics::SetWorldTransformPlayerWeapon(bool rotation, float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float rx, float size)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5, matWorld6;

	size = size * 0.3f;

	//�s����쐬
	D3DXMatrixTranslation(&matWorld1, camera_x, camera_y, camera_z);
	D3DXMatrixRotationY(&matWorld2, camera_rx *-1);
	D3DXMatrixRotationZ(&matWorld3, camera_ry);
	// matWorld4 = [���s��, �c, ��]
	if( rotation == true ){
		D3DXMatrixTranslation(&matWorld4, HUD_myweapon_x[0], HUD_myweapon_y[0], HUD_myweapon_z[0]);
		D3DXMatrixRotationY(&matWorld5, rx);
	}
	else{
		D3DXMatrixTranslation(&matWorld4, HUD_myweapon_x[1], HUD_myweapon_y[1], HUD_myweapon_z[1]);
		D3DXMatrixRotationY(&matWorld5, D3DX_PI);
	}
	D3DXMatrixScaling(&matWorld6, size, size, size);

	//�v�Z
	matWorld = matWorld6 * matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//�K�p
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//! @brief ���[���h��Ԃ̍��W���擾
//! @param *x x�����󂯎��|�C���^
//! @param *y y�����󂯎��|�C���^
//! @param *z z�����󂯎��|�C���^
void D3DGraphics::GetWorldTransformPos(float *x, float *y, float *z)
{
	D3DXMATRIX matWorld;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
	*x = matWorld._41;
	*y = matWorld._42;
	*z = matWorld._43;
}

//! @brief �t�H�O��ݒ�
//! @param skynumber ��̔ԍ�
void D3DGraphics::SetFog(int skynumber)
{
	D3DCOLOR skycolor;

	//��̔ԍ��ɂ��F������
	switch(skynumber){
		case 1: skycolor = D3DCOLOR_RGBA(64, 64+16, 64, 0); break;
		case 2: skycolor = D3DCOLOR_RGBA(16, 16, 16, 0); break;
		case 3: skycolor = D3DCOLOR_RGBA(0, 16, 32, 0); break;
		case 4: skycolor = D3DCOLOR_RGBA(32, 16, 16, 0); break;
		case 5: skycolor = D3DCOLOR_RGBA(64, 32, 32, 0); break;
		default: skycolor = D3DCOLOR_RGBA(0, 0, 0, 0); break;
	}

	//�t�H�O��ݒ�
	pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, skycolor);
}

//! @brief �J�����i���_�j��ݒ�
//! @param camera_x �J������X���W
//! @param camera_y �J������Y���W
//! @param camera_z �J������Z���W
//! @param camera_rx �J�����̉����p�x
//! @param camera_ry �J�����̏c���p�x
//! @param viewangle ����p
void D3DGraphics::SetCamera(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float viewangle)
{
	float vUpVecF;
	D3DXMATRIX matWorld;
	D3DXMATRIXA16 matView;

	//camera_ry�� -PI�`PI �̊Ԃɐ��K��
	for(; camera_ry>D3DX_PI; camera_ry -= D3DX_PI*2){}
	for(; camera_ry<D3DX_PI*-1; camera_ry += D3DX_PI*2){}

	//�J�����̌���������
	if( fabs(camera_ry) < D3DX_PI/2 ){
		vUpVecF = 1.0f;
	}
	else{
		vUpVecF = -1.0f;
	}

	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	//�J�������W
	D3DXVECTOR3 vEyePt( camera_x, camera_y, camera_z );
	D3DXVECTOR3 vLookatPt( cos(camera_rx)*cos(camera_ry) + camera_x, sin(camera_ry) + camera_y, sin(camera_rx)*cos(camera_ry) + camera_z );
	D3DXVECTOR3 vUpVec( 0.0f, vUpVecF, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//�J�����ݒ�i�ˉe�ϊ��s��jviewangle
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, viewangle, aspect, CLIPPINGPLANE_NEAR, CLIPPINGPLANE_FAR);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

//! @brief �}�b�v�f�[�^����荞��
//! @param in_blockdata �u���b�N�f�[�^
//! @param directory �u���b�N�f�[�^�����݂���f�B���N�g��
void D3DGraphics::LoadMapdata(BlockDataInterface* in_blockdata, char *directory)
{
	//�u���b�N�f�[�^���w�肳��Ă��Ȃ���΁A�������Ȃ��B
	if( in_blockdata == NULL ){ return; }

	char fname[MAX_PATH];
	char fnamefull[MAX_PATH];
	//int bs;
	struct blockdata data;
	int vID[4];
	int uvID[4];

	//�N���X��ݒ�
	blockdata = in_blockdata;

	//�e�N�X�`���ǂݍ���
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		//�e�N�X�`�������擾
		blockdata->GetTexture(fname, i);

		if( strcmp(fname, "") == 0 ){	//�w�肳��Ă��Ȃ���΁A�������Ȃ�
			mapTextureID[i] = -1;
		}
		else{
			//�u�f�B���N�g���{�t�@�C�����v�𐶐����A�ǂݍ���
			strcpy(fnamefull, directory);
			strcat(fnamefull, fname);
			mapTextureID[i] = LoadTexture(fnamefull, false, false);
		}
	}

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "�}�b�v", "�i���_�f�[�^�j");
#endif

#ifdef BLOCKDATA_GPUMEMORY
	VERTEXTXTA* pVertices;

	//�u���b�N�����擾
	bs = blockdata->GetTotaldatas();

	//�u���b�N�����̃o�b�t�@�[���쐬
	pd3dDevice->CreateVertexBuffer(bs*6*4*sizeof(VERTEXTXTA),0,D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,D3DPOOL_DEFAULT,&g_pVB,NULL);

	for(int i=0; i<bs; i++){
		//�f�[�^���擾
		blockdata->Getdata(&data, i);

		for(int j=0; j<6; j++){
			//�ʂ̒��_�f�[�^�̊֘A�t�����擾
			blockdataface(j, &vID[0], &uvID[0]);

			//GPU�����b�N�i1�ʕ��j
			g_pVB->Lock((i*6+j)*4*sizeof(VERTEXTXTA), 4*sizeof(VERTEXTXTA), (void**)&pVertices, 0);

			//���_���W�EUV���W�E�F��ݒ�
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

			//GPU�̃��b�N������
			g_pVB->Unlock();
		}
	}
#else
	//�u���b�N�����擾
	bs = blockdata->GetTotaldatas();

	for(int i=0; i<bs; i++){
		//�f�[�^���擾
		blockdata->Getdata(&data, i);

		for(int j=0; j<6; j++){
			//�ʂ̒��_�f�[�^�̊֘A�t�����擾
			blockdataface(j, vID, uvID);

			//���_���W�EUV���W�E�F��ݒ�
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
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
}

//! @brief �}�b�v�f�[�^��`��
//! @param wireframe ���C���[�t���[���\��
void D3DGraphics::DrawMapdata(bool wireframe)
{
	//�u���b�N�f�[�^���ǂݍ��܂�Ă��Ȃ���΁A�������Ȃ��B
	if( blockdata == NULL ){ return; }

	struct blockdata data;
	int textureID;

	if( wireframe == true ){
		//���C���[�t���[���\��
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

	//�[�x�o�b�t�@��r�֐���ݒ�
	//pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);


#ifdef BLOCKDATA_GPUMEMORY
	//�f�[�^�ݒ�
	pd3dDevice->SetStreamSource(0,g_pVB,0,sizeof(VERTEXTXTA));

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		//�e�N�X�`��������ɓǂݍ��߂Ă��Ȃ���ΐݒ�
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
			//�f�[�^�擾
			blockdata->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//�e�N�X�`���F���ԍ����擾
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//�ʂ�`��
					pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (i*6+j)*4, 2);
				}
			}
		}
	}
#else
	//�f�[�^��ݒ�
	pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		//�e�N�X�`��������ɓǂݍ��߂Ă��Ȃ���ΐݒ�
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
			//�f�[�^�擾
			blockdata->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//�e�N�X�`���F���ԍ����擾
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//�ʂ�`��
					pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_pVertices[i][j], sizeof(VERTEXTXTA));
				}
			}
		}
	}
#endif

	//�[�x�o�b�t�@��r�֐������ɖ߂�
	//pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

//! @brief �}�b�v�e�N�X�`�����擾
//! @param id �e�N�X�`���ԍ�
//! @return �e�N�X�`���F���ԍ��i���s�F-1�j
int D3DGraphics::GetMapTextureID(int id)
{
	if( (id < 0)||((TOTAL_BLOCKTEXTURE -1) < id ) ){ return -1; }
	return mapTextureID[id];
}

//! @brief �}�b�v�f�[�^�����
void D3DGraphics::CleanupMapdata()
{
	//�e�N�X�`�����J��
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		CleanupTexture(mapTextureID[i]);
	}

#ifdef BLOCKDATA_GPUMEMORY
	//���_�f�[�^���
	if( g_pVB != NULL ){
		g_pVB->Release();
		g_pVB = NULL;
	}
#endif
	bs = 0;

	blockdata = NULL;

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "�}�b�v", "�i���_�f�[�^�j");
#endif
}

//! @brief ���f���t�@�C����`��
//! @param id_model ���f���F���ԍ�
//! @param id_texture �e�N�X�`���F���ԍ�
void D3DGraphics::RenderModel(int id_model, int id_texture)
{
	//�����Ȉ������ݒ肳��Ă���Ύ��s
	if( id_model == -1 ){ return; }
	//if( id_texture == -1 ){ return; }

	//�w�肵�����f��������������Ă��Ȃ���Ύ��s
	if( pmesh[id_model] == NULL) return;

	//�`��
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

//! @brief ��`��
//! @param id_texture �e�N�X�`���F���ԍ�
//! @param alpha �����x�@�i0.0�`1.0�@0.0�F���S�����j
void D3DGraphics::RenderBoard(int id_texture, float alpha)
{
	//�e�N�X�`�����ݒ肳��Ă��Ȃ���΁A�������Ȃ��B
	if( id_texture == -1 ){ return; }

	VERTEXTXTA BoardVertices[4];

	//���_���W�EUV���W�E�F/�����x��ݒ�
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

	//�A���t�@�u�����h��ݒ�
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//�e�N�X�`���ƃf�[�^�`����ݒ肵�`��
	pd3dDevice->SetTexture(0, ptextures[id_texture]);
	pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, BoardVertices, sizeof(VERTEXTXTA));

	//�A���t�@�u�����h�����ɖ߂�
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

//! @brief ��ʂ̖��邳��ݒ�
//! @param Width ��
//! @param Height ����
//! @param Brightness ��ʂ̖��邳�@�i0 �ŕs�ρA1 �ȏ�Ŗ��邳�̓x�����j
void D3DGraphics::ScreenBrightness(int Width, int Height, int Brightness)
{
	//���邳�s�ςȂ珈�����Ȃ��i�y�ʉ��j
	if( Brightness == 0 ){ return; }

	//�����x��ݒ肵�A�`��
	float alpha = 0.02f * Brightness;
	Draw2DBox(0, 0, Width, Height, D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,alpha));
}

//! @brief �y�f�o�b�N�p�z���S���`��
void D3DGraphics::Centerline()
{
	ResetWorldTransform();
	Drawline(100.0f, 0.0f, 0.0f, -100.0f, 0.0f, 0.0f);
	Drawline(0.0f, 100.0f, 0.0f, 0.0f, -100.0f, 0.0f);
	Drawline(0.0f, 0.0f, 100.0f, 0.0f, 0.0f, -100.0f);
}

//! @brief �y�f�o�b�N�p�z�ΐ��`��
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

//! @brief 2D �V�X�e���t�H���g�ɂ��e�L�X�g�`����J�n
//! @attention DirectX�� ID3DXSprite �����������Ă��܂��B
void D3DGraphics::Start2DMSFontTextRender()
{
	ptextsprite->Begin(D3DXSPRITE_ALPHABLEND);
}

//! @brief ������`��i�V�X�e���t�H���g�g�p�j
//! @param x x���W
//! @param y y���W
//! @param str ������@�i���s�R�[�h�F�j
//! @param color �F
//! @warning <b>�`��͔��ɒᑬ�ł��B</b>��ʓ��ŉ��x���Ăяo���ƃp�t�H�[�}���X�ɉe�����܂��B
//! @warning�u���s�R�[�h�����p����x�ɕ`�悷��v�u���{�ꂪ�K�v�Ȃ������̓e�N�X�`���t�H���g�����p����v�Ȃǂ̑Ή����u���Ă��������B
//! @attention DirectX�� ID3DXSprite ���g�p���A�V�X�e���t�H���g�ŕ`��Ă��܂��B
//! @attention �t�H���g�̎�ނ�T�C�Y�͌Œ�ł��B�@�������d�ɏd�˂ė��̊����o���Ȃ��ƌ��ɂ����Ȃ�܂��B
void D3DGraphics::Draw2DMSFontText(int x, int y, char *str, int color)
{
	//if( ptextsprite == NULL ){ return; }

	//�e�L�X�g�X�v���C�g������
	Start2DMSFontTextRender();

	//����W��ݒ�
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	//��������`��
	RECT rc = {x, y, 0, 0};
	pxmsfont->DrawText(ptextsprite, str, -1, &rc, DT_NOCLIP, color);

	//�e�L�X�g�X�v���C�g���
	End2DMSFontTextRender();
}

//! @brief �����𒆉������ŕ`��i�V�X�e���t�H���g�g�p�j
//! @param x x���W
//! @param y y���W
//! @param w ���̑傫��
//! @param h �c�̑傫��
//! @param str ������@�i���s�R�[�h�F�j
//! @param color �F
//! @warning <b>�`��͔��ɒᑬ�ł��B</b>��ʓ��ŉ��x���Ăяo���ƃp�t�H�[�}���X�ɉe�����܂��B
//! @warning�u���s�R�[�h�����p����x�ɕ`�悷��v�u���{�ꂪ�K�v�Ȃ������̓e�N�X�`���t�H���g�����p����v�Ȃǂ̑Ή����u���Ă��������B
//! @attention DirectX�� ID3DXSprite ���g�p���A�V�X�e���t�H���g�ŕ`��Ă��܂��B
//! @attention �t�H���g�̎�ނ�T�C�Y�͌Œ�ł��B�@�������d�ɏd�˂ė��̊����o���Ȃ��ƌ��ɂ����Ȃ�܂��B
void D3DGraphics::Draw2DMSFontTextCenter(int x, int y, int w, int h, char *str, int color)
{
	//if( ptextsprite == NULL ){ return; }

	//�e�L�X�g�X�v���C�g������
	Start2DMSFontTextRender();

	//����W��ݒ�
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	//��������`��
	RECT rc = {x, y, x+w, y+h};
	pxmsfont->DrawText(ptextsprite, str, -1, &rc, DT_CENTER, color);

	//�e�L�X�g�X�v���C�g���
	End2DMSFontTextRender();
}

//! @brief 2D �V�X�e���t�H���g�ɂ��e�L�X�g�`����I��
//! @attention DirectX�� ID3DXSprite ��������Ă��܂��B
void D3DGraphics::End2DMSFontTextRender()
{
	ptextsprite->End();
}

//! @brief 2D�`��p�ݒ�
void D3DGraphics::Start2DRender()
{
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//�[�x�o�b�t�@��r�֐���ݒ�
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
}

//! @brief ������`��i�e�N�X�`���t�H���g�g�p�j
//! @param x x���W
//! @param y y���W
//! @param str ������@�i���s�R�[�h�F<b>�s��</b>�j
//! @param color �F
//! @param fontwidth �ꕶ���̕�
//! @param fontheight �ꕶ���̍���
//! @attention �������d�ɏd�˂ė��̊����o���Ȃ��ƌ��ɂ����Ȃ�܂��B
void D3DGraphics::Draw2DTextureFontText(int x, int y, char *str, int color, int fontwidth, int fontheight)
{
	//�e�N�X�`���t�H���g�̎擾�Ɏ��s���Ă���΁A�������Ȃ�
	if( TextureFont == -1 ){ return; }

	//2D�`��p�ݒ��K�p
	Start2DRender();

	int w;
	float font_u, font_v;
	float t_u, t_v;
	TLVERTX pBoxVertices[4];

	//1������UV���W���v�Z
	font_u = 1.0f / 16;
	font_v = 1.0f / 16;

	//���[���h���W�����_�ɖ߂�
	ResetWorldTransform();

	//�e�N�X�`�����t�H���g�e�N�X�`���ɐݒ�
	pd3dDevice->SetTexture( 0, ptextures[TextureFont] );

	//�f�[�^�`����ݒ�
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	// �^����ꂽ�����������[�v
	for(int i=0; i<(int)strlen(str); i++){
		//UV���W���v�Z
		w = str[i];
		if( w < 0 ){ w += 256; }
		t_u = (w % 16) * font_u;
		t_v = (w / 16) * font_v;

		//���_���W�EUV���W�E�F��ݒ�
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

		//�`��
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));
	}

	//2D�`��p�ݒ������
	End2DRender();
}

//! @brief ����`��
//! @param x1 �n�_�� x���W
//! @param y1 �n�_�� y���W
//! @param x2 �I�_�� x���W
//! @param y2 �I�_�� y���W
//! @param color �F
void D3DGraphics::Draw2DLine(int x1, int y1, int x2, int y2, int color)
{
	TLVERTX pLineVertices[2];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	//���[���h���W�����_�ɖ߂�
	ResetWorldTransform();

	//���_���W�ƐF�Ȃǂ�ݒ�
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

	//�f�[�^�`����ݒ肵�A�`��B
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, pLineVertices, sizeof(TLVERTX));

	//2D�`��p�ݒ������
	End2DRender();
}

//! @brief �~�i16�p�`�j��`��
//! @param x ���S�� x���W
//! @param y ���S�� y���W
//! @param r ���a
//! @param color �F
void D3DGraphics::Draw2DCycle(int x, int y, int r, int color)
{
	TLVERTX pLineVertices[16+1];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	//���[���h���W�����_�ɖ߂�
	ResetWorldTransform();

	//���_���W�ƐF�Ȃǂ�ݒ�
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

	//�f�[�^�`����ݒ肵�A�`��B
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, pLineVertices, sizeof(TLVERTX));

	//2D�`��p�ݒ������
	End2DRender();
}

//! @brief �l�p�`��`��
//! @param x1 ����� x���W
//! @param y1 ����� y���W
//! @param x2 �E���� x���W
//! @param y2 �E���� y���W
//! @param color �F
void D3DGraphics::Draw2DBox(int x1, int y1, int x2, int y2, int color)
{
	TLVERTX pBoxVertices[4];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	//���[���h���W�����_�ɖ߂�
	ResetWorldTransform();

	//���_���W�ƐF�Ȃǂ�ݒ�
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

	//�f�[�^�`����ݒ肵�A�`��B
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));

	//2D�`��p�ݒ������
	End2DRender();
}

//! @brief �摜��`��
//! @param x x���W
//! @param y y���W
//! @param id �e�N�X�`���F���ԍ�
//! @param width ��
//! @param height ����
//! @param alpha �����x�i0.0�`1.0�j
void D3DGraphics::Draw2DTexture(int x, int y, int id, int width, int height, float alpha)
{
	//�����ȃe�N�X�`���ԍ����w�肳��Ă���Ώ������Ȃ�
	if( id == -1 ){ return; }

	TLVERTX pBoxVertices[4];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	//���[���h���W�����_�ɖ߂�
	ResetWorldTransform();

	//���_���W�EUV���W�E�F��ݒ�
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

	//�e�N�X�`���ƃf�[�^�`����ݒ肵�A�`��
	pd3dDevice->SetTexture( 0, ptextures[id] );
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));

	//2D�`��p�ݒ������
	End2DRender();
}

//! @brief 2D�`��p�ݒ������
void D3DGraphics::End2DRender()
{
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	//�[�x�o�b�t�@��r�֐������ɖ߂�
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

//! @brief ��ʂ̃X�N���[���V���b�g��ۑ�
//! @param filename �t�@�C����
//! @return �����Ftrue�@���s�Ffalse
bool D3DGraphics::SaveScreenShot(char* filename)
{
	LPDIRECT3DSURFACE9 pSurface = NULL;
	HRESULT hr;

	//�T�[�t�F�[�X���쐬���A��ʂ��擾
	pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);

	//�T�[�t�F�C�X���摜�ɏo��
	hr = D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, pSurface, NULL, NULL);

	//���
	pSurface->Release();

	if( hr == D3D_OK ){
		return true;
	}
	return false;
}

//! @brief �J���[�R�[�h���擾
//! @param red �ԁi0.0f�`1.0f�j
//! @param green �΁i0.0f�`1.0f�j
//! @param blue �i0.0f�`1.0f�j
//! @param alpha �����x�i0.0f�`1.0f�j
//! @return �J���[�R�[�h
int D3DGraphics::GetColorCode(float red, float green, float blue, float alpha)
{
	return D3DCOLOR_COLORVALUE(red, green, blue, alpha);
}

#endif	//GRAPHICS_OPENGL