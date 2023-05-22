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
		int m_x;
		int m_y;
		RGBA m_color;
		floatV2 m_uv;

		Point(int _x = 0, int _y = 0, RGBA _color = RGBA(0, 0, 0, 0), floatV2 _uv = floatV2(0.0, 0.0))
		{
			m_x = _x;
			m_y = _y;
			m_color = _color;
			m_uv = _uv;
		}

		~Point()
		{

		}
	};

	
	class Canvas
	{
	private:   // C++ �ж���private�ķ�ʽ���в�ͬ
		int m_width;
		int m_height;
		RGBA* m_buffer;

		byte m_alphaLimit; // alphaֵ���ڸ�ֵ�����زſɱ�����
		bool m_useBlend;
		bool m_enableTexture; //�Ƿ�����������ͼ

		const Image* m_texture; // ��ǰʹ�õ�����
		Image::TEXTURE_TYPE m_texType; // ����ȡ���ķ�ʽ��clamp or repeat
	
	public:
		Canvas(int _width, int _height, void* _buffer) // ���캯��
		{
			// ��ȫ���
			if (_width <= 0 || _height <= 0)
			{
				m_width = -1;
				m_height = -1;
				m_buffer = nullptr;  // C++Ĭ�Ͽ�ֵ���в�ͬ
			}

			m_width = _width;
			m_height = _height;
			m_buffer = (RGBA*)_buffer;
			m_useBlend = false; // Ĭ������²�����ɫ���
			m_enableTexture = false; // Ĭ������²�ʹ������
			
		}

		~Canvas()
		{
		}

		void clear()
		{
			if (m_buffer != nullptr)
			{
				memset(m_buffer, 0, sizeof(RGBA) * m_width * m_height);
			}
		}

		// ���㣬����canvas��������Ӧ�ô��ڵĻ�������
		void drawPoint(int x, int y, RGBA _color);

		// ��ȡ��ǰ��ɫbuffer���е���ɫ
		RGBA getColor(int x, int y);

		// ���ߣ�����canvas��������Ӧ�ô��ڵĻ�������
		void drawLine(Point pt1, Point pt2);

		// �������Σ�����canvas��������Ӧ�ô��ڵĻ������ܣ���С��Χ�б�����
		void drawTriangle_scan(Point pt1, Point pt2, Point pt3);

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

		// �Ż����㣺�ж��������Ƿ��ھ��λ�����
		bool judgeInRect(Point pt, GT_RECT _rect);

		// �Ż����㣺�жϵ��Ƿ�����������
		bool judgeInTriangle(Point pt, std::vector<Point> _ptArray);

		// ��ͼƬ����������
		void drawImage(int _x, int _y, Image* _image);

		// �Ƿ���alpha����
		void setAlphaLimit(byte _limit)
		{
			m_alphaLimit = _limit;
		}

		// �Ƿ���alpha���
		void setBlend(bool _useBlend)
		{
			m_useBlend = _useBlend;
		}

		// �Ƿ�������
		void enableTexture(bool _enable)
		{
			m_enableTexture = _enable;
		}

		// ������
		void bindTexture(const Image* _image)
		{
			m_texture = _image;
		}

		// 
		void setTextureType(Image::TEXTURE_TYPE _type)
		{
			m_texType = _type;
		}
	};
}


