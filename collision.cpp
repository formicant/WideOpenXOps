//! @file collision.cpp
//! @brief Collisionクラスの定義

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

#include "collision.h"

//! コンストラクタ
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

//! ディストラクタ
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

//! ブロックデータを取り込む
//! @param in_blockdata ブロックデータ
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

	//判定用平面作成
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

			//2つの三角形が持つ法線のなす角を求める
			g0 = sqrt(bdata_plane[i][j][0].a * bdata_plane[i][j][0].a + bdata_plane[i][j][0].b * bdata_plane[i][j][0].b + bdata_plane[i][j][0].c * bdata_plane[i][j][0].c);
			g1 = sqrt(bdata_plane[i][j][1].a * bdata_plane[i][j][1].a + bdata_plane[i][j][1].b * bdata_plane[i][j][1].b + bdata_plane[i][j][1].c * bdata_plane[i][j][1].c);
			costheta = (bdata_plane[i][j][0].a * bdata_plane[i][j][1].a + bdata_plane[i][j][0].b * bdata_plane[i][j][1].b + bdata_plane[i][j][0].c * bdata_plane[i][j][1].c) / (g0 * g1);

			//1つの面で法線が90度以上違う（＝異常）なら～
			if( acos(costheta) > (float)M_PI/2 ){
				//違う三角形で作る
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

	//板状のブロックを検出
	for(int i=0; i<bs; i++){
		blockdata->Getdata(&data, i);

		BoardBlock[i] = false;

		for(int j=0; j<8; j++){
			for(int k=j+1; k<8; k++){
				//一ヵ所でも頂点が同じなら、板状になっていると判定。
				if( (data.x[j] == data.x[k])&&(data.y[j] == data.y[k])&&(data.z[j] == data.z[k]) ){
					BoardBlock[i] = true;
				}
			}
		}
	}


	//ブロックAABB作成
	for(int i=0; i<bs; i++){
		blockdata->Getdata(&data, i);
		GetBlockPosMINMAX(data, &bmin_x[i], &bmin_y[i], &bmin_z[i], &bmax_x[i], &bmax_y[i], &bmax_z[i]);
	}

	//ブロックの空間分割グループを計算
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

//! ブロックの座標最大値・最小値を返す
//! @param data ブロックデータの構造体
//! @param *min_x 最小 X座標を返すポインタ
//! @param *min_y 最小 Y座標を返すポインタ
//! @param *min_z 最小 Z座標を返すポインタ
//! @param *max_x 最大 X座標を返すポインタ
//! @param *max_y 最大 Y座標を返すポインタ
//! @param *max_z 最大 Z座標を返すポインタ
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

//! 空間分割のグループを算出
//! @param x X座標
//! @param z Z座標
//! @return グループ番号
//! @attention 「グループ番号」の概念は別途ドキュメントを参照
int Collision::GetWorldGroup(float x, float z)
{
	if( (x > 0)&&(z > 0) ){ return 1; }
	if( (x < 0)&&(z > 0) ){ return 2; }
	if( (x < 0)&&(z < 0) ){ return 3; }
	if( (x > 0)&&(z < 0) ){ return 4; }
	return 0;
}

//! ブロックに埋まっていないか調べる
//! @param blockid 判定するブロック番号
//! @param x X座標
//! @param y X座標
//! @param z X座標
//! @param worldgroup 空間のグループを利用して計算省略を試みる（true：有効・計算省略　false：無効・完全検索）
//! @param *planeid 表にある面番号（NULL可）
//! @return 埋っている：true　埋っていない：false
//! @warning *planeid が返す表面（0～5）は、複数の面が該当する場合でも、最初に見つけた1面のみ返します。
bool Collision::CheckBlockInside(int blockid, float x, float y, float z, bool worldgroup, int *planeid)
{
	if( blockdata == NULL ){ return false; }
	if( (blockid < 0)||(blockdata->GetTotaldatas() <= blockid) ){ return false; }

	//板状のブロックは計算外
	if( BoardBlock[blockid] == true ){ return false; }


	//判定の荒削り
	if( worldgroup == true ){

		//空間分割
		if( bdata_worldgroup[blockid] != 0 ){
			//観測点の空間のグループを取得
			int worldgroup = GetWorldGroup(x, z);

			if( worldgroup != 0 ){
				//空間のグループが違えば計算外
				if( bdata_worldgroup[blockid] != worldgroup ){
					return false;
				}
			}
		}

		//範囲で検索
		if( (x < bmin_x[blockid])||(bmax_x[blockid] < x) ){ return false; }
		if( (y < bmin_y[blockid])||(bmax_y[blockid] < y) ){ return false; }
		if( (z < bmin_z[blockid])||(bmax_z[blockid] < z) ){ return false; }
	}


	//6面から見て全て裏面かどうか
	for(int i=0; i<6; i++){
		if( (D3DXPlaneDotCoord(&bdata_plane[blockid][i][0], &D3DXVECTOR3( x, y, z )) > 0)||(D3DXPlaneDotCoord(&bdata_plane[blockid][i][1], &D3DXVECTOR3( x, y, z )) > 0) ){
			if( planeid != NULL ){ *planeid = i; }
			return false;	//表面ならば終了
		}
	}

	return true;
}

//! 全てのブロックに埋まっていないか調べる
//! @param x X座標
//! @param y X座標
//! @param z X座標
//! @return 埋っている：true　埋っていない：false
bool Collision::CheckALLBlockInside(float x, float y, float z)
{
	if( blockdata == NULL ){ return false; }

	int bs = blockdata->GetTotaldatas();

	for(int i=0; i<bs; i++){
		if( CheckBlockInside(i, x, y, z, true, NULL) == true ){ return true; }
	}

	return false;
}

//! 全てのブロックとレイ（光線）のあたり判定
//! @param blockid 判定するブロック番号
//! @param RayPos_x レイの位置（始点）を指定する X座標
//! @param RayPos_y レイの位置（始点）を指定する Y座標
//! @param RayPos_z レイの位置（始点）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param face 当たったブロックの面番号（0～5）を受け取るポインタ　（NULL可）
//! @param Dist 当たったブロックとの距離を受け取るポインタ
//! @param maxDist 判定を行う最大距離　（0.0 未満で無効・無限）
//! @return 当たっている：true　当たっていない：false
//! @warning RayPos（始点）と RayDir（ベクトル）を間違えないこと。
//! @warning 判定を行う最大距離を指定しないと、パフォーマンスが大幅に低下します。
//! @attention レイの始点から裏側になるブロックの面は無視されます。厚さをゼロに変形させた板状のブロックも無視します。
//! @attention また、レイが複数のブロックに当たる場合は、レイの始点から一番近い判定を返します。
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

	//板状のブロックは計算外
	if( BoardBlock[blockid] == true ){
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	if( maxDist > 0.0f ){
		//レイのAABBを作る
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
		//始点と終点の空間グループを取得
		worldgroupA = GetWorldGroup(RayPos_x, RayPos_z);
		worldgroupB = GetWorldGroup(RayPos_x + RayDir_x * maxDist, RayPos_z + RayDir_z * maxDist);
	}

	blockdata->Getdata(&data, blockid);

	if( maxDist > 0.0f ){
		if( bdata_worldgroup[blockid] != 0 ){
			//始点と終点が、空間のグループから出ていなければ
			if( (worldgroupA != 0)&&(worldgroupA == worldgroupB) ){

				//空間のグループが違えば計算外
				if( bdata_worldgroup[blockid] == worldgroupA ){		//worldgroupA == worldgroupB
					//境界ボックス同士で判定
					if( CollideBoxAABB(bmin_x[blockid], bmin_y[blockid], bmin_z[blockid], bmax_x[blockid], bmax_y[blockid], bmax_z[blockid], rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z) == false ){
						//当たってなければ、このブロックは調べない。
						if( face != NULL ){ *face = 0; }
						*Dist = 0.0f;
						return false;
					}
				}
			}
		}
	}

	//境界ボックスとレイで判定
	if( CollideAABBRay(bmin_x[blockid], bmin_y[blockid], bmin_z[blockid], bmax_x[blockid], bmax_y[blockid], bmax_z[blockid], RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, NULL, -1.0f) == false ){
		//当たってなければ、このブロックは調べない。
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//各ポリゴン単位で判定
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

	//見つからなければ、ポインタに適当な数字を入れて返す。
	if( min_blockface == -1 ){
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//計算結果を入れて返す
	if( face != NULL ){ *face = min_blockface; }
	*Dist = min_pDist;
	return true;
}

//! 全てのブロックとレイ（光線）のあたり判定
//! @param RayPos_x レイの位置（始点）を指定する X座標
//! @param RayPos_y レイの位置（始点）を指定する Y座標
//! @param RayPos_z レイの位置（始点）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param id 当たったブロックのIDを受け取るポインタ　（NULL可）
//! @param face 当たったブロックの面番号（0～5）を受け取るポインタ　（NULL可）
//! @param Dist 当たったブロックとの距離を受け取るポインタ
//! @param maxDist 判定を行う最大距離　（0.0 未満で無効・無限）
//! @return 当たっている：true　当たっていない：false
//! @warning RayPos（始点）と RayDir（ベクトル）を間違えないこと。
//! @warning 判定を行う最大距離を指定しないと、パフォーマンスが大幅に低下します。
//! @warning 使い方は CheckALLBlockIntersectDummyRay()関数 と類似していますが、同関数より高精度で低速です。
//! @attention レイの始点から裏側になるブロックの面は無視されます。厚さをゼロに変形させた板状のブロックも無視します。
//! @attention また、レイが複数のブロックに当たる場合は、<b>レイの始点から一番近い判定</b>を返します。
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
		//レイのAABBを作る
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
		//始点と終点の空間グループを取得
		worldgroupA = GetWorldGroup(RayPos_x, RayPos_z);
		worldgroupB = GetWorldGroup(RayPos_x + RayDir_x * maxDist, RayPos_z + RayDir_z * maxDist);
	}
	
	for(int i=0; i<bs; i++){
		//板状のブロックは計算外
		if( BoardBlock[i] == true ){ continue; }

		blockdata->Getdata(&data, i);

		if( maxDist > 0.0f ){
			if( bdata_worldgroup[i] != 0 ){
				//始点と終点が、空間のグループから出ていなければ
				if( (worldgroupA != 0)&&(worldgroupA == worldgroupB) ){

					//空間のグループが違えば計算外
					if( bdata_worldgroup[i] == worldgroupA ){		//worldgroupA == worldgroupB

						//境界ボックス同士で判定
						if( CollideBoxAABB(bmin_x[i], bmin_y[i], bmin_z[i], bmax_x[i], bmax_y[i], bmax_z[i], rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z) == false ){
							continue;	//当たってなければ、このブロックは調べない。
						}

					}
				}
			}
		}

		//境界ボックスとレイで判定
		if( CollideAABBRay(bmin_x[i], bmin_y[i], bmin_z[i], bmax_x[i], bmax_y[i], bmax_z[i], RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, NULL, -1.0f) == false ){
			continue;		//当たってなければ、このブロックは調べない。
		}

		//各ポリゴン単位で判定
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

	//見つけた距離が最大距離を超えていれば、判定を無効に。
	if( maxDist >= 0.0f ){
		if( min_pDist > maxDist ){
			min_blockid = -1;
		}
	}

	//見つからなければ、ポインタに適当な数字を入れて返す。
	if( min_blockid == -1 ){
		if( id != NULL ){ *id = 0; }
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//計算結果を入れて返す
	if( id != NULL ){ *id = min_blockid; }
	if( face != NULL ){ *face = min_blockface; }
	*Dist = min_pDist;
	return true;
}

//! 全てのブロックと衝突しているか判定
//! @param RayPos_x 始点 X座標
//! @param RayPos_y 始点 Y座標
//! @param RayPos_z 始点 Z座標
//! @param RayDir_x ベクトル X成分
//! @param RayDir_y ベクトル Y成分
//! @param RayDir_z ベクトル Z成分
//! @param id 当たったブロックのIDを受け取るポインタ　（NULL可）
//! @param face 当たったブロックの面番号（0～5）を受け取るポインタ　（NULL可）
//! @param Dist ダミー変数のポインタ（常に 0.0f を返す）
//! @param maxDist 判定を行う最大距離　（0.0 未満指定 不可）
//! @return 当たっている：true　当たっていない：false
//! @warning 始点と ベクトルを間違えないこと。
//! @warning 使い方は CheckALLBlockIntersectRay()関数 と類似していますが、同関数より高速で低精度です。
//! @attention レイの始点から裏側になるブロックの面は無視されます。厚さをゼロに変形させた板状のブロックも無視します。
//! @attention また、レイが複数のブロックに当たる場合は、<b>一番最初に発見した判定</b>を返します。
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
			//終了時点
			if( CheckBlockInside(i, RayPos3_x, RayPos3_y, RayPos3_z, true, NULL) == true ){
				*Dist = 0.0f;
				return true;
			}

			//中間時点
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

		//開始地点
		CheckBlockInside(i, RayPos_x, RayPos_y, RayPos_z, false, &surface);

		//終了時点
		if( CheckBlockInside(i, RayPos3_x, RayPos3_y, RayPos3_z, true, &surface) == true ){
			if( id != NULL ){ *id = i; }
			if( face != NULL ){ *face = surface; }
			*Dist = 0.0f;
			return true;
		}

		//中間時点
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

//! ブロックに沿って移動するベクトルを求める
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

//! ブロックに反射するベクトルを求める
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


//! AABBによるあたり判定
//! @param box1_min_x 物体Aの最少 X座標
//! @param box1_min_y 物体Aの最少 Y座標
//! @param box1_min_z 物体Aの最少 Z座標
//! @param box1_max_x 物体Aの最大 X座標
//! @param box1_max_y 物体Aの最大 Y座標
//! @param box1_max_z 物体Aの最大 Z座標
//! @param box2_min_x 物体Bの最少 X座標
//! @param box2_min_y 物体Bの最少 Y座標
//! @param box2_min_z 物体Bの最少 Z座標
//! @param box2_max_x 物体Bの最大 X座標
//! @param box2_max_y 物体Bの最大 Y座標
//! @param box2_max_z 物体Bの最大 Z座標
//! @return 当たっている：true　当たっていない：false
//! @attention エラーがある場合「当たっていない：false」が返されます。
bool CollideBoxAABB(float box1_min_x, float box1_min_y, float box1_min_z, float box1_max_x, float box1_max_y, float box1_max_z, float box2_min_x, float box2_min_y, float box2_min_z, float box2_max_x, float box2_max_y, float box2_max_z)
{
	//エラー対策
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

//! 円柱同士の当たり判定
//! @param c1_x 円柱1 底辺のx座標
//! @param c1_y 円柱1 底辺のy座標
//! @param c1_z 円柱1 底辺のz座標
//! @param c1_r 円柱1 の半径
//! @param c1_h 円柱1 の高さ
//! @param c2_x 円柱2 底辺のx座標
//! @param c2_y 円柱2 底辺のy座標
//! @param c2_z 円柱2 底辺のz座標
//! @param c2_r 円柱2 の半径
//! @param c2_h 円柱2 の高さ
//! @param *angle 円柱1からみた接触角度を受け取るポインタ（NULL可）
//! @param *length 円柱1からみた接触距離を受け取るポインタ（NULL可）
//! @return 当たっている：true　当たっていない：false
bool CollideCylinder(float c1_x, float c1_y, float c1_z, float c1_r, float c1_h, float c2_x, float c2_y, float c2_z, float c2_r, float c2_h, float *angle, float *length)
{
	//先にお手軽な高さで判定
	if( (c1_y < c2_y + c2_h)&&(c1_y + c1_h > c2_y) ){

		//距離で判定
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

//! 球体とレイ（光線）のあたり判定
//! @param s_x 球体のx座標
//! @param s_y 球体のy座標
//! @param s_z 球体のz座標
//! @param s_r 球体の半径
//! @param RayPos_x レイの位置（始点）を指定する X座標
//! @param RayPos_y レイの位置（始点）を指定する Y座標
//! @param RayPos_z レイの位置（始点）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param Dist 当たった球体との距離を受け取るポインタ
//! @param maxDist 判定を行う最大距離　（0.0 未満で無効・無限）
//! @return 当たっている：true　当たっていない：false
//! @warning RayPos（始点）と RayDir（ベクトル）を間違えないこと。
//! @warning 判定を行う最大距離を指定しないと、パフォーマンスが大幅に低下します。
bool CollideSphereRay(float s_x, float s_y, float s_z, float s_r, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist)
{
	if( maxDist > 0.0f ){
		float pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z;

		//レイのAABBを作る
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

		//境界ボックス同士で判定
		if( CollideBoxAABB(s_x - s_r, s_y - s_r, s_z - s_r, s_x + s_r, s_y + s_r, s_z + s_r, pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z) == FALSE ){
			return false;
		}
	}

	D3DXVECTOR3 pCenter(s_x, s_y, s_z);
	D3DXVECTOR3 pRayPos(RayPos_x, RayPos_y, RayPos_z);
	D3DXVECTOR3 pRayDir(RayDir_x, RayDir_y, RayDir_z);

	//球体とレイの判定
	if( D3DXSphereBoundProbe(&pCenter, s_r, &pRayPos, &pRayDir) == TRUE ){
		if( Dist != NULL ){
			if( maxDist < 0.0f ){ maxDist = s_r * 2; }

			//点とレイ始点の距離
			float x, y, z, d;
			x = s_x - RayPos_x;
			y = s_y - RayPos_y;
			z = s_z - RayPos_z;
			d = sqrt(x*x + y*y + z*z);

			//レイ始点が半径より近い（＝めり込んでいる）
			if( d < s_r ){
				*Dist = 0.0f;
				return true;
			}

			//点（球体の中心）とレイの最短距離を求める
			float MinDist, RayDist, RDist;
				//点とレイの最短距離
			MinDist = DistancePosRay(s_x, s_y, s_z, RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, maxDist);
			RayDist = sqrt(d*d - MinDist*MinDist);		//（レイ始点から）点に最も近づく距離
			RDist = sqrt(s_r*s_r - MinDist*MinDist);	//（点半径から）点に最も近づく距離

			*Dist = RayDist - RDist;	//レイ視点最短 - 半径最短 = レイ視点から半径までの最短
		}
		return true;
	}

	return false;
}

//! AABBとレイ（光線）のあたり判定
//! @param box_min_x 物体の最少 X座標
//! @param box_min_y 物体の最少 Y座標
//! @param box_min_z 物体の最少 Z座標
//! @param box_max_x 物体の最大 X座標
//! @param box_max_y 物体の最大 Y座標
//! @param box_max_z 物体の最大 Z座標
//! @param RayPos_x レイの位置（始点）を指定する X座標
//! @param RayPos_y レイの位置（始点）を指定する Y座標
//! @param RayPos_z レイの位置（始点）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param Dist 当たったAABBとの距離を受け取るポインタ
//! @param maxDist 判定を行う最大距離　（0.0 未満で無効・無限）
//! @return 当たっている：true　当たっていない：false
//! @warning RayPos（始点）と RayDir（ベクトル）を間違えないこと。
//! @warning 判定を行う最大距離を指定しないと、パフォーマンスが大幅に低下します。
//! @todo 当たった距離として近似値を返すので、正確な値を求める。
bool CollideAABBRay(float box_min_x, float box_min_y, float box_min_z, float box_max_x, float box_max_y, float box_max_z, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist)
{
	if( box_min_x > box_max_x ){ return false; }
	if( box_min_y > box_max_y ){ return false; }
	if( box_min_z > box_max_z ){ return false; }

	if( maxDist > 0.0f ){
		float pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z;

		//レイのAABBを作る
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

		//境界ボックス同士で判定
		if( CollideBoxAABB(box_min_x, box_min_y, box_min_z, box_max_x, box_max_y, box_max_z, pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z) == FALSE ){
			return false;
		}
	}

	D3DXVECTOR3 pMin(box_min_x, box_min_y, box_min_z);
	D3DXVECTOR3 pMax(box_max_x, box_max_y, box_max_z);
	D3DXVECTOR3 pRayPos(RayPos_x, RayPos_y, RayPos_z);
	D3DXVECTOR3 pRayDir(RayDir_x, RayDir_y, RayDir_z);

	//AABBとレイ（光線）の判定
	if( D3DXBoxBoundProbe(&pMin, &pMax, &pRayPos, &pRayDir) == TRUE ){
		if( Dist != NULL ){
			float x, y, z;
			float d, mind;

			//AABBの各頂点から距離を算出して、一番近い距離を「当たった距離」とする。
			//　　ちゃんと計算しろよ　＞＜

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

//! 点とレイの最短距離を求める
//! @param Pos_x 点のX座標
//! @param Pos_y 点のY座標
//! @param Pos_z 点のZ座標
//! @param RayPos_x レイの位置（始点）を指定する X座標
//! @param RayPos_y レイの位置（始点）を指定する Y座標
//! @param RayPos_z レイの位置（始点）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param maxDist 判定を行う最大距離
//! @return 最短距離
//! @warning RayPos（始点）と RayDir（ベクトル）を間違えないこと。
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

//! 観測点から対象点への 距離判定・角度算出
//! @param pos_x 観測点のX座標
//! @param pos_y 観測点のY座標
//! @param pos_z 観測点のZ座標
//! @param rx 観測点の水平角度
//! @param ry 観測点の垂直角度
//! @param target_x 対象点のX座標
//! @param target_y 対象点のY座標
//! @param target_z 対象点のZ座標
//! @param checkdist 判定距離（0.0f以下で判定無効）
//! @param out_rx 対象点への水平角度（π～-π）を受け取るポインタ（NULL可）
//! @param out_ry 対象点への垂直角度を受け取るポインタ（NULL可）
//! @param out_dist2 対象点への距離<b>の二乗</b>を受け取るポインタ（NULL可）
//! @return 成功：true 失敗：false
//! @warning out_dist2は距離の<b>二乗</b>です。必要に応じて改めて sqrt()関数 などを用いてください。
//! @attention 引数 checkdist に有効な距離を与えた場合は、観測点から対象点への距離判定も行います。指定された距離より離れている場合、角度を計算せずに false を返します。
//! @attention 逆に、引数 checkdist に0.0f以下を与えた場合、距離による判定を行いません。関数は常に true を返します。
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