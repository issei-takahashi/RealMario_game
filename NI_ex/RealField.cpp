#include "RealField.hpp"
#include "IO.hpp"
#include "FT.hpp"
#include "utils.hpp"

#define HERE RealField

/* Kinectからとったもの */
cv::Mat                       HERE::depthImage;
cv::Mat                       HERE::colorImage;
/* カメラ関連 */
openni::VideoStream           HERE::depthStream;
vector<openni::VideoStream*>  HERE::v_pDepthStream;
openni::VideoStream           HERE::colorStream;
vector<openni::VideoStream*>  HERE::v_pColorStream;
openni::Device                HERE::device;
/* 実空間のパラメータ */
int                           HERE::deskDepth; // 机までの深さ(mm)
int                           HERE::blockSide; // ブロックの一辺(mm)
int                           HERE::errLen;    // 認識の許容誤差(mm)
int                           HERE::blockNumX;          // x方向のブロックの最大数
int                           HERE::blockNumY;          // y方向のブロックの最大数
int                           HERE::blockNumZ;          // z方向のブロックの最大数
vector<int>                   HERE::v_blockSideScr;      // 投影変換後の画面上でのブロックの1辺(key:下から数えたブロックの数)
vector<int>                   HERE::v_blockDan[3];           // 今積まれてるブロックの段数
map<int,int>                  HERE::m_highest;        // 最も高い場所一覧
Wave                          HERE::heightWave;     // 連続的な高さ分布
Wave                          HERE::smoothedWave;     // 連続的な高さ分布を平滑化したもの
/* スレッド関連 */
HANDLE                        HERE::hMutex = CreateMutex( NULL, FALSE, NULL);
bool                          HERE::isMutexLocked = false;
/* その他 */
bool                          HERE::kinectUseFlag = true;

void HERE::mainLoop()
{
	if(HERE::kinectUseFlag){
		while(1){
			for(int i=0;i<3;i++){
				HERE::updateDepthImageAndFieldInfo(i);
			}
			//HERE::updateColorImage();
		}
	}
	el{
		while(1){
			HERE::updateFieldArrWithoutKinect();
		}
	}
}

// カメラの初期化
void HERE::initCamera()
{
    try {
		/* OpenNIの初期化 */
        openni::OpenNI::initialize();
        auto ret = HERE::device.open( openni::ANY_DEVICE );
        if ( ret != openni::STATUS_OK ) {
            throw std::runtime_error( "" );
        }

		/* 深さカメラの初期化 */
        HERE::depthStream.create( device, openni::SensorType::SENSOR_DEPTH );
        HERE::depthStream.start();
		HERE::v_pDepthStream.push_back( &HERE::depthStream );

		/* カラーカメラの初期化 */
		HERE::colorStream.create( device, openni::SensorType::SENSOR_COLOR );
        HERE::colorStream.start();
		HERE::v_pColorStream.push_back( &HERE::colorStream );
	}
    catch ( std::exception& ) {
        //std::cout << openni::OpenNI::getExtendedError() << std::endl;
		cout << "Kinectが見つかりませんでした。Kinect無しモードで起動します。" << endl;
		HERE::kinectUseFlag = false;
	}
}

// 実空間に関する各種パラメータの初期化
void HERE::initParams()
{
	HERE::lockMutex();

	HERE::deskDepth    = (int)IO::getConst("DESK_DEPTH");
	HERE::blockSide    = (int)IO::getConst("BLOCK_SIDE");
	HERE::errLen       = (int)IO::getConst("ERR_LEN");
	HERE::blockNumX     = (int)IO::getConst("BLOCK_NUM_X");
	HERE::blockNumY     = (int)IO::getConst("BLOCK_NUM_Y");
	HERE::blockNumZ     = (int)IO::getConst("BLOCK_NUM_Z");

	for(int i=0;i<3;i++){
		VZERO(HERE::v_blockDan[i],HERE::blockNumX,0); // 要素確保
	}
	/*
	長さW(mm)の物体が、Kinectの面からL(mm)の距離にあるとき、画面上での幅Wm(px)は、
	Wm = W * 526.1146341 / (L+30)
	となる。（この式は13/05/10の実験に基づく）
	*/
	int W = HERE::blockSide;
	int L = 0;
	for( int i = 0; i <= HERE::blockNumY; i++ ){
		L = HERE::deskDepth - i * HERE::blockSide;
		HERE::v_blockSideScr.push_back( (int)( W * 526.1146341 / (L+30) ) );
	}

	HERE::unlockMutex();
}

