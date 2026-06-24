/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraboloid.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "paraboloid.h"
#include "interval.h"
#include <math.h>

/* paraboloid2.c */
bool	paraboloid_in_band(const t_paraboloid *pb, const t_ray *r, real_t t);
void	paraboloid_set_record(const t_paraboloid *pb, const t_ray *r,
			real_t t, t_hit_record *rec);

/* AABB: rim disk of radius sqrt(k*height) swept from vertex to the top.
   A conservative box of vertex +/- radius is used on every axis, plus the
   axial extent so the bowl's full height is covered. */
static void	paraboloid_init_bbox(t_paraboloid *pb)
{
	real_t		radius;
	t_vec3		top_off;
	t_point3	top;
	t_point3	low;
	t_point3	high;

	radius = (real_t)sqrt((double)(pb->k * pb->height));
	top_off = vec3_mul_scalar(&pb->axis, pb->height);
	top = vec3_add(&pb->vertex, &top_off);
	low = point3_create(fmin(pb->vertex.x, top.x) - radius,
			fmin(pb->vertex.y, top.y) - radius,
			fmin(pb->vertex.z, top.z) - radius);
	high = point3_create(fmax(pb->vertex.x, top.x) + radius,
			fmax(pb->vertex.y, top.y) + radius,
			fmax(pb->vertex.z, top.z) + radius);
	pb->bbox = aabb_from_points(&low, &high);
}

t_paraboloid	paraboloid_create(const t_point3 *vertex, const t_vec3 *axis,
		const t_shape_dims *dims, t_material *mat)
{
	t_paraboloid	pb;
	real_t			radius;

	pb.vertex = *vertex;
	pb.axis = unit_vector(axis);
	if (dims->height > (real_t)0.0)
		pb.height = dims->height;
	else
		pb.height = (real_t)1.0;
	radius = dims->size / (real_t)2.0;
	pb.k = (radius * radius) / pb.height;
	pb.mat = mat;
	paraboloid_init_bbox(&pb);
	return (pb);
}

/* Quadratic coefficients of F(o + t*D) = a*t^2 + b*t + c, with oc = o - vertex,
   dd = dot(D,axis), oa = dot(oc,axis).  out = {a, b, c}. */
static void	paraboloid_coeffs(const t_paraboloid *pb, const t_ray *r,
		t_vec3 *oc, real_t out[3])
{
	real_t	dd;
	real_t	oa;

	*oc = vec3_sub(&r->orig, &pb->vertex);
	dd = dot(&r->dir, &pb->axis);
	oa = dot(oc, &pb->axis);
	out[0] = dot(&r->dir, &r->dir) - dd * dd;
	out[1] = (real_t)2.0 * dot(oc, &r->dir)
		- (real_t)2.0 * oa * dd - pb->k * dd;
	out[2] = dot(oc, oc) - oa * oa - pb->k * oa;
}

/* Nearest valid root of a*t^2+b*t+c=0 inside (rayt) and the axial band.
   Returns the root, or a sentinel above rayt.max when none is valid. */
static real_t	paraboloid_nearest(const t_paraboloid *pb, const t_ray *r,
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
			&& paraboloid_in_band(pb, r, roots[i]))
			return (roots[i]);
	return (rayt.max + (real_t)1.0);
}

bool	paraboloid_hit(const t_paraboloid *pb, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_vec3	oc;
	real_t	abc[3];
	real_t	disc;
	real_t	t;

	if (!pb || !r || !rec)
		return (false);
	paraboloid_coeffs(pb, r, &oc, abc);
	if (fabs((double)abc[0]) < 1e-12)
		return (false);
	disc = abc[1] * abc[1] - (real_t)4.0 * abc[0] * abc[2];
	if (disc < (real_t)0.0)
		return (false);
	t = paraboloid_nearest(pb, r, abc, rayt);
	if (!contains(rayt.min, rayt.max, t))
		return (false);
	paraboloid_set_record(pb, r, t, rec);
	return (true);
}
