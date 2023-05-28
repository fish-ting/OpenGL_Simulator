#pragma once
#include <string.h>
#include "GTMATH.h"
#include <vector>
#include "Image.h"
// 与系统命名区分开
namespace GT
{
	class Point
	{
	public:
		float m_x;
		float m_y;
		float m_z;
		RGBA m_color;
		floatV2 m_uv;

		Point(float _x = 0, float _y = 0, float _z = 0, RGBA _color = RGBA(0, 0, 0, 0), floatV2 _uv = floatV2(0.0, 0.0))
		{
			m_x = _x;
			m_y = _y;
			m_z = _z;
			m_color = _color;
			m_uv = _uv;
		}

		~Point()
		{

		}
	};

	enum DATA_TYPE
	{
		GT_FlOAT = 0,
		GT_INT = 1
	};

	enum  DRAW_MODE
	{
		GT_LINE = 0,
		GT_TRIANGLE = 1
	};

	// 对传入状态机里的数据类型的模拟
	struct DataElement
	{
		int m_size;
		DATA_TYPE m_type;
		int m_stride; // 步长
		byte* m_data;

		DataElement()
		{
			m_size = -1;
			m_type = GT_FlOAT;
			m_stride = -1;
			m_data = NULL;
		}
	};


	// 模拟状态机
	struct Statement
	{
		bool m_useBlend;
		bool m_enableTexture; //是否启用纹理贴图

		const Image* m_texture; // 当前使用的纹理
		Image::TEXTURE_TYPE m_texType; // 纹理取样的方式：clamp or repeat
		byte m_alphaLimit; // alpha值大于该值的像素才可被绘制

		DataElement m_vertexData;
		DataElement m_colorData;
		DataElement m_texCoordData;

		Statement() // 构造函数赋初值
		{
			m_useBlend = false;
			m_enableTexture = false;
			m_texture = NULL;
			m_texType = Image::TX_REPEAT;
			m_alphaLimit = 0;
		}
	};

	
	class Canvas
	{
	private:   // C++ 中定义private的方式略有不同
		int m_width;
		int m_height;
		RGBA* m_buffer;
		float* m_zBuffer;

		Statement m_state;
	
	public:
		Canvas(int _width, int _height, void* _buffer) // 构造函数
		{
			// 安全检查
			if (_width <= 0 || _height <= 0)
			{
				m_width = -1;
				m_height = -1;
				m_buffer = NULL;
			}

			m_width = _width;
			m_height = _height;
			m_buffer = (RGBA*)_buffer;
			m_zBuffer = new float[_width * _height];
			m_state.m_useBlend = false; // 默认情况下不开颜色混合
			m_state.m_enableTexture = false; // 默认情况下不使用纹理
			
		}

		~Canvas()
		{
			delete[] m_zBuffer;
		}

		void clear()
		{
			if (m_buffer != NULL)
			{
				memset(m_buffer, 0, sizeof(RGBA) * m_width * m_height);
				memset(m_zBuffer, 255, sizeof(float) * m_width * m_height); // 最大值填入深度缓冲区
			}
		}

		// 画点，属于canvas画布本身应该存在的基本功能
		void drawPoint(Point _pt);

		// 获取当前颜色buffer区中的颜色
		RGBA getColor(int x, int y);

		// 画线，属于canvas画布本身应该存在的基本功能
		void drawLine(Point pt1, Point pt2);

		// 画三角形，属于canvas画布本身应该存在的基本功能（最小包围盒遍历）
		//void drawTriangle_scan(Point pt1, Point pt2, Point pt3);

		// 优化三角形扫描算法：1）画平底三角形
		void drawTriangleFlat(Point ptFlat1, Point ptFlat2, Point pt);

		// 优化三角形扫描算法：2）画任意三角形
		void drawTriangle(Point ptFlat1, Point ptFlat2, Point pt);

		// 颜色插值计算
		inline RGBA colorLerp(RGBA _color1, RGBA _color2, float _scale)
		{
			RGBA _color;
			_color.m_r = _color1.m_r + (float)(_color2.m_r - _color1.m_r) * _scale;
			_color.m_g = _color1.m_g + (float)(_color2.m_g - _color1.m_g) * _scale;
			_color.m_b = _color1.m_b + (float)(_color2.m_b - _color1.m_b) * _scale;
			_color.m_a = _color1.m_a + (float)(_color2.m_a - _color1.m_a) * _scale;
			return _color;
		}

		// UV插值计算
		inline floatV2 uvLerp(floatV2 _uv1, floatV2 _uv2, float _scale)
		{
			floatV2 _uv;
			_uv.x = _uv1.x + (_uv2.x - _uv1.x) * _scale;
			_uv.y = _uv1.y + (_uv2.y - _uv1.y) * _scale;
			return _uv;
		}

		// zbuffer插值计算
		inline float zLerp(float _z1, float _z2, float _scale)
		{
			return _z1 + (_z2 - _z1) * _scale;
		}

		// 优化计算：判断三角形是否在矩形画布中
		bool judgeInRect(Point pt, GT_RECT _rect);

		// 优化计算：判断点是否在三角形中
		bool judgeInTriangle(Point pt, std::vector<Point> _ptArray);

		// 将图片画到画布上
		void drawImage(int _x, int _y, Image* _image);

		// 是否开启alpha测试
		void setAlphaLimit(byte _limit)
		{
			m_state.m_alphaLimit = _limit;
		}

		// 是否开启alpha混合
		void setBlend(bool _useBlend)
		{
			m_state.m_useBlend = _useBlend;
		}

		// 是否开启纹理
		void enableTexture(bool _enable)
		{
			m_state.m_enableTexture = _enable;
		}

		// 绑定纹理
		void bindTexture(const Image* _image)
		{
			m_state.m_texture = _image;
		}

		// 设置纹理取样的方式
		void setTextureType(Image::TEXTURE_TYPE _type)
		{
			m_state.m_texType = _type;
		}

		// 绑定顶点数据
		void gtVertexPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);
		
		// 绑定颜色数据
		void gtColorPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);

		// 绑定纹理数据
		void gtTexCoordPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);

		// 根据状态模式画图
		void gtDrawArray(DRAW_MODE _mode, int _first, int _count);

		bool judgeLineAndRect(int _x1, int _x2, int& _x1Cut, int& x2Cut);
	};
}


