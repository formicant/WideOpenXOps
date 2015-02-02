//! @file collision.cpp
//! @brief Collision�N���X�̒�`

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

#include "collision.h"

//! �R���X�g���N�^
Collision::Collision()
{
	blockdata = NULL;
	bmin_x = new float[MAX_BLOCKS];
	bmin_y = new float[MAX_BLOCKS];
	bmin_z = new float[MAX_BLOCKS];
	bmax_x = new float[MAX_BLOCKS];
	bmax_y = new float[MAX_BLOCKS];
	bmax_z = new float[MAX_BLOCKS];
	BoardBlock = new bool[MAX_BLOCKS];
	bdata_worldgroup = new int[MAX_BLOCKS];
	for(int i=0; i<MAX_BLOCKS; i++){
		bmin_x[i] = 0.0f;
		bmin_y[i] = 0.0f;
		bmin_z[i] = 0.0f;
		bmax_x[i] = 0.0f;
		bmax_y[i] = 0.0f;
		bmax_z[i] = 0.0f;
		BoardBlock[i] = false;
		bdata_worldgroup[i] = 0;
	}
}

//! �f�B�X�g���N�^
Collision::~Collision()
{
	if( bmin_x != NULL ){ delete [] bmin_x; }
	if( bmin_y != NULL ){ delete [] bmin_y; }
	if( bmin_z != NULL ){ delete [] bmin_z; }
	if( bmax_x != NULL ){ delete [] bmax_x; }
	if( bmax_y != NULL ){ delete [] bmax_y; }
	if( bmax_z != NULL ){ delete [] bmax_z; }
	if( BoardBlock != NULL ){ delete [] BoardBlock; }
	if( bdata_worldgroup != NULL ){ delete [] bdata_worldgroup; }
}

//! �u���b�N�f�[�^����荞��
//! @param in_blockdata �u���b�N�f�[�^
int Collision::InitCollision(BlockDataInterface* in_blockdata)
{
	int bs;
	struct blockdata data;
	int vID[4];
	float g0, g1, costheta;

	if( in_blockdata == NULL ){ return 1; }
	blockdata = in_blockdata;

	bs = blockdata->GetTotaldatas();

	for(int i=0; i<bs; i++){
		BoardBlock[i] = false;
	}

	//����p���ʍ쐬
	for(int i=0; i<bs; i++){
		blockdata->Getdata(&data, i);
		for(int j=0; j<6; j++){
			blockdataface(j, &vID[0], NULL);

			D3DXPlaneFromPoints(&bdata_plane[i][j][0],
				&D3DXVECTOR3( data.x[ vID[1] ], data.y[ vID[1] ], data.z[ vID[1] ] ),
				&D3DXVECTOR3( data.x[ vID[2] ], data.y[ vID[2] ], data.z[ vID[2] ] ),
				&D3DXVECTOR3( data.x[ vID[0] ], data.y[ vID[0] ], data.z[ vID[0] ] ) );
			D3DXPlaneFromPoints(&bdata_plane[i][j][1],
				&D3DXVECTOR3( data.x[ vID[0] ], data.y[ vID[0] ], data.z[ vID[0] ] ),
				&D3DXVECTOR3( data.x[ vID[2] ], data.y[ vID[2] ], data.z[ vID[2] ] ),
				&D3DXVECTOR3( data.x[ vID[3] ], data.y[ vID[3] ], data.z[ vID[3] ] ) );

			//2�̎O�p�`�����@���̂Ȃ��p�����߂�
			g0 = sqrt(bdata_plane[i][j][0].a * bdata_plane[i][j][0].a + bdata_plane[i][j][0].b * bdata_plane[i][j][0].b + bdata_plane[i][j][0].c * bdata_plane[i][j][0].c);
			g1 = sqrt(bdata_plane[i][j][1].a * bdata_plane[i][j][1].a + bdata_plane[i][j][1].b * bdata_plane[i][j][1].b + bdata_plane[i][j][1].c * bdata_plane[i][j][1].c);
			costheta = (bdata_plane[i][j][0].a * bdata_plane[i][j][1].a + bdata_plane[i][j][0].b * bdata_plane[i][j][1].b + bdata_plane[i][j][0].c * bdata_plane[i][j][1].c) / (g0 * g1);

			//1�̖ʂŖ@����90�x�ȏ�Ⴄ�i���ُ�j�Ȃ�`
			if( acos(costheta) > (float)M_PI/2 ){
				//�Ⴄ�O�p�`�ō��
				D3DXPlaneFromPoints(&bdata_plane[i][j][0],
					&D3DXVECTOR3( data.x[ vID[2] ], data.y[ vID[2] ], data.z[ vID[2] ] ),
					&D3DXVECTOR3( data.x[ vID[3] ], data.y[ vID[3] ], data.z[ vID[3] ] ),
					&D3DXVECTOR3( data.x[ vID[1] ], data.y[ vID[1] ], data.z[ vID[1] ] ) );
				D3DXPlaneFromPoints(&bdata_plane[i][j][1],
					&D3DXVECTOR3( data.x[ vID[1] ], data.y[ vID[1] ], data.z[ vID[1] ] ),
					&D3DXVECTOR3( data.x[ vID[3] ], data.y[ vID[3] ], data.z[ vID[3] ] ),
					&D3DXVECTOR3( data.x[ vID[0] ], data.y[ vID[0] ], data.z[ vID[0] ] ) );
			}
		}
	}

	//��̃u���b�N�����o
	for(int i=0; i<bs; i++){
		blockdata->Getdata(&data, i);

		BoardBlock[i] = false;

		for(int j=0; j<8; j++){
			for(int k=j+1; k<8; k++){
				//�ꃕ���ł����_�������Ȃ�A��ɂȂ��Ă���Ɣ���B
				if( (data.x[j] == data.x[k])&&(data.y[j] == data.y[k])&&(data.z[j] == data.z[k]) ){
					BoardBlock[i] = true;
				}
			}
		}
	}


	//�u���b�NAABB�쐬
	for(int i=0; i<bs; i++){
		blockdata->Getdata(&data, i);
		GetBlockPosMINMAX(data, &bmin_x[i], &bmin_y[i], &bmin_z[i], &bmax_x[i], &bmax_y[i], &bmax_z[i]);
	}

	//�u���b�N�̋�ԕ����O���[�v���v�Z
	for(int i=0; i<bs; i++){
		///*
		blockdata->Getdata(&data, i);

		bdata_worldgroup[i] = GetWorldGroup(bmin_x[i], bmin_z[i]);
		if( GetWorldGroup(bmax_x[i], bmax_z[i]) != bdata_worldgroup[i] ){
			bdata_worldgroup[i] = 0;
		}
		//*/
		//bdata_worldgroup[i] = 0;
	}

	return 0;
}

