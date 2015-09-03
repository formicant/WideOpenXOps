//! @file datafile.cpp
//! @brief データ管理クラスの定義

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

//! @brief コンストラクタ
DataInterface::DataInterface()
{
	datas = 0;
}

//! @brief ディストラクタ
DataInterface::~DataInterface()
{}

//! @brief データファイルを読みこむ
//! @param fname ファイル名
//! @return 成功：0　失敗：1
int DataInterface::LoadFiledata(char *fname)
{
	return 0;
}

//! @brief 合計データ数の取得
//! @return 合計データ数
int DataInterface::GetTotaldatas()
{
	return datas;
}

//! @brief データを取得
//! @param out_data 受け取るポインタ
//! @param id 認識番号
//! @return 成功：0　失敗：0以外
int DataInterface::Getdata(void *out_data, int id)
{
	return 0;
}

//! @brief コンストラクタ
BlockDataInterface::BlockDataInterface()
{
	//blockdata構造体初期化
	data = new blockdata[MAX_BLOCKS];

	//テクスチャ設定初期化
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		strcpy(texture[i], "");
	}
}

//! @brief ディストラクタ
BlockDataInterface::~BlockDataInterface()
{
	//blockdata構造体解放
	if( data != NULL ) delete [] data;

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "BD1", "");
#endif
}

//! @brief ブロックデータファイルを読みこむ
//! @param fname ファイル名
//! @return 成功：0　失敗：1
int BlockDataInterface::LoadFiledata(char *fname)
{
	FILE *fp;
	unsigned char bdata_header[2];
	float bdata_main[80];
	char bdata_mainb[30];

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "BD1", fname);
#endif

#ifdef PATH_DELIMITER_SLASH
	//パス区切り文字を変換
	fname = ChangePathDelimiter(fname);
