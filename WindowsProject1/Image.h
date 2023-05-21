#pragma once
#include "GTMATH.h"
#include <string.h>

namespace GT
{
	class Image
	{
	private:
		int m_width;
		int m_height;
		RGBA* m_data;

	public:
		int getWidth()const
		{
			return m_width;
		}

		int getHeight()const
		{
			return m_height;
		}

		RGBA getColor(int x, int y)
		{
			if (x < 0 || x > m_width - 1 || y < 0 || y > m_height - 1)
			{
				return RGBA(0, 0, 0, 1);
			}
			return m_data[m_width * y + x];
		}

		Image(int _width = 0, int _height = 0, byte* _data = NULL)
		{
			m_width = _width;
			m_height = _height;
			if (_data)
			{
				m_data = new RGBA[m_width * m_height];
				memcpy(m_data, _data, sizeof(RGBA) * m_width * m_height);
			}
		}

		~Image()
		{
			if (m_data)
			{
				delete[] m_data;
			}
		}

	public:
		// 功能函数：从图片路径读入指定图片
		static Image* readFromFile(const char* _fileName);
	};
}


