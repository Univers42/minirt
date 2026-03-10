/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_test2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 21:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 15:12:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "bvh.h"
#include "constant_medium.h"

static t_point3	random_point3(real_t min, real_t max)
{
	return (point3_create(
			random_real_interval(min, max),
			random_real_interval(min, max),
			random_real_interval(min, max)));
}

static void	build_ground_boxes(t_hittable_list *world)
{
	t_hittable_list	boxes1;
	t_material		*ground;
	t_bvh_node		*bvh;
	int				i;
	int				j;

	hittable_list_init(&boxes1);
	ground = lambertian_create(vec3_create(0.48, 0.83, 0.53));
	i = -1;
	while (++i < 20)
	{
		j = -1;
		while (++j < 20)
		{
			real_t w = 100.0;
			real_t x0 = -1000.0 + i * w;
			real_t z0 = -1000.0 + j * w;
			t_point3 a = point3_create(x0, 0.0, z0);
			t_point3 b = point3_create(x0 + w,
					random_real_interval(1.0, 101.0), z0 + w);
			box(&boxes1, &a, &b, ground);
		}
	}
	bvh = bvh_node_create(&boxes1);
	if (bvh)
	{
		t_hittable_wrapper wrap = {
			.object = bvh, .owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit,
			.bbox = bvh->bbox};
		hittable_list_add_wrapper(world, &wrap);
	}
}

static void	add_light_quad(t_hittable_list *world)
{
	t_material	*light_mat;
	t_point3	lq;
	t_vec3		lu;
	t_vec3		lv;
	t_quad		q;
	t_quad		*qptr;

	light_mat = diffuse_light_create(vec3_create(7.0, 7.0, 7.0));
	lq = point3_create(123.0, 554.0, 147.0);
	lu = vec3_create(300.0, 0.0, 0.0);
	lv = vec3_create(0.0, 0.0, 265.0);
	q = quad_create(&lq, &lu, &lv, light_mat);
	qptr = (t_quad *)malloc(sizeof(t_quad));
	if (qptr)
	{
		*qptr = q;
		t_hittable_wrapper qw = {
			.object = qptr, .owned = true,
			.set_current = set_current_quad,
			.hit_noobj = quad_hit_noobj,
			.bbox = q.bbox};
		hittable_list_add_wrapper(world, &qw);
	}
}

static void	add_basic_spheres(t_hittable_list *world)
{
	t_material	*move_mat;
	t_material	*glass;
	t_material	*metal_mat;

	move_mat = lambertian_create(vec3_create(0.7, 0.3, 0.1));
	{
		t_point3 c1 = point3_create(400.0, 400.0, 200.0);
		t_point3 c2 = point3_create(430.0, 400.0, 200.0);
		t_sphere moving = create_sphere_moving(&c1, &c2, 50.0,
				vec3_create(0.7, 0.3, 0.1), move_mat);
		hittable_list_add_sphere(world, &moving);
	}
	glass = dielectric_create(1.5);
	{
		t_point3 p1 = point3_create(260.0, 150.0, 45.0);
		t_sphere s1 = create_sphere(&p1, 50.0,
				vec3_create(1.0, 1.0, 1.0), glass);
		hittable_list_add_sphere(world, &s1);
	}
	metal_mat = metal_create_fuzz(vec3_create(0.8, 0.8, 0.9), 1.0);
	{
		t_point3 p2 = point3_create(0.0, 150.0, 145.0);
		t_sphere s2 = create_sphere(&p2, 50.0,
				vec3_create(0.8, 0.8, 0.9), metal_mat);
		hittable_list_add_sphere(world, &s2);
	}
}

static void	add_volumes(t_hittable_list *world)
{
	t_material			*glass;
	t_sphere			*boundary;
	t_constant_medium	*med;

	glass = dielectric_create(1.5);
	boundary = (t_sphere *)malloc(sizeof(t_sphere));
	if (boundary)
	{
		t_point3 bc = point3_create(360.0, 150.0, 145.0);
		*boundary = create_sphere(&bc, 70.0,
				vec3_create(1.0, 1.0, 1.0), glass);
		t_hittable_wrapper bw = {
			.object = boundary, .owned = true,
			.set_current = set_current_sphere,
			.hit_noobj = sphere_hit_noobj,
			.bbox = boundary->bbox};
		hittable_list_add_wrapper(world, &bw);
		med = constant_medium_create_color(&bw, 0.2,
				vec3_create(0.2, 0.4, 0.9));
		if (med)
		{
			t_hittable_wrapper mw = {
				.object = med, .owned = true,
				.set_current = set_current_medium,
				.hit_noobj = constant_medium_hit_noobj,
				.bbox = constant_medium_bounding_box(med)};
			hittable_list_add_wrapper(world, &mw);
		}
	}
	boundary = (t_sphere *)malloc(sizeof(t_sphere));
	if (boundary)
	{
		t_point3 bc2 = point3_create(0.0, 0.0, 0.0);
		*boundary = create_sphere(&bc2, 5000.0,
				vec3_create(1.0, 1.0, 1.0), glass);
		t_hittable_wrapper bw2 = {
			.object = boundary, .owned = false,
			.set_current = set_current_sphere,
			.hit_noobj = sphere_hit_noobj,
			.bbox = boundary->bbox};
		med = constant_medium_create_color(&bw2, 0.0001,
				vec3_create(1.0, 1.0, 1.0));
		if (med)
		{
			t_hittable_wrapper mw2 = {
				.object = med, .owned = true,
				.set_current = set_current_medium,
				.hit_noobj = constant_medium_hit_noobj,
				.bbox = constant_medium_bounding_box(med)};
			hittable_list_add_wrapper(world, &mw2);
		}
	}
}

