/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 01:21:52 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "common.h"

int main(void)
{
	/* World */
	t_hittable_list world;
	FILE *out;
	t_camera camera;

	
	hittable_list_init(&world);

	/* add spheres: center(0,0,-1) radius 0.5 and ground sphere */
	t_point3 c1 = point3_create(0.0, 0.0, -1.0);
	t_sphere s1 = create_sphere(&c1, 0.5);
	hittable_list_add_sphere(&world, &s1);

	t_point3 c2 = point3_create(0.0, -100.5, -1.0);
	t_sphere s2 = create_sphere(&c2, 100.0);
	hittable_list_add_sphere(&world, &s2);

	t_point3 c3 = point3_create(0.0, 100.5, 50.0);
	t_sphere s3 = create_sphere(&c3, 500.0);
	hittable_list_add_sphere(&world, &s3);

	/* setup camera */
	camera.samples_per_pixel = 100;
	camera_init(&camera, (real_t)(16.0 / 9.0), 400);

	/* render to stdout (you can change to fopen("out.ppm","w") if you prefer) */
	out = fopen("out.ppm", "w");
	camera_render(&camera, out, &world);

	/* cleanup */
	hittable_list_clear(&world);
	return 0;
}