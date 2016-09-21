//! @file d3dgraphics.h
//! @brief D3DGraphicsクラスの宣言

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

// ***** OpenGL core only *****
//
// libjpeg
//    Copyright (C) 1991-2014, Thomas G. Lane, Guido Vollbeding.
//    this software is based in part on the work of the Independent JPEG Group
//
// zlib
//    Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler
//
// libpng
//    Copyright (c) 1998-2014 Glenn Randers-Pehrson
//
// ****************************

#ifndef D3DGRAPHICS_H
#define D3DGRAPHICS_H

#define MAX_MODEL 96		//!< 最大モデル数
#define MAX_TEXTURE 64		//!< 最大テクスチャ数

#define CLIPPINGPLANE_NEAR 1.0f		//!< 近クリップ面（≒描画する最小距離）
#define CLIPPINGPLANE_FAR 800.0f	//!< 遠クリップ面（≒描画する最大距離）

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

//#define GRAPHICS_OPENGL		//!< @brief 使用するグラフィックスコアの選択 @details 定数宣言有効：OpenGL 1.1　定数宣言無効（コメント化）：DirectX 9.0c

#ifndef GRAPHICS_OPENGL

#define BLOCKDATA_GPUMEMORY	//!< @brief ブロックデータを格納するメモリーを選択 @details 定数宣言有効：GPUメモリー　定数宣言無効（コメント化）：メインメモリー

#pragma warning(disable:4819)		//VC++警告防止
#include <d3dx9.h>
#pragma warning(default:4819)

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define GRAPHICS_CORE "DirectX 9.0"		//!< バージョン表示用情報

#else	//GRAPHICS_OPENGL

#include <ctype.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "lib/libjpeg/jpeglib.h"
#include "lib/zlib/zlib.h"
#include "lib/libpng/png.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#pragma comment(lib, "lib/libjpeg/libjpeg_t.lib")
#pragma comment(lib, "lib/zlib/zlib.lib")
#pragma comment(lib, "lib/libpng/libpng.lib")

#define GRAPHICS_CORE "OpenGL 1.1"		//!< バージョン表示用情報

#endif	//GRAPHICS_OPENGL

#ifndef GRAPHICS_OPENGL

//! 3Dポリゴン描画用構造体
struct VERTEXTXTA
{
	D3DXVECTOR3 position;	//!< The position
	DWORD       color;		//!< The color 
	FLOAT       tu;			//!< The texture coordinates
	FLOAT       tv;			//!< The texture coordinates
};

//! 2Dポリゴン描画用構造体
struct TLVERTX
{
	FLOAT    x;		//!< position
	FLOAT    y;		//!< position
	FLOAT    z;		//!< position
	FLOAT    rhw;	//!< vector
	D3DCOLOR color;	//!< color
	FLOAT    tu;	//!< texture coordinates
	FLOAT    tv;	//!< texture coordinates
};

#else	//GRAPHICS_OPENGL

//! モデルデータの頂点格納構造体
struct MODELVDATA
{
	float x;	//!< position
	float y;	//!< position
	float z;	//!< position
	float u;	//!< texture coordinates
	float v;	//!< texture coordinates
};

//! モデルデータ構造体
struct MODELDATA
{
	bool useflag;			//!< 有効化フラグ
	int polygons;			//!< ポリゴン数
	float *VertexAry;		//!< 頂点格納配列
	float *ColorAry;		//!< 色格納配列
	float *TexCoordAry;		//!< テクスチャ座標格納配列
};

//! テクスチャ構造体
struct TEXTUREDATA
{
	bool useflag;			//!< 有効化フラグ
	int width;				//!< 幅
	int height;				//!< 高さ
	unsigned char *data;	//!< 実データ
};

#endif	//GRAPHICS_OPENGL


//! @brief 画面描画を行うクラス
//! @details 画面の描画機能やそれに直接関連する処理を行います。
//! @details 具体的に、3D描画・2D描画・モデルファイルやテクスチャ管理　を行う機能があります。
class D3DGraphics
{

#ifndef GRAPHICS_OPENGL

	LPDIRECT3D9 pD3D;					//!< DIRECT3D9のポインタ
	LPDIRECT3DDEVICE9 pd3dDevice;		//!< DIRECT3DDEVICE9のポインタ
	float aspect;						//!< 画面のアスペクト比
	bool fullscreenflag;				//!< フルスクリーン表示
	LPD3DXMESH pmesh[MAX_MODEL];				//!< （Xファイル用）D3DXMESHのポインタ
	D3DMATERIAL9* pmaterials[MAX_MODEL];		//!< （Xファイル用）D3DMATERIAL9のポインタ
	DWORD nummaterials[MAX_MODEL];				//!< （Xファイル用）マテリアル数
	LPDIRECT3DTEXTURE9 ptextures[MAX_TEXTURE];	//!< テクスチャを格納

	class BlockDataInterface* blockdata;		//!< 読み込んだブロックデータを格納するクラスへのポインタ
	int bs;										//!< ブロック数
	int mapTextureID[TOTAL_BLOCKTEXTURE];		//!< テクスチャ番号
#ifdef BLOCKDATA_GPUMEMORY
	LPDIRECT3DVERTEXBUFFER9 g_pVB;				//!< 頂点情報（GPU管理）
#else
	VERTEXTXTA g_pVertices[MAX_BLOCKS][6][4];	//!< 頂点情報（CPU管理）
#endif

