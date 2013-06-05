#include "RealField.hpp"
#include "IO.hpp"
#include "FT.hpp"
#include "utils.hpp"

#define HERE RealField

/* Kinect����Ƃ������� */
cv::Mat                       HERE::depthImage;
cv::Mat                       HERE::colorImage;
/* �J�����֘A */
openni::VideoStream           HERE::depthStream;
vector<openni::VideoStream*>  HERE::v_pDepthStream;
openni::VideoStream           HERE::colorStream;
vector<openni::VideoStream*>  HERE::v_pColorStream;
openni::Device                HERE::device;
/* ����Ԃ̃p�����[�^ */
int                           HERE::deskDepth; // ���܂ł̐[��(mm)
int                           HERE::blockSide; // �u���b�N�̈��(mm)
int                           HERE::errLen;    // �F���̋��e�덷(mm)
int                           HERE::blockNumX;          // x�����̃u���b�N�̍ő吔
int                           HERE::blockNumY;          // y�����̃u���b�N�̍ő吔
int                           HERE::blockNumZ;          // z�����̃u���b�N�̍ő吔
vector<int>                   HERE::v_blockSideScr;      // ���e�ϊ���̉�ʏ�ł̃u���b�N��1��(key:�����琔�����u���b�N�̐�)
vector<int>                   HERE::v_blockDan[3];           // ���ς܂�Ă�u���b�N�̒i��
map<int,int>                  HERE::m_highest;        // �ł������ꏊ�ꗗ
Wave                          HERE::heightWave;     // �A���I�ȍ������z
Wave                          HERE::smoothedWave;     // �A���I�ȍ������z�𕽊�����������
/* �X���b�h�֘A */
HANDLE                        HERE::hMutex = CreateMutex( NULL, FALSE, NULL);
bool                          HERE::isMutexLocked = false;
/* ���̑� */
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

// �J�����̏�����
void HERE::initCamera()
{
    try {
		/* OpenNI�̏����� */
        openni::OpenNI::initialize();
        auto ret = HERE::device.open( openni::ANY_DEVICE );
        if ( ret != openni::STATUS_OK ) {
            throw std::runtime_error( "" );
        }

		/* �[���J�����̏����� */
        HERE::depthStream.create( device, openni::SensorType::SENSOR_DEPTH );
        HERE::depthStream.start();
		HERE::v_pDepthStream.push_back( &HERE::depthStream );

		/* �J���[�J�����̏����� */
		HERE::colorStream.create( device, openni::SensorType::SENSOR_COLOR );
        HERE::colorStream.start();
		HERE::v_pColorStream.push_back( &HERE::colorStream );
	}
    catch ( std::exception& ) {
        //std::cout << openni::OpenNI::getExtendedError() << std::endl;
		cout << "Kinect��������܂���ł����BKinect�������[�h�ŋN�����܂��B" << endl;
		HERE::kinectUseFlag = false;
	}
}

// ����ԂɊւ���e��p�����[�^�̏�����
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
		VZERO(HERE::v_blockDan[i],HERE::blockNumX,0); // �v�f�m��
	}
	/*
	����W(mm)�̕��̂��AKinect�̖ʂ���L(mm)�̋����ɂ���Ƃ��A��ʏ�ł̕�Wm(px)�́A
	Wm = W * 526.1146341 / (L+30)
	�ƂȂ�B�i���̎���13/05/10�̎����Ɋ�Â��j
	*/
	int W = HERE::blockSide;
	int L = 0;
	for( int i = 0; i <= HERE::blockNumY; i++ ){
		L = HERE::deskDepth - i * HERE::blockSide;
		HERE::v_blockSideScr.push_back( (int)( W * 526.1146341 / (L+30) ) );
	}

	HERE::unlockMutex();
}

// �[���摜���X�V���ăt�B�[���h�����X�V
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
			// �i����\��
			for( int i = 0; i < 3; i++ ){
				HERE::updateFieldInfo( i );
			}

            // 0-10000mm�܂ł̃f�[�^��0-255�ɂ���
			HERE::depthImage.convertTo( depthImage, CV_8U, 255.0 / 10000 );

			// �u���b�N����ǂݎ��ꏊ�ɐ���`��
			for( int i = 0; i < 3; i++ ){
				for( int x = 1; x <= HERE::blockNumX; x++ ){
					cv::Point p1( IO::configData.startPoint.x , IO::configData.startPoint.y + i*IO::configData.zPixel );
					cv::Point p2( IO::configData.startPoint.x + x * HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + i*IO::configData.zPixel );
					cv::rectangle( HERE::depthImage, p1, p2, cv::Scalar(255), 1 );
				}
			}

			// �i���̐�����`��
			for( int i = 0; i < 3; i++ ){
				HERE::drawDanToDepthImage(i);
			}

            cv::imshow( "Depth Camera", depthImage );
			// �L�[����
			int key = cv::waitKey( 1 );

        }
    }
	return 0;
}

