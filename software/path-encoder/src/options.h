#ifndef IMG_OPTIONS_H
#define IMG_OPTIONS_H

#include "image/image.h"

struct options_t {
	const char * input_image;
	const char * output;
};

int parse_options(struct options_t *opts, int argc, char ** argv);

#endif