//! �u���b�N�̍��W�ő�l�E�ŏ��l��Ԃ�
//! @param data �u���b�N�f�[�^�̍\����
//! @param *min_x �ŏ� X���W��Ԃ��|�C���^
//! @param *min_y �ŏ� Y���W��Ԃ��|�C���^
//! @param *min_z �ŏ� Z���W��Ԃ��|�C���^
//! @param *max_x �ő� X���W��Ԃ��|�C���^
//! @param *max_y �ő� Y���W��Ԃ��|�C���^
//! @param *max_z �ő� Z���W��Ԃ��|�C���^
void Collision::GetBlockPosMINMAX(struct blockdata data, float *min_x, float *min_y, float *min_z, float *max_x, float *max_y, float *max_z)
{
	*min_x = data.x[0];
	*min_y = data.y[0];
	*min_z = data.z[0];
	*max_x = data.x[0];
	*max_y = data.y[0];
	*max_z = data.z[0];
	for(int i=1; i<8; i++){
		if( *min_x > data.x[i] ){ *min_x = data.x[i]; }
		if( *min_y > data.y[i] ){ *min_y = data.y[i]; }
		if( *min_z > data.z[i] ){ *min_z = data.z[i]; }

		if( *max_x < data.x[i] ){ *max_x = data.x[i]; }
		if( *max_y < data.y[i] ){ *max_y = data.y[i]; }
		if( *max_z < data.z[i] ){ *max_z = data.z[i]; }
	}
}

//! ��ԕ����̃O���[�v���Z�o
//! @param x X���W
//! @param z Z���W
//! @return �O���[�v�ԍ�
//! @attention �u�O���[�v�ԍ��v�̊T�O�͕ʓr�h�L�������g���Q��
int Collision::GetWorldGroup(float x, float z)
{
	if( (x > 0)&&(z > 0) ){ return 1; }
	if( (x < 0)&&(z > 0) ){ return 2; }
	if( (x < 0)&&(z < 0) ){ return 3; }
	if( (x > 0)&&(z < 0) ){ return 4; }
	return 0;
}

//! �u���b�N�ɖ��܂��Ă��Ȃ������ׂ�
//! @param blockid ���肷��u���b�N�ԍ�
//! @param x X���W
//! @param y X���W
//! @param z X���W
//! @param worldgroup ��Ԃ̃O���[�v�𗘗p���Čv�Z�ȗ������݂�itrue�F�L���E�v�Z�ȗ��@false�F�����E���S�����j
//! @param *planeid �\�ɂ���ʔԍ��iNULL�j
//! @return �����Ă���Ftrue�@�����Ă��Ȃ��Ffalse
//! @warning *planeid ���Ԃ��\�ʁi0�`5�j�́A�����̖ʂ��Y������ꍇ�ł��A�ŏ��Ɍ�����1�ʂ̂ݕԂ��܂��B
bool Collision::CheckBlockInside(int blockid, float x, float y, float z, bool worldgroup, int *planeid)
{
	if( blockdata == NULL ){ return false; }
	if( (blockid < 0)||(blockdata->GetTotaldatas() <= blockid) ){ return false; }

	//��̃u���b�N�͌v�Z�O
	if( BoardBlock[blockid] == true ){ return false; }


	//����̍r���
	if( worldgroup == true ){

		//��ԕ���
		if( bdata_worldgroup[blockid] != 0 ){
			//�ϑ��_�̋�Ԃ̃O���[�v���擾
			int worldgroup = GetWorldGroup(x, z);

			if( worldgroup != 0 ){
				//��Ԃ̃O���[�v���Ⴆ�Όv�Z�O
				if( bdata_worldgroup[blockid] != worldgroup ){
					return false;
				}
			}
		}

		//�͈͂Ō���
		if( (x < bmin_x[blockid])||(bmax_x[blockid] < x) ){ return false; }
		if( (y < bmin_y[blockid])||(bmax_y[blockid] < y) ){ return false; }
		if( (z < bmin_z[blockid])||(bmax_z[blockid] < z) ){ return false; }
	}


	//6�ʂ��猩�đS�ė��ʂ��ǂ���
	for(int i=0; i<6; i++){
		if( (D3DXPlaneDotCoord(&bdata_plane[blockid][i][0], &D3DXVECTOR3( x, y, z )) > 0)||(D3DXPlaneDotCoord(&bdata_plane[blockid][i][1], &D3DXVECTOR3( x, y, z )) > 0) ){
			if( planeid != NULL ){ *planeid = i; }
			return false;	//�\�ʂȂ�ΏI��
		}
	}

	return true;
}

//! �S�Ẵu���b�N�ɖ��܂��Ă��Ȃ������ׂ�
//! @param x X���W
//! @param y X���W
//! @param z X���W
//! @return �����Ă���Ftrue�@�����Ă��Ȃ��Ffalse
bool Collision::CheckALLBlockInside(float x, float y, float z)
{
	if( blockdata == NULL ){ return false; }

	int bs = blockdata->GetTotaldatas();

	for(int i=0; i<bs; i++){
		if( CheckBlockInside(i, x, y, z, true, NULL) == true ){ return true; }
	}

	return false;
}

