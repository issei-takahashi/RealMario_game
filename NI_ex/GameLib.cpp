#include "GameLib.hpp"

#include <DxLib.h> // ここでのみインクルードする

#define HERE GameLib

// ライブラリ初期化
int HERE::initLib()
{
	//ウィンドウモードにする
	DxLib::ChangeWindowMode(TRUE);
	DxLib::SetMainWindowText( "苗村研究室展示「実空間ゲーム」" ) ;
	DxLib::SetGraphMode( SCR_X , SCR_Y , 16 ) ;
	//DxLib::SetBackgroundColor(16,16,16);

	int ret = DxLib::DxLib_Init();

	//裏側描画に設定
	DxLib::SetDrawScreen(DX_SCREEN_BACK);

	// 窓のサイズを可変に
	//DxLib::SetWindowSizeChangeEnableFlag( true ) ;

	//マウスを非表示状態にする
	//DxLib::SetMouseDispFlag(false);

	// デバッグ用コンソールを呼び出す
	AllocConsole();
	freopen("CONOUT$", "w", stdout); 
	freopen("CONIN$", "r", stdin);

	// ウィンドウが非アクティブでも動作
	DxLib::SetAlwaysRunFlag( true ) ;

	// フォント初期化
	DxLib::ChangeFontType( DX_FONTTYPE_EDGE ) ;

	return ret;
}

// ライブラリ終了
int HERE::endLib()
{
	return DxLib::DxLib_End();
}


/* 描画 */

int HERE::draw::DrawCircle( int _x, int _y, int _r, int _col, int _fillFlag )
{
	return DxLib::DrawCircle(_x,_y,_r,_col,_fillFlag);
}

int HERE::draw::ScreenFlip()
{
	return DxLib::ScreenFlip();
}