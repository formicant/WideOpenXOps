//! @file event.h
//! @brief EventControlクラスの宣言

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

#ifndef EVENT_H
#define EVENT_H

#define TOTAL_EVENTFRAMESTEP 6			//!< 1フレーム中に1ラインあたり実行するステップ数
#define DISTANCE_CHECKPOINT 25.0f		//!< 「到着待ち」「ケース待ち」の判定距離

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! @brief イベントを処理するクラス
//! @details ミッションのイベントポイントを処理します。
//! @details このクラスは1つのイベント処理（流れ）に専念します。複数のイベントを並列に処理させる場合は、このクラスのオブジェクトを複数呼び出す必要があります。（例えば3つ同時に処理させる場合、このクラスのオブジェクトを3つ作成してください。）
class EventControl
{
	class PointDataInterface *Point;		//!< PointDataInterfaceクラスのポインタ
	class ObjectManager *ObjMgr;			//!< ObjectManagerクラスのポインタ
	signed char nextp4;				//!< 次に処理するP4：認識番号
	int waitcnt;					//!< 時間待ち用カウント

	bool CheckHaveCase(human *in_human);

public:
	EventControl(PointDataInterface *in_Point = NULL, ObjectManager *in_ObjMgr = NULL);
	~EventControl();
	void SetClass(PointDataInterface *in_Point, ObjectManager *in_ObjMgr);
	void Reset(signed char EntryP4);
	signed char GetNextP4();
	int Execution(int *endcnt, bool *complete, int *MessageID, bool *SetMessageID);
};

#endif