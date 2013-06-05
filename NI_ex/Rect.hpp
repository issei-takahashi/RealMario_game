#pragma once
#include "includes.hpp"
#include "Point.hpp"


#define LOC_LEFT  (1)
#define LOC_RIGHT (2)
#define LOC_UP    (4)
#define LOC_DOWN  (8)
#define LOC_LRHIT (16)
#define LOC_UDHIT (32) 
#define LOC_UP_FIT   (64)
#define LOC_DOWN_FIT (128)
#define LOC_LEFT_FIT (256)
#define LOC_RIGHT_FIT (512)


template<class T>
class Rect
{
interface__:
	Rect()
	{

	}
	Rect( T x1, T y1, T x2, T y2 )
		:p1(Point<T>(x1,y1)),p2(Point<T>(x2,y2))
	{

	}
	// �ʂ�Rect�ɑ΂��āu�������v�ǂ�������Ԃɂ��邩
	int Rect::getLocationType( Rect<T> const & that ) const
	{
		int ret = 0;
		
		/* �㉺���� */
		// �����̂ق�����
		if( this->p2.y < that.p1.y ){
			ret |= LOC_UP;
		}
		// �����̂ق�����Őڂ��Ă�
		ef( this->p2.y == that.p1.y ){
			ret |= LOC_UP_FIT;
		}
		// �����̂ق�����
		ef( that.p2.y < this->p1.y ){
			ret |= LOC_DOWN;
		}
		// �����̂ق����Őڂ��Ă�
		ef( that.p2.y == this->p1.y ){
			ret |= LOC_DOWN_FIT;
		}
		// �������Ă���
		el{
			ret |= LOC_UDHIT;
		}

		/* ���E���� */
		// �����̂ق�����
		if( this->p2.x < that.p1.x ){
			ret |= LOC_LEFT;
		}
		// �����̂ق������Őڂ��Ă�
		ef( this->p2.x == that.p1.x ){
			ret |= LOC_LEFT_FIT;
		}
		// �����̂ق����E
		ef( that.p2.x < this->p1.x ){
			ret |= LOC_RIGHT;
		}
		// �����̂ق����E�Őڂ��Ă�
		ef( that.p2.x == this->p1.x ){
			ret |= LOC_RIGHT_FIT;
		}
		el{
			ret |= LOC_LRHIT;
		}
		//cout << endl;
		return ret;
	}
open__:
	Point<T> p1,p2; // ����̓_�ƉE���̓_
};