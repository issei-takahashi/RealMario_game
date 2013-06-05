#include "IO.hpp"
#include "utils.hpp"

#define HERE IO
#define getcell(i,cells) cells.at(i)
#define getcelli(i,cells) utils::s2i(cells.at(i))
#define getcelld(i,cells) utils::s2d(cells.at(i))
#define INSERT(MAP,KEY,VAR) MAP.insert(make_pair(KEY,VAR)) 

map< string, string > HERE::m_path; 
map< string, double > HERE::m_const;
IO::ConfigData HERE::configData( 1.0, 100, 700 );

// パスを読み込み
void HERE::loadPaths( string _path )
{
	HERE::m_path.clear();
	ifstream ifs( _path );
	//1行分のバッファ
	string line;
	//最初の１行は捨てる
	getline( ifs, line );
	int lineCount = -1;
	while( ifs && getline( ifs, line ) ){
		lineCount++;
		vector<string> cells;
		HERE::cutLine( line, cells );
		int i=0;
		if( cells.size() <= 1 ){ 
			continue;
		}
		string ailias = getcell(i++,cells);
		string path   = getcell(i++,cells);
		INSERT( HERE::m_path, ailias, path );
	}
}

// loadPaths()で読み込んだ全てのパスのデータを読み込み
void HERE::loadAllData()
{
	foreach(it,HERE::m_path){
		// 定数データ（csv）
		if( it->first == "CONST_DATA" ){
			HERE::loadConst( it->second );
		}
		// コンフィグデータ(xml)
		else if( it->first == "CONFIG_DATA" ){
			HERE::loadConfigData( it->second );
		}
	}
}

// 定数の値を取得
double HERE::getConst( string const & _name )
{
	auto it = IO::m_const.find( _name );

	passert( _name+"という定数は存在しません。", it != IO::m_const.end() );

	return it->second;
}

// csvの1行をセルごとにカット
void HERE::cutLine( string _line, vector<string>& _dst )
{
	_dst.clear();
	if( _line == "" || _line.find("//")==0 || _line.find(",")==0 ){
		return;
	}
	// タブとスペースとクォーテーションマークをカット
	boost::algorithm::replace_all( _line, " ", "" );
	boost::algorithm::replace_all( _line, "\t", "" );
	boost::algorithm::replace_all( _line, "\"", "" );
	// 文字列_lineを","で区切って_dstに格納
	boost::algorithm::split( _dst, _line,  boost::is_any_of(",") );
}

// 定数データ読み込み
void HERE::loadConst( stringc& _path )
{
	ifstream ifs( _path );
	//1行分のバッファ
	string line;
	//最初の１行は捨てる
	getline( ifs, line );

	int lineCount = -1;
	while( ifs && getline( ifs, line ) ){
		lineCount++;
		vector<string> cells;
		HERE::cutLine( line, cells );
		int i=0;
		if( cells.size() <= 1 ){ 
			continue;
		}
		string name = getcell(i++,cells);
		double var = getcelld(i++,cells);
		INSERT( HERE::m_const, name, var );
	}
}

// パスを指定してコンフィグデータ(xml)読み込み
void HERE::loadConfigData( string const & _path )
{
	ifstream ifs( _path );
	// ファイルがなかったら
	if( !ifs ){
		ofstream ofs( _path );
		boost::archive::xml_oarchive oa(ofs);
		// データを書き込む
		oa << boost::serialization::make_nvp( "Root", HERE::configData );
	}
	// ファイルがあったら
	else{
		boost::archive::xml_iarchive ia(ifs);
		ia >> boost::serialization::make_nvp( "Root", HERE::configData );
	}
}

void HERE::saveConfigData()
{
	string const path = HERE::m_path.at("CONFIG_DATA");
	ofstream ofs( path );
	boost::archive::xml_oarchive oa(ofs);
	// データを書き込む
	oa << boost::serialization::make_nvp( "Root", HERE::configData );
	cout << path+"を保存しました。" <<endl;
}