#endif

	//ファイルを読み込む
	fp = fopen(fname, "rb");
	if( fp == NULL ){
		return 1;
	}

	//テクスチャを取得
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		fread(texture[i], 1, 31, fp );
	}

	//データ数を取得
	fread( bdata_header, 1, 2, fp );
	datas = (int)bdata_header[1]*256 + bdata_header[0];
	if( datas > MAX_BLOCKS ){
		datas = MAX_BLOCKS;
	}

	//ブロックデータ数分取得
	for(int i=0; i<datas; i++){
		//ブロックのを頂点情報と面情報を取得
		fread( bdata_main, 4, 24+48, fp );
		fread( bdata_mainb, 1, 28, fp );

		//データ番号を設定
		data[i].id = i;

		//ブロックの頂点情報に設定
		for(int j=0; j<8; j++){
			data[i].x[j] = bdata_main[j];
			data[i].y[j] = bdata_main[j+8];
			data[i].z[j] = bdata_main[j+16];
		}

		//ブロックの面情報を設定
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

	//ファイルハンドルを解放
	fclose( fp );

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief ブロックデータの法線・影を算出する
//! @param screen ブロックを暗くする
//! @attention LoadBlockdata()関数で読みこんだ後、一度だけ実行。
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

	//光源の角度を設定
	lx = cos(LIGHT_RX);
	ly = sin(LIGHT_RY);
	lz = sin(LIGHT_RX);

	//各ブロックの面情報分処理する
	for(int i=0; i<datas; i++){
		for(int j=0; j<6; j++){

			//ブロック頂点データの関連付けを取得
			blockdataface(j, vID, uvID);

			//面の法線（ベクトル）と、その長さを求める
			xs1 = ((data[i].y[ vID[3] ] - data[i].y[ vID[2] ]) * (data[i].z[ vID[0] ] - data[i].z[ vID[2] ])) - ((data[i].y[ vID[0] ] - data[i].y[ vID[2] ]) * (data[i].z[ vID[3] ] - data[i].z[ vID[2] ]));
			ys1 = ((data[i].z[ vID[3] ] - data[i].z[ vID[2] ]) * (data[i].x[ vID[0] ] - data[i].x[ vID[2] ])) - ((data[i].z[ vID[0] ] - data[i].z[ vID[2] ]) * (data[i].x[ vID[3] ] - data[i].x[ vID[2] ]));
			zs1 = ((data[i].x[ vID[3] ] - data[i].x[ vID[2] ]) * (data[i].y[ vID[0] ] - data[i].y[ vID[2] ])) - ((data[i].x[ vID[0] ] - data[i].x[ vID[2] ]) * (data[i].y[ vID[3] ] - data[i].y[ vID[2] ]));
			g1 = (float)sqrt(xs1 * xs1 + ys1 * ys1 + zs1 * zs1);

			//もう一方の三角形でも計算する
			xs2 = ((data[i].y[ vID[1] ] - data[i].y[ vID[0] ]) * (data[i].z[ vID[2] ] - data[i].z[ vID[0] ])) - ((data[i].y[ vID[2] ] - data[i].y[ vID[0] ]) * (data[i].z[ vID[1] ] - data[i].z[ vID[0] ]));
			ys2 = ((data[i].z[ vID[1] ] - data[i].z[ vID[0] ]) * (data[i].x[ vID[2] ] - data[i].x[ vID[0] ])) - ((data[i].z[ vID[2] ] - data[i].z[ vID[0] ]) * (data[i].x[ vID[1] ] - data[i].x[ vID[0] ]));
			zs2 = ((data[i].x[ vID[1] ] - data[i].x[ vID[0] ]) * (data[i].y[ vID[2] ] - data[i].y[ vID[0] ])) - ((data[i].x[ vID[2] ] - data[i].x[ vID[0] ]) * (data[i].y[ vID[1] ] - data[i].y[ vID[0] ]));
			g2 = (float)sqrt(xs2 * xs2 + ys2 * ys2 + zs2 * zs2);

			//正しい計算結果を採用する
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

			//法線（ベクトル）を正規化
			data[i].material[j].vx = xs / g;
			data[i].material[j].vy = ys / g;
			data[i].material[j].vz = zs / g;

			//面の明るさを求める
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

//! @brief ブロックデータに設定されたテクスチャのファイル名を取得
//! @param fname ファイル名を受け取る文字列型ポインタ
//! @param id テクスチャ番号
//! @return 成功：0　失敗：0以外
int BlockDataInterface::GetTexture(char *fname, int id)
{
	if( data == NULL ){ return 1; }
	if( (id < 0)||((TOTAL_BLOCKTEXTURE -1) < id) ){ return 2; }

	//ポインタにテクスチャ名をコピー
	strcpy(fname, texture[id]);

	return 0;
}

//! @brief ブロックデータを取得
//! @param out_data 受け取るblockdata型ポインタ
//! @param id 認識番号
//! @return 成功：0　失敗：0以外
int BlockDataInterface::Getdata(blockdata *out_data, int id)
{
	if( data == NULL ){ return 1; }
	if( (id < 0)||((datas -1) < id) ){ return 2; }

	//ブロックデータを取得
	memcpy(out_data, &(data[id]), sizeof(blockdata));

	return 0;
}

//! @brief ブロック頂点データの関連付けを取得
//
//	    3 ・----・2
//	     /|    /|
//	    / |   / |
//	0 ・----・1 |
//	  |7 ・-| --・6
//	  |  /  |  /
//	  | /   | /
//	4 ・----・5
//
//	C-----D
//	|     |
//	|     |
//	B-----A
//
//	上[0](ABCD) =  (1032)
//	下[1]		=  (6745)
//	前[2]		=  (5401)
//	右[3]		=  (6512)
//	奥[4]		=  (7623)
//	左[5]		=  (4730)
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

//! @brief コンストラクタ
PointDataInterface::PointDataInterface()
{
	//pointdata構造体初期化
	data = new pointdata[MAX_POINTS];

	//イベントメッセージ初期化
	for(int i=0; i<MAX_POINTMESSAGES; i++){
		text[i] = new char[MAX_POINTMESSAGEBYTE];
		text[i][0] = NULL;
	}
}

//! @brief ディストラクタ
PointDataInterface::~PointDataInterface()
{
	//pointdata構造体解放
	if( data != NULL ) delete [] data;

	//イベントメッセージ解放
	for(int i=0; i<MAX_POINTMESSAGES; i++){
		if( text[i] != NULL ) delete [] text[i];
	}

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_CLEANUP, "PD1", "");
#endif
}

//! @brief ポイントデータファイルを読みこむ
//! @param fname ファイル名
//! @return 成功：0　失敗：1
int PointDataInterface::LoadFiledata(char *fname)
{
	FILE *fp;
	unsigned char pdata_header[2];
	float pdata_mainf[200][4];
	char pdata_mainc[200][4];
	char fname2[MAX_PATH];

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "PD1", fname);
#endif

#ifdef PATH_DELIMITER_SLASH
	//パス区切り文字を変換
	fname = ChangePathDelimiter(fname);
#endif

	//ファイルを読み込む
	fp = fopen( fname, "rb" );
	if( fp == NULL ){
		return 1;
	}

	//データ数を取得
	fread( pdata_header, 1, 2, fp );
	datas = (int)pdata_header[1]*256 + pdata_header[0];
	if( datas > MAX_POINTS ){
		datas = MAX_POINTS;
	}

	//ポイントデータ数分処理する
	for(int i=0; i<datas; i++){
		//データ番号設定
		data[i].id = i;

		//座標データ取得
		fread( pdata_mainf[i], 4, 4, fp );
		data[i].x = pdata_mainf[i][0];
		data[i].y = pdata_mainf[i][1];
		data[i].z = pdata_mainf[i][2];
		data[i].r = pdata_mainf[i][3];

		//パラメータ取得
		fread( pdata_mainc[i], 1, 4, fp );
		data[i].p1 = pdata_mainc[i][0];
		data[i].p2 = pdata_mainc[i][1];
		data[i].p3 = pdata_mainc[i][2];
		data[i].p4 = pdata_mainc[i][3];
	}

	//ファイルポインタを閉じる
	fclose( fp );

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//「同ファイル名.msg」を生成
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
	//パス区切り文字を変換
	strcpy(fname2, ChangePathDelimiter(fname2));
#endif

	//ファイルを読み込み
	fp = fopen( fname2, "r" );
	if( fp != NULL ){
#ifdef ENABLE_DEBUGLOG
		//ログに出力
		OutputLog.WriteLog(LOG_LOAD, "MSG", fname2);
#endif

		//メッセージデータを取得
		for(int i=0; i<MAX_POINTMESSAGES; i++){
			if( fgets(text[i], MAX_POINTMESSAGEBYTE, fp) == NULL ){ break; }

			//'\r'があれば'\n'に置き換える
			for(int j=0; j<MAX_POINTMESSAGEBYTE; j++){
				if( text[i][j] == '\r' ){ text[i][j] = '\n'; }
			}
		}

#ifdef ENABLE_DEBUGLOG
		//ログに出力
		OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

		//ファイルポインタを開放
		fclose( fp );
	}
	else{
#ifdef ENABLE_DEBUGLOG
		//ログに出力
		OutputLog.WriteLog(LOG_CHECK, "MSG", "ファイルなし");
#endif
	}

	return 0;
}

//! @brief ポイントデータを取得
//! @param out_data 受け取るpointdata型ポインタ
//! @param id 認識番号
//! @return 成功：0　失敗：0以外
int PointDataInterface::Getdata(pointdata *out_data, int id)
{
	if( data == NULL ){ return 1; }
	if( (id < 0)||((datas -1) < id) ){ return 2; }

	//データをポインタにコピー
	memcpy(out_data, &(data[id]), sizeof(pointdata));

	return 0;
}

//! @brief ポイントのパラメーターを書き換え
//! @param id 認識番号
//! @param p1 第1パラメータ
//! @param p2 第2パラメータ
//! @param p3 第3パラメータ
//! @param p4 第4パラメータ
//! @return 成功：0　失敗：0以外
//! @warning AIが使用するパス（p1:3 or p1:8）以外への使用は、保証されておらず推奨しません。
int PointDataInterface::SetParam(int id, signed char p1, signed char p2, signed char p3, signed char p4)
{
	if( data == NULL ){ return 1; }
	if( (id < 0)||((datas -1) < id) ){ return 2; }

	//パラメータを上書き
	data[id].p1 = p1;
	data[id].p2 = p2;
	data[id].p3 = p3;
	data[id].p4 = p4;

	return 0;
}

//! @brief メッセージ（1行）を取得
//! @param str 文字列を受け取るポインタ
//! @param id 認識番号
//! @return 成功：0　失敗：1
int PointDataInterface::GetMessageText(char *str, int id)
{
	if( (id < 0)||((MAX_POINTMESSAGES -1) < id) ){ return 1; }

	//ポインタにメッセージをコピー
	strcpy(str, text[id]);
	return 0;
}

//! @brief ポイントデータを検索
//! @param id 最初に該当したデータ番号を受け取るポインタ　（NULL 可）
//! @param pmask パラメータのマスク
//! @param p1 第1パラメータ
//! @param p2 第2パラメータ
//! @param p3 第3パラメータ
//! @param p4 第4パラメータ
//! @param offset 検索を開始するデータ
//! @return 該当ポイント数
//! @note パラメータのマスクは、p1～p4の検索対象を指定します。
//! @note 1～4ビット目までを使用し、p1：1ビット目、p2：2ビット目、p3：3ビット目、p4：4ビット目　をそれぞれ意味します。
//! @note 検索の対象ビット：1　対象外のビット：0　に設定してください。　なお、7～5ビット目は無視されます。
//! @code
//! //16進数で記述の場合―
//! 0x02	// p2のみを検索対象にする
//! 0x05	// p1とp3を検索対象にする
//! 0xF1	// p1のみを検索対象にする　（7～5ビット目は無視）
//! @endcode
int PointDataInterface::SearchPointdata(int* id, unsigned char pmask, signed char p1, signed char p2, signed char p3, signed char p4, int offset)
{
	int cnt = 0;

	if( offset < 0 ){ offset = 0; }
	if( offset >= datas ){ offset = datas; }

	//オフセット値からデータ数分処理
	for(int i=offset; i<datas; i++){

		if( ((pmask&0x01) == 0)||(p1 == data[i].p1) ){		//マスクで指定されていないか、p1が一致
			if( ((pmask&0x02) == 0)||(p2 == data[i].p2) ){		//マスクで指定されていないか、p2が一致
				if( ((pmask&0x04) == 0)||(p3 == data[i].p3) ){		//マスクで指定されていないか、p3が一致
					if( ((pmask&0x08) == 0)||(p4 == data[i].p4) ){		//マスクで指定されていないか、p4が一致

						if( (cnt == 0)&&(id != NULL) ){			//最初に該当し、idがNULLでない。
							*id = i;
						}

						//検索該当数を+1
						cnt += 1;

					}
				}
			}
		}

	}

	//検索該当数を返す
	return cnt;
}

//! @brief ポイントデータを検索
//! @param out_data 最初に該当したデータを受け取るpointdata型ポインタ　（NULL 可）
//! @param pmask パラメータのマスク
//! @param p1 第1パラメータ
//! @param p2 第2パラメータ
//! @param p3 第3パラメータ
//! @param p4 第4パラメータ
//! @param offset 検索を開始するデータ
//! @return 該当ポイント数
//! @attention この関数の詳しい説明は、同オーバーロード関数をご覧ください。
int PointDataInterface::SearchPointdata(pointdata *out_data, unsigned char pmask, signed char p1, signed char p2, signed char p3, signed char p4, int offset)
{
	int id, total;

	//同条件でポイントを検索
	total = SearchPointdata(&id, pmask, p1, p2, p3, p4, offset);
	
	//該当すればデータを取得
	if( total > 0 ){
		Getdata(out_data, id);
	}

	//検索該当数を返す
	return total;
}

//! @brief コンストラクタ
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

//! @brief ディストラクタ
MIFInterface::~MIFInterface()
{}

//! @brief データファイルを読みこむ
//! @param fname ファイル名
//! @return 成功：0　失敗：1
int MIFInterface::LoadFiledata(char *fname)
{
	char str[64];

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_LOAD, "MIF", fname);
#endif

	mif = true;

	//拡張子が.txtならば
	//if( strcmp(PathFindExtension(fname), ".txt") == 0 ){
	//	//MIFフラグを無効に
	//	mif = false;
	//}
	for(int i=strlen(fname)-1; i>0; i--){
		if( fname[i] == '.' ){
			if( strcmp(&(fname[i]), ".txt") == 0 ){
				//MIFフラグを無効に
				mif = false;
			}
			break;
		}
	}

	FILE *fp;

#ifdef PATH_DELIMITER_SLASH
	//パス区切り文字を変換
	fname = ChangePathDelimiter(fname);
#endif

	//ファイルを開く
	fp = fopen( fname, "r" );
	if( fp == NULL ){
		//briefing data open failed
		return 1;
	}

	if( mif == true ){
		//ミッション識別名
		fgets(mission_name, 24, fp);
		DeleteLinefeed(mission_name);

		//ミッション正式名称
		fgets(mission_fullname, 64, fp);
		DeleteLinefeed(mission_fullname);

		//ブロックデータファイル
		fgets(blockfile_path, _MAX_PATH, fp);
		DeleteLinefeed(blockfile_path);

		//ポイントデータファイル
		fgets(pointfile_path, _MAX_PATH, fp);
		DeleteLinefeed(pointfile_path);

		//背景空の番号
		fgets(str, 16, fp);
		DeleteLinefeed(str);
		skynumber = atoi(str);

		//あたり判定・画面設定の取得
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

		//追加小物情報ファイル取得
		fgets(addsmallobject_path, _MAX_PATH, fp);
		DeleteLinefeed(addsmallobject_path);

		//画像Aを取得
		fgets(picturefileA_path, _MAX_PATH, fp);
		DeleteLinefeed(picturefileA_path);

		//画像Bを取得
		fgets(picturefileB_path, _MAX_PATH, fp);
		DeleteLinefeed(picturefileB_path);
	}
	else{
		//画像Aを取得
		fgets(str, 64, fp);
		DeleteLinefeed(str);
		if( strcmp(str, "!") == 0 ){
			strcpy(picturefileA_path, "!");
		}
		else{
			//「data\\briefing\\　～　.bmp」を生成
			strcpy(picturefileA_path, "data\\briefing\\");
			strcat(picturefileA_path, str);
			strcat(picturefileA_path, ".bmp");
		}

		//画像Bを取得
		fgets(str, 64, fp);
		DeleteLinefeed(str);
		if( strcmp(str, "!") == 0 ){
			strcpy(picturefileB_path, "!");
		}
		else{
			//「data\\briefing\\　～　.bmp」を生成
			strcpy(picturefileB_path, "data\\briefing\\");
			strcat(picturefileB_path, str);
			strcat(picturefileB_path, ".bmp");
		}

		//背景空の番号
		fgets(str, 16, fp);
		DeleteLinefeed(str);
		skynumber = atoi(str);

		//取得できない値の初期化
		strcpy(mission_name, "");
		strcpy(mission_fullname, "");
		strcpy(blockfile_path, "");
		strcpy(pointfile_path, "");
		strcpy(addsmallobject_path, "");
		collision = false;
		screen = false;
	}

	//ブリーフィングテキストを取得
	strcpy(briefingtext, "");
	for(int i=0; i<17; i++ ){
		if( fgets(str, 50, fp) == NULL ){ break; }
		strcat(briefingtext, str);
		datas += 1;
	}

	//ファイルハンドルを開放
	fclose( fp );

#ifdef ENABLE_DEBUGLOG
	//ログに出力
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif


	//追加小物情報を初期値へ
	strcpy(addsmallobject_modelpath, "");
	strcpy(addsmallobject_texturepath, "");
	addsmallobject_decide = 0;
	addsmallobject_hp = 0;
	strcpy(addsmallobject_soundpath, "");
	addsmallobject_jump = 0;

	//何かしらの追加小物情報ファイルが指定されていれば
	if( (strcmp(addsmallobject_path, "") != 0)&&(strcmp(addsmallobject_path, "!") != 0) ){

#ifdef ENABLE_DEBUGLOG
		//ログに出力
		OutputLog.WriteLog(LOG_LOAD, "（追加小物）", addsmallobject_path);
#endif

#ifdef PATH_DELIMITER_SLASH
		//パス区切り文字を変換
		strcpy(addsmallobject_path, ChangePathDelimiter(addsmallobject_path));
#endif

		//ファイルを開く
		fp = fopen( addsmallobject_path, "r" );
		if( fp != NULL ){
			//モデルデータパス
			fgets(addsmallobject_modelpath, _MAX_PATH, fp);
			DeleteLinefeed(addsmallobject_modelpath);

			//テクスチャパス
			fgets(addsmallobject_texturepath, _MAX_PATH, fp);
			DeleteLinefeed(addsmallobject_texturepath);

			//当たり判定の大きさ
			fgets(str, 16, fp);
			DeleteLinefeed(str);
			addsmallobject_decide = atoi(str);

			//耐久力
			fgets(str, 16, fp);
			DeleteLinefeed(str);
			addsmallobject_hp = atoi(str);

			//サウンドデータパス
			fgets(addsmallobject_soundpath, _MAX_PATH, fp);
			DeleteLinefeed(addsmallobject_soundpath);

			//飛び具合
			fgets(str, 16, fp);
			DeleteLinefeed(str);
			addsmallobject_jump = atoi(str);

			//ファイルハンドルを開放
			fclose( fp );
		}

#ifdef ENABLE_DEBUGLOG
		//ログに出力
		OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
	}
#ifdef ENABLE_DEBUGLOG
	else{
		//ログに出力
		OutputLog.WriteLog(LOG_CHECK, "（追加小物）", "ファイルなし");
	}
#endif

	return 0;
}

