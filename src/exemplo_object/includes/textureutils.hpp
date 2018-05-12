#pragma once

#include <lodepng.h>
#include <vector>
#include <iostream>

namespace textureutils {
	std::vector<unsigned char> getTextureFromImage(const char *path, size_t &s, size_t &t){
	  // Load file and decode image.
		std::vector<unsigned char> image;
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, path);
		
		if(error != 0)
		{
			throw string("error: texture can not be loaded!!! ");
		}
		

	  // Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	  size_t u2 = 1; while(u2 < width) u2 *= 2;

	  size_t v2 = 1; while(v2 < height) v2 *= 2;

	  // Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	  //GLfloat u3 = (GLfloat)width / u2;

	  //GLfloat v3 = (GLfloat)height / v2;

	  	// Make power of two version of the image.
		std::vector<unsigned char> image2(u2 * v2 * 4);

		for(size_t y = 0; y < height; y++) {
			for(size_t x = 0; x < width; x++) {
				for(size_t c = 0; c < 4; c++)
				{
					image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];

				}
			}
		}

		s = u2;
		t = v2;
		return image2;
	}
}
