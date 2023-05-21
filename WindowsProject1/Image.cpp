#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GT
{
	// 功能函数：从图片路径读入指定图片
	Image* Image::readFromFile(const char* _fileName)
	{
		int m_picType = 0;
		int m_width = 0;
		int m_height = 0;

		// stbImage读入的图片是反过来的
		stbi_set_flip_vertically_on_load(true);
		// 读取
		unsigned char* bits = stbi_load(_fileName, &m_width, &m_height, &m_picType, STBI_rgb_alpha);
		
		// 图片显示，r 和 b 通道要反过来
		for (int i = 0; i < m_width * m_height * 4; i+=4)
		{
			byte tmp = bits[i];
			bits[i] = bits[i + 2];
			bits[i + 2] = tmp;
		}
		
		Image* m_image = new Image(m_width, m_height, bits);
		// 释放
		stbi_image_free(bits);
		return m_image;
	}

	// 功能函数：处理图片缩放，线性插值算法
	Image* Image::simpleZoomImage(const Image* _image, float _zoomX, float _zoomY)
	{
		int _width = _image->getWidth() * _zoomX;
		int _height = _image->getHeight() * _zoomY;
		byte* _data = new byte[_width * _height * sizeof(RGBA)];
		Image* _resultImage = NULL;

		for (int i = 0; i < _width; i++)
		{
			for (int j = 0; j < _height; j++)
			{
				// 找到原图中对应的像素点
				int _imageX = (float)i / _zoomX;
				int _imageY = (float)j / _zoomY;

				_imageX = _imageX < _image->getWidth() ? _imageX : (_image->getWidth() - 1);
				_imageY = _imageY < _image->getHeight() ? _imageY : (_image->getHeight() - 1);

				RGBA _color = _image->getColor(_imageX, _imageY);
				memcpy(_data + (j * _width + i) * sizeof(RGBA), &_color, sizeof(RGBA));
			}
		}
		_resultImage = new Image(_width, _height, _data);
		delete[] _data;
		return _resultImage;
	}

	// 功能函数：处理图片缩放，双线性插值算法
	Image* Image::zoomImage(const Image* _image, float _zoomX, float _zoomY)
	{
		int _width = _image->getWidth() * _zoomX;
		int _height = _image->getHeight() * _zoomY;
		byte* _data = new byte[_width * _height * sizeof(RGBA)];
		Image* _resultImage = NULL;

		// 双线性插值
		float coordX = 0, coordY = 0;
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		float disX1 = 0, disY1 = 0, disX2 = 0, disY2 = 0;

		for (int i = 0; i < _width; i++)
		{
			coordX = (float)i / _zoomX;
			x1 = (int)coordX;
			if (x1 >= _image->getWidth() - 1)
			{
				x1 = _image->getWidth() - 1;
				x2 = x1;
			}
			else 
			{
				x2 = x1 + 1;
			}
			disX1 = coordX - x1;
			disX2 = 1 - disX1;
			
			for (int j = 0; j < _height; j++)
			{
				coordY = (float)j / _zoomY;
				y1 = (int)coordY;
				if (y1 >= _image->getHeight() - 1)
				{
					y1 = _image->getHeight() - 1;
					y2 = y1;
				}
				else 
				{
					y2 = y1 + 1;
				}
				disY1 = coordY - y1;
				disY2 = 1 - disY1;

				// 计算颜色
				RGBA _color11 = _image->getColor(x1, y1);
				RGBA _color12 = _image->getColor(x1, y2);
				RGBA _color21 = _image->getColor(x2, y1);
				RGBA _color22 = _image->getColor(x2, y2);

				RGBA _targetColor;
				_targetColor.m_r = (float)_color11.m_r * disX2 * disY2
					+ (float)_color12.m_r * disX2 * disY1
					+ (float)_color21.m_r * disX1 * disY2
					+ (float)_color22.m_r * disX1 * disY1;
				_targetColor.m_g = (float)_color11.m_g * disX2 * disY2
					+ (float)_color12.m_g * disX2 * disY1
					+ (float)_color21.m_g * disX1 * disY2
					+ (float)_color22.m_g * disX1 * disY1;
				_targetColor.m_b = (float)_color11.m_b * disX2 * disY2
					+ (float)_color12.m_b * disX2 * disY1
					+ (float)_color21.m_b * disX1 * disY2
					+ (float)_color22.m_b * disX1 * disY1;
				_targetColor.m_a = (float)_color11.m_a * disX2 * disY2
					+ (float)_color12.m_a * disX2 * disY1
					+ (float)_color21.m_a * disX1 * disY2
					+ (float)_color22.m_a * disX1 * disY1;
				memcpy(_data + (j * _width + i) * sizeof(RGBA), &_targetColor, sizeof(RGBA));
			}
		}

		_resultImage = new Image(_width, _height, _data);
		delete[] _data;
		return _resultImage;
	}
}