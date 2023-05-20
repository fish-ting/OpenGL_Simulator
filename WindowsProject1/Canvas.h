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
			byte _a = 255 )
		{
			m_r = _r;
			m_g = _g;
			m_b = _b;
			m_a = _a;
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

		// 画点，属于canvas画布本身应该存在的功能方法
		void drawPoint(int x, int y, RGBA _color)
		{
			if (x < 0 || x >= m_width || y < 0 || y >= m_height)
			{
				return;
			}

			m_buffer[y * m_width + x] = _color;
		}


	};
}


