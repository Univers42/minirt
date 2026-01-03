/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   distant_view.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 15:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 14:59:40 by dlesieur         ###   ########.fr       */
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

	/* Create spheres with materials */
	t_point3 p_ground = point3_create(0.0, -100.5, -1.0);
	t_sphere sphere_ground = create_sphere(&p_ground, 100.0, vec3_create(0.8, 0.8, 0.0), material_ground);

	t_point3 p_center = point3_create(0.0, 0.0, -1.2);
	t_sphere sphere_center = create_sphere(&p_center, 0.5, vec3_create(0.1, 0.2, 0.5), material_center);

	t_point3 p_left = point3_create(-1.0, 0.0, -1.0);
	t_sphere sphere_left = create_sphere(&p_left, 0.5, vec3_create(1.0, 1.0, 1.0), material_left);

	t_point3 p_bubble = point3_create(-1.0, 0.0, -1.0);
	t_sphere sphere_bubble = create_sphere(&p_bubble, 0.4, vec3_create(1.0, 1.0, 1.0), material_bubble);

	t_point3 p_right = point3_create(1.0, 0.0, -1.0);
	t_sphere sphere_right = create_sphere(&p_right, 0.5, vec3_create(0.8, 0.6, 0.2), material_right);

	/* Add spheres to world */
	hittable_list_add_sphere(&world, &sphere_ground);
	hittable_list_add_sphere(&world, &sphere_center);
	hittable_list_add_sphere(&world, &sphere_left);
	hittable_list_add_sphere(&world, &sphere_bubble);
	hittable_list_add_sphere(&world, &sphere_right);

	/* Initialize camera with distant viewpoint */
	t_camera camera;
	camera.aspect_ratio = 16.0 / 9.0;
	camera.image_width = 400;
	camera.samples_per_pixel = 100;
	camera.max_depth = 50;
	camera.vfov = 20.0;
	camera.lookfrom = point3_create(-2.0, 2.0, 1.0);
	camera.lookat = point3_create(0.0, 0.0, -1.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	/* Initialize u, v, w to avoid uninitialized values */
	camera.u = vec3_zero();
	camera.v = vec3_zero();
	camera.w = vec3_zero();
	srand(42);
	camera_init(&camera, camera.aspect_ratio, camera.image_width);

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

	camera_render(&camera, out, &world);
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
