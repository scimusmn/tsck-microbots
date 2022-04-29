#ifndef IMG_OPTIONS_H
#define IMG_OPTIONS_H

#ifndef MAX_INPUT_IMAGES
#define MAX_INPUT_IMAGES 128
#endif

struct options_t {
	/* input files */
	char * filenames[MAX_INPUT_IMAGES];
	size_t n_filenames;

	/* output files */
	char * file_data;
	char * file_constants;
};

int parse_options(struct options_t *opts, int argc, char ** argv);

#endif
