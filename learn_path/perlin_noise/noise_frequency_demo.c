/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   noise_frequency_demo.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 22:47:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "material.h"
#include "texture.h"
#include "bvh.h"
#include <stdio.h>
#include <stdlib.h>
#include "../error.h"

/* Render four spheres with increasing frequency (scale).
   Lower scale = rougher (larger features), higher scale = smoother (finer detail). */
static int perlin_spheres_frequency(void)
{
	/* Frequency/scale values: 1, 2, 4, 8 (increasing detail) */
	real_t scales[4] = {1.0, 2.0, 4.0, 8.0};
	real_t mix = 0.8;						   /* blend base color with noise */
	int octaves = 5;						   /* FBM octaves for complexity */
	t_color base = vec3_create(0.7, 0.5, 0.9); /* purple base */

	t_material *mats[4];
	for (int i = 0; i < 4; ++i)
	{
		t_texture *nt = noise_texture_create(scales[i], mix, octaves, base);
		if (!nt)
		{
			fprintf(stderr, "Error: failed to create noise texture (i=%d, scale=%.1f)\n", i, scales[i]);
			for (int k = 0; k < i; ++k)
				if (mats[k])
				{
					mats[k]->destroy(mats[k]);
					free(mats[k]);
				}
			return 1;
		}
		mats[i] = lambertian_create_texture(nt);
		if (!mats[i])
		{
			nt->destroy(nt);
			free(nt);
			fprintf(stderr, "Error: failed to create material (i=%d)\n", i);
			for (int k = 0; k < i; ++k)
				if (mats[k])
				{
					mats[k]->destroy(mats[k]);
					free(mats[k]);
				}
			return 1;
		}
	}

	t_hittable_list world;
	hittable_list_init(&world);

	/* ground (plain gray) */
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(0.5, 0.5, 0.5), mats[0]);
	if (!hittable_list_add_sphere(&world, &ground))
	{
		fprintf(stderr, "Error: failed to add ground sphere\n");
		hittable_list_clear(&world);
		for (int i = 0; i < 4; ++i)
		{
			mats[i]->destroy(mats[i]);
			free(mats[i]);
		}
		return 1;
	}

	/* place four spheres in a row with increasing frequency (scale) */
	real_t xs[4] = {-6.0, -2.0, 2.0, 6.0};
	for (int i = 0; i < 4; ++i)
	{
		t_point3 center = point3_create(xs[i], 2.0, 0.0);
		t_sphere s = create_sphere(&center, 2.0, vec3_create(1.0, 1.0, 1.0), mats[i]);
		if (!hittable_list_add_sphere(&world, &s))
		{
			fprintf(stderr, "Error: failed to add sphere i=%d\n", i);
			hittable_list_clear(&world);
			for (int k = 0; k < 4; ++k)
			{
				mats[k]->destroy(mats[k]);
				free(mats[k]);
			}
			return 1;
		}
	}

	/* camera positioned to view all four spheres */
	t_camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 800;
	cam.samples_per_pixel = 200;
	cam.max_depth = 50;

	cam.vfov = 20.0;
	cam.lookfrom = point3_create(0.0, 4.0, 18.0);
	cam.lookat = point3_create(0.0, 1.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);

	cam.defocus_angle = 0.0;
	{
		t_vec3 tmp_diff = vec3_sub(&cam.lookfrom, &cam.lookat);
		cam.focus_dist = vec3_length(&tmp_diff);
	}

	cam.u = vec3_zero();
	cam.v = vec3_zero();
	cam.w = vec3_zero();
	cam.defocus_disk_u = vec3_zero();
	cam.defocus_disk_v = vec3_zero();

	camera_init(&cam, cam.aspect_ratio, cam.image_width);

	FILE *out = fopen("perlin_frequency.ppm", "w");
	if (!out)
	{
		fprintf(stderr, "Error: failed to open output file\n");
		hittable_list_clear(&world);
		for (int i = 0; i < 4; ++i)
		{
			mats[i]->destroy(mats[i]);
			free(mats[i]);
		}
		return 1;
	}

	printf("Rendering four spheres with frequency scales 1.0, 2.0, 4.0, 8.0...\n");
	printf("Left to right: rougher → smoother (as frequency increases)\n");
	camera_render(&cam, out, &world);
	fclose(out);
	printf("Render complete! Output saved to perlin_frequency.ppm\n");

	hittable_list_clear(&world);
	for (int i = 0; i < 4; ++i)
	{
		mats[i]->destroy(mats[i]);
		free(mats[i]);
	}
	return 0;
}

int main(void)
{
	return perlin_spheres_frequency();
}
