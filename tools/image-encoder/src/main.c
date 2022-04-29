#include <stdio.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "options.h"

uint16_t * convert_image(char * filename, size_t *size);

int main(int argc, char ** argv)
{
	struct options_t opts;
	if (!parse_options(&opts, argc, argv))
		return 0;

	size_t size;
	uint16_t *buffer;

	FILE * output = fopen(opts.file_data, "wb");
	for (int i=0; i<opts.n_filenames; i++) {
		buffer = convert_image(opts.filenames[i], &size);
		if (buffer == NULL) continue;
		fwrite(buffer, sizeof(uint16_t), size/sizeof(uint16_t), output);
		free(buffer);
	}
	fclose(output);

	return 0;
}

struct pixel_t {
	unsigned char r, g, b;
};

uint16_t * convert_image(char * filename, size_t *size)
{
	/* load image from file */
	int width, height, channels;
	struct pixel_t *pixels = (struct pixel_t *) stbi_load(filename, &width, &height, &channels, 3);	

	if (pixels == NULL) {
		fprintf(stderr, "WARNING: could not load file '%s'. It will be excluded from the output.\n", filename);
		return NULL;
	}

	/* allocate buffer for GCI data */
	size_t header_size = 3 * sizeof(uint16_t);
	size_t pixel_buffer = 2 * width * height;
	*size = header_size + pixel_buffer;
	uint16_t *gci = malloc(*size);

	if (gci == NULL) {
		fprintf(stderr,
		        "WARNING: failed to allocate %lu bytes of memory! Image '%s' will not be processed",
		        header_size + pixel_buffer, filename);
		stbi_image_free((unsigned char *)pixels);
		return NULL;
	}

	/* create gci header */
	gci[0] = width;
	gci[1] = height;
	gci[2] = 0x1000;

	/* convert pixels */
	for (int i=0; i<width*height; i++) {
		struct pixel_t pixel = pixels[i];
		uint16_t p565 = 0;
		p565 |= (pixel.r >> 3) << 11; /* red in first 5 bits */
		p565 |= (pixel.g >> 2) << 5;  /* green in middle 6 bits */
		p565 |= (pixel.b >> 3);       /* blue in final 5 bits */
		gci[3+i] = p565;
	}

	/* clean up */
	stbi_image_free((unsigned char *)pixels);
	return gci;
}
