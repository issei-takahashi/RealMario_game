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

/* �e��X���b�h */

// ���A���t�B�[���h�X���b�h
unsigned __stdcall  realFieldThread(void *);
// �v���W�F�N�V�����X���b�h
unsigned __stdcall projectionThread(void *);


/*







*/