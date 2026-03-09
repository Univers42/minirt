/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:43:36 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:14:57 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "aabb.h"

int	aabb_longest_axis(const t_aabb *box)
{
	real_t	x_size;
	real_t	y_size;
	real_t	z_size;

	x_size = interval_size(&box->x);
	y_size = interval_size(&box->y);
	z_size = interval_size(&box->z);
	if (x_size > y_size)
	{
		if (x_size > z_size)
			return (0);
		return (2);
	}
	if (y_size > z_size)
		return (1);
	return (2);
}

static void	aabb_hit_slab(real_t slab_min, real_t slab_max, real_t orig,
		real_t inv_dir, t_interval *ray_t)
{
	real_t	t0;
	real_t	t1;
	real_t	tmp;

	t0 = (slab_min - orig) * inv_dir;
	t1 = (slab_max - orig) * inv_dir;
	if (t0 > t1)
	{
		tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
	if (t0 > ray_t->min)
		ray_t->min = t0;
	if (t1 < ray_t->max)
		ray_t->max = t1;
}

bool	aabb_hit(const t_aabb *box, const t_ray *r, t_interval *ray_t)
{
	if (!box || !r || !ray_t)
		return (false);
	aabb_hit_slab(box->x.min, box->x.max, r->orig.x,
		(real_t)1.0 / r->dir.x, ray_t);
	if (ray_t->max <= ray_t->min)
		return (false);
	aabb_hit_slab(box->y.min, box->y.max, r->orig.y,
		(real_t)1.0 / r->dir.y, ray_t);
	if (ray_t->max <= ray_t->min)
		return (false);
	aabb_hit_slab(box->z.min, box->z.max, r->orig.z,
		(real_t)1.0 / r->dir.z, ray_t);
	if (ray_t->max <= ray_t->min)
		return (false);
	return (true);
}

t_aabb	aabb_add_vec3(const t_aabb *box, const t_vec3 *offset)
{
	t_interval	nx;
	t_interval	ny;
	t_interval	nz;

	nx = interval_add(&box->x, offset->x);
	ny = interval_add(&box->y, offset->y);
	nz = interval_add(&box->z, offset->z);
	return (aabb_from_intervals(&nx, &ny, &nz));
}
