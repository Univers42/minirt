/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
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

static void	camera_init_viewport(t_camera *cam)
{
	real_t	theta;
	real_t	h;
	real_t	vp_h;
	real_t	vp_w;
	t_vec3	vp_u;
	t_vec3	vp_v;
	t_vec3	neg_v;
	t_vec3	focal_w;
	t_vec3	upper_left;
	t_vec3	sum;

	theta = degrees_to_radians(cam->vfov);
	h = (real_t)tan((double)theta / 2.0);
	vp_h = (real_t)(2.0 * h * cam->focus_dist);
	vp_w = vp_h * ((real_t)cam->image_width / (real_t)cam->image_height);
	vp_u = vec3_mul_scalar(&cam->u, vp_w);
	neg_v = vec3_neg(&cam->v);
	vp_v = vec3_mul_scalar(&neg_v, vp_h);
	cam->pixel_delta_u = vec3_div_scalar(&vp_u, (real_t)cam->image_width);
	cam->pixel_delta_v = vec3_div_scalar(&vp_v, (real_t)cam->image_height);
	focal_w = vec3_mul_scalar(&cam->w, cam->focus_dist);
	upper_left = vec3_sub(&cam->center, &focal_w);
	sum = vec3_div_scalar(&vp_u, (real_t)2.0);
	upper_left = vec3_sub(&upper_left, &sum);
	sum = vec3_div_scalar(&vp_v, (real_t)2.0);
	upper_left = vec3_sub(&upper_left, &sum);
	sum = vec3_add(&cam->pixel_delta_u, &cam->pixel_delta_v);
	sum = vec3_mul_scalar(&sum, (real_t)0.5);
	cam->pixel00_loc = vec3_add(&upper_left, &sum);
}

void	camera_init(t_camera *cam, real_t aspect_ratio, int image_width)
{
	if (!cam)
		return ;
	cam->max_depth = 50;
	if (aspect_ratio > 0)
		cam->aspect_ratio = aspect_ratio;
	else
		cam->aspect_ratio = (real_t)1.0;
	if (image_width > 0)
		cam->image_width = image_width;
	else
		cam->image_width = 100;
	cam->image_height = (int)((real_t)cam->image_width / cam->aspect_ratio);
	if (cam->image_height < 1)
		cam->image_height = 1;
	cam->center = cam->lookfrom;
	camera_init_basis(cam);
	if (cam->samples_per_pixel <= (real_t)0.0)
		cam->samples_per_pixel = (real_t)1.0;
	cam->sqrt_spp = (int)sqrt((double)cam->samples_per_pixel);
	cam->recip_sqrt_spp = (real_t)1.0 / (real_t)cam->sqrt_spp;
	cam->pixel_samples_scale = (real_t)1.0
		/ (cam->sqrt_spp * cam->sqrt_spp);
	camera_init_viewport(cam);
}

t_vec3	sample_square(void)
{
	return (vec3_create(random_real() - (real_t)0.5,
			random_real() - (real_t)0.5, (real_t)0.0));
}

t_vec3	sample_square_stratified(int s_i, int s_j, real_t recip_sqrt_spp)
{
	real_t	px;
	real_t	py;

	px = ((real_t)s_i + random_real()) * recip_sqrt_spp - (real_t)0.5;
	py = ((real_t)s_j + random_real()) * recip_sqrt_spp - (real_t)0.5;
	return (vec3_create(px, py, (real_t)0.0));
}
