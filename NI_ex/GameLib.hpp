#pragma once
#include "includes.hpp"

namespace GameLib
{
	// ライブラリ初期化
	int initLib();
	// ライブラリ終了
	int endLib();

	namespace draw{
		int DrawCircle( int _x, int _y, int _r, int _col, int _fillFlag );
		int	ScreenFlip() ;
	}
};

#define SCR_X 800
#define SCR_Y 600