//! �S�Ẵu���b�N�ƃ��C�i�����j�̂����蔻��
//! @param blockid ���肷��u���b�N�ԍ�
//! @param RayPos_x ���C�̈ʒu�i�n�_�j���w�肷�� X���W
//! @param RayPos_y ���C�̈ʒu�i�n�_�j���w�肷�� Y���W
//! @param RayPos_z ���C�̈ʒu�i�n�_�j���w�肷�� Z���W
//! @param RayDir_x ���C�̃x�N�g�����w�肷�� X����
//! @param RayDir_y ���C�̃x�N�g�����w�肷�� Y����
//! @param RayDir_z ���C�̃x�N�g�����w�肷�� Z����
//! @param face ���������u���b�N�̖ʔԍ��i0�`5�j���󂯎��|�C���^�@�iNULL�j
//! @param Dist ���������u���b�N�Ƃ̋������󂯎��|�C���^
//! @param maxDist ������s���ő勗���@�i0.0 �����Ŗ����E�����j
//! @return �������Ă���Ftrue�@�������Ă��Ȃ��Ffalse
//! @warning RayPos�i�n�_�j�� RayDir�i�x�N�g���j���ԈႦ�Ȃ����ƁB
//! @warning ������s���ő勗�����w�肵�Ȃ��ƁA�p�t�H�[�}���X���啝�ɒቺ���܂��B
//! @attention ���C�̎n�_���痠���ɂȂ�u���b�N�̖ʂ͖�������܂��B�������[���ɕό`��������̃u���b�N���������܂��B
//! @attention �܂��A���C�������̃u���b�N�ɓ�����ꍇ�́A���C�̎n�_�����ԋ߂������Ԃ��܂��B
bool Collision::CheckBlockIntersectRay(int blockid, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *face, float *Dist, float maxDist)
{
	if( blockdata == NULL ){ return false; }

	int bs = blockdata->GetTotaldatas();
	struct blockdata data;
	int vID[4];
	float pDist;
	float min_pDist = FLT_MAX;
	int min_blockface = -1;
	float rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z;
	int worldgroupA, worldgroupB;

	//��̃u���b�N�͌v�Z�O
	if( BoardBlock[blockid] == true ){
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	if( maxDist > 0.0f ){
		//���C��AABB�����
		rmin_x = RayPos_x + RayDir_x * maxDist;
		rmin_y = RayPos_y + RayDir_y * maxDist;
		rmin_z = RayPos_z + RayDir_z * maxDist;
		rmax_x = rmin_x;
		rmax_y = rmin_y;
		rmax_z = rmin_z;
		if( rmin_x > RayPos_x ){ rmin_x = RayPos_x; }
		if( rmin_y > RayPos_y ){ rmin_y = RayPos_y; }
		if( rmin_z > RayPos_z ){ rmin_z = RayPos_z; }
		if( rmax_x < RayPos_x ){ rmax_x = RayPos_x; }
		if( rmax_y < RayPos_y ){ rmax_y = RayPos_y; }
		if( rmax_z < RayPos_z ){ rmax_z = RayPos_z; }
	}

	D3DXVECTOR3 pRayPos(RayPos_x, RayPos_y, RayPos_z);
	D3DXVECTOR3 pRayDir(RayDir_x, RayDir_y, RayDir_z);

	if( maxDist > 0.0f ){
		//�n�_�ƏI�_�̋�ԃO���[�v���擾
		worldgroupA = GetWorldGroup(RayPos_x, RayPos_z);
		worldgroupB = GetWorldGroup(RayPos_x + RayDir_x * maxDist, RayPos_z + RayDir_z * maxDist);
	}

	blockdata->Getdata(&data, blockid);

	if( maxDist > 0.0f ){
		if( bdata_worldgroup[blockid] != 0 ){
			//�n�_�ƏI�_���A��Ԃ̃O���[�v����o�Ă��Ȃ����
			if( (worldgroupA != 0)&&(worldgroupA == worldgroupB) ){

				//��Ԃ̃O���[�v���Ⴆ�Όv�Z�O
				if( bdata_worldgroup[blockid] == worldgroupA ){		//worldgroupA == worldgroupB
					//���E�{�b�N�X���m�Ŕ���
					if( CollideBoxAABB(bmin_x[blockid], bmin_y[blockid], bmin_z[blockid], bmax_x[blockid], bmax_y[blockid], bmax_z[blockid], rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z) == false ){
						//�������ĂȂ���΁A���̃u���b�N�͒��ׂȂ��B
						if( face != NULL ){ *face = 0; }
						*Dist = 0.0f;
						return false;
					}
				}
			}
		}
	}

	//���E�{�b�N�X�ƃ��C�Ŕ���
	if( CollideAABBRay(bmin_x[blockid], bmin_y[blockid], bmin_z[blockid], bmax_x[blockid], bmax_y[blockid], bmax_z[blockid], RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, NULL, -1.0f) == false ){
		//�������ĂȂ���΁A���̃u���b�N�͒��ׂȂ��B
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//�e�|���S���P�ʂŔ���
	for(int i=0; i<6; i++){
		blockdataface(i, vID, NULL);
		if( (D3DXPlaneDotCoord(&bdata_plane[blockid][i][0], &pRayPos) >= 0)||(D3DXPlaneDotCoord(&bdata_plane[blockid][i][1], &pRayPos) >= 0) ){
			if( (D3DXIntersectTri(
				&D3DXVECTOR3( data.x[vID[0]], data.y[vID[0]], data.z[vID[0]]),
				&D3DXVECTOR3( data.x[vID[1]], data.y[vID[1]], data.z[vID[1]]),
				&D3DXVECTOR3( data.x[vID[2]], data.y[vID[2]], data.z[vID[2]]), 
				&pRayPos, &pRayDir, NULL,  NULL, &pDist) == TRUE)||
				(D3DXIntersectTri(
				&D3DXVECTOR3( data.x[vID[2]], data.y[vID[2]], data.z[vID[2]]),
				&D3DXVECTOR3( data.x[vID[3]], data.y[vID[3]], data.z[vID[3]]),
				&D3DXVECTOR3( data.x[vID[0]], data.y[vID[0]], data.z[vID[0]]), 
				&pRayPos, &pRayDir, NULL,  NULL, &pDist) == TRUE)
			){
				if( min_pDist > pDist ){
					min_pDist = pDist;
					min_blockface = i;
				}
			}
		}
	}

	//������Ȃ���΁A�|�C���^�ɓK���Ȑ��������ĕԂ��B
	if( min_blockface == -1 ){
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//�v�Z���ʂ����ĕԂ�
	if( face != NULL ){ *face = min_blockface; }
	*Dist = min_pDist;
	return true;
}

//! �S�Ẵu���b�N�ƃ��C�i�����j�̂����蔻��
//! @param RayPos_x ���C�̈ʒu�i�n�_�j���w�肷�� X���W
//! @param RayPos_y ���C�̈ʒu�i�n�_�j���w�肷�� Y���W
//! @param RayPos_z ���C�̈ʒu�i�n�_�j���w�肷�� Z���W
//! @param RayDir_x ���C�̃x�N�g�����w�肷�� X����
//! @param RayDir_y ���C�̃x�N�g�����w�肷�� Y����
//! @param RayDir_z ���C�̃x�N�g�����w�肷�� Z����
//! @param id ���������u���b�N��ID���󂯎��|�C���^�@�iNULL�j
//! @param face ���������u���b�N�̖ʔԍ��i0�`5�j���󂯎��|�C���^�@�iNULL�j
//! @param Dist ���������u���b�N�Ƃ̋������󂯎��|�C���^
//! @param maxDist ������s���ő勗���@�i0.0 �����Ŗ����E�����j
//! @return �������Ă���Ftrue�@�������Ă��Ȃ��Ffalse
//! @warning RayPos�i�n�_�j�� RayDir�i�x�N�g���j���ԈႦ�Ȃ����ƁB
//! @warning ������s���ő勗�����w�肵�Ȃ��ƁA�p�t�H�[�}���X���啝�ɒቺ���܂��B
//! @warning �g������ CheckALLBlockIntersectDummyRay()�֐� �Ɨގ����Ă��܂����A���֐���荂���x�Œᑬ�ł��B
//! @attention ���C�̎n�_���痠���ɂȂ�u���b�N�̖ʂ͖�������܂��B�������[���ɕό`��������̃u���b�N���������܂��B
//! @attention �܂��A���C�������̃u���b�N�ɓ�����ꍇ�́A<b>���C�̎n�_�����ԋ߂�����</b>��Ԃ��܂��B
bool Collision::CheckALLBlockIntersectRay(float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *id, int *face, float *Dist, float maxDist)
{
	if( blockdata == NULL ){ return false; }

	int bs = blockdata->GetTotaldatas();
	struct blockdata data;
	int vID[4];
	float pDist;
	float min_pDist = FLT_MAX;
	int min_blockid = -1;
	int min_blockface = -1;
	float rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z;
	int worldgroupA, worldgroupB;

	if( maxDist > 0.0f ){
		//���C��AABB�����
		rmin_x = RayPos_x + RayDir_x * maxDist;
		rmin_y = RayPos_y + RayDir_y * maxDist;
		rmin_z = RayPos_z + RayDir_z * maxDist;
		rmax_x = rmin_x;
		rmax_y = rmin_y;
		rmax_z = rmin_z;
		if( rmin_x > RayPos_x ){ rmin_x = RayPos_x; }
		if( rmin_y > RayPos_y ){ rmin_y = RayPos_y; }
		if( rmin_z > RayPos_z ){ rmin_z = RayPos_z; }
		if( rmax_x < RayPos_x ){ rmax_x = RayPos_x; }
		if( rmax_y < RayPos_y ){ rmax_y = RayPos_y; }
		if( rmax_z < RayPos_z ){ rmax_z = RayPos_z; }
	}

	D3DXVECTOR3 pRayPos(RayPos_x, RayPos_y, RayPos_z);
	D3DXVECTOR3 pRayDir(RayDir_x, RayDir_y, RayDir_z);

	if( maxDist > 0.0f ){
		//�n�_�ƏI�_�̋�ԃO���[�v���擾
		worldgroupA = GetWorldGroup(RayPos_x, RayPos_z);
		worldgroupB = GetWorldGroup(RayPos_x + RayDir_x * maxDist, RayPos_z + RayDir_z * maxDist);
	}
	
	for(int i=0; i<bs; i++){
		//��̃u���b�N�͌v�Z�O
		if( BoardBlock[i] == true ){ continue; }

		blockdata->Getdata(&data, i);

		if( maxDist > 0.0f ){
			if( bdata_worldgroup[i] != 0 ){
				//�n�_�ƏI�_���A��Ԃ̃O���[�v����o�Ă��Ȃ����
				if( (worldgroupA != 0)&&(worldgroupA == worldgroupB) ){

					//��Ԃ̃O���[�v���Ⴆ�Όv�Z�O
					if( bdata_worldgroup[i] == worldgroupA ){		//worldgroupA == worldgroupB

						//���E�{�b�N�X���m�Ŕ���
						if( CollideBoxAABB(bmin_x[i], bmin_y[i], bmin_z[i], bmax_x[i], bmax_y[i], bmax_z[i], rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z) == false ){
							continue;	//�������ĂȂ���΁A���̃u���b�N�͒��ׂȂ��B
						}

					}
				}
			}
		}

		//���E�{�b�N�X�ƃ��C�Ŕ���
		if( CollideAABBRay(bmin_x[i], bmin_y[i], bmin_z[i], bmax_x[i], bmax_y[i], bmax_z[i], RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, NULL, -1.0f) == false ){
			continue;		//�������ĂȂ���΁A���̃u���b�N�͒��ׂȂ��B
		}

		//�e�|���S���P�ʂŔ���
		for(int j=0; j<6; j++){
			blockdataface(j, vID, NULL);
			if( (D3DXPlaneDotCoord(&bdata_plane[i][j][0], &pRayPos) >= 0)||(D3DXPlaneDotCoord(&bdata_plane[i][j][1], &pRayPos) >= 0) ){
				if( (D3DXIntersectTri(
					&D3DXVECTOR3( data.x[vID[0]], data.y[vID[0]], data.z[vID[0]]),
					&D3DXVECTOR3( data.x[vID[1]], data.y[vID[1]], data.z[vID[1]]),
					&D3DXVECTOR3( data.x[vID[2]], data.y[vID[2]], data.z[vID[2]]), 
					&pRayPos, &pRayDir, NULL,  NULL, &pDist) == TRUE)||
					(D3DXIntersectTri(
					&D3DXVECTOR3( data.x[vID[2]], data.y[vID[2]], data.z[vID[2]]),
					&D3DXVECTOR3( data.x[vID[3]], data.y[vID[3]], data.z[vID[3]]),
					&D3DXVECTOR3( data.x[vID[0]], data.y[vID[0]], data.z[vID[0]]), 
					&pRayPos, &pRayDir, NULL,  NULL, &pDist) == TRUE)
				){
					if( min_pDist > pDist ){
						min_pDist = pDist;
						min_blockid = i;
						min_blockface = j;
					}
				}
			}
		}
	}

	//�������������ő勗���𒴂��Ă���΁A����𖳌��ɁB
	if( maxDist >= 0.0f ){
		if( min_pDist > maxDist ){
			min_blockid = -1;
		}
	}

	//������Ȃ���΁A�|�C���^�ɓK���Ȑ��������ĕԂ��B
	if( min_blockid == -1 ){
		if( id != NULL ){ *id = 0; }
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//�v�Z���ʂ����ĕԂ�
	if( id != NULL ){ *id = min_blockid; }
	if( face != NULL ){ *face = min_blockface; }
	*Dist = min_pDist;
	return true;
}

//! �S�Ẵu���b�N�ƏՓ˂��Ă��邩����
//! @param RayPos_x �n�_ X���W
//! @param RayPos_y �n�_ Y���W
//! @param RayPos_z �n�_ Z���W
//! @param RayDir_x �x�N�g�� X����
//! @param RayDir_y �x�N�g�� Y����
//! @param RayDir_z �x�N�g�� Z����
//! @param id ���������u���b�N��ID���󂯎��|�C���^�@�iNULL�j
//! @param face ���������u���b�N�̖ʔԍ��i0�`5�j���󂯎��|�C���^�@�iNULL�j
//! @param Dist �_�~�[�ϐ��̃|�C���^�i��� 0.0f ��Ԃ��j
//! @param maxDist ������s���ő勗���@�i0.0 �����w�� �s�j
//! @return �������Ă���Ftrue�@�������Ă��Ȃ��Ffalse
//! @warning �n�_�� �x�N�g�����ԈႦ�Ȃ����ƁB
//! @warning �g������ CheckALLBlockIntersectRay()�֐� �Ɨގ����Ă��܂����A���֐���荂���Œᐸ�x�ł��B
//! @attention ���C�̎n�_���痠���ɂȂ�u���b�N�̖ʂ͖�������܂��B�������[���ɕό`��������̃u���b�N���������܂��B
//! @attention �܂��A���C�������̃u���b�N�ɓ�����ꍇ�́A<b>��ԍŏ��ɔ�����������</b>��Ԃ��܂��B
bool Collision::CheckALLBlockIntersectDummyRay(float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *id, int *face, float *Dist, float maxDist)
{
	if( blockdata == NULL ){ return false; }
	if( maxDist <= 0.0f ){ return false; }

	float RayPos2_x = RayPos_x + RayDir_x * maxDist/2;
	float RayPos2_y = RayPos_y + RayDir_y * maxDist/2;
	float RayPos2_z = RayPos_z + RayDir_z * maxDist/2;
	float RayPos3_x = RayPos_x + RayDir_x * maxDist;
	float RayPos3_y = RayPos_y + RayDir_y * maxDist;
	float RayPos3_z = RayPos_z + RayDir_z * maxDist;
	
	if( (id == NULL)&&(face == NULL) ){
		for(int i=0; i<MAX_BLOCKS; i++){
			//�I�����_
			if( CheckBlockInside(i, RayPos3_x, RayPos3_y, RayPos3_z, true, NULL) == true ){
				*Dist = 0.0f;
				return true;
			}

			//���Ԏ��_
			if( CheckBlockInside(i, RayPos2_x, RayPos2_y, RayPos2_z, true, NULL) == true ){
				*Dist = 0.0f;
				return true;
			}
		}

		*Dist = 0.0f;
		return false;
	}

	for(int i=0; i<MAX_BLOCKS; i++){
		int surface;

		//�J�n�n�_
		CheckBlockInside(i, RayPos_x, RayPos_y, RayPos_z, false, &surface);

		//�I�����_
		if( CheckBlockInside(i, RayPos3_x, RayPos3_y, RayPos3_z, true, &surface) == true ){
			if( id != NULL ){ *id = i; }
			if( face != NULL ){ *face = surface; }
			*Dist = 0.0f;
			return true;
		}

		//���Ԏ��_
		if( CheckBlockInside(i, RayPos2_x, RayPos2_y, RayPos2_z, true, &surface) == true ){
			if( id != NULL ){ *id = i; }
			if( face != NULL ){ *face = surface; }
			*Dist = 0.0f;
			return true;
		}
	}

	if( id != NULL ){ *id = 0; }
	if( face != NULL ){ *face = 0; }
	*Dist = 0.0f;
	return false;
}

//! �u���b�N�ɉ����Ĉړ�����x�N�g�������߂�
void Collision::ScratchVector(BlockDataInterface* in_blockdata, int id, int face, float in_vx, float in_vy, float in_vz, float *out_vx, float *out_vy, float *out_vz)
{
	struct blockdata bdata;
	in_blockdata->Getdata(&bdata, id);

	D3DXVECTOR3 out;
	D3DXVECTOR3 front(in_vx, in_vy, in_vz);
	D3DXVECTOR3 normal(bdata.material[face].vx, bdata.material[face].vy, bdata.material[face].vz);

	//D3DXVec3Normalize(&out, &(front - D3DXVec3Dot(&front, &normal) * normal));
	out = (front - D3DXVec3Dot(&front, &normal) * normal);

	*out_vx = out.x;
	*out_vy = out.y;
	*out_vz = out.z;
}

//! �u���b�N�ɔ��˂���x�N�g�������߂�
void Collision::ReflectVector(BlockDataInterface* in_blockdata, int id, int face, float in_vx, float in_vy, float in_vz, float *out_vx, float *out_vy, float *out_vz)
{
	struct blockdata bdata;
	in_blockdata->Getdata(&bdata, id);

	D3DXVECTOR3 out;
	D3DXVECTOR3 front(in_vx, in_vy, in_vz);
	D3DXVECTOR3 normal(bdata.material[face].vx, bdata.material[face].vy, bdata.material[face].vz);

	//D3DXVec3Normalize(&out, &(front - 2.0f * D3DXVec3Dot(&front, &normal) * normal));
	out = (front - 2.0f * D3DXVec3Dot(&front, &normal) * normal);

	*out_vx = out.x;
	*out_vy = out.y;
	*out_vz = out.z;
}


//! AABB�ɂ�邠���蔻��
//! @param box1_min_x ����A�̍ŏ� X���W
//! @param box1_min_y ����A�̍ŏ� Y���W
//! @param box1_min_z ����A�̍ŏ� Z���W
//! @param box1_max_x ����A�̍ő� X���W
//! @param box1_max_y ����A�̍ő� Y���W
//! @param box1_max_z ����A�̍ő� Z���W
//! @param box2_min_x ����B�̍ŏ� X���W
//! @param box2_min_y ����B�̍ŏ� Y���W
//! @param box2_min_z ����B�̍ŏ� Z���W
//! @param box2_max_x ����B�̍ő� X���W
//! @param box2_max_y ����B�̍ő� Y���W
//! @param box2_max_z ����B�̍ő� Z���W
//! @return �������Ă���Ftrue�@�������Ă��Ȃ��Ffalse
//! @attention �G���[������ꍇ�u�������Ă��Ȃ��Ffalse�v���Ԃ���܂��B
bool CollideBoxAABB(float box1_min_x, float box1_min_y, float box1_min_z, float box1_max_x, float box1_max_y, float box1_max_z, float box2_min_x, float box2_min_y, float box2_min_z, float box2_max_x, float box2_max_y, float box2_max_z)
{
	//�G���[�΍�
	if( box1_min_x > box1_max_x ){ return false; }
	if( box1_min_y > box1_max_y ){ return false; }
	if( box1_min_z > box1_max_z ){ return false; }
	if( box2_min_x > box2_max_x ){ return false; }
	if( box2_min_y > box2_max_y ){ return false; }
	if( box2_min_z > box2_max_z ){ return false; }

	if(
		(box1_min_x < box2_max_x)&&(box1_max_x > box2_min_x)&&
		(box1_min_y < box2_max_y)&&(box1_max_y > box2_min_y)&&
		(box1_min_z < box2_max_z)&&(box1_max_z > box2_min_z)
	){
		return true;
	}

	return false;
}

//! �~�����m�̓����蔻��
//! @param c1_x �~��1 ��ӂ�x���W
//! @param c1_y �~��1 ��ӂ�y���W
//! @param c1_z �~��1 ��ӂ�z���W
//! @param c1_r �~��1 �̔��a
//! @param c1_h �~��1 �̍���
//! @param c2_x �~��2 ��ӂ�x���W
//! @param c2_y �~��2 ��ӂ�y���W
//! @param c2_z �~��2 ��ӂ�z���W
//! @param c2_r �~��2 �̔��a
//! @param c2_h �~��2 �̍���
//! @param *angle �~��1����݂��ڐG�p�x���󂯎��|�C���^�iNULL�j
//! @param *length �~��1����݂��ڐG�������󂯎��|�C���^�iNULL�j
//! @return �������Ă���Ftrue�@�������Ă��Ȃ��Ffalse
bool CollideCylinder(float c1_x, float c1_y, float c1_z, float c1_r, float c1_h, float c2_x, float c2_y, float c2_z, float c2_r, float c2_h, float *angle, float *length)
{
	//��ɂ���y�ȍ����Ŕ���
	if( (c1_y < c2_y + c2_h)&&(c1_y + c1_h > c2_y) ){

		//�����Ŕ���
		float x = c1_x - c2_x;
		float z = c1_z - c2_z;
		float caser = x*x + z*z;
		float minr = (c1_r+c2_r) * (c1_r+c2_r);
		if( caser < minr ){
			if( angle != NULL ){ *angle = atan2(z, x); }
			if( length != NULL ){ *length = sqrt(minr) - sqrt(caser); }
			return true;
		}
	}

	return false;
}

//! ���̂ƃ��C�i�����j�̂����蔻��
//! @param s_x ���̂�x���W
//! @param s_y ���̂�y���W
//! @param s_z ���̂�z���W
//! @param s_r ���̂̔��a
//! @param RayPos_x ���C�̈ʒu�i�n�_�j���w�肷�� X���W
//! @param RayPos_y ���C�̈ʒu�i�n�_�j���w�肷�� Y���W
//! @param RayPos_z ���C�̈ʒu�i�n�_�j���w�肷�� Z���W
//! @param RayDir_x ���C�̃x�N�g�����w�肷�� X����
//! @param RayDir_y ���C�̃x�N�g�����w�肷�� Y����
//! @param RayDir_z ���C�̃x�N�g�����w�肷�� Z����
//! @param Dist �����������̂Ƃ̋������󂯎��|�C���^
//! @param maxDist ������s���ő勗���@�i0.0 �����Ŗ����E�����j
//! @return �������Ă���Ftrue�@�������Ă��Ȃ��Ffalse
//! @warning RayPos�i�n�_�j�� RayDir�i�x�N�g���j���ԈႦ�Ȃ����ƁB
//! @warning ������s���ő勗�����w�肵�Ȃ��ƁA�p�t�H�[�}���X���啝�ɒቺ���܂��B
bool CollideSphereRay(float s_x, float s_y, float s_z, float s_r, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist)
{
	if( maxDist > 0.0f ){
		float pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z;

		//���C��AABB�����
		pmin_x = RayPos_x + RayDir_x * maxDist;
		pmin_y = RayPos_y + RayDir_y * maxDist;
		pmin_z = RayPos_z + RayDir_z * maxDist;
		pmax_x = pmin_x;
		pmax_y = pmin_y;
		pmax_z = pmin_z;
		if( pmin_x > RayPos_x ){ pmin_x = RayPos_x; }
		if( pmin_y > RayPos_y ){ pmin_y = RayPos_y; }
		if( pmin_z > RayPos_z ){ pmin_z = RayPos_z; }
		if( pmax_x < RayPos_x ){ pmax_x = RayPos_x; }
		if( pmax_y < RayPos_y ){ pmax_y = RayPos_y; }
		if( pmax_z < RayPos_z ){ pmax_z = RayPos_z; }

		//���E�{�b�N�X���m�Ŕ���
		if( CollideBoxAABB(s_x - s_r, s_y - s_r, s_z - s_r, s_x + s_r, s_y + s_r, s_z + s_r, pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z) == FALSE ){
			return false;
		}
	}

	D3DXVECTOR3 pCenter(s_x, s_y, s_z);
	D3DXVECTOR3 pRayPos(RayPos_x, RayPos_y, RayPos_z);
	D3DXVECTOR3 pRayDir(RayDir_x, RayDir_y, RayDir_z);

	//���̂ƃ��C�̔���
	if( D3DXSphereBoundProbe(&pCenter, s_r, &pRayPos, &pRayDir) == TRUE ){
		if( Dist != NULL ){
			if( maxDist < 0.0f ){ maxDist = s_r * 2; }

			//�_�ƃ��C�n�_�̋���
			float x, y, z, d;
			x = s_x - RayPos_x;
			y = s_y - RayPos_y;
			z = s_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);

			//���C�n�_�����a���߂��i���߂荞��ł���j
			if( d < s_r ){
				*Dist = 0.0f;
				return true;
			}

			//�_�i���̂̒��S�j�ƃ��C�̍ŒZ���������߂�
			float MinDist, RayDist, RDist;
				//�_�ƃ��C�̍ŒZ����
			MinDist = DistancePosRay(s_x, s_y, s_z, RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, maxDist);
			RayDist = sqrt(d*d - MinDist*MinDist);		//�i���C�n�_����j�_�ɍł��߂Â�����
			RDist = sqrt(s_r*s_r - MinDist*MinDist);	//�i�_���a����j�_�ɍł��߂Â�����

			*Dist = RayDist - RDist;	//���C���_�ŒZ - ���a�ŒZ = ���C���_���甼�a�܂ł̍ŒZ
		}
		return true;
	}

	return false;
}

//! AABB�ƃ��C�i�����j�̂����蔻��
//! @param box_min_x ���̂̍ŏ� X���W
//! @param box_min_y ���̂̍ŏ� Y���W
//! @param box_min_z ���̂̍ŏ� Z���W
//! @param box_max_x ���̂̍ő� X���W
//! @param box_max_y ���̂̍ő� Y���W
//! @param box_max_z ���̂̍ő� Z���W
//! @param RayPos_x ���C�̈ʒu�i�n�_�j���w�肷�� X���W
//! @param RayPos_y ���C�̈ʒu�i�n�_�j���w�肷�� Y���W
//! @param RayPos_z ���C�̈ʒu�i�n�_�j���w�肷�� Z���W
//! @param RayDir_x ���C�̃x�N�g�����w�肷�� X����
//! @param RayDir_y ���C�̃x�N�g�����w�肷�� Y����
//! @param RayDir_z ���C�̃x�N�g�����w�肷�� Z����
//! @param Dist ��������AABB�Ƃ̋������󂯎��|�C���^
//! @param maxDist ������s���ő勗���@�i0.0 �����Ŗ����E�����j
//! @return �������Ă���Ftrue�@�������Ă��Ȃ��Ffalse
//! @warning RayPos�i�n�_�j�� RayDir�i�x�N�g���j���ԈႦ�Ȃ����ƁB
//! @warning ������s���ő勗�����w�肵�Ȃ��ƁA�p�t�H�[�}���X���啝�ɒቺ���܂��B
//! @todo �������������Ƃ��ċߎ��l��Ԃ��̂ŁA���m�Ȓl�����߂�B
bool CollideAABBRay(float box_min_x, float box_min_y, float box_min_z, float box_max_x, float box_max_y, float box_max_z, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist)
{
	if( box_min_x > box_max_x ){ return false; }
	if( box_min_y > box_max_y ){ return false; }
	if( box_min_z > box_max_z ){ return false; }

	if( maxDist > 0.0f ){
		float pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z;

		//���C��AABB�����
		pmin_x = RayPos_x + RayDir_x * maxDist;
		pmin_y = RayPos_y + RayDir_y * maxDist;
		pmin_z = RayPos_z + RayDir_z * maxDist;
		pmax_x = pmin_x;
		pmax_y = pmin_y;
		pmax_z = pmin_z;
		if( pmin_x > RayPos_x ){ pmin_x = RayPos_x; }
		if( pmin_y > RayPos_y ){ pmin_y = RayPos_y; }
		if( pmin_z > RayPos_z ){ pmin_z = RayPos_z; }
		if( pmax_x < RayPos_x ){ pmax_x = RayPos_x; }
		if( pmax_y < RayPos_y ){ pmax_y = RayPos_y; }
		if( pmax_z < RayPos_z ){ pmax_z = RayPos_z; }

		//���E�{�b�N�X���m�Ŕ���
		if( CollideBoxAABB(box_min_x, box_min_y, box_min_z, box_max_x, box_max_y, box_max_z, pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z) == FALSE ){
			return false;
		}
	}

	D3DXVECTOR3 pMin(box_min_x, box_min_y, box_min_z);
	D3DXVECTOR3 pMax(box_max_x, box_max_y, box_max_z);
	D3DXVECTOR3 pRayPos(RayPos_x, RayPos_y, RayPos_z);
	D3DXVECTOR3 pRayDir(RayDir_x, RayDir_y, RayDir_z);

	//AABB�ƃ��C�i�����j�̔���
	if( D3DXBoxBoundProbe(&pMin, &pMax, &pRayPos, &pRayDir) == TRUE ){
		if( Dist != NULL ){
			float x, y, z;
			float d, mind;

			//AABB�̊e���_���狗�����Z�o���āA��ԋ߂��������u�������������v�Ƃ���B
			//�@�@�����ƌv�Z�����@����

			x = box_min_x - RayPos_x;
			y = box_min_y - RayPos_y;
			z = box_min_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);
			mind = d;

			x = box_max_x - RayPos_x;
			y = box_min_y - RayPos_y;
			z = box_min_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);
			if( d < mind ){ mind = d; }

			x = box_min_x - RayPos_x;
			y = box_max_y - RayPos_y;
			z = box_min_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);
			if( d < mind ){ mind = d; }

			x = box_max_x - RayPos_x;
			y = box_max_y - RayPos_y;
			z = box_min_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);
			if( d < mind ){ mind = d; }

			x = box_min_x - RayPos_x;
			y = box_min_y - RayPos_y;
			z = box_max_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);
			if( d < mind ){ mind = d; }

			x = box_max_x - RayPos_x;
			y = box_min_y - RayPos_y;
			z = box_max_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);
			if( d < mind ){ mind = d; }

			x = box_min_x - RayPos_x;
			y = box_max_y - RayPos_y;
			z = box_max_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);
			if( d < mind ){ mind = d; }

			x = box_max_x - RayPos_x;
			y = box_max_y - RayPos_y;
			z = box_max_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);
			if( d < mind ){ mind = d; }

			*Dist = mind;
		}
		return true;
	}

	return false;
}

