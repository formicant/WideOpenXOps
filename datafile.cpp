//! @file datafile.cpp
//! @brief �f�[�^�Ǘ��N���X�̒�`

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

#include "datafile.h"

//! @brief �R���X�g���N�^
DataInterface::DataInterface()
{
	datas = 0;
}

//! @brief �f�B�X�g���N�^
DataInterface::~DataInterface()
{}

//! @brief �f�[�^�t�@�C����ǂ݂���
//! @param fname �t�@�C����
//! @return �����F0�@���s�F1
int DataInterface::LoadFiledata(char *fname)
{
	return 0;
}

//! @brief ���v�f�[�^���̎擾
//! @return ���v�f�[�^��
int DataInterface::GetTotaldatas()
{
	return datas;
}

//! @brief �f�[�^���擾
//! @param out_data �󂯎��|�C���^
//! @param id �F���ԍ�
//! @return �����F0�@���s�F0�ȊO
int DataInterface::Getdata(void *out_data, int id)
{
	return 0;
}

//! @brief �R���X�g���N�^
BlockDataInterface::BlockDataInterface()
{
	//blockdata�\���̏�����
	data = new blockdata[MAX_BLOCKS];

	//�e�N�X�`���ݒ菉����
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		strcpy(texture[i], "");
	}
}

//! @brief �f�B�X�g���N�^
BlockDataInterface::~BlockDataInterface()
{
	//blockdata�\���̉��
	if( data != NULL ) delete [] data;

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "BD1", "");
#endif
}

//! @brief �u���b�N�f�[�^�t�@�C����ǂ݂���
//! @param fname �t�@�C����
//! @return �����F0�@���s�F1
int BlockDataInterface::LoadFiledata(char *fname)
{
	FILE *fp;
	unsigned char bdata_header[2];
	float bdata_main[80];
	char bdata_mainb[30];

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "BD1", fname);
#endif

#ifdef PATH_DELIMITER_SLASH
	//�p�X��؂蕶����ϊ�
	fname = ChangePathDelimiter(fname);
