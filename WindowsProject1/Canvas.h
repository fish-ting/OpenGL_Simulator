#pragma once
#include <string.h>
#include "GTMATH.h"
#include <vector>
#include "Image.h"
// ��ϵͳ�������ֿ�
namespace GT
{
	class Point
	{
	public:
		float m_x;
		float m_y;
		float m_z;
		RGBA m_color;
		floatV2 m_uv;

		Point(float _x = 0, float _y = 0, float _z = 0, RGBA _color = RGBA(0, 0, 0, 0), floatV2 _uv = floatV2(0.0, 0.0))
		{
			m_x = _x;
			m_y = _y;
			m_z = _z;
			m_color = _color;
			m_uv = _uv;
		}

		~Point()
		{

		}
	};

	enum DATA_TYPE
	{
		GT_FlOAT = 0,
		GT_INT = 1
	};

	enum  DRAW_MODE
	{
		GT_LINE = 0,
		GT_TRIANGLE = 1
	};

	// �Դ���״̬������������͵�ģ��
	struct DataElement
	{
		int m_size;
		DATA_TYPE m_type;
		int m_stride; // ����
		byte* m_data;

		DataElement()
		{
			m_size = -1;
			m_type = GT_FlOAT;
			m_stride = -1;
			m_data = NULL;
		}
	};


	// ģ��״̬��
	struct Statement
	{
		bool m_useBlend;
		bool m_enableTexture; //�Ƿ�����������ͼ

		const Image* m_texture; // ��ǰʹ�õ�����
		Image::TEXTURE_TYPE m_texType; // ����ȡ���ķ�ʽ��clamp or repeat
		byte m_alphaLimit; // alphaֵ���ڸ�ֵ�����زſɱ�����

		DataElement m_vertexData;
		DataElement m_colorData;
		DataElement m_texCoordData;

		Statement() // ���캯������ֵ
		{
			m_useBlend = false;
			m_enableTexture = false;
			m_texture = NULL;
			m_texType = Image::TX_REPEAT;
			m_alphaLimit = 0;
		}
	};

	
	class Canvas
	{
	private:   // C++ �ж���private�ķ�ʽ���в�ͬ
		int m_width;
		int m_height;
		RGBA* m_buffer;
		float* m_zBuffer;

		Statement m_state;
	
	public:
		Canvas(int _width, int _height, void* _buffer) // ���캯��
		{
			// ��ȫ���
			if (_width <= 0 || _height <= 0)
			{
				m_width = -1;
				m_height = -1;
				m_buffer = NULL;
			}

			m_width = _width;
			m_height = _height;
			m_buffer = (RGBA*)_buffer;
			m_zBuffer = new float[_width * _height];
			m_state.m_useBlend = false; // Ĭ������²�����ɫ���
			m_state.m_enableTexture = false; // Ĭ������²�ʹ������
			
		}

		~Canvas()
		{
			delete[] m_zBuffer;
		}

		void clear()
		{
			if (m_buffer != NULL)
			{
				memset(m_buffer, 0, sizeof(RGBA) * m_width * m_height);
				memset(m_zBuffer, 255, sizeof(float) * m_width * m_height); // ���ֵ������Ȼ�����
			}
		}

		// ���㣬����canvas��������Ӧ�ô��ڵĻ�������
		void drawPoint(Point _pt);

		// ��ȡ��ǰ��ɫbuffer���е���ɫ
		RGBA getColor(int x, int y);

		// ���ߣ�����canvas��������Ӧ�ô��ڵĻ�������
		void drawLine(Point pt1, Point pt2);

		// �������Σ�����canvas��������Ӧ�ô��ڵĻ������ܣ���С��Χ�б�����
		//void drawTriangle_scan(Point pt1, Point pt2, Point pt3);

		// �Ż�������ɨ���㷨��1����ƽ��������
		void drawTriangleFlat(Point ptFlat1, Point ptFlat2, Point pt);

		// �Ż�������ɨ���㷨��2��������������
		void drawTriangle(Point ptFlat1, Point ptFlat2, Point pt);

		// ��ɫ��ֵ����
		inline RGBA colorLerp(RGBA _color1, RGBA _color2, float _scale)
		{
			RGBA _color;
			_color.m_r = _color1.m_r + (float)(_color2.m_r - _color1.m_r) * _scale;
			_color.m_g = _color1.m_g + (float)(_color2.m_g - _color1.m_g) * _scale;
			_color.m_b = _color1.m_b + (float)(_color2.m_b - _color1.m_b) * _scale;
			_color.m_a = _color1.m_a + (float)(_color2.m_a - _color1.m_a) * _scale;
			return _color;
		}

		// UV��ֵ����
		inline floatV2 uvLerp(floatV2 _uv1, floatV2 _uv2, float _scale)
		{
			floatV2 _uv;
			_uv.x = _uv1.x + (_uv2.x - _uv1.x) * _scale;
			_uv.y = _uv1.y + (_uv2.y - _uv1.y) * _scale;
			return _uv;
		}

		// zbuffer��ֵ����
		inline float zLerp(float _z1, float _z2, float _scale)
		{
			return _z1 + (_z2 - _z1) * _scale;
		}

		// �Ż����㣺�ж��������Ƿ��ھ��λ�����
		bool judgeInRect(Point pt, GT_RECT _rect);

		// �Ż����㣺�жϵ��Ƿ�����������
		bool judgeInTriangle(Point pt, std::vector<Point> _ptArray);

		// ��ͼƬ����������
		void drawImage(int _x, int _y, Image* _image);

		// �Ƿ���alpha����
		void setAlphaLimit(byte _limit)
		{
			m_state.m_alphaLimit = _limit;
		}

		// �Ƿ���alpha���
		void setBlend(bool _useBlend)
		{
			m_state.m_useBlend = _useBlend;
		}

		// �Ƿ�������
		void enableTexture(bool _enable)
		{
			m_state.m_enableTexture = _enable;
		}

		// ������
		void bindTexture(const Image* _image)
		{
			m_state.m_texture = _image;
		}

		// ��������ȡ���ķ�ʽ
		void setTextureType(Image::TEXTURE_TYPE _type)
		{
			m_state.m_texType = _type;
		}

		// �󶨶�������
		void gtVertexPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);
		
		// ����ɫ����
		void gtColorPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);

		// ����������
		void gtTexCoordPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);

		// ����״̬ģʽ��ͼ
		void gtDrawArray(DRAW_MODE _mode, int _first, int _count);

		bool judgeLineAndRect(int _x1, int _x2, int& _x1Cut, int& x2Cut);
	};
}