//! �_�ƃ��C�̍ŒZ���������߂�
//! @param Pos_x �_��X���W
//! @param Pos_y �_��Y���W
//! @param Pos_z �_��Z���W
//! @param RayPos_x ���C�̈ʒu�i�n�_�j���w�肷�� X���W
//! @param RayPos_y ���C�̈ʒu�i�n�_�j���w�肷�� Y���W
//! @param RayPos_z ���C�̈ʒu�i�n�_�j���w�肷�� Z���W
//! @param RayDir_x ���C�̃x�N�g�����w�肷�� X����
//! @param RayDir_y ���C�̃x�N�g�����w�肷�� Y����
//! @param RayDir_z ���C�̃x�N�g�����w�肷�� Z����
//! @param maxDist ������s���ő勗��
//! @return �ŒZ����
//! @warning RayPos�i�n�_�j�� RayDir�i�x�N�g���j���ԈႦ�Ȃ����ƁB
float DistancePosRay(float Pos_x, float Pos_y, float Pos_z, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float maxDist)
{
	float x1, y1, z1;
	float x2, y2, z2;
	D3DXVECTOR3 out;

	x1 = Pos_x - RayPos_x;
	y1 = Pos_y - RayPos_y;
	z1 = Pos_z - RayPos_z;
	x2 = RayDir_x * maxDist;
	y2 = RayDir_y * maxDist;
	z2 = RayDir_z * maxDist;

	D3DXVec3Cross(&out, &D3DXVECTOR3(x1, y1, z1), &D3DXVECTOR3(x2, y2, z2));

	return sqrt(out.x*out.x + out.y*out.y + out.z*out.z) / maxDist;
}

