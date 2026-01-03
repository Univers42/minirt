/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 15:08:32 by dlesieur         ###   ########.fr       */
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
	t_material *material_ground = lambertian_create(vec3_create(0.8, 0.8, 0.0));
	t_material *material_center = lambertian_create(vec3_create(0.1, 0.2, 0.5));
	t_material *material_left = dielectric_create(1.50);
	t_material *material_bubble = dielectric_create(1.00 / 1.50);
	t_material *material_right = metal_create_fuzz(vec3_create(0.8, 0.6, 0.2), 1.0);

	if (!material_ground || !material_center || !material_left || !material_bubble || !material_right)
	{
		fprintf(stderr, "Error: failed to allocate materials\n");
		if (material_ground)
			material_ground->destroy(material_ground);
		if (material_center)
			material_center->destroy(material_center);
		if (material_left)
			material_left->destroy(material_left);
		if (material_bubble)
			material_bubble->destroy(material_bubble);
		if (material_right)
			material_right->destroy(material_right);
		return 1;
	}

	/* Add spheres to world */
	t_point3 p1 = point3_create(0.0, -100.5, -1.0);
	t_sphere sphere1 = create_sphere(&p1, 100.0, vec3_create(0.8, 0.8, 0.0), material_ground);
	hittable_list_add_sphere(&world, &sphere1);

	t_point3 p2 = point3_create(0.0, 0.0, -1.2);
	t_sphere sphere2 = create_sphere(&p2, 0.5, vec3_create(0.1, 0.2, 0.5), material_center);
	hittable_list_add_sphere(&world, &sphere2);

	t_point3 p3 = point3_create(-1.0, 0.0, -1.0);
	t_sphere sphere3 = create_sphere(&p3, 0.5, vec3_create(1.0, 1.0, 1.0), material_left);
	hittable_list_add_sphere(&world, &sphere3);

	t_point3 p4 = point3_create(-1.0, 0.0, -1.0);
	t_sphere sphere4 = create_sphere(&p4, 0.4, vec3_create(1.0, 1.0, 1.0), material_bubble);
	hittable_list_add_sphere(&world, &sphere4);

	t_point3 p5 = point3_create(1.0, 0.0, -1.0);
	t_sphere sphere5 = create_sphere(&p5, 0.5, vec3_create(0.8, 0.6, 0.2), material_right);
	hittable_list_add_sphere(&world, &sphere5);

	/* Initialize camera */
	t_camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20.0;
	cam.lookfrom = point3_create(-2.0, 2.0, 1.0);
	cam.lookat = point3_create(0.0, 0.0, -1.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);

	cam.defocus_angle = 10.0;
	cam.focus_dist = 3.4;

	/* Initialize basis vectors to avoid uninitialized values */
	cam.u = vec3_zero();
	cam.v = vec3_zero();
	cam.w = vec3_zero();
	cam.defocus_disk_u = vec3_zero();
	cam.defocus_disk_v = vec3_zero();

	srand(42);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);

	/* Render to file */
	FILE *out = fopen("output.ppm", "w");
	if (!out)
	{
		fprintf(stderr, "Error: failed to open output file\n");
		hittable_list_clear(&world);
		material_ground->destroy(material_ground);
		material_center->destroy(material_center);
		material_left->destroy(material_left);
		material_bubble->destroy(material_bubble);
		material_right->destroy(material_right);
		free(material_ground);
		free(material_center);
		free(material_left);
		free(material_bubble);
		free(material_right);
		return 1;
	}

	camera_render(&cam, out, &world);
	fclose(out);

	/* Cleanup */
	hittable_list_clear(&world);
	material_ground->destroy(material_ground);
	material_center->destroy(material_center);
	material_left->destroy(material_left);
	material_bubble->destroy(material_bubble);
	material_right->destroy(material_right);
	free(material_ground);
	free(material_center);
	free(material_left);
	free(material_bubble);
	free(material_right);

	return 0;
}