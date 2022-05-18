#include <string.h>

#include "test/suites.h"
#include "test/lily-test.h"
#include "processing/proc.h"

static void test_alpha_blend();
static void test_add_background();

void suite_processing()
{
	lily_run_test(test_alpha_blend);
	lily_run_test(test_add_background);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static void check_rgb(struct rgba_t rgba, uint8_t r, uint8_t g, uint8_t b)
{
	lily_assert_int_equal(rgba.r, r);
	lily_assert_int_equal(rgba.g, g);
	lily_assert_int_equal(rgba.b, b);
}

static void test_alpha_blend()
{
	struct rgba_t a, b, result;

	a = (struct rgba_t) { 255, 255, 255, 255 };
	b = (struct rgba_t) { 0, 0, 0, 255 };
	result = alpha_blend(a, b);
	check_rgb(result, 255, 255, 255);

	a.a = 0;
	result = alpha_blend(a, b);
	check_rgb(result, 0, 0, 0);

	a.a = 127;
	result = alpha_blend(a, b);
	check_rgb(result, 127, 127, 127);

	a.g = 0; b.g = 255;
	result = alpha_blend(a, b);
	check_rgb(result, 127, 128, 127);
}


static void test_add_background()
{
	struct image_t *img = allocate_image(64, 64);
	struct image_t *bg = allocate_image(64, 64);

	/* set bg to solid white */
	memset(bg->pixels, 255, 64*64*sizeof(struct rgba_t));

	/* set fg to half-transparent red, fully solid on the diagonals */
	for (int x=0; x<64; x++) {
		for (int y=0; y<64; y++) {
			int index = x + (64*y);
			if (x == y)
				img->pixels[index] = (struct rgba_t) { 255, 0, 0, 255 };
			else
				img->pixels[index] = (struct rgba_t) { 255, 0, 0, 127 };
		}
	}

	add_background(&img, bg);
	for (int x=0; x<64; x++) {
		for (int y=0; y<64; y++) {
			int index = x + (64*y);
			if (x == y)
				check_rgb(img->pixels[index], 255, 0, 0);
			else
				check_rgb(img->pixels[index], 255, 128, 128);
		}
	}

	free_image(img);
	free_image(bg);
}

