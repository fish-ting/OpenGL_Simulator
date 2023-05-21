#include "Canvas.h"
#include <math.h>
#include <vector>
#include <algorithm>

namespace GT
{
	// ���㣬����canvas��������Ӧ�ô��ڵĻ�������
	void Canvas::drawPoint(int x, int y, RGBA _color)
	{
		if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		{
			return;
		}

		m_buffer[y * m_width + x] = _color;
	}

	// ���ߣ�����canvas��������Ӧ�ô��ڵĻ�������
	void Canvas::drawLine(Point pt1, Point pt2)
	{
		int disX = abs(pt2.m_x - pt1.m_x);
		int disY = abs(pt2.m_y - pt1.m_y);

		// ��¼������
		int xNow = pt1.m_x;
		int yNow = pt1.m_y;

		int stepX = 0;
		int stepY = 0;

		// �ж��������򲽽�������
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

		// �Ա�xyƫ���������������ķ����� x ���� y
		// Ĭ��������� X ����
		int sumStep = disX;
		bool useXStep = true;

		if (disX < disY) // k > 1��ѡȡ y Ϊ��������
		{
			sumStep = disY;
			useXStep = false;
			SWAP_INT(disX, disY);  // ����dx �� dy
		}

		// ��ʼ�� p ֵ
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
			if (p >= 0) // k > 1�����
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

			// ����������
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

	// �������Σ�����canvas��������Ӧ�ô��ڵĻ������ܣ����������㷨
	void Canvas::drawTriangle_scan(Point pt1, Point pt2, Point pt3)
	{
		// ������Χ��
		int left   = MIN(pt3.m_x, MIN(pt1.m_x, pt2.m_x));
		int top    = MIN(pt3.m_y, MIN(pt1.m_y, pt2.m_y));
		int right  = MAX(pt3.m_x, MAX(pt1.m_x, pt2.m_x));
		int bottom = MAX(pt3.m_y, MAX(pt1.m_y, pt2.m_y));

		// ������Ļ�����õ�����
		left = left < 0 ? 0 : left;
		top = top < 0 ? 0 : top;
		right = right > (m_width - 1) ? (m_width - 1) : right;
		bottom = bottom > (m_height - 1) ? (m_height - 1) : bottom;

		// ����ֱ��б��
		float k1 = (float)(pt2.m_y - pt3.m_y) / (float)(pt2.m_x - pt3.m_x);
		float k2 = (float)(pt1.m_y - pt3.m_y) / (float)(pt1.m_x - pt3.m_x);
		float k3 = (float)(pt1.m_y - pt2.m_y) / (float)(pt1.m_x - pt2.m_x);
	
		// ����ֱ�� b ֵ
		float b1 = (float)pt2.m_y - k1 * (float)pt2.m_x;
		float b2 = (float)pt3.m_y - k2 * (float)pt3.m_x;
		float b3 = (float)pt1.m_y - k3 * (float)pt1.m_x;

		// ѭ���ж��Ƿ��������η�Χ��
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

	// �Ż����������㷨��������������
	void Canvas::drawTriangle(Point pt1, Point pt2, Point pt3)
	{
		std::vector<Point> pVec;
		pVec.push_back(pt1);
		pVec.push_back(pt2);
		pVec.push_back(pt3);

		GT_RECT _rect(0, m_width, 0, m_height);
		// �Ż��жϣ��������Ƿ��ڻ���������(�򵥲ü�)
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
		
		// ����� Y ֵ�������򣬴Ӵ�С����
		std::sort(pVec.begin(), pVec.end(), [](const Point& pt1, const Point& pt2) {return pt1.m_y > pt2.m_y; });
		
		Point ptMax = pVec[0];
		Point ptMid = pVec[1];
		Point ptMin = pVec[2];
		
		// ���⴦���Ѿ���ƽ��������
		if (ptMax.m_y == ptMid.m_y)
		{
			drawTriangleFlat(ptMax, ptMid, ptMin);
			return;
		}
		// ���⴦���Ѿ���ƽ��������
		if (ptMin.m_y == ptMid.m_y)
		{
			drawTriangleFlat(ptMin, ptMid, ptMax);
			return;
		}

		// �ж��Ƿ���б��
		float k = 0;
		if (ptMax.m_x != ptMin.m_x)
		{
			// ���
			k = (float)(ptMax.m_y - ptMin.m_y) / (float)(ptMax.m_x - ptMin.m_x);
		}
		float b = (float)ptMax.m_y - k * (float)ptMax.m_x;

		// ����ߵĽ���
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

	// �Ż����������㷨����ƽ��������
	void Canvas::drawTriangleFlat(Point ptFlat1, Point ptFlat2, Point pt)
	{
		float k1 = 0;
		float k2 = 0;

		if (ptFlat1.m_x != pt.m_x) // ��һ���ߵ�б�ʲ�Ϊ0ʱ
		{
			k1 = (float)(ptFlat1.m_y - pt.m_y) / (float)(ptFlat1.m_x - pt.m_x);
		}
		if (ptFlat2.m_x != pt.m_x) // �ڶ����ߵ�б�ʲ�Ϊ0ʱ
		{
			k2 = (float)(ptFlat2.m_y - pt.m_y) / (float)(ptFlat2.m_x - pt.m_x);
		}

		// ʹ�ù�����pt���� b ֵ
		float b1 = (float)pt.m_y - (float)pt.m_x * k1;
		float b2 = (float)pt.m_y - (float)pt.m_x * k2;

		int yStart = MIN(pt.m_y, ptFlat1.m_y);
		int yEnd   = MAX(pt.m_y, ptFlat1.m_y);

		// �Ż�����
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
				x1 = ptFlat1.m_x; // ��һ����б��Ϊ0ʱ
			}
			else
			{
				x1 = ((float)y - b1) / k1;
			}

			// ���� x1
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
				x2 = ptFlat2.m_x; // �ڶ�����б��Ϊ0ʱ
			}
			else
			{
				x2 = ((float)y - b2) / k2;
			}

			// ���� x2
			if (x2 < 0)
			{
				x2 = 0;
			}
			if (x2 > m_width)
			{
				x2 = m_width - 1;
			}

			// �ҵ�ÿ�������ı��ཻ��������
			Point pt1(x1, y, RGBA(255, 0, 0, 1));
			Point pt2(x2, y, RGBA(255, 0, 0, 1));

			drawLine(pt1, pt2);
		}
	}

