/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   visualization_spheres.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 20:10:36 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 20:59:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"

/**
 * This yields a picture taht is really just a visualization of where the spheres are located along
 * wiht their surface normal. This if oftena great way to view any flaws or specific
 * characteristics of a geometric model
 */
int main(void)
{
	/* Image */
	const double aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	int image_height = (int)(image_width / aspect_ratio);
	image_height = (image_height < 1) ? 1 : image_height;

	/* World */
	t_hittable_list world;
	hittable_list_init(&world);

	/* add spheres: center(0,0,-1) radius 0.5 and ground sphere */
	t_point3 c1 = point3_create(0.0, 0.0, -1.0);
	t_sphere s1 = create_sphere(&c1, 0.5);
	hittable_list_add_sphere(&world, &s1);

	t_point3 c2 = point3_create(0.0, -100.5, -1.0);
	t_sphere s2 = create_sphere(&c2, 100.0);
	hittable_list_add_sphere(&world, &s2);

	t_point3	c3 = point3_create(0.0, 100.5, 50);
	t_sphere	s3 = create_sphere(&c3, 500.0);
	hittable_list_add_sphere(&world, &s3);
	/* Camera */
	real_t focal_length = 1.0;
	real_t viewport_height = 2.0;
	real_t viewport_width = (real_t)(viewport_height * ((double)image_width / image_height));
	t_vec3 camera_center = vec3_create(0.0, 0.0, 0.0);

	/* viewport basis */
	t_vec3 viewport_u = vec3_create(viewport_width, 0.0, 0.0);
	t_vec3 viewport_v = vec3_create(0.0, -viewport_height, 0.0);

	/* per-pixel deltas */
	t_vec3 pixel_delta_u = vec3_div_scalar(&viewport_u, (real_t)image_width);
	t_vec3 pixel_delta_v = vec3_div_scalar(&viewport_v, (real_t)image_height);

	/* upper-left pixel center */
	t_vec3 focal_vec = vec3_create(0.0, 0.0, focal_length);
	t_vec3 tmp_u = vec3_div_scalar(&viewport_u, (real_t)2.0);
	t_vec3 tmp_v = vec3_div_scalar(&viewport_v, (real_t)2.0);
	t_vec3 upper = vec3_sub(&camera_center, &focal_vec); /* camera - focal */
	t_vec3 ul = vec3_sub(&upper, &tmp_u);				 /* - u/2 */
	ul = vec3_sub(&ul, &tmp_v);							 /* - v/2 */

	t_vec3 sum_delta = vec3_add(&pixel_delta_u, &pixel_delta_v);
	t_vec3 half_delta = vec3_mul_scalar(&sum_delta, (real_t)0.5);
	t_vec3 pixel00_loc = vec3_add(&ul, &half_delta);

	/* write header to out.ppm */
	FILE *out = fopen("out.ppm", "w");
	if (!out)
	{
		perror("fopen out.ppm");
		return 1;
	}
	fprintf(out, "P3\n%d %d\n255\n", image_width, image_height);

	/* render */
	for (int j = 0; j < image_height; ++j)
	{
		fprintf(stderr, "\rScanlines remaining: %d ", (image_height - j));
		fflush(stderr);
		for (int i = 0; i < image_width; ++i)
		{
			t_vec3 du = vec3_mul_scalar(&pixel_delta_u, (real_t)i);
			t_vec3 dv = vec3_mul_scalar(&pixel_delta_v, (real_t)j);
			t_vec3 tmp = vec3_add(&du, &dv);
			t_vec3 pixel_center = vec3_add(&pixel00_loc, &tmp);

			/* ray from camera_center through pixel_center */
			t_vec3 ray_dir = vec3_sub(&pixel_center, &camera_center);
			t_ray r = ray_create(camera_center, ray_dir);

			t_vec3 pixel_color = ray_color_world(&r, &world);
			write_color(out, &pixel_color);
		}
	}
	fprintf(stderr, "\rDone.\n");

	/* cleanup */
	fclose(out);
	hittable_list_clear(&world);
	return 0;
}