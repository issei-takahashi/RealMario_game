#pragma once


/*--------------- TemplateClass Singleton-----------------

�ړI�F
�V���O���g���p�^�[���̊��N���X�B�ȉ���3�X�e�b�v��Hoge�N��
�X�̃V���O���g������������B
 1.�^T��Hoge�Ɏw�肵��Singleton���p��
 2.�p������Hoge����Singleton<Hoge>��friend�ɂ���
 3.Hoge�̃R���X�g���N�^�A�f�X�g���N�^��private�w��ɂ���

����:�e���v���[�g

--------------------------------------------------------*/

template< class T >
class Singleton
{
public:
	static T* get()
	{
		static T instance;
		return &instance;
	}
private:
	// �R�s�[�֎~
	Singleton( const T& other ){}
	Singleton& operator = ( const Singleton& other ){}
protected:
	Singleton(){}
	virtual ~Singleton(){}
};
