#ifndef IMG_OPTIONS_H
#define IMG_OPTIONS_H

#ifndef MAX_INPUT_IMAGES
#define MAX_INPUT_IMAGES 128
#endif

#include "image.h"

struct image_settings_t {
	const char * filename;

	size_t array_w, array_h;

	const char * background_image;
	size_t bg_x, bg_y;
	struct pixel_t color;
};

struct options_t {
	/* input files */
	struct image_settings_t image_settings[MAX_INPUT_IMAGES];
	size_t n_images;

	/* output files */
	const char * file_binary;
	const char * file_header;
};

int parse_options(struct options_t *opts, int argc, char ** argv);

#endif
