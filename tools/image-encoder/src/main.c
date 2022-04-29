#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "options.h"

int main(int argc, char ** argv)
{
	struct options_t opts;
	if (!parse_options(&opts, argc, argv))
		return 0;

	printf("files:\n");
	for (int i=0; i<opts.n_filenames; i++) {
		printf("  %s\n", opts.filenames[i]);
	}
	printf("data output: %s\n", opts.file_data);
	printf("constants output: %s\n", opts.file_constants);

	return 0;
}
