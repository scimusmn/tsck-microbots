#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include "image/image.h"
#include "options.h"

#define MAX_ARRAY_IMAGES 256

/* structure to store GCI images */
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
	/* get command line options */
	struct options_t opts;
	if (!parse_options(&opts, argc, argv))
		return 0;

	/* initialize variables */
	struct gci_arr_t g;
	g.n_gci = 0;
	g.offset = 0;
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


/* convert a subimage into GCI and store with an appropriate label */
static void process_subimage(struct gci_arr_t *g, struct image_t *base, char *base_label, struct image_t *bg, 
                             size_t w, size_t h, int x, int y, int omit_array_label) {
	/* extract subimage */
	struct point_t p0 = { w*x, h*y };
	struct point_t p1 = p0;
	p1.x += w-1; p1.y += h-1;
	struct image_t *img = extract_subimage(base, p0, p1);
	printf("\tsubimage (%d, %d): (%lux%lu)\n", x, y, img->width, img->height);
	
	if (bg != NULL) {
		add_background(&img, bg);
	}

	/* set label */
	char *label = malloc(256*sizeof(char));
	if (omit_array_label)
		sprintf(label, "%s", base_label); /* for single images, don't include array labels */
	else
		sprintf(label, "%s_%dx%d", base_label, x, y);

	/* convert to gci */
	struct gci_t *gci = convert_gci(img);
	free_image(img);

	/* save to g */
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
	if (settings.label != NULL)
		strcpy(label, settings.label);
	else
		strcpy(label, settings.filename); /* default to filename */

	/* clean label so that it can be a valid macro identifier */
	for (char *c = label; *c != 0; c++) {
		if (!isalnum(*c))
			*c = '_';
	}

	/* load image */
	struct image_t *base = load_image(settings.filename);
	struct image_t *bg = NULL;
	if (settings.background_image != NULL)
		bg = load_image(settings.background_image);

	printf("%s: (%lux%lu)\n", settings.filename, base->width, base->height);
	
	/* compute constants */
	size_t w = base->width / settings.array_w;
	size_t h = base->height / settings.array_h;
	struct image_t *sub_bg = NULL;
	if (bg != NULL) {
		/* extract bg image of the same size as each sub-image */
		struct point_t end = settings.offset;
		end.x += w-1;
		end.y += h-1;
		sub_bg = extract_subimage(bg, settings.offset, end);
	}
	int omit_array_label = settings.array_h == 1 && settings.array_w == 1; /* omit array label if there's just one image */

	/* process sub-images */
	for (int y = 0; y<settings.array_h; y++) {
		for (int x = 0; x<settings.array_w; x++) {
			process_subimage(g, base, label, sub_bg, w, h, x, y, omit_array_label);
		}
	}

	/* clean up */
	free_image(base);
	if (bg != NULL) {
		free_image(bg);
		free_image(sub_bg);
	}
}
