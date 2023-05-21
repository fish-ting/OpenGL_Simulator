#pragma once
#include <string.h>
#include "GTMATH.h"
// 与系统命名区分开
namespace GT
{
	// C++ 的结构体与 C 不太一样
	// 注意：系统存储是以 BGRA 的形式进行存储，所以结构体定义的顺序要进行调整
	struct RGBA  // 每个像素即为颜色
	{
		byte m_b; // 成员变量，以 m_ 开头
		byte m_g;
		byte m_r;
		byte m_a;

		// 构造函数
		RGBA(byte _r = 255,
			byte _g = 255,
			byte _b = 255,
			byte _a = 1 )
		{
			m_r = _r;
			m_g = _g;
			m_b = _b;
			m_a = _a;
		}
	};

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
	private:   // C++ 中定义private的方式略有不同
		int m_width;
		int m_height;
		RGBA* m_buffer;
	
	public:
		Canvas(int _width, int _height, void* _buffer) // 构造函数
		{
			// 安全检查
			if (_width <= 0 || _height <= 0)
			{
				m_width = -1;
				m_height = -1;
				m_buffer = nullptr;  // C++默认空值略有不同
			}

			m_width = _width;
			m_height = _height;
			m_buffer = (RGBA*)_buffer;
			
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

		// 画点，属于canvas画布本身应该存在的基本功能
		void drawPoint(int x, int y, RGBA _color);

		// 画线，属于canvas画布本身应该存在的基本功能
		void drawLine(Point pt1, Point pt2);

		// 画三角形，属于canvas画布本身应该存在的基本功能（最小包围盒遍历）
		void drawTriangle_scan(Point pt1, Point pt2, Point pt3);

		// 优化三角形扫描算法：1）画平底三角形
		void drawTriangleFlat(Point ptFlat1, Point ptFlat2, Point pt);

		// 优化三角形扫描算法：2）画任意三角形
		void drawTriangle(Point ptFlat1, Point ptFlat2, Point pt);

		// 颜色插值计算
		inline RGBA colorLerp(RGBA _color1, RGBA _color2, float _scale)
		{
			RGBA _color;
			_color.m_r = _color1.m_r + (float)(_color2.m_r - _color1.m_r) * _scale;
			_color.m_g = _color1.m_g + (float)(_color2.m_g - _color1.m_g) * _scale;
			_color.m_b = _color1.m_b + (float)(_color2.m_b - _color1.m_b) * _scale;
			_color.m_a = _color1.m_a + (float)(_color2.m_a - _color1.m_a) * _scale;
			return _color;
		}
	};
}


