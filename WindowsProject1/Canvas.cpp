#include "Canvas.h"
#include <math.h>

namespace GT
{
	// 画点，属于canvas画布本身应该存在的基本功能
	void Canvas::drawPoint(int x, int y, RGBA _color)
	{
		if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		{
			return;
		}

		m_buffer[y * m_width + x] = _color;
	}

	// 画线，属于canvas画布本身应该存在的基本功能
	void Canvas::drawLine(Point pt1, Point pt2)
	{
		int disX = abs(pt2.m_x - pt1.m_x);
		int disY = abs(pt2.m_y - pt1.m_y);

		// 记录步进点
		int xNow = pt1.m_x;
		int yNow = pt1.m_y;

		int stepX = 0;
		int stepY = 0;

		// 判断两个方向步进的正负
		if (pt1.m_x < pt2.m_x)
		{
			stepX = 1;
		}
		else {
			stepX = -1;

		}
		if (pt1.m_y < pt2.m_y)
		{
			stepY = 1;
		}
		else {
			stepY = -1;
		}

		// 对比xy偏移量，决定步进的方向是 x 还是 y
		// 默认情况下是 X 方向
		int sumStep = disX;
		bool useXStep = true;

		if (disX < disY) // k > 1，选取 y 为步进方向
		{
			sumStep = disY;
			useXStep = false;
			SWAP_INT(disX, disY);  // 交换dx 和 dy
		}

		// 初始化 p 值
		int p = 2 * disY - disX;

		for (int i = 0; i < sumStep; i++)
		{
			RGBA _color;
			float _scale;
			if (useXStep)
			{
				_scale = (float)(xNow - pt1.m_x) / (float)(pt2.m_x - pt1.m_x);
			}
			else
			{
				_scale = (float)(yNow - pt1.m_y) / (float)(pt2.m_y - pt1.m_y);
			}
			_color = colorLerp(pt1.m_color, pt2.m_color, _scale);
			drawPoint(xNow, yNow, _color);
			if (p >= 0) // k > 1的情况
			{
				if (useXStep)
				{
					yNow += stepY;
				}
				else
				{
					xNow += stepX;
				}
				p = p - 2 * disX;
			}

			// 步进主坐标
			if (useXStep)
			{
				xNow += stepX;
			}
			else {
				yNow += stepY;
			}
			p = p + 2 * disY;
		}
	}
}