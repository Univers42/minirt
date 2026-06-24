/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera5.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include <math.h>

/* Compute viewport edge vectors vp[0]=u, vp[1]=v and the per-pixel deltas. */
static void	camera_viewport_vectors(t_camera *cam, t_vec3 vp[2])
{
	real_t	h;
	real_t	vp_h;
	real_t	vp_w;
	t_vec3	neg_v;

	h = (real_t)tan((double)degrees_to_radians(cam->vfov) / 2.0);
	vp_h = (real_t)(2.0 * h * cam->focus_dist);
	vp_w = vp_h * ((real_t)cam->image_width / (real_t)cam->image_height);
	vp[0] = vec3_mul_scalar(&cam->u, vp_w);
	neg_v = vec3_neg(&cam->v);
	vp[1] = vec3_mul_scalar(&neg_v, vp_h);
	cam->pixel_delta_u = vec3_div_scalar(&vp[0], (real_t)cam->image_width);
	cam->pixel_delta_v = vec3_div_scalar(&vp[1], (real_t)cam->image_height);
}

void	camera_init_viewport(t_camera *cam)
{
	t_vec3	vp[2];
	t_vec3	focal_w;
	t_vec3	upper_left;
	t_vec3	sum;

	camera_viewport_vectors(cam, vp);
	focal_w = vec3_mul_scalar(&cam->w, cam->focus_dist);
	upper_left = vec3_sub(&cam->center, &focal_w);
	sum = vec3_div_scalar(&vp[0], (real_t)2.0);
	upper_left = vec3_sub(&upper_left, &sum);
	sum = vec3_div_scalar(&vp[1], (real_t)2.0);
	upper_left = vec3_sub(&upper_left, &sum);
	sum = vec3_add(&cam->pixel_delta_u, &cam->pixel_delta_v);
	sum = vec3_mul_scalar(&sum, (real_t)0.5);
	cam->pixel00_loc = vec3_add(&upper_left, &sum);
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