#endif

	//�t�@�C����ǂݍ���
	fp = fopen(fname, "rb");
	if( fp == NULL ){
		return 1;
	}

	//�e�N�X�`�����擾
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		fread(texture[i], 1, 31, fp );
	}

	//�f�[�^�����擾
	fread( bdata_header, 1, 2, fp );
	datas = (int)bdata_header[1]*256 + bdata_header[0];
	if( datas > MAX_BLOCKS ){
		datas = MAX_BLOCKS;
	}

	//�u���b�N�f�[�^�����擾
	for(int i=0; i<datas; i++){
		//�u���b�N�̂𒸓_���Ɩʏ����擾
		fread( bdata_main, 4, 24+48, fp );
		fread( bdata_mainb, 1, 28, fp );

		//�f�[�^�ԍ���ݒ�
		data[i].id = i;

		//�u���b�N�̒��_���ɐݒ�
		for(int j=0; j<8; j++){
			data[i].x[j] = bdata_main[j];
			data[i].y[j] = bdata_main[j+8];
			data[i].z[j] = bdata_main[j+16];
		}

		//�u���b�N�̖ʏ���ݒ�
		for(int j=0; j<6; j++){
			data[i].material[j].textureID = (int)bdata_mainb[j*4];
			data[i].material[j].u[0] = bdata_main[j*4+24];
			data[i].material[j].v[0] = bdata_main[j*4+48];
			data[i].material[j].u[1] = bdata_main[j*4+25];
			data[i].material[j].v[1] = bdata_main[j*4+49];
			data[i].material[j].u[2] = bdata_main[j*4+26];
			data[i].material[j].v[2] = bdata_main[j*4+50];
			data[i].material[j].u[3] = bdata_main[j*4+27];
			data[i].material[j].v[3] = bdata_main[j*4+51];
		}
	}

	//�t�@�C���n���h�������
	fclose( fp );

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief �u���b�N�f�[�^�̖@���E�e���Z�o����
//! @param screen �u���b�N���Â�����
//! @attention LoadBlockdata()�֐��œǂ݂��񂾌�A��x�������s�B
void BlockDataInterface::CalculationBlockdata(bool screen)
{
	int vID[4];
	int uvID[4];
	float g;
	float xs, ys, zs;
	float g1;
	float xs1, ys1, zs1;
	float g2;
	float xs2, ys2, zs2;
	float lx, ly, lz;
	float rx, ry, rz, a;

	//�����̊p�x��ݒ�
	lx = cos(LIGHT_RX);
	ly = sin(LIGHT_RY);
	lz = sin(LIGHT_RX);

	//�e�u���b�N�̖ʏ�񕪏�������
	for(int i=0; i<datas; i++){
		for(int j=0; j<6; j++){

			//�u���b�N���_�f�[�^�̊֘A�t�����擾
			blockdataface(j, vID, uvID);

			//�ʂ̖@���i�x�N�g���j�ƁA���̒��������߂�
			xs1 = ((data[i].y[ vID[3] ] - data[i].y[ vID[2] ]) * (data[i].z[ vID[0] ] - data[i].z[ vID[2] ])) - ((data[i].y[ vID[0] ] - data[i].y[ vID[2] ]) * (data[i].z[ vID[3] ] - data[i].z[ vID[2] ]));
			ys1 = ((data[i].z[ vID[3] ] - data[i].z[ vID[2] ]) * (data[i].x[ vID[0] ] - data[i].x[ vID[2] ])) - ((data[i].z[ vID[0] ] - data[i].z[ vID[2] ]) * (data[i].x[ vID[3] ] - data[i].x[ vID[2] ]));
			zs1 = ((data[i].x[ vID[3] ] - data[i].x[ vID[2] ]) * (data[i].y[ vID[0] ] - data[i].y[ vID[2] ])) - ((data[i].x[ vID[0] ] - data[i].x[ vID[2] ]) * (data[i].y[ vID[3] ] - data[i].y[ vID[2] ]));
			g1 = (float)sqrt(xs1 * xs1 + ys1 * ys1 + zs1 * zs1);

			//��������̎O�p�`�ł��v�Z����
			xs2 = ((data[i].y[ vID[1] ] - data[i].y[ vID[0] ]) * (data[i].z[ vID[2] ] - data[i].z[ vID[0] ])) - ((data[i].y[ vID[2] ] - data[i].y[ vID[0] ]) * (data[i].z[ vID[1] ] - data[i].z[ vID[0] ]));
			ys2 = ((data[i].z[ vID[1] ] - data[i].z[ vID[0] ]) * (data[i].x[ vID[2] ] - data[i].x[ vID[0] ])) - ((data[i].z[ vID[2] ] - data[i].z[ vID[0] ]) * (data[i].x[ vID[1] ] - data[i].x[ vID[0] ]));
			zs2 = ((data[i].x[ vID[1] ] - data[i].x[ vID[0] ]) * (data[i].y[ vID[2] ] - data[i].y[ vID[0] ])) - ((data[i].x[ vID[2] ] - data[i].x[ vID[0] ]) * (data[i].y[ vID[1] ] - data[i].y[ vID[0] ]));
			g2 = (float)sqrt(xs2 * xs2 + ys2 * ys2 + zs2 * zs2);

			//�������v�Z���ʂ��̗p����
			if( g1 > g2 ){
				xs = xs1;
				ys = ys1;
				zs = zs1;
				g = g1;
			}
			else{
				xs = xs2;
				ys = ys2;
				zs = zs2;
				g = g2;
			}

			//�@���i�x�N�g���j�𐳋K��
			data[i].material[j].vx = xs / g;
			data[i].material[j].vy = ys / g;
			data[i].material[j].vz = zs / g;

			//�ʂ̖��邳�����߂�
			rx = data[i].material[j].vx + lx;
			ry = data[i].material[j].vy + ly;
			rz = data[i].material[j].vz + lz;
			//a = sqrt(fabs(rx*rx + ry*ry + rz*rz)) / 2.0f;
			//data[i].material[j].shadow = a/2 + 0.5f;
			a = sqrt(rx*rx + ry*ry + rz*rz) / 3.0f;
			data[i].material[j].shadow = a/2;
			if( screen == false ){
				data[i].material[j].shadow += 0.5f;
			}
			else{
				data[i].material[j].shadow += 0.2f;
			}
		}
	}
}

//! @brief �u���b�N�f�[�^�ɐݒ肳�ꂽ�e�N�X�`���̃t�@�C�������擾
//! @param fname �t�@�C�������󂯎�镶����^�|�C���^
//! @param id �e�N�X�`���ԍ�
//! @return �����F0�@���s�F0�ȊO
int BlockDataInterface::GetTexture(char *fname, int id)
{
	if( data == NULL ){ return 1; }
	if( (id < 0)||((TOTAL_BLOCKTEXTURE -1) < id) ){ return 2; }

	//�|�C���^�Ƀe�N�X�`�������R�s�[
	strcpy(fname, texture[id]);

	return 0;
}

//! @brief �u���b�N�f�[�^���擾
//! @param out_data �󂯎��blockdata�^�|�C���^
//! @param id �F���ԍ�
//! @return �����F0�@���s�F0�ȊO
int BlockDataInterface::Getdata(blockdata *out_data, int id)
{
	if( data == NULL ){ return 1; }
	if( (id < 0)||((datas -1) < id) ){ return 2; }

	//�u���b�N�f�[�^���擾
	memcpy(out_data, &(data[id]), sizeof(blockdata));

	return 0;
}

//! @brief �u���b�N���_�f�[�^�̊֘A�t�����擾
//
//	    3 �E----�E2
//	     /|    /|
//	    / |   / |
//	0 �E----�E1 |
//	  |7 �E-| --�E6
//	  |  /  |  /
//	  | /   | /
//	4 �E----�E5
//
//	C-----D
//	|     |
//	|     |
//	B-----A
//
//	��[0](ABCD) =  (1032)
//	��[1]		=  (6745)
//	�O[2]		=  (5401)
//	�E[3]		=  (6512)
//	��[4]		=  (7623)
//	��[5]		=  (4730)
bool blockdataface(int faceID, int* vID, int* uvID)
{
	if( faceID == 0 ){
		if( vID != NULL ){
			vID[0] = 1;		vID[1] = 0;		vID[2] = 3;		vID[3] = 2;
		}
		if( uvID != NULL ){
			uvID[0] = 2;	uvID[1] = 3;	uvID[2] = 0;	uvID[3] = 1;
		}
		return true;
	}
	if( faceID == 1 ){
		if( vID != NULL ){
			vID[0] = 6;		vID[1] = 7;		vID[2] = 4;		vID[3] = 5;
		}
		if( uvID != NULL ){
			uvID[0] = 2;	uvID[1] = 3;	uvID[2] = 0;	uvID[3] = 1;
		}
		return true;
	}
	if( faceID == 2 ){
		if( vID != NULL ){
			vID[0] = 5;		vID[1] = 4;		vID[2] = 0;		vID[3] = 1;
		}
		if( uvID != NULL ){
			uvID[0] = 2;	uvID[1] = 3;	uvID[2] = 0;	uvID[3] = 1;
		}
		return true;
	}
	if( faceID == 3 ){
		if( vID != NULL ){
			vID[0] = 6;		vID[1] = 5;		vID[2] = 1;		vID[3] = 2;
		}
		if( uvID != NULL ){
			uvID[0] = 2;	uvID[1] = 3;	uvID[2] = 0;	uvID[3] = 1;
		}
		return true;
	}
	if( faceID == 4 ){
		if( vID != NULL ){
			vID[0] = 7;		vID[1] = 6;		vID[2] = 2;		vID[3] = 3;
		}
		if( uvID != NULL ){
			uvID[0] = 2;	uvID[1] = 3;	uvID[2] = 0;	uvID[3] = 1;
		}
		return true;
	}
	if( faceID == 5 ){
		if( vID != NULL ){
			vID[0] = 4;		vID[1] = 7;		vID[2] = 3;		vID[3] = 0;
		}
		if( uvID != NULL ){
			uvID[0] = 2;	uvID[1] = 3;	uvID[2] = 0;	uvID[3] = 1;
		}
		return true;
	}
	return false;
}

//! @brief �R���X�g���N�^
PointDataInterface::PointDataInterface()
{
	//pointdata�\���̏�����
	data = new pointdata[MAX_POINTS];

	//�C�x���g���b�Z�[�W������
	for(int i=0; i<MAX_POINTMESSAGES; i++){
		text[i] = new char[MAX_POINTMESSAGEBYTE];
		text[i][0] = NULL;
	}
}

//! @brief �f�B�X�g���N�^
PointDataInterface::~PointDataInterface()
{
	//pointdata�\���̉��
	if( data != NULL ) delete [] data;

	//�C�x���g���b�Z�[�W���
	for(int i=0; i<MAX_POINTMESSAGES; i++){
		if( text[i] != NULL ) delete [] text[i];
	}

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CLEANUP, "PD1", "");
#endif
}

