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

		RGBA _color;
		for (int i = 0; i <= sumStep; i++)
		{
			float _scale;
			if (useXStep)
			{
				if (pt2.m_x == pt1.m_x)
				{
					_scale = 0;
				}
				else 
				{
					_scale = (float)(xNow - pt1.m_x) / (float)(pt2.m_x - pt1.m_x);
				}
			}
			else
			{
				if (pt2.m_y == pt1.m_y)
				{
					_scale = 0;
				}
				else 
				{
					_scale = (float)(yNow - pt1.m_y) / (float)(pt2.m_y - pt1.m_y);
				}
			}

			if (m_state.m_enableTexture)
			{
				floatV2 _uv = uvLerp(pt1.m_uv, pt2.m_uv, _scale); // 使用纹理
				if (m_state.m_texture)
				{
					_color = m_state.m_texture->getColorByUV(_uv.x, _uv.y, m_state.m_texType);
				}
				else
				{
					_color = colorLerp(pt1.m_color, pt2.m_color, _scale); // 加载不到图片时使用颜色
				}
			}
			else 
			{
				_color = colorLerp(pt1.m_color, pt2.m_color, _scale);
			}
			
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

			Point rpt1(0, 0, 0, RGBA());
			Point rpt2(0, m_width, 0, RGBA());
			Point rpt3(0, m_height, 0, RGBA());
			Point rpt4(m_width, m_height, 0, RGBA());

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
		Point newPoint(0, 0, 0, RGBA(255, 0, 0));
		newPoint.m_y = ptMid.m_y;
		if (k == 0)
		{
			newPoint.m_x = ptMax.m_x;
		}
		else
		{
			newPoint.m_x = ((float)newPoint.m_y - b) / k;
		}

		float s = (float)(newPoint.m_y - ptMin.m_y) / (float)(ptMax.m_y - ptMin.m_y);
		newPoint.m_color = colorLerp(ptMin.m_color, ptMax.m_color, s);
		newPoint.m_uv = uvLerp(ptMin.m_uv, ptMax.m_uv, s);

		drawTriangleFlat(ptMid, newPoint, ptMax);
		drawTriangleFlat(ptMid, newPoint, ptMin);

		return;
	}

	bool Canvas::judgeLineAndRect(int _x1, int _x2, int& _x1Cut, int& _x2Cut)
	{
		// 完全出界，直接丢弃不画
		if (_x1 < 0 && _x2 < 0)
		{
			return false;
		}
		if (_x1 > m_width - 1 && _x2 >m_width - 1)
		{
			return false;
		}

		// 裁剪x1
		_x1Cut = _x1;
		if (_x1 < 0)
		{
			_x1Cut = 0;
		}
		if (_x1 > m_width - 1)
		{
			_x1Cut = m_width - 1;
		}
		// 裁剪x2
		_x2Cut = _x2;
		if (_x2 < 0)
		{
			_x2Cut = 0;
		}
		if (_x2 > m_width - 1)
		{
			_x2Cut = m_width - 1;
		}
		return true;
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

		// 颜色插值计算
		RGBA colorStart1;
		RGBA colorEnd1;
		RGBA colorStart2;
		RGBA colorEnd2;

		floatV2 uvStart1;
		floatV2 uvEnd1;
		floatV2 uvStart2;
		floatV2 uvEnd2;

		if (pt.m_y < ptFlat1.m_y)
		{
			yStart = pt.m_y;
			yEnd = ptFlat1.m_y;

			colorStart1 = pt.m_color;
			colorEnd1 = ptFlat1.m_color;
			colorStart2 = pt.m_color;
			colorEnd2 = ptFlat2.m_color;

			uvStart1 = pt.m_uv;
			uvEnd1 = ptFlat1.m_uv;
			uvStart2 = pt.m_uv;
			uvEnd2 = ptFlat2.m_uv;
		}
		else
		{
			yStart = ptFlat1.m_y;
			yEnd = pt.m_y;

			colorStart1 = ptFlat1.m_color;
			colorEnd1 = pt.m_color;
			colorStart2 = ptFlat2.m_color;
			colorEnd2 = pt.m_color;

			uvStart1 = ptFlat1.m_uv;
			uvEnd1 = pt.m_uv;
			uvStart2 = ptFlat2.m_uv;
			uvEnd2 = pt.m_uv;
		}

		float yColorStep = 1.0 / (float)(yEnd - yStart);
		int yColorStart = yStart;

		// 优化计算
		if (yStart < 0) {
			yStart = 0;
		}

		if (yEnd > m_height) {
			yEnd = m_height - 1;
		}

		for (int y = yStart; y < yEnd; y++)
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

			int x2 = 0;
			if (k2 == 0)
			{
				x2 = ptFlat2.m_x; // 第二条边斜率为0时
			}
			else
			{
				x2 = ((float)y - b2) / k2;
			}

			// 剪裁 x1 和 x2
			int x1Cut = x1;
			int x2Cut = x2;
			if (!judgeLineAndRect(x1, x2, x1Cut, x2Cut))
			{
				continue;
			}

			// 找到每条步进的边相交的两个点
			float s = (float)(y - yColorStart) * yColorStep;
			RGBA _color1 = colorLerp(colorStart1, colorEnd1, s);
			RGBA _color2 = colorLerp(colorStart2, colorEnd2, s);

			// 避免裁剪x坐标产生的影响
			RGBA _color1Cut = _color1;
			RGBA _color2Cut = _color2;
			if (x2 != x1)
			{
				_color1Cut = colorLerp(_color1, _color2, (float)(x1Cut - x1) / (float)(x2 - x1));
				_color2Cut = colorLerp(_color1, _color2, (float)(x2Cut - x1) / (float)(x2 - x1));
			}

			floatV2 _uv1 = uvLerp(uvStart1, uvEnd1, s);
			floatV2 _uv2 = uvLerp(uvStart2, uvEnd2, s);

			floatV2 _uv1Cut = _uv1;
			floatV2 _uv2Cut = _uv2;
			if (x2 != x1)
			{
				_uv1Cut = uvLerp(_uv1, _uv2, (float)(x1Cut - x1) / (float)(x2 - x1));
				_uv2Cut = uvLerp(_uv1, _uv2, (float)(x2Cut - x1) / (float)(x2 - x1));
			}

			Point pt1(x1Cut, y, 0, _color1Cut, _uv1Cut);
			Point pt2(x2Cut, y, 0, _color2Cut, _uv2Cut);

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

		if (judge1 > 0 && judge2 > 0 && judge3 > 0) {
			return true;
		}
		return false;
	}

	// 将图片画到画布上
	void Canvas::drawImage(int _x, int _y, Image* _image)
	{
		for (int u = 0; u < _image->getWidth(); u++)
		{
			for (int v = 0; v < _image->getHeight(); v++)
			{
				RGBA _srcColor = _image->getColor(u, v);
				if (!m_state.m_useBlend) // 非混合
				{
					drawPoint(_x + u, _y + v, _srcColor); // 未开启颜色混合
				}
				else // 混合
				{
					RGBA _dstColor = getColor(_x + u, _y + v);
					float _srcAlpha = (float)_srcColor.m_a / 255.0;
					RGBA _finalColor = colorLerp(_dstColor, _srcColor, _image->getAlpha() * _srcAlpha);
					drawPoint(_x + u, _y + v, _finalColor);
				}
			}
		}
	}

	// 获取当前颜色buffer区中的颜色
	RGBA Canvas::getColor(int x, int y)
	{
		if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		{
			return RGBA(0, 0, 0, 0);;
		}

		return m_buffer[y * m_width + x];
	}

	// 绑定顶点数据
	void Canvas::gtVertexPointer(int _size, DATA_TYPE _type, int _stride, byte* _data)
	{
		m_state.m_vertexData.m_size = _size;
		m_state.m_vertexData.m_type = _type;
		m_state.m_vertexData.m_stride = _stride;
		m_state.m_vertexData.m_data = _data;
	}

	// 绑定颜色数据
	void Canvas::gtColorPointer(int _size, DATA_TYPE _type, int _stride, byte* _data)
	{
		m_state.m_colorData.m_size = _size;
		m_state.m_colorData.m_type = _type;
		m_state.m_colorData.m_stride = _stride;
		m_state.m_colorData.m_data = _data;
	}
	
	// 绑定纹理数据
	void Canvas::gtTexCoordPointer(int _size, DATA_TYPE _type, int _stride, byte* _data)
	{
		m_state.m_texCoordData.m_size = _size;
		m_state.m_texCoordData.m_type = _type;
		m_state.m_texCoordData.m_stride = _stride;
		m_state.m_texCoordData.m_data = _data;
	}

	// 根据状态模式画图
	void Canvas::gtDrawArray(DRAW_MODE _mode, int _first, int _count)
	{
		// 直线
		Point pt0, pt1, pt2;
		byte* _vertexData = m_state.m_vertexData.m_data + _first * m_state.m_vertexData.m_stride;
		byte* _colorData = m_state.m_colorData.m_data + _first * m_state.m_colorData.m_stride;
		byte* _texCoordData = m_state.m_texCoordData.m_data + _first * m_state.m_texCoordData.m_stride;
		_count = _count - _first;
		switch (_mode)
		{
		case GT::GT_LINE:
		{
			_count = _count / 2;
			for (int i = 0; i < _count; i++) // 每条线一次消耗两个点
			{
				// 取点的坐标
				float* _vertexDataFloat = (float*)_vertexData;
				pt0.m_x = _vertexDataFloat[0];
				pt0.m_y = _vertexDataFloat[1];
				_vertexData += m_state.m_vertexData.m_stride;

				_vertexDataFloat = (float*)_vertexData;
				pt1.m_x = _vertexDataFloat[0];
				pt1.m_y = _vertexDataFloat[1];
				_vertexData += m_state.m_vertexData.m_stride;

				// 取点的颜色
				RGBA* _colorDataRGBA = (RGBA*)_colorData;
				pt0.m_color = _colorDataRGBA[0];

				_colorData += m_state.m_colorData.m_stride;

				_colorDataRGBA = (RGBA*)_colorData;
				pt1.m_color = _colorDataRGBA[0];

				_colorData += m_state.m_colorData.m_stride;
				drawLine(pt0, pt1);
			}
		}
			break;
		case GT::GT_TRIANGLE:
			_count = _count / 3;
			for (int i = 0; i < _count; i++) // 每次消耗三个点
			{
				// 取点的坐标
				float* _vertexDataFloat = (float*)_vertexData;
				pt0.m_x = _vertexDataFloat[0];
				pt0.m_y = _vertexDataFloat[1];
				_vertexData += m_state.m_vertexData.m_stride;

				_vertexDataFloat = (float*)_vertexData;
				pt1.m_x = _vertexDataFloat[0];
				pt1.m_y = _vertexDataFloat[1];
				_vertexData += m_state.m_vertexData.m_stride;

				_vertexDataFloat = (float*)_vertexData;
				pt2.m_x = _vertexDataFloat[0];
				pt2.m_y = _vertexDataFloat[1];
				_vertexData += m_state.m_vertexData.m_stride;

				// 取点的颜色
				RGBA* _colorDataRGBA = (RGBA*)_colorData;
				pt0.m_color = _colorDataRGBA[0];

				_colorData += m_state.m_colorData.m_stride;

				_colorDataRGBA = (RGBA*)_colorData;
				pt1.m_color = _colorDataRGBA[0];

				_colorData += m_state.m_colorData.m_stride;

				_colorDataRGBA = (RGBA*)_colorData;
				pt2.m_color = _colorDataRGBA[0];

				_colorData += m_state.m_colorData.m_stride;
				drawTriangle(pt0, pt1, pt2);

				// 取点的UV
				floatV2* _uvData = (floatV2*)_texCoordData;
				pt0.m_uv = _uvData[0];
				_texCoordData += m_state.m_texCoordData.m_stride;

				_uvData = (floatV2*)_texCoordData;
				pt1.m_uv = _uvData[0];
				_texCoordData += m_state.m_texCoordData.m_stride;

				_uvData = (floatV2*)_texCoordData;
				pt2.m_uv = _uvData[0];
				_texCoordData += m_state.m_texCoordData.m_stride;

				drawTriangle(pt0, pt1, pt2);
			}
			break;
		default:
			break;
		}
	}
	
}