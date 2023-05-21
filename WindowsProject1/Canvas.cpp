#include "Canvas.h"
#include <math.h>
#include <vector>
#include <algorithm>

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

	// 画三角形，属于canvas画布本身应该存在的基本功能，基本遍历算法
	void Canvas::drawTriangle_scan(Point pt1, Point pt2, Point pt3)
	{
		// 构建包围体
		int left   = MIN(pt3.m_x, MIN(pt1.m_x, pt2.m_x));
		int top    = MIN(pt3.m_y, MIN(pt1.m_y, pt2.m_y));
		int right  = MAX(pt3.m_x, MAX(pt1.m_x, pt2.m_x));
		int bottom = MAX(pt3.m_y, MAX(pt1.m_y, pt2.m_y));

		// 剪裁屏幕外无用的像素
		left = left < 0 ? 0 : left;
		top = top < 0 ? 0 : top;
		right = right > (m_width - 1) ? (m_width - 1) : right;
		bottom = bottom > (m_height - 1) ? (m_height - 1) : bottom;

		// 计算直线斜率
		float k1 = (float)(pt2.m_y - pt3.m_y) / (float)(pt2.m_x - pt3.m_x);
		float k2 = (float)(pt1.m_y - pt3.m_y) / (float)(pt1.m_x - pt3.m_x);
		float k3 = (float)(pt1.m_y - pt2.m_y) / (float)(pt1.m_x - pt2.m_x);
	
		// 计算直线 b 值
		float b1 = (float)pt2.m_y - k1 * (float)pt2.m_x;
		float b2 = (float)pt3.m_y - k2 * (float)pt3.m_x;
		float b3 = (float)pt1.m_y - k3 * (float)pt1.m_x;

		// 循环判断是否在三角形范围内
		for (int x = left; x <= right; x++)
		{
			for (int y = top; y <= bottom; y++)
			{
				float judge1 = (y - (k1 * x + b1)) * (pt1.m_y - (k1 * pt1.m_x + b1));
				float judge2 = (y - (k2 * x + b2)) * (pt2.m_y - (k2 * pt2.m_x + b2));
				float judge3 = (y - (k3 * x + b3)) * (pt3.m_y - (k3 * pt3.m_x + b3));

				if (judge1 >= 0 && judge2 >= 0 && judge3 >= 0) {
					RGBA _color(255, 0, 0, 0);
					drawPoint(x, y, _color);
				}
			}
		}
	}

	// 优化画三角形算法：画任意三角形
	void Canvas::drawTriangle(Point pt1, Point pt2, Point pt3)
	{
		std::vector<Point> pVec;
		pVec.push_back(pt1);
		pVec.push_back(pt2);
		pVec.push_back(pt3);

		GT_RECT _rect(0, m_width, 0, m_height);
		// 优化判断：三角形是否在画布矩形上(简单裁剪)
		while (true)
		{
			if (judgeInRect(pt1, _rect) || judgeInRect(pt2, _rect) || judgeInRect(pt3, _rect))
			{
				break;
			}

			Point rpt1(0, 0, RGBA());
			Point rpt2(0, m_width, RGBA());
			Point rpt3(0, m_height, RGBA());
			Point rpt4(m_width, m_height, RGBA());

			if (judgeInTriangle(rpt1, pVec) || judgeInTriangle(rpt2, pVec) || judgeInTriangle(rpt3, pVec) || judgeInTriangle(rpt4, pVec))
			{
				break;
			}
			return;
		}
		
		// 按点的 Y 值进行排序，从大到小排序
		std::sort(pVec.begin(), pVec.end(), [](const Point& pt1, const Point& pt2) {return pt1.m_y > pt2.m_y; });
		
		Point ptMax = pVec[0];
		Point ptMid = pVec[1];
		Point ptMin = pVec[2];
		
		// 特殊处理：已经是平底三角形
		if (ptMax.m_y == ptMid.m_y)
		{
			drawTriangleFlat(ptMax, ptMid, ptMin);
			return;
		}
		// 特殊处理：已经是平顶三角形
		if (ptMin.m_y == ptMid.m_y)
		{
			drawTriangleFlat(ptMin, ptMid, ptMax);
			return;
		}

		// 判断是否有斜率
		float k = 0;
		if (ptMax.m_x != ptMin.m_x)
		{
			// 最长边
			k = (float)(ptMax.m_y - ptMin.m_y) / (float)(ptMax.m_x - ptMin.m_x);
		}
		float b = (float)ptMax.m_y - k * (float)ptMax.m_x;

		// 求割线的焦点
		Point newPoint(0, 0, RGBA(255, 0, 0));
		newPoint.m_y = ptMid.m_y;
		if (k == 0)
		{
			newPoint.m_x = ptMax.m_x;
		}
		else
		{
			newPoint.m_x = ((float)newPoint.m_y - b) / k;
		}

		drawTriangleFlat(ptMid, newPoint, ptMax);
		drawTriangleFlat(ptMid, newPoint, ptMin);

		return;
	}

	// 优化画三角形算法：画平底三角形
	void Canvas::drawTriangleFlat(Point ptFlat1, Point ptFlat2, Point pt)
	{
		float k1 = 0;
		float k2 = 0;

		if (ptFlat1.m_x != pt.m_x) // 第一条边的斜率不为0时
		{
			k1 = (float)(ptFlat1.m_y - pt.m_y) / (float)(ptFlat1.m_x - pt.m_x);
		}
		if (ptFlat2.m_x != pt.m_x) // 第二条边的斜率不为0时
		{
			k2 = (float)(ptFlat2.m_y - pt.m_y) / (float)(ptFlat2.m_x - pt.m_x);
		}

		// 使用公共点pt计算 b 值
		float b1 = (float)pt.m_y - (float)pt.m_x * k1;
		float b2 = (float)pt.m_y - (float)pt.m_x * k2;

		int yStart = MIN(pt.m_y, ptFlat1.m_y);
		int yEnd   = MAX(pt.m_y, ptFlat1.m_y);

		// 优化计算
		if (yStart < 0) {
			yStart = 0;
		}

		if (yEnd > m_height) {
			yEnd = m_height - 1;
		}

		for (int y = yStart; y <= yEnd; y++)
		{
			int x1 = 0;
			if (k1 == 0)
			{
				x1 = ptFlat1.m_x; // 第一条边斜率为0时
			}
			else
			{
				x1 = ((float)y - b1) / k1;
			}

			// 剪裁 x1
			if (x1 < 0)
			{
				x1 = 0;
			}
			if (x1 > m_width)
			{
				x1 = m_width - 1;
			}

			int x2 = 0;
			if (k2 == 0)
			{
				x2 = ptFlat2.m_x; // 第二条边斜率为0时
			}
			else
			{
				x2 = ((float)y - b2) / k2;
			}

			// 剪裁 x2
			if (x2 < 0)
			{
				x2 = 0;
			}
			if (x2 > m_width)
			{
				x2 = m_width - 1;
			}

			// 找到每条步进的边相交的两个点
			Point pt1(x1, y, RGBA(255, 0, 0, 1));
			Point pt2(x2, y, RGBA(255, 0, 0, 1));

			drawLine(pt1, pt2);
		}
	}

	// 优化：判断点是否在Rect中
	bool Canvas::judgeInRect(Point pt, GT_RECT _rect)
	{
		if (pt.m_x > _rect.m_left && pt.m_x < _rect.m_right && pt.m_y > _rect.m_top && pt.m_y < _rect.m_bottom)
		{
			return true;
		}
		return false;
	}

	// 优化计算：判断点是否在三角形中
	bool Canvas::judgeInTriangle(Point pt, std::vector<Point> _ptArray)
	{
		Point pt1 = _ptArray[0];
		Point pt2 = _ptArray[1];
		Point pt3 = _ptArray[2];

		int x = pt.m_x;
		int y = pt.m_y;

		// 计算直线斜率
		float k1 = (float)(pt2.m_y - pt3.m_y) / (float)(pt2.m_x - pt3.m_x);
		float k2 = (float)(pt1.m_y - pt3.m_y) / (float)(pt1.m_x - pt3.m_x);
		float k3 = (float)(pt1.m_y - pt2.m_y) / (float)(pt1.m_x - pt2.m_x);

		// 计算直线 b 值
		float b1 = (float)pt2.m_y - k1 * (float)pt2.m_x;
		float b2 = (float)pt3.m_y - k2 * (float)pt3.m_x;
		float b3 = (float)pt1.m_y - k3 * (float)pt1.m_x;

		// 循环判断是否在三角形范围内
		float judge1 = (y - (k1 * x + b1)) * (pt1.m_y - (k1 * pt1.m_x + b1));
		float judge2 = (y - (k2 * x + b2)) * (pt2.m_y - (k2 * pt2.m_x + b2));
		float judge3 = (y - (k3 * x + b3)) * (pt3.m_y - (k3 * pt3.m_x + b3));

		if (judge1 >= 0 && judge2 >= 0 && judge3 >= 0) {
			return true;
		}
		return false;
	}
}