// 深さ画像を更新してフィールド情報を更新
int HERE::updateDepthImageAndFieldInfo( int _line )
{
    int changedIndex;
    openni::OpenNI::waitForAnyStream( &HERE::v_pDepthStream[0], HERE::v_pDepthStream.size(), &changedIndex );
    if ( changedIndex == 0 ) {
        openni::VideoFrameRef depthFrame;
        depthStream.readFrame( &depthFrame );
        if ( depthFrame.isValid() ) {
            HERE::depthImage = cv::Mat( depthStream.getVideoMode().getResolutionY(),
                                    depthStream.getVideoMode().getResolutionX(),
                                    CV_16U, (char*)depthFrame.getData() );
			// 段数を表示
			for( int i = 0; i < 3; i++ ){
				HERE::updateFieldInfo( i );
			}

            // 0-10000mmまでのデータを0-255にする
			HERE::depthImage.convertTo( depthImage, CV_8U, 255.0 / 10000 );

			// ブロック情報を読み取る場所に線を描く
			for( int i = 0; i < 3; i++ ){
				for( int x = 1; x <= HERE::blockNumX; x++ ){
					cv::Point p1( IO::configData.startPoint.x , IO::configData.startPoint.y + i*IO::configData.zPixel );
					cv::Point p2( IO::configData.startPoint.x + x * HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + i*IO::configData.zPixel );
					cv::rectangle( HERE::depthImage, p1, p2, cv::Scalar(255), 1 );
				}
			}

			// 段数の数字を描く
			for( int i = 0; i < 3; i++ ){
				HERE::drawDanToDepthImage(i);
			}

            cv::imshow( "Depth Camera", depthImage );
			// キー操作
			int key = cv::waitKey( 1 );

        }
    }
	return 0;
}

// 色画像を更新する
void HERE::updateColorImage()
{
    int changedIndex;
    openni::OpenNI::waitForAnyStream( &HERE::v_pColorStream[0], HERE::v_pColorStream.size(), &changedIndex );
    if ( changedIndex == 0 ) {
        openni::VideoFrameRef colorFrame;
        HERE::colorStream.readFrame( &colorFrame );
        if ( colorFrame.isValid() ) {
            colorImage = cv::Mat( colorStream.getVideoMode().getResolutionY(),
                                    colorStream.getVideoMode().getResolutionX(),
                                    CV_8UC3, (char*)colorFrame.getData() );
        }

        cv::cvtColor( colorImage, colorImage, CV_BGR2RGB );
       

		cv::imshow( "Color Camera", HERE::colorImage );
		int key = cv::waitKey( 10 );
    }
}

// 深さ画像を解析してゲームフィールドの情報を更新する
void HERE::updateFieldInfo( int _line )
{
	HERE::lockMutex();
	static double DETECT_TH = IO::getConst("DETECT_TH");

	VZERO(HERE::v_blockDan[_line],HERE::blockNumX,0);

	int* a_dan = new int[HERE::blockNumY+1];

	// ブロック情報を読み取る場所の矩形
	for( int X = 0; X < HERE::blockNumX; X++ ){
		cv::Point p1( IO::configData.startPoint.x + X*HERE::v_blockSideScr.at(0), IO::configData.startPoint.y + _line*IO::configData.zPixel );
		cv::Point p2( IO::configData.startPoint.x + (X+1)*HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + _line*IO::configData.zPixel );
		NZERO(a_dan,HERE::blockNumY+1);
		// 矩形内の各ピクセル
		for( int x = p1.x; x < p2.x; x++ ){
			int depthSum = 0;
			int activeYcount = 0;
			for( int y = p1.y; y < p2.y; y++ ){
				// 0mm-10000mmまでの深さ情報を取得
				int dep = (int)HERE::depthImage.at<ushort>(y, x);
				
				/* FieldState で使う　離散的なブロック位置　に関するパラメータ */

				// 距離0（黒詰め）だったら
				if( dep == 0 ){
					// 無視
					continue;
				}
				// 机より遠かったら
				else if( dep > HERE::deskDepth + HERE::errLen ){
					activeYcount++;
					depthSum += dep;
					// 無視
					continue;
				}
				// 机より手前にあったら
				else {
					activeYcount++;
					depthSum += dep;
					double hei = (HERE::deskDepth-dep+HERE::blockSide/2);
					// 段数
					int dan = (int)(hei/HERE::blockSide);
					// 認識できる高さだったら
					if( dan <= HERE::blockNumY ){
						a_dan[dan]++;
					}
				}
			}
			HERE::heightWave.m_r[x] = HERE::deskDepth - (depthSum / ((activeYcount==0)?1:activeYcount));
		}
		// 段数配列の中で、１つの値が6割以上を占めていたら、ブロックの段数をその値とする。
		for( int i = 0; i <= HERE::blockNumY; i++ ){
			if( a_dan[i] > powf( (double)HERE::v_blockSideScr.at(0), 2 ) * DETECT_TH / 100 ){
				HERE::v_blockDan[_line][X] = i;
				break;
			}
		}
	}
	delete [] a_dan;

	// 波形を平滑化
	HERE::smoothHeightWave();

	HERE::unlockMutex();
}