//! @brief 読み込んだデータファイルの形式を取得
//! @return 標準形式：false　MIF形式：true
//! @attention ファイルを正常に読み込んだ後に実行してください。
bool MIFInterface::GetFiletype()
{
	return mif;
}

//! @brief ミッション識別名を取得
//! @return 識別名のポインタ（最大：24）
char* MIFInterface::GetMissionName()
{
	return mission_name;
}

//! @brief ミッション正式名称を取得
//! @return 正式名称のポインタ（最大：64）
char* MIFInterface::GetMissionFullname()
{
	return mission_fullname;
}

//! @brief ブロックデータとポイントデータのパスを取得
//! @param *blockfile ブロックデータを受け取るポインタ
//! @param *pointfile ポイントデータを受け取るポインタ
void MIFInterface::GetDatafilePath(char *blockfile, char *pointfile)
{
	strcpy(blockfile, blockfile_path);
	strcpy(pointfile, pointfile_path);
}

//! @brief 背景空を取得
//! @return 空の番号（0～5）
//! @attention 番号 0 は「背景なし」を意味します。
int MIFInterface::GetSkynumber()
{
	return skynumber;
}

//! @brief ブリーフィング画像ファイルのパスを取得
//! @param *picturefileA 画像ファイルAのパスを受け取るポインタ
//! @param *picturefileB 画像ファイルBのパスを受け取るポインタ
//! @attention 設定されていない場合は「!」が返されます。
//! @attention 画像を1枚しか使用しない場合、画像ファイルBは「!」を返します。
void MIFInterface::GetPicturefilePath(char *picturefileA, char *picturefileB)
{
	strcpy(picturefileA, picturefileA_path);
	strcpy(picturefileB, picturefileB_path);
}

