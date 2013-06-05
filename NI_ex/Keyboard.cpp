#include "Keyboard.hpp"
#include "Timer.hpp"
#include "macro.hpp"
#include <DxLib.h>

//static�ϐ��̏�����
int Keyboard::keyInput[KEY_NUM] = {0};
int Keyboard::keyInputN[KEY_NUM] = {0};

// �L�[�̓��͏�Ԃ��X�V����
void Keyboard::updateKey()
{
	char tmpKey[KEY_NUM];                    // ���݂̃L�[�̓��͏�Ԃ��i�[����
	DxLib::GetHitKeyStateAll( tmpKey );             // �S�ẴL�[�̓��͏�Ԃ𓾂�
	for( int i=0; i < KEY_NUM; i++ ){ 
		if( tmpKey[i] != 0 ){            // i�Ԃ̃L�[�R�[�h�ɑΉ�����L�[��������Ă�����
			keyInputN[i] = 0;
			keyInput[i]++;     // 1�t���[�������Z
			keyInput[i] = min( keyInput[i], macro::KEY_INPUT_FRAME_MAX ); // �傫���Ȃ�߂��Ȃ��悤��
		}
		else{                         // ������Ă��Ȃ����
			keyInput[i] = 0;   // 0�ɂ���
			keyInputN[i]++;
			keyInputN[i] = min( keyInputN[i], macro::KEY_INPUT_FRAME_MAX );
		}
	}
}

//keyInput��get
int Keyboard::getKeyInputFrame( int keynum )
{
	assert( keynum < KEY_NUM );
	return keyInput[keynum];
}

//keyInput��get
int Keyboard::getKeyInputFrameN( int keynum )
{
	assert( keynum < KEY_NUM );
	return keyInputN[keynum];
}

// isKeyInputStart��get
bool Keyboard::getIsKeyInputStart( int keynum )
{
	assert( keynum < KEY_NUM );
	return ( keyInput[keynum] == 1 ) ? 1 : 0;
}

// isKeyInputEnd��get
bool Keyboard::getIsKeyInputEnd( int keynum )
{
	assert( keynum < KEY_NUM );
	return ( keyInputN[keynum] == 1 ) ? 1 : 0;
}