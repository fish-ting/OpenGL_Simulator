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
		Point(int _x, int _y, RGBA _color)
		{
			m_x = _x;
			m_y = _y;
			m_color = _color;
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

	};
}


