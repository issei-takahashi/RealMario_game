#pragma once
#include "includes.hpp"
#include "Rect.hpp"
#include "Singleton.hpp"

class GameMain
{
public:
	// ゲームのメインループ
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

// 開始画面
class TitleState : public GameState, public Singleton<TitleState>
{
	friend class Singleton<TitleState>;
interface__:
	void mainFlow();
};

// フィールド
class FieldState : public GameState, public Singleton<FieldState>
{
	friend class Singleton<FieldState>;
	friend class AnalogState;
interface__:
	FieldState() : calibFlag(false), blockDispFlag(true), line(0) {}
	// メインフロー
	void mainFlow();
	// 特定の場所の段数を取得
	int getDan( int _line, int _x ) const;
	// RealFieldの段数にもとづいてブロック一覧を更新
	void updateBlock();
	// Moverとブロックとの当たり判定を全探索で行って速度を補正する
	bool hitBlockAndChangeVelEvent( class Mover* _pMover );
	// CharaとObjectとの当たり判定を全探索で行う
	bool hitObjectEvent( class Chara * _pChara );

inner__:
	// キー操作
	void keyInputAction();
	// ブロック描画
	void drawBlock();
	// Charaをジェネレート
	void generateChara( int _num, Point<int> _pos );
	// Objectをジェネレート
	void generateObject( int _index, Point<int> _pos );
	// 全てのMoverを実行
	void executeAllMovers();
	// 乱数を用いてオブジェクトをジェネレート
	void generateObjectEvent();

inner__:
	bool calibFlag; // キャリブレーションモードかどうか
	bool blockDispFlag; // ブロックを表示するかどうか
	map<int,sp<class Chara> > m_spChara; // 物体一覧
	map< class Object *, sp<class Object> > m_spObject; // オブジェクト一覧
	vector<vector<class Block> > v_v_Block[3]; // ブロック一覧
	int line;    // 今いるライン
};

// アナログ認識モード
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