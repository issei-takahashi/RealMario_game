#pragma once

#include "Point.hpp"

struct macro{

	//ゲーム画面サイズ
	static int X_SCREEN_SIZE_DEFAULT;
	static int Y_SCREEN_SIZE_DEFAULT;
	//ゲーム画面中央
	static Point<int> SCREEN_CENTER;
	//円周率
	static double PI;
	//誤差
	static double EPS;
	// デバッグビルドかどうか
	static bool DBG_FLAG;
	// キー入力の最大フレーム数
	static int KEY_INPUT_FRAME_MAX;
};


#define foreach(it,container) for(auto (it)=(container).begin();(it)!=(container).end();(it)++)
