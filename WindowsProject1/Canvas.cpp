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
	void Canvas::drawLine(intV2 pt1, intV2 pt2, RGBA _color)
	{
		int disX = abs(pt2.x - pt1.x);
		int disY = abs(pt2.y - pt1.y);

		// ��¼������
		int xNow = pt1.x;
		int yNow = pt1.y;

		int stepX = 0;
		int stepY = 0;

		// �ж��������򲽽�������
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
}