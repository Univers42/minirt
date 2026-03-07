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
#include <sys/stat.h>
#include <sys/types.h>

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

t_ray	get_ray_stratified(const t_camera *cam, int i, int j,
			int s_i, int s_j)
{
	t_vec3	offset;
	t_vec3	tmp_u;
	t_vec3	tmp_v;
	t_vec3	pixel_sample;
	t_vec3	ray_origin;
	t_vec3	ray_direction;

	offset = sample_square_stratified(s_i, s_j, cam->recip_sqrt_spp);
	tmp_u = vec3_mul_scalar(&cam->pixel_delta_u, (real_t)(i + offset.x));
	tmp_v = vec3_mul_scalar(&cam->pixel_delta_v, (real_t)(j + offset.y));
	pixel_sample = vec3_add(&cam->pixel00_loc, &tmp_u);
	pixel_sample = vec3_add(&pixel_sample, &tmp_v);
	if (cam->defocus_angle <= (real_t)0.0)
		ray_origin = cam->center;
	else
		ray_origin = defocus_disk_sample(cam);
	ray_direction = vec3_sub(&pixel_sample, &ray_origin);
	return (ray_create(ray_origin, ray_direction, random_real()));
}

t_ray	get_ray(const t_camera *cam, int i, int j)
{
	return (get_ray_stratified(cam, i, j, 0, 0));
}

void	ensure_output_dir(void)
{
	const char	*output_dir;
	struct stat	st;

	output_dir = "images";
	if (stat(output_dir, &st) == -1)
		mkdir(output_dir, 0755);
}

void	get_output_filename(char *filename, size_t size,
			const char *base_name)
{
	ensure_output_dir();
	snprintf(filename, size, "images/%s.ppm", base_name);
}
