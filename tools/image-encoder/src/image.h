#ifndef TOOL_IMAGE_H
#define TOOL_IMAGE_H

#include <stdint.h>

struct point_t {
	size_t x, y;
};

struct rgba_t {
	uint8_t r, g, b, a;
};

struct image_t {
	struct rgba_t * pixels;
	size_t width;
	size_t height;
};

struct gci_t {
	uint16_t *array;
	size_t size;
};

typedef struct rgba_t (*pixel_operation_t)(struct rgba_t, struct rgba_t);


struct image_t * load_image(char *filename);
void free_image(struct image_t *image);

/* extract a sub-region from an image as a new region.
 * the extracted image does not share memory with the original,
 * so you can free the original without consequence. */
struct image_t * extract_subimage(struct image_t *image, struct point_t p0, struct point_t p1);

/* combine images together
 * the function pointer accepts two pixels and returns a new pixel to be put into the output. 
 * the output is a new image and must be freed as normal. the originals are not modified. */
 struct image_t * combine_images(struct image_t *a, struct image_t *b, pixel_operation_t op);

/* convert an image to the GCI format. */
struct gci_t * convert_gci(struct image_t *image);
void free_gci(struct gci_t *gci);

#endif
