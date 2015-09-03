//! @file input.h
//! @brief InputControl�N���X�̐錾

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

#ifndef INPUT_H
#define INPUT_H

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

#include <windows.h>

//#define INPUT_DIRECTINPUT	//!< @brief ���͂��擾����C���^�[�t�F�[�X�̑I�� @details �萔�錾�L���FDirectInput�@�萔�錾�����i�R�����g���j�FWinAPI

#ifdef INPUT_DIRECTINPUT
 #define DIRECTINPUT_VERSION 0x0800	//!< �x���uDIRECTINPUT_VERSION undefined. Defaulting to version 0x0800�v��h�~
 #include <dinput.h>

 #pragma comment(lib, "dinput8.lib")
 #pragma comment(lib, "dxguid.lib")
#endif

//! @brief ���̓f�o�C�X���Ǘ�����N���X
//! @details �}�E�X��L�[�{�[�h�Ȃǂ̓��̓f�o�C�X���Ǘ����Ă��܂��B
class InputControl
{
	HWND hWnd;				//!< �E�C���h�E�n���h��
#ifdef INPUT_DIRECTINPUT
	LPDIRECTINPUT8 pDI;					//!< DIRECTINPUT8�̃|�C���^
	LPDIRECTINPUTDEVICE8 pDIDevice;		//!< DIRECTINPUTDEVICE8�̃|�C���^�E�L�[�{�[�h�p
	LPDIRECTINPUTDEVICE8 pMouse;		//!< DIRECTINPUTDEVICE8�̃|�C���^�E�}�E�X�p
#endif
	char keys[256];			//!< �L�[�����i�[����z��
	char keys_lt[256];		//!< �i�O��́j�L�[�����i�[����z��
	int mx;				//!< �}�E�X��X���W
	int my;				//!< �}�E�X��Y���W
	POINT point_lt;		//!< �i�O��́j�}�E�X���W
	bool mbl;			//!< �}�E�X�̍��{�^��
	bool mbr;			//!< �}�E�X�̉E�{�^��
	bool mbl_lt;		//!< �i�O��́j�}�E�X�̍��{�^��
	bool mbr_lt;		//!< �i�O��́j�}�E�X�̉E�{�^��

public:
	InputControl();
	~InputControl();
	int InitInput(WindowControl *WindowCtrl);
	void GetInputState(bool mousemode);
	void MoveMouseCenter();
	bool CheckKeyNow(int id);
	bool CheckKeyDown(int id);
	bool CheckKeyUp(int id);
	void GetMouseMovement(int *x, int *y);
	bool CheckMouseButtonNowL();
	bool CheckMouseButtonDownL();
	bool CheckMouseButtonUpL();
	bool CheckMouseButtonNowR();
	bool CheckMouseButtonDownR();
	bool CheckMouseButtonUpR();
};

int OriginalkeycodeToDinputdef(int code);
bool GetDoubleKeyCode(int id, int *CodeL, int *CodeR);
int GetEscKeycode();
int GetHomeKeycode();
int GetFunctionKeycode(int key);

#endif