/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 14:54:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "material.h"

int main(void)
{
	/* Create world */
	t_hittable_list world;
	hittable_list_init(&world);

	/* Create materials */
	real_t R = (real_t)cos(PI / 4.0);
	t_material *material_left = lambertian_create(vec3_create(0.0, 0.0, 1.0));
	t_material *material_right = lambertian_create(vec3_create(1.0, 0.0, 0.0));

	if (!material_left || !material_right)
	{
		fprintf(stderr, "Error: failed to allocate materials\n");
		if (material_left)
			material_left->destroy(material_left);
		if (material_right)
			material_right->destroy(material_right);
		return 1;
	}

	/* Create spheres with materials */
	t_point3 p_left = point3_create(-R, 0.0, -1.0);
	t_sphere sphere_left = create_sphere(&p_left, R, vec3_create(0.0, 0.0, 1.0), material_left);

	t_point3 p_right = point3_create(R, 0.0, -1.0);
	t_sphere sphere_right = create_sphere(&p_right, R, vec3_create(1.0, 0.0, 0.0), material_right);

	/* Add spheres to world */
	hittable_list_add_sphere(&world, &sphere_left);
	hittable_list_add_sphere(&world, &sphere_right);

	/* Initialize camera */
	t_camera camera;
	camera.aspect_ratio = 16.0 / 9.0;
	camera.image_width = 400;
	camera.samples_per_pixel = 100;
	camera.max_depth = 50;
	camera.vfov = 90.0;
	srand(42);
	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render to file */
	FILE *out = fopen("output.ppm", "w");
	if (!out)
	{
		fprintf(stderr, "Error: failed to open output file\n");
		hittable_list_clear(&world);
		material_left->destroy(material_left);
		material_right->destroy(material_right);
		free(material_left);
		free(material_right);
		return 1;
	}

	camera_render(&camera, out, &world);
	fclose(out);

	/* Cleanup */
	hittable_list_clear(&world);
	material_left->destroy(material_left);
	material_right->destroy(material_right);
	free(material_left);
	free(material_right);

	return 0;
}