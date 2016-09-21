//! @file d3dgraphics-opengl.cpp
//! @brief D3DGraphicsクラスの定義（OpenGL版）

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

#include "d3dgraphics.h"

#ifdef GRAPHICS_OPENGL

//! @brief コンストラクタ
D3DGraphics::D3DGraphics()
{
	hGLRC = NULL;
	width = 0;
	height = 0;
	SystemFont = NULL;
	now_SystemFontUStr = new WCHAR [1];
	now_SystemFontUStr[0] = NULL;
	SystemFontListIdx = 0;
	SystemFontListIdxSize = 0;
	SystemFont_posz = 1.0f;
	now_textureid = -1;

	camera_x = 0.0f;
	camera_y = 0.0f;
	camera_z = 0.0f;
	camera_rx = 0.0f;
	camera_ry = 0.0f;
	viewangle = 0.0f;

	blockdata = NULL;
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		mapTextureID[i] = -1;
	}

	TextureFont = -1;

#ifdef ENABLE_DEBUGCONSOLE
	TextureDebugFont = -1;
#endif
}

//! @brief ディストラクタ
D3DGraphics::~D3DGraphics()
{
	DestroyD3D();
}

//! @brief 初期化@n
//! （OpenGL 1.1）
//! @param WindowCtrl WindowControlクラスのポインタ
//! @param TextureFontFilename 使用するテクスチャフォントのファイル名
//! @param fullscreen false：ウィンドウ表示　true：フルスクリーン用表示
//! @return 成功：0　失敗：1
int D3DGraphics::InitD3D(WindowControl *WindowCtrl, char *TextureFontFilename, bool fullscreen)
{
#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_INIT, "グラフィック", "OpenGL");
#endif

	hWnd = WindowCtrl->GethWnd();

	RECT prc;
	GetClientRect(hWnd, &prc);
	width = prc.right;
	height = prc.bottom;

	//フルスクリーン化
	if( fullscreen == true ){
		DEVMODE devmode;
		ZeroMemory(&devmode, sizeof(devmode));
		devmode.dmSize = sizeof(devmode);
		devmode.dmPelsWidth = width;
		devmode.dmPelsHeight = height;
		devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

		if( ChangeDisplaySettings(&devmode, CDS_TEST) != DISP_CHANGE_SUCCESSFUL ){
			return 1;
		}
		ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);
	}



	HDC hDC;
	int pfdID;
	BOOL bResult;

	//ピクセルフォーマット
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof (PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0,
		0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,
		0,
		0
	};

	//デバイスコンテキスト取得
	hDC = GetDC(hWnd);

	//ピクセルフォーマットを取得
	pfdID = ChoosePixelFormat(hDC, &pfd);	
	if( pfdID == 0 ){ return 1; }

	//ピクセルフォーマットを指定
	bResult = SetPixelFormat(hDC, pfdID, &pfd);
	if( bResult == FALSE ){ return 1; }

	//コンテキストを指定
	hGLRC = wglCreateContext(hDC);
	if( hGLRC == NULL ){ return 1; }

	//デバイスコンテキスト解放
	ReleaseDC(hWnd, hDC);

	//システムフォント用意
	//フォント名：ＭＳ ゴシック　サイズ：18
	SystemFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック");

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//テクスチャフォント用画像のファイル名を設定
	strcpy(TextureFontFname, TextureFontFilename);

	//テクスチャフォント用画像を取得
	TextureFont = LoadTexture(TextureFontFname, true, false);


	float aspecth, prx, pry, r;
	aspecth = (float)SCREEN_WIDTH/SCREEN_HEIGHT;

	//HUD_myweapon [奥行き, 縦, 横]

	//HUD_A　現在持っている武器を描画する座標
	prx = (float)M_PI/180*-39 * aspecth /2;
	pry = (float)M_PI/180*-55 /2;
	r = 7.5f;
	HUD_myweapon_x[0] = cos(pry)*r;
	HUD_myweapon_y[0] = sin(pry)*r;
	HUD_myweapon_z[0] = sin(prx)*r;

	//HUD_A　予備の武器を描画する座標
	prx = (float)M_PI/180*-52 * aspecth /2;
	pry = (float)M_PI/180*-60 /2;
	r = 16.0f;
	HUD_myweapon_x[1] = cos(pry)*r;
	HUD_myweapon_y[1] = sin(pry)*r;
	HUD_myweapon_z[1] = sin(prx)*r;

#ifdef ENABLE_DEBUGCONSOLE
	InitDebugFontData();
	if( LoadDebugFontTexture() == false ){
		return 1;
	}
#endif


	//libjpeg初期化
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	return 0;
}

//! @brief リセット@n
//! （ウィンドウ最小化からの復帰　など）
//! @param WindowCtrl WindowControlクラスのポインタ
//! @return 成功：0　待ち：1　失敗：2
int D3DGraphics::ResetD3D(WindowControl *WindowCtrl)
{
#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_INIT, "グラフィック", "OpenGL（リセット）");
#endif

	hWnd = WindowCtrl->GethWnd();

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_ERROR, "", "");
#endif

	return 2;
}

