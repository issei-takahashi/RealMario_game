#pragma once
#include "includes.hpp"
#include "Point.hpp"

class DrawBuffer
{
open__:
	class DrawInterface
	{
    interface__:
		virtual void libDraw()  = 0;
	};

interface__:
	// �o�b�t�@����Ɂi�S���߂��N���A�j
	static void clearBuffer();
	// �`��J�n����
	static void drawStartCall( int _layer, DrawInterface* _inter );
	// �`���~����
	static  void drawStopCall( int _layer, DrawInterface* _inter );
	// �o�b�t�@�ɂ��܂��Ă�����̂�S���`��
	static void drawAll();
inner__:
	static map<int,DrawInterface*> m_inter;
};

class MovieImage : public DrawBuffer::DrawInterface
{
interface__:
	MovieImage( string const & _dir, string const & _fileName, int _intervalFrame, double _xEx=1.0, double _yEx=1.0 );

	void libDraw();

open__:
	Point<int> position;    // �Q�[�������W�̒��S
	double xExRate;
	double yExRate;
inner__:
	string const fileDir;     // ����t�@�C���̂���f�B���N�g��
	string const fileName;    // ����t�@�C����
	int const handle;         // ���[�r�[�n���h��
	int const intervalFrame;  // �`��Ԋu�t���[��
	int xSize;
	int ySize;
	int frameCount;           // 1�t���[���`�悵�Ă���o�߂�������
};