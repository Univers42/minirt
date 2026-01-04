/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       cornell_box.c                                                   */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/04 17:14:23                                             */
/*  Updated:    2026/01/04 17:14:23                                             */
/*                                                                              */
/* ============================================================================ */

#include "common.h"

void cornell_box(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Create materials */
	t_material *red = lambertian_create(vec3_create(0.65, 0.05, 0.05));
	t_material *white = lambertian_create(vec3_create(0.73, 0.73, 0.73));
	t_material *green = lambertian_create(vec3_create(0.12, 0.45, 0.15));
	t_material *light = diffuse_light_create(vec3_create(15.0, 15.0, 15.0));

	/* Cornell box sides: quads */
	/* Left wall (green) */
	t_point3 p1 = point3_create(555.0, 0.0, 0.0);
	t_vec3 u1 = vec3_create(0.0, 0.0, 555.0);
	t_vec3 v1 = vec3_create(0.0, 555.0, 0.0);
	t_quad q1 = quad_create(&p1, &u1, &v1, green);
	t_quad *q1_copy = (t_quad *)malloc(sizeof(t_quad));
	if (q1_copy)
	{
		*q1_copy = q1;
		hittable_list_add_nonowned(&world, q1_copy, set_current_quad, quad_hit_noobj, &q1.bbox);
	}

	/* Right wall (red) */
	t_point3 p2 = point3_create(0.0, 0.0, 555.0);
	t_vec3 u2 = vec3_create(0.0, 0.0, -555.0);
	t_vec3 v2 = vec3_create(0.0, 555.0, 0.0);
	t_quad q2 = quad_create(&p2, &u2, &v2, red);
	t_quad *q2_copy = (t_quad *)malloc(sizeof(t_quad));
	if (q2_copy)
	{
		*q2_copy = q2;
		hittable_list_add_nonowned(&world, q2_copy, set_current_quad, quad_hit_noobj, &q2.bbox);
	}

	/* Top wall (white) */
	t_point3 p3 = point3_create(0.0, 555.0, 0.0);
	t_vec3 u3 = vec3_create(555.0, 0.0, 0.0);
	t_vec3 v3 = vec3_create(0.0, 0.0, 555.0);
	t_quad q3 = quad_create(&p3, &u3, &v3, white);
	t_quad *q3_copy = (t_quad *)malloc(sizeof(t_quad));
	if (q3_copy)
	{
		*q3_copy = q3;
		hittable_list_add_nonowned(&world, q3_copy, set_current_quad, quad_hit_noobj, &q3.bbox);
	}

	/* Bottom wall (white) */
	t_point3 p4 = point3_create(0.0, 0.0, 555.0);
	t_vec3 u4 = vec3_create(555.0, 0.0, 0.0);
	t_vec3 v4 = vec3_create(0.0, 0.0, -555.0);
	t_quad q4 = quad_create(&p4, &u4, &v4, white);
	t_quad *q4_copy = (t_quad *)malloc(sizeof(t_quad));
	if (q4_copy)
	{
		*q4_copy = q4;
		hittable_list_add_nonowned(&world, q4_copy, set_current_quad, quad_hit_noobj, &q4.bbox);
	}

	/* Back wall (white) */
	t_point3 p5 = point3_create(555.0, 0.0, 555.0);
	t_vec3 u5 = vec3_create(-555.0, 0.0, 0.0);
	t_vec3 v5 = vec3_create(0.0, 555.0, 0.0);
	t_quad q5 = quad_create(&p5, &u5, &v5, white);
	t_quad *q5_copy = (t_quad *)malloc(sizeof(t_quad));
	if (q5_copy)
	{
		*q5_copy = q5;
		hittable_list_add_nonowned(&world, q5_copy, set_current_quad, quad_hit_noobj, &q5.bbox);
	}

	/* Light quad */
	t_point3 light_p = point3_create(213.0, 554.0, 227.0);
	t_vec3 light_u = vec3_create(130.0, 0.0, 0.0);
	t_vec3 light_v = vec3_create(0.0, 0.0, 105.0);
	t_quad light_q = quad_create(&light_p, &light_u, &light_v, light);
	t_quad *light_q_copy = (t_quad *)malloc(sizeof(t_quad));
	if (light_q_copy)
	{
		*light_q_copy = light_q;
		hittable_list_add_nonowned(&world, light_q_copy, set_current_quad, quad_hit_noobj, &light_q.bbox);
	}

	/* Box 1: rotated and translated */
	t_point3 box1_a = point3_create(0.0, 0.0, 0.0);
	t_point3 box1_b = point3_create(165.0, 330.0, 165.0);
	t_hittable_list box1_list;
	hittable_list_init(&box1_list);
	box(&box1_list, &box1_a, &box1_b, white);

	t_bvh_node *box1_bvh = bvh_node_create(&box1_list);
	if (box1_bvh)
	{
		t_hittable_wrapper bvh_wrap = {
			.object = box1_bvh,
			.owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit,
			.bbox = box1_bvh->bbox};

		t_rotate_y_wrap *rot1 = rotate_y_create(&bvh_wrap, 15.0);
		if (rot1)
		{
			t_hittable_wrapper rot_wrap = {
				.object = rot1,
				.owned = true,
				.set_current = set_current_rotate,
				.hit_noobj = rotate_y_hit_noobj,
				.bbox = rot1->bbox};

			t_vec3 trans1_offset = vec3_create(265.0, 0.0, 295.0);
			t_translate_wrap *trans1 = translate_create(&rot_wrap, &trans1_offset);
			if (trans1)
			{
				t_hittable_wrapper trans_wrap = {
					.object = trans1,
					.owned = true,
					.set_current = set_current_translate,
					.hit_noobj = translate_hit_noobj,
					.bbox = trans1->bbox};
				hittable_list_add_wrapper(&world, &trans_wrap);
			}
		}
	}

	/* Box 2: rotated and translated */
	t_point3 box2_a = point3_create(0.0, 0.0, 0.0);
	t_point3 box2_b = point3_create(165.0, 165.0, 165.0);
	t_hittable_list box2_list;
	hittable_list_init(&box2_list);
	box(&box2_list, &box2_a, &box2_b, white);

	t_bvh_node *box2_bvh = bvh_node_create(&box2_list);
	if (box2_bvh)
	{
		t_hittable_wrapper bvh_wrap = {
			.object = box2_bvh,
			.owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit,
			.bbox = box2_bvh->bbox};

		t_rotate_y_wrap *rot2 = rotate_y_create(&bvh_wrap, -18.0);
		if (rot2)
		{
			t_hittable_wrapper rot_wrap = {
				.object = rot2,
				.owned = true,
				.set_current = set_current_rotate,
				.hit_noobj = rotate_y_hit_noobj,
				.bbox = rot2->bbox};

			t_vec3 trans2_offset = vec3_create(130.0, 0.0, 65.0);
			t_translate_wrap *trans2 = translate_create(&rot_wrap, &trans2_offset);
			if (trans2)
			{
				t_hittable_wrapper trans_wrap = {
					.object = trans2,
					.owned = true,
					.set_current = set_current_translate,
					.hit_noobj = translate_hit_noobj,
					.bbox = trans2->bbox};
				hittable_list_add_wrapper(&world, &trans_wrap);
			}
		}
	}

	/* Setup camera */
	t_camera cam;
	cam.aspect_ratio = 1.0;
	cam.image_width = 600;
	cam.samples_per_pixel = 64.0;
	cam.max_depth = 50;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	cam.vfov = 40.0;
	cam.lookfrom = point3_create(278.0, 278.0, -800.0);
	cam.lookat = point3_create(278.0, 278.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	t_vec3 focus_vec = vec3_sub(&cam.lookfrom, &cam.lookat);
	cam.focus_dist = vec3_length(&focus_vec);

	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
}
