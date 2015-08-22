//! @file d3dgraphics-opengl.cpp
//! @brief D3DGraphics�N���X�̒�`�iOpenGL�Łj

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

//! @brief �R���X�g���N�^
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
}

//! @brief �f�B�X�g���N�^
D3DGraphics::~D3DGraphics()
{
	for(int i=0; i<MAX_MODEL; i++){
		CleanupModel(i);
	}
	for(int i=0; i<MAX_TEXTURE; i++){
		CleanupTexture(i);
	}

	if( SystemFont != NULL ){
		DeleteObject(SystemFont);
	}
	if( now_SystemFontUStr != NULL ){
		delete [] now_SystemFontUStr;
	}
	if( SystemFontListIdx != 0 ){
		glDeleteLists(SystemFontListIdx, SystemFontListIdxSize);
	}

	if( hGLRC != NULL ){ wglDeleteContext(hGLRC); }

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "�O���t�B�b�N", "OpenGL");
#endif

	//libjpeg���
	jpeg_destroy_decompress(&cinfo);
}

//! @brief ������@n
//! �iOpenGL 1.1�j
//! @param WindowCtrl WindowControl�N���X�̃|�C���^
//! @param TextureFontFilename �g�p����e�N�X�`���t�H���g�̃t�@�C����
//! @param fullscreen false�F�E�B���h�E�\���@true�F�t���X�N���[���p�\��
//! @return �����F0�@���s�F1
int D3DGraphics::InitD3D(WindowControl *WindowCtrl, char *TextureFontFilename, bool fullscreen)
{
#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_INIT, "�O���t�B�b�N", "OpenGL");
#endif

	hWnd = WindowCtrl->GethWnd();

	RECT prc;
	GetClientRect(hWnd, &prc);
	width = prc.right;
	height = prc.bottom;

	//�t���X�N���[����
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

	//�s�N�Z���t�H�[�}�b�g
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

	//�f�o�C�X�R���e�L�X�g�擾
	hDC = GetDC(hWnd);

	//�s�N�Z���t�H�[�}�b�g���擾
	pfdID = ChoosePixelFormat(hDC, &pfd);	
	if (pfdID == 0) { return 1; }

	//�s�N�Z���t�H�[�}�b�g���w��
	bResult = SetPixelFormat(hDC, pfdID, &pfd);
	if (bResult == FALSE) { return 1; }

	//�R���e�L�X�g���w��
	hGLRC = wglCreateContext(hDC);
	if (hGLRC == NULL) { return 1; }

	//�f�o�C�X�R���e�L�X�g���
	ReleaseDC(hWnd, hDC);

	//�V�X�e���t�H���g�p��
	//�t�H���g���F�l�r �S�V�b�N�@�T�C�Y�F18
	SystemFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "�l�r �S�V�b�N");

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//�e�N�X�`���t�H���g�p�摜�̃t�@�C������ݒ�
	strcpy(TextureFontFname, TextureFontFilename);

	//�e�N�X�`���t�H���g�p�摜���擾
	TextureFont = LoadTexture(TextureFontFname, true, false);


	float aspecth, prx, pry, r;
	aspecth = (float)SCREEN_WIDTH/SCREEN_HEIGHT;

	//HUD_myweapon [���s��, �c, ��]

	//HUD_A�@���ݎ����Ă��镐���\��������W
	prx = (float)M_PI/180*-39 * aspecth /2;
	pry = (float)M_PI/180*-55 /2;
	r = 7.5f;
	HUD_myweapon_x[0] = cos(pry)*r;
	HUD_myweapon_y[0] = sin(pry)*r;
	HUD_myweapon_z[0] = sin(prx)*r;

	//HUD_A�@�\���̕����\��������W
	prx = (float)M_PI/180*-52 * aspecth /2;
	pry = (float)M_PI/180*-60 /2;
	r = 16.0f;
	HUD_myweapon_x[1] = cos(pry)*r;
	HUD_myweapon_y[1] = sin(pry)*r;
	HUD_myweapon_z[1] = sin(prx)*r;


	//libjpeg������
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	return 0;
}

