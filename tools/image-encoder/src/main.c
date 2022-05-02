#include <stdio.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image.h"
#include "options.h"

#define MAX_ARRAY_IMAGES 256

int main(int argc, char ** argv)
{
	struct options_t opts;
	if (!parse_options(&opts, argc, argv))
		return 0;

	/*size_t size;
	uint16_t *buffer;

	FILE * output = fopen(opts.file_data, "wb");
	for (int i=0; i<opts.n_filenames; i++) {
		buffer = convert_image(opts.filenames[i], &size);
		if (buffer == NULL) continue;
		fwrite(buffer, sizeof(uint16_t), size/sizeof(uint16_t), output);
		free(buffer);
	}
	fclose(output);*/


	size_t size;
	uint16_t *buffer;

	for (int i=0; i<opts.n_images; i++) {
		struct image_settings_t *img = opts.image_settings + i;

	}

	return 0;
}

void process_array(struct image_settings_t *img_settings)
{
	size_t n_images;
	struct image_t images[MAX_ARRAY_IMAGES];

	struct image_t *img = load_image(img_settings->filename);
	size_t w = img->width / img_settings->array_w;
	size_t h = img->height / img_settings->array_h;

	/* check for bg */
	struct image_t *bg = NULL;
	if (img_settings->background_image != NULL) {
		struct image_t *bg_full = load_image(img_settings->background_image);
		if (bg_full->width != w || bg_full->height != h) {
			/* extract sub-image */
			size_t x0 = img_settings->bg_x;
			size_t y0 = img_settings->bg_y;
			size_t x1 = x0 + w;
			size_t y1 = y0 + h;
			bg = extract_subimage(bg_full, x0, y0, x1, y1);
			free_image(bg_full);
		}
		else {
			bg = bg_full;
		}
	}

	for (int y=0; y<img_settings->array_h; y++) {
		for (int x=0; x<img_settngs->array_w; x++) {
			size_t x0 = x*w;
			size_t y0 = y*h;
			struct image_t *sub = extract_subimage(img, x0, y0, x0+w, y0+h);

