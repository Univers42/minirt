/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 03:15:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"

int main(void)
{
	/* Render settings: single full-width image divided into 5 vertical stripes */
	const int cols = 5;
	const int full_width = 1000;				/* total image width (evenly divisible by 5) */
	const int stripe_width = full_width / cols; /* 200 px per stripe */
	const real_t aspect = (real_t)(16.0 / 9.0);
	const int image_height = (int)((real_t)full_width / aspect);

	/* Prepare camera for full-width rendering */
	t_camera camera;
	camera.samples_per_pixel = 100;
	camera.max_depth = 50;
	srand(42);
	camera_init(&camera, aspect, full_width);

	/* Build a single world with colored objects (keep colors) */
	t_hittable_list world;
	hittable_list_init(&world);

	t_point3 c1 = point3_create(0.0, 0.0, -1.0);
	/* darker gray sphere for better contrast */
	t_vec3 sphere_gray = vec3_create((real_t)0.3, (real_t)0.3, (real_t)0.3);
	t_sphere s1 = create_sphere(&c1, 0.5, sphere_gray);
	hittable_list_add_sphere(&world, &s1);

	t_point3 c2 = point3_create(0.0, -100.5, -1.0);
	/* lighter gray ground to see lightness difference */
	t_vec3 ground_gray = vec3_create((real_t)0.7, (real_t)0.7, (real_t)0.7);
	t_sphere s2 = create_sphere(&c2, 100.0, ground_gray);
	hittable_list_add_sphere(&world, &s2);

	/* Allocate image buffer for full-width render (linear colors) + hit-mask */
	t_vec3 *img = (t_vec3 *)malloc(sizeof(t_vec3) * (size_t)full_width * (size_t)image_height);
	/* hit_mask: 1 if pixel is object-hit, 0 if background */
	char *hit_mask = (char *)calloc((size_t)full_width * (size_t)image_height, sizeof(char));
	if (!img || !hit_mask)
	{
		free(img);
		free(hit_mask);
		hittable_list_clear(&world);
		return 1;
	}

	/* Render the full-width image into img[] and populate hit_mask */
	for (int j = 0; j < image_height; ++j)
	{
		fprintf(stderr, "\rRendering: scanlines remaining %d ", (image_height - j));
		fflush(stderr);
		for (int i = 0; i < full_width; ++i)
		{
			t_vec3 pixel_color = vec3_zero();
			int hit_count = 0;
			for (int sample = 0; sample < (int)camera.samples_per_pixel; ++sample)
			{
				t_ray r = get_ray(&camera, i, j);
				/* check if this sample hits an object */
				t_hit_record rec;
				if (hittable_list_hit(&world, &r, interval((real_t)1e-4, INFINITY), &rec))
					hit_count++;
				t_vec3 sample_color = ray_color_world(&r, &world);
				pixel_color = vec3_add(&pixel_color, &sample_color);
			}
			t_vec3 scaled = vec3_mul_scalar(&pixel_color, camera.pixel_samples_scale);
			img[j * full_width + i] = scaled;
			/* if majority of samples hit an object, mark as object pixel */
			hit_mask[j * full_width + i] = (hit_count > (int)camera.samples_per_pixel / 2) ? 1 : 0;
		}
	}
	fprintf(stderr, "\rRendering complete.                      \n");

	/* Reflectance for each vertical stripe: stronger contrast for visual clarity */
	static const real_t reflectances[] = {0.05, 0.25, 0.5, 0.75, 1.0};

	/* Write output PPM with same dimensions, applying stripe-based reflectance only to object pixels */
	FILE *out = fopen("out_gamut.ppm", "w");
	if (!out)
	{
		free(img);
		free(hit_mask);
		hittable_list_clear(&world);
		return 1;
	}
	fprintf(out, "P3\n%d %d\n255\n", full_width, image_height);

	for (int j = 0; j < image_height; ++j)
	{
		fprintf(stderr, "\rWriting output: scanlines remaining %d ", (image_height - j));
		fflush(stderr);
		for (int i = 0; i < full_width; ++i)
		{
			/* determine which vertical stripe this pixel belongs to */
			int stripe = i / stripe_width;
			if (stripe >= cols)
				stripe = cols - 1; /* clamp for safety */
			real_t refl = reflectances[stripe];

			t_vec3 base = img[j * full_width + i];
			/* apply reflectance filter only if pixel is an object hit */
			if (hit_mask[j * full_width + i])
			{
				t_vec3 filtered = vec3_mul_scalar(&base, refl);
				write_color(out, &filtered);
			}
			else
			{
				/* background: write unmodified (full sky gradient) */
				write_color(out, &base);
			}
		}
	}
	fprintf(stderr, "\rDone writing output.                 \n");
	fclose(out);

	/* cleanup */
	free(img);
	free(hit_mask);
	hittable_list_clear(&world);
	return 0;
}