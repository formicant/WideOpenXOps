//! @file main.cpp
//! @brief WinMain()�֐��̒�`����уe�X�g�v���O���� 

//--------------------------------------------------------------------------------
// 
// OpenXOPS
// Copyright (c) 2014-2017, OpenXOPS Project / [-_-;](mikan) All rights reserved.
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

//Doxygen�ݒ�t�@�C��
#include "doxygen.h"

#include "main.h"

#ifdef _DEBUG
 #include <shlwapi.h>
 #pragma comment(lib, "Shlwapi.lib")
#endif

#ifdef ENABLE_DEBUGLOG
 //! ���O�o��
 DebugLog OutputLog;
#endif

//! �X�e�[�g�}�V��
StateMachine GameState;

//! �Q�[���ݒ�f�[�^
Config GameConfig;

//! ���C���E�B���h�E
WindowControl MainWindow;

//! @brief WinMain()�֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//���g�p�����΍�
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//����������
	InitRand();

#ifdef _DEBUG
	//���������[�N�̌��o
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//���s�t�@�C���̂���ꏊ���A�J�����g�f�B���N�g���ɂ���B
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	SetCurrentDirectory(path);

	//char str[24];
	//GameConfig.GetPlayerName(str);
	//MessageBox(NULL, str, "�v���C���[��", MB_OK);
	char str[255];
	strcpy(str, "[Information]\nThe compiler is the Debug mode.\nIf release the software, Switch compiler to Release mode.");
	MessageBox(NULL, str, GAMENAME, MB_OK);
#endif

#ifdef ENABLE_DEBUGLOG
	//�����FWindows���Ȃ�AlpCmdLine�����Ƃ�GetCommandLine()�֐��ł����܂����ǂˁE�E�B

	//�����𕪉�
	for(int i=0; i<__argc; i++){

		//"log"���^�����Ă�����A���O�o�͂�L����
		if( strcmp(__argv[i], "log") == 0 ){
			//�t�@�C���쐬
			OutputLog.MakeLog();

			MainWindow.ErrorInfo("Enable Debug Log...");
		}
	}

	//���[�U�[�����o��
	GetOperatingEnvironment();

	char infostr[64];

	/*
	//���O�ɏo��
	sprintf(infostr, "%d��", __argc);
	OutputLog.WriteLog(LOG_CHECK, "����", infostr);
	for(int i=0; i<__argc; i++){
		sprintf(infostr, "����[%d]", i);
		OutputLog.WriteLog(LOG_CHECK, infostr, __argv[i]);
	}
	*/

	//���O�ɏo��
	OutputLog.WriteLog(LOG_CHECK, "�N��", "�G���g���[�|�C���g�J�n");
#endif

#ifdef _DEBUG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CHECK, "�N��", "Visual C++ �f�o�b�O���[�h");
#endif

	//�ݒ�t�@�C���ǂݍ���
	if( GameConfig.LoadFile("config.dat") == 1 ){
		MainWindow.ErrorInfo("config data open failed");
		return 1;
	}

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	sprintf(infostr, "�𑜓x�F%d x %d", SCREEN_WIDTH, SCREEN_HEIGHT);
	OutputLog.WriteLog(LOG_CHECK, "��", infostr);
	if( GameConfig.GetFullscreenFlag() == false ){
		OutputLog.WriteLog(LOG_CHECK, "��", "�E�B���h�E���[�h�F�E�B���h�E");
	}
	else{
		OutputLog.WriteLog(LOG_CHECK, "��", "�E�B���h�E���[�h�F�t���X�N���[��");
	}
#endif

	//�E�B���h�E������
	MainWindow.SetParam(hPrevInstance, nCmdShow);
	MainWindow.InitWindow(GAMENAME, SCREEN_WIDTH, SCREEN_HEIGHT, GameConfig.GetFullscreenFlag());

	//��{�I�ȏ���������
	if( InitGame(&MainWindow) ){
		return 1;
	}

	opening Opening;
	mainmenu MainMenu;
	briefing Briefing;
	maingame MainGame;
	result Result;
	InitScreen(&MainWindow, &Opening, &MainMenu, &Briefing, &MainGame, &Result);


#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CHECK, "�N��", "���C�����[�v�˓�");
#endif

	unsigned int framecnt = 0;

	for(int flag = 0; flag != -1; flag = MainWindow.CheckMainLoop()){
		if( flag == 1 ){
			//���C������
			ProcessScreen(&MainWindow, &Opening, &MainMenu, &Briefing, &MainGame, &Result, framecnt);

			//FPS����
			ControlFps();

			framecnt++;
		}
	}

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CHECK, "�I��", "���C�����[�v�E�o");
#endif


	//��{�I�ȉ������
	CleanupGame();

#ifdef ENABLE_DEBUGLOG
	//���O�ɏo��
	OutputLog.WriteLog(LOG_CHECK, "�I��", "�G���g���[�|�C���g�I��");
#endif
	return 0;
}