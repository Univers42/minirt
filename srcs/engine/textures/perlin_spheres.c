/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin_spheres.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:20:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 01:19:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "noise_texture.h"
#include <stdio.h>

static t_material	*perlin_spheres_world(t_hittable_list *world,
	t_texture **pertext)
{
	t_material	*mat;
	t_point3	center;
	t_sphere	sphere;

	hittable_list_init(world);
	*pertext = noise_texture_create((real_t)4.0);
	if (!*pertext)
		return (NULL);
	mat = lambertian_create_texture(*pertext);
	if (!mat)
		return (NULL);
	center = point3_create(0.0, -1000.0, 0.0);
	sphere = create_sphere(&center, 1000.0, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(world, &sphere);
	center = point3_create(0.0, 2.0, 0.0);
	sphere = create_sphere(&center, 2.0, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(world, &sphere);
	return (mat);
}

static void	perlin_spheres_camera(t_camera *camera)
{
	camera->aspect_ratio = 16.0 / 9.0;
	camera->image_width = 800;
	camera->samples_per_pixel = 300.0;
	camera->max_depth = 50;
	camera->vfov = 20.0;
	camera->lookfrom = point3_create(13.0, 2.0, 3.0);
	camera->lookat = point3_create(0.0, 0.0, 0.0);
	camera->vup = vec3_create(0.0, 1.0, 0.0);
	camera->defocus_angle = 0.0;
	camera->focus_dist = 10.0;
	camera->background = vec3_create(0.70, 0.80, 1.00);
	camera_init(camera, camera->aspect_ratio, camera->image_width);
}

void	perlin_spheres(void)
{
	t_hittable_list	world;
	t_material		*mat;
	t_texture		*pertext;
	t_camera		camera;

	pertext = NULL;
	mat = perlin_spheres_world(&world, &pertext);
	if (!mat)
	{
		fprintf(stderr, "Failed to build perlin scene\n");
		hittable_list_clear(&world);
		if (pertext)
		{
			pertext->destroy(pertext);
			free(pertext);
		}
		return ;
	}
	perlin_spheres_camera(&camera);
	camera_render(&camera, stdout, &world);
	hittable_list_clear(&world);
	mat->destroy(mat);
	free(mat);
	pertext->destroy(pertext);
	free(pertext);
}
