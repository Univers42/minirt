/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "studio_config.h"
#include <math.h>

static void	camera_init_basis(t_camera *cam)
{
	t_vec3	dir;
	t_vec3	cross_vup_w;
	real_t	defocus_radius;

	dir = vec3_sub(&cam->lookat, &cam->lookfrom);
	cam->w = unit_vector(&dir);
	cam->w = vec3_neg(&cam->w);
	cross_vup_w = cross(&cam->vup, &cam->w);
	cam->u = unit_vector(&cross_vup_w);
	cam->v = cross(&cam->w, &cam->u);
	defocus_radius = cam->focus_dist
		* (real_t)tan((double)degrees_to_radians(cam->defocus_angle
				/ (real_t)2.0));
	cam->defocus_disk_u = vec3_mul_scalar(&cam->u, defocus_radius);
	cam->defocus_disk_v = vec3_mul_scalar(&cam->v, defocus_radius);
}

/* Resolve image dimensions + aspect ratio from the given/defaults. */
static void	camera_init_dims(t_camera *cam, real_t aspect_ratio,
				int image_width)
{
	if (aspect_ratio > 0)
		cam->aspect_ratio = aspect_ratio;
	else
		cam->aspect_ratio = (real_t)1.0;
	if (image_width > 0)
		cam->image_width = image_width;
	else
		cam->image_width = 100;
	if (cam->image_height > 0)
		cam->aspect_ratio = (real_t)cam->image_width
			/ (real_t)cam->image_height;
	else
	{
		cam->image_height = (int)((real_t)cam->image_width
				/ cam->aspect_ratio);
		if (cam->image_height < 1)
			cam->image_height = 1;
	}
}

/* Derive stratified sampling counts from samples_per_pixel. */
static void	camera_init_spp(t_camera *cam)
{
	if (cam->samples_per_pixel <= (real_t)0.0)
		cam->samples_per_pixel = (real_t)1.0;
	cam->sqrt_spp = (int)sqrt((double)cam->samples_per_pixel);
	cam->recip_sqrt_spp = (real_t)1.0 / (real_t)cam->sqrt_spp;
	cam->pixel_samples_scale = (real_t)1.0
		/ (cam->sqrt_spp * cam->sqrt_spp);
}

void	camera_init(t_camera *cam, real_t aspect_ratio, int image_width)
{
	if (!cam)
		return ;
	if (cam->max_depth <= 0)
		cam->max_depth = 50;
	camera_init_dims(cam, aspect_ratio, image_width);
	cam->center = cam->lookfrom;
	camera_init_basis(cam);
	camera_init_spp(cam);
	camera_init_viewport(cam);
}
