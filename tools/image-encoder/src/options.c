#include "cargs.h"

#include "options.h"


static struct cag_option options[] = {
	{ .identifier = 'i',
	  .access_letters = "i",
	  .access_name = "image",
	  .value_name = "FILENAME",
	  .description = "Add an image to the output."
	},
	{ .identifier = 'o',
	  .access_letters = "o",
	  .access_name = "output",
	  .value_name = "FILENAME",
	  .description = "Set the binary output file. (default: output.gci)"
	},
	{ .identifier = 'c',
	  .access_letters = "c",
	  .access_name = "constants",
	  .value_name = "FILENAME",
	  .description = "Set the C constants output file. If omitted, output to stdout."
	},
	{ .identifier = 'h',
	  .access_letters = "h",
	  .access_name = "help",
	  .description = "Show help information"
	}
};

int parse_options(struct options_t *opts, int argc, char ** argv)
{
	opts->n_filenames = 0;
	opts->file_data = "output.gci";
	opts->file_constants = NULL;

	cag_option_context context;
	cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

	while(cag_option_fetch(&context)) {
		char opt = cag_option_get(&context);
		switch (opt) {
			case 'i':
				opts->filenames[opts->n_filenames] = cag_option_get_value(&context);
				opts->n_filenames += 1;
				break;

			case 'o':
				opts->file_data = cag_option_get_value(&context);
				break;

			case 'c':
				opts->file_constants = cag_option_get_value(&context);
				break;

			case 'h':
				printf("Usage: %s [OPTIONS]\n", argv[0]);
				printf("Convert images into GCI data for use on 4D Systems displays\n");
				cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
				return 0;
		}
	}
	return 1;
}

