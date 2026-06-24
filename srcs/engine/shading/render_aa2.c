/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_aa2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 04:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 04:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shading.h"
#include "camera.h"
#include "interval.h"
#include "studio_config.h"
#include <math.h>

/* Linear BT.709 luma of a pixel colour (matches color.c saturation). */
real_t	aa_luma(const t_vec3 *p)
{
	return ((real_t)0.2126 * p->x + (real_t)0.7152 * p->y
		+ (real_t)0.0722 * p->z);
}

/* Primary ray through pixel (i,j) offset by (dx,dy) sub-pixel units.  */
static t_ray	aa_offset_ray(const t_camera *cam, int i, int j, t_vec3 off)
{
	t_vec3	u;
	t_vec3	v;
	t_vec3	sample;
	t_vec3	dir;

	u = vec3_mul_scalar(&cam->pixel_delta_u, (real_t)i + off.x);
	v = vec3_mul_scalar(&cam->pixel_delta_v, (real_t)j + off.y);
	sample = vec3_add(&cam->pixel00_loc, &u);
	sample = vec3_add(&sample, &v);
	dir = vec3_sub(&sample, &cam->center);
	return (ray_create(cam->center, dir, (real_t)0.0));
}

/* Average a fixed rotated 2x2 grid of deterministic sub-samples.      */
t_vec3	aa_supersample(const t_camera *cam,
			const t_hittable_list *world, int x, int y)
{
	static const t_vec3	off[4] = {{0.375, 0.125, 0.0}, {0.875, 0.375, 0.0},
	{0.125, 0.625, 0.0}, {0.625, 0.875, 0.0}};
	t_vec3				acc;
	t_vec3				sc;
	t_ray				r;
	int					k;

	acc = vec3_zero();
	k = 0;
	while (k < 4)
	{
		r = aa_offset_ray(cam, x, y, off[k]);
		sc = ray_color_direct(&r, world, RT_FAST_MAX_DEPTH, &cam->background);
		acc = vec3_add(&acc, &sc);
		k++;
	}
	return (vec3_mul_scalar(&acc, (real_t)0.25));
}

/* True if pixel luma differs from right/down neighbour past threshold. */
int	aa_is_edge(const real_t *lum, int x, int y, int w)
{
	real_t	c;

	c = lum[y * w + x];
	if (fabs(c - lum[y * w + x + 1]) > (real_t)RT_AA_EDGE_THRESH)
		return (1);
	if (fabs(c - lum[(y + 1) * w + x]) > (real_t)RT_AA_EDGE_THRESH)
		return (1);
	return (0);
}