//! @brief 解放
//! @attention 本関数は自動的に呼び出されますが、明示的に呼び出すことも可能です。
void D3DGraphics::DestroyD3D()
{
	if( hGLRC == NULL ){ return; }

	for(int i=0; i<MAX_MODEL; i++){
		CleanupModel(i);
	}
	for(int i=0; i<MAX_TEXTURE; i++){
		CleanupTexture(i);
	}

	if( SystemFont != NULL ){
		DeleteObject(SystemFont);
		SystemFont = NULL;
	}
	if( now_SystemFontUStr != NULL ){
		delete [] now_SystemFontUStr;
		now_SystemFontUStr = NULL;
	}
	if( SystemFontListIdx != 0 ){
		glDeleteLists(SystemFontListIdx, SystemFontListIdxSize);
		SystemFontListIdx = 0;
	}

	if( hGLRC != NULL ){
		wglDeleteContext(hGLRC);
		hGLRC = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "グラフィック", "OpenGL");
#endif

	//libjpeg解放
	jpeg_destroy_decompress(&cinfo);
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
	FILE *fp;
	char buf[256];
	char str[256];

	int vertexs = 0;
	MODELVDATA *vertex = NULL;
	int polygons = 0;
	int *index = NULL;
	int in_vertexs, in_polygons;
	MODELVDATA *old_vertex;
	int *old_index;

	char stroks[] = " ;,";		//区切る文字列

	//空いている認識番号を探す
	for(int i=0; i<MAX_TEXTURE; i++){
		if( pmodel[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	//ファイルを読み込む
	fp = fopen(filename, "r");
	if( fp == NULL ){
		return -1;		//ファイルが読めない
	}

	//マジックコード取得
	fgets(buf, 256, fp);
	buf[ strlen("xof 0302txt") ] = '\0';
	if( strcmp(buf, "xof 0302txt") != 0 ){
		fclose( fp );
		return -1;		//Xファイルでない
	}

	while( fgets(buf, 256, fp) != NULL ){
		strcpy(str, buf);
		str[ strlen("Mesh") ] = '\0';
		if( strcmp(str, "Mesh") == 0 ){
			
			fgets(buf, 256, fp);
			in_vertexs = atoi(buf);

			if( vertexs == 0 ){
				//1つ目のメッシュデータならば、領域を作成するだけ。
				vertex = new MODELVDATA [in_vertexs];
			}
			else{
				//2つ目の以降なら、領域を確保し直してコピーし、古い領域は削除。
				old_vertex = vertex;
				vertex = new MODELVDATA [vertexs+in_vertexs];
				memcpy(vertex, old_vertex, sizeof(MODELVDATA)*vertexs);
				delete [] old_vertex;
			}

			for(int i=0; i<in_vertexs; i++){
				fgets(buf, 256, fp);
				vertex[i+vertexs].x = (float)atof( strtok(buf, stroks) ) * -1;
				vertex[i+vertexs].y = (float)atof( strtok(NULL, stroks) );
				vertex[i+vertexs].z = (float)atof( strtok(NULL, stroks) );
			}

			fgets(buf, 256, fp);

			fgets(buf, 256, fp);
			in_polygons = atoi(buf);

			if( polygons == 0 ){
				//1つ目のインデックスデータならば、領域を作成するだけ。
				index = new int [in_polygons*5];
			}
			else{
				//2つ目の以降なら、領域を確保し直してコピーし、古い領域は削除。
				old_index = index;
				index = new int [(polygons+in_polygons)*5];
				memcpy(index, old_index, sizeof(int)*polygons*5);
				delete [] old_index;
			}

			for(int i=0; i<in_polygons; i++){
				fgets(buf, 256, fp);
				index[(i+polygons)*5] = atoi( strtok(buf, stroks) );
				for(int j=0; j<index[(i+polygons)*5]; j++){
					index[(i+polygons)*5 + j + 1] = atoi( strtok(NULL, stroks) ) + vertexs;
				}
			}

			while( fgets(buf, 256, fp) != NULL ){
				strcpy(str, buf);
				str[ strlen(" MeshTextureCoords") ] = '\0';
				if( strcmp(str, " MeshTextureCoords") == 0 ){

					fgets(buf, 256, fp);
					if( atoi(buf) != in_vertexs ){ break; }

					for(int i=0; i<in_vertexs; i++){
						fgets(buf, 256, fp);
						vertex[i+vertexs].u = (float)atof( strtok(buf, stroks) );
						vertex[i+vertexs].v = (float)atof( strtok(NULL, stroks) );
					}

					break;
				}
			}

			vertexs += in_vertexs;
			polygons += in_polygons;
		}
	}

	//ファイルハンドルを解放
	fclose( fp );

	float *VertexAry = new float [polygons*6*3];
	float *ColorAry = new float [polygons*6*4];
	float *TexCoordAry = new float [polygons*6*2];
	int vid;
	int cnt = 0;

	for(int i=0; i<polygons; i++){
		if( index[i*5] == 3 ){
			//三角形
			vid = index[i*5+1];
			VertexAry[0 + cnt*3] = vertex[vid].x;
			VertexAry[1 + cnt*3] = vertex[vid].y;
			VertexAry[2 + cnt*3] = vertex[vid].z;
			TexCoordAry[0 + cnt*2] = vertex[vid].u;
			TexCoordAry[1 + cnt*2] = vertex[vid].v;

			VertexAry[3 + cnt*3] = vertex[vid].x;
			VertexAry[4 + cnt*3] = vertex[vid].y;
			VertexAry[5 + cnt*3] = vertex[vid].z;
			TexCoordAry[2 + cnt*2] = vertex[vid].u;
			TexCoordAry[3 + cnt*2] = vertex[vid].v;

			vid = index[i*5+3];
			VertexAry[6 + cnt*3] = vertex[vid].x;
			VertexAry[7 + cnt*3] = vertex[vid].y;
			VertexAry[8 + cnt*3] = vertex[vid].z;
			TexCoordAry[4 + cnt*2] = vertex[vid].u;
			TexCoordAry[5 + cnt*2] = vertex[vid].v;

			vid = index[i*5+2];
			VertexAry[9 + cnt*3] = vertex[vid].x;
			VertexAry[10 + cnt*3] = vertex[vid].y;
			VertexAry[11 + cnt*3] = vertex[vid].z;
			TexCoordAry[6 + cnt*2] = vertex[vid].u;
			TexCoordAry[7 + cnt*2] = vertex[vid].v;

			VertexAry[12 + cnt*3] = vertex[vid].x;
			VertexAry[13 + cnt*3] = vertex[vid].y;
			VertexAry[14 + cnt*3] = vertex[vid].z;
			TexCoordAry[8 + cnt*2] = vertex[vid].u;
			TexCoordAry[9 + cnt*2] = vertex[vid].v;

			VertexAry[15 + cnt*3] = vertex[vid].x;
			VertexAry[16 + cnt*3] = vertex[vid].y;
			VertexAry[17 + cnt*3] = vertex[vid].z;
			TexCoordAry[10 + cnt*2] = vertex[vid].u;
			TexCoordAry[11 + cnt*2] = vertex[vid].v;

			cnt += 6;
		}
		else{
			//四角形
			vid = index[i*5+1];
			VertexAry[0 + cnt*3] = vertex[vid].x;
			VertexAry[1 + cnt*3] = vertex[vid].y;
			VertexAry[2 + cnt*3] = vertex[vid].z;
			TexCoordAry[0 + cnt*2] = vertex[vid].u;
			TexCoordAry[1 + cnt*2] = vertex[vid].v;

			VertexAry[3 + cnt*3] = vertex[vid].x;
			VertexAry[4 + cnt*3] = vertex[vid].y;
			VertexAry[5 + cnt*3] = vertex[vid].z;
			TexCoordAry[2 + cnt*2] = vertex[vid].u;
			TexCoordAry[3 + cnt*2] = vertex[vid].v;

			vid = index[i*5+4];
			VertexAry[6 + cnt*3] = vertex[vid].x;
			VertexAry[7 + cnt*3] = vertex[vid].y;
			VertexAry[8 + cnt*3] = vertex[vid].z;
			TexCoordAry[4 + cnt*2] = vertex[vid].u;
			TexCoordAry[5 + cnt*2] = vertex[vid].v;

			vid = index[i*5+2];
			VertexAry[9 + cnt*3] = vertex[vid].x;
			VertexAry[10 + cnt*3] = vertex[vid].y;
			VertexAry[11 + cnt*3] = vertex[vid].z;
			TexCoordAry[6 + cnt*2] = vertex[vid].u;
			TexCoordAry[7 + cnt*2] = vertex[vid].v;

			vid = index[i*5+3];
			VertexAry[12 + cnt*3] = vertex[vid].x;
			VertexAry[13 + cnt*3] = vertex[vid].y;
			VertexAry[14 + cnt*3] = vertex[vid].z;
			TexCoordAry[8 + cnt*2] = vertex[vid].u;
			TexCoordAry[9 + cnt*2] = vertex[vid].v;

			VertexAry[15 + cnt*3] = vertex[vid].x;
			VertexAry[16 + cnt*3] = vertex[vid].y;
			VertexAry[17 + cnt*3] = vertex[vid].z;
			TexCoordAry[10 + cnt*2] = vertex[vid].u;
			TexCoordAry[11 + cnt*2] = vertex[vid].v;

			cnt += 6;
		}
	}

	//色情報配列を用意
	ColorAry[0] = 1.0f;
	ColorAry[1] = 1.0f;
	ColorAry[2] = 1.0f;
	ColorAry[3] = 1.0f;
	for(int i=1; i<cnt; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(float)*4);
	}

	delete [] vertex;
	delete [] index;

	pmodel[id].useflag = true;
	pmodel[id].polygons = polygons;
	pmodel[id].VertexAry = VertexAry;
	pmodel[id].ColorAry = ColorAry;
	pmodel[id].TexCoordAry = TexCoordAry;

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

	/*
	//データが正しいか調べる
	if( (idA < 0)||((MAX_MODEL -1) < idA) ){ return -1; }
	if( pmodel[idA].useflag == false ){ return; }
	if( (idB < 0)||((MAX_MODEL -1) < idB) ){ return -1; }
	if( pmodel[idB].useflag == false ){ return; }

	int id = -1;

	//空いている認識番号を探す
	for(int i=0; i<MAX_TEXTURE; i++){
		if( pmodel[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	return -1;
	*/

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_ERROR, "", "");
#endif

	return idA;
}

//! @brief モデルファイルを解放
//! @param id モデル認識番号
void D3DGraphics::CleanupModel(int id)
{
	if( (id < 0)||((MAX_MODEL -1) < id) ){ return; }
	if( pmodel[id].useflag == false ){ return; }

	delete pmodel[id].VertexAry;
	delete pmodel[id].ColorAry;
	delete pmodel[id].TexCoordAry;
	pmodel[id].useflag = false;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "モデル", id);
#endif
}

//! @brief テクスチャフォーマットを拡張子で判定
//! @param filename ファイル名
//! @param nowformat 現在の判別値
//! @return 新たな判別値
int D3DGraphics::CheckFileExtension(char* filename, int nowformat)
{
	char filename2[MAX_PATH];

	//ファイル名を小文字へ変換（拡張子判定用）
	for(int i=0; i<strlen(filename); i++){
		filename2[i] = (char)tolower(filename[i]);
	}
	filename2[ strlen(filename) ] = '\0';

	//拡張子でファイルフォーマットを判定
	for(int i=strlen(filename2)-1; i>0; i--){
		if( filename2[i] == '.' ){
			if( strcmp(&(filename2[i]), ".bmp") == 0 ){
				return 1;
			}
			if( strcmp(&(filename2[i]), ".dds") == 0 ){
				return 2;
			}
			if( strcmp(&(filename2[i]), ".jpeg") == 0 ){
				return 3;
			}
			if( strcmp(&(filename2[i]), ".jpg") == 0 ){
				return 3;
			}
			if( strcmp(&(filename2[i]), ".jpe") == 0 ){
				return 3;
			}
			if( strcmp(&(filename2[i]), ".png") == 0 ){
				return 4;
			}
		}
	}

	return nowformat;
}

