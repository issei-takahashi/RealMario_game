#pragma once

#include "Point.hpp"

struct macro{

	//�Q�[����ʃT�C�Y
	static int X_SCREEN_SIZE_DEFAULT;
	static int Y_SCREEN_SIZE_DEFAULT;
	//�Q�[����ʒ���
	static Point<int> SCREEN_CENTER;
	//�~����
	static double PI;
	//�덷
	static double EPS;
	// �f�o�b�O�r���h���ǂ���
	static bool DBG_FLAG;
	// �L�[���͂̍ő�t���[����
	static int KEY_INPUT_FRAME_MAX;
};


#define foreach(it,container) for(auto (it)=(container).begin();(it)!=(container).end();(it)++)