//! @brief ���Z�b�g@n
//! �i�E�B���h�E�ŏ�������̕��A�@�Ȃǁj
//! @param WindowCtrl WindowControl�N���X�̃|�C���^
//! @return �����F0�@�҂��F1�@���s�F2
int D3DGraphics::ResetD3D(WindowControl *WindowCtrl)
{
#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_INIT, "�O���t�B�b�N", "OpenGL�i���Z�b�g�j");
#endif

	hWnd = WindowCtrl->GethWnd();

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_ERROR, "", "");
#endif

	return 2;
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

	char stroks[] = " ;,";		//��؂镶����

	//�󂢂Ă���F���ԍ���T��
	for(int i=0; i<MAX_TEXTURE; i++){
		if( pmodel[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	//�t�@�C����ǂݍ���
	fp = fopen(filename, "r");
	if( fp == NULL ){
		return -1;		//�t�@�C�����ǂ߂Ȃ�
	}

	//�}�W�b�N�R�[�h�擾
	fgets(buf, 256, fp);
	buf[ strlen("xof 0302txt") ] = 0x00;
	if( strcmp(buf, "xof 0302txt") != 0 ){
		fclose( fp );
		return -1;		//X�t�@�C���łȂ�
	}

	while( fgets(buf, 256, fp) != NULL ){
		strcpy(str, buf);
		str[ strlen("Mesh") ] = 0x00;
		if( strcmp(str, "Mesh") == 0 ){
			
			fgets(buf, 256, fp);
			in_vertexs = atoi(buf);

			if( vertexs == 0 ){
				//1�ڂ̃��b�V���f�[�^�Ȃ�΁A�̈���쐬���邾���B
				vertex = new MODELVDATA [in_vertexs];
			}
			else{
				//2�ڂ̈ȍ~�Ȃ�A�̈���m�ۂ������ăR�s�[���A�Â��̈�͍폜�B
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
				//1�ڂ̃C���f�b�N�X�f�[�^�Ȃ�΁A�̈���쐬���邾���B
				index = new int [in_polygons*5];
			}
			else{
				//2�ڂ̈ȍ~�Ȃ�A�̈���m�ۂ������ăR�s�[���A�Â��̈�͍폜�B
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
				str[ strlen(" MeshTextureCoords") ] = 0x00;
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

	//�t�@�C���n���h�������
	fclose( fp );

	float *VertexAry = new float [polygons*6*3];
	float *ColorAry = new float [polygons*6*4];
	float *TexCoordAry = new float [polygons*6*2];
	int vid;
	int cnt = 0;

	for(int i=0; i<polygons; i++){
		if( index[i*5] == 3 ){
			//�O�p�`
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
			//�l�p�`
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

	//�F���z���p��
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

	/*
	//�f�[�^�������������ׂ�
	if( (idA < 0)||((MAX_MODEL -1) < idA) ){ return -1; }
	if( pmodel[idA].useflag == false ){ return; }
	if( (idB < 0)||((MAX_MODEL -1) < idB) ){ return -1; }
	if( pmodel[idB].useflag == false ){ return; }

	int id = -1;

	//�󂢂Ă���F���ԍ���T��
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
	//���O�ɏo��
	OutputLog.WriteLog(LOG_ERROR, "", "");
#endif

	return idA;
}

//! @brief ���f���t�@�C�������
//! @param id ���f���F���ԍ�
void D3DGraphics::CleanupModel(int id)
{
	if( (id < 0)||((MAX_MODEL -1) < id) ){ return; }
	if( pmodel[id].useflag == false ){ return; }

	delete pmodel[id].VertexAry;
	delete pmodel[id].ColorAry;
	delete pmodel[id].TexCoordAry;
	pmodel[id].useflag = false;

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "���f��", id);
#endif
}

//! @brief �e�N�X�`����ǂݍ���
//! @param filename �t�@�C����
//! @param texturefont �e�N�X�`���t�H���g�t���O
//! @param BlackTransparent ���𓧉߂���
//! @return �����F�e�N�X�`���F���ԍ��i0�ȏ�j�@���s�F-1
int D3DGraphics::LoadTexture(char* filename, bool texturefont, bool BlackTransparent)
{
	int id = -1;
	char filename2[MAX_PATH];
	int format = 0;

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "�e�N�X�`��", filename);
#endif

	//�󂢂Ă���F���ԍ���T��
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	//�t�@�C�������������֕ϊ��i�g���q����p�j
	for(int i=0; i<strlen(filename); i++){
		filename2[i] = (char)tolower(filename[i]);
	}
	filename2[ strlen(filename) ] = NULL;

	//�g���q�Ńt�@�C���t�H�[�}�b�g�𔻒�
	for(int i=strlen(filename2)-1; i>0; i--){
		if( filename2[i] == '.' ){
			if( strcmp(&(filename2[i]), ".bmp") == 0 ){
				format = 1;
			}
			if( strcmp(&(filename2[i]), ".dds") == 0 ){
				format = 2;
			}
			if( strcmp(&(filename2[i]), ".jpeg") == 0 ){
				format = 3;
			}
			if( strcmp(&(filename2[i]), ".jpg") == 0 ){
				format = 3;
			}
			if( strcmp(&(filename2[i]), ".png") == 0 ){
				format = 4;
			}
			break;
		}
	}

	//�Ή����ĂȂ��t�H�[�}�b�g
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


	//�e�N�X�`���L��
	glEnable(GL_TEXTURE_2D);

	HDC hDC;
	hDC = GetDC(hWnd);
	wglMakeCurrent(hDC, hGLRC);
	glGenTextures(1 , &(textureobjname[id]));
	ReleaseDC(hWnd, hDC);

	glBindTexture(GL_TEXTURE_2D, textureobjname[id]);

	//OpenGL�ɃZ�b�g
	int width = ptextures[id].width;
	int height = ptextures[id].height;
	unsigned char *data = ptextures[id].data;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//�~�b�v�}�b�v�ݒ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//��Z����
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�e�N�X�`������
	glDisable(GL_TEXTURE_2D);

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
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

//! @brief BMP�t�@�C������ǂݍ���
//! @param filename �t�@�C����
//! @param BlackTransparent ���𓧉߂���
//! @param ptexture �󂯎��TEXTUREDATA�\���̂̃|�C���^
//! @return �����Ftrue�@���s�Ffalse
//! @attention �ʏ�� LoadTexture()�֐� ����Ăт������ƁB
bool D3DGraphics::LoadBMPTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int index;

	//�t�@�C����ǂݍ���
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//�t�@�C�����ǂ߂Ȃ�
	}

	//�w�b�_�[��ǂ�
	fread(header, 1, 54, fp);

	if( (header[0x00] != 'B')||(header[0x01] != 'M') ){
		fclose(fp);
		return false;		//.bmp�ł͂Ȃ�
	}

	// �o�C�g�z�񂩂琮����ǂݍ���
	dataPos = *(int*)&(header[0x0E]) + 14;
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	index = *(int*)&(header[0x1C]);

	//���f�[�^�̐擪�܂ňړ�
	fseek(fp, dataPos, SEEK_SET);

	unsigned char *data = new unsigned char [width*height*4];

	//�e�s�N�Z��8�r�b�g�Ȃ�A256�F�p���b�g���[�h
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
					//���Ȃ�Γ��߂���
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}

		delete []pallet;
	}

	//�e�s�N�Z��24�r�b�g�Ȃ�A�t���J���[
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
					//���Ȃ�Γ��߂���
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}
	}

	//�t�@�C���n���h�������
	fclose( fp );

	//�\���̂ɑ��
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

//! @brief DDS�t�@�C������ǂݍ���
//! @param filename �t�@�C����
//! @param BlackTransparent ���𓧉߂���
//! @param ptexture �󂯎��TEXTUREDATA�\���̂̃|�C���^
//! @return �����Ftrue�@���s�Ffalse
//! @attention �ʏ�� LoadTexture()�֐� ����Ăт������ƁB
bool D3DGraphics::LoadDDSTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	unsigned char header[124+4];
	unsigned int width, height;
	unsigned int index;

	//�t�@�C����ǂݍ���
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//�t�@�C�����ǂ߂Ȃ�
	}

	//�w�b�_�[��ǂ�
	fread(header, 1, 124+4, fp);

	if( (header[0x00] != 'D')||(header[0x01] != 'D')||(header[0x02] != 'S')||(header[0x03] != ' ') ){
		fclose(fp);
		return false;		//.dds�ł͂Ȃ�
	}

	// �o�C�g�z�񂩂琮����ǂݍ���
	width = *(int*)&(header[0x10]);
	height = *(int*)&(header[0x0C]);
	index = *(int*)&(header[0x58]);

	if( (index != 16)&&(index != 32) ){
		fclose(fp);
		return false;		//�Ή����ĂȂ��t�H�[�}�b�g
	}

	unsigned char *data = new unsigned char [width*height*4];

	for(int h=0; h<height; h++){
		for(int w=0; w<width; w++){
			unsigned char pixel[4];
			fread(&pixel, 1, index/8, fp);

			if( index == 16 ){		//�e�s�N�Z��16�r�b�g
				data[(h*width+w)*4 + 0] = (pixel[1]<<4)&0xF0;
				data[(h*width+w)*4 + 1] = pixel[0]&0xF0;
				data[(h*width+w)*4 + 2] = (pixel[0]<<4)&0xF0;
				data[(h*width+w)*4 + 3] = pixel[1]&0xF0;
			}
			if( index == 32 ){		//�e�s�N�Z��32�r�b�g
				data[(h*width+w)*4 + 0] = pixel[2];
				data[(h*width+w)*4 + 1] = pixel[1];
				data[(h*width+w)*4 + 2] = pixel[0];
				data[(h*width+w)*4 + 3] = pixel[3];
			}

			if( BlackTransparent == true ){
				//���Ȃ�Γ��߂���
				if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
					data[(h*width+w)*4 + 3] = 0;
				}
			}
		}
	}

	//�t�@�C���n���h�������
	fclose( fp );

	//�\���̂ɑ��
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

//! @brief JPEG�t�@�C������ǂݍ���
//! @param filename �t�@�C����
//! @param BlackTransparent ���𓧉߂���
//! @param ptexture �󂯎��TEXTUREDATA�\���̂̃|�C���^
//! @return �����Ftrue�@���s�Ffalse
//! @attention �ʏ�� LoadTexture()�֐� ����Ăт������ƁB
bool D3DGraphics::LoadJPEGTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	JSAMPARRAY img;
	unsigned int width, height;

	//�t�@�C����ǂݍ���
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//�t�@�C�����ǂ߂Ȃ�
	}
	jpeg_stdio_src(&cinfo, fp);

	//�p�����[�^�̐ݒ�
	jpeg_read_header(&cinfo, true);

	//�W�J�J�n
	jpeg_start_decompress(&cinfo);

	//�̈�m��
	img = (JSAMPARRAY)new JSAMPROW [cinfo.output_height];
	for(int i=0; i<cinfo.output_height; i++){
		img[i] = (JSAMPROW)new JSAMPLE [cinfo.output_width * cinfo.out_color_components];
	}

	//�W�J
	while( cinfo.output_scanline < cinfo.output_height ) {
		jpeg_read_scanlines(&cinfo, img + cinfo.output_scanline, cinfo.output_height - cinfo.output_scanline);
	}

	//�W�J�I��
	jpeg_finish_decompress(&cinfo);


	//�t�@�C���n���h�������
	fclose( fp );


	// �o�C�g�z�񂩂琮����ǂݍ���
	width = (int)cinfo.output_width;
	height = (int)cinfo.output_height;

	unsigned char *data = new unsigned char [width*height*4];

	for(int h=0; h<height; h++){
		//1���C�����擾
		JSAMPROW p = img[h];

		for(int w=0; w<width; w++){
			data[(h*width+w)*4 + 0] = p[w*3 + 0];
			data[(h*width+w)*4 + 1] = p[w*3 + 1];
			data[(h*width+w)*4 + 2] = p[w*3 + 2];
			data[(h*width+w)*4 + 3] = 255;

			if( BlackTransparent == true ){
				//���Ȃ�Γ��߂���
				if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
					data[(h*width+w)*4 + 3] = 0;
				}
			}

		}
	}

	//�̈���
	for(int i=0; i<cinfo.output_height; i++){
		delete [] img[i];
	}
	delete [] img;

	//�\���̂ɑ��
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

//! @brief PNG�t�@�C������ǂݍ���
//! @param filename �t�@�C����
//! @param BlackTransparent ���𓧉߂���
//! @param ptexture �󂯎��TEXTUREDATA�\���̂̃|�C���^
//! @return �����Ftrue�@���s�Ffalse
//! @attention �ʏ�� LoadTexture()�֐� ����Ăт������ƁB
bool D3DGraphics::LoadPNGTexture(char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	png_byte sig[4];
	png_structp pPng;
    png_infop pInfo;
	unsigned int width, height;

	//�t�@�C����ǂݍ���
	fp = fopen(filename, "rb");
	if( fp == NULL ){
		return false;		//�t�@�C�����ǂ߂Ȃ�
	}

	//PNG�t�@�C��������
	fread(sig, 4, 1, fp);
	if( png_sig_cmp(sig, 0, 4) != 0 ){
		fclose(fp);
		return false;	//.png�ł͂Ȃ�
	}

	//�\���̂�������
	pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    pInfo = png_create_info_struct(pPng);

	//�����擾
	png_init_io(pPng, fp);
	png_set_sig_bytes(pPng, 4);
	png_read_info(pPng, pInfo);

	//�e�N�X�`���̑傫�����擾
	width = png_get_image_width(pPng, pInfo);
	height =  png_get_image_height(pPng, pInfo);

	//�C���^�[���X����������
	if( png_get_interlace_type(pPng, pInfo) != PNG_INTERLACE_NONE ){
		png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
		fclose(fp);
		return false;	//�C���^�[���X�ɂ͑Ή����Ȃ��B
	}

	//�A���t�@�`�����l����������
	png_set_add_alpha(pPng, 0xff, PNG_FILLER_AFTER);

	// tRNS�`�����N������΁A�A���t�@�`�����l���ɕϊ�
	if (png_get_valid(pPng, pInfo, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(pPng);
	}

	unsigned char *data = new unsigned char [width*height*4];

	//1���C�����̍�Ɨ̈���m��
	png_bytep buf = new png_byte[width*4];

	for(int h=0; h<height; h++){
		//1���C�����擾
		png_read_row(pPng,buf,NULL);

		for(int w=0; w<width; w++){
			data[(h*width+w)*4 + 0] = buf[w*4 + 0];
			data[(h*width+w)*4 + 1] = buf[w*4 + 1];
			data[(h*width+w)*4 + 2] = buf[w*4 + 2];
			data[(h*width+w)*4 + 3] = buf[w*4 + 3];

			if( BlackTransparent == true ){
				//���Ȃ�Γ��߂���
				if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
					data[(h*width+w)*4 + 3] = 0;
				}
			}
		}
	}

	//1���C�����̍�Ɨ̈�����
	delete [] buf;

	//���
	png_read_end(pPng, NULL);
	png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);

	//�t�@�C���n���h�������
	fclose( fp );

	//�\���̂ɑ��
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
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
	if( ptextures[id].useflag == false ){ return 1; }

	*width = ptextures[id].width;
	*height = ptextures[id].height;

	return 0;
}

//! @brief �e�N�X�`�����w��
//! @param TextureID �e�N�X�`���F���ԍ�
void D3DGraphics::SetTexture(int TextureID)
{
	if( now_textureid == TextureID ){
		return;
	}

	//OpenGL�ɃZ�b�g
	glBindTexture(GL_TEXTURE_2D, textureobjname[TextureID]);

	now_textureid = TextureID;
}

//! @brief �e�N�X�`�������
//! @param id �e�N�X�`���F���ԍ�
void D3DGraphics::CleanupTexture(int id)
{
	if( (id < 0)||((MAX_TEXTURE -1) < id) ){ return; }
	if( ptextures[id].useflag == false ){ return; }

	delete ptextures[id].data;
	glDeleteTextures(1 , &(textureobjname[id]));
	ptextures[id].useflag = false;

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "�e�N�X�`��", id);
#endif
}

//! @brief �S�Ă̕`�揈�����J�n
//! @return �����F0�@���s�F1
//! @attention �`�揈���̍ŏ��ɌĂяo���K�v������܂��B
int D3DGraphics::StartRender()
{
	HDC hDC;

	hDC = BeginPaint(hWnd, &Paint_ps);

	//�R���e�L�X�g���w��
	wglMakeCurrent(hDC, hGLRC);

	//������
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//���������i���ߗL�����j
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//�A���t�@�e�X�g
	glAlphaFunc(GL_GREATER, 0.0f);
	glEnable(GL_ALPHA_TEST);

	return 0;
}

//! @brief �S�Ă̕`�揈�����I��
//! @return �����Ffalse�@���s�Ftrue
//! @attention �`�揈���̍Ō�ɌĂяo���K�v������܂��B
bool D3DGraphics::EndRender()
{
	glFlush();

	wglMakeCurrent(NULL, NULL);

	EndPaint(hWnd, &Paint_ps);

	return false;
}

//! @brief Z�o�b�t�@�����Z�b�g
void D3DGraphics::ResetZbuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

//! @brief ���[���h��Ԃ����_�i0,0,0�j�ɖ߂��@�Ȃ�
void D3DGraphics::ResetWorldTransform()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewangle*(180.0f/(float)M_PI), (float)width/height, CLIPPINGPLANE_NEAR, CLIPPINGPLANE_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x*-1, camera_y, camera_z, camera_x*-1 + cos(camera_rx*-1 + (float)M_PI)*cos(camera_ry), camera_y + sin(camera_ry), camera_z + sin(camera_rx*-1 + (float)M_PI)*cos(camera_ry), 0.0f, 1.0f, 0.0f);
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
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry1*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glRotatef(ry2*-1*(180.0f/(float)M_PI), 0.0f, 0.0f, 1.0f);
	glScalef(size, size, size);
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
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry*-1*(180.0f/(float)M_PI), 0.0f, 0.0f, 1.0f);
	glRotatef(rt*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glScalef(size, size, size);
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
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glTranslatef(mx*-1, my, mz);
	glScalef(size, size, size);
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

//! @brief ���[���h��Ԃ̍��W���擾
//! @param *x x�����󂯎��|�C���^
//! @param *y y�����󂯎��|�C���^
//! @param *z z�����󂯎��|�C���^
void D3DGraphics::GetWorldTransformPos(float *x, float *y, float *z)
{
	GLfloat model[16];
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, model);
	*x = model[12];
	*y = model[13];
	*z = model[14];
}

//! @brief �t�H�O��ݒ�
//! @param skynumber ��̔ԍ�
void D3DGraphics::SetFog(int skynumber)
{
	float fogColor[4];

	//��̔ԍ��ɂ��F������
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

//! @brief �J�����i���_�j��ݒ�
//! @param in_camera_x �J������X���W
//! @param in_camera_y �J������Y���W
//! @param in_camera_z �J������Z���W
//! @param in_camera_rx �J�����̉����p�x
//! @param in_camera_ry �J�����̏c���p�x
//! @param in_viewangle ����p
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

//! @brief �}�b�v�f�[�^����荞��
//! @param in_blockdata �u���b�N�f�[�^
//! @param directory �u���b�N�f�[�^�����݂���f�B���N�g��
void D3DGraphics::LoadMapdata(BlockDataInterface* in_blockdata, char *directory)
{
	//�u���b�N�f�[�^���w�肳��Ă��Ȃ���΁A�������Ȃ��B
	if( in_blockdata == NULL ){ return; }

	char fname[MAX_PATH];
	char fnamefull[MAX_PATH];

	//�N���X��ݒ�
	blockdata = in_blockdata;

	//�u���b�N�����擾
	bs = blockdata->GetTotaldatas();

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
}

//! @brief �}�b�v�f�[�^��`��
//! @param wireframe ���C���[�t���[���\��
void D3DGraphics::DrawMapdata(bool wireframe)
{
	//�u���b�N�f�[�^���ǂݍ��܂�Ă��Ȃ���΁A�������Ȃ��B
	if( blockdata == NULL ){ return; }

	struct blockdata data;
	int textureID;
	int vID[4];
	int uvID[4];
	float *VertexAry = new float [bs*6 * 6*3];
	float *ColorAry = new float [bs*6 * 6*4];
	float *TexCoordAry = new float [bs*6 * 6*2];

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

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		int cnt = 0;

		//�e�N�X�`��������ɓǂݍ��߂Ă��Ȃ���ΐݒ�
		if( mapTextureID[textureID] == -1 ){
			//�e�N�X�`������
			glDisable(GL_TEXTURE_2D);
		}
		else if( ptextures[ mapTextureID[textureID] ].useflag == false ){
			//�e�N�X�`������
			glDisable(GL_TEXTURE_2D);
		}
		else{
			//�e�N�X�`���L��
			glEnable(GL_TEXTURE_2D);

			//�e�N�X�`�����Z�b�g
			SetTexture(mapTextureID[textureID]);
		}

		for(int i=0; i<bs; i++){
			//�f�[�^�擾
			blockdata->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//�e�N�X�`���F���ԍ����擾
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//�ʂ̒��_�f�[�^�̊֘A�t�����擾
					blockdataface(j, &vID[0], &uvID[0]);

					//���_�z���p��
					VertexAry[0 + cnt*18] = data.x[ vID[1] ]*-1;		VertexAry[1 + cnt*18] = data.y[ vID[1] ];	VertexAry[2 + cnt*18] = data.z[ vID[1] ];
					VertexAry[3 + cnt*18] = data.x[ vID[1] ]*-1;		VertexAry[4 + cnt*18] = data.y[ vID[1] ];	VertexAry[5 + cnt*18] = data.z[ vID[1] ];
					VertexAry[6 + cnt*18] = data.x[ vID[0] ]*-1;		VertexAry[7 + cnt*18] = data.y[ vID[0] ];	VertexAry[8 + cnt*18] = data.z[ vID[0] ];
					VertexAry[9 + cnt*18] = data.x[ vID[2] ]*-1;		VertexAry[10 + cnt*18] = data.y[ vID[2] ];	VertexAry[11 + cnt*18] = data.z[ vID[2] ];
					VertexAry[12 + cnt*18] = data.x[ vID[3] ]*-1;		VertexAry[13 + cnt*18] = data.y[ vID[3] ];	VertexAry[14 + cnt*18] = data.z[ vID[3] ];
					VertexAry[15 + cnt*18] = data.x[ vID[3] ]*-1;		VertexAry[16 + cnt*18] = data.y[ vID[3] ];	VertexAry[17 + cnt*18] = data.z[ vID[3] ];

					//�F���z���p��
					ColorAry[0 + cnt*24] = data.material[j].shadow;
					ColorAry[1 + cnt*24] = data.material[j].shadow;
					ColorAry[2 + cnt*24] = data.material[j].shadow;
					ColorAry[3 + cnt*24] = 1.0f;
					for(int i=1; i<6; i++){
						memcpy(&(ColorAry[i*4 + cnt*24]), ColorAry, sizeof(float)*4);
					}

					//UV���W�z���p��
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

		//�`��
		glVertexPointer(3, GL_FLOAT, 0, VertexAry);
		glColorPointer(4, GL_FLOAT, 0, ColorAry);
		glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
		glDrawArrays(GL_TRIANGLE_STRIP, 1, 6*cnt-2);
	}

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	delete [] VertexAry;
	delete [] ColorAry;
	delete [] TexCoordAry;
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

	bs = 0;

	blockdata = NULL;
}

//! @brief ���f���t�@�C����`��
//! @param id_model ���f���F���ԍ�
//! @param id_texture �e�N�X�`���F���ԍ�
void D3DGraphics::RenderModel(int id_model, int id_texture)
{
	//�����Ȉ������ݒ肳��Ă���Ύ��s
	if( id_model == -1 ){ return; }
	if( pmodel[id_model].useflag == false ){ return; }

	//�e�N�X�`��������ɓǂݍ��߂Ă��Ȃ���ΐݒ�
	if( id_texture == -1 ){
		//�e�N�X�`������
		glDisable(GL_TEXTURE_2D);
	}
	else if( ptextures[id_texture].useflag == false ){
		//�e�N�X�`������
		glDisable(GL_TEXTURE_2D);
	}
	else{
		//�e�N�X�`���L��
		glEnable(GL_TEXTURE_2D);

		//�e�N�X�`�����Z�b�g
		SetTexture(id_texture);
	}

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//�`��
	glVertexPointer(3, GL_FLOAT, 0, pmodel[id_model].VertexAry);
	glColorPointer(4, GL_FLOAT, 0, pmodel[id_model].ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, pmodel[id_model].TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, pmodel[id_model].polygons*6-2);

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	/*
	Drawline(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	Drawline(0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f);
	Drawline(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f);
	*/
}

//! @brief ��`��
//! @param id_texture �e�N�X�`���F���ԍ�
//! @param alpha �����x�@�i0.0�`1.0�@0.0�F���S�����j
void D3DGraphics::RenderBoard(int id_texture, float alpha)
{
	//�e�N�X�`�����ݒ肳��Ă��Ȃ���΁A�������Ȃ��B
	if( id_texture == -1 ){ return; }
	if( ptextures[id_texture].useflag == false ){ return; }

	float VertexAry[4*3];
	float ColorAry[4*4];
	float TexCoordAry[4*2];

	//�e�N�X�`���L��
	glEnable(GL_TEXTURE_2D);

	//�e�N�X�`�����Z�b�g
	SetTexture(id_texture);

	//���_�z���p��
	VertexAry[0] = 0.0f;	VertexAry[1] = 0.5f;	VertexAry[2] = 0.5f;
	VertexAry[3] = 0.0f;	VertexAry[4] = -0.5f;	VertexAry[5] = 0.5f;
	VertexAry[6] = 0.0f;	VertexAry[7] = 0.5f;	VertexAry[8] = -0.5f;
	VertexAry[9] = 0.0f;	VertexAry[10] = -0.5f;	VertexAry[11] = -0.5f;

	//�F���z���p��
	ColorAry[0] = 1.0f;
	ColorAry[1] = 1.0f;
	ColorAry[2] = 1.0f;
	ColorAry[3] = alpha;
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(float)*4);
	}

	//UV���W�z���p��
	TexCoordAry[0] = 1.0f;	TexCoordAry[1] = 0.0f;
	TexCoordAry[2] = 1.0f;	TexCoordAry[3] = 1.0f;
	TexCoordAry[4] = 0.0f;	TexCoordAry[5] = 0.0f;
	TexCoordAry[6] = 0.0f;	TexCoordAry[7] = 1.0f;

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//�`��
	glVertexPointer(3, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_FLOAT, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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
	Draw2DBox(0, 0, Width, Height, GetColorCode(1.0f,1.0f,1.0f,alpha));
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
	float VertexAry[2*3];
	unsigned char ColorAry[2*4];

	//�e�N�X�`������
	glDisable(GL_TEXTURE_2D);

	//���_�z���p��
	VertexAry[0] = (float)x1*-1;	VertexAry[1] = (float)y1;	VertexAry[2] = (float)z1;
	VertexAry[3] = (float)x2*-1;	VertexAry[4] = (float)y2;	VertexAry[5] = (float)z2;

	//�F���z���p��
	ColorAry[0] = 0;
	ColorAry[1] = 255;
	ColorAry[2] = 0;
	ColorAry[3] = 255;
	memcpy(&(ColorAry[4]), ColorAry, sizeof(unsigned char)*4);

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//�`��
	glVertexPointer(3, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

//! @brief ������`��i�V�X�e���t�H���g�g�p�j
//! @param x x���W
//! @param y y���W
//! @param str ������@�i���s�R�[�h�F�j
//! @param color �F
//! @warning <b>�`��͔��ɒᑬ�ł��B</b>��ʓ��ŉ��x���Ăяo���ƃp�t�H�[�}���X�ɉe�����܂��B
//! @warning�u���s�R�[�h�����p����x�ɕ`�悷��v�u���{�ꂪ�K�v�Ȃ������̓e�N�X�`���t�H���g�����p����v�Ȃǂ̑Ή����u���Ă��������B
//! @attention �t�H���g�̎�ނ�T�C�Y�͌Œ�ł��B�@�������d�ɏd�˂ė��̊����o���Ȃ��ƌ��ɂ����Ȃ�܂��B
//! @todo �������d�ɏd�˂�ƁA�㉺�֌W����������������Ȃ��B
//! @todo 1�����ڂ�������ꍇ������B
void D3DGraphics::Draw2DMSFontText(int x, int y, char *str, int color)
{
	int len = strlen(str);
	WCHAR *ustr;

	Start2DRender();

	//�e�N�X�`������
	glDisable(GL_TEXTURE_2D);

	//Unicode������֕ϊ�
	ustr = new WCHAR [len+1];
	MultiByteToWideChar(CP_ACP,	0, str, -1, ustr, len + 1);

	//�V���ȕ�����Ȃ�A���\�[�X����蒼��
	if( lstrcmpW(ustr, now_SystemFontUStr) != 0 ){
		GLuint listIdx;
		HDC hDC;

		//�Â��f�[�^���폜
		glDeleteLists(SystemFontListIdx, SystemFontListIdxSize);
		delete [] now_SystemFontUStr;

		//�f�o�C�X�R���e�L�X�g�ݒ�
		hDC = GetDC(hWnd);
		wglMakeCurrent(hDC, hGLRC);
		SelectObject(hDC, SystemFont);

		//�f�B�X�v���C���X�g���쐬
		listIdx = glGenLists(len);
		for(int i=0; i<lstrlenW(ustr); i++){
			wglUseFontBitmapsW(hDC, ustr[i], 1, listIdx+i);
		}

		//�f�o�C�X�R���e�L�X�g�p��
		ReleaseDC(hWnd, hDC);

		//�ݒ���L�^
		now_SystemFontUStr = new WCHAR [len+1];
		lstrcpyW(now_SystemFontUStr, ustr);
		SystemFontListIdx = listIdx;
		SystemFontListIdxSize = len;
	}

	//���W�ƐF��ݒ�
	glBitmap(0, 0, 0, 0, 10, 0, NULL);
	glRasterPos2i(x, y);
	glColor4ub((color>>24)&0xFF, (color>>16)&0xFF, (color>>8)&0xFF, color&0xFF);

	for(int i=0; i<lstrlenW(ustr); i++){
		if( ustr[i] == '\n' ){
			//���s����
			y += 19;
			glRasterPos2i(x, y);
		}
		else{
			//�f�B�X�v���C���X�g�`��
			glCallList(SystemFontListIdx + i);
		}
	}

	//Unicode������̔p��
	delete [] ustr;

	End2DRender();
}

//! @brief �����𒆉������ŕ`��i�V�X�e���t�H���g�g�p�j
//! @param x x���W
//! @param y y���W
//! @param w ���̑傫��
//! @param h �c�̑傫��
//! @param str ������@�i���s�R�[�h�F�j
//! @param color �F
//! @warning <b>���������������ɂȂ�܂���B</b>
void D3DGraphics::Draw2DMSFontTextCenter(int x, int y, int w, int h, char *str, int color)
{
	Draw2DMSFontText(x, y, str, color);
}

//! @brief 2D�`��p�ݒ�
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

	int strlens = (int)strlen(str);

	float *VertexAry = new float [strlens*6*2];
	unsigned char *ColorAry = new unsigned char [strlens*6*4];
	float *TexCoordAry = new float [strlens*6*2];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	int w;
	float font_u, font_v;
	float t_u, t_v;

	//1������UV���W���v�Z
	font_u = 1.0f / 16;
	font_v = 1.0f / 16;

	//�e�N�X�`���L��
	glEnable(GL_TEXTURE_2D);

	//�e�N�X�`�����Z�b�g
	SetTexture(TextureFont);

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//�F���z���p��
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	for(int i=1; i<strlens*6; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	// �^����ꂽ�����������[�v
	for(int i=0; i<strlens; i++){
		//UV���W���v�Z
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

	//�`��
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, strlens*6-2);

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//2D�`��p�ݒ������
	End2DRender();

	delete [] VertexAry;
	delete [] ColorAry;
	delete [] TexCoordAry;
}

//! @brief ����`��
//! @param x1 �n�_�� x���W
//! @param y1 �n�_�� y���W
//! @param x2 �I�_�� x���W
//! @param y2 �I�_�� y���W
//! @param color �F
void D3DGraphics::Draw2DLine(int x1, int y1, int x2, int y2, int color)
{
	float VertexAry[2*2];
	unsigned char ColorAry[2*4];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	//�e�N�X�`������
	glDisable(GL_TEXTURE_2D);

	//���_�z���p��
	VertexAry[0] = (float)x1;	VertexAry[1] = (float)y1;
	VertexAry[2] = (float)x2;	VertexAry[3] = (float)y2;

	//�F���z���p��
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	memcpy(&(ColorAry[4]), ColorAry, sizeof(unsigned char)*4);

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//�`��
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

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
	float VertexAry[(16+1)*2];
	unsigned char ColorAry[(16+1)*4];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	//�e�N�X�`������
	glDisable(GL_TEXTURE_2D);

	//���_���W��ݒ�
	for(int i=0; i<16+1; i++){
		float x2, y2;
		x2 = (float)x + cos((float)M_PI*2/16 * i) * r;
		y2 = (float)y + sin((float)M_PI*2/16 * i) * r;
		VertexAry[i*2] = x2;	VertexAry[i*2+1] = y2;
	}

	//�F���z���p��
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	for(int i=1; i<16+1; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//�`��
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 16+1);

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

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
	float VertexAry[4*2];
	unsigned char ColorAry[4*4];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	//�e�N�X�`������
	glDisable(GL_TEXTURE_2D);

	//���_�z���p��
	VertexAry[0] = (float)x1;	VertexAry[1] = (float)y1;
	VertexAry[2] = (float)x2;	VertexAry[3] = (float)y1;
	VertexAry[4] = (float)x1;	VertexAry[5] = (float)y2;
	VertexAry[6] = (float)x2;	VertexAry[7] = (float)y2;

	//�F���z���p��
	ColorAry[0] = (color>>24)&0xFF;
	ColorAry[1] = (color>>16)&0xFF;
	ColorAry[2] = (color>>8)&0xFF;
	ColorAry[3] = color&0xFF;
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//�`��
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

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

	float VertexAry[4*2];
	float ColorAry[4*4];
	float TexCoordAry[4*2];

	//2D�`��p�ݒ��K�p
	Start2DRender();

	//�e�N�X�`���L��
	glEnable(GL_TEXTURE_2D);

	//�e�N�X�`�����Z�b�g
	SetTexture(id);

	//���_�z���p��
	VertexAry[0] = (float)x;		VertexAry[1] = (float)y;
	VertexAry[2] = (float)x+width;	VertexAry[3] = (float)y;
	VertexAry[4] = (float)x;		VertexAry[5] = (float)y+height;
	VertexAry[6] = (float)x+width;	VertexAry[7] = (float)y+height;

	//�F���z���p��
	ColorAry[0] = 1.0f;
	ColorAry[1] = 1.0f;
	ColorAry[2] = 1.0f;
	ColorAry[3] = alpha;
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(float)*4);
	}

	//UV���W�z���p��
	TexCoordAry[0] = 0.0f;	TexCoordAry[1] = 0.0f;
	TexCoordAry[2] = 1.0f;	TexCoordAry[3] = 0.0f;
	TexCoordAry[4] = 0.0f;	TexCoordAry[5] = 1.0f;
	TexCoordAry[6] = 1.0f;	TexCoordAry[7] = 1.0f;

	//�z��L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//�`��
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_FLOAT, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//�z�񖳌���
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//2D�`��p�ݒ������
	End2DRender();
}

//! @brief 2D�`��p�ݒ������
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

//! @brief ��ʂ̃X�N���[���V���b�g��ۑ�
//! @param filename �t�@�C����
//! @return �����Ftrue�@���s�Ffalse
bool D3DGraphics::SaveScreenShot(char* filename)
{
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
	unsigned char red2, green2, blue2, alpha2;
	red2 = (unsigned char)(red*255);
	green2 = (unsigned char)(green*255);
	blue2 = (unsigned char)(blue*255);
	alpha2 = (unsigned char)(alpha*255);

	return (red2 << 24) | (green2 << 16) | (blue2 << 8) | alpha2;
}

#endif	//GRAPHICS_OPENGL