//! @brief テクスチャフォーマットをファイルヘッダーで判定
//! @param filename ファイル名
//! @param nowformat 現在の判別値
//! @return 新たな判別値
int D3DGraphics::CheckFileTypeFlag(char* filename, int nowformat)
{
	FILE *fp;
	unsigned char header[4];

	//ファイルを読み込む
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//ファイルが読めない
	}

	//ヘッダーを読む
	fread(header, 1, 4, fp);

	//ファイルハンドルを解放
	fclose( fp );

	if( (header[0x00] == 'B')&&(header[0x01] == 'M') ){															//.bmp
		return 1;
	}
	if( (header[0x00] == 'D')&&(header[0x01] == 'D')&&(header[0x02] == 'S') ){									//.dds
		return 2;
	}
	if( (header[0x00] == 0xFF)&&(header[0x01] == 0xD8) ){														//.jpg
		return 3;
	}
	if( (header[0x00] == 0x89)&&(header[0x01] == 'P')&&(header[0x02] == 'N')&&(header[0x03] == 'G') ){			//.png　※本当は8バイト
		return 4;
	}

	return nowformat;
}

//! @brief テクスチャを読み込む
//! @param filename ファイル名
//! @param texturefont テクスチャフォントフラグ
//! @param BlackTransparent 黒を透過する
//! @return 成功：テクスチャ認識番号（0以上）　失敗：-1
int D3DGraphics::LoadTexture(char* filename, bool texturefont, bool BlackTransparent)
{
	int id = -1;
	int format = 0;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "テクスチャ", filename);
#endif

	//空いている認識番号を探す
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	//まず拡張子でファイルフォーマットを判定
	format = CheckFileExtension(filename, format);

	//ファイルヘッダーの情報でも確認
	format = CheckFileTypeFlag(filename, format);

	//対応してないフォーマット
	if( format == 0 ){ return -1; }

	if( format == 1 ){	// .bmp
		if( LoadBMPTexture(filename, BlackTransparent, &(ptextures[id])) == false ){
			return -1;
		}
	}
	if( format == 2 ){	// .dds
		if( LoadDDSTexture(filename, BlackTransparent, &(ptextures[id])) == false ){
			return -1;
		}
	}
	if( format == 3 ){	// .jpeg
		if( LoadJPEGTexture(filename, BlackTransparent, &(ptextures[id])) == false ){
			return -1;
		}
	}
	if( format == 4 ){	// .png
		if( LoadPNGTexture(filename, BlackTransparent, &(ptextures[id])) == false ){
			return -1;
		}
	}


	//テクスチャ有効
	glEnable(GL_TEXTURE_2D);

	HDC hDC;
	hDC = GetDC(hWnd);
	wglMakeCurrent(hDC, hGLRC);
	glGenTextures(1, &(textureobjname[id]));
	ReleaseDC(hWnd, hDC);

	glBindTexture(GL_TEXTURE_2D, textureobjname[id]);

	//OpenGLにセット
	int width = ptextures[id].width;
	int height = ptextures[id].height;
	unsigned char *data = ptextures[id].data;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//ミップマップ設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//乗算合成
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//テクスチャ無効
	glDisable(GL_TEXTURE_2D);

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif

	return id;


	/*
	unsigned char *data = new unsigned char [16*4];

	data[0*4 + 0] = 255;	data[0*4 + 1] = 255;	data[0*4 + 2] = 255;	data[0*4 + 3] = 255;
	data[1*4 + 0] = 0;		data[1*4 + 1] = 0;		data[1*4 + 2] = 0;		data[1*4 + 3] = 255;
	data[2*4 + 0] = 255;	data[2*4 + 1] = 255;	data[2*4 + 2] = 255;	data[2*4 + 3] = 255;
	data[3*4 + 0] = 0;		data[3*4 + 1] = 0;		data[3*4 + 2] = 0;		data[3*4 + 3] = 255;
	data[4*4 + 0] = 255;	data[4*4 + 1] = 0;		data[4*4 + 2] = 0;		data[4*4 + 3] = 255;
	data[5*4 + 0] = 0;		data[5*4 + 1] = 255;	data[5*4 + 2] = 0;		data[5*4 + 3] = 255;
	data[6*4 + 0] = 0;		data[6*4 + 1] = 0;		data[6*4 + 2] = 255;	data[6*4 + 3] = 255;
	data[7*4 + 0] = 0;		data[7*4 + 1] = 0;		data[7*4 + 2] = 0;		data[7*4 + 3] = 255;
	data[8*4 + 0] = 128;	data[8*4 + 1] = 0;		data[8*4 + 2] = 0;		data[8*4 + 3] = 255;
	data[9*4 + 0] = 0;		data[9*4 + 1] = 128;	data[9*4 + 2] = 0;		data[9*4 + 3] = 255;
	data[10*4 + 0] = 0;		data[10*4 + 1] = 0;		data[10*4 + 2] = 128;	data[10*4 + 3] = 255;
	data[11*4 + 0] = 0;		data[11*4 + 1] = 0;		data[11*4 + 2] = 0;		data[11*4 + 3] = 255;
	data[12*4 + 0] = 255;	data[12*4 + 1] = 255;	data[12*4 + 2] = 0;		data[12*4 + 3] = 255;
	data[13*4 + 0] = 255;	data[13*4 + 1] = 0;		data[13*4 + 2] = 255;	data[13*4 + 3] = 255;
	data[14*4 + 0] = 0;		data[14*4 + 1] = 255;	data[14*4 + 2] = 255;	data[14*4 + 3] = 255;
	data[15*4 + 0] = 255;	data[15*4 + 1] = 255;	data[15*4 + 2] = 255;	data[15*4 + 3] = 255;

	ptextures[id].data = data;
	ptextures[id].width = 4;
	ptextures[id].height = 4;

	ptextures[id].useflag = true;

	return id;
	*/
}