	float HUD_myweapon_x[TOTAL_HAVEWEAPON];		//!< HUD_Aの武器描画 X座標
	float HUD_myweapon_y[TOTAL_HAVEWEAPON];		//!< HUD_Aの武器描画 Y座標
	float HUD_myweapon_z[TOTAL_HAVEWEAPON];		//!< HUD_Aの武器描画 Z座標

	bool StartRenderFlag;			//!< StartRender()関数 実行中を表すフラグ

	LPD3DXSPRITE ptextsprite;		//!< 2Dテクスチャスプライト
	LPD3DXFONT pxmsfont;			//!< システムフォント
	char TextureFontFname[_MAX_PATH];	//!< テクスチャフォントのファイル名
	int TextureFont;				//!< テクスチャフォントのテクスチャID

	int InitSubset();
	void CleanupD3Dresource();
	void Start2DMSFontTextRender();
	void End2DMSFontTextRender();
	void Start2DRender();
	void End2DRender();

#ifdef ENABLE_DEBUGCONSOLE
	unsigned char DebugFontData[96][16];	//!< デバック用フォントデータ格納配列
	int TextureDebugFont;					//!< デバック用フォントのテクスチャID
	void InitDebugFontData();
	bool LoadDebugFontTexture();
#endif

#else	//GRAPHICS_OPENGL

	HWND hWnd;				//!< ウィンドウハンドル
	HGLRC hGLRC;			//!< OpenGLのコンテキスト
	int width;				//!< 幅
	int height;				//!< 高さ
	PAINTSTRUCT Paint_ps;					//!< BeginPaint()関数とEndPaint()関数用
	MODELDATA pmodel[MAX_MODEL];			//!< モデルデータを格納
	TEXTUREDATA ptextures[MAX_TEXTURE];		//!< テクスチャを格納
	GLuint textureobjname[MAX_TEXTURE];		//!< テクスチャオブジェクト
	HFONT SystemFont;				//!< システムフォント用の論理フォント
	WCHAR *now_SystemFontUStr;		//!< 現在表示中のシステムフォントによる文字列（Unicode）
	GLuint SystemFontListIdx;		//!< システムフォントのディスプレイリスト
	int SystemFontListIdxSize;		//!< システムフォントのディスプレイリストのサイズ
	float SystemFont_posz;				//!< システムフォントのZ座標
	int now_textureid;		//!< 現在設定中のテクスチャ番号

	float camera_x;			//!< カメラ座標
	float camera_y;			//!< カメラ座標
	float camera_z;			//!< カメラ座標
	float camera_rx;		//!< カメラ回転角度
	float camera_ry;		//!< カメラ回転角度
	float viewangle;		//!< カメラの視野角

	class BlockDataInterface* blockdata;		//!< 読み込んだブロックデータを格納するクラスへのポインタ
	int bs;										//!< ブロック数
	int mapTextureID[TOTAL_BLOCKTEXTURE];		//!< テクスチャ番号

	char TextureFontFname[_MAX_PATH];	//!< テクスチャフォントのファイル名
	int TextureFont;				//!< テクスチャフォントのテクスチャID

	float HUD_myweapon_x[TOTAL_HAVEWEAPON];		//!< HUD_Aの武器描画 X座標
	float HUD_myweapon_y[TOTAL_HAVEWEAPON];		//!< HUD_Aの武器描画 Y座標
	float HUD_myweapon_z[TOTAL_HAVEWEAPON];		//!< HUD_Aの武器描画 Z座標

	jpeg_decompress_struct cinfo;	//!< libjpeg
	jpeg_error_mgr jerr;			//!< libjpeg

	int CheckFileExtension(char* filename, int nowformat);
	int CheckFileTypeFlag(char* filename, int nowformat);
	bool LoadBMPTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture);
	bool LoadDDSTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture);
	bool LoadJPEGTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture);
	bool LoadPNGTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture);
	void SetTexture(int TextureID);
	int StrMaxLineLen(char *str);
	void Start2DRender();
	void End2DRender();

#ifdef ENABLE_DEBUGCONSOLE
	unsigned char DebugFontData[96][16];	//!< デバック用フォントデータ格納配列
	int TextureDebugFont;					//!< デバック用フォントのテクスチャID
	void InitDebugFontData();
	bool LoadDebugFontTexture();
#endif

#endif	//GRAPHICS_OPENGL

public:
	D3DGraphics();
	~D3DGraphics();
	int InitD3D(WindowControl *WindowCtrl, char *TextureFontFilename, bool fullscreen);
	int ResetD3D(WindowControl *WindowCtrl);
	void DestroyD3D();
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
	void Drawline(float x1, float y1, float z1, float x2, float y2, float z2, int color);
	void Draw2DMSFontText(int x, int y, char *str, int color);
	void Draw2DMSFontTextCenter(int x, int y, int w, int h, char *str, int color);
	void Draw2DTextureFontText(int x, int y, char *str, int color, int fontwidth, int fontheight);
#ifdef ENABLE_DEBUGCONSOLE
	void Draw2DTextureDebugFontText(int x, int y, char *str, int color);
#endif
	void Draw2DLine(int x1, int y1, int x2, int y2, int color);
	void Draw2DCycle(int x, int y, int r, int color);
	void Draw2DBox(int x1, int y1, int x2, int y2, int color);
	void Draw2DTexture(int x, int y, int id, int width, int height, float alpha);
	bool SaveScreenShot(char *fname);
	int GetColorCode(float red, float green, float blue, float alpha);
};

#endif