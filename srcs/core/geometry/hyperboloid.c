/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hyperboloid.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hyperboloid.h"
#include "interval.h"
#include <math.h>

/* hyperboloid2.c */
bool	hyperboloid_in_band(const t_hyperboloid *hy, const t_ray *r, real_t t);
void	hyperboloid_set_record(const t_hyperboloid *hy, const t_ray *r,
			real_t t, t_hit_record *rec);

/* AABB: widest rim radius rmax (at the caps) swept over the full height.
   rmax^2 = (1 + half_h^2/c^2) / inv_a2 from F = 0 at y = +/- half_h. */
static void	hyperboloid_init_bbox(t_hyperboloid *hy)
{
	real_t		rmax;
	t_vec3		off;
	t_point3	top;
	t_point3	bot;

	rmax = (real_t)sqrt((double)(((real_t)1.0
					+ hy->half_h * hy->half_h * hy->inv_c2) / hy->inv_a2));
	off = vec3_mul_scalar(&hy->axis, hy->half_h);
	top = vec3_add(&hy->center, &off);
	bot = vec3_sub(&hy->center, &off);
	hy->bbox = aabb_from_points(
			&(t_point3){fmin(top.x, bot.x) - rmax, fmin(top.y, bot.y) - rmax,
			fmin(top.z, bot.z) - rmax},
			&(t_point3){fmax(top.x, bot.x) + rmax, fmax(top.y, bot.y) + rmax,
			fmax(top.z, bot.z) + rmax});
}

t_hyperboloid	hyperboloid_create(const t_point3 *center, const t_vec3 *axis,
		const t_shape_dims *dims, t_material *mat)
{
	t_hyperboloid	hy;
	real_t			a;
	real_t			c;

	hy.center = *center;
	hy.axis = unit_vector(axis);
	if (dims->height > (real_t)0.0)
		hy.half_h = dims->height / (real_t)2.0;
	else
		hy.half_h = (real_t)0.5;
	a = dims->size / (real_t)2.0;
	if (a <= (real_t)0.0)
		a = (real_t)0.5;
	c = hy.half_h;
	hy.inv_a2 = (real_t)1.0 / (a * a);
	hy.inv_c2 = (real_t)1.0 / (c * c);
	hy.mat = mat;
	hyperboloid_init_bbox(&hy);
	return (hy);
}

/* Quadratic coefficients of F(o + t*D) = a*t^2 + b*t + c with oc = o - center,
   dd = dot(D,axis), oa = dot(oc,axis), kk = inv_a2 + inv_c2.  out = {a,b,c}. */
static void	hyperboloid_coeffs(const t_hyperboloid *hy, const t_ray *r,
		t_vec3 *oc, real_t out[3])
{
	real_t	dd;
	real_t	oa;
	real_t	kk;

	*oc = vec3_sub(&r->orig, &hy->center);
	dd = dot(&r->dir, &hy->axis);
	oa = dot(oc, &hy->axis);
	kk = hy->inv_a2 + hy->inv_c2;
	out[0] = hy->inv_a2 * dot(&r->dir, &r->dir) - kk * dd * dd;
	out[1] = (real_t)2.0 * (hy->inv_a2 * dot(oc, &r->dir) - kk * oa * dd);
	out[2] = hy->inv_a2 * dot(oc, oc) - kk * oa * oa - (real_t)1.0;
}

/* Nearest valid root of a*t^2+b*t+c=0 inside (rayt) and the axial band.
   Returns the root, or a sentinel above rayt.max when none is valid. */
static real_t	hyperboloid_nearest(const t_hyperboloid *hy, const t_ray *r,
		const real_t abc[3], t_interval rayt)
{
	real_t	sq;
	real_t	roots[2];
	int		i;

	sq = (real_t)sqrt((double)(abc[1] * abc[1]
				- (real_t)4.0 * abc[0] * abc[2]));
	roots[0] = (-abc[1] - sq) / ((real_t)2.0 * abc[0]);
	roots[1] = (-abc[1] + sq) / ((real_t)2.0 * abc[0]);
	i = -1;
	while (++i < 2)
		if (contains(rayt.min, rayt.max, roots[i])
			&& hyperboloid_in_band(hy, r, roots[i]))
			return (roots[i]);
	return (rayt.max + (real_t)1.0);
}

bool	hyperboloid_hit(const t_hyperboloid *hy, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_vec3	oc;
	real_t	abc[3];
	real_t	disc;
	real_t	t;

	if (!hy || !r || !rec)
		return (false);
	hyperboloid_coeffs(hy, r, &oc, abc);
	if (fabs((double)abc[0]) < 1e-12)
		return (false);
	disc = abc[1] * abc[1] - (real_t)4.0 * abc[0] * abc[2];
	if (disc < (real_t)0.0)
		return (false);
	t = hyperboloid_nearest(hy, r, abc, rayt);
	if (!contains(rayt.min, rayt.max, t))
		return (false);
	hyperboloid_set_record(hy, r, t, rec);
	return (true);
}
