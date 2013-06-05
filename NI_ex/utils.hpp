#pragma once
#include "includes.hpp"

class utils{
public:
	//string‚ðdouble‚É•ÏŠ·
	static double s2d(const string& str)
	{
		double rt;
		stringstream ss;
		ss << str;
		ss >> rt;
		return rt;
	}

	//string‚ðint‚É•ÏŠ·
	static int s2i(const string& str)
	{
		int rt;
		stringstream ss;
		ss << str;
		ss >> rt;
		return rt;
	}

	//double‚ðstring‚É•ÏŠ·
	static string d2s(double d)
	{
		string rt;
		stringstream ss;
		ss << d;
		ss >> rt;
		return rt;
	}

	//int‚ðstring‚É•ÏŠ·
	static string i2s(int d)
	{
		string rt;
		stringstream ss;
		ss << d;
		ss >> rt;
		return rt;
	}

	// 2^n < var ‚Æ‚È‚éÅ‘å‚Ìn
	static int lowerPow( int var, int fund )
	{
		assert(fund>1);
		int tmp = 1;
		int n = 0;
		while( tmp < var ){
			tmp *= fund;
			n++;
		}
		return n-1;
	}

	static int upperPow( int var, int fund )
	{
		return 1 + lowerPow( var, fund );
	}
};