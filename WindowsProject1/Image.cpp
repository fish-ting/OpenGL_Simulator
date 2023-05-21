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

	// 功能函数：处理图片缩放
	Image* Image::zoomImage(const Image* _image, float _zoomX, float _zoomY)
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
}