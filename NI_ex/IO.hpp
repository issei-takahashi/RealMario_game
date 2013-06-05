#pragma once
#include "includes.hpp"
#include "Point.hpp"

class IO
{
interface__:
	// �p�X��ǂݍ���
	static void loadPaths( string _path );
	// loadPaths()�œǂݍ��񂾑S�Ẵp�X�̃f�[�^��ǂݍ���
	static void loadAllData();
	// �萔�̒l���擾
	static double getConst( string const & _name );
	// �R���t�B�O�f�[�^(xml)��������
	static void saveConfigData(  );

	// configData��get
	static double get_zoom(){ return configData.zoom; }
	static Point<int> get_cameraPos() { return configData.cameraPos; }


	// ���s���ɏ��������ăZ�[�u����f�[�^
	class ConfigData
	{
		friend class boost::serialization::access;
	public:
		ConfigData( double _zoom, int _startx, int _starty ) 
			: zoom(_zoom), cameraPos( Point<int>(_startx,_starty) ) {}
		double zoom;           // �u���b�N��100px���牽�{�Ɋg�傷�邩
		Point<int> cameraPos;  // �t�B�[���h���J�n����u�`����W�v
		Point<int> startPoint; // �t�B�[���h��ŔF�����J�n����ʒu
		int zPixel;            // �u���b�N�񂪁Az�����ɉ��s�N�Z�����Ƃɂ��邩
	private:
		ConfigData(){}
		//�V���A���C�Y�p
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
	static map< string, string > m_path; // �f�[�^�p�X
	static map< string, double > m_const;   // �萔�e�[�u��


inner__:
	// csv��1�s���Z�����ƂɃJ�b�g
	static void cutLine( string _line, vector<string>& _dst );
	// �p�X���w�肵�Ē萔�f�[�^(csv)�ǂݍ���
	static void loadConst( stringc& _path );
	// �p�X���w�肵�ăR���t�B�O�f�[�^(xml)�ǂݍ���
	static void loadConfigData( string const & _path );

};