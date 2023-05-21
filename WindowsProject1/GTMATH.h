#pragma once
namespace GT
{
	template<typename T>
	void swapT(T& a, T& b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

	#define MIN(a, b) ( (a) < (b) ? (a) : (b))
	#define MAX(a, b) ( (a) > (b) ? (a) : (b))

	#define SWAP_INT(a, b) swapT<int>(a, b)


	template<typename T>
	struct tVec2
	{
		T x;
		T y;

		tVec2(T _x, T _y)
		{
			x = _x;
			y = _y;
		}

		tVec2()
		{
			x = -1;
			y = -1;
		}
	};

	typedef tVec2<int>     intV2;
	typedef tVec2<float>   floatV2;

	typedef unsigned int   uint;
	typedef unsigned char  byte;

	struct GT_RECT
	{
		int m_left;
		int m_right;
		int m_top;
		int m_bottom;

		GT_RECT(int _left = 0,
				int _right = 0,
				int _top = 0,
				int _bottom = 0)
		{
			m_left = _left;
			m_right = _right;
			m_top = _top;
			m_bottom = _bottom;
		}
	};

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
			byte _a = 1)
		{
			m_r = _r;
			m_g = _g;
			m_b = _b;
			m_a = _a;
		}
	};
}
