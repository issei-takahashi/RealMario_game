#include "GameMain.hpp"
#include "RealField.hpp"
#include "GameLib.hpp"
#include "IO.hpp"
#include "Keyboard.hpp"
#include "Mover.hpp"
#include "Block.h"
#include "Camera.h"
#include "Image.hpp"
#include <DxLib.h>

#define BLOCK_PIX 100
#define BLOCK_CENTER(X,Y) Point<int>((int)(BLOCK_PIX*(X+0.5)),5*BLOCK_PIX+(int)(-BLOCK_PIX*(Y+0.5)))

GameState* GameMain::pState = FieldState::get();

void GameMain::mainLoop()
{
	while( ProcessMessage()==0 ){
		Keyboard::updateKey();
		GameMain::pState->mainFlow();
	}
}

void GameMain::changeState( class GameState * _pState )
{
	GameMain::pState = _pState;
}


void TitleState::mainFlow()
{

}

void FieldState::mainFlow()
{

	DxLib::ClearDrawScreen();
	DxLib::clsDx();

	this->generateObjectEvent();
	this->keyInputAction();
	this->executeAllMovers();
	this->updateBlock();

	if( this->blockDispFlag ){
		this->drawBlock();
	}

	DxLib::ScreenFlip();
}

// ����̏ꏊ�̒i�����擾
int FieldState::getDan( int _line, int _x ) const
{
	return RealField::getDan( _line, _x );
}

// RealField�̒i���ɂ��ƂÂ��ău���b�N�ꗗ���X�V
void FieldState::updateBlock()
{
	static int const blockNumX = (int)IO::getConst("BLOCK_NUM_X");
	static Block blockBuf;
	
	for(int i=0; i<3; i++){
		this->v_v_Block[i].clear();
		for( int x = 0; x < blockNumX; x++ ){
			vector<Block> tmpv;
			for( int y = -1; y < RealField::getDan(i,x); y++ ){
				blockBuf.p1 = Point<int>((int)(+BLOCK_PIX*x)    , 5*BLOCK_PIX+(int)(-BLOCK_PIX*(y+1)));
				blockBuf.p2 = Point<int>((int)(BLOCK_PIX*(x+1)), 5*BLOCK_PIX+(int)(-BLOCK_PIX*y));
				tmpv.push_back( blockBuf );
			}
			this->v_v_Block[i].push_back( tmpv );
		}
	}
}

// Mover�ƃu���b�N�Ƃ̓����蔻���S�T���ōs���đ��x��␳����
bool FieldState::hitBlockAndChangeVelEvent( Mover* _pMover )
{
	bool ret = false;

	/* ���ɂ��Ă̓����蔻�� */
	static Rect<int> myRect1,myRect2;
	myRect1 = _pMover->getBodyRectBeforeVelAdded();
	myRect2 = _pMover->getBodyRectAfterVelAdded();

	//cout << _pMover->pos.x << "," << _pMover->pos.y << endl;

	/// ���܂��郉�C���ɂ���
	/* �S�Ẵu���b�N�ɂ��Ă̓����蔻�� */
	foreach(it,this->v_v_Block[this->line]){
		foreach(it2,*it){
			int locTypeAfter = myRect2.getLocationType(*it2);
			// �����蔻��
			if((locTypeAfter&LOC_UDHIT)&&(locTypeAfter&LOC_LRHIT)){
				ret = true;
				int locTypeBefore = myRect1.getLocationType(*it2);
				/* ���n�̃P�[�X */
				if( locTypeBefore&LOC_UP ){
					//_pMover->setStandingBlockAddr(&*it2);
					_pMover->vel.y -= (myRect2.p2.y-it2->p1.y);
				}
				/* �����Ă���P�[�X */
				ef( locTypeBefore&LOC_UP_FIT ){
					_pMover->vel.y = 0;
				}
				/* ������q�b�g�̃P�[�X */
				ef( (locTypeBefore&LOC_DOWN) || (locTypeBefore&LOC_DOWN_FIT) ){
					_pMover->vel.y += (it2->p2.y-myRect2.p1.y);
				}
				/* ������q�b�g�̃P�[�X */
				ef( (locTypeBefore&LOC_LEFT) || (locTypeBefore&LOC_LEFT_FIT) ){
					_pMover->vel.x -= (myRect2.p2.x-it2->p1.x);
				}
				/* �E����q�b�g�̃P�[�X */
				ef( (locTypeBefore&LOC_RIGHT) || (locTypeBefore&LOC_RIGHT_FIT) ){
					_pMover->vel.x += (it2->p2.x-myRect2.p1.x);
				}
				el{
					//passert("��`����Ă��Ȃ������蔻��ł�",0);
				}
			}
		}
	}

	return ret;
}

