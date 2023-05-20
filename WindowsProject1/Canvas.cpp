#include "Canvas.h"
#include <math.h>

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

	// �������Σ�����canvas��������Ӧ�ô��ڵĻ�������
	void Canvas::drawTriangle(Point pt1, Point pt2, Point pt3)
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
}