//! @brief �|�C���g�f�[�^�t�@�C����ǂ݂���
//! @param fname �t�@�C����
//! @return �����F0�@���s�F1
int PointDataInterface::LoadFiledata(char *fname)
{
	FILE *fp;
	unsigned char pdata_header[2];
	float pdata_mainf[200][4];
	char pdata_mainc[200][4];
	char fname2[MAX_PATH];

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "PD1", fname);
#endif

#ifdef PATH_DELIMITER_SLASH
	//�p�X��؂蕶����ϊ�
	fname = ChangePathDelimiter(fname);
#endif

	//�t�@�C����ǂݍ���
	fp = fopen( fname, "rb" );
	if( fp == NULL ){
		return 1;
	}

	//�f�[�^�����擾
	fread( pdata_header, 1, 2, fp );
	datas = (int)pdata_header[1]*256 + pdata_header[0];
	if( datas > MAX_POINTS ){
		datas = MAX_POINTS;
	}

	//�|�C���g�f�[�^������������
	for(int i=0; i<datas; i++){
		//�f�[�^�ԍ��ݒ�
		data[i].id = i;

		//���W�f�[�^�擾
		fread( pdata_mainf[i], 4, 4, fp );
		data[i].x = pdata_mainf[i][0];
		data[i].y = pdata_mainf[i][1];
		data[i].z = pdata_mainf[i][2];
		data[i].r = pdata_mainf[i][3];

		//�p�����[�^�擾
		fread( pdata_mainc[i], 1, 4, fp );
		data[i].p1 = pdata_mainc[i][0];
		data[i].p2 = pdata_mainc[i][1];
		data[i].p3 = pdata_mainc[i][2];
		data[i].p4 = pdata_mainc[i][3];
	}

	//�t�@�C���|�C���^�����
	fclose( fp );

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//�u���t�@�C����.msg�v�𐶐�
	strcpy(fname2, fname);
	//PathRemoveExtension(fname2);
	for(int i=strlen(fname2)-1; i>0; i--){
		if( fname2[i] == '.' ){
			fname2[i] = 0x00;
			break;
		}
	}
	strcat(fname2, ".msg");

