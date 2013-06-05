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

// 特定の場所の段数を取得
int FieldState::getDan( int _line, int _x ) const
{
	return RealField::getDan( _line, _x );
}

// RealFieldの段数にもとづいてブロック一覧を更新
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

// Moverとブロックとの当たり判定を全探索で行って速度を補正する
bool FieldState::hitBlockAndChangeVelEvent( Mover* _pMover )
{
	bool ret = false;

	/* 床についての当たり判定 */
	static Rect<int> myRect1,myRect2;
	myRect1 = _pMover->getBodyRectBeforeVelAdded();
	myRect2 = _pMover->getBodyRectAfterVelAdded();

	//cout << _pMover->pos.x << "," << _pMover->pos.y << endl;

	/// いまいるラインについて
	/* 全てのブロックについての当たり判定 */
	foreach(it,this->v_v_Block[this->line]){
		foreach(it2,*it){
			int locTypeAfter = myRect2.getLocationType(*it2);
			// 当たり判定
			if((locTypeAfter&LOC_UDHIT)&&(locTypeAfter&LOC_LRHIT)){
				ret = true;
				int locTypeBefore = myRect1.getLocationType(*it2);
				/* 着地のケース */
				if( locTypeBefore&LOC_UP ){
					//_pMover->setStandingBlockAddr(&*it2);
					_pMover->vel.y -= (myRect2.p2.y-it2->p1.y);
				}
				/* 立っているケース */
				ef( locTypeBefore&LOC_UP_FIT ){
					_pMover->vel.y = 0;
				}
				/* 下からヒットのケース */
				ef( (locTypeBefore&LOC_DOWN) || (locTypeBefore&LOC_DOWN_FIT) ){
					_pMover->vel.y += (it2->p2.y-myRect2.p1.y);
				}
				/* 左からヒットのケース */
				ef( (locTypeBefore&LOC_LEFT) || (locTypeBefore&LOC_LEFT_FIT) ){
					_pMover->vel.x -= (myRect2.p2.x-it2->p1.x);
				}
				/* 右からヒットのケース */
				ef( (locTypeBefore&LOC_RIGHT) || (locTypeBefore&LOC_RIGHT_FIT) ){
					_pMover->vel.x += (it2->p2.x-myRect2.p1.x);
				}
				el{
					//passert("定義されていないあたり判定です",0);
				}
			}
		}
	}

	return ret;
}

// CharaとObjectとの当たり判定を全探索で行う
bool FieldState::hitObjectEvent( class Chara * _pChara )
{
	bool ret = false;

	static Rect<int> myRect2;
	myRect2 = _pChara->getBodyRectAfterVelAdded();

	/* 全てのObjectについての当たり判定 */
	for(auto it=this->m_spObject.begin(); it!=this->m_spObject.end(); ){
		int locTypeAfter = myRect2.getLocationType(it->second->getBodyRectAfterVelAdded());
		// 当たり判定
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

// ブロック描画
void FieldState::drawBlock()
{
	foreach(it,this->v_v_Block[this->line]){
		foreach(it2,*it){
			it2->libDraw();
			
		}
	}
}

// Moverをジェネレート
void FieldState::generateChara( int _index, Point<int> _pos )
{
	this->m_spChara[_index] = (sp<Chara>)(new Chara(_index,_pos));
}

// Objectをジェネレート
void FieldState::generateObject( int _index, Point<int> _pos )
{
	sp<Object> p = (sp<Object>)(new Object(_index,_pos));
	this->m_spObject[ p.get() ] = p;
}

// 全てのMoverを実行
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

// 乱数を用いてオブジェクトをジェネレート
void FieldState::generateObjectEvent()
{
	switch( rand() % 53 ){
	case 0: // コイン
		{
			this->generateObject(4,Point<int>(rand()%SCR_X,0));
			break;
		}
	}
}

void FieldState::keyInputAction()
{

	// Wキー：ウィンドウモード切り替え
	if( Keyboard::getIsKeyInputStart(KEY_INPUT_W) ){
		static bool windowFlag = TRUE;
		windowFlag = !windowFlag;
		DxLib::ChangeWindowMode(windowFlag);
	}
	// Sキー：コンフィグデータセーブ
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_S) ){
		IO::saveConfigData();
	}
	// Cキー：キャリブレーションモード変更
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_C) ){
		this->calibFlag = !this->calibFlag;
	}
	// Bキー：ブロック表示切り替え
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_B) ){
		this->blockDispFlag = !this->blockDispFlag;
	}
	// 1キー：Mover 1をジェネレート
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_1) ){
		this->generateChara(1,Point<int>(0,0));
	}
	// 2キー：Mover 2をジェネレート
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_2) ){
		this->generateChara(2,Point<int>(50,0));
	}
	// 3キー：Mover 3をジェネレート
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_3) ){
		this->generateChara(3,Point<int>(150,0));
	}
	// Aキー：AnalogStateに移動
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_A) ){
		GameMain::changeState( AnalogState::get() );
	}
	// Fキー：FieldStateに移動
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_F) ){
		GameMain::changeState( FieldState::get() );
	}



	/* キャリブレーションの時のみ行うこと */
	if( this->calibFlag ){
		DxLib::printfDx("<< calibration mode >>\n方向キーでブロックの位置調整\n[キーと]キーで表示倍率調整\nShiftを同時に押すと調整速度0.1倍\n");
		double shiftTime = (Keyboard::getKeyInputFrame(KEY_INPUT_LSHIFT)!=0) ? 0.1 : 1.0;
		// Ctrl中→カメラキャリブレーション
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