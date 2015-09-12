//! @file debug.cpp
//! @brief DebugLogクラスの定義

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

#include "debug.h"

#ifdef ENABLE_DEBUGLOG

//! @brief コンストラクタ
DebugLog::DebugLog()
{
	OutputFlag = false;
	strcpy(fname, GAMENAME"_log.html");
	error = false;
}

//! @brief ディストラクタ
DebugLog::~DebugLog()
{
	//出力フラグを有効ならば、ファイルにフッターを追加
	if( OutputFlag == true ){
		FILE *fp;

		fp = fopen(fname, "a");
		if( fp == NULL ){
			return;
		}

		fprintf(fp, "</table>\n");
		fprintf(fp, "</body>\n");
		fprintf(fp, "</html>\n");

		fclose(fp);
	}
}

//! @brief ログファイルのファイル名を変更
//! @param name ファイル名
void DebugLog::SetLogName(char* name)
{
	strcpy(fname, name);
}

//! @brief ログファイルを新規作成
//! @return 成功：false　失敗:true
//! @warning この関数を正しく実行しないと、クラス自体が動作しません。
//! @attention 既にファイルが存在する場合、上書き保存されます。既存のファイルは失われます。
bool DebugLog::MakeLog()
{
	FILE *fp;

	time_t timer;
	struct tm *local;

	//日時を取得
	timer = time(NULL);
	local = localtime(&timer);


	fp = fopen(fname, "w");
	if( fp == NULL ){
		return true;
	}

	//ヘッダー
	fprintf(fp, "<html>\n");
	fprintf(fp, "<head>\n");
	fprintf(fp, "<title>%s %s Debug Log</title>\n", GAMENAME, GAMEVERSION);
	fprintf(fp, "<style type=text/css>\n");
	fprintf(fp, "<!--//\n");
	fprintf(fp, "body{ font-size : 12px; }\n");
	fprintf(fp, "table { font-size : 12px; }\n");
	fprintf(fp, "//-->\n");
	fprintf(fp, "</style>\n");
	fprintf(fp, "</head>\n");

	fprintf(fp, "<body>\n");

	//ページ上部
	fprintf(fp, "<p>\n");
	fprintf(fp, "<b>%s %s 実行ログ</b><br>\n", GAMENAME, GAMEVERSION);
	fprintf(fp, "日時：%04d年%02d月%02d日　%02d時%02d分%02d秒<br>\n", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	fprintf(fp, "</p>\n");

	fprintf(fp, "<table>\n");

	fclose(fp);

	//出力フラグを有効
	OutputFlag = true;

	return false;
}

//! @brief ログファイルに追記
//! @param tag 出力モードを表す定数
//! @param title タイトル
//! @param id ID
//! @return 成功：false　失敗:true
//! @attention WriteLog(int tag, char* title, char* text)関数をオーバーロードします。
bool DebugLog::WriteLog(int tag, char* title, int id)
{
	//出力フラグを有効なければ処理しない
	if( OutputFlag == false ){ return true; }

	char str[16];
	sprintf(str, "ID：%d", id);

	//オーバーロード
	return WriteLog(tag, title, str);
}

//! @brief ログファイルに追記
//! @param tag 出力モードを表す定数
//! @param title タイトル
//! @param text 本文
//! @return 成功：false　失敗:true
//! @attention 前回、初期化（LOG_INIT）か読み込み（LOG_LOAD）が指定された状態で、完了（LOG_COMPLETE）が指定されなかった場合、先にエラー（LOG_ERROR）情報を記憶します。
//! @attention すなわち、初期化か読み込みを記録した場合、次に完了が記録されなければ、自動的にエラーとして記録します。
bool DebugLog::WriteLog(int tag, char* title, char* text)
{
	//出力フラグを有効なければ処理しない
	if( OutputFlag == false ){ return true; }

	FILE *fp;

	char tagcolor[32];
	char tagstr[32];

	//完了情報か素直にエラーを伝えてきたなら、エラーフラグを下す
	if( (tag == LOG_COMPLETE)||(tag == LOG_ERROR) ){
		error = false;
	}

	//エラーフラグが有効ならば、エラーが発生したものとして記録。
	if( error == true ){
		error = false;
		WriteLog(LOG_ERROR, "", "");
	}

	//定数から文字と色を求める
	switch(tag){
		case LOG_CHECK:
			strcpy(tagcolor, "008000");
			strcpy(tagstr, "情報");
			break;
		case LOG_INIT:
			strcpy(tagcolor, "FF0080");
			strcpy(tagstr, "初期化");
			break;
		case LOG_LOAD:
			strcpy(tagcolor, "0000FF");
			strcpy(tagstr, "読み込み");
			break;
		case LOG_CLEANUP:
			strcpy(tagcolor, "FF8000");
			strcpy(tagstr, "解放");
			break;
		case LOG_COMPLETE:
			strcpy(tagcolor, "008000");
			strcpy(tagstr, "成功");
			break;
		case LOG_ERROR:
			strcpy(tagcolor, "FF0000");
			strcpy(tagstr, "失敗");
			break;
		default:
			strcpy(tagcolor, "000000");
			strcpy(tagstr, "");
	}

	fp = fopen(fname, "a");
	if( fp == NULL ){
		return true;
	}

	//現在のミリ秒
	fprintf(fp, "<tr><td>　%d　</td>", GetTimeMS());

	if( (tag != LOG_COMPLETE)&&(tag != LOG_ERROR) ){
		//通常の情報
		fprintf(fp, "<td width=100><b><font color=#%s>%s</font></b></td>", tagcolor, tagstr);
		fprintf(fp, "<td width=100>%s</td><td colspan=3>%s</td>", title, text);
	}
	else{
		//完了・エラー情報
		fprintf(fp, "<td colspan=2></td><td width=100><b><font color=#%s>%s</font></b></td>", tagcolor, tagstr);
		fprintf(fp, "<td>%s</td><td>%s</td>", title, text);
	}

	fprintf(fp, "</tr>\n");

	fclose(fp);

	//初期化か読み込みなら、エラーフラグを立てる
	if( (tag == LOG_INIT)||(tag == LOG_LOAD) ){
		error = true;
	}

	return false;
}

#endif	//ENABLE_DEBUGLOG