#ifdef PATH_DELIMITER_SLASH
	//�p�X��؂蕶����ϊ�
	strcpy(fname2, ChangePathDelimiter(fname2));
#endif

	//�t�@�C����ǂݍ���
	fp = fopen( fname2, "r" );
	if( fp != NULL ){
#ifdef ENABLE_DEBUGLOG
		//���O�ɏo��
		OutputLog.WriteLog(LOG_LOAD, "MSG", fname2);
#endif

		//���b�Z�[�W�f�[�^���擾
		for(int i=0; i<MAX_POINTMESSAGES; i++){
			if( fgets(text[i], MAX_POINTMESSAGEBYTE, fp) == NULL ){ break; }

			//'\r'�������'\n'�ɒu��������
			for(int j=0; j<MAX_POINTMESSAGEBYTE; j++){
				if( text[i][j] == '\r' ){ text[i][j] = '\n'; }
			}
		}

#ifdef ENABLE_DEBUGLOG
		//���O�ɏo��
		OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

		//�t�@�C���|�C���^���J��
		fclose( fp );
	}
	else{
#ifdef ENABLE_DEBUGLOG
		//���O�ɏo��
		OutputLog.WriteLog(LOG_CHECK, "MSG", "�t�@�C���Ȃ�");
#endif
	}

	return 0;
}

//! @brief �|�C���g�f�[�^���擾
//! @param out_data �󂯎��pointdata�^�|�C���^
//! @param id �F���ԍ�
//! @return �����F0�@���s�F0�ȊO
int PointDataInterface::Getdata(pointdata *out_data, int id)
{
	if( data == NULL ){ return 1; }
	if( (id < 0)||((datas -1) < id) ){ return 2; }

	//�f�[�^���|�C���^�ɃR�s�[
	memcpy(out_data, &(data[id]), sizeof(pointdata));

	return 0;
}

//! @brief �|�C���g�̃p�����[�^�[����������
//! @param id �F���ԍ�
//! @param p1 ��1�p�����[�^
//! @param p2 ��2�p�����[�^
//! @param p3 ��3�p�����[�^
//! @param p4 ��4�p�����[�^
//! @return �����F0�@���s�F0�ȊO
//! @warning AI���g�p����p�X�ip1:3 or p1:8�j�ȊO�ւ̎g�p�́A�ۏ؂���Ă��炸�������܂���B
int PointDataInterface::SetParam(int id, signed char p1, signed char p2, signed char p3, signed char p4)
{
	if( data == NULL ){ return 1; }
	if( (id < 0)||((datas -1) < id) ){ return 2; }

	//�p�����[�^���㏑��
	data[id].p1 = p1;
	data[id].p2 = p2;
	data[id].p3 = p3;
	data[id].p4 = p4;

	return 0;
}

//! @brief ���b�Z�[�W�i1�s�j���擾
//! @param str ��������󂯎��|�C���^
//! @param id �F���ԍ�
//! @return �����F0�@���s�F1
int PointDataInterface::GetMessageText(char *str, int id)
{
	if( (id < 0)||((MAX_POINTMESSAGES -1) < id) ){ return 1; }

	//�|�C���^�Ƀ��b�Z�[�W���R�s�[
	strcpy(str, text[id]);
	return 0;
}

