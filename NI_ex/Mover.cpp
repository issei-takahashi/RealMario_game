#include "Mover.hpp"
#include "Image.hpp"
#include "Block.h"
#include "GameMain.hpp"
#include "SoundManager.hpp"
#include <DxLib.h>

#define HERE Mover
#define IS_IN_AIR (this->aStandBlock==NULL)
#define CHANGE_IMAGE(STR) auto it=this->m_pImg.find(STR);assert(it!=this->m_pImg.end());if(it!=this->it_m_pImg){this->it_m_pImg=it;this->it_m_pImg->second->initIterator();}

Mover::Mover( int _index, Point<int> _pos )
	:index( _index ), pos( _pos ), vel(), bodyX(40), bodyY(100)
{
	switch( _index ){
	//case 1:  // �N�[�j��
	//	{
	//		this->m_pImg["stand.png"] = (sp<Image>)(new Image("image/cunya/no/stand.png",1,1,1,128,128,4,0));
	//		this->m_pImg["run.png"]   = (sp<Image>)(new Image("image/cunya/no/run.png"  ,8,8,1,128,128,4,0));
	//		this->m_pImg["jump.png"]  = (sp<Image>)(new Image("image/cunya/no/jump.png" ,4,4,1,128,128,4,1));
	//		this->m_pImg["fall.png"]  = (sp<Image>)(new Image("image/cunya/no/fall.png" ,3,3,1,128,128,4,1));
	//		break;
	//	}
	//case 2:  // �i�c��
	//	{
	//		this->m_pImg["stand.png"] = (sp<Image>)(new Image("image/natsume/stand.png",1,1,1,128,128,4,0));
	//		this->m_pImg["run.png"]   = (sp<Image>)(new Image("image/natsume/run.png"  ,8,8,1,128,128,4,0));
	//		this->m_pImg["jump.png"]  = (sp<Image>)(new Image("image/natsume/jump.png" ,5,5,1,128,128,4,1));
	//		this->m_pImg["fall.png"]  = (sp<Image>)(new Image("image/natsume/fall.png" ,5,5,1,128,128,4,2));
	//		break;
	//	}
	//case 3:  // �g�g
	//	{
	//		this->m_pImg["stand.png"] = (sp<Image>)(new Image("image/toto/stand.png",1,1,1,128,128,4,0));
	//		this->m_pImg["run.png"]   = (sp<Image>)(new Image("image/toto/run.png"  ,8,8,1,128,128,4,0));
	//		this->m_pImg["jump.png"]  = (sp<Image>)(new Image("image/toto/jump.png" ,6,6,1,128,128,4,1));
	//		this->m_pImg["fall.png"]  = (sp<Image>)(new Image("image/toto/fall.png" ,3,3,1,128,128,4,1));
	//		break;
	//	}
	//case 4:  // �R�C��
	//	{
	//		this->m_pImg["stand.png"] = (sp<Image>)(new Image("image/coin/stand.png",1,1,1,80,100,1,0));
	//		this->m_pImg["run.png"]   = (sp<Image>)(new Image("image/coin/run.png"  ,1,1,1,80,100,1,0));
	//		this->m_pImg["jump.png"]  = (sp<Image>)(new Image("image/coin/jump.png" ,1,1,1,80,100,1,0));
	//		this->m_pImg["fall.png"]  = (sp<Image>)(new Image("image/coin/fall.png" ,1,1,1,80,100,1,0));
	//		break;
	//	}
	case 1: case 2: case 3: case 4: case 5:  // �Ђ悱
		{
			this->m_pImg["stand.png"] = (sp<Image>)(new Image("image/hiyoko/stand.png",1,1,1,100,100,5,0));
			this->m_pImg["run.png"]   = (sp<Image>)(new Image("image/hiyoko/run.png"  ,5,5,1,100,100,5,0));
			this->m_pImg["jump.png"]  = (sp<Image>)(new Image("image/hiyoko/jump.png" ,4,4,1,100,100,5,0));
			this->m_pImg["fall.png"]  = (sp<Image>)(new Image("image/hiyoko/fall.png" ,8,8,1,100,100,5,0));
			break;
		}
	}
	this->it_m_pImg = this->m_pImg.find("stand.png");
	this->loadImage();
}

Mover::~Mover()
{

}


void HERE::mainFlow()
{
	this->move();
	// �n�`�Ƃ̓����蔻��
	this->hitGroundEvent();
	this->updateImage();
	this->pos += this->vel;
	this->libDraw();
}


// ���x���Z�O�̃{�f�B���擾
Rect<int> HERE::getBodyRectBeforeVelAdded() const
{
	Rect<int> myRect1;
	myRect1.p1.x = this->pos.x-this->bodyX/2;
	myRect1.p1.y = this->pos.y-this->bodyY/2;
	myRect1.p2.x = this->pos.x+this->bodyX/2;
	myRect1.p2.y = this->pos.y+this->bodyY/2;
	return myRect1;
}

// ���x���Z��̃{�f�B���擾
Rect<int> HERE::getBodyRectAfterVelAdded() const
{
	Rect<int> myRect2;
	myRect2.p1.x = this->pos.x-this->bodyX/2+this->vel.x;
	myRect2.p1.y = this->pos.y-this->bodyY/2+this->vel.y;
	myRect2.p2.x = this->pos.x+this->bodyX/2+this->vel.x;
	myRect2.p2.y = this->pos.y+this->bodyY/2+this->vel.y;
	return myRect2;
}


// @ovr
void HERE::libDraw()
{
	this->it_m_pImg->second->setCenter(this->pos);
	this->it_m_pImg->second->libDraw();
}


void HERE::loadImage()
{
	foreach(it,this->m_pImg){
		it->second->loadImage();
	}
}

// �`�悷��摜�X�V
void HERE::updateImage()
{
	string newName = "stand.png";
	// �n��
	if( this->vel.y==0 ){
		// �Î~
		if(this->vel.x==0){
			newName = "stand.png";
		}
		// ����
		else{
			newName = "run.png";
		}
	}
	// ��
	else{
		// �㏸
		if(this->vel.y<0){
			newName = "jump.png";
		}
		// ����
		else{
			newName = "fall.png";
		}
	}
	CHANGE_IMAGE(newName);
}

// �n�`�Ƃ̓����蔻��
bool HERE::hitGroundEvent()
{
	bool ret = false;

	ret = FieldState::get()->hitBlockAndChangeVelEvent(this);
	
	return ret;
}


Chara::Chara( int _index, Point<int> _pos )
	:Mover(_index,_pos)
{

}

// �I�u�W�F�N�g�Ƃ̓����蔻��
bool Chara::hitObjectEvent()
{
	bool ret = false;

	ret = FieldState::get()->hitObjectEvent(this);
	
	return ret;
}


// �ړ�
void Chara::move()
{
	this->vel.x=3;
	this->vel.y+=3;
}

Object::Object( int _index, Point<int> _pos )
	:Mover( _index, _pos )
{

}

void Object::hitAction( class Chara * _pChara )
{
	SoundManager::play("sound/coin.wav");
}

void Object::move()
{
	this->vel.y = 4;
}