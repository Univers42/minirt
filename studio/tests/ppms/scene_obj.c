/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_obj.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:11:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "triangle.h"
#include "bvh.h"
#include "wf_rt.h"
#include <stdio.h>

static void	add_ground(t_hittable_list *world)
{
	t_material			*ground;
	t_point3			qp;
	t_vec3				qu;
	t_vec3				qv;
	t_quad				q;
	t_quad				*qptr;

	ground = lambertian_create(vec3_create(0.48, 0.83, 0.53));
	qp = point3_create(-20.0, 0.0, -20.0);
	qu = vec3_create(40.0, 0.0, 0.0);
	qv = vec3_create(0.0, 0.0, 40.0);
	q = quad_create(&qp, &qu, &qv, ground);
	qptr = (t_quad *)malloc(sizeof(t_quad));
	if (!qptr)
		return ;
	*qptr = q;
	{
		t_hittable_wrapper qw = {.object = qptr, .owned = true,
			.set_current = set_current_quad,
			.hit_noobj = quad_hit_noobj, .bbox = q.bbox};
		hittable_list_add_wrapper(world, &qw);
	}
}

static void	add_light(t_hittable_list *world)
{
	t_material			*light;
	t_point3			lp;
	t_vec3				lu;
	t_vec3				lv;
	t_quad				lq;
	t_quad				*lqptr;

	light = diffuse_light_create(vec3_create(15.0, 15.0, 15.0));
	lp = point3_create(-5.0, 12.0, -5.0);
	lu = vec3_create(10.0, 0.0, 0.0);
	lv = vec3_create(0.0, 0.0, 10.0);
	lq = quad_create(&lp, &lu, &lv, light);
	lqptr = (t_quad *)malloc(sizeof(t_quad));
	if (!lqptr)
		return ;
	*lqptr = lq;
	{
		t_hittable_wrapper lw = {.object = lqptr, .owned = true,
			.set_current = set_current_quad,
			.hit_noobj = quad_hit_noobj, .bbox = lq.bbox};
		hittable_list_add_wrapper(world, &lw);
	}
}

static void	add_objs(t_hittable_list *world)
{
	t_material	*gold;
	t_material	*red;

	gold = metal_create_fuzz(vec3_create(0.85, 0.65, 0.15), 0.05);
	if (!wf_obj_to_hittable_list("../wavefront/input/suzanne.obj",
			world, gold, 3.0f, 0.0f, 2.5f, 0.0f))
		fprintf(stderr, "Error: cannot load suzanne.obj\n");
	else
		fprintf(stderr, "Loaded suzanne.obj\n");
	red = lambertian_create(vec3_create(0.8, 0.2, 0.2));
	if (!wf_obj_to_hittable_list("../wavefront/input/cube.obj",
			world, red, 2.0f, -5.0f, 1.0f, 0.0f))
		fprintf(stderr, "Error: cannot load cube.obj\n");
	else
		fprintf(stderr, "Loaded cube.obj\n");
}

static void	add_spheres(t_hittable_list *world)
{
	t_point3	p;
	t_sphere	s;

	p = point3_create(4.5, 1.0, 3.0);
	s = create_sphere(&p, 1.0, vec3_create(1.0, 1.0, 1.0),
			dielectric_create(1.5));
	hittable_list_add_sphere(world, &s);
	p = point3_create(-3.0, 0.6, 4.0);
	s = create_sphere(&p, 0.6, vec3_create(0.9, 0.9, 0.92),
			metal_create_fuzz(vec3_create(0.9, 0.9, 0.92), 0.0));
	hittable_list_add_sphere(world, &s);
}

static void	setup_camera(t_camera *cam, int w, int spp, int depth)
{
	t_vec3	fv;

	cam->aspect_ratio = 16.0 / 9.0;
	cam->image_width = w;
	cam->samples_per_pixel = spp;
	cam->max_depth = depth;
	cam->background = vec3_create(0.70, 0.80, 1.00);
	cam->vfov = 40.0;
	cam->lookfrom = point3_create(12.0, 6.0, 12.0);
	cam->lookat = point3_create(0.0, 2.0, 0.0);
	cam->vup = vec3_create(0.0, 1.0, 0.0);
	cam->defocus_angle = 0.0;
	fv = vec3_sub(&cam->lookfrom, &cam->lookat);
	cam->focus_dist = vec3_length(&fv);
	camera_init(cam, cam->aspect_ratio, cam->image_width);
}

int	main(void)
{
	t_hittable_list	world;
	t_bvh_node		*bvh;
	t_hittable_list	accel;
	t_camera		cam;

	hittable_list_init(&world);
	add_ground(&world);
	add_light(&world);
	add_objs(&world);
	add_spheres(&world);
	bvh = bvh_node_create(&world);
	hittable_list_init(&accel);
	if (bvh)
	{
		t_hittable_wrapper bw = {.object = bvh, .owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit, .bbox = bvh->bbox};
		hittable_list_add_wrapper(&accel, &bw);
	}
	setup_camera(&cam, 400, 100, 20);
	camera_render(&cam, stdout, bvh ? &accel : &world);
	hittable_list_clear(&accel);
	hittable_list_clear(&world);
	return (0);
}
