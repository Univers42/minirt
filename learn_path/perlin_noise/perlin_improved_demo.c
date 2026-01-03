/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin_improved_demo.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 23:01:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "material.h"
#include "texture.h"
#include "bvh.h"
#include <stdio.h>
#include <stdlib.h>
#include "../error.h"

/* Render ground and sphere with improved Perlin noise texture */
static int perlin_improved_spheres(void)
{
	/* Create improved noise texture with higher detail:
	   - scale=10.0: higher frequency for finer features
	   - mix=1.0: full noise (no base color blending)
	   - octaves=8: more octaves for labyrinthic complexity
	   - base: darker color for better contrast */
	t_texture *pertext = noise_texture_create(10.0, 1.0, 8, vec3_create(0.2, 0.2, 0.2));
	if (!pertext)
	{
		fprintf(stderr, "Error: failed to create noise texture\n");
		return 1;
	}

	t_material *mat = lambertian_create_texture(pertext);
	if (!mat)
	{
		pertext->destroy(pertext);
		free(pertext);
		fprintf(stderr, "Error: failed to create lambertian material\n");
		return 1;
	}

	t_hittable_list world;
	hittable_list_init(&world);

	/* Ground sphere (large) */
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(1.0, 1.0, 1.0), mat);
	if (!hittable_list_add_sphere(&world, &ground))
	{
		fprintf(stderr, "Error: failed to add ground sphere\n");
		hittable_list_clear(&world);
		mat->destroy(mat);
		free(mat);
		return 1;
	}

	/* Small sphere above ground */
	t_point3 center = point3_create(0.0, 2.0, 0.0);
	t_sphere globe = create_sphere(&center, 2.0, vec3_create(1.0, 1.0, 1.0), mat);
	if (!hittable_list_add_sphere(&world, &globe))
	{
		fprintf(stderr, "Error: failed to add perlin sphere\n");
		hittable_list_clear(&world);
		mat->destroy(mat);
		free(mat);
		return 1;
	}

	/* Camera */
	t_camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20.0;
	cam.lookfrom = point3_create(13.0, 2.0, 3.0);
	cam.lookat = point3_create(0.0, 0.0, 0.0);
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

	FILE *out = fopen("perlin_improved.ppm", "w");
	if (!out)
	{
		fprintf(stderr, "Error: failed to open output file\n");
		hittable_list_clear(&world);
		mat->destroy(mat);
		free(mat);
		return 1;
	}

	printf("Rendering improved Perlin noise spheres (gradient-based, high detail)...\n");
	printf("  Scale: 10.0 (fine frequency)\n");
	printf("  Octaves: 8 (labyrinthic complexity)\n");
	printf("  Mix: 1.0 (full noise)\n");
	camera_render(&cam, out, &world);
	fclose(out);
	printf("Render complete! Output saved to perlin_improved.ppm\n");

	hittable_list_clear(&world);
	mat->destroy(mat);
	free(mat);
	return 0;
}

int main(void)
{
	return perlin_improved_spheres();
}
