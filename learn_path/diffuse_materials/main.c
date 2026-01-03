/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 02:25:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"

int main(void)
{
	t_hittable_list world;
	FILE *out;
	t_camera camera;

	hittable_list_init(&world);

	/* add spheres: center(0,0,-1) radius 0.5 and ground sphere */
	t_point3 c1 = point3_create(0.0, 0.0, -1.0);
	/* darker, stronger green for the small sphere so it reads as green */
	t_vec3 green = vec3_create((real_t)0.0, (real_t)0.75, (real_t)0.0);
	t_sphere s1 = create_sphere(&c1, 0.5, green);
	hittable_list_add_sphere(&world, &s1);

	t_point3 c2 = point3_create(0.0, -100.5, -1.0);
	/* darker ground green (less bright than before) */
	t_vec3 ground_green = vec3_create((real_t)0.20, (real_t)0.50, (real_t)0.20);
	t_sphere s2 = create_sphere(&c2, 100.0, ground_green);
	hittable_list_add_sphere(&world, &s2);

	/* setup camera */
	camera.samples_per_pixel = 100;
	/* deterministic seed for reproducible debug runs */
	srand(42);
	camera_init(&camera, (real_t)(16.0 / 9.0), 400);

	/* render to stdout (you can change to fopen("out.ppm","w") if you prefer) */
	out = fopen("out.ppm", "w");
	camera_render(&camera, out, &world);

	/* cleanup */
	hittable_list_clear(&world);
	return 0;
}