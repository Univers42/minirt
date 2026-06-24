/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 20:13:18 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"

t_vec3	defocus_disk_sample(const t_camera *cam)
{
	t_vec3	p;
	t_vec3	p0_term;
	t_vec3	p1_term;
	t_vec3	offset;

	p = random_in_unit_disk();
	p0_term = vec3_mul_scalar(&cam->defocus_disk_u, p.x);
	p1_term = vec3_mul_scalar(&cam->defocus_disk_v, p.y);
	offset = vec3_add(&p0_term, &p1_term);
	return (vec3_add(&cam->center, &offset));
}

/* Pixel-sample world location for stratified sub-sample (px->s_i, px->s_j). */
static t_vec3	pixel_sample_loc(const t_camera *cam, const t_pix *px)
{
	t_vec3	offset;
	t_vec3	tmp_u;
	t_vec3	tmp_v;
	t_vec3	loc;

	offset = sample_square_stratified(px->s_i, px->s_j, cam->recip_sqrt_spp);
	tmp_u = vec3_mul_scalar(&cam->pixel_delta_u, (real_t)(px->i + offset.x));
	tmp_v = vec3_mul_scalar(&cam->pixel_delta_v, (real_t)(px->j + offset.y));
	loc = vec3_add(&cam->pixel00_loc, &tmp_u);
	return (vec3_add(&loc, &tmp_v));
}

t_ray	get_ray_stratified(const t_camera *cam, const t_pix *px)
{
	t_vec3	pixel_sample;
	t_vec3	ray_origin;
	t_vec3	ray_direction;

	pixel_sample = pixel_sample_loc(cam, px);
	if (cam->defocus_angle <= (real_t)0.0)
		ray_origin = cam->center;
	else
		ray_origin = defocus_disk_sample(cam);
	ray_direction = vec3_sub(&pixel_sample, &ray_origin);
	return (ray_create(ray_origin, ray_direction, random_real()));
}

t_ray	get_ray(const t_camera *cam, int i, int j)
{
	t_pix	px;

	px = (t_pix){i, j, 0, 0};
	return (get_ray_stratified(cam, &px));
}
