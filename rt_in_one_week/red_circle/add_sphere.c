#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "settings.h"
#include "types.h"
#include "Color.h"
#include "Vector.h"
#include "ray.h"



int main(void)
{
	int i;
	int j;
	const char *out_path = "create_scene_rays.ppm";
	FILE *out;
	const double aspect_ratio = 12.0 / 15.0;
	const int image_width = IMAGE_WIDTH;
	int image_height = (int)(image_width / aspect_ratio);
	real_t focal_length = 1.0;
	real_t viewport_height = 2.0;

	image_height = (image_height < 1) ? 1 : image_height;

	out = fopen(out_path, "w");
	if (!out)
	{
		perror("fopen");
		return (1);
	}

	/* compute viewport in world space */
	real_t viewport_width = (real_t)(viewport_height * ((double)image_width / image_height));
	// For simplete we choose center at 0,0,0, where y-axis go up, x-axis to the right, negative z-axis pointing in view diretion
	t_vec3 camera_center = vec3_create(0.0, 0.0, 0.0);
	/**
	 * 
	 */
	t_vec3 viewport_u = vec3_create(viewport_width, 0.0, 0.0);
	t_vec3 viewport_v = vec3_create(0.0, -viewport_height, 0.0);
	t_vec3 focal_vec = vec3_create(0.0, 0.0, focal_length);

	t_vec3 pixel_delta_u = vec3_div_scalar(&viewport_u, (real_t)image_width);
	t_vec3 pixel_delta_v = vec3_div_scalar(&viewport_v, (real_t)image_height);

	/* upper-left corner of the viewport */
	t_vec3 tmp = vec3_div_scalar(&viewport_u, (real_t)2.0);	 /* u/2 */
	t_vec3 tmp2 = vec3_div_scalar(&viewport_v, (real_t)2.0); /* v/2 */
	t_vec3 upper = vec3_sub(&camera_center, &focal_vec);	 /* camera - focal */
	t_vec3 ul = vec3_sub(&upper, &tmp);						 /* - u/2 */
	ul = vec3_sub(&ul, &tmp2);								 /* - v/2 */

	/* location of pixel (0,0) center */
	t_vec3 sum_delta = vec3_add(&pixel_delta_u, &pixel_delta_v);
	t_vec3 half_delta = vec3_mul_scalar(&sum_delta, (real_t)0.5);
	t_vec3 pixel00_loc = vec3_add(&ul, &half_delta);

	/* write header */
	fprintf(out, "P3\n%d %d\n255\n", image_width, image_height);

	for (j = 0; j < image_height; ++j)
	{
		fprintf(stderr, "\rScanlines remaining: %d ", (image_height - j));
		fflush(stderr);
		for (i = 0; i < image_width; ++i)
		{
			t_vec3 du = vec3_mul_scalar(&pixel_delta_u, (real_t)i);
			t_vec3 dv = vec3_mul_scalar(&pixel_delta_v, (real_t)j);
			t_vec3 tmp1 = vec3_add(&du, &dv);
			t_vec3 pixel_center = vec3_add(&pixel00_loc, &tmp1);

			/* ray from camera_center through pixel_center */
			t_vec3 ray_dir = vec3_sub(&pixel_center, &camera_center);
			t_ray r = ray_create(camera_center, ray_dir);

			t_vec3 pixel_color = ray_color(&r);
			write_color(out, &pixel_color);
		}
	}
	fprintf(stderr, "\rDone.\n");
	fclose(out);
	return (0);
}