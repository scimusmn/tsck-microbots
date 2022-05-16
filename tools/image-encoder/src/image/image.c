#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"
#include "image/image.h"

struct image_t * load_image(char *filename)
{
	/* load image from file */
	int width, height, channels;
	struct rgba_t *pixels = (struct rgba_t *) stbi_load(filename, &width, &height, &channels, 4);	

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

struct image_t * extract_subimage(struct image_t *image, struct point_t p0, struct point_t p1)
{
	if (p1.x < p0.x || p1.y < p0.y ||
	    out_of_range(image, p0.x, p0.y) || out_of_range(image, p1.x, p1.y)) {
		fprintf(stderr, "ERROR: attempted to extract subimage with invalid range (%lu,%lu) - (%lu, %lu)\n",
			p0.x, p0.y, p1.x, p1.y);
		return NULL;	
	}

	struct image_t *sub_image = malloc(sizeof(struct image_t));
	if (sub_image == NULL) {
		fprintf(stderr, "ERROR: failed to allocate memory for sub-image!\n");
		return NULL;
	}

	sub_image->width = p1.x - p0.x + 1;
	sub_image->height = p1.y - p0.y + 1;
	sub_image->pixels = malloc(sub_image->width * sub_image->height * sizeof(struct rgba_t));
	if (sub_image->pixels == NULL) {
		fprintf(stderr, "ERROR: failed to allocate memory for sub-image pixel data!\n");
		free(sub_image);
		return NULL;
	}

	for (size_t y = p0.y; y<=p1.y; y++) {
		for (size_t x = p0.x; x<=p1.x; x++) {
			int source_index = get_index(image, x, y);
			int dest_index = get_index(sub_image, x-p0.x, y-p0.y);
			sub_image->pixels[dest_index] = image->pixels[source_index];
		}
	}

	return sub_image;
}


struct image_t * combine_images(struct image_t *a, struct image_t *b, pixel_operation_t op)
{
	if (a->width != b->width || a->height != b->height) {
		fprintf(stderr, "ERROR: images are different sizes! (%lu, %lu) != (%lu, %lu)\n",
		        a->width, a->height, b->width, b->height);
		return NULL;
	}

	size_t n_pixels = a->width * b->width;
	struct image_t *output = malloc(sizeof(struct image_t));
	if (output == NULL) {
		fprintf(stderr, "ERROR: failed to allocate memory for image struct!\n");
		return NULL;
	}
	output->pixels = malloc(n_pixels * sizeof(struct rgba_t));
	if (output->pixels == NULL) {
		fprintf(stderr, "ERROR: failed to allocate memory for pixel array with %lu elements!\n", n_pixels);
		free(output);
		return NULL;
	}
	output->width = a->width;
	output->height = a->height;

	for (int i=0; i<a->width * a->height; i++) {
		output->pixels[i] = op(a->pixels[i], b->pixels[i]);
	}
	
	return output;
}


static uint8_t hi(uint16_t n) { return (n & 0xff00) >> 8; }
static uint8_t lo(uint16_t n) { return n & 0x00ff; }

struct gci_t * convert_gci(struct image_t *image)
{
	/* useful size constants */
	const size_t header_size = 6 * sizeof(uint8_t);
	const size_t rgb565_size = 2 * sizeof(uint8_t);
	const size_t n_pixels = image->width * image->height;
	const size_t array_size = n_pixels * rgb565_size;

	/* allocate output */
	struct gci_t *output = malloc(sizeof(struct gci_t));
	if (output == NULL) {
		fprintf(stderr, "ERROR: failed to allocate memory for gci struct!\n");
		return NULL;
	}
	output->array = malloc(header_size + array_size);
	if (output->array == NULL) {
		fprintf(stderr, "ERROR: failed to allocate memory for GCI array with %lu elements!\n", header_size + array_size);
		free(output);
		return NULL;
	}

	/* set up header */
	output->size = header_size + array_size;
	output->array[0] = hi(image->width);
	output->array[1] = lo(image->width);
	output->array[2] = hi(image->height);
	output->array[3] = lo(image->height);
	output->array[4] = 0x10;
	output->array[5] = 0x00;

	/* do rgb565 conversion */
	for (int i=0; i<n_pixels; i++) {
		struct rgba_t pixel = image->pixels[i];
		uint16_t px = 0;
		px |= (pixel.r >> 3) << 11; /* red in first 5 bits */
		px |= (pixel.g >> 2) << 5;  /* green in middle 6 bits */
		px |= (pixel.b >> 3);       /* blue in final 5 bits */	
		int index = 2*i + 6;
		output->array[index]   = hi(px);
		output->array[index+1] = lo(px);
	}

	return output;
}


void free_gci(struct gci_t *gci)
{
	free(gci->array);
	free(gci);
}
