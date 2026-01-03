/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 23:53:07 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 14:44:50 by dlesieur         ###   ########.fr       */
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
#include "random.h"
#include "color.h"

/* Camera type */
typedef struct s_camera
{
	real_t aspect_ratio;
	real_t samples_per_pixel;
	real_t pixel_samples_scale;
	int image_width;
	int image_height;
	int max_depth; /* new: maximum path depth */
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

	/* Ensure max_depth has a safe default to avoid reading uninitialized memory
	   (caller may or may not have initialized camera->max_depth). */
	camera->max_depth = 50;

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

	/* ensure samples_per_pixel is sane and compute pixel_samples_scale last */
	if (camera->samples_per_pixel <= (real_t)0.0)
		camera->samples_per_pixel = (real_t)1.0;
	camera->pixel_samples_scale = (real_t)1.0 / camera->samples_per_pixel;

	/* pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v) */
	t_vec3 sum = vec3_add(&camera->pixel_delta_u, &camera->pixel_delta_v);
	t_vec3 half_sum = vec3_mul_scalar(&sum, (real_t)0.5);
	camera->pixel00_loc = vec3_add(&viewport_upper_left, &half_sum);
}

/* Return a random point in the [-0.5,-0.5] .. [+0.5,+0.5] square (z = 0) */
static inline t_vec3 sample_square(void)
{
	return vec3_create(random_real() - (real_t)0.5, random_real() - (real_t)0.5, (real_t)0.0);
}

/* construct a camera ray originating from the camera center and directed at a
   randomly sampled point around pixel (i,j) */
static inline t_ray get_ray(const t_camera *cam, int i, int j)
{
	t_vec3 offset = sample_square();

	/* tmp_u = (i + offset.x) * pixel_delta_u */
	t_vec3 tmp_u = vec3_mul_scalar(&cam->pixel_delta_u, (real_t)(i + offset.x));
	/* tmp_v = (j + offset.y) * pixel_delta_v */
	t_vec3 tmp_v = vec3_mul_scalar(&cam->pixel_delta_v, (real_t)(j + offset.y));

	t_vec3 tmp = vec3_add(&cam->pixel00_loc, &tmp_u);
	t_vec3 pixel_sample = vec3_add(&tmp, &tmp_v);

	/* subtract camera center (no cast) */
	t_vec3 ray_direction = vec3_sub(&pixel_sample, &cam->center);
	return ray_create(cam->center, ray_direction);
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
			t_color pixel_color = vec3_zero();
			for (int sample = 0; sample < (int)camera->samples_per_pixel; ++sample)
			{
				t_ray r = get_ray(camera, i, j);
				/* use camera-controlled max depth */
				t_vec3 sample_color = ray_color_depth(&r, world, camera->max_depth);
				pixel_color = vec3_add(&pixel_color, &sample_color);
			}
			t_vec3 scaled = vec3_mul_scalar(&pixel_color, camera->pixel_samples_scale);
			/* debug: print central pixel computed color once */
			if (i == camera->image_width / 2 && j == camera->image_height / 2)
			{
				fprintf(stderr, "\nDEBUG: central pixel (i=%d,j=%d) accumulated = (%f,%f,%f) scaled = (%f,%f,%f)\n",
						i, j,
						pixel_color.x, pixel_color.y, pixel_color.z,
						scaled.x, scaled.y, scaled.z);
			}
			write_color(out, &scaled);
		}
	}
	fprintf(stderr, "\rDone.                 \n");
}

#endif