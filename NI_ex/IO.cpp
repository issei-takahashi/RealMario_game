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

// �p�X��ǂݍ���
void HERE::loadPaths( string _path )
{
	HERE::m_path.clear();
	ifstream ifs( _path );
	//1�s���̃o�b�t�@
	string line;
	//�ŏ��̂P�s�͎̂Ă�
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

// loadPaths()�œǂݍ��񂾑S�Ẵp�X�̃f�[�^��ǂݍ���
void HERE::loadAllData()
{
	foreach(it,HERE::m_path){
		// �萔�f�[�^�icsv�j
		if( it->first == "CONST_DATA" ){
			HERE::loadConst( it->second );
		}
		// �R���t�B�O�f�[�^(xml)
		else if( it->first == "CONFIG_DATA" ){
			HERE::loadConfigData( it->second );
		}
	}
}

// �萔�̒l���擾
double HERE::getConst( string const & _name )
{
	auto it = IO::m_const.find( _name );

	passert( _name+"�Ƃ����萔�͑��݂��܂���B", it != IO::m_const.end() );

	return it->second;
}

// csv��1�s���Z�����ƂɃJ�b�g
void HERE::cutLine( string _line, vector<string>& _dst )
{
	_dst.clear();
	if( _line == "" || _line.find("//")==0 || _line.find(",")==0 ){
		return;
	}
	// �^�u�ƃX�y�[�X�ƃN�H�[�e�[�V�����}�[�N���J�b�g
	boost::algorithm::replace_all( _line, " ", "" );
	boost::algorithm::replace_all( _line, "\t", "" );
	boost::algorithm::replace_all( _line, "\"", "" );
	// ������_line��","�ŋ�؂���_dst�Ɋi�[
	boost::algorithm::split( _dst, _line,  boost::is_any_of(",") );
}

// �萔�f�[�^�ǂݍ���
void HERE::loadConst( stringc& _path )
{
	ifstream ifs( _path );
	//1�s���̃o�b�t�@
	string line;
	//�ŏ��̂P�s�͎̂Ă�
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

// �p�X���w�肵�ăR���t�B�O�f�[�^(xml)�ǂݍ���
void HERE::loadConfigData( string const & _path )
{
	ifstream ifs( _path );
	// �t�@�C�����Ȃ�������
	if( !ifs ){
		ofstream ofs( _path );
		boost::archive::xml_oarchive oa(ofs);
		// �f�[�^����������
		oa << boost::serialization::make_nvp( "Root", HERE::configData );
	}
	// �t�@�C������������
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
	// �f�[�^����������
	oa << boost::serialization::make_nvp( "Root", HERE::configData );
	cout << path+"��ۑ����܂����B" <<endl;
}