//! @brief BMPファイルを読み込む
//! @param filename ファイル名
//! @param BlackTransparent 黒を透過する
//! @param ptexture 受け取るTEXTUREDATA構造体のポインタ
//! @return 成功：true　失敗：false
//! @attention 通常は LoadTexture()関数 から呼びだすこと。
bool D3DGraphics::LoadBMPTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int index;

	//ファイルを読み込む
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//ファイルが読めない
	}

	//ヘッダーを読む
	fread(header, 1, 54, fp);

	if( (header[0x00] != 'B')||(header[0x01] != 'M') ){
		fclose(fp);
		return false;		//.bmpではない
	}

	// バイト配列から整数を読み込む
	dataPos = *(int*)&(header[0x0E]) + 14;
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	index = *(int*)&(header[0x1C]);

	//実データの先頭まで移動
	fseek(fp, dataPos, SEEK_SET);

	unsigned char *data = new unsigned char [width*height*4];

	//各ピクセル4ビットなら、16色パレットモード
	if( index == 4 ){
		unsigned char pixel;
		unsigned char pixelH;
		unsigned char pixelL;
		unsigned char *pallet = new unsigned char [16*4];
		fread(pallet, 1, 16*4, fp);

		for(int h=height-1; h>=0; h--){
			for(int w=0; w<(width/2); w++){
				fread(&pixel, 1, 1, fp);
				pixelH = (pixel >> 4)&0x0F;
				pixelL = pixel&0x0F;

				data[(h*width+w*2)*4 + 0] = pallet[pixelH*4 + 2];
				data[(h*width+w*2)*4 + 1] = pallet[pixelH*4 + 1];
				data[(h*width+w*2)*4 + 2] = pallet[pixelH*4 + 0];
				data[(h*width+w*2)*4 + 3] = 255;

				data[(h*width+w*2+1)*4 + 0] = pallet[pixelL*4 + 2];
				data[(h*width+w*2+1)*4 + 1] = pallet[pixelL*4 + 1];
				data[(h*width+w*2+1)*4 + 2] = pallet[pixelL*4 + 0];
				data[(h*width+w*2+1)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//黒ならば透過する
					if( (data[(h*width+w*2)*4 + 0] == 0)&&(data[(h*width+w*2)*4 + 1] == 0)&&(data[(h*width+w*2)*4 + 2] == 0) ){
						data[(h*width+w*2)*4 + 3] = 0;
					}
					if( (data[(h*width+w*2+1)*4 + 0] == 0)&&(data[(h*width+w*2+1)*4 + 1] == 0)&&(data[(h*width+w*2+1)*4 + 2] == 0) ){
						data[(h*width+w*2+1)*4 + 3] = 0;
					}
				}
			}
		}

		delete []pallet;
	}

	//各ピクセル8ビットなら、256色パレットモード
	if( index == 8 ){
		unsigned char pixel;
		unsigned char *pallet = new unsigned char [256*4];
		fread(pallet, 1, 256*4, fp);

		for(int h=height-1; h>=0; h--){
			for(int w=0; w<width; w++){
				fread(&pixel, 1, 1, fp);

				data[(h*width+w)*4 + 0] = pallet[pixel*4 + 2];
				data[(h*width+w)*4 + 1] = pallet[pixel*4 + 1];
				data[(h*width+w)*4 + 2] = pallet[pixel*4 + 0];
				data[(h*width+w)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//黒ならば透過する
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}

		delete []pallet;
	}

	//各ピクセル24ビットなら、フルカラー
	if( index == 24 ){
		unsigned char pixel[3];
		for(int h=height-1; h>=0; h--){
			for(int w=0; w<width; w++){
				fread(&pixel, 1, 3, fp);

				data[(h*width+w)*4 + 0] = pixel[2];
				data[(h*width+w)*4 + 1] = pixel[1];
				data[(h*width+w)*4 + 2] = pixel[0];
				data[(h*width+w)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//黒ならば透過する
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}
	}

	//各ピクセル32ビットなら、フルカラー
	if( index == 32 ){
		unsigned char pixel[4];
		for(int h=height-1; h>=0; h--){
			for(int w=0; w<width; w++){
				fread(&pixel, 1, 4, fp);

				data[(h*width+w)*4 + 0] = pixel[2];
				data[(h*width+w)*4 + 1] = pixel[1];
				data[(h*width+w)*4 + 2] = pixel[0];
				data[(h*width+w)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//黒ならば透過する
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}
	}

	//ファイルハンドルを解放
	fclose( fp );

	//構造体に代入
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

//! @brief DDSファイルを読み込む
//! @param filename ファイル名
//! @param BlackTransparent 黒を透過する
//! @param ptexture 受け取るTEXTUREDATA構造体のポインタ
//! @return 成功：true　失敗：false
//! @attention 通常は LoadTexture()関数 から呼びだすこと。
bool D3DGraphics::LoadDDSTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	unsigned char header[124+4];
	unsigned int width, height;
	unsigned int index;
	unsigned int flag;

	//ファイルを読み込む
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//ファイルが読めない
	}

	//ヘッダーを読む
	fread(header, 1, 124+4, fp);

	if( (header[0x00] != 'D')||(header[0x01] != 'D')||(header[0x02] != 'S')||(header[0x03] != ' ') ){
		fclose(fp);
		return false;		//.ddsではない
	}

	// バイト配列から整数を読み込む
	width = *(int*)&(header[0x10]);
	height = *(int*)&(header[0x0C]);
	index = *(int*)&(header[0x58]);

	if( (index != 16)&&(index != 32) ){
		fclose(fp);
		return false;		//対応してないフォーマット
	}

	unsigned char *data = new unsigned char [width*height*4];

	for(int h=0; h<height; h++){
		for(int w=0; w<width; w++){
			unsigned char pixel[4];
			fread(&pixel, 1, index/8, fp);

			if( index == 16 ){		//各ピクセル16ビット
				data[(h*width+w)*4 + 0] = (pixel[1]<<4)&0xF0;
				data[(h*width+w)*4 + 1] = pixel[0]&0xF0;
				data[(h*width+w)*4 + 2] = (pixel[0]<<4)&0xF0;
				data[(h*width+w)*4 + 3] = pixel[1]&0xF0;
			}
			if( index == 32 ){		//各ピクセル32ビット
				data[(h*width+w)*4 + 0] = pixel[2];
				data[(h*width+w)*4 + 1] = pixel[1];
				data[(h*width+w)*4 + 2] = pixel[0];
				data[(h*width+w)*4 + 3] = pixel[3];
			}

			if( BlackTransparent == true ){
				//黒ならば透過する
				if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
					data[(h*width+w)*4 + 3] = 0;
				}
			}
		}
	}

	//ファイルハンドルを解放
	fclose( fp );

	//構造体に代入
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

//! @brief JPEGファイルを読み込む
//! @param filename ファイル名
//! @param BlackTransparent 黒を透過する
//! @param ptexture 受け取るTEXTUREDATA構造体のポインタ
//! @return 成功：true　失敗：false
//! @attention 通常は LoadTexture()関数 から呼びだすこと。
bool D3DGraphics::LoadJPEGTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	JSAMPARRAY img;
	unsigned int width, height;

	//ファイルを読み込む
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//ファイルが読めない
	}
	jpeg_stdio_src(&cinfo, fp);

	//パラメータの設定
	jpeg_read_header(&cinfo, true);

	//展開開始
	jpeg_start_decompress(&cinfo);

	//領域確保
	img = (JSAMPARRAY)new JSAMPROW [cinfo.output_height];
	for(int i=0; i<cinfo.output_height; i++){
		img[i] = (JSAMPROW)new JSAMPLE [cinfo.output_width * cinfo.out_color_components];
	}

	//展開
	while( cinfo.output_scanline < cinfo.output_height ) {
		jpeg_read_scanlines(&cinfo, img + cinfo.output_scanline, cinfo.output_height - cinfo.output_scanline);
	}

	//展開終了
	jpeg_finish_decompress(&cinfo);


	//ファイルハンドルを解放
	fclose( fp );


	// バイト配列から整数を読み込む
	width = (int)cinfo.output_width;
	height = (int)cinfo.output_height;

	unsigned char *data = new unsigned char [width*height*4];

	for(int h=0; h<height; h++){
		//1ライン分取得
		JSAMPROW p = img[h];

		for(int w=0; w<width; w++){
			data[(h*width+w)*4 + 0] = p[w*3 + 0];
			data[(h*width+w)*4 + 1] = p[w*3 + 1];
			data[(h*width+w)*4 + 2] = p[w*3 + 2];
			data[(h*width+w)*4 + 3] = 255;

			if( BlackTransparent == true ){
				//黒ならば透過する
				if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
					data[(h*width+w)*4 + 3] = 0;
				}
			}

		}
	}

	//領域解放
	for(int i=0; i<cinfo.output_height; i++){
		delete [] img[i];
	}
	delete [] img;

	//構造体に代入
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

//! @brief PNGファイルを読み込む
//! @param filename ファイル名
//! @param BlackTransparent 黒を透過する
//! @param ptexture 受け取るTEXTUREDATA構造体のポインタ
//! @return 成功：true　失敗：false
//! @attention 通常は LoadTexture()関数 から呼びだすこと。
bool D3DGraphics::LoadPNGTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	png_byte sig[4];
	png_structp pPng;
    png_infop pInfo;
	unsigned int width, height;
	bool pallet;

	//ファイルを読み込む
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//ファイルが読めない
	}

	//PNGファイルか判定
	fread(sig, 4, 1, fp);
	if( png_sig_cmp(sig, 0, 4) != 0 ){
		fclose(fp);
		return false;	//.pngではない
	}

	//構造体を初期化
	pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    pInfo = png_create_info_struct(pPng);

	//情報を取得
	png_init_io(pPng, fp);
	png_set_sig_bytes(pPng, 4);
	png_read_info(pPng, pInfo);

	//テクスチャの大きさを取得
	width = png_get_image_width(pPng, pInfo);
	height =  png_get_image_height(pPng, pInfo);

	//インターレス方式か判定
	if( png_get_interlace_type(pPng, pInfo) != PNG_INTERLACE_NONE ){
		png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
		fclose(fp);
		return false;	//インターレスには対応しない。
	}

	//ビット深度判定
	if( png_get_bit_depth(pPng, pInfo) != 8 ){
		png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
		fclose(fp);
		return false;	//深度が8ビット以外は対応してない。
	}

	//カラータイプ判定
	if( (png_get_color_type(pPng, pInfo) == PNG_COLOR_TYPE_GRAY)||(png_get_color_type(pPng, pInfo) == PNG_COLOR_TYPE_GRAY_ALPHA) ){
		png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
		fclose(fp);
		return false;	//グレースケールには対応してない・・っと思われるので除外。（未検証）
	}

	//カラータイプ取得
	if( png_get_color_type(pPng, pInfo) == PNG_COLOR_TYPE_PALETTE ){
		pallet = true;
	}
	else{
		pallet = false;
	}

	unsigned char *data = new unsigned char [width*height*4];

	//アルファチャンネルを初期化
	png_set_add_alpha(pPng, 0xff, PNG_FILLER_AFTER);

	if( pallet == false ){
		// tRNSチャンクがあれば、アルファチャンネルに変換
		if( png_get_valid(pPng, pInfo, PNG_INFO_tRNS) ){
			png_set_tRNS_to_alpha(pPng);
		}

		//1ライン分の作業領域を確保
		png_bytep buf = new png_byte[width*4];

		for(int h=0; h<height; h++){
			//1ライン分取得
			png_read_row(pPng, buf, NULL);

			for(int w=0; w<width; w++){
				data[(h*width+w)*4 + 0] = buf[w*4 + 0];
				data[(h*width+w)*4 + 1] = buf[w*4 + 1];
				data[(h*width+w)*4 + 2] = buf[w*4 + 2];
				data[(h*width+w)*4 + 3] = buf[w*4 + 3];

				if( BlackTransparent == true ){
					//黒ならば透過する
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}

		//1ライン分の作業領域を解放
		delete [] buf;
	}
	else{
		png_colorp palette;
		int num;

		//パレット取得
		png_get_PLTE(pPng, pInfo, &palette, &num);

		//1ライン分の作業領域を確保
		png_bytep buf = new png_byte[width];

		for(int h=0; h<height; h++){
			//1ライン分取得
			png_read_row(pPng, buf, NULL);

			for(int w=0; w<width; w++){
				data[(h*width+w)*4 + 0] = palette[ buf[w] ].red;
				data[(h*width+w)*4 + 1] = palette[ buf[w] ].green;
				data[(h*width+w)*4 + 2] = palette[ buf[w] ].blue;
				data[(h*width+w)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//黒ならば透過する
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}

		//1ライン分の作業領域を解放
		delete [] buf;
	}

	//解放
	png_read_end(pPng, NULL);
	png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);

	//ファイルハンドルを解放
	fclose( fp );

	//構造体に代入
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

#ifdef ENABLE_DEBUGCONSOLE
//! @brief デバック用フォントを読み込む
//! @return 成功：true　失敗：false
//! @attention この関数を呼び出す前に、InitDebugFontData()関数を実行してください。
bool D3DGraphics::LoadDebugFontTexture()
{
	int charwidth = 8;
	int charheight = 16;
	int width = charwidth * 16;
	int height = charheight * 8;

	int datacnt = 0;
	int id = -1;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "テクスチャ", "DebugFontTexture");
#endif

	//既に読み込まれているなら失敗
	if( TextureDebugFont != -1 ){ return false; }

	//空いている認識番号を探す
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return false; }

	unsigned char *data = new unsigned char [width*height*4];

	//制御コードが入っている2行 32文字分は、空欄にする。
	for(int cnt_y=0; cnt_y<2; cnt_y++){
		for(int line_y=0; line_y<charheight; line_y++){
			for(int cnt_x=0; cnt_x<16; cnt_x++){
				for(int line_x=0; line_x<charwidth; line_x++){
					data[datacnt + 0] = 0;
					data[datacnt + 1] = 0;
					data[datacnt + 2] = 0;
					data[datacnt + 3] = 0;

					datacnt += 4;
				}
			}
		}
	}

	//6行分のデータを作成
	for(int cnt_y=0; cnt_y<6; cnt_y++){
		for(int line_y=0; line_y<charheight; line_y++){
			for(int cnt_x=0; cnt_x<16; cnt_x++){
				for(int line_x=(charwidth-1); line_x>=0; line_x--){
					unsigned char mask;

					//ビット判定用マスク作成
					switch(line_x){
						case 0: mask = 0x01; break;
						case 1: mask = 0x02; break;
						case 2: mask = 0x04; break;
						case 3: mask = 0x08; break;
						case 4: mask = 0x10; break;
						case 5: mask = 0x20; break;
						case 6: mask = 0x40; break;
						case 7: mask = 0x80; break;
						default: mask = 0x00;		//事実上エラー
					}

					//該当ビットが1なら白、0なら黒。
					if( (DebugFontData[cnt_y*16 + cnt_x][line_y] & mask) != 0 ){
						data[datacnt + 0] = 255;
						data[datacnt + 1] = 255;
						data[datacnt + 2] = 255;
						data[datacnt + 3] = 255;
					}
					else{
						data[datacnt + 0] = 0;
						data[datacnt + 1] = 0;
						data[datacnt + 2] = 0;
						data[datacnt + 3] = 0;
					}

					datacnt += 4;
				}
			}
		}
	}

	//テクスチャ有効
	glEnable(GL_TEXTURE_2D);

	HDC hDC;
	hDC = GetDC(hWnd);
	wglMakeCurrent(hDC, hGLRC);
	glGenTextures(1, &(textureobjname[id]));
	ReleaseDC(hWnd, hDC);

	glBindTexture(GL_TEXTURE_2D, textureobjname[id]);

	//OpenGLにセット
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//ミップマップ設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//乗算合成
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//テクスチャ無効
	glDisable(GL_TEXTURE_2D);


	//構造体に代入
	ptextures[id].data = data;
	ptextures[id].width = width;
	ptextures[id].height = height;

	ptextures[id].useflag = true;


#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif

	//テクスチャID設定
	TextureDebugFont = id;
	return true;
}
#endif

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
	if( ptextures[id].useflag == false ){ return 1; }

	*width = ptextures[id].width;
	*height = ptextures[id].height;

	return 0;
}

//! @brief テクスチャを指定
//! @param TextureID テクスチャ認識番号
void D3DGraphics::SetTexture(int TextureID)
{
	if( now_textureid == TextureID ){
		return;
	}

	//OpenGLにセット
	glBindTexture(GL_TEXTURE_2D, textureobjname[TextureID]);

	now_textureid = TextureID;
}

//! @brief テクスチャを解放
//! @param id テクスチャ認識番号
void D3DGraphics::CleanupTexture(int id)
{
	if( (id < 0)||((MAX_TEXTURE -1) < id) ){ return; }
	if( ptextures[id].useflag == false ){ return; }

	delete ptextures[id].data;
	glDeleteTextures(1, &(textureobjname[id]));
	ptextures[id].useflag = false;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "テクスチャ", id);
#endif
}