// Chara��Object�Ƃ̓����蔻���S�T���ōs��
bool FieldState::hitObjectEvent( class Chara * _pChara )
{
	bool ret = false;

	static Rect<int> myRect2;
	myRect2 = _pChara->getBodyRectAfterVelAdded();

	/* �S�Ă�Object�ɂ��Ă̓����蔻�� */
	for(auto it=this->m_spObject.begin(); it!=this->m_spObject.end(); ){
		int locTypeAfter = myRect2.getLocationType(it->second->getBodyRectAfterVelAdded());
		// �����蔻��
		if((locTypeAfter&LOC_UDHIT)&&(locTypeAfter&LOC_LRHIT)){
			it->second->hitAction(_pChara);
			it = this->m_spObject.erase( it );
			ret = true;
		}
		el{
			it++;
		}
	}
	return ret;
}

// �u���b�N�`��
void FieldState::drawBlock()
{
	foreach(it,this->v_v_Block[this->line]){
		foreach(it2,*it){
			it2->libDraw();
			
		}
	}
}

// Mover���W�F�l���[�g
void FieldState::generateChara( int _index, Point<int> _pos )
{
	this->m_spChara[_index] = (sp<Chara>)(new Chara(_index,_pos));
}

// Object���W�F�l���[�g
void FieldState::generateObject( int _index, Point<int> _pos )
{
	sp<Object> p = (sp<Object>)(new Object(_index,_pos));
	this->m_spObject[ p.get() ] = p;
}

// �S�Ă�Mover�����s
void FieldState::executeAllMovers()
{
	static int count = 0;
	foreach(it,this->m_spChara){
		it->second->mainFlow();
		this->hitObjectEvent(it->second.get());
	}
	foreach(it,this->m_spObject){
		it->second->mainFlow();
	}
}

// ������p���ăI�u�W�F�N�g���W�F�l���[�g
void FieldState::generateObjectEvent()
{
	switch( rand() % 53 ){
	case 0: // �R�C��
		{
			this->generateObject(4,Point<int>(rand()%SCR_X,0));
			break;
		}
	}
}

