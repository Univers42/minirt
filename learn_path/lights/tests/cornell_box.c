/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cornell_box.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:55:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 15:40:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../common.h"
#include "../quad.h"
#include <stdio.h>
#include <stdlib.h>

void cornell_box(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Materials */
	t_material *red = lambertian_create(vec3_create(0.65, 0.05, 0.05));
	t_material *white = lambertian_create(vec3_create(0.73, 0.73, 0.73));
	t_material *green = lambertian_create(vec3_create(0.12, 0.45, 0.15));
	t_material *light = diffuse_light_create(vec3_create(15.0, 15.0, 15.0));

	if (!red || !white || !green || !light)
	{
		fprintf(stderr, "Failed to create materials\n");
		return;
	}

	/* Right wall (green) */
	t_point3 pos1 = point3_create(555.0, 0.0, 0.0);
	t_vec3 u1 = vec3_create(0.0, 555.0, 0.0);
	t_vec3 v1 = vec3_create(0.0, 0.0, 555.0);
	t_quad quad1 = quad_create(&pos1, &u1, &v1, green);
	t_quad *quad1_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad1_copy)
	{
		*quad1_copy = quad1;
		hittable_list_add_nonowned(&world, quad1_copy, set_current_quad,
								   quad_hit_noobj, &quad1.bbox);
	}

	/* Left wall (red) */
	t_point3 pos2 = point3_create(0.0, 0.0, 0.0);
	t_vec3 u2 = vec3_create(0.0, 555.0, 0.0);
	t_vec3 v2 = vec3_create(0.0, 0.0, 555.0);
	t_quad quad2 = quad_create(&pos2, &u2, &v2, red);
	t_quad *quad2_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad2_copy)
	{
		*quad2_copy = quad2;
		hittable_list_add_nonowned(&world, quad2_copy, set_current_quad,
								   quad_hit_noobj, &quad2.bbox);
	}

	/* Ceiling light */
	t_point3 pos3 = point3_create(343.0, 554.0, 332.0);
	t_vec3 u3 = vec3_create(-130.0, 0.0, 0.0);
	t_vec3 v3 = vec3_create(0.0, 0.0, -105.0);
	t_quad quad3 = quad_create(&pos3, &u3, &v3, light);
	t_quad *quad3_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad3_copy)
	{
		*quad3_copy = quad3;
		hittable_list_add_nonowned(&world, quad3_copy, set_current_quad,
								   quad_hit_noobj, &quad3.bbox);
	}

	/* Floor (white) */
	t_point3 pos4 = point3_create(0.0, 0.0, 0.0);
	t_vec3 u4 = vec3_create(555.0, 0.0, 0.0);
	t_vec3 v4 = vec3_create(0.0, 0.0, 555.0);
	t_quad quad4 = quad_create(&pos4, &u4, &v4, white);
	t_quad *quad4_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad4_copy)
	{
		*quad4_copy = quad4;
		hittable_list_add_nonowned(&world, quad4_copy, set_current_quad,
								   quad_hit_noobj, &quad4.bbox);
	}

	/* Ceiling (white) */
	t_point3 pos5 = point3_create(555.0, 555.0, 555.0);
	t_vec3 u5 = vec3_create(-555.0, 0.0, 0.0);
	t_vec3 v5 = vec3_create(0.0, 0.0, -555.0);
	t_quad quad5 = quad_create(&pos5, &u5, &v5, white);
	t_quad *quad5_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad5_copy)
	{
		*quad5_copy = quad5;
		hittable_list_add_nonowned(&world, quad5_copy, set_current_quad,
								   quad_hit_noobj, &quad5.bbox);
	}

	/* Back wall (white) */
	t_point3 pos6 = point3_create(0.0, 0.0, 555.0);
	t_vec3 u6 = vec3_create(555.0, 0.0, 0.0);
	t_vec3 v6 = vec3_create(0.0, 555.0, 0.0);
	t_quad quad6 = quad_create(&pos6, &u6, &v6, white);
	t_quad *quad6_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad6_copy)
	{
		*quad6_copy = quad6;
		hittable_list_add_nonowned(&world, quad6_copy, set_current_quad,
								   quad_hit_noobj, &quad6.bbox);
	}

	/* Add rotated/translated boxes */
	t_point3 box1_min = point3_create(0.0, 0.0, 0.0);
	t_point3 box1_max = point3_create(165.0, 330.0, 165.0);
	t_hittable_list box1_list;
	box_create_list(&box1_min, &box1_max, white, &box1_list);
	t_hittable_wrapper box1_wrap = hittable_list_wrapper(&box1_list);
	t_rotate_y_wrap *box1_rot = rotate_y_create(&box1_wrap, 15.0);
	t_hittable_wrapper box1_rot_wrap = {
		.object = box1_rot,
		.owned = true,
		.set_current = set_current_rotate,
		.hit_noobj = rotate_y_hit_noobj,
		.bbox = box1_rot ? box1_rot->bbox : aabb_empty()};
	t_vec3 box1_offset = vec3_create(265.0, 0.0, 295.0);
	t_translate_wrap *box1_trans = translate_create(&box1_rot_wrap, &box1_offset);
	if (box1_trans)
	{
		hittable_list_add_nonowned(&world, box1_trans, set_current_translate, translate_hit_noobj, &box1_trans->bbox);
	}

	t_point3 box2_min = point3_create(0.0, 0.0, 0.0);
	t_point3 box2_max = point3_create(165.0, 165.0, 165.0);
	t_hittable_list box2_list;
	box_create_list(&box2_min, &box2_max, white, &box2_list);
	t_hittable_wrapper box2_wrap = hittable_list_wrapper(&box2_list);
	t_rotate_y_wrap *box2_rot = rotate_y_create(&box2_wrap, -18.0);
	t_hittable_wrapper box2_rot_wrap = {
		.object = box2_rot,
		.owned = true,
		.set_current = set_current_rotate,
		.hit_noobj = rotate_y_hit_noobj,
		.bbox = box2_rot ? box2_rot->bbox : aabb_empty()};
	t_vec3 box2_offset = vec3_create(130.0, 0.0, 65.0);
	t_translate_wrap *box2_trans = translate_create(&box2_rot_wrap, &box2_offset);
	if (box2_trans)
	{
		hittable_list_add_nonowned(&world, box2_trans, set_current_translate, translate_hit_noobj, &box2_trans->bbox);
	}

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 1.0;
	camera.image_width = 600;
	camera.samples_per_pixel = 200.0;
	camera.max_depth = 50;
	camera.vfov = 40.0;
	camera.lookfrom = point3_create(278.0, 278.0, -800.0);
	camera.lookat = point3_create(278.0, 278.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;
	camera.background = vec3_create(0.0, 0.0, 0.0);

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	red->destroy(red);
	free(red);
	white->destroy(white);
	free(white);
	green->destroy(green);
	free(green);
	light->destroy(light);
	free(light);
	hittable_list_clear(&world);
}

int main(void)
{
	cornell_box();
}