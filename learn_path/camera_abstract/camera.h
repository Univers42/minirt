/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 23:53:07 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 00:53:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include "types.h"
#include "point.h"
#include "vector.h"
#include "ray.h"
#include "hittable_list.h"

/* Camera type */
typedef struct s_camera
{
	real_t aspect_ratio;
	int image_width;
	int image_height;
	t_point3 center;
	t_point3 pixel00_loc;
	t_vec3 pixel_delta_u;
	t_vec3 pixel_delta_v;
} t_camera;

/* Initialize camera in-place */
static inline void camera_init(t_camera *camera, real_t aspect_ratio, int image_width)
{
	real_t focal_length;
	real_t viewport_height;
	real_t viewport_width;
	t_vec3 viewport_u;
	t_vec3 viewport_v;
	t_vec3 viewport_upper_left;
	t_vec3 tmp;

	if (!camera)
		return;

	camera->aspect_ratio = (aspect_ratio > 0) ? aspect_ratio : (real_t)1.0;
	camera->image_width = (image_width > 0) ? image_width : 100;
	camera->image_height = (int)((real_t)camera->image_width / camera->aspect_ratio);
	if (camera->image_height < 1)
		camera->image_height = 1;

	camera->center = point3_create(0.0, 0.0, 0.0);

	focal_length = 1.0;
	viewport_height = 2.0;
	viewport_width = viewport_height * ((real_t)camera->image_width / (real_t)camera->image_height);

	viewport_u = vec3_create(viewport_width, 0.0, 0.0);
	viewport_v = vec3_create(0.0, -viewport_height, 0.0);

	camera->pixel_delta_u = vec3_div_scalar(&viewport_u, (real_t)camera->image_width);
	camera->pixel_delta_v = vec3_div_scalar(&viewport_v, (real_t)camera->image_height);

	/* viewport_upper_left = center - vec3(0,0,focal_length) - viewport_u/2 - viewport_v/2; */
	tmp = vec3_create(0.0, 0.0, focal_length);
	viewport_upper_left = vec3_sub(&camera->center, &tmp);
	t_vec3 half_u = vec3_div_scalar(&viewport_u, (real_t)2.0);
	t_vec3 half_v = vec3_div_scalar(&viewport_v, (real_t)2.0);
	viewport_upper_left = vec3_sub(&viewport_upper_left, &half_u);
	viewport_upper_left = vec3_sub(&viewport_upper_left, &half_v);

	/* pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v) */
	t_vec3 sum = vec3_add(&camera->pixel_delta_u, &camera->pixel_delta_v);
	t_vec3 half_sum = vec3_mul_scalar(&sum, (real_t)0.5);
	camera->pixel00_loc = vec3_add(&viewport_upper_left, &half_sum);
}

/* Render function: stream PPM output to `out` using given world */
static inline void camera_render(const t_camera *camera, FILE *out, const t_hittable_list *world)
{
	if (!camera || !out)
		return;

	fprintf(out, "P3\n%d %d\n255\n", camera->image_width, camera->image_height);

	for (int j = 0; j < camera->image_height; ++j)
	{
		fprintf(stderr, "\rScanlines remaining: %d ", (camera->image_height - j));
		fflush(stderr);
		for (int i = 0; i < camera->image_width; ++i)
		{
			t_vec3 du = vec3_mul_scalar(&camera->pixel_delta_u, (real_t)i);
			t_vec3 dv = vec3_mul_scalar(&camera->pixel_delta_v, (real_t)j);
			t_vec3 tmp = vec3_add(&du, &dv);
			t_vec3 pixel_center = vec3_add((const t_vec3 *)&camera->pixel00_loc, &tmp);
			t_vec3 ray_dir = vec3_sub(&pixel_center, (const t_vec3 *)&camera->center);
			t_ray r = ray_create((t_vec3)camera->center, ray_dir);
			t_vec3 pixel_color = ray_color_world(&r, world);
			write_color(out, &pixel_color);
		}
	}
	fprintf(stderr, "\rDone.                 \n");
}

#endif