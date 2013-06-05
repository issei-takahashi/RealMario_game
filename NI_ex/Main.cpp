#include "Main.hpp"
#include "link.hpp"
#include "IO.hpp"
#include "RealField.hpp"
#include "GameMain.hpp"
#include "GameLib.hpp"
// windows�X���b�h�֘A
#include <windows.h>
#include <process.h>
#include "MemMapFile.hpp"

#define HERE Main

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){ Main::main(); return 0; }

void HERE::main()
{

	HERE::init();
	HERE::createAndExecuteThread();

	//MemMapFile mmfile;
	//mmfile.openFile( "data/testmm.txt" );
	//iostream* pStream = mmfile.getFileStream();
	//*pStream << "aaaaaaaa" << endl;
}

void HERE::init()
{
	IO::loadPaths( "path.csv" );
	IO::loadAllData();
	RealField::initCamera();
	RealField::initParams();
	GameLib::initLib();
}

void HERE::createAndExecuteThread()
{
	HANDLE hThread[2];     // �X���b�h�̃n���h��
	unsigned int thID[2];  // �X���b�h��ID


	if( ( hThread[0] = (HANDLE)_beginthreadex( NULL, 0, ::realFieldThread, NULL, CREATE_SUSPENDED, &thID[0] ) ) == 0 ){  
		cout << "Main.cpp \t ���A���t�B�[���h�X���b�h�쐬���s" << endl;
		exit(0);
	}
	if( ( hThread[1] = (HANDLE)_beginthreadex( NULL, 0, ::projectionThread, NULL, CREATE_SUSPENDED, &thID[1] ) ) == 0 ){  
		cout << "Main.cpp \t �v���W�F�N�V�����X���b�h�쐬���s" << endl;
		exit(0);
	}

	ResumeThread( hThread[0] );
	ResumeThread( hThread[1] );

	GameMain::mainLoop();

	// �ҋ@
	WaitForMultipleObjects( 2, hThread, true, INFINITE );

	//�n���h���N���[�Y
	CloseHandle( hThread );
	

}

/* �e�X���b�h */

// ���A���t�B�[���h�X���b�h
unsigned __stdcall realFieldThread(void *)
{
	RealField::mainLoop();
	return 0;
}

// �v���W�F�N�V�����X���b�h
unsigned __stdcall projectionThread(void *)
{
	while(1){
		break;
	}
	return 0;
}