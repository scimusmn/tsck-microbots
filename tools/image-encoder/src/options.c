#include "cargs/cargs.h"

#include "options.h"

#define OPT_IMAGE 'i'
#define OPT_ARRAY 'a'
#define OPT_BACKGROUND 'b'
#define OPT_OFFSET 'f'
#define OPT_COLOR 'c'

#define OPT_BINARY_OUTPUT 'o'
#define OPT_HEADER_OUTPUT 'r'

#define OPT_HELP 'h'


static struct cag_option options[] = {
	{
		.identifier = OPT_IMAGE,
		.access_letters = "i",
		.access_name = "image",
		.value_name = "FILENAME",
		.description = "Add an image to the outputs."
	},
	{
		.identifier = OPT_ARRAY,
		.access_letters = NULL,
		.access_name = "array",
		.value_name = "WIDTHxHEIGHT",
		.description = "Set the next image to be an array of WIDTH * HEIGHT sub-images",
	},
	{
		.identifier = OPT_BACKGROUND,
		.access_letters = NULL,
		.access_name = "background",
		.value_name = "FILENAME",
		.description = "Set the next image to use FILENAME as a background image."
	},
	{
		.identifier = OPT_OFFSET,
		.access_letters = NULL,
		.access_name = "offset",
		.value_name = "XxY",
		.description = "When replacing background pixels, start at (X, Y) in the background image rather than (0, 0).",
	},
	{
		.identifier = OPT_BINARY_OUTPUT,
		.access_letters = NULL,
		.access_name = "gci",
		.value_name = "FILENAME",
		.description = "Set the binary output file. (default: output.gci)"
	},
	{
		.identifier = OPT_HEADER_OUTPUT,
		.access_letters = NULL,
		.access_name = "header",
		.value_name = "FILENAME",
		.description = "Set the C constants output file. If omitted, output to stdout."
	},
	{
		.identifier = 'h',
		.access_letters = "h",
		.access_name = "help",
		.description = "Show help information"
	}
};


static void setup_image(struct image_settings_t *img)
{
	img->filename = NULL;
	img->array_w = 1;
	img->array_h = 1;

	img->background_image = NULL;
	img->offset.x = 0;
	img->offset.y = 0;
}

int parse_options(struct options_t *opts, int argc, char ** argv)
{
	opts->n_images = 0;
	opts->file_binary = "images.gci";
	opts->file_header = NULL;

	/* configure first image */
	setup_image(opts->image_settings);

	cag_option_context context;
	cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

	while(cag_option_fetch(&context)) {
		char opt = cag_option_get(&context);
		struct image_settings_t *current_image = opts->image_settings + opts->n_images;

		switch (opt) {
			case OPT_IMAGE:
				/* finalize current image */
				current_image->filename = cag_option_get_value(&context);
				opts->n_images += 1;

				/* setup next image */
				setup_image(current_image + 1);
				break;

			case OPT_ARRAY:
				do {
					const char *arg = cag_option_get_value(&context);
					int w, h;
					int result = sscanf(arg, "%dx%d", &w, &h);
					if (result != 2) {
						fprintf(stderr, "ERROR: bad array format '%s'\n", arg);
						return 0;
					}
					current_image->array_w = w;
					current_image->array_h = h;
				} while(0);
				break;
				
			case OPT_BACKGROUND:
				current_image->background_image = cag_option_get_value(&context);
				break;

			case OPT_OFFSET:
				do {
					const char *arg = cag_option_get_value(&context);
					int x, y;
					int result = sscanf(arg, "%dx%d", &x, &y);
					if (result != 2) {
						fprintf(stderr, "ERROR: bad offset format '%s'\n", arg);
						return 0;
					}
					current_image->offset.x = x;
					current_image->offset.y = y;
				} while(0);
				break;

			case OPT_BINARY_OUTPUT:
				opts->file_binary = cag_option_get_value(&context);
				break;

			case OPT_HEADER_OUTPUT:
				opts->file_header = cag_option_get_value(&context);
				break;

			case OPT_HELP:
				printf("Usage: %s [OPTIONS]\n", argv[0]);
				printf("Convert images into GCI data for use on 4D Systems displays\n");
				cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
				return 0;
		}
	}
	return 1;
}

