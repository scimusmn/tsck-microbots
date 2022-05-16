#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include "image/image.h"
#include "options.h"

#define MAX_ARRAY_IMAGES 256

struct gci_arr_t {
	size_t n_gci;
	struct gci_t *array[MAX_ARRAY_IMAGES];
	size_t offset;
	size_t offset_array[MAX_ARRAY_IMAGES];
	char *label[MAX_ARRAY_IMAGES];
};


void process_image(struct gci_arr_t *g, struct image_settings_t settings);

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

	struct gci_arr_t g;
	FILE * binary = fopen(opts.file_binary, "wb");
	FILE * header;
	if (opts.file_header != NULL)
		header = fopen(opts.file_header, "w");
	else
		header = stdout;

	/* load and process all images */
	for (int i=0; i<opts.n_images; i++) {
		process_image(&g, opts.image_settings[i]);
	}

	/* output and free */
	fprintf(header, "#ifndef GCI_OFFSETS_H\n"
	                "#define GCI_OFFSETS_H\n\n");
	for (int i=0; i<g.n_gci; i++) {
		fprintf(header, "#define %s\t\t0x%lx\n", g.label[i], g.offset_array[i]);
		fwrite(g.array[i]->array, sizeof(uint8_t), g.array[i]->size/sizeof(uint8_t), binary);
		free_gci(g.array[i]);
		free(g.label[i]);
	}
	fprintf(header, "\n#endif");
	fclose(binary);
	if (header != stdout)
		fclose(header);

	return 0;
}


static struct rgba_t alpha_blend(struct rgba_t a, struct rgba_t b)
{
	struct rgba_t result;
	result.r = (a.a*a.r) + ((255-a.a)*b.r);
	result.g = (a.a*a.g) + ((255-a.a)*b.g);
	result.b = (a.a*a.b) + ((255-a.a)*b.b);
	result.a = 255;
	return result;
}

static void process_subimage(struct gci_arr_t *g, char *base_label, struct image_t *img, struct image_t *bg, 
                             size_t w, size_t h, int x, int y) {
	if (bg != NULL) {
		struct image_t *tmp = combine_images(img, bg, alpha_blend);
		free_image(img);
		img = tmp;
	}

	char *label = malloc(256*sizeof(char));
	sprintf(label, "%s%dx%d", base_label, x, y);

	struct gci_t *gci = convert_gci(img);
	free_image(img);
	int index = g->n_gci;
	g->array[index] = gci;
	g->offset_array[index] = g->offset;
	g->label[index] = label;
	g->offset += gci->size;
	g->n_gci += 1;
}

void process_image(struct gci_arr_t *g, struct image_settings_t settings)
{
	/* create base label */
	char label[256];
	strcpy(label, settings.filename);
	for (char *c = label; *c != 0; c++) {
		if (!isalnum(*c))
			*c = '_';
	}
	printf("%s\n", label);

	/* load image */
	struct image_t *base = load_image(settings.filename);
	struct image_t *bg = NULL;
	if (settings.background_image != NULL)
		bg = load_image(settings.background_image);
	
	/* process sub-images */
	size_t w = base->width / settings.array_w;
	size_t h = base->height / settings.array_h;
	struct image_t *sub_bg = NULL;
	if (bg != NULL) {
		struct point_t end = settings.offset;
		end.x += w-1;
		end.y += h-1;
		sub_bg = extract_subimage(bg, settings.offset, end);
	}

	for (int y = 0; y<settings.array_h; y++) {
		for (int x = 0; x<settings.array_w; x++) {
			struct point_t p0 = { w*x, h*y };
			struct point_t p1 = p0;
			p1.x += w-1; p1.y += h-1;

			struct image_t *img = extract_subimage(base, p0, p1);
			process_subimage(g, label, img, sub_bg, w, h, x, y);
		}
	}

	free_image(base);
	if (bg != NULL) {
		free_image(bg);
		free_image(sub_bg);
	}
}