void FieldState::keyInputAction()
{

	// W�L�[�F�E�B���h�E���[�h�؂�ւ�
	if( Keyboard::getIsKeyInputStart(KEY_INPUT_W) ){
		static bool windowFlag = TRUE;
		windowFlag = !windowFlag;
		DxLib::ChangeWindowMode(windowFlag);
	}
	// S�L�[�F�R���t�B�O�f�[�^�Z�[�u
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_S) ){
		IO::saveConfigData();
	}
	// C�L�[�F�L�����u���[�V�������[�h�ύX
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_C) ){
		this->calibFlag = !this->calibFlag;
	}
	// B�L�[�F�u���b�N�\���؂�ւ�
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_B) ){
		this->blockDispFlag = !this->blockDispFlag;
	}
	// 1�L�[�FMover 1���W�F�l���[�g
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_1) ){
		this->generateChara(1,Point<int>(0,0));
	}
	// 2�L�[�FMover 2���W�F�l���[�g
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_2) ){
		this->generateChara(2,Point<int>(50,0));
	}
	// 3�L�[�FMover 3���W�F�l���[�g
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_3) ){
		this->generateChara(3,Point<int>(150,0));
	}
	// A�L�[�FAnalogState�Ɉړ�
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_A) ){
		GameMain::changeState( AnalogState::get() );
	}
	// F�L�[�FFieldState�Ɉړ�
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_F) ){
		GameMain::changeState( FieldState::get() );
	}



	/* �L�����u���[�V�����̎��̂ݍs������ */
	if( this->calibFlag ){
		DxLib::printfDx("<< calibration mode >>\n�����L�[�Ńu���b�N�̈ʒu����\n[�L�[��]�L�[�ŕ\���{������\nShift�𓯎��ɉ����ƒ������x0.1�{\n");
		double shiftTime = (Keyboard::getKeyInputFrame(KEY_INPUT_LSHIFT)!=0) ? 0.1 : 1.0;
		// Ctrl�����J�����L�����u���[�V����
		if( Keyboard::getKeyInputFrame(KEY_INPUT_LCONTROL) ){
			if( Keyboard::getKeyInputFrame(KEY_INPUT_LBRACKET) ){
				IO::configData.zPixel += 1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_RBRACKET) ){
				IO::configData.zPixel -=1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_UP) ){
				IO::configData.startPoint.y -=1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_DOWN) ){
				IO::configData.startPoint.y +=1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_LEFT) ){
				IO::configData.startPoint.x -=1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_RIGHT) ){
				IO::configData.startPoint.x +=1;
			}
		}
		el{
			if( Keyboard::getKeyInputFrame(KEY_INPUT_LBRACKET) ){
				IO::configData.zoom += (0.1*shiftTime);
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_RBRACKET) ){
				IO::configData.zoom -= (0.1*shiftTime);
			}

			if( IO::configData.zoom < 0.1 ){
				IO::configData.zoom = 0.1;
			}

			if( Keyboard::getKeyInputFrame(KEY_INPUT_UP) ){
				IO::configData.cameraPos.y -= (int)(10*shiftTime);
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_DOWN) ){
				IO::configData.cameraPos.y += (int)(10*shiftTime);
			}
			if( Keyboard::getKeyInputFrame(KEY_INPUT_LEFT) ){
				IO::configData.cameraPos.x -= (int)(10*shiftTime);
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_RIGHT) ){
				IO::configData.cameraPos.x += (int)(10*shiftTime);
			}
		}
		DxLib::printfDx("\n(startX,startY,zoom)=%d,%d,%.3f\n",IO::configData.cameraPos.x,IO::configData.cameraPos.y,IO::configData.zoom);
	}
}


AnalogState::AnalogState()
{
	this->loadAndPrepareMovie();
}

void AnalogState::mainFlow()
{
	DxLib::ClearDrawScreen();
	DxLib::clsDx();

	//FieldState::get()->generateObjectEvent();
	FieldState::get()->keyInputAction();
	//FieldState::get()->executeAllMovers();
	FieldState::get()->updateBlock();
	FieldState::get()->drawBlock();

	this->updateHighestPoint();
	this->drawMovie();

	DxLib::ScreenFlip();
}

void AnalogState::loadAndPrepareMovie()
{
	this->m_spImage["fire_finger"] = (sp<Image>)(new Image("image/fire/fire_finger.png",27,9,3,125,180,4,2,5,1));
	this->m_spImage["fire_finger"]->loadImage();
}

void AnalogState::updateHighestPoint()
{
	int sideScr = RealField::getBlockSideScr0();
	int startX = IO::getConst("START_POINT_X");
	Point<int> p1,p2,dp1,dp2;
	p1.y = max(0.0, BLOCK_PIX*5-RealField::heightWave.m_r.begin()->second*((double)BLOCK_PIX/60));
	Point<int> hPoint(-1,99999);
	foreach(it,RealField::smoothedWave.m_r){
		int p2y = max(0.0, BLOCK_PIX*5-it->second*((double)BLOCK_PIX/60) );
		if( p2y != 0 ){
			p2.x = (it->first-startX) * ((double)BLOCK_PIX/sideScr);
			p2.y = p2y;
			dp1 = Camera::getDrawPos(p1);
			dp2 = Camera::getDrawPos(p2);
			if( FieldState::get()->blockDispFlag ){
				DxLib::DrawLine(dp1.x,dp1.y,dp2.x,dp2.y,GetColor(0,0,255),2);
			}
			p1 = p2;
			if( p1.y < hPoint.y ){
				hPoint.x = p1.x;
				hPoint.y = p1.y;
			}
		}
	}
	static int count = 0;
	if( (++count)%10 == 0 ){
		this->highestPoint = hPoint;
	}
}

void AnalogState::drawMovie()
{
	auto it = this->m_spImage.find("fire_finger");
	assert(it!=this->m_spImage.end());
	it->second->setCenter( this->highestPoint );
	it->second->libDraw();
}