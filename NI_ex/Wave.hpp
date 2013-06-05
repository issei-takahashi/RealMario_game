#pragma once
#include "includes.hpp"

class Wave
{
	//friend class FT;
public:
	int size() const { return this->m_r.size(); }
	double getRe( double i, bool zeroFlag = false ) const 
	{
		auto it = this->m_r.find(i);
		if( it == this->m_r.end() ){
			if( zeroFlag ){
				return 0;
			}
			el{
				passert("Wave��Re�͈͊O�A�N�Z�X�ł��B",0);
			}
		}
		el{
			return it->second;
		}

	}
	double getIm( double i, bool zeroFlag = false ) const
	{
		auto it = this->m_i.find(i);
		if( it == this->m_i.end() ){
			if( zeroFlag ){
				return 0;
			}
			el{
				passert("Wave��Im�͈͊O�A�N�Z�X�ł��B",0);
			}
		}
		el{
			return it->second;
		}

	}

//private:
	map< double, double > m_r;
	map< double, double > m_i;
};