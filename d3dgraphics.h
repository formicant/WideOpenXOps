//! @file d3dgraphics.h
//! @brief D3DGraphics�N���X�̐錾

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

#ifndef DIRECTX_H
#define DIRECTX_H

#define MAX_MODEL 96		//!< �ő僂�f����
#define MAX_TEXTURE 64		//!< �ő�e�N�X�`����

#define CLIPPINGPLANE_NEAR 1.0f		//!< �߃N���b�v�ʁi���`�悷��ŏ������j
#define CLIPPINGPLANE_FAR 800.0f	//!< ���N���b�v�ʁi���`�悷��ő勗���j

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

#define BLOCKDATA_GPUMEMORY	//!< @brief �u���b�N�f�[�^���i�[���郁�����[��I�� @details �萔�錾�L���FGPU�������[�@�萔�錾�����i�R�����g���j�F���C���������[

#pragma warning(disable:4819)		//VC++�x���h�~
#include <d3dx9.h>
#pragma warning(default:4819)

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//! 3D�|���S���`��p�\����
struct VERTEXTXTA
{
	D3DXVECTOR3 position;	//!< The position
	DWORD       color;		//!< The color 
	FLOAT       tu;			//!< The texture coordinates
	FLOAT       tv;			//!< The texture coordinates
};

//! 2D�|���S���`��p�\����
struct TLVERTX {
	FLOAT    x;		//!< position
	FLOAT    y;		//!< position
	FLOAT    z;		//!< position
	FLOAT    rhw;	//!< vector
	D3DCOLOR color;	//!< color
	FLOAT    tu;	//!< texture coordinates
	FLOAT    tv;	//!< texture coordinates
};

//! @brief ��ʕ`����s���N���X
//! @details ��ʂ̕`��@�\�₻��ɒ��ڊ֘A���鏈�����s���܂��B
//! @details ��̓I�ɁA3D�`��E2D�`��E���f���t�@�C����e�N�X�`���Ǘ��@���s���@�\������܂��B
//! @details �����ł�DirectX 9���g�p���Ă��܂��B
class D3DGraphics
{
	LPDIRECT3D9 pD3D;					//!< DIRECT3D9�̃|�C���^
	LPDIRECT3DDEVICE9 pd3dDevice;		//!< DIRECT3DDEVICE9�̃|�C���^
	float aspect;						//!< ��ʂ̃A�X�y�N�g��
	bool fullscreenflag;				//!< �t���X�N���[���\��
	LPD3DXMESH pmesh[MAX_MODEL];				//!< �iX�t�@�C���p�jD3DXMESH�̃|�C���^
	D3DMATERIAL9* pmaterials[MAX_MODEL];		//!< �iX�t�@�C���p�jD3DMATERIAL9�̃|�C���^
	DWORD nummaterials[MAX_MODEL];				//!< �iX�t�@�C���p�j�}�e���A����
	LPDIRECT3DTEXTURE9 ptextures[MAX_TEXTURE];	//!< �e�N�X�`�����i�[

	class BlockDataInterface* blockdata;		//!< �ǂݍ��񂾃u���b�N�f�[�^���i�[����N���X�ւ̃|�C���^
	int bs;										//!< �u���b�N��
	int mapTextureID[TOTAL_BLOCKTEXTURE];		//!< �e�N�X�`���ԍ�
#ifdef BLOCKDATA_GPUMEMORY
	LPDIRECT3DVERTEXBUFFER9 g_pVB;				//!< ���_���iGPU�Ǘ��j
#else
	VERTEXTXTA g_pVertices[MAX_BLOCKS][6][4];	//!< ���_���iCPU�Ǘ��j
#endif

	float HUD_myweapon_x[TOTAL_HAVEWEAPON];		//!< HUD_A�̕���\�� X���W
	float HUD_myweapon_y[TOTAL_HAVEWEAPON];		//!< HUD_A�̕���\�� Y���W
	float HUD_myweapon_z[TOTAL_HAVEWEAPON];		//!< HUD_A�̕���\�� Z���W

	bool StartRenderFlag;			//!< StartRender()�֐� ���s����\���t���O

	LPD3DXSPRITE ptextsprite;		//!< 2D�e�N�X�`���X�v���C�g
	LPD3DXFONT pxmsfont;			//!< �V�X�e���t�H���g
	char TextureFontFname[_MAX_PATH];	//!< �e�N�X�`���t�H���g�̃t�@�C����
	int TextureFont;				//!< �e�N�X�`���t�H���g�̃e�N�X�`��ID
	int TextureFont_width;			//!< �e�N�X�`���t�H���g�t�@�C���̉���
	int TextureFont_height;			//!< �e�N�X�`���t�H���g�t�@�C���̍���

	int InitSubset();
	void CleanupD3Dresource();
	void Start2DMSFontTextRender();
	void End2DMSFontTextRender();
	void Start2DRender();
	void End2DRender();

public:
	D3DGraphics();
	~D3DGraphics();
	int InitD3D(HWND hWnd, char *TextureFontFilename, bool fullscreen);
	int ResetD3D(HWND hWnd);
	int LoadModel(char* filename);
	int MorphingModel(int idA, int idB);
	void CleanupModel(int id);
	int LoadTexture(char* filename, bool texturefont, bool BlackTransparent);
	int GetTextureSize(int id, int *width, int *height);
	void CleanupTexture(int id);
	int StartRender();
	bool EndRender();
	void ResetZbuffer();
	void ResetWorldTransform();
	void SetWorldTransform(float x, float y, float z, float rx, float ry, float size);
	void SetWorldTransform(float x, float y, float z, float rx, float ry1, float ry2, float size);
	void SetWorldTransformEffect(float x, float y, float z, float rx, float ry, float rt, float size);
	void SetWorldTransformHumanWeapon(float x, float y, float z, float mx, float my, float mz, float rx, float ry, float size);
	void SetWorldTransformPlayerWeapon(bool rotation, float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float rx, float size);
	void GetWorldTransformPos(float *x, float *y, float *z);
	void SetFog(int skynumber);
	void SetCamera(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float viewangle);
	void LoadMapdata(class BlockDataInterface* in_blockdata, char *directory);
	void DrawMapdata(bool wireframe);
	int GetMapTextureID(int id);
	void CleanupMapdata();
	void RenderModel(int id_model, int id_texture);
	void RenderBoard(int id_texture, float alpha);
	void ScreenBrightness(int Width, int Height, int Brightness);
	void Centerline();
	void Drawline(float x1, float y1, float z1, float x2, float y2, float z2);
	void Draw2DMSFontText(int x, int y, char *str, int color);
	void Draw2DMSFontTextCenter(int x, int y, int w, int h, char *str, int color);
	void Draw2DTextureFontText(int x, int y, char *str, int color, int fontwidth, int fontheight);
	void Draw2DLine(int x1, int y1, int x2, int y2, int color);
	void Draw2DBox(int x1, int y1, int x2, int y2, int color);
	void Draw2DTexture(int x, int y, int id, int width, int height, float alpha);
	bool SaveScreenShot(char *fname);
};

#endif