// キー入力を元にfieldArrを更新する
void HERE::updateFieldArrWithoutKinect()
{
	HERE::lockMutex();

	for(int i=0;i<3;i++){
		VZERO(HERE::v_blockDan[i],HERE::blockNumX,0);
		for( int X = 0; X < HERE::blockNumX; X++ ){
			HERE::v_blockDan[i][X] = 2;
		}
	}
	
	HERE::unlockMutex();
}

// 段数をdepthImageに表示
void RealField::drawDanToDepthImage( int _line )
{
	HERE::lockMutex();

	int maxHeight = 0; // 一番高いところの高さ
	HERE::m_highest.clear();

	// すべての認識マスについて
	for( int X = 0; X < HERE::blockNumX; X++ ){
		cv::Point p1( IO::configData.startPoint.x + X*HERE::v_blockSideScr.at(0), IO::configData.startPoint.y + _line*IO::configData.zPixel );
		cv::Point p2( IO::configData.startPoint.x + (X+1)*HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + _line*IO::configData.zPixel );
		// 段数を表示する
		std::stringstream ss;
		ss << HERE::v_blockDan[_line][X];
		cv::putText( depthImage, ss.str(), cv::Point( (p1.x+p2.x)/2-10, p1.y ),
			cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar( 255, 0, 0 ), 1 );
		// 最高点より高い
		if( HERE::v_blockDan[_line].at(X) > maxHeight ){
			HERE::m_highest.clear();
			MINSERT(HERE::m_highest,X,HERE::v_blockDan[_line].at(X));
		}
		// 最高点と同じ
		ef( HERE::v_blockDan[_line].at(X) == maxHeight ){
			MINSERT(HERE::m_highest,X,HERE::v_blockDan[_line].at(X));
		}
	}
	HERE::unlockMutex();
}

// 高さ波形を平滑化
void RealField::smoothHeightWave(  )
{
	//FT::FFT( HERE::heightWave, HERE::smoothedWave, powf(2,utils::upperPow( HERE::heightWave.size(),2 )) );

	double before=0,after=0;
	foreach(it,HERE::heightWave.m_r){
		before = it->second;
		after = it->second;
		if( (++it)!=HERE::heightWave.m_r.end() ){
			after = it->second;
		}
		--it;
		if( it!=HERE::heightWave.m_r.begin() ){
			before = (--it)->second;
		}
		++it;

		double averaged = (before+it->second+after)/3;
		HERE::smoothedWave.m_r[it->first] = averaged;
	}

	//FT::IFFT( HERE::smoothedWave, HERE::smoothedWave, powf(2,utils::upperPow( HERE::smoothedWave.size(),2 )) );
}

int RealField::getDan( int _line, int _x )
{
	int ret = 0;
	
	HERE::lockMutex();
	// フィールドの範囲内
	if( 0 <= _x && _x < HERE::blockNumX ){
		ret = HERE::v_blockDan[_line].at(_x);
	}
	else{
		ret = 0;
	}
	HERE::unlockMutex();
	
	return ret;
}

void HERE::lockMutex()
{
	if( WaitForSingleObject( HERE::hMutex, INFINITE ) == WAIT_FAILED ){ // ミューテックスのロック
		cout << "Mutexのロック失敗" << endl;
		exit(1);
	}
	HERE::isMutexLocked = true;
}

void HERE::unlockMutex()
{
	if( ReleaseMutex( HERE::hMutex ) == 0 ){ // ミューテックスを解放
		cout << "Mutexアンロック失敗" << endl;
		exit(1);
	}
}