#pragma once
#include <string.h>
#include "GTMATH.h"
#include <vector>
#include "Image.h"
// 与系统命名区分开
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
	private:   // C++ 中定义private的方式略有不同
		int m_width;
		int m_height;
		RGBA* m_buffer;

		byte m_alphaLimit; // alpha值大于该值的像素才可被绘制
		bool m_useBlend;
	
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
			m_useBlend = false; // 默认情况下不开颜色混合
			
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

		// 获取当前颜色buffer区中的颜色
		RGBA getColor(int x, int y);

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

		// 优化计算：判断三角形是否在矩形画布中
		bool judgeInRect(Point pt, GT_RECT _rect);

		// 优化计算：判断点是否在三角形中
		bool judgeInTriangle(Point pt, std::vector<Point> _ptArray);

		// 将图片画到画布上
		void drawImage(int _x, int _y, Image* _image);

		// 是否开启alpha测试
		void setAlphaLimit(byte _limit)
		{
			m_alphaLimit = _limit;
		}

		// 是否开启alpha混合
		void setBlend(bool _useBlend)
		{
			m_useBlend = _useBlend;
		}

	};
}


