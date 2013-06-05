#include "MemMapFile.hpp"

#include <stdio.h>

// �������}�b�v�h�t�@�C��������

MemMapFile::MemMapFile()
	:hFile(INVALID_HANDLE_VALUE),hMap(0),pPointer(NULL),dwFileSize(0)
{

}


MemMapFile::~MemMapFile()
{
	UnmapViewOfFile( this->pPointer );
	if( this->hMap != 0 ){
		CloseHandle( this->hMap );
	}
	if( this->hFile != INVALID_HANDLE_VALUE ){
		CloseHandle( this->hFile );
	}
}

// �t�@�C���I�[�v��
bool MemMapFile::openFile( string _fileName, DWORD rwflag, DWORD openflag)
{
	/* �t�@�C���I�[�v�� */
	this->hFile = CreateFile( _fileName.c_str(), rwflag, 0, 0, openflag, 0, 0 );
	if( this->hFile == INVALID_HANDLE_VALUE ){
		return false;
	}

	/* �t�@�C���}�b�s���O�I�u�W�F�N�g���쐬 */
	DWORD mapflag = PAGE_READWRITE;
	if( rwflag == GENERIC_READ ){
		mapflag = PAGE_READONLY; // �ǂݍ��ݐ�p�ɐݒ�
	}
	this->hMap = CreateFileMapping( this->hFile, 0, mapflag, 0, 0, _fileName.c_str() );
	if( this->hMap <= 0 ){
		CloseHandle( this->hFile );
		this->hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	/* �|�C���^���擾 */

	DWORD mapviewflag = FILE_MAP_WRITE;
	if( mapflag == PAGE_READONLY ){
		mapviewflag = FILE_MAP_READ;
	}
	this->pPointer = (FILE*)MapViewOfFile( this->hMap, mapviewflag, 0, 0, 0);
	if( this->pPointer == NULL){
		CloseHandle( this->hMap );
		CloseHandle( this->hFile );
		this->hMap = 0;
		this->hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	/* �t�@�C���T�C�Y���擾 */

	DWORD high;
	this->dwFileSize = ::GetFileSize( this->hFile, &high );

	/* �t�@�C���X�g���[���ϊ� */

	// �t�@�C���|�C���^���t�@�C���f�B�X�N���v�^
	int fd = fileno( this->pPointer );
	// �t�@�C���f�B�X�N���v�^ �� boost�t�@�C���X�g���[��
	this->pFileStream = ( sp< io::stream<io::file_descriptor> > )( new io::stream<io::file_descriptor>( fd, io::never_close_handle) );

	//io::stream_buffer<io::file_descriptor_source> sb( io::file_descriptor_source( fd, io::never_close_handle) );

	return true;
}


// �t�@�C���|�C���^�擾
iostream * MemMapFile::getFileStream() const
{
	return this->pFileStream.get();
}