static void	add_textured_spheres(t_hittable_list *world)
{
	t_texture	*earth_tex;
	t_material	*earth_mat;
	t_texture	*pertex;
	t_material	*permat;

	earth_tex = image_texture_create_png(
			"/home/dlesieur/Documents/minirt/assets/images/earthmap.png");
	if (earth_tex)
		earth_mat = lambertian_create_texture(earth_tex);
	else
		earth_mat = lambertian_create(vec3_create(0.4, 0.2, 0.1));
	{
		t_point3 ec = point3_create(400.0, 200.0, 400.0);
		t_sphere es = create_sphere(&ec, 100.0,
				vec3_create(1.0, 1.0, 1.0), earth_mat);
		hittable_list_add_sphere(world, &es);
	}
	pertex = noise_texture_create(0.2);
	if (pertex)
		permat = lambertian_create_texture(pertex);
	else
		permat = lambertian_create(vec3_create(1.0, 1.0, 1.0));
	{
		t_point3 pc = point3_create(220.0, 280.0, 300.0);
		t_sphere ps = create_sphere(&pc, 80.0,
				vec3_create(1.0, 1.0, 1.0), permat);
		hittable_list_add_sphere(world, &ps);
	}
}

static void	add_sphere_cluster(t_hittable_list *world)
{
	t_hittable_list		boxes2;
	t_material			*white;
	t_bvh_node			*bvh;
	int					j;

	hittable_list_init(&boxes2);
	white = lambertian_create(vec3_create(0.73, 0.73, 0.73));
	j = -1;
	while (++j < 1000)
	{
		t_point3 rp = random_point3(0.0, 165.0);
		t_sphere s = create_sphere(&rp, 10.0,
				vec3_create(0.73, 0.73, 0.73), white);
		hittable_list_add_sphere(&boxes2, &s);
	}
	bvh = bvh_node_create(&boxes2);
	if (bvh)
	{
		t_hittable_wrapper bw = {
			.object = bvh, .owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit,
			.bbox = bvh->bbox};
		t_rotate_y_wrap *rot = rotate_y_create(&bw, 15.0);
		if (rot)
		{
			t_hittable_wrapper rw = {
				.object = rot, .owned = true,
				.set_current = set_current_rotate,
				.hit_noobj = rotate_y_hit_noobj,
				.bbox = rot->bbox};
			t_vec3 offset = vec3_create(-100.0, 270.0, 395.0);
			t_translate_wrap *tr = translate_create(&rw, &offset);
			if (tr)
			{
				t_hittable_wrapper tw = {
					.object = tr, .owned = true,
					.set_current = set_current_translate,
					.hit_noobj = translate_hit_noobj,
					.bbox = tr->bbox};
				hittable_list_add_wrapper(world, &tw);
			}
		}
	}
}

void	final_scene2(int image_width, int samples_per_pixel, int max_depth)
{
	t_hittable_list	world;
	t_bvh_node		*world_bvh;
	t_hittable_list	accel;
	t_camera		cam;

	hittable_list_init(&world);
	build_ground_boxes(&world);
	add_light_quad(&world);
	add_basic_spheres(&world);
	add_volumes(&world);
	add_textured_spheres(&world);
	add_sphere_cluster(&world);
	world_bvh = bvh_node_create(&world);
	hittable_list_init(&accel);
	if (world_bvh)
	{
		t_hittable_wrapper bvh_wrap = {
			.object = world_bvh, .owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit,
			.bbox = world_bvh->bbox};
		hittable_list_add_wrapper(&accel, &bvh_wrap);
	}
	cam.aspect_ratio = 1.0;
	cam.image_width = image_width;
	cam.samples_per_pixel = samples_per_pixel;
	cam.max_depth = max_depth;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	cam.vfov = 40.0;
	cam.lookfrom = point3_create(478.0, 278.0, -600.0);
	cam.lookat = point3_create(278.0, 278.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	{
		t_vec3 fv = vec3_sub(&cam.lookfrom, &cam.lookat);
		cam.focus_dist = vec3_length(&fv);
	}
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	if (world_bvh)
		camera_render(&cam, stdout, &accel);
	else
		camera_render(&cam, stdout, &world);
	hittable_list_clear(&accel);
	hittable_list_clear(&world);
}

int	main(void)
{
	final_scene2(800, 1000, 40);
	return (0);
}
