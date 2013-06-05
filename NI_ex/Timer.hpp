#pragma once

#include "includes.hpp"

/*------------------- Class Timer ---------------------------

�ړI�F���ݎ����i�~���b�j�̌v��
�����F���m�X�e�[�g

--------------------------------------------------------*/

class Timer
{
	friend class GameMain;

private:
	Timer(){} //��Ύg��Ȃ��B���R���X�g���N�^


	static double lastTime; //���O�̌v������(s)
	static double frameTime; //1�t���[���ɂ�����������(s)
	static int frameNum; //�@�Q�[���N��������̃t���[����

	/* ----------�����֐�(GameMain�݂̂���Ăяo��)----------*/

	static void setLastTime(); //�ŏI�v�����Ԃ̑���
	static void setFrameTime(); //1�t���[���ɂ����������Ԃ̑���
	static void setStartTime(); //�v���J�n���Ԃ̑���

public:


	/* --------------------�ǂ��ł��g����֐�--------------------*/


	/* --------------------Get()--------------------*/
	static double GetLastTime(){return lastTime;}
	static double GetFrameTime(){return frameTime;}
	static int    GetFrameNum(){ return frameNum; }
};