//! @file gamemain.h
//! @brief �Q�[�����C�������̃w�b�_�[

//--------------------------------------------------------------------------------
// 
// OpenXOPS
// Copyright (c) 2014-2018, OpenXOPS Project / [-_-;](mikan) All rights reserved.
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

#ifndef GAMEMAIN_H
#define GAMEMAIN_H

#define MAINMENU_X (SCREEN_WIDTH - 360)			//!< ���j���[�̕\�� X���W�i�����j
#define MAINMENU_Y (SCREEN_HEIGHT - 340)		//!< ���j���[�̕\�� Y���W�i�V�j
#define TOTAL_MENUITEMS 8						//!< ���j���[1��ʂɕ\������~�b�V������
#define MAINMENU_H (TOTAL_MENUITEMS+2)*30 + 25	//!< ���j���[�̕\���T�C�Y�E����

#define INPUT_ARROWKEYS_ANGLE DegreeToRadian(4)		//!< �����L�[�ł̉�]�p�x
#define INPUT_F1NUMKEYS_ANGLE DegreeToRadian(2)		//!< �O�l�̎��_�ł̃e���L�[�̉�]�p�x

#define HUDA_WEAPON_POSX (SCREEN_WIDTH - 255)	//!< �������`�悷��̈�EX���W
#define HUDA_WEAPON_POSY (SCREEN_HEIGHT - 98)	//!< �������`�悷��̈�EY���W
#define HUDA_WEAPON_SIZEW 8		//!< �������`�悷��̈�E���T�C�Y�i32�s�N�Z���̔z�u���j
#define HUDA_WEAPON_SIZEH 3		//!< �������`�悷��̈�E�c�T�C�Y�i32�s�N�Z���̔z�u���j

#define VIEW_HEIGHT 19.0f							//!< ���_�̍���
#define VIEW_DIST 0.1f								//!< ���S���王�_�܂ł̋���
#define VIEW_F1MODE_ANGLE DegreeToRadian(-22.5f)	//!< �O�l�̎��_�ł̎��_�p�x�i�����j
#define VIEW_F1MODE_DIST 14.0f						//!< �O�l�̎��_�ł̎��_�����i�ő�j
#define VIEW_FREECAMERA_SCALE 1.5f					//!< �t���[�J�����ł̈ړ����x�i�������[�h�́~2�j
#define VIEWANGLE_NORMAL DegreeToRadian(65)			//!< ����p�@�W��
#define VIEWANGLE_SCOPE_1 DegreeToRadian(30)		//!< ����p�@�X�R�[�v1
#define VIEWANGLE_SCOPE_2 DegreeToRadian(15)		//!< ����p�@�X�R�[�v2

#define TOTAL_EVENTLINE 3				//!< �C�x���g�̃��C����
#define TOTAL_EVENTENTRYPOINT_0 -100	//!< ���C�� 0 �̊J�n�F���ԍ�
#define TOTAL_EVENTENTRYPOINT_1 -110	//!< ���C�� 1 �̊J�n�F���ԍ�
#define TOTAL_EVENTENTRYPOINT_2 -120	//!< ���C�� 2 �̊J�n�F���ԍ�

#define TOTAL_EVENTENT_SHOWMESSEC 5.0f		//!< �C�x���g���b�Z�[�W��\������b��

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

#ifdef ENABLE_DEBUGCONSOLE
 #define MAX_CONSOLELEN 78		//!< �f�o�b�N�p�R���\�[���̕������i�s�j
 #define MAX_CONSOLELINES 12	//!< �f�o�b�N�p�R���\�[���̍s��
 #define CONSOLE_PROMPT ">"		//!< �f�o�b�N�p�R���\�[���̃v�����v�g
#endif

int InitGame(WindowControl *WindowCtrl);
void CleanupGame();

//! �Q�[���̏�Ԃ��󂯓n������\����
struct GameInfo{
	bool selectaddon;		//!< addon��I��
	int selectmission_id;	//!< �I�����ꂽ�~�b�V����
	bool missioncomplete;	//!< �~�b�V��������
	unsigned int framecnt;	//!< �t���[����
	int fire;		//!< �ˌ���
	float ontarget;	//!< ������
	int kill;		//!< �|�����G�̐�	
	int headshot;	//!< �G�̓����ɖ���������
};

