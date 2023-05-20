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
	void Canvas::drawLine(intV2 pt1, intV2 pt2, RGBA _color)
	{
		int disX = abs(pt2.x - pt1.x);
		int disY = abs(pt2.y - pt1.y);

		// 记录步进点
		int xNow = pt1.x;
		int yNow = pt1.y;

		int stepX = 0;
		int stepY = 0;

		// 判断两个方向步进的正负
		if (pt1.x < pt2.x)
		{
			stepX = 1;
		}
		else {
			stepX = -1;

		}
		if (pt1.y < pt2.y)
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