//! @brief ブリーフィング文章（本文）を取得
//! @return 文章のポインタ（最大：816）
//! @attention 改行コードも含めて、最大17行分が一度に返されます。
//! @attention 行数は GetTotaldatas() で取得できます。
char* MIFInterface::GetBriefingText()
{
	return briefingtext;
}

//! @brief 追加のあたり判定を示すフラグを取得
//! @return 有効：true　無効：false
bool MIFInterface::GetCollisionFlag()
{
	return collision;
}

//! @brief 画面を暗くを示すフラグを取得
//! @return 有効：true　無効：false
bool MIFInterface::GetScreenFlag()
{
	return screen;
}

//! @brief 追加小物のモデルデータパスを取得
//! @return モデルデータパスのポインタ（最大：_MAX_PATH）
char* MIFInterface::GetAddSmallobjectModelPath()
{
	return addsmallobject_modelpath;
}

//! @brief 追加小物のテクスチャパスを取得
//! @return テクスチャパスのポインタ（最大：_MAX_PATH）
char* MIFInterface::GetAddSmallobjectTexturePath()
{
	return addsmallobject_texturepath;
}

//! @brief 追加小物の当たり判定の大きさを取得
//! @return 当たり判定の大きさ
int MIFInterface::GetAddSmallobjectDecide()
{
	return addsmallobject_decide;
}

