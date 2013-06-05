#ifdef __MAYFES_MODE__

#include "Mover.hpp"
#include "Image.hpp"
#include "Block.h"
#include "GameMain.hpp"
#include <DxLib.h>

#define HERE Mover
#define IS_IN_AIR (this->aStandBlock==NULL)
#define CHANGE_IMAGE(STR) auto it=this->m_pImg.find(STR);assert(it!=this->m_pImg.end());if(it!=this->it_m_pImg){this->it_m_pImg=it;this->it_m_pImg->second->initIterator();}

//////////////////////// by DDT /////////////////////////////
void HERE::move()
{
	// �n�`�Ƃ̓����蔻��B�@�n�`������ movingState �� vel ���Ǘ�
	// �n�`�ɂ����movingState ��ύX
	HERE::ChangeMovingStateByField();

	// movingState �ɂ���đ��x������
	HERE::ChangeVelByMovingState();

}


///////////// by DDT ///////////////////////////
// �n�`�ɂ����movingState ��ύX
void HERE::ChangeMovingStateByField()
{
	//cout << movingState  << std::endl;
	// �ǂ̈ʃu���b�N�̎�O����W�����v���邩
	int distance_for_jump = 70;

	// �󒆂ɂ���Ƃ�
	if( this->movingState == eMovingState::IN_AIR )
	{
		// ������ɓ����Ă���i�W�����v���j�Ȃ炻�̂܂܁@���n������s��Ȃ�
		if(this->vel.y <= 0)
		{
			this->movingState = eMovingState::IN_AIR ;
		}
		// ���n������
		ef( FieldState::get()->JudgeLandOn(this) )
		{
			this->movingState = eMovingState::LAND_ON ;
		}

	}
	// ���n������
	ef( this->movingState == eMovingState::LAND_ON )
	{
		// ���s�Ɉڍs
		this->movingState = eMovingState::STAND_ON_BLOCK;
	}
	// �����Ă�����
	ef( this->movingState == eMovingState::STAND_ON_BLOCK )
	{
		// �u���b�N�[�܂ł̋���
		int distance_to_block_end = this->aStandBlock->p2.x - pos.x;

		// ���̃u���b�N���Ȃ��Ȃ��Ă�����
		if( FieldState::get()->shitani_block_nai(this) )
		{
			this->movingState = eMovingState::IN_AIR;
		}
		// �O�Ƀu���b�N�̕ǂ���������
		ef( FieldState::get()->maeni_block_takai(this) && (distance_to_block_end <= distance_for_jump ) )
		{
			this->movingState = eMovingState::STOP ;
		}
		// �O�����R��������
		ef( FieldState::get()->maeni_block_heimen(this) )
		{
			// �u���b�N�[�ɗ�����A�����̏���Ă���u���b�N��O���̃u���b�N�ɍX�V
			if(distance_to_block_end <= 0)
			{
				if( this->index == 1 ) cout << "�u���b�N�[���B, distance:" << distance_to_block_end << endl;
				//out std::vector<vector<Block> >& _v_v_Block = FieldState::get()->getBlock();

				this->standOnBlockX = this->standOnBlockX+1;
				this->standOnBlockY = this->standOnBlockY;
				//out aStandBlock = &_v_v_Block[this->standOnBlockX][this->standOnBlockY];
				this->aStandBlock = FieldState::get()->getaBlock(this->standOnBlockX,this->standOnBlockY);
			}
		}
		// �O�̃u���b�N���ʏ�Ԃ�������
		ef( FieldState::get()->maeni_block_aru(this) && (distance_to_block_end <= distance_for_jump ) )
		{
			this->movingState = eMovingState::JUMP ;
		}
		// �O�̃u���b�N������Ԃ�������
		ef( FieldState::get()->maeni_block_hekomi(this) && (distance_to_block_end <= -10) ) // �J�h�ɂЂ����邽��-10
		{
			this->movingState = eMovingState::IN_AIR ;
		}
		// �O�̃u���b�N���Ȃ�������
		ef( FieldState::get()->maeni_block_zero(this)  && (distance_to_block_end <= 20) )
		{
			if( ( this->standOnBlockX <= 3 )
				 && (FieldState::get()->getDan(this->standOnBlockX+2) > this->standOnBlockY + 1) )
			{
				this->movingState = eMovingState::STOP ;
			}
			el
			{
				this->movingState = eMovingState::JUMP ;
			}
		}
	}
	// �W�����v������󒆂Ɉڍs
	ef( this->movingState == eMovingState::JUMP )
	{
		this->movingState = eMovingState::IN_AIR ;
	}
	// �~�܂��Ă���Ƃ�
	ef( this->movingState == eMovingState::STOP )
	{
		if( FieldState::get()->shitani_block_nai(this) )
		{
			this->movingState = eMovingState::IN_AIR;
		}
		ef( FieldState::get()->maeni_block_takai(this) )
		{
			this->movingState = eMovingState::STOP ;
		}
		ef( FieldState::get()->maeni_block_heimen(this) )
		{
			this->movingState = eMovingState::STAND_ON_BLOCK ;
		}
		ef( FieldState::get()->maeni_block_aru(this) )
		{
			this->movingState = eMovingState::JUMP ;
		}
	}

}

// this->movingState �ɂ���đ��x������
void HERE::ChangeVelByMovingState()
{
	Point<int> walking_speed = Point<int>(3,0);

	if( this->movingState == eMovingState::IN_AIR )
	{
		// �d�͉���
		this->vel.y += GRAV;

		// �ő呬�x�œ��ł�
		PEAK(this->vel.y,VEL_MAX_Y);
	}
	ef( this->movingState == eMovingState::LAND_ON )
	{
		vel = walking_speed;
	}
	ef( this->movingState == eMovingState::STAND_ON_BLOCK )
	{
		vel = walking_speed;
	}
	ef( this->movingState == eMovingState::JUMP )
	{
		vel.x = walking_speed.x;
		vel.y = -18;
	}
	ef( this->movingState == eMovingState::STOP )
	{
		vel.x = 0;
		vel.y = 0;
	}

}

#endif