#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "image.h"

struct image_t * load_image(char *filename)
{
	/* load image from file */
	int width, height, channels;
	struct pixel_t *pixels = (struct pixel_t *) stbi_load(filename, &width, &height, &channels, 3);	

	if (pixels == NULL) {
		fprintf(stderr, "ERROR: failed to load image data from '%s'\n", filename);
		return NULL;
	}

	struct image_t *image = malloc(sizeof(struct image_t));
	if (image == NULL) {
		fprintf(stderr, "ERROR: failed to allocated %lu bytes of memory to store image data!\n", sizeof(struct image_t));
		stbi_image_free((unsigned char *)pixels);
		return NULL;
	}
	image->pixels = pixels;
	image->width = width;
	image->height = height;

	return image;
}


void free_image(struct image_t *image)
{
	free(image->pixels);
	free(image);
}


static int get_index(struct image_t *image, size_t x, size_t y)
{
	return (image->width * y) + x;
}

static int out_of_range(struct image_t *image, size_t x, size_t y) {
	if (x > image->width || y > image->height)
		return 1;
	return 0;
}

struct image_t * extract_subimage(struct image_t *image, size_t x0, size_t y0, size_t x1, size_t y1)
{
	if (x1 < x0 || y1 < y0 ||
	    out_of_range(image, x0, y0) || out_of_range(image, x1, y1)) {
		fprintf(stderr, "ERROR: attempted to extract subimage with invalid range (%lu,%lu) - (%lu, %lu)\n",
			x0, y0, x1, y1);
		return NULL;	
	}

	struct image_t *sub_image = malloc(sizeof(struct image_t));
	if (sub_image == NULL) {
		fprintf(stderr, "ERROR: failed to allocate memory for sub-image!\n");
		return NULL;
	}

	sub_image->width = x1 - x0 + 1;
	sub_image->height = y1 - y0 + 1;
	sub_image->pixels = malloc(sub_image->width * sub_image->height * sizeof(struct pixel_t));
	if (sub_image->pixels == NULL) {
		fprintf(stderr, "ERROR: failed to allocate memory for sub-image pixel data!\n");
		free(sub_image);
		return NULL;
	}

	for (size_t y = y0; y<=y1; y++) {
		for (size_t x = x0; x<=x1; x++) {
			int source_index = get_index(image, x, y);
			int dest_index = get_index(sub_image, x-x0, y-y0);
			sub_image->pixels[dest_index] = image->pixels[source_index];
		}
	}

	return sub_image;
}


void replace_color(struct image_t *image, struct image_t *bg, struct pixel_t color)
{
	for (int i=0; i<(image->width * image->height); i++) {
		if (image->pixels[i].r == color.r &&
		    image->pixels[i].g == color.g &&
		    image->pixels[i].b == color.b) {
			image->pixels[i] = bg->pixels[i];
		}
	}
}


static uint16_t big_endian(uint16_t n)
{
	static int is_little_endian = -1;
	if (is_little_endian == -1) {
		int test = 0x01;	
		uint8_t *ptr = (uint8_t *) &test;
		if (*ptr == 1) {
			/* system is little endian */
			is_little_endian = 1;
		}
		else {
			/* system is big endian */
			is_little_endian = 0;
		}
	}

	if (!is_little_endian)
		return n;

	uint16_t swapped = 0;
	swapped |= (n & 0xff00) >> 8;
	swapped |= (n & 0x00ff) << 8;
	return swapped;
}

void convert_gci(uint16_t **destination, size_t *size, struct image_t *image)
{
	/* allocate buffer for GCI data */
	size_t header_size = 3 * sizeof(uint16_t);
	size_t pixel_buffer = 2 * image->width * image->height;
	*size = header_size + pixel_buffer;
	uint16_t *gci = malloc(*size);

	if (gci == NULL) {
		fprintf(stderr,
		        "WARNING: failed to allocate %lu bytes of memory!", header_size + pixel_buffer);
		*destination = NULL;
		return;
	}

	/* create gci header */
	gci[0] = big_endian(image->width);
	gci[1] = big_endian(image->height);
	gci[2] = big_endian(0x1000);

	/* convert pixels */
	for (int i=0; i<image->width*image->height; i++) {
		struct pixel_t pixel = image->pixels[i];
		uint16_t p565 = 0;
		p565 |= (pixel.r >> 3) << 11; /* red in first 5 bits */
		p565 |= (pixel.g >> 2) << 5;  /* green in middle 6 bits */
		p565 |= (pixel.b >> 3);       /* blue in final 5 bits */
		gci[3+i] = big_endian(p565);
	}

	*destination = gci;
}

