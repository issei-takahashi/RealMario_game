#pragma once

#include "includes.hpp"

/*------------------- Class Timer ---------------------------

目的：現在時刻（ミリ秒）の計測
属性：モノステート

--------------------------------------------------------*/

class Timer
{
	friend class GameMain;

private:
	Timer(){} //絶対使わない隠しコンストラクタ


	static double lastTime; //直前の計測時間(s)
	static double frameTime; //1フレームにかかった時間(s)
	static int frameNum; //　ゲーム起動時からのフレーム数

	/* ----------内部関数(GameMainのみから呼び出し)----------*/

	static void setLastTime(); //最終計測時間の測定
	static void setFrameTime(); //1フレームにかかった時間の測定
	static void setStartTime(); //計測開始時間の測定

public:


	/* --------------------どこでも使える関数--------------------*/


	/* --------------------Get()--------------------*/
	static double GetLastTime(){return lastTime;}
	static double GetFrameTime(){return frameTime;}
	static int    GetFrameNum(){ return frameNum; }
};