//! @brief �|�C���g�f�[�^������
//! @param id �ŏ��ɊY�������f�[�^�ԍ����󂯎��|�C���^�@�iNULL �j
//! @param pmask �p�����[�^�̃}�X�N
//! @param p1 ��1�p�����[�^
//! @param p2 ��2�p�����[�^
//! @param p3 ��3�p�����[�^
//! @param p4 ��4�p�����[�^
//! @param offset �������J�n����f�[�^
//! @return �Y���|�C���g��
//! @note �p�����[�^�̃}�X�N�́Ap1�`p4�̌����Ώۂ��w�肵�܂��B
//! @note 1�`4�r�b�g�ڂ܂ł��g�p���Ap1�F1�r�b�g�ځAp2�F2�r�b�g�ځAp3�F3�r�b�g�ځAp4�F4�r�b�g�ځ@�����ꂼ��Ӗ����܂��B
//! @note �����̑Ώۃr�b�g�F1�@�ΏۊO�̃r�b�g�F0�@�ɐݒ肵�Ă��������B�@�Ȃ��A7�`5�r�b�g�ڂ͖�������܂��B
//! @code
//! //16�i���ŋL�q�̏ꍇ�\
//! 0x02	// p2�݂̂������Ώۂɂ���
//! 0x05	// p1��p3�������Ώۂɂ���
//! 0xF1	// p1�݂̂������Ώۂɂ���@�i7�`5�r�b�g�ڂ͖����j
//! @endcode
int PointDataInterface::SearchPointdata(int* id, unsigned char pmask, signed char p1, signed char p2, signed char p3, signed char p4, int offset)
{
	int cnt = 0;

	if( offset < 0 ){ offset = 0; }
	if( offset >= datas ){ offset = datas; }

	//�I�t�Z�b�g�l����f�[�^��������
	for(int i=offset; i<datas; i++){

		if( ((pmask&0x01) == 0)||(p1 == data[i].p1) ){		//�}�X�N�Ŏw�肳��Ă��Ȃ����Ap1����v
			if( ((pmask&0x02) == 0)||(p2 == data[i].p2) ){		//�}�X�N�Ŏw�肳��Ă��Ȃ����Ap2����v
				if( ((pmask&0x04) == 0)||(p3 == data[i].p3) ){		//�}�X�N�Ŏw�肳��Ă��Ȃ����Ap3����v
					if( ((pmask&0x08) == 0)||(p4 == data[i].p4) ){		//�}�X�N�Ŏw�肳��Ă��Ȃ����Ap4����v

						if( (cnt == 0)&&(id != NULL) ){			//�ŏ��ɊY�����Aid��NULL�łȂ��B
							*id = i;
						}

						//�����Y������+1
						cnt += 1;

					}
				}
			}
		}

	}

	//�����Y������Ԃ�
	return cnt;
}

//! @brief �|�C���g�f�[�^������
//! @param out_data �ŏ��ɊY�������f�[�^���󂯎��pointdata�^�|�C���^�@�iNULL �j
//! @param pmask �p�����[�^�̃}�X�N
//! @param p1 ��1�p�����[�^
//! @param p2 ��2�p�����[�^
//! @param p3 ��3�p�����[�^
//! @param p4 ��4�p�����[�^
//! @param offset �������J�n����f�[�^
//! @return �Y���|�C���g��
//! @attention ���̊֐��̏ڂ��������́A���I�[�o�[���[�h�֐����������������B
int PointDataInterface::SearchPointdata(pointdata *out_data, unsigned char pmask, signed char p1, signed char p2, signed char p3, signed char p4, int offset)
{
	int id, total;

	//�������Ń|�C���g������
	total = SearchPointdata(&id, pmask, p1, p2, p3, p4, offset);
	
	//�Y������΃f�[�^���擾
	if( total > 0 ){
		Getdata(out_data, id);
	}

	//�����Y������Ԃ�
	return total;
}

//! @brief �R���X�g���N�^
MIFInterface::MIFInterface()
{
	datas = 0;

	mif = false;
	strcpy(mission_name, "");
	strcpy(mission_fullname, "");
	strcpy(blockfile_path, "");
	strcpy(pointfile_path, "");
	skynumber = 0;
	strcpy(picturefileA_path, "!");
	strcpy(picturefileB_path, "!");
	strcpy(addsmallobject_path, "");
	strcpy(briefingtext, "");
	collision = false;
	screen = false;
	strcpy(addsmallobject_modelpath, "");
	strcpy(addsmallobject_texturepath, "");
	addsmallobject_decide = 0;
	addsmallobject_hp = 0;
	strcpy(addsmallobject_soundpath, "");
	addsmallobject_jump = 0;
}

//! @brief �f�B�X�g���N�^
MIFInterface::~MIFInterface()
{}

//! @brief �f�[�^�t�@�C����ǂ݂���
//! @param fname �t�@�C����
//! @return �����F0�@���s�F1
int MIFInterface::LoadFiledata(char *fname)
{
	char str[64];

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_LOAD, "MIF", fname);
#endif

	mif = true;

	//�g���q��.txt�Ȃ��
	//if( strcmp(PathFindExtension(fname), ".txt") == 0 ){
	//	//MIF�t���O�𖳌���
	//	mif = false;
	//}
	for(int i=strlen(fname)-1; i>0; i--){
		if( fname[i] == '.' ){
			if( strcmp(&(fname[i]), ".txt") == 0 ){
				//MIF�t���O�𖳌���
				mif = false;
			}
			break;
		}
	}

	FILE *fp;

#ifdef PATH_DELIMITER_SLASH
	//�p�X��؂蕶����ϊ�
	fname = ChangePathDelimiter(fname);
