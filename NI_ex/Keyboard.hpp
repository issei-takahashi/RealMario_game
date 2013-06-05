#pragma once

#include "includes.hpp"

#define KEY_NUM 256

/*------------------- Class Keyboard ---------------------------

�ړI�F�L�[�����t���[��������Ă��邩��ۑ�����N���X
�����F���m�X�e�[�g
�g�p���@�FgetKeyCount( keyCode )�ŁAkeyCode�ɑΉ�����L�[����ms������Ă��邩�擾����B

--------------------------------------------------------*/

class Keyboard
{
interface__:
	// �L�[�̓��͏�Ԃ��X�V����
	static void updateKey();
	// ����̃L�[�̓��̓t���[�����擾
	static int getKeyInputFrame( int keynum );
	// ����̃L�[�̔���̓t���[�����擾
	static int getKeyInputFrameN( int keynum );
	// ����̃L�[�����͊J�n�������ǂ������擾
	static bool getIsKeyInputStart( int keynum );
	// ����̃L�[�����͏I���������ǂ������擾
	static bool getIsKeyInputEnd( int keynum );

outofuse__:
	//�B���R���X�g���N�^
	Keyboard(){}

inner__:
	// �L�[��������Ă���t���[�������i�[����z��
	static int keyInput[KEY_NUM]; 
	// �L�[��������Ă��Ȃ��t���[�������i�[����z��
	static int keyInputN[KEY_NUM];
};