#pragma once

#include "includes.hpp"

template<class T>
class SingleMutexer
{
interface__:

	// Mutex�����b�N
	static void lockMutex()
	{
		
	}
	
	// Mutex���A�����b�N
	static void unlockMutex()
	{

	}

inner__:
	/* Mutex�֘A */
	static HANDLE hMutex; // �~���[�e�b�N�X�n���h��
	static bool isMutexLocked; // �~���[�e�b�N�X�����b�N����Ă��邩�ǂ���
};

#define HERE SingleMutexer

HANDLE HERE::hMutex;
bool HERE::isMutexLocked;