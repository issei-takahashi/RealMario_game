#include "Timer.hpp"
#include <DxLib.h>

//static�ϐ��̏�����
double Timer::lastTime;
double Timer::frameTime;
int Timer::frameNum;


 //�ŏI�Ăяo�����Ԃ̋L�^
void Timer::setLastTime()
{
	lastTime = (double)(DxLib::GetNowCount()/1000.0);
	frameNum++;
}

//1�t���[���ɂ����������Ԃ��v��
void Timer::setFrameTime()
{
	// �ŐV�̃t���[�����Ԃ��L�^
	frameTime = (double)(DxLib::GetNowCount()/1000.0) - lastTime;
}
