#include "Timer.hpp"
#include <DxLib.h>

//static変数の初期化
double Timer::lastTime;
double Timer::frameTime;
int Timer::frameNum;


 //最終呼び出し時間の記録
void Timer::setLastTime()
{
	lastTime = (double)(DxLib::GetNowCount()/1000.0);
	frameNum++;
}

//1フレームにかかった時間を計測
void Timer::setFrameTime()
{
	// 最新のフレーム時間を記録
	frameTime = (double)(DxLib::GetNowCount()/1000.0) - lastTime;
}