#ifdef ENABLE_DEBUGCONSOLE
//! �R���\�[�����Ǘ�����\����
struct ConsoleData{
	int color;						//!< �F
	char textdata[MAX_CONSOLELEN+1];	//!< ������
};
#endif

//! @brief �I�[�v�j���O��ʊǗ��N���X
//! @details �I�[�v�j���O��ʂ��Ǘ����܂��B
class opening : public D3Dscene
{
	float add_camera_x;			//!< �J�������W X�̑�����
	float add_camera_y;			//!< �J�������W Y�̑�����
	float add_camera_z;			//!< �J�������W Z�̑�����
	float add_camera_rx;		//!< �J������]�p�x RX�̑�����
	float add_camera_ry;		//!< �J������]�p�x RY�̑�����
	//int opening_banner;		//!< �I�[�v�j���O�ŕ\������e�N�X�`��ID
	void Render3D();
	void Render2D();

public:
	opening();
	~opening();
	int Create();
	int Recovery();
	void Input();
	void Process();
	void Destroy();
};

//! @brief ���j���[��ʊǗ��N���X
//! @details ���j���[��ʂ��Ǘ����܂��B
class mainmenu : public D3Dscene
{
	char demopath[MAX_PATH];		//!< �f���t�@�C���̃p�X
	int mainmenu_mouseX;		//!< ���j���[��ʃ}�E�XX���W
	int mainmenu_mouseY;		//!< ���j���[��ʃ}�E�XY���W
	int mainmenu_scrollitems_official;	//!< ���j���[��ʂ̃X�N���[�������A�C�e����
	int mainmenu_scrollitems_addon;		//!< ���j���[��ʂ̃X�N���[�������A�C�e����
	float mainmenu_scrollbar_official_height;	//!< ���j���[��ʂ̃X�N���[���o�[�̍���
	float mainmenu_scrollbar_official_scale;	//!< ���j���[��ʂ̃X�N���[���o�[�̖ڐ�
	int mainmenu_scrollbar_official_y;			//!< ���j���[��ʂ̃X�N���[���o�[��Y���W
	float mainmenu_scrollbar_addon_height;		//!< ���j���[��ʂ̃X�N���[���o�[�̍���
	float mainmenu_scrollbar_addon_scale;		//!< ���j���[��ʂ̃X�N���[���o�[�̖ڐ�
	int mainmenu_scrollbar_addon_y;				//!< ���j���[��ʂ̃X�N���[���o�[��Y���W
	bool mainmenu_scrollbar_flag;		//!< ���j���[��ʂ̃X�N���[���o�[�𑀍쒆�������t���O
	int gametitle;				//!< �Q�[���^�C�g���摜
	void Render3D();
	void Render2D();

public:
	mainmenu();
	~mainmenu();
	int Create();
	int Recovery();
	void Input();
	void Process();
	void Destroy();
};

//! @brief �u���[�t�B���O��ʊǗ��N���X
//! @details �u���[�t�B���O��ʂ��Ǘ����܂��B
class briefing : public D2Dscene
{
	bool TwoTexture;	//!< �u���[�t�B���O�摜��2���g�p
	int TextureA;	//!< �u���[�t�B���O�摜A
	int TextureB;	//!< �u���[�t�B���O�摜B
	void Render2D();

public:
	briefing();
	~briefing();
	int Create();
	int Recovery();
	void Destroy();
};

