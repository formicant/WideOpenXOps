//! @file window.cpp
//! @brief ���C���E�B���h�E�̍쐬�E����

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

#include "window.h"

LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//! ���C���E�B���h�E�쐬
//! @param hInstance �C���X�^���X �n���h��
//! @param title �E�B���h�E�^�C�g��
//! @param width �E�B���h�E�̕�
//! @param height �E�B���h�E�̍���
//! @param nCmdShow �E�B���h�E�̕\�����
//! @param fullscreen false�F�E�B���h�E�\���@true�F�t���X�N���[���p�\��
//! @return �E�B���h�E�n���h��
HWND InitWindow(HINSTANCE hInstance, char* title, int width, int height, int nCmdShow, bool fullscreen)
{
	WNDCLASS wc;
	//int x, y;
	//int w, h;
	RECT Rect;
	DWORD dwStyle;
	HWND hWnd;

	//�E�B���h�E�N���X�̓o�^
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	= WindowProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= hInstance;
	wc.hIcon		= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(MAINICON));
	wc.hCursor		= NULL;
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName	= NULL;
	wc.lpszClassName	= "MainWindow";
	if( !RegisterClass(&wc) ){
		return NULL;
	}

	if( fullscreen == false ){
		dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
	}
	else{
		dwStyle = WS_POPUP;
	}

	//�E�B���h�E�T�C�Y���v�Z
	Rect.left = 0;
	Rect.top = 0;
	Rect.right = width;
	Rect.bottom = height;
	AdjustWindowRect(&Rect, dwStyle, FALSE);
	width = Rect.right - Rect.left;
	height = Rect.bottom - Rect.top;

	//�E�B���h�E�쐬
	hWnd = CreateWindow( "MainWindow", title, dwStyle, 0, 0, width, height, NULL, NULL, hInstance, NULL );
	if( hWnd == NULL ){
		return NULL;
	}

	//�\��
	ShowWindow( hWnd, nCmdShow );

	return hWnd;

}

//! ���C���E�B���h�E�̃E�B���h�E�v���V�[�W��
LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg ){
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//! fps�iFrames Per Second�F�t���[�����[�g�j�v�Z
//! @param getcnt fps���擾��������i�t���[���P�ʁj
//! @return fps��
float GetFps(int getcnt)
{
	static unsigned int ptimeg = 0;
	unsigned int nowtime;
	static float pfps = 0.0f;
	static int cnt = 0;

	if( cnt == 0 ){
		ptimeg = GetTimeMS();
	}
	if( cnt == getcnt ){
		nowtime = GetTimeMS();
		pfps = 1000.0f / ((nowtime - ptimeg)/getcnt);
		cnt = 0;
	}
	else{
		cnt += 1;
	}

	return pfps;
}

//! fps�iFrames Per Second�F�t���[�����[�g�j����
//! @return ���������{�Ftrue�@�@���������{�����Ffalse
bool ControlFps()
{
	static unsigned int ptimec = 0;
	unsigned int nowtime;
	int waittime;

	nowtime = GetTimeMS();
	waittime = GAMEFRAMEMS - (nowtime - ptimec);
	if( waittime > 0 ){
		Sleep(waittime);
		ptimec = GetTimeMS();
		return true;
	}

	ptimec = nowtime;
	return false;
}

//! �~���b�P�ʂ�Ԃ�
//! @return �~���b
unsigned int GetTimeMS()
{
	unsigned int time;

	timeBeginPeriod(1);
	time = timeGetTime();
	timeEndPeriod(1);

	return time;
}

//! ������������
void InitRand()
{
	srand(GetTimeMS());
}

//! �����_���Ȑ����l��Ԃ�
//! @param num �͈�
//! @return 0�`num-1
int GetRand(int num)
{
	return rand()%num;

	//return rand() / (RAND_MAX/num);

	//�{��XOPS�̃A���S���Y���H
	//static int memory = GetTimeMS();
	//int x;
	//memory = memory * 214013 + 2745024;
	//x = memory >> 16;
	//x = x & 0x00007FFF;
	//return x%num;
}
