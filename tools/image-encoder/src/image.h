#ifndef TOOL_IMAGE_H
#define TOOL_IMAGE_H

#include <stdint.h>

struct rgb_t {
	uint8_t r, g, b;
};

struct rgba_t {
	uint8_t r, g, b, a;
};

struct image_t {
	struct rgba_t * pixels;
	size_t width;
	size_t height;
};

struct image_t * load_image(char *filename);
void free_image(struct image_t *image);
struct image_alpha_t * load_image_alpha(char *filename);
void free_image_alpha(struct image_alpha_t *image);

/* extract a sub-region from an image as a new region.
 * the extracted image does not share memory with the original,
 * so you can free the original without consequence. */
struct image_t * extract_subimage(struct image_t *image, size_t x0, size_t y0, size_t x1, size_t y1);

/* alpha blend with another image's data.
 * alpha data from the background is ignored.
 * the image is modified in-place. */
void add_background(struct image_t *image, struct image_t *bg);

/* convert an image to the GCI format. */
void convert_gci(uint16_t **destination, size_t *size, struct image_t *image);
#endif
