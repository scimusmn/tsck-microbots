#include <string.h>
#include <stdlib.h>

#include "suites.h"
#include "lily-test.h"
#include "image.h"

static void test_extract_subimage();

void suite_image()
{
	lily_run_test(test_extract_subimage);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static void test_extract_subimage()
{
	struct image_t img;
	img.width = 4;
	img.height = 4;
	img.pixels = malloc(img.width * img.height * sizeof(struct pixel_t));
	lily_assert_not_null(img.pixels);

	/* top left: white */
	img.pixels[0] = (struct pixel_t){ 255, 255, 255 };
	img.pixels[1] = (struct pixel_t){ 255, 255, 255 };
	img.pixels[4] = (struct pixel_t){ 255, 255, 255 };
	img.pixels[5] = (struct pixel_t){ 255, 255, 255 };

	/* top right: red */
	img.pixels[2] = (struct pixel_t){ 255, 0, 0 };
	img.pixels[3] = (struct pixel_t){ 255, 0, 0 };
	img.pixels[6] = (struct pixel_t){ 255, 0, 0 };
	img.pixels[7] = (struct pixel_t){ 255, 0, 0 };

	/* bottom left: green */
	img.pixels[8]  = (struct pixel_t){ 0, 255, 0 };
	img.pixels[9]  = (struct pixel_t){ 0, 255, 0 };
	img.pixels[12] = (struct pixel_t){ 0, 255, 0 };
	img.pixels[13] = (struct pixel_t){ 0, 255, 0 };

	/* bottom right: blue */
	img.pixels[10] = (struct pixel_t){ 0, 0, 255 };
	img.pixels[11] = (struct pixel_t){ 0, 0, 255 };
	img.pixels[14] = (struct pixel_t){ 0, 0, 255 };
	img.pixels[15] = (struct pixel_t){ 0, 0, 255 };

	/* extract top left */
	struct image_t *top_left = extract_subimage(&img, 0, 0, 1, 1);
	lily_assert_not_null(top_left);
	lily_assert_int_equal(top_left->width, 2);
	lily_assert_int_equal(top_left->height, 2);
	lily_assert_memory_equal(&(top_left->pixels[0]), &(img.pixels[0]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(top_left->pixels[1]), &(img.pixels[0]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(top_left->pixels[2]), &(img.pixels[0]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(top_left->pixels[3]), &(img.pixels[0]), sizeof(struct pixel_t));
	free_image(top_left);

	/* extract top right */
	struct image_t *top_right = extract_subimage(&img, 2, 0, 3, 1);
	lily_assert_not_null(top_right);
	lily_assert_int_equal(top_right->width, 2);
	lily_assert_int_equal(top_right->height, 2);
	lily_assert_memory_equal(&(top_right->pixels[0]), &(img.pixels[2]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(top_right->pixels[1]), &(img.pixels[2]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(top_right->pixels[2]), &(img.pixels[2]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(top_right->pixels[3]), &(img.pixels[2]), sizeof(struct pixel_t));
	free_image(top_right);

	/* extract bottom left */
	struct image_t *bottom_left = extract_subimage(&img, 0, 2, 1, 3);
	lily_assert_not_null(bottom_left);
	lily_assert_int_equal(bottom_left->width, 2);
	lily_assert_int_equal(bottom_left->height, 2);
	lily_assert_memory_equal(&(bottom_left->pixels[0]), &(img.pixels[8]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(bottom_left->pixels[1]), &(img.pixels[8]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(bottom_left->pixels[2]), &(img.pixels[8]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(bottom_left->pixels[3]), &(img.pixels[8]), sizeof(struct pixel_t));
	free_image(bottom_left);

	/* extract bottom right */
	struct image_t *bottom_right = extract_subimage(&img, 2, 2, 3, 3); 
	lily_assert_not_null(bottom_right);
	lily_assert_int_equal(bottom_right->width, 2);
	lily_assert_int_equal(bottom_right->height, 2);
	lily_assert_memory_equal(&(bottom_right->pixels[0]), &(img.pixels[10]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(bottom_right->pixels[1]), &(img.pixels[10]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(bottom_right->pixels[2]), &(img.pixels[10]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(bottom_right->pixels[3]), &(img.pixels[10]), sizeof(struct pixel_t));
	free_image(bottom_right);

	/* extract center */
	struct image_t *center = extract_subimage(&img, 1, 1, 2, 2);
	lily_assert_not_null(center);
	lily_assert_int_equal(center->width, 2);
	lily_assert_int_equal(center->height, 2);
	lily_assert_memory_equal(&(center->pixels[0]), &(img.pixels[0]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(center->pixels[1]), &(img.pixels[2]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(center->pixels[2]), &(img.pixels[8]), sizeof(struct pixel_t));
	lily_assert_memory_equal(&(center->pixels[3]), &(img.pixels[10]), sizeof(struct pixel_t));
	free_image(center);

	free(img.pixels);
}
