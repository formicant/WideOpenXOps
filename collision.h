//! @file collision.h
//! @brief Collisionクラスの宣言

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

#ifndef COLLISION_H
#define COLLISION_H

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 2		//!< Select include file.
#endif
#include "main.h"

#pragma warning(disable:4819)		//VC++警告防止
#include <d3dx9math.h>
#pragma warning(default:4819)

#pragma comment(lib, "d3dx9.lib")

//! @brief 当たり判定を行うクラス
//! @details マップとして使用されるブロックデータへの当たり判定（追突検出）を行います。
//! @details 内部ではDirectX 9を使用しています。
class Collision
{
	class BlockDataInterface* blockdata;		//!< 読み込んだブロックデータが格納されたクラスへのポインタ
	D3DXPLANE bdata_plane[MAX_BLOCKS][6][2];	//!< 各ブロックの面情報
	float *bmin_x;		//!< 各ブロック X座標の最大値
	float *bmin_y;		//!< 各ブロック Y座標の最大値
	float *bmin_z;		//!< 各ブロック Z座標の最大値
	float *bmax_x;		//!< 各ブロック X座標の最小値
	float *bmax_y;		//!< 各ブロック Y座標の最小値
	float *bmax_z;		//!< 各ブロック Z座標の最小値
	bool *BoardBlock;	//!< 各ブロック が厚さ0で板状になっているか
	int *bdata_worldgroup;	//!< 空間分割のグループ

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