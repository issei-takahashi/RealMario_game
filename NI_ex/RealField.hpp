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
	// �J�����̏�����
	static void initCamera();
	// ����ԂɊւ���e��p�����[�^�̏�����
	static void initParams();

	// get
	// ������x�Ԗ�(X==0,1,2..)�̒i�����擾
	static int getDan( int _line, int _x );

	// get_
	static int get_blockNumX() { return blockNumX; } 
	static vector<int> get_v_blockDan( int _line ) { return v_blockDan[_line]; }
	static map<int,int> get_m_highest() { return m_highest; }
	static int getBlockSideScr0() { return v_blockSideScr.at(0); }

inner__:
	// �[���摜���X�V���ăt�B�[���h�����X�V
	static int updateDepthImageAndFieldInfo( int _line );
	// �F�摜���X�V����
	static void updateColorImage();
	// �[���摜����͂���fieldArr���X�V����
	static void updateFieldInfo( int _line );
	// �L�[���͂�����fieldArr���X�V����
	static void updateFieldArrWithoutKinect();
	// �i����depthImage�ɕ\��
	static void drawDanToDepthImage( int _line );
	// �����g�`�𕽊���
	static void smoothHeightWave(  );

	// Mutex�����b�N
	static void lockMutex();
	// Mutex���A�����b�N
	static void unlockMutex();

capcelled__:
	static vector<int> v_blockDan[3];           // ���ς܂�Ă�u���b�N�̒i��

open__:
	static Wave heightWave;   // �����g�`
	static Wave smoothedWave; // �����g�`�𕽊�����������

inner__:
	/* Kinect����Ƃ������� */
	static cv::Mat depthImage;        // �[���摜
	static cv::Mat colorImage;        // �J���[�摜
	/* �J�����֘A */
	static openni::VideoStream            depthStream;
	static vector<openni::VideoStream*>   v_pDepthStream;
	static openni::VideoStream            colorStream;
	static vector<openni::VideoStream*>   v_pColorStream;
	static openni::Device                 device;
	/* ����Ԃ̃p�����[�^ */
	static int deskDepth; // ���܂ł̐[��(mm)
	static int blockSide; // �u���b�N�̈��(mm)
	static int errLen;    // �F���̋��e�덷(mm)
	static int blockNumX;          // x�����̃u���b�N�̍ő吔
	static int blockNumY;          // y�����̃u���b�N�̍ő吔
	static int blockNumZ;          // z�����̃u���b�N�̍ő吔
	static vector<int> v_blockSideScr;      // ���e�ϊ���̉�ʏ�ł̃u���b�N��1��(key:�����琔�����u���b�N�̐�)
	static map<int,int> m_highest;        // �ł������ꏊ�ꗗ
	/* �X���b�h�֘A */
	static HANDLE hMutex; // �~���[�e�b�N�X�n���h��
	static bool isMutexLocked; // �~���[�e�b�N�X�����b�N����Ă��邩�ǂ���
	/* ���̑� */
	static bool kinectUseFlag; // Kinect���g�����ǂ����Bfalse�̂Ƃ��̓Q�[���݂̂œ��삷��B
};
