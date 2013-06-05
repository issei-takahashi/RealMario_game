#include "GameLib.hpp"

#include <DxLib.h> // �����ł̂݃C���N���[�h����

#define HERE GameLib

// ���C�u����������
int HERE::initLib()
{
	//�E�B���h�E���[�h�ɂ���
	DxLib::ChangeWindowMode(TRUE);
	DxLib::SetMainWindowText( "�c���������W���u����ԃQ�[���v" ) ;
	DxLib::SetGraphMode( SCR_X , SCR_Y , 16 ) ;
	//DxLib::SetBackgroundColor(16,16,16);

	int ret = DxLib::DxLib_Init();

	//�����`��ɐݒ�
	DxLib::SetDrawScreen(DX_SCREEN_BACK);

	// ���̃T�C�Y���ς�
	//DxLib::SetWindowSizeChangeEnableFlag( true ) ;

	//�}�E�X���\����Ԃɂ���
	//DxLib::SetMouseDispFlag(false);

	// �f�o�b�O�p�R���\�[�����Ăяo��
	AllocConsole();
	freopen("CONOUT$", "w", stdout); 
	freopen("CONIN$", "r", stdin);

	// �E�B���h�E����A�N�e�B�u�ł�����
	DxLib::SetAlwaysRunFlag( true ) ;

	// �t�H���g������
	DxLib::ChangeFontType( DX_FONTTYPE_EDGE ) ;

	return ret;
}

// ���C�u�����I��
int HERE::endLib()
{
	return DxLib::DxLib_End();
}


/* �`�� */

int HERE::draw::DrawCircle( int _x, int _y, int _r, int _col, int _fillFlag )
{
	return DxLib::DrawCircle(_x,_y,_r,_col,_fillFlag);
}

int HERE::draw::ScreenFlip()
{
	return DxLib::ScreenFlip();
}