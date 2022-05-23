#include <stdio.h>

#include "processing/proc.h"
#include "image/image.h"

struct rgba_t alpha_blend(struct rgba_t a, struct rgba_t b)
{
	double alpha = ((double) a.a)/255;
	struct rgba_t result;
	result.r = (alpha * a.r) + ((1-alpha) * b.r);
	result.g = (alpha * a.g) + ((1-alpha) * b.g);
	result.b = (alpha * a.b) + ((1-alpha) * b.b);
	return result;
}


void add_background(struct image_t **img, struct image_t *bg)
{
	struct image_t *result = combine_images(*img, bg, alpha_blend);
	if (result != NULL) {
		free_image(*img);
		*img = result;
	}
	else
		fprintf(stderr, "failed to set image background!\n");
}