//! @brief 全ての描画処理を開始
//! @return 成功：0　失敗：1
//! @attention 描画処理の最初に呼び出す必要があります。
int D3DGraphics::StartRender()
{
	HDC hDC;

	hDC = BeginPaint(hWnd, &Paint_ps);

	//コンテキストを指定
	wglMakeCurrent(hDC, hGLRC);

	//初期化
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//混合処理（透過有効化）
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//アルファテスト
	glAlphaFunc(GL_GREATER, 0.0f);
	glEnable(GL_ALPHA_TEST);

	//2DシステムフォントZ座標初期化
	SystemFont_posz = 1.0f;

	return 0;
}

//! @brief 全ての描画処理を終了
//! @return 成功：false　失敗：true
//! @attention 描画処理の最後に呼び出す必要があります。
bool D3DGraphics::EndRender()
{
	glFlush();

	wglMakeCurrent(NULL, NULL);

	EndPaint(hWnd, &Paint_ps);

	return false;
}

//! @brief Zバッファをリセット
void D3DGraphics::ResetZbuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

//! @brief ワールド空間を原点（0,0,0）に戻す　など
void D3DGraphics::ResetWorldTransform()
{
	float camera_y_flag;

	if( fabs(camera_ry) <= (float)M_PI/2 ){
		camera_y_flag = 1.0f;
	}
	else{
		camera_y_flag = -1.0f;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewangle*(180.0f/(float)M_PI), (float)width/height, CLIPPINGPLANE_NEAR, CLIPPINGPLANE_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x*-1, camera_y, camera_z, camera_x*-1 + cos(camera_rx*-1 + (float)M_PI)*cos(camera_ry), camera_y + sin(camera_ry), camera_z + sin(camera_rx*-1 + (float)M_PI)*cos(camera_ry), 0.0f, camera_y_flag, 0.0f);
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
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry1*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glRotatef(ry2*-1*(180.0f/(float)M_PI), 0.0f, 0.0f, 1.0f);
	glScalef(size, size, size);
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
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry*-1*(180.0f/(float)M_PI), 0.0f, 0.0f, 1.0f);
	glRotatef(rt*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glScalef(size, size, size);
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
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glTranslatef(mx*-1, my, mz);
	glScalef(size, size, size);
}

//! @brief ワールド空間を所持している武器を描画する場所に設定
//! @param rotation 武器を回転させる
//! @param camera_x カメラのX座標
//! @param camera_y カメラのY座標
//! @param camera_z カメラのZ座標
//! @param camera_rx カメラの横軸角度
//! @param camera_ry カメラの縦軸角度
//! @param rx 武器のの縦軸角度
//! @param size 描画サイズ
//! @note rotation・・　true：現在持っている武器です。　false：予備の武器です。（rx は無視されます）
//! @todo 位置やサイズの微調整
void D3DGraphics::SetWorldTransformPlayerWeapon(bool rotation, float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float rx, float size)
{
	size = size * 0.3f;

	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(camera_x*-1, camera_y, camera_z);
	glRotatef(camera_rx*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(camera_ry*-1*(180.0f/(float)M_PI), 0.0f, 0.0f, 1.0f);

	if( rotation == true ){
		glTranslatef(HUD_myweapon_x[0]*-1, HUD_myweapon_y[0], HUD_myweapon_z[0]);
		glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	}
	else{
		glTranslatef(HUD_myweapon_x[1]*-1, HUD_myweapon_y[1], HUD_myweapon_z[1]);
		glRotatef(180, 0.0f, 1.0f, 0.0f);
	}
	glScalef(size, size, size);
}

//! @brief ワールド空間の座標を取得
//! @param *x x軸を受け取るポインタ
//! @param *y y軸を受け取るポインタ
//! @param *z z軸を受け取るポインタ
void D3DGraphics::GetWorldTransformPos(float *x, float *y, float *z)
{
	GLfloat model[16];
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, model);
	*x = model[12];
	*y = model[13];
	*z = model[14];
}

