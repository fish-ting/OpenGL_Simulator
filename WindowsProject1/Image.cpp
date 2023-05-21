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
}