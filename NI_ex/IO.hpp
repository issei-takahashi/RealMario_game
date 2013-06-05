#pragma once
#include "includes.hpp"
#include "Point.hpp"

class IO
{
interface__:
	// パスを読み込み
	static void loadPaths( string _path );
	// loadPaths()で読み込んだ全てのパスのデータを読み込み
	static void loadAllData();
	// 定数の値を取得
	static double getConst( string const & _name );
	// コンフィグデータ(xml)書き込み
	static void saveConfigData(  );

	// configDataのget
	static double get_zoom(){ return configData.zoom; }
	static Point<int> get_cameraPos() { return configData.cameraPos; }


	// 実行中に書き換えてセーブするデータ
	class ConfigData
	{
		friend class boost::serialization::access;
	public:
		ConfigData( double _zoom, int _startx, int _starty ) 
			: zoom(_zoom), cameraPos( Point<int>(_startx,_starty) ) {}
		double zoom;           // ブロックを100pxから何倍に拡大するか
		Point<int> cameraPos;  // フィールドが開始する「描画座標」
		Point<int> startPoint; // フィールド上で認識を開始する位置
		int zPixel;            // ブロック列が、z方向に何ピクセルごとにあるか
	private:
		ConfigData(){}
		//シリアライズ用
		template<class Archive>
		void serialize( Archive& ar, unsigned int ver )
		{
			ar & boost::serialization::make_nvp("zoom", this->zoom);
			ar & boost::serialization::make_nvp("cameraPos", this->cameraPos);
			ar & boost::serialization::make_nvp("startPoint", this->startPoint);
			ar & boost::serialization::make_nvp("zPixel", this->zPixel);
		}
	};

	static ConfigData configData;


capcelled__:
	static map< string, string > m_path; // データパス
	static map< string, double > m_const;   // 定数テーブル


inner__:
	// csvの1行をセルごとにカット
	static void cutLine( string _line, vector<string>& _dst );
	// パスを指定して定数データ(csv)読み込み
	static void loadConst( stringc& _path );
	// パスを指定してコンフィグデータ(xml)読み込み
	static void loadConfigData( string const & _path );

};