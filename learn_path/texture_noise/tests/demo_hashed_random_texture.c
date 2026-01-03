/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_hashed_random_texture.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 23:47:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "../common.h"
#include "../noise_texture.h"
#include "../material.h"
#include "../hittable_list.h"
#include "../sphere.h"
#include "../camera.h"

void perlin_spheres(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Create noise texture with frequency scale of 4.0 and turbulence depth 7 */
	t_texture *pertext = noise_texture_create_turb((real_t)2.0, 7);
	if (!pertext)
	{
		fprintf(stderr, "Failed to create noise texture\n");
		return;
	}

	/* Create material using the noise texture */
	t_material *mat = lambertian_create_texture(pertext);
	if (!mat)
	{
		fprintf(stderr, "Failed to create lambertian material\n");
		pertext->destroy(pertext);
		free(pertext);
		return;
	}

	/* Ground sphere: large radius, positioned below */
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(&world, &ground);

	/* Upper sphere */
	t_point3 upper_center = point3_create(0.0, 2.0, 0.0);
	t_sphere upper = create_sphere(&upper_center, 2.0, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(&world, &upper);

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 16.0 / 9.0;
	camera.image_width = 400;
	camera.samples_per_pixel = 100.0;
	camera.max_depth = 50;
	camera.vfov = 20.0;
	camera.lookfrom = point3_create(13.0, 2.0, 3.0);
	camera.lookat = point3_create(0.0, 0.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render to file (camera_render writes to ../output/) */
	camera_render(&camera, NULL, &world);

	/* Cleanup: free in reverse order of creation */
	hittable_list_clear(&world);

	/* Only destroy material, not texture (material owns it via lambertian_create_texture) */
	if (mat)
	{
		mat->destroy(mat);
		free(mat);
	}
}

int main(void)
{
	perlin_spheres();
	return 0;
}
