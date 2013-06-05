#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include "Block.h"
#include "Wave.hpp"
#include <OpenNI.h>
#include <opencv2\opencv.hpp>

class RealField
{
interface__:
	static void mainLoop();
	// カメラの初期化
	static void initCamera();
	// 実空間に関する各種パラメータの初期化
	static void initParams();

	// get
	// 左からx番目(X==0,1,2..)の段数を取得
	static int getDan( int _line, int _x );

	// get_
	static int get_blockNumX() { return blockNumX; } 
	static vector<int> get_v_blockDan( int _line ) { return v_blockDan[_line]; }
	static map<int,int> get_m_highest() { return m_highest; }
	static int getBlockSideScr0() { return v_blockSideScr.at(0); }

inner__:
	// 深さ画像を更新してフィールド情報を更新
	static int updateDepthImageAndFieldInfo( int _line );
	// 色画像を更新する
	static void updateColorImage();
	// 深さ画像を解析してfieldArrを更新する
	static void updateFieldInfo( int _line );
	// キー入力を元にfieldArrを更新する
	static void updateFieldArrWithoutKinect();
	// 段数をdepthImageに表示
	static void drawDanToDepthImage( int _line );
	// 高さ波形を平滑化
	static void smoothHeightWave(  );

	// Mutexをロック
	static void lockMutex();
	// Mutexをアンロック
	static void unlockMutex();

capcelled__:
	static vector<int> v_blockDan[3];           // 今積まれてるブロックの段数

open__:
	static Wave heightWave;   // 高さ波形
	static Wave smoothedWave; // 高さ波形を平滑化したもの

inner__:
	/* Kinectからとったもの */
	static cv::Mat depthImage;        // 深さ画像
	static cv::Mat colorImage;        // カラー画像
	/* カメラ関連 */
	static openni::VideoStream            depthStream;
	static vector<openni::VideoStream*>   v_pDepthStream;
	static openni::VideoStream            colorStream;
	static vector<openni::VideoStream*>   v_pColorStream;
	static openni::Device                 device;
	/* 実空間のパラメータ */
	static int deskDepth; // 机までの深さ(mm)
	static int blockSide; // ブロックの一辺(mm)
	static int errLen;    // 認識の許容誤差(mm)
	static int blockNumX;          // x方向のブロックの最大数
	static int blockNumY;          // y方向のブロックの最大数
	static int blockNumZ;          // z方向のブロックの最大数
	static vector<int> v_blockSideScr;      // 投影変換後の画面上でのブロックの1辺(key:下から数えたブロックの数)
	static map<int,int> m_highest;        // 最も高い場所一覧
	/* スレッド関連 */
	static HANDLE hMutex; // ミューテックスハンドル
	static bool isMutexLocked; // ミューテックスがロックされているかどうか
	/* その他 */
	static bool kinectUseFlag; // Kinectを使うかどうか。falseのときはゲームのみで動作する。
};