#endif

	//�t�@�C�����J��
	fp = fopen( fname, "r" );
	if( fp == NULL ){
		//briefing data open failed
		return 1;
	}

	if( mif == true ){
		//�~�b�V�������ʖ�
		fgets(mission_name, 24, fp);
		DeleteLinefeed(mission_name);

		//�~�b�V������������
		fgets(mission_fullname, 64, fp);
		DeleteLinefeed(mission_fullname);

		//�u���b�N�f�[�^�t�@�C��
		fgets(blockfile_path, _MAX_PATH, fp);
		DeleteLinefeed(blockfile_path);

		//�|�C���g�f�[�^�t�@�C��
		fgets(pointfile_path, _MAX_PATH, fp);
		DeleteLinefeed(pointfile_path);

		//�w�i��̔ԍ�
		fgets(str, 16, fp);
		DeleteLinefeed(str);
		skynumber = atoi(str);

		//�����蔻��E��ʐݒ�̎擾
		fgets(str, 16, fp);
		DeleteLinefeed(str);
		if( strcmp(str, "1") == 0 ){
			collision = true;
			screen = false;
		}
		else if( strcmp(str, "2") == 0 ){
			collision = false;
			screen = true;
		}
		else if( strcmp(str, "3") == 0 ){
			collision = true;
			screen = true;
		}
		else{	// == "0"
			collision = false;
			screen = false;
		}

		//�ǉ��������t�@�C���擾
		fgets(addsmallobject_path, _MAX_PATH, fp);
		DeleteLinefeed(addsmallobject_path);

		//�摜A���擾
		fgets(picturefileA_path, _MAX_PATH, fp);
		DeleteLinefeed(picturefileA_path);

		//�摜B���擾
		fgets(picturefileB_path, _MAX_PATH, fp);
		DeleteLinefeed(picturefileB_path);
	}
	else{
		//�摜A���擾
		fgets(str, 64, fp);
		DeleteLinefeed(str);
		if( strcmp(str, "!") == 0 ){
			strcpy(picturefileA_path, "!");
		}
		else{
			//�udata\\briefing\\�@�`�@.bmp�v�𐶐�
			strcpy(picturefileA_path, "data\\briefing\\");
			strcat(picturefileA_path, str);
			strcat(picturefileA_path, ".bmp");
		}

		//�摜B���擾
		fgets(str, 64, fp);
		DeleteLinefeed(str);
		if( strcmp(str, "!") == 0 ){
			strcpy(picturefileB_path, "!");
		}
		else{
			//�udata\\briefing\\�@�`�@.bmp�v�𐶐�
			strcpy(picturefileB_path, "data\\briefing\\");
			strcat(picturefileB_path, str);
			strcat(picturefileB_path, ".bmp");
		}

		//�w�i��̔ԍ�
		fgets(str, 16, fp);
		DeleteLinefeed(str);
		skynumber = atoi(str);

		//�擾�ł��Ȃ��l�̏�����
		strcpy(mission_name, "");
		strcpy(mission_fullname, "");
		strcpy(blockfile_path, "");
		strcpy(pointfile_path, "");
		strcpy(addsmallobject_path, "");
		collision = false;
		screen = false;
	}

	//�u���[�t�B���O�e�L�X�g���擾
	strcpy(briefingtext, "");
	for(int i=0; i<17; i++ ){
		if( fgets(str, 50, fp) == NULL ){ break; }
		strcat(briefingtext, str);
		datas += 1;
	}

	//�t�@�C���n���h�����J��
	fclose( fp );

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif


	//�ǉ��������������l��
	strcpy(addsmallobject_modelpath, "");
	strcpy(addsmallobject_texturepath, "");
	addsmallobject_decide = 0;
	addsmallobject_hp = 0;
	strcpy(addsmallobject_soundpath, "");
	addsmallobject_jump = 0;

	//��������̒ǉ��������t�@�C�����w�肳��Ă����
	if( (strcmp(addsmallobject_path, "") != 0)&&(strcmp(addsmallobject_path, "!") != 0) ){

#ifdef ENABLE_DEBUGLOG
		//���O�ɏo��
		OutputLog.WriteLog(LOG_LOAD, "�i�ǉ������j", addsmallobject_path);
#endif

#ifdef PATH_DELIMITER_SLASH
		//�p�X��؂蕶����ϊ�
		strcpy(addsmallobject_path, ChangePathDelimiter(addsmallobject_path));
#endif

		//�t�@�C�����J��
		fp = fopen( addsmallobject_path, "r" );
		if( fp != NULL ){
			//���f���f�[�^�p�X
			fgets(addsmallobject_modelpath, _MAX_PATH, fp);
			DeleteLinefeed(addsmallobject_modelpath);

			//�e�N�X�`���p�X
			fgets(addsmallobject_texturepath, _MAX_PATH, fp);
			DeleteLinefeed(addsmallobject_texturepath);

			//�����蔻��̑傫��
			fgets(str, 16, fp);
			DeleteLinefeed(str);
			addsmallobject_decide = atoi(str);

			//�ϋv��
			fgets(str, 16, fp);
			DeleteLinefeed(str);
			addsmallobject_hp = atoi(str);

			//�T�E���h�f�[�^�p�X
			fgets(addsmallobject_soundpath, _MAX_PATH, fp);
			DeleteLinefeed(addsmallobject_soundpath);

			//��ы
			fgets(str, 16, fp);
			DeleteLinefeed(str);
			addsmallobject_jump = atoi(str);

			//�t�@�C���n���h�����J��
			fclose( fp );
		}

#ifdef ENABLE_DEBUGLOG
		//���O�ɏo��
		OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
	}
