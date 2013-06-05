#pragma once 

#include "includes.hpp"

class Main
{
interface__:
	static void main();

inner__:
	static void init();
	static void createAndExecuteThread();
};

/* 各種スレッド */

// リアルフィールドスレッド
unsigned __stdcall  realFieldThread(void *);
// プロジェクションスレッド
unsigned __stdcall projectionThread(void *);


/*







*/