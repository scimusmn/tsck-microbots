#include <string.h>
#include <stdlib.h>

#include "suites.h"
#include "lily-test.h"
#include "image.h"

static void test_extract_subimage();
static void test_combine_images();

void suite_image()
{
	lily_run_test(test_extract_subimage);
	lily_run_test(test_combine_images);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static void test_extract_subimage()
{
	struct image_t img;
	img.width = 4;
	img.height = 4;
	img.pixels = malloc(img.width * img.height * sizeof(struct rgba_t));
	lily_assert_not_null(img.pixels);

	struct point_t p0, p1;

	/* top left: white */
	img.pixels[0] = (struct rgba_t){ 255, 255, 255 };
	img.pixels[1] = (struct rgba_t){ 255, 255, 255 };
	img.pixels[4] = (struct rgba_t){ 255, 255, 255 };
	img.pixels[5] = (struct rgba_t){ 255, 255, 255 };

	/* top right: red */
	img.pixels[2] = (struct rgba_t){ 255, 0, 0 };
	img.pixels[3] = (struct rgba_t){ 255, 0, 0 };
	img.pixels[6] = (struct rgba_t){ 255, 0, 0 };
	img.pixels[7] = (struct rgba_t){ 255, 0, 0 };

	/* bottom left: green */
	img.pixels[8]  = (struct rgba_t){ 0, 255, 0 };
	img.pixels[9]  = (struct rgba_t){ 0, 255, 0 };
	img.pixels[12] = (struct rgba_t){ 0, 255, 0 };
	img.pixels[13] = (struct rgba_t){ 0, 255, 0 };

	/* bottom right: blue */
	img.pixels[10] = (struct rgba_t){ 0, 0, 255 };
	img.pixels[11] = (struct rgba_t){ 0, 0, 255 };
	img.pixels[14] = (struct rgba_t){ 0, 0, 255 };
	img.pixels[15] = (struct rgba_t){ 0, 0, 255 };

	/* extract top left */
	p0.x = 0; p0.y = 0;
	p1.x = 1; p1.y = 1;
	struct image_t *top_left = extract_subimage(&img, p0, p1); 
	lily_assert_not_null(top_left);
	lily_assert_int_equal(top_left->width, 2);
	lily_assert_int_equal(top_left->height, 2);
	lily_assert_memory_equal(&(top_left->pixels[0]), &(img.pixels[0]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(top_left->pixels[1]), &(img.pixels[0]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(top_left->pixels[2]), &(img.pixels[0]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(top_left->pixels[3]), &(img.pixels[0]), sizeof(struct rgba_t));
	free_image(top_left);

	/* extract top right */
	p0.x=2; p0.y=0;
	p1.x=3; p1.y=1;
	struct image_t *top_right = extract_subimage(&img, p0, p1);
	lily_assert_not_null(top_right);
	lily_assert_int_equal(top_right->width, 2);
	lily_assert_int_equal(top_right->height, 2);
	lily_assert_memory_equal(&(top_right->pixels[0]), &(img.pixels[2]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(top_right->pixels[1]), &(img.pixels[2]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(top_right->pixels[2]), &(img.pixels[2]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(top_right->pixels[3]), &(img.pixels[2]), sizeof(struct rgba_t));
	free_image(top_right);

	/* extract bottom left */
	p0.x=0; p0.y=2;
	p1.x=1; p1.y=3;
	struct image_t *bottom_left = extract_subimage(&img, p0, p1);
	lily_assert_not_null(bottom_left);
	lily_assert_int_equal(bottom_left->width, 2);
	lily_assert_int_equal(bottom_left->height, 2);
	lily_assert_memory_equal(&(bottom_left->pixels[0]), &(img.pixels[8]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(bottom_left->pixels[1]), &(img.pixels[8]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(bottom_left->pixels[2]), &(img.pixels[8]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(bottom_left->pixels[3]), &(img.pixels[8]), sizeof(struct rgba_t));
	free_image(bottom_left);

	/* extract bottom right */
	p0.x=2; p0.y=2;
	p1.x=3; p1.y=3;
	struct image_t *bottom_right = extract_subimage(&img, p0, p1); 
	lily_assert_not_null(bottom_right);
	lily_assert_int_equal(bottom_right->width, 2);
	lily_assert_int_equal(bottom_right->height, 2);
	lily_assert_memory_equal(&(bottom_right->pixels[0]), &(img.pixels[10]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(bottom_right->pixels[1]), &(img.pixels[10]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(bottom_right->pixels[2]), &(img.pixels[10]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(bottom_right->pixels[3]), &(img.pixels[10]), sizeof(struct rgba_t));
	free_image(bottom_right);

	/* extract center */
	p0.x=1; p0.y=1;
	p1.x=2; p1.y=2;
	struct image_t *center = extract_subimage(&img, p0, p1);
	lily_assert_not_null(center);
	lily_assert_int_equal(center->width, 2);
	lily_assert_int_equal(center->height, 2);
	lily_assert_memory_equal(&(center->pixels[0]), &(img.pixels[0]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(center->pixels[1]), &(img.pixels[2]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(center->pixels[2]), &(img.pixels[8]), sizeof(struct rgba_t));
	lily_assert_memory_equal(&(center->pixels[3]), &(img.pixels[10]), sizeof(struct rgba_t));
	free_image(center);

	free(img.pixels);
}


static struct rgba_t pixel_max(struct rgba_t a, struct rgba_t b)
{
	size_t a_combined = a.r + a.g + a.b + a.a;
	size_t b_combined = b.r + b.g + b.b + b.a;
	if (a_combined > b_combined) return a;
	return b;
}

static void test_combine_images()
{
	const size_t width = 16;
	const size_t height = 16;
	
	srand(0);
	struct image_t a, b;
	struct rgba_t px_a[width*height], px_b[width*height];
	a.width = width; a.height = height; a.pixels = px_a;
	b.width = width; b.height = height; b.pixels = px_b;

	for (int i=0; i<width*height; i++) {
		a.pixels[i] = (struct rgba_t) { rand()%256, rand()%256, rand()%256, rand()%256 };
		b.pixels[i] = (struct rgba_t) { rand()%256, rand()%256, rand()%256, rand()%256 };
	}

	struct image_t *combined = combine_images(&a, &b, pixel_max);
	lily_assert_not_null(combined);

	for (int i=0; i<width*height; i++) {
		struct rgba_t expected = pixel_max(a.pixels[i], b.pixels[i]);
		lily_assert_memory_equal(combined->pixels + i, &expected, sizeof(struct rgba_t));
	}

	free_image(combined);
}