//! @brief フォグを設定
//! @param skynumber 空の番号
void D3DGraphics::SetFog(int skynumber)
{
	float fogColor[4];

	//空の番号により色を決定
	switch(skynumber){
		case 1:		fogColor[0] = 0.25f;	fogColor[1] = 0.25f+0.0625;	fogColor[2] = 0.25f;	fogColor[3] = 1.0f;		break;
		case 2:		fogColor[0] = 0.0625;	fogColor[1] = 0.0625;		fogColor[2] = 0.0625;	fogColor[3] = 1.0f;		break;
		case 3:		fogColor[0] = 0.0f;		fogColor[1] = 0.0625;		fogColor[2] = 0.125;	fogColor[3] = 1.0f;		break;
		case 4:		fogColor[0] = 0.125;	fogColor[1] = 0.0625;		fogColor[2] = 0.0625;	fogColor[3] = 1.0f;		break;
		case 5:		fogColor[0] = 0.25f;	fogColor[1] = 0.125;		fogColor[2] = 0.125;	fogColor[3] = 1.0f;		break;
		default:	fogColor[0] = 0.0f;		fogColor[1] = 0.0f;			fogColor[2] = 0.0f;		fogColor[3] = 1.0f;		break;
	}

	float fog_st = 100;
	float fog_end = 800;
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_START, fog_st);
	glFogf(GL_FOG_END, fog_end);

	glEnable(GL_FOG);
}

//! @brief カメラ（視点）を設定
//! @param in_camera_x カメラのX座標
//! @param in_camera_y カメラのY座標
//! @param in_camera_z カメラのZ座標
//! @param in_camera_rx カメラの横軸角度
//! @param in_camera_ry カメラの縦軸角度（-π～π以内）
//! @param in_viewangle 視野角
void D3DGraphics::SetCamera(float in_camera_x, float in_camera_y, float in_camera_z, float in_camera_rx, float in_camera_ry, float in_viewangle)
{
	glViewport(0, 0, width, height);
	
	camera_x = in_camera_x;
	camera_y = in_camera_y;
	camera_z = in_camera_z;
	camera_rx = in_camera_rx;
	camera_ry = in_camera_ry;
	viewangle = in_viewangle;

	ResetWorldTransform();
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

	//クラスを設定
	blockdata = in_blockdata;

	//ブロック数を取得
	bs = blockdata->GetTotaldatas();

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
}

//! @brief マップデータを描画
//! @param wireframe ワイヤーフレーム表示
void D3DGraphics::DrawMapdata(bool wireframe)
{
	//ブロックデータが読み込まれていなければ、処理しない。
	if( blockdata == NULL ){ return; }

	struct blockdata data;
	int textureID;
	int vID[4];
	int uvID[4];
	float *VertexAry = new float [bs*6 * 6*3];
	float *ColorAry = new float [bs*6 * 6*4];
	float *TexCoordAry = new float [bs*6 * 6*2];

	if( wireframe == true ){
		//ワイヤーフレーム表示
		for(int i=0; i<bs; i++){
			blockdata->Getdata(&data, i);
			Drawline(data.x[0], data.y[0], data.z[0], data.x[1], data.y[1], data.z[1], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[1], data.y[1], data.z[1], data.x[2], data.y[2], data.z[2], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[2], data.y[2], data.z[2], data.x[3], data.y[3], data.z[3], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[3], data.y[3], data.z[3], data.x[0], data.y[0], data.z[0], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[4], data.y[4], data.z[4], data.x[5], data.y[5], data.z[5], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[5], data.y[5], data.z[5], data.x[6], data.y[6], data.z[6], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[6], data.y[6], data.z[6], data.x[7], data.y[7], data.z[7], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[7], data.y[7], data.z[7], data.x[4], data.y[4], data.z[4], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[0], data.y[0], data.z[0], data.x[4], data.y[4], data.z[4], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[1], data.y[1], data.z[1], data.x[5], data.y[5], data.z[5], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[2], data.y[2], data.z[2], data.x[6], data.y[6], data.z[6], GetColorCode(0.0f,1.0f,0.0f,1.0f));
			Drawline(data.x[3], data.y[3], data.z[3], data.x[7], data.y[7], data.z[7], GetColorCode(0.0f,1.0f,0.0f,1.0f));
		}
		return;
	}

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		int cnt = 0;

		//テクスチャが正常に読み込めていなければ設定
		if( mapTextureID[textureID] == -1 ){
			//テクスチャ無効
			glDisable(GL_TEXTURE_2D);
		}
		else if( ptextures[ mapTextureID[textureID] ].useflag == false ){
			//テクスチャ無効
			glDisable(GL_TEXTURE_2D);
		}
		else{
			//テクスチャ有効
			glEnable(GL_TEXTURE_2D);

			//テクスチャをセット
			SetTexture(mapTextureID[textureID]);
		}

		for(int i=0; i<bs; i++){
			//データ取得
			blockdata->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//テクスチャ認識番号を取得
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//面の頂点データの関連付けを取得
					blockdataface(j, &vID[0], &uvID[0]);

					//頂点配列を用意
					VertexAry[0 + cnt*18] = data.x[ vID[1] ]*-1;		VertexAry[1 + cnt*18] = data.y[ vID[1] ];	VertexAry[2 + cnt*18] = data.z[ vID[1] ];
					VertexAry[3 + cnt*18] = data.x[ vID[1] ]*-1;		VertexAry[4 + cnt*18] = data.y[ vID[1] ];	VertexAry[5 + cnt*18] = data.z[ vID[1] ];
					VertexAry[6 + cnt*18] = data.x[ vID[0] ]*-1;		VertexAry[7 + cnt*18] = data.y[ vID[0] ];	VertexAry[8 + cnt*18] = data.z[ vID[0] ];
					VertexAry[9 + cnt*18] = data.x[ vID[2] ]*-1;		VertexAry[10 + cnt*18] = data.y[ vID[2] ];	VertexAry[11 + cnt*18] = data.z[ vID[2] ];
					VertexAry[12 + cnt*18] = data.x[ vID[3] ]*-1;		VertexAry[13 + cnt*18] = data.y[ vID[3] ];	VertexAry[14 + cnt*18] = data.z[ vID[3] ];
					VertexAry[15 + cnt*18] = data.x[ vID[3] ]*-1;		VertexAry[16 + cnt*18] = data.y[ vID[3] ];	VertexAry[17 + cnt*18] = data.z[ vID[3] ];

					//色情報配列を用意
					ColorAry[0 + cnt*24] = data.material[j].shadow;
					ColorAry[1 + cnt*24] = data.material[j].shadow;
					ColorAry[2 + cnt*24] = data.material[j].shadow;
					ColorAry[3 + cnt*24] = 1.0f;
					for(int k=1; k<6; k++){
						memcpy(&(ColorAry[k*4 + cnt*24]), &(ColorAry[cnt*24]), sizeof(float)*4);
					}

					//UV座標配列を用意
					TexCoordAry[0 + cnt*12] = data.material[j].u[ uvID[1] ];	TexCoordAry[1 + cnt*12] = data.material[j].v[ uvID[1] ];
					TexCoordAry[2 + cnt*12] = data.material[j].u[ uvID[1] ];	TexCoordAry[3 + cnt*12] = data.material[j].v[ uvID[1] ];
					TexCoordAry[4 + cnt*12] = data.material[j].u[ uvID[0] ];	TexCoordAry[5 + cnt*12] = data.material[j].v[ uvID[0] ];
					TexCoordAry[6 + cnt*12] = data.material[j].u[ uvID[2] ];	TexCoordAry[7 + cnt*12] = data.material[j].v[ uvID[2] ];
					TexCoordAry[8 + cnt*12] = data.material[j].u[ uvID[3] ];	TexCoordAry[9 + cnt*12] = data.material[j].v[ uvID[3] ];
					TexCoordAry[10 + cnt*12] = data.material[j].u[ uvID[3] ];	TexCoordAry[11 + cnt*12] = data.material[j].v[ uvID[3] ];

					cnt += 1;
				}
			}
		}

		//描画
		glVertexPointer(3, GL_FLOAT, 0, VertexAry);
		glColorPointer(4, GL_FLOAT, 0, ColorAry);
		glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
		glDrawArrays(GL_TRIANGLE_STRIP, 1, 6*cnt-2);
	}

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	delete [] VertexAry;
	delete [] ColorAry;
	delete [] TexCoordAry;
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

	bs = 0;

	blockdata = NULL;
}

//! @brief モデルファイルを描画
//! @param id_model モデル認識番号
//! @param id_texture テクスチャ認識番号
void D3DGraphics::RenderModel(int id_model, int id_texture)
{
	//無効な引数が設定されていれば失敗
	if( id_model == -1 ){ return; }
	if( pmodel[id_model].useflag == false ){ return; }

	//テクスチャが正常に読み込めていなければ設定
	if( id_texture == -1 ){
		//テクスチャ無効
		glDisable(GL_TEXTURE_2D);
	}
	else if( ptextures[id_texture].useflag == false ){
		//テクスチャ無効
		glDisable(GL_TEXTURE_2D);
	}
	else{
		//テクスチャ有効
		glEnable(GL_TEXTURE_2D);

		//テクスチャをセット
		SetTexture(id_texture);
	}

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//描画
	glVertexPointer(3, GL_FLOAT, 0, pmodel[id_model].VertexAry);
	glColorPointer(4, GL_FLOAT, 0, pmodel[id_model].ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, pmodel[id_model].TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, pmodel[id_model].polygons*6-2);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	/*
	Drawline(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	Drawline(0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f);
	Drawline(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f);
	*/
}