#ifdef ENABLE_DEBUGLOG
	else{
		//���O�ɏo��
		OutputLog.WriteLog(LOG_CHECK, "�i�ǉ������j", "�t�@�C���Ȃ�");
	}
#endif

	return 0;
}

//! @brief �ǂݍ��񂾃f�[�^�t�@�C���̌`�����擾
//! @return �W���`���Ffalse�@MIF�`���Ftrue
//! @attention �t�@�C���𐳏�ɓǂݍ��񂾌�Ɏ��s���Ă��������B
bool MIFInterface::GetFiletype()
{
	return mif;
}

//! @brief �~�b�V�������ʖ����擾
//! @return ���ʖ��̃|�C���^�i�ő�F24�j
char* MIFInterface::GetMissionName()
{
	return mission_name;
}

//! @brief �~�b�V�����������̂��擾
//! @return �������̂̃|�C���^�i�ő�F64�j
char* MIFInterface::GetMissionFullname()
{
	return mission_fullname;
}

//! @brief �u���b�N�f�[�^�ƃ|�C���g�f�[�^�̃p�X���擾
//! @param *blockfile �u���b�N�f�[�^���󂯎��|�C���^
//! @param *pointfile �|�C���g�f�[�^���󂯎��|�C���^
void MIFInterface::GetDatafilePath(char *blockfile, char *pointfile)
{
	strcpy(blockfile, blockfile_path);
	strcpy(pointfile, pointfile_path);
}

//! @brief �w�i����擾
//! @return ��̔ԍ��i0�`5�j
//! @attention �ԍ� 0 �́u�w�i�Ȃ��v���Ӗ����܂��B
int MIFInterface::GetSkynumber()
{
	return skynumber;
}

//! @brief �u���[�t�B���O�摜�t�@�C���̃p�X���擾
//! @param *picturefileA �摜�t�@�C��A�̃p�X���󂯎��|�C���^
//! @param *picturefileB �摜�t�@�C��B�̃p�X���󂯎��|�C���^
//! @attention �ݒ肳��Ă��Ȃ��ꍇ�́u!�v���Ԃ���܂��B
//! @attention �摜��1�������g�p���Ȃ��ꍇ�A�摜�t�@�C��B�́u!�v��Ԃ��܂��B
void MIFInterface::GetPicturefilePath(char *picturefileA, char *picturefileB)
{
	strcpy(picturefileA, picturefileA_path);
	strcpy(picturefileB, picturefileB_path);
}

//! @brief �u���[�t�B���O���́i�{���j���擾
//! @return ���͂̃|�C���^�i�ő�F816�j
//! @attention ���s�R�[�h���܂߂āA�ő�17�s������x�ɕԂ���܂��B
//! @attention �s���� GetTotaldatas() �Ŏ擾�ł��܂��B
char* MIFInterface::GetBriefingText()
{
	return briefingtext;
}

//! @brief �ǉ��̂����蔻��������t���O���擾
//! @return �L���Ftrue�@�����Ffalse
bool MIFInterface::GetCollisionFlag()
{
	return collision;
}

//! @brief ��ʂ��Â��������t���O���擾
//! @return �L���Ftrue�@�����Ffalse
bool MIFInterface::GetScreenFlag()
{
	return screen;
}

//! @brief �ǉ������̃��f���f�[�^�p�X���擾
//! @return ���f���f�[�^�p�X�̃|�C���^�i�ő�F_MAX_PATH�j
char* MIFInterface::GetAddSmallobjectModelPath()
{
	return addsmallobject_modelpath;
}

//! @brief �ǉ������̃e�N�X�`���p�X���擾
//! @return �e�N�X�`���p�X�̃|�C���^�i�ő�F_MAX_PATH�j
char* MIFInterface::GetAddSmallobjectTexturePath()
{
	return addsmallobject_texturepath;
}

//! @brief �ǉ������̓����蔻��̑傫�����擾
//! @return �����蔻��̑傫��
int MIFInterface::GetAddSmallobjectDecide()
{
	return addsmallobject_decide;
}

//! @brief �ǉ������̑ϋv�͂��擾
//! @return �ϋv��
int MIFInterface::GetAddSmallobjectHP()
{
	return addsmallobject_hp;
}

