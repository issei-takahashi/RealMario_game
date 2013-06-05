// windows�X���b�h�֘A
#include <windows.h>
#include <process.h>
#include <iostream>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "includes.hpp"

namespace io = boost::iostreams;

class MemMapFile
{
interface__:
	MemMapFile();
	virtual ~MemMapFile();

	// �t�@�C���I�[�v��
	virtual bool openFile( string _fileName, DWORD rwflag=GENERIC_READ | GENERIC_WRITE, DWORD openflag=OPEN_EXISTING);

	// �t�@�C���X�g���[���擾
	iostream * getFileStream() const;

capcelled__:
	FILE* pPointer;   // �t�@�C���|�C���^
	DWORD dwFileSize; // �t�@�C���T�C�Y
	sp< io::stream<io::file_descriptor> > pFileStream; // �t�@�C���X�g���[��

inner__:
	HANDLE hFile;
	HANDLE hMap;
};