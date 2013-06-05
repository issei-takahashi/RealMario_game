#include "SingleMutexer.hpp"

#define HERE SingleMutexer

HERE::SingleMutexer()
	:hMutex( CreateMutex( NULL, FALSE, NULL) ), isMutexLocked(false)
{

}

void HERE::lockMutex()
{
	if( WaitForSingleObject( HERE::hMutex, INFINITE ) == WAIT_FAILED ){ // �~���[�e�b�N�X�̃��b�N
		cout << "Mutex�̃��b�N���s" << endl;
		exit(1);
	}
	HERE::isMutexLocked = true;
}

void HERE::unlockMutex()
{
	if( ReleaseMutex( HERE::hMutex ) == 0 ){ // �~���[�e�b�N�X�����
		cout << "Mutex�A�����b�N���s" << endl;
		exit(1);
	}
}