//! @brief 板を描画
//! @param id_texture テクスチャ認識番号
//! @param alpha 透明度　（0.0～1.0　0.0：完全透明）
void D3DGraphics::RenderBoard(int id_texture, float alpha)
{
	//テクスチャが設定されていなければ、処理しない。
	if( id_texture == -1 ){ return; }
	if( ptextures[id_texture].useflag == false ){ return; }

	float VertexAry[4*3];
	float ColorAry[4*4];
	float TexCoordAry[4*2];

	//テクスチャ有効
	glEnable(GL_TEXTURE_2D);

	//テクスチャをセット
	SetTexture(id_texture);

	//頂点配列を用意
	VertexAry[0] = 0.0f;	VertexAry[1] = 0.5f;	VertexAry[2] = 0.5f;
	VertexAry[3] = 0.0f;	VertexAry[4] = -0.5f;	VertexAry[5] = 0.5f;
	VertexAry[6] = 0.0f;	VertexAry[7] = 0.5f;	VertexAry[8] = -0.5f;
	VertexAry[9] = 0.0f;	VertexAry[10] = -0.5f;	VertexAry[11] = -0.5f;

	//色情報配列を用意
	ColorAry[0] = 1.0f;
	ColorAry[1] = 1.0f;
	ColorAry[2] = 1.0f;
	ColorAry[3] = alpha;
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(float)*4);
	}

	//UV座標配列を用意
	TexCoordAry[0] = 1.0f;	TexCoordAry[1] = 0.0f;
	TexCoordAry[2] = 1.0f;	TexCoordAry[3] = 1.0f;
	TexCoordAry[4] = 0.0f;	TexCoordAry[5] = 0.0f;
	TexCoordAry[6] = 0.0f;	TexCoordAry[7] = 1.0f;

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//描画
	glVertexPointer(3, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_FLOAT, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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
	Draw2DBox(0, 0, Width, Height, GetColorCode(1.0f,1.0f,1.0f,alpha));
}

//! @brief 【デバック用】中心線表示
void D3DGraphics::Centerline()
{
	ResetWorldTransform();
	Drawline(100.0f, 0.0f, 0.0f, -100.0f, 0.0f, 0.0f, GetColorCode(1.0f,0.0f,0.0f,1.0f));
	Drawline(0.0f, 100.0f, 0.0f, 0.0f, -100.0f, 0.0f, GetColorCode(0.0f,1.0f,0.0f,1.0f));
	Drawline(0.0f, 0.0f, 100.0f, 0.0f, 0.0f, -100.0f, GetColorCode(0.0f,0.0f,1.0f,1.0f));
}

