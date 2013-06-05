#pragma once
#include "includes.hpp"
#include "Rect.hpp"
#include "Singleton.hpp"

class GameMain
{
public:
	// �Q�[���̃��C�����[�v
	static void mainLoop();
	static void changeState( class GameState * _pState );

inner__:
	static class GameState* pState;
};


class GameState
{
interface__:
	virtual void mainFlow() = 0;
};

// �J�n���
class TitleState : public GameState, public Singleton<TitleState>
{
	friend class Singleton<TitleState>;
interface__:
	void mainFlow();
};

// �t�B�[���h
class FieldState : public GameState, public Singleton<FieldState>
{
	friend class Singleton<FieldState>;
	friend class AnalogState;
interface__:
	FieldState() : calibFlag(false), blockDispFlag(true), line(0) {}
	// ���C���t���[
	void mainFlow();
	// ����̏ꏊ�̒i�����擾
	int getDan( int _line, int _x ) const;
	// RealField�̒i���ɂ��ƂÂ��ău���b�N�ꗗ���X�V
	void updateBlock();
	// Mover�ƃu���b�N�Ƃ̓����蔻���S�T���ōs���đ��x��␳����
	bool hitBlockAndChangeVelEvent( class Mover* _pMover );
	// Chara��Object�Ƃ̓����蔻���S�T���ōs��
	bool hitObjectEvent( class Chara * _pChara );

inner__:
	// �L�[����
	void keyInputAction();
	// �u���b�N�`��
	void drawBlock();
	// Chara���W�F�l���[�g
	void generateChara( int _num, Point<int> _pos );
	// Object���W�F�l���[�g
	void generateObject( int _index, Point<int> _pos );
	// �S�Ă�Mover�����s
	void executeAllMovers();
	// ������p���ăI�u�W�F�N�g���W�F�l���[�g
	void generateObjectEvent();

inner__:
	bool calibFlag; // �L�����u���[�V�������[�h���ǂ���
	bool blockDispFlag; // �u���b�N��\�����邩�ǂ���
	map<int,sp<class Chara> > m_spChara; // ���̈ꗗ
	map< class Object *, sp<class Object> > m_spObject; // �I�u�W�F�N�g�ꗗ
	vector<vector<class Block> > v_v_Block[3]; // �u���b�N�ꗗ
	int line;    // �����郉�C��
};

// �A�i���O�F�����[�h
class AnalogState : public GameState, public Singleton<AnalogState>
{
	friend class Singleton<AnalogState>;
interface__:
	AnalogState();
	// @ovr 
	void mainFlow();

inner__:
	void loadAndPrepareMovie();
	void updateHighestPoint();
	void drawMovie();

inner__:
	Point<int> highestPoint;
	map<string,sp<class Image> > m_spImage;
};