//! �ϑ��_����Ώۓ_�ւ� ��������E�p�x�Z�o
//! @param pos_x �ϑ��_��X���W
//! @param pos_y �ϑ��_��Y���W
//! @param pos_z �ϑ��_��Z���W
//! @param rx �ϑ��_�̐����p�x
//! @param ry �ϑ��_�̐����p�x
//! @param target_x �Ώۓ_��X���W
//! @param target_y �Ώۓ_��Y���W
//! @param target_z �Ώۓ_��Z���W
//! @param checkdist ���苗���i0.0f�ȉ��Ŕ��薳���j
//! @param out_rx �Ώۓ_�ւ̐����p�x�i�΁`-�΁j���󂯎��|�C���^�iNULL�j
//! @param out_ry �Ώۓ_�ւ̐����p�x���󂯎��|�C���^�iNULL�j
//! @param out_dist2 �Ώۓ_�ւ̋���<b>�̓��</b>���󂯎��|�C���^�iNULL�j
//! @return �����Ftrue ���s�Ffalse
//! @warning out_dist2�͋�����<b>���</b>�ł��B�K�v�ɉ����ĉ��߂� sqrt()�֐� �Ȃǂ�p���Ă��������B
//! @attention ���� checkdist �ɗL���ȋ�����^�����ꍇ�́A�ϑ��_����Ώۓ_�ւ̋���������s���܂��B�w�肳�ꂽ������藣��Ă���ꍇ�A�p�x���v�Z������ false ��Ԃ��܂��B
//! @attention �t�ɁA���� checkdist ��0.0f�ȉ���^�����ꍇ�A�����ɂ�锻����s���܂���B�֐��͏�� true ��Ԃ��܂��B
bool CheckTargetAngle(float pos_x, float pos_y, float pos_z, float rx, float ry, float target_x, float target_y, float target_z, float checkdist, float *out_rx, float *out_ry, float *out_dist2)
{
	float x, y, z;
	float dist2 = 0.0f;
	float angleX, angleY;

	x = target_x - pos_x;
	y = target_y - pos_y;
	z = target_z - pos_z;

	if( (checkdist > 0.0f)||(out_dist2 != NULL) ){
		dist2 = (x*x + y*y + z*z);
	}

	if( checkdist > 0.0f ){
		if( dist2 > checkdist * checkdist ){
			return false;
		}
	}

	if( out_rx != NULL ){
		angleX = atan2(z, x) - rx;
		for(; angleX > (float)M_PI; angleX -= (float)M_PI*2){}
		for(; angleX < (float)M_PI*-1; angleX += (float)M_PI*2){}

		*out_rx = angleX;
	}
	if( out_ry != NULL ){
		angleY = atan2(y, sqrt(x*x + z*z))  - ry;

		*out_ry = angleY;
	}
	if( out_dist2 != NULL ){ *out_dist2 = dist2; }

	return true;
}