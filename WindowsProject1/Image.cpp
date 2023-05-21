#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GT
{
	// ���ܺ�������ͼƬ·������ָ��ͼƬ
	Image* Image::readFromFile(const char* _fileName)
	{
		int m_picType = 0;
		int m_width = 0;
		int m_height = 0;

		// stbImage�����ͼƬ�Ƿ�������
		stbi_set_flip_vertically_on_load(true);
		// ��ȡ
		unsigned char* bits = stbi_load(_fileName, &m_width, &m_height, &m_picType, STBI_rgb_alpha);
		
		// ͼƬ��ʾ��r �� b ͨ��Ҫ������
		for (int i = 0; i < m_width * m_height * 4; i+=4)
		{
			byte tmp = bits[i];
			bits[i] = bits[i + 2];
			bits[i + 2] = tmp;
		}
		
		Image* m_image = new Image(m_width, m_height, bits);
		// �ͷ�
		stbi_image_free(bits);
		return m_image;
	}
}