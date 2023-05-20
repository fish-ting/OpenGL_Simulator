#pragma once
#include <string.h>
#include "GTMATH.h"
// ��ϵͳ�������ֿ�
namespace GT
{
	// C++ �Ľṹ���� C ��̫һ��
	// ע�⣺ϵͳ�洢���� BGRA ����ʽ���д洢�����Խṹ�嶨���˳��Ҫ���е���
	struct RGBA  // ÿ�����ؼ�Ϊ��ɫ
	{
		byte m_b; // ��Ա�������� m_ ��ͷ
		byte m_g;
		byte m_r;
		byte m_a;

		// ���캯��
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
	private:   // C++ �ж���private�ķ�ʽ���в�ͬ
		int m_width;
		int m_height;
		RGBA* m_buffer;
	
	public:
		Canvas(int _width, int _height, void* _buffer) // ���캯��
		{
			// ��ȫ���
			if (_width <= 0 || _height <= 0)
			{
				m_width = -1;
				m_height = -1;
				m_buffer = nullptr;  // C++Ĭ�Ͽ�ֵ���в�ͬ
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

		// ���㣬����canvas��������Ӧ�ô��ڵĹ��ܷ���
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