// �F�摜���X�V����
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

// �[���摜����͂��ăQ�[���t�B�[���h�̏����X�V����
void HERE::updateFieldInfo( int _line )
{
	HERE::lockMutex();
	static double DETECT_TH = IO::getConst("DETECT_TH");

	VZERO(HERE::v_blockDan[_line],HERE::blockNumX,0);

	int* a_dan = new int[HERE::blockNumY+1];

	// �u���b�N����ǂݎ��ꏊ�̋�`
	for( int X = 0; X < HERE::blockNumX; X++ ){
		cv::Point p1( IO::configData.startPoint.x + X*HERE::v_blockSideScr.at(0), IO::configData.startPoint.y + _line*IO::configData.zPixel );
		cv::Point p2( IO::configData.startPoint.x + (X+1)*HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + _line*IO::configData.zPixel );
		NZERO(a_dan,HERE::blockNumY+1);
		// ��`���̊e�s�N�Z��
		for( int x = p1.x; x < p2.x; x++ ){
			int depthSum = 0;
			int activeYcount = 0;
			for( int y = p1.y; y < p2.y; y++ ){
				// 0mm-10000mm�܂ł̐[�������擾
				int dep = (int)HERE::depthImage.at<ushort>(y, x);
				
				/* FieldState �Ŏg���@���U�I�ȃu���b�N�ʒu�@�Ɋւ���p�����[�^ */

				// ����0�i���l�߁j��������
				if( dep == 0 ){
					// ����
					continue;
				}
				// ����艓��������
				else if( dep > HERE::deskDepth + HERE::errLen ){
					activeYcount++;
					depthSum += dep;
					// ����
					continue;
				}
				// ������O�ɂ�������
				else {
					activeYcount++;
					depthSum += dep;
					double hei = (HERE::deskDepth-dep+HERE::blockSide/2);
					// �i��
					int dan = (int)(hei/HERE::blockSide);
					// �F���ł��鍂����������
					if( dan <= HERE::blockNumY ){
						a_dan[dan]++;
					}
				}
			}
			HERE::heightWave.m_r[x] = HERE::deskDepth - (depthSum / ((activeYcount==0)?1:activeYcount));
		}
		// �i���z��̒��ŁA�P�̒l��6���ȏ���߂Ă�����A�u���b�N�̒i�������̒l�Ƃ���B
		for( int i = 0; i <= HERE::blockNumY; i++ ){
			if( a_dan[i] > powf( (double)HERE::v_blockSideScr.at(0), 2 ) * DETECT_TH / 100 ){
				HERE::v_blockDan[_line][X] = i;
				break;
			}
		}
	}
	delete [] a_dan;

	// �g�`�𕽊���
	HERE::smoothHeightWave();

	HERE::unlockMutex();
}

// �L�[���͂�����fieldArr���X�V����
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

// �i����depthImage�ɕ\��
void RealField::drawDanToDepthImage( int _line )
{
	HERE::lockMutex();

	int maxHeight = 0; // ��ԍ����Ƃ���̍���
	HERE::m_highest.clear();

	// ���ׂĂ̔F���}�X�ɂ���
	for( int X = 0; X < HERE::blockNumX; X++ ){
		cv::Point p1( IO::configData.startPoint.x + X*HERE::v_blockSideScr.at(0), IO::configData.startPoint.y + _line*IO::configData.zPixel );
		cv::Point p2( IO::configData.startPoint.x + (X+1)*HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + _line*IO::configData.zPixel );
		// �i����\������
		std::stringstream ss;
		ss << HERE::v_blockDan[_line][X];
		cv::putText( depthImage, ss.str(), cv::Point( (p1.x+p2.x)/2-10, p1.y ),
			cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar( 255, 0, 0 ), 1 );
		// �ō��_��荂��
		if( HERE::v_blockDan[_line].at(X) > maxHeight ){
			HERE::m_highest.clear();
			MINSERT(HERE::m_highest,X,HERE::v_blockDan[_line].at(X));
		}
		// �ō��_�Ɠ���
		ef( HERE::v_blockDan[_line].at(X) == maxHeight ){
			MINSERT(HERE::m_highest,X,HERE::v_blockDan[_line].at(X));
		}
	}
	HERE::unlockMutex();
}

// �����g�`�𕽊���
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
	// �t�B�[���h�͈͓̔�
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
	if( WaitForSingleObject( HERE::hMutex, INFINITE ) == WAIT_FAILED ){ // �~���[�e�b�N�X�̃��b�N
		cout << "Mutex�̃��b�N���s" << endl;
		exit(1);
	}
	HERE::isMutexLocked = true;
}

void HERE::unlockMutex()
{
	if( ReleaseMutex( HERE::hMutex ) == 0 ){ // �~���[�e�b�N�X�����
		cout << "Mutex�A�����b�N���s" << endl;
		exit(1);
	}
}