	// �Ż����жϵ��Ƿ���Rect��
	bool Canvas::judgeInRect(Point pt, GT_RECT _rect)
	{
		if (pt.m_x > _rect.m_left && pt.m_x < _rect.m_right && pt.m_y > _rect.m_top && pt.m_y < _rect.m_bottom)
		{
			return true;
		}
		return false;
	}

	// �Ż����㣺�жϵ��Ƿ�����������
	bool Canvas::judgeInTriangle(Point pt, std::vector<Point> _ptArray)
	{
		Point pt1 = _ptArray[0];
		Point pt2 = _ptArray[1];
		Point pt3 = _ptArray[2];

		int x = pt.m_x;
		int y = pt.m_y;

		// ����ֱ��б��
		float k1 = (float)(pt2.m_y - pt3.m_y) / (float)(pt2.m_x - pt3.m_x);
		float k2 = (float)(pt1.m_y - pt3.m_y) / (float)(pt1.m_x - pt3.m_x);
		float k3 = (float)(pt1.m_y - pt2.m_y) / (float)(pt1.m_x - pt2.m_x);

		// ����ֱ�� b ֵ
		float b1 = (float)pt2.m_y - k1 * (float)pt2.m_x;
		float b2 = (float)pt3.m_y - k2 * (float)pt3.m_x;
		float b3 = (float)pt1.m_y - k3 * (float)pt1.m_x;

		// ѭ���ж��Ƿ��������η�Χ��
		float judge1 = (y - (k1 * x + b1)) * (pt1.m_y - (k1 * pt1.m_x + b1));
		float judge2 = (y - (k2 * x + b2)) * (pt2.m_y - (k2 * pt2.m_x + b2));
		float judge3 = (y - (k3 * x + b3)) * (pt3.m_y - (k3 * pt3.m_x + b3));

		if (judge1 >= 0 && judge2 >= 0 && judge3 >= 0) {
			return true;
		}
		return false;
	}
}