//! @brief ���C���Q�[����ʊǗ��N���X
//! @details ���C���Q�[����ʂ��Ǘ����܂��B
class maingame : public D3Dscene
{
	//class EventControl Event[TOTAL_EVENTLINE];	//!< �C�x���g����N���X
	int SkyNumber;			//!< �w�i��ԍ�
	bool DarkScreenFlag;	//!< ��ʂ��Â�
	float mouse_rx;				//!< �}�E�X�ɂ�鐅�����p�x
	float mouse_ry;				//!< �}�E�X�ɂ�鐂�����p�x
	float view_rx;				//!< �}�E�X�p�x�ƃJ�����p�x�̍��i�������j
	float view_ry;				//!< �}�E�X�p�x�ƃJ�����p�x�̍��i�������j
	float add_camera_rx;		//!< �J������]�p�x RX�̑�����
	float add_camera_ry;		//!< �J������]�p�x RY�̑�����
	bool ShowInfo_Debugmode;	//!< ���W�Ȃǂ�\������f�o�b�N���[�h
	bool Camera_Debugmode;		//!< �J�����f�o�b�N���[�h
	bool tag;					//!< �I�u�W�F�N�g�̃^�O��\��
	bool radar;					//!< �ȈՃ��[�_�[�\��
	bool wireframe;				//!< �}�b�v�����C���[�t���[���\��
	bool CenterLine;			//!< 3D��Ԃɒ��S����\��
	bool Camera_Blind;			//!< �ډB���`��
	bool Camera_F1mode;			//!< �J����F1���[�h
	int Camera_F2mode;			//!< �J����F2���[�h
	bool Camera_HOMEmode;		//!< �J����HOME���[�h
	bool Cmd_F5;				//!< ���ZF5���[�h
	int InvincibleID;			//!< ���G�Ȑl�̔���
	bool PlayerAI;				//!< �v���C���[�����AI�Ɉς˂�
	bool AIstop;				//!< AI�������~����
	bool AINoFight;				//!< AI����퓬������i���Ȃ��j
	int AIdebuginfoID;			//!< AI�̃f�o�b�N���\��
	int start_framecnt;			//!< ���C���Q�[���J�n���̃J�E���g
	int end_framecnt;			//!< ���C���Q�[���I���̃J�E���g
	bool EventStop;				//!< �C�x���g�������~����
	int GameSpeed;				//!< �Q�[���̎��s���x�i1�ȏ�̐����l�j
	int message_id;				//!< �\�����̃C�x���g���b�Z�[�W�ԍ�
	int message_cnt;			//!< �\�����̃C�x���g���b�Z�[�W�J�E���g
	bool redflash_flag;		//!< ���b�h�t���b�V���`��t���O
	int time;		//!< timer
	int time_input;					//!< ���͎擾�̏�������
	int time_process_object;		//!< ��{�I�u�W�F�N�g�̏�������
	int time_process_ai;			//!< AI�̏�������
	int time_process_event;			//!< �C�x���g�̏�������
	int time_sound;				//!< �T�E���h�̏�������
	int time_render;				//!< �`��̏�������
	GameInfo MainGameInfo;			//!< ���U���g�p�Ǘ��N���X
	bool CheckInputControl(int CheckKey, int mode);
	void InputPlayer(human *myHuman, int mouse_x, int mouse_y, float MouseSensitivity);
	void InputViewCamera(int mouse_x, int mouse_y, float MouseSensitivity);
	void Render3D();
	void Render2D();
	bool GetRadarPos(float in_x, float in_y, float in_z, int RadarPosX, int RadarPosY, int RadarSize, float RadarWorldR, int *out_x, int *out_y, float *local_y, bool check);
	void RenderRadar();

#ifdef ENABLE_DEBUGCONSOLE
	bool Show_Console;			//!< �f�o�b�N�p�R���\�[����\��
	ConsoleData *InfoConsoleData;			//!< �f�o�b�N�p�R���\�[���f�[�^�i�\���ς݁j
	ConsoleData *InputConsoleData;			//!< �f�o�b�N�p�R���\�[���f�[�^�i���͒��j
	char NewCommand[MAX_CONSOLELEN];	//!< �V���ɓ��͂��ꂽ�������̃R�}���h
	int ScreenShot;		//!< SS���B�e����
	void AddInfoConsole(int color, char *str);
	void ConsoleInputText(char inchar);
	void ConsoleDeleteText();
	bool GetCommandNum(char *cmd, int *num);
	void InputConsole();
	void ProcessConsole();
	void RenderConsole();
#endif

public:
	maingame();
	~maingame();
	int GetGameSpeed();
	int Create();
	int Recovery();
	void Input();
	void Process();
	void Sound();
	void Destroy();
};

//! @brief ���U���g��ʊǗ��N���X
//! @details ���U���g�i���ʕ\���j��ʂ��Ǘ����܂��B
class result : public D2Dscene
{
	void Render2D();

public:
	result();
	~result();
};

void InitScreen(WindowControl *WindowCtrl, opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result);
void ProcessScreen(WindowControl *WindowCtrl, opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result, unsigned int framecnt);

#endif