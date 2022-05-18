#ifndef PROCESSING_H
#define PROCESSING_H

#include "image/image.h"

struct rgba_t alpha_blend(struct rgba_t a, struct rgba_t b);
void add_background(struct image_t **img, struct image_t *bg);

#endif
