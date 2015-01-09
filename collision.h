//! @file collision.h
//! @brief Collision�N���X�̐錾

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

#ifndef COLLISION_H
#define COLLISION_H

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 2		//!< Select include file.
#endif
#include "main.h"

#pragma warning(disable:4819)		//VC++�x���h�~
#include <d3dx9math.h>
#pragma warning(default:4819)

#pragma comment(lib, "d3dx9.lib")

//! @brief �����蔻����s���N���X
//! @details �}�b�v�Ƃ��Ďg�p�����u���b�N�f�[�^�ւ̓����蔻��i�Ǔˌ��o�j���s���܂��B
//! @details �����ł�DirectX 9���g�p���Ă��܂��B
class Collision
{
	class BlockDataInterface* blockdata;		//!< �ǂݍ��񂾃u���b�N�f�[�^���i�[���ꂽ�N���X�ւ̃|�C���^
	D3DXPLANE bdata_plane[MAX_BLOCKS][6][2];	//!< �e�u���b�N�̖ʏ��
	float *bmin_x;		//!< �e�u���b�N X���W�̍ő�l
	float *bmin_y;		//!< �e�u���b�N Y���W�̍ő�l
	float *bmin_z;		//!< �e�u���b�N Z���W�̍ő�l
	float *bmax_x;		//!< �e�u���b�N X���W�̍ŏ��l
	float *bmax_y;		//!< �e�u���b�N Y���W�̍ŏ��l
	float *bmax_z;		//!< �e�u���b�N Z���W�̍ŏ��l
	bool *BoardBlock;	//!< �e�u���b�N ������0�Ŕ�ɂȂ��Ă��邩
	int *bdata_worldgroup;	//!< ��ԕ����̃O���[�v

public:
	Collision();
	~Collision();
	int InitCollision(BlockDataInterface* in_blockdata);
	void GetBlockPosMINMAX(struct blockdata data, float *min_x, float *min_y, float *min_z, float *max_x, float *max_y, float *max_z);
	int GetWorldGroup(float x, float z);
	bool CheckBlockInside(int blockid, float x, float y, float z, bool worldgroup, int *planeid);
	bool CheckALLBlockInside(float x, float y, float z);
	bool CheckBlockIntersectRay(int blockid, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *face, float *Dist, float maxDist);
	bool CheckALLBlockIntersectRay(float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *id, int *face, float *Dist, float maxDist);
	bool CheckALLBlockIntersectDummyRay(float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *id, int *face, float *Dist, float maxDist);
	void ScratchVector(BlockDataInterface* in_blockdata, int id, int face, float in_vx, float in_vy, float in_vz, float *out_vx, float *out_vy, float *out_vz);
	void ReflectVector(BlockDataInterface* in_blockdata, int id, int face, float in_vx, float in_vy, float in_vz, float *out_vx, float *out_vy, float *out_vz);
};

bool CollideBoxAABB(float box1_min_x, float box1_min_y, float box1_min_z, float box1_max_x, float box1_max_y, float box1_max_z, float box2_min_x, float box2_min_y, float box2_min_z, float box2_max_x, float box2_max_y, float box2_max_z);
bool CollideCylinder(float c1_x, float c1_y, float c1_z, float c1_r, float c1_h, float c2_x, float c2_y, float c2_z, float c2_r, float c2_h, float *angle, float *length);
bool CollideSphereRay(float s_x, float s_y, float s_z, float s_r, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist);
bool CollideAABBRay(float box_min_x, float box_min_y, float box_min_z, float box_max_x, float box_max_y, float box_max_z, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist);
float DistancePosRay(float Pos_x, float Pos_y, float Pos_z, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float maxDist);

#endif