//! @brief �ǉ������̃T�E���h�f�[�^�p�X���擾
//! @return �T�E���h�f�[�^�p�X�̃|�C���^�i�ő�F_MAX_PATH�j
char* MIFInterface::GetAddSmallobjectSoundPath()
{
	return addsmallobject_soundpath;
}

//! @brief �ǉ������̔�ы���擾
//! @return ��ы
int MIFInterface::GetAddSmallobjectJump()
{
	return addsmallobject_jump;
}

//! @brief �R���X�g���N�^
AddonList::AddonList()
{
	datas = 0;

	for(int i=0; i<MAX_ADDONLIST; i++){
		strcpy(filename[i], "");
		strcpy(mission_name[i], "");
	}
}

//! @brief �f�B�X�g���N�^
AddonList::~AddonList()
{}

//! @brief .mif�t�@�C�����擾
void AddonList::GetMIFlist(char *dir)
{
	char SearchDIR[_MAX_PATH];
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	//.mif�̌��������𐶐�
	strcpy(SearchDIR, dir);
	strcat(SearchDIR, "\\*.mif");

	//����
	hFind = FindFirstFile(SearchDIR, &FindFileData);
	if( hFind != INVALID_HANDLE_VALUE ){
		strcpy(filename[0], FindFileData.cFileName);
		datas += 1;

		while( FindNextFile(hFind, &FindFileData) == TRUE ){
			strcpy(filename[datas], FindFileData.cFileName);
			datas += 1;
		}
	}
	FindClose(hFind);
}

//! @brief �~�b�V���������擾
void AddonList::GetMissionName(char *dir)
{
	char str[_MAX_PATH];
	MIFInterface mifdata;

	for(int i=0; i<datas; i++){
		//�t�@�C�����𐶐�
		strcpy(str, dir);
		strcat(str, "\\");
		strcat(str, filename[i]);

		//MIFInterface�œǂݍ���
		mifdata.LoadFiledata(str);

		//�~�b�V���������擾
		strcpy(mission_name[i], mifdata.GetMissionName());
	}
}

//! @brief �~�b�V���������\�[�g����
void AddonList::Sort()
{
	char mission_name_c[MAX_ADDONLIST][24];
	char temp[_MAX_PATH];
	int cmp;

	//�~�b�V����������x�������ɕϊ�����
	for(int i=0; i<datas; i++){
		for(int j=0; j<(int)strlen(mission_name[i]); j++){
			mission_name_c[i][j] = (char)tolower(mission_name[i][j]);
		}
		mission_name_c[i][strlen(mission_name[i])] = 0x00;
	}

	//�ᑬ�ȃo�u���\�[�g�@�i�O�O�G
	for(int i=0; i<datas-1; i++){
		for(int j=i+1; j<datas; j++){
			cmp = strcmp(mission_name_c[i], mission_name_c[j]);
			if(cmp > 0){
				strcpy(temp, mission_name_c[i]);
				strcpy(mission_name_c[i], mission_name_c[j]);
				strcpy(mission_name_c[j], temp);

				strcpy(temp, mission_name[i]);
				strcpy(mission_name[i], mission_name[j]);
				strcpy(mission_name[j], temp);

				strcpy(temp, filename[i]);
				strcpy(filename[i], filename[j]);
				strcpy(filename[j], temp);
			}
		}
	}

}

//! @brief ADDON���X�g���擾����
//! @param dir ADDON�i.mif�j���������f�B���N�g���@�i�W���F"addon\\"�j
//! @return addon�̑���
int AddonList::LoadFiledata(char *dir)
{
	datas = 0;

	//.mif�t�@�C�����擾
	GetMIFlist(dir);

	//�~�b�V���������擾
	GetMissionName(dir);

	//�~�b�V���������\�[�g����
	Sort();

#ifdef ENABLE_DEBUGLOG
	char str[128];
	sprintf(str, "addon�t�H���_�����i�f�B���N�g���F%s�@���v���F%d�j", dir, datas);

	//���O�ɏo��
	OutputLog.WriteLog(LOG_CHECK, "�f�B���N�g��", str);
#endif

	return datas;
}

//! @brief �~�b�V���������擾
//! @param id �F���ԍ�
//! @return �~�b�V������
char* AddonList::GetMissionName(int id)
{
	return mission_name[id];
}

//! @brief �t�@�C�������擾
//! @param id �F���ԍ�
//! @return �t�@�C����
char* AddonList::GetFileName(int id)
{
	return filename[id];
}


//! @brief fgets()�p�@���s�R�[�h����菜��
//! @param str ������
//! @return �����F0�@���s�F1
int DeleteLinefeed(char str[])
{
	char *pstr;

	pstr = strchr(str, '\n');
	if( pstr ){
		*pstr = '\0';
		return 0;
	}
	return 1;
}