//! @brief 【デバック用】線表示
void D3DGraphics::Drawline(float x1, float y1, float z1, float x2, float y2, float z2, int color)
{
	float VertexAry[2*3];
	unsigned char ColorAry[2*4];

	//テクスチャ無効
	glDisable(GL_TEXTURE_2D);

	//頂点配列を用意
	VertexAry[0] = (float)x1*-1;	VertexAry[1] = (float)y1;	VertexAry[2] = (float)z1;
	VertexAry[3] = (float)x2*-1;	VertexAry[4] = (float)y2;	VertexAry[5] = (float)z2;

	//色情報配列を用意
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	memcpy(&(ColorAry[4]), ColorAry, sizeof(unsigned char)*4);

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//表示
	glVertexPointer(3, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

//! @brief 最も長い行の文字数を取得
//! @param str 文字列　（改行コード：可）
//! @return 文字数
//! @attention マルチバイト文字は 1文字あたり 2 としてカウントされます。
int D3DGraphics::StrMaxLineLen(char *str)
{
	int maxlen = 0;
	int cnt = 0;

	for(int i=0; i<strlen(str); i++){
		if( str[i] == '\n' ){
			if( maxlen < cnt ){
				maxlen = cnt;
			}
			cnt = 0;
		}
		else{
			cnt += 1;
		}
	}

	if( maxlen < cnt ){
		maxlen = cnt;
	}

	return maxlen;
}

//! @brief 文字を表示（システムフォント使用）
//! @param x x座標
//! @param y y座標
//! @param str 文字列　（改行コード：可）
//! @param color 色
//! @warning 本関数は1フレーム間で100回までしか呼び出せません。（OpenGLコアのみ）
//! @warning <b>表示は非常に低速です。</b>画面内で何度も呼び出すとパフォーマンスに影響します。
//! @warning「改行コードを活用し一度に表示する」「日本語が必要ない文字はテクスチャフォントを活用する」などの対応を講じてください。
//! @attention フォントの種類やサイズは固定です。　文字を二重に重ねて立体感を出さないと見にくくなります。
//! @todo 1文字目が欠ける場合がある。
void D3DGraphics::Draw2DMSFontText(int x, int y, char *str, int color)
{
	int len = strlen(str);
	WCHAR *ustr;

	y += 18;

	Start2DRender();
	glEnable(GL_DEPTH_TEST);

	//テクスチャ無効
	glDisable(GL_TEXTURE_2D);

	//Unicode文字列へ変換
	ustr = new WCHAR [len+1];
	MultiByteToWideChar(CP_ACP,	0, str, -1, ustr, len + 1);

	//新たな文字列なら、リソースを作り直す
	if( lstrcmpW(ustr, now_SystemFontUStr) != 0 ){
		GLuint listIdx;
		HDC hDC;

		//古いデータを削除
		glDeleteLists(SystemFontListIdx, SystemFontListIdxSize);
		delete [] now_SystemFontUStr;

		//デバイスコンテキスト設定
		hDC = GetDC(hWnd);
		wglMakeCurrent(hDC, hGLRC);
		SelectObject(hDC, SystemFont);

		//ディスプレイリストを作成
		listIdx = glGenLists(len);
		wglUseFontBitmapsW(hDC, ustr[0], 1, listIdx);	//ダミー
		for(int i=0; i<lstrlenW(ustr); i++){
			wglUseFontBitmapsW(hDC, ustr[i], 1, listIdx+i);
		}

		//デバイスコンテキスト廃棄
		ReleaseDC(hWnd, hDC);

		//設定を記録
		now_SystemFontUStr = new WCHAR [len+1];
		lstrcpyW(now_SystemFontUStr, ustr);
		SystemFontListIdx = listIdx;
		SystemFontListIdxSize = len;
	}

	//座標と色を設定
	glBitmap(0, 0, 0, 0, 10, 0, NULL);
	glRasterPos3f((float)x, (float)y, SystemFont_posz);
	glColor4ub((color>>24)&0xFF, (color>>16)&0xFF, (color>>8)&0xFF, color&0xFF);

	for(int i=0; i<lstrlenW(ustr); i++){
		if( ustr[i] == '\n' ){
			//改行する
			y += 19;
			glRasterPos3f((float)x, (float)y, SystemFont_posz);
		}
		else{
			//ディスプレイリスト描画
			glCallList(SystemFontListIdx + i);
		}
	}

	SystemFont_posz -= 0.01f;

	//Unicode文字列の廃棄
	delete [] ustr;

	//glDisable(GL_DEPTH_TEST);
	End2DRender();
}

//! @brief 文字を中央揃えで表示（システムフォント使用）
//! @param x x座標
//! @param y y座標
//! @param w 横の大きさ
//! @param h 縦の大きさ
//! @param str 文字列　（改行コード：可）
//! @param color 色
void D3DGraphics::Draw2DMSFontTextCenter(int x, int y, int w, int h, char *str, int color)
{
	Draw2DMSFontText(x + (SCREEN_WIDTH/2 - (StrMaxLineLen(str)*9/2)), y, str, color);
}

//! @brief 2D描画用設定
void D3DGraphics::Start2DRender()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
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

	int strlens = (int)strlen(str);

	float *VertexAry = new float [strlens*6*2];
	unsigned char *ColorAry = new unsigned char [strlens*6*4];
	float *TexCoordAry = new float [strlens*6*2];

	//2D描画用設定を適用
	Start2DRender();

	int w;
	float font_u, font_v;
	float t_u, t_v;

	//1文字のUV座標を計算
	font_u = 1.0f / 16;
	font_v = 1.0f / 16;

	//テクスチャ有効
	glEnable(GL_TEXTURE_2D);

	//テクスチャをセット
	SetTexture(TextureFont);

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//色情報配列を用意
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	for(int i=1; i<strlens*6; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	// 与えられた文字数分ループ
	for(int i=0; i<strlens; i++){
		//UV座標を計算
		w = str[i];
		if( w < 0 ){ w += 256; }
		t_u = (w % 16) * font_u;
		t_v = (w / 16) * font_v;

		VertexAry[0 + i*12] = (float)x + i*fontwidth;			VertexAry[1 + i*12] = (float)y;
		VertexAry[2 + i*12] = (float)x + i*fontwidth;			VertexAry[3 + i*12] = (float)y;
		VertexAry[4 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[5 + i*12] = (float)y;
		VertexAry[6 + i*12] = (float)x + i*fontwidth;			VertexAry[7 + i*12] = (float)y + fontheight;
		VertexAry[8 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[9 + i*12] = (float)y + fontheight;
		VertexAry[10 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[11 + i*12] = (float)y + fontheight;
		TexCoordAry[0 + i*12] = t_u;		TexCoordAry[1 + i*12] = t_v;
		TexCoordAry[2 + i*12] = t_u;		TexCoordAry[3 + i*12] = t_v;
		TexCoordAry[4 + i*12] = t_u + font_u;	TexCoordAry[5 + i*12] = t_v;
		TexCoordAry[6 + i*12] = t_u;		TexCoordAry[7 + i*12] = t_v + font_v;
		TexCoordAry[8 + i*12] = t_u + font_u;	TexCoordAry[9 + i*12] = t_v + font_v;
		TexCoordAry[10 + i*12] = t_u + font_u;	TexCoordAry[11 + i*12] = t_v + font_v;
	}

	//描画
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, strlens*6-2);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//2D描画用設定を解除
	End2DRender();

	delete [] VertexAry;
	delete [] ColorAry;
	delete [] TexCoordAry;
}

#ifdef ENABLE_DEBUGCONSOLE
//! @brief 文字を表示（デバック用フォント使用）
//! @param x x座標
//! @param y y座標
//! @param str 文字列　（改行コード：<b>不可</b>）
//! @param color 色
//! @attention 一文字の幅および高さは 8x16 固定です。
//! @attention 文字を二重に重ねて立体感を出さないと見にくくなります。
void D3DGraphics::Draw2DTextureDebugFontText(int x, int y, char *str, int color)
{
	int fontwidth = 8;
	int fontheight = 16;

	//テクスチャフォントの取得に失敗していれば、処理しない
	if( TextureDebugFont == -1 ){ return; }

	int strlens = (int)strlen(str);

	float *VertexAry = new float [strlens*6*2];
	unsigned char *ColorAry = new unsigned char [strlens*6*4];
	float *TexCoordAry = new float [strlens*6*2];

	//2D描画用設定を適用
	Start2DRender();

	int w;
	float font_u, font_v;
	float t_u, t_v;

	//1文字のUV座標を計算
	font_u = 1.0f / 16;
	font_v = 1.0f / 8;

	//テクスチャ有効
	glEnable(GL_TEXTURE_2D);

	//テクスチャをセット
	SetTexture(TextureDebugFont);

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//色情報配列を用意
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	for(int i=1; i<strlens*6; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	// 与えられた文字数分ループ
	for(int i=0; i<strlens; i++){
		//UV座標を計算
		w = str[i];
		if( w < 0 ){ w = ' '; }
		t_u = (w % 16) * font_u;
		t_v = (w / 16) * font_v;

		VertexAry[0 + i*12] = (float)x + i*fontwidth;			VertexAry[1 + i*12] = (float)y;
		VertexAry[2 + i*12] = (float)x + i*fontwidth;			VertexAry[3 + i*12] = (float)y;
		VertexAry[4 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[5 + i*12] = (float)y;
		VertexAry[6 + i*12] = (float)x + i*fontwidth;			VertexAry[7 + i*12] = (float)y + fontheight;
		VertexAry[8 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[9 + i*12] = (float)y + fontheight;
		VertexAry[10 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[11 + i*12] = (float)y + fontheight;
		TexCoordAry[0 + i*12] = t_u;		TexCoordAry[1 + i*12] = t_v;
		TexCoordAry[2 + i*12] = t_u;		TexCoordAry[3 + i*12] = t_v;
		TexCoordAry[4 + i*12] = t_u + font_u;	TexCoordAry[5 + i*12] = t_v;
		TexCoordAry[6 + i*12] = t_u;		TexCoordAry[7 + i*12] = t_v + font_v;
		TexCoordAry[8 + i*12] = t_u + font_u;	TexCoordAry[9 + i*12] = t_v + font_v;
		TexCoordAry[10 + i*12] = t_u + font_u;	TexCoordAry[11 + i*12] = t_v + font_v;
	}

	//描画
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, strlens*6-2);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//2D描画用設定を解除
	End2DRender();

	delete [] VertexAry;
	delete [] ColorAry;
	delete [] TexCoordAry;
}
#endif

//! @brief 線を描画
//! @param x1 始点の x座標
//! @param y1 始点の y座標
//! @param x2 終点の x座標
//! @param y2 終点の y座標
//! @param color 色
void D3DGraphics::Draw2DLine(int x1, int y1, int x2, int y2, int color)
{
	float VertexAry[2*2];
	unsigned char ColorAry[2*4];

	//2D描画用設定を適用
	Start2DRender();

	//テクスチャ無効
	glDisable(GL_TEXTURE_2D);

	//頂点配列を用意
	VertexAry[0] = (float)x1;	VertexAry[1] = (float)y1;
	VertexAry[2] = (float)x2;	VertexAry[3] = (float)y2;

	//色情報配列を用意
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	memcpy(&(ColorAry[4]), ColorAry, sizeof(unsigned char)*4);

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//描画
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

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
	float VertexAry[(16+1)*2];
	unsigned char ColorAry[(16+1)*4];

	//2D描画用設定を適用
	Start2DRender();

	//テクスチャ無効
	glDisable(GL_TEXTURE_2D);

	//頂点座標を設定
	for(int i=0; i<16+1; i++){
		float x2, y2;
		x2 = (float)x + cos((float)M_PI*2/16 * i) * r;
		y2 = (float)y + sin((float)M_PI*2/16 * i) * r;
		VertexAry[i*2] = x2;	VertexAry[i*2+1] = y2;
	}

	//色情報配列を用意
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	for(int i=1; i<16+1; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//描画
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 16+1);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

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
	float VertexAry[4*2];
	unsigned char ColorAry[4*4];

	//2D描画用設定を適用
	Start2DRender();

	//テクスチャ無効
	glDisable(GL_TEXTURE_2D);

	//頂点配列を用意
	VertexAry[0] = (float)x1;	VertexAry[1] = (float)y1;
	VertexAry[2] = (float)x2;	VertexAry[3] = (float)y1;
	VertexAry[4] = (float)x1;	VertexAry[5] = (float)y2;
	VertexAry[6] = (float)x2;	VertexAry[7] = (float)y2;

	//色情報配列を用意
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//描画
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

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

	float VertexAry[4*2];
	float ColorAry[4*4];
	float TexCoordAry[4*2];

	//2D描画用設定を適用
	Start2DRender();

	//テクスチャ有効
	glEnable(GL_TEXTURE_2D);

	//テクスチャをセット
	SetTexture(id);

	//頂点配列を用意
	VertexAry[0] = (float)x;		VertexAry[1] = (float)y;
	VertexAry[2] = (float)x+width;	VertexAry[3] = (float)y;
	VertexAry[4] = (float)x;		VertexAry[5] = (float)y+height;
	VertexAry[6] = (float)x+width;	VertexAry[7] = (float)y+height;

	//色情報配列を用意
	ColorAry[0] = 1.0f;
	ColorAry[1] = 1.0f;
	ColorAry[2] = 1.0f;
	ColorAry[3] = alpha;
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(float)*4);
	}

	//UV座標配列を用意
	TexCoordAry[0] = 0.0f;	TexCoordAry[1] = 0.0f;
	TexCoordAry[2] = 1.0f;	TexCoordAry[3] = 0.0f;
	TexCoordAry[4] = 0.0f;	TexCoordAry[5] = 1.0f;
	TexCoordAry[6] = 1.0f;	TexCoordAry[7] = 1.0f;

	//配列有効化
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//描画
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_FLOAT, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//配列無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//2D描画用設定を解除
	End2DRender();
}

//! @brief 2D描画用設定を解除
void D3DGraphics::End2DRender()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

//! @brief 画面のスクリーンショットを保存
//! @param filename ファイル名
//! @return 成功：true　失敗：false
bool D3DGraphics::SaveScreenShot(char* filename)
{
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
	unsigned char red2, green2, blue2, alpha2;
	red2 = (unsigned char)(red*255);
	green2 = (unsigned char)(green*255);
	blue2 = (unsigned char)(blue*255);
	alpha2 = (unsigned char)(alpha*255);

	return (red2 << 24) | (green2 << 16) | (blue2 << 8) | alpha2;
}

#endif	//GRAPHICS_OPENGL