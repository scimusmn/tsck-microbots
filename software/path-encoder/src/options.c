#include <stdio.h>
#include <string.h>

#include "options.h"

static void print_help(char *progname)
{
	printf("usage: %s [-h] INPUT_IMAGE OUTPUT_FILE\n"
	       "    -h            show this help\n"
	       "    INPUT_IMAGE   the image containing the path information. must be 128x128\n"
	       "    OUTPUT_FILE   the binary path file to output\n",
	       progname);
}

int parse_options(struct options_t *opts, int argc, char ** argv)
{
	char *progname = argv[0];
	if (argc == 1) {
		fprintf(stderr, "no input!\n");
		print_help(progname);
		return 1;
	}

	if (argc == 2 && strcmp(argv[1], "-h")) {
		print_help(progname);
		return 1;
	}

	if (argc > 3) {
		fprintf(stderr, "too many arguments!\n");
		print_help(progname);
		return 1;
	}

	opts->input_image = argv[1];
	opts->output = argv[2];
	return 0;
}
