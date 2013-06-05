#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include "DrawBuffer.hpp"
#include "luas.hpp"
#include "Rect.hpp"
#include "Observer.hpp"

#define VEL_MAX_Y 15 
#define GRAV 1

/* �ړ����̑S�� */

class Mover : public DrawBuffer::DrawInterface //, public Observer
{
	friend class FieldState;
interface__:
	Mover( int _index, Point<int> _pos );
	~Mover();
	void mainFlow();
	class Block* getStandingBlockAddr() const;
	Rect<int> getBodyRectBeforeVelAdded() const;  // ���x���Z�O�̃{�f�B���擾
	Rect<int> getBodyRectAfterVelAdded() const;  // ���x���Z��̃{�f�B���擾
open__:
	Point<int> pos;   // ���W
	Point<int> vel;   // ���x
inner__:
	int const bodyX; // �����蔻�蕝
	int const bodyY; // �����蔻�荂��
	int        index; // ���ʔԍ�
	map<string,sp<class Image> > m_pImg; // �摜
	map<string,sp<class Image> >::iterator it_m_pImg; // �摜�C�e���[�^

inner__:
	// @ovr
	void libDraw();
	// �ړ�
	virtual void move() = 0;
	// �`�悷��摜�X�V
	void updateImage();
	// �摜���[�h
	void loadImage();
	// �n�`�Ƃ̓����蔻��
	bool hitGroundEvent();
};

/* �L�����N�^�[ */

class Chara : public Mover
{
interface__:
	Chara( int _index, Point<int> _pos );
	
inner__:
	// @ovr
	void move();
	// �I�u�W�F�N�g�Ƃ̓����蔻��
	bool hitObjectEvent();
};

/* �I�u�W�F�N�g */

class Object : public Mover
{
interface__:
	Object( int _index, Point<int> _pos );
	void hitAction( class Chara * _pChara );
inner__:
	// @ovr
	void move();
};