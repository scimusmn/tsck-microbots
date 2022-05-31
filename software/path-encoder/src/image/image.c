#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"
#include "image/image.h"

struct image_t * allocate_image(size_t width, size_t height)
{
	struct image_t *img = malloc(sizeof(struct image_t));
	if (img == NULL) {
		fprintf(stderr, 
		        "ERROR: failed to allocate memory for struct image_t (%lu bytes)\n", 
		        sizeof(struct image_t));
		return NULL;
	}

	img->pixels = malloc(width * height * sizeof(struct rgba_t));
	if (img->pixels == NULL) {
		fprintf(stderr,
		        "ERROR: failed to allocate memory for %lu pixels (%lu bytes)\n",
				width * height, width * height * sizeof(struct rgba_t));
		free(img);
		return NULL;
	}

	img->width = width;
	img->height = height;

	return img;
}


struct image_t * load_image(char *filename)
{
	/* load image from file */
	int width, height, channels;
	struct rgba_t *pixels = (struct rgba_t *) stbi_load(filename, &width, &height, &channels, 4);	

	if (pixels == NULL) {
		fprintf(stderr, "ERROR: failed to load image data from '%s'\n", filename);
		return NULL;
	}

	/* copy pixel data to struct image_t */
	struct image_t *img = allocate_image(width, height);
	if (img == NULL)
		return NULL;
	memcpy(img->pixels, pixels, width * height * sizeof(struct rgba_t));
	free(pixels);

	return img;
}


void free_image(struct image_t *img)
{
	free(img->pixels);
	free(img);
}


static int get_index(struct image_t *image, size_t x, size_t y)
{
	return (image->width * y) + x;
}

/* check if a given point is outside the image */
static int out_of_range(struct image_t *image, struct point_t p) {
	if (p.x > image->width || p.y > image->height)
		return 1;
	return 0;
}

struct image_t * extract_subimage(struct image_t *image, struct point_t p0, struct point_t p1)
{
	/* check if indices are out of range */
	if (p1.x < p0.x || p1.y < p0.y ||
	    out_of_range(image, p0) || out_of_range(image, p1)) {
		fprintf(stderr, "ERROR: attempted to extract subimage with invalid range (%lu,%lu) - (%lu, %lu)\n",
			p0.x, p0.y, p1.x, p1.y);
		return NULL;	
	}

	/* allocate memory for extracted image */
	size_t width = p1.x - p0.x + 1;
	size_t height = p1.y - p0.y + 1;
	struct image_t *sub_image = allocate_image(width, height);
	if (sub_image == NULL)
		return NULL;

	/* copy data from source */
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
	/* check that images have the same dimensions */
	if (a->width != b->width || a->height != b->height) {
		fprintf(stderr, "ERROR: images are different sizes! (%lu, %lu) != (%lu, %lu)\n",
		        a->width, a->height, b->width, b->height);
		return NULL;
	}

	/* allocate memory for result */
	struct image_t *output = allocate_image(a->width, a->height);
	if (output == NULL)
		return NULL;

	/* compute combination */
	size_t n_pixels = a->width * a->height;
	for (size_t i=0; i<n_pixels; i++) {
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


struct image_t * gci_to_image(struct gci_t *gci)
{
	uint8_t *array = gci->array;
	size_t width = (array[0] << 8) | array[1];
	size_t height = (array[2] << 8) | array[3];
	struct image_t *img = allocate_image(width, height);

	for (int i=0; i<width*height; i++) {
		int index = 2*i+6;
		img->pixels[i].r = array[index] & 0xf8;
		img->pixels[i].g = ((array[index] & 0x07) << 5) | ((array[index+1] & 0xe0) >> 3);
		img->pixels[i].b = (array[index+1] & 0x1f) << 3;
		img->pixels[i].a = 255;
	}

	return img;
}


void free_gci(struct gci_t *gci)
{
	free(gci->array);
	free(gci);
}