//! @brief 追加小物の耐久力を取得
//! @return 耐久力
int MIFInterface::GetAddSmallobjectHP()
{
	return addsmallobject_hp;
}

//! @brief 追加小物のサウンドデータパスを取得
//! @return サウンドデータパスのポインタ（最大：_MAX_PATH）
char* MIFInterface::GetAddSmallobjectSoundPath()
{
	return addsmallobject_soundpath;
}

//! @brief 追加小物の飛び具合を取得
//! @return 飛び具合
int MIFInterface::GetAddSmallobjectJump()
{
	return addsmallobject_jump;
}

//! @brief コンストラクタ
AddonList::AddonList()
{
	datas = 0;

	for(int i=0; i<MAX_ADDONLIST; i++){
		strcpy(filename[i], "");
		strcpy(mission_name[i], "");
	}
}

//! @brief ディストラクタ
AddonList::~AddonList()
{}

//! @brief .mifファイルを取得
void AddonList::GetMIFlist(char *dir)
{
	char SearchDIR[_MAX_PATH];
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	//.mifの検索条件を生成
	strcpy(SearchDIR, dir);
	strcat(SearchDIR, "\\*.mif");

	//検索
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

//! @brief ミッション名を取得
void AddonList::GetMissionName(char *dir)
{
	char str[_MAX_PATH];
	MIFInterface mifdata;

	for(int i=0; i<datas; i++){
		//ファイル名を生成
		strcpy(str, dir);
		strcat(str, "\\");
		strcat(str, filename[i]);

		//MIFInterfaceで読み込む
		mifdata.LoadFiledata(str);

		//ミッション名を取得
		strcpy(mission_name[i], mifdata.GetMissionName());
	}
}

//! @brief ミッション名をソートする
void AddonList::Sort()
{
	char mission_name_c[MAX_ADDONLIST][24];
	char temp[_MAX_PATH];
	int cmp;

	//ミッション名を一度小文字に変換する
	for(int i=0; i<datas; i++){
		for(int j=0; j<(int)strlen(mission_name[i]); j++){
			mission_name_c[i][j] = (char)tolower(mission_name[i][j]);
		}
		mission_name_c[i][strlen(mission_name[i])] = 0x00;
	}

	//低速なバブルソート　（＾＾；
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

//! @brief ADDONリストを取得する
//! @param dir ADDON（.mif）が入ったディレクトリ　（標準："addon\\"）
//! @return addonの総数
int AddonList::LoadFiledata(char *dir)
{
	datas = 0;

	//.mifファイルを取得
	GetMIFlist(dir);

	//ミッション名を取得
	GetMissionName(dir);

	//ミッション名をソートする
	Sort();

#ifdef ENABLE_DEBUGLOG
	char str[128];
	sprintf(str, "addonフォルダ調査（ディレクトリ：%s　合計数：%d）", dir, datas);

	//ログに出力
	OutputLog.WriteLog(LOG_CHECK, "ディレクトリ", str);
#endif

	return datas;
}

//! @brief ミッション名を取得
//! @param id 認識番号
//! @return ミッション名
char* AddonList::GetMissionName(int id)
{
	return mission_name[id];
}

//! @brief ファイル名を取得
//! @param id 認識番号
//! @return ファイル名
char* AddonList::GetFileName(int id)
{
	return filename[id];
}


//! @brief fgets()用　改行コードを取り除く
//! @param str 文字列
//! @return 成功：0　失敗：1
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
