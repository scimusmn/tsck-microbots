#ifndef TOOL_IMAGE_H
#define TOOL_IMAGE_H

#include <stdint.h>

struct pixel_t {
	uint8_t r, g, b;
};

struct image_t {
	struct pixel_t * pixels;
	size_t width;
	size_t height;
};

struct image_t * load_image(char *filename);
void free_image(struct image_t *image);

/* extract a sub-region from an image as a new region.
 * the extracted image does not share memory with the original,
 * so you can free the original without consequence. */
struct image_t * extract_subimage(struct image_t *image, size_t x0, size_t y0, size_t x1, size_t y1);

/* replace a color with another image's data.
 * the image is modified in-place. */
void replace_color(struct image_t *image, struct image_t *bg, struct pixel_t color);

/* convert an image to the GCI format. */
void convert_gci(uint16_t **destination, size_t *size, struct image_t *image);
#endif
