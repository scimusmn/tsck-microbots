#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include "image/image.h"
#include "options.h"

/* print binary bytes */
#define BYTE_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

struct point {
	uint8_t x, y;
	int visited;
};

/* get all points in the path.
 *
 * the first point is the start, the next (n-2) points are
 * the path, and the final point is the end.
 *
 * we return the number of points in the path.
 */
int get_path_points(struct point *path, struct image_t *img);

/* get the path obstruction raster */
void get_obstructions(uint8_t *obstructions, struct image_t *img);


int main(int argc, char ** argv)
{
	/* get command line options */
	struct options_t opts;
	if (parse_options(&opts, argc, argv) != 0)
		return 0;

	/* load image */
	struct image_t *img = load_image(opts.input_image);
	if (img->width != 128 || img->height != 128) {
		fprintf(stderr, "input image must be 128x128! (got %lux%lu instead)\n",
		        img->width, img->height);
		return 1;
	}

	/* get path data */
	struct point path[128];
	int n_points = get_path_points(path, img);

	/* get obstructions */
	uint8_t obstruction[2048];
	get_obstructions(obstruction, img);

	/* print path data for user verification */
	for (int i=0; i<n_points; i++) {
		printf("(%d, %d) -> ", path[i].x, path[i].y);
	}
	printf("\n");

	/* output to file */
	FILE *output = fopen(opts.output, "wb");

	fputc(n_points, output); /* start with number of points */
	for (int i=0; i<n_points; i++) {
		/* output points in X, Y format */
		fputc(path[i].x, output);
		fputc(path[i].y, output);
	}

	/* output path data */
	for (int y=0; y<128; y++) {
		for (int i=0; i<16; i++) {
			fputc(obstruction[16*y + i], output);
		}
	}

	/* clean up */
	fclose(output);
	free_image(img);
	return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static int test_color(struct rgba_t px, int r, int g, int b)
{
	return (px.r == r) && (px.g == g) && (px.b == b);
}

static float dist(struct point a, struct point b)
{
	int dx = ((int)a.x) - ((int)b.x);
	int dy = ((int)a.y) - ((int)b.y);
	float result = (dx*dx) + (dy*dy);
	return result;
}

static int get_closest(struct point pt, struct point *points, int n)
{
	float min_dist = 1000;
	int closest;
	
	for (int i=0; i<n; i++) {
		/* ignore used points */
		if (points[i].visited) continue;
		
		float d = dist(pt, points[i]);
		if (d < min_dist) {
			min_dist = d;
			closest = i;
		}
	}
	return closest; 
}

int get_path_points(struct point *path, struct image_t *img)
{
	struct point start, end, points[128];
	int n_points = 0;

	/* scan image for points */
	for (int x=0; x<img->width; x++) {
		for (int y=0; y<img->height; y++) {
			struct rgba_t px = img->pixels[x + (img->width*y)];
			if (test_color(px, 255, 0, 0)) {
				/* start point */
				start.x = x; start.y = y;
			}
			else if (test_color(px, 0, 0, 255)) {
				/* end point */
				end.x = x; end.y = y;
			}
			else if (test_color(px, 255, 0, 255)) {
				/* path point */
				points[n_points].x = x; points[n_points].y = y;
				points[n_points].visited = 0;
				n_points += 1;
			}
		}
	}

	/* get points in order */
	path[0] = start;
	int n_path = 1;
	for (int i=0; i<n_points; i++) {
		struct point current = path[n_path-1];
		int closest = get_closest(current, points, n_points);
		path[n_path] = points[closest];
		points[closest].visited = 1;
		n_path += 1;
	}
	path[n_path] = end;
	n_path += 1;

	return n_path;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static void set_bit(uint8_t *o, int x, int y, int state) {
	int row_offset = 16 * y; /* 16 bytes is 128 bits */
	int col_offset = x / 8; /* get byte */
	int bit_offset = 7 - (x % 8); /* get bit */
	int bit_mask = state ? 1 : 0;
	bit_mask = bit_mask << bit_offset;

	o[row_offset + col_offset] |= bit_mask;
}

void get_obstructions(uint8_t *obstructions, struct image_t *img)
{
	memset(obstructions, 0, 2048);
	for (int x=0; x<128; x++) {
		for (int y=0; y<128; y++) {
			struct rgba_t px = img->pixels[128*y + x];
			if (test_color(px, 0, 0, 0))
				/* true == there is an obstruction */
				set_bit(obstructions, x, y, 1);
			else
				/* false == no obstruction */
				set_bit(obstructions, x, y, 0);
		}
	}
}
