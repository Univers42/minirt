/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:54:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 00:07:19 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUAD_H
#define QUAD_H

#include "common.h"

typedef struct s_quad
{
	t_point3 q;
	t_vec3		u;
	t_vec3		v;
	t_vec3		w;
	t_material *mat;
	t_aabb bbox;
	t_vec3 normal;
	real_t d;
} t_quad;

/* Compute bounding box from quad vertices: Q, Q+u, Q+v, Q+u+v
   Also compute plane normal = unit(cross(u, v)) and d = dot(normal, Q) */
static inline void set_bounding_box(t_quad *quad)
{
	if (!quad)
		return;

	/* compute plane normal and D (store even if degenerate) */
	{
		t_vec3 n = cross(&quad->u, &quad->v);
		t_vec3 n_unit = unit_vector(&n);
		/* If u and v are colinear, unit_vector returns zero; handle gracefully */
		quad->normal = n_unit;
		quad->d = (real_t)dot(&quad->normal, &quad->q);
	}

	/* build four corner points */
	t_point3 p0 = quad->q;
	t_point3 p1 = point3_create(quad->q.x + quad->u.x, quad->q.y + quad->u.y, quad->q.z + quad->u.z);
	t_point3 p2 = point3_create(quad->q.x + quad->v.x, quad->q.y + quad->v.y, quad->q.z + quad->v.z);
	t_point3 p3 = point3_create(quad->q.x + quad->u.x + quad->v.x,
								quad->q.y + quad->u.y + quad->v.y,
								quad->q.z + quad->u.z + quad->v.z);

	/* compute min/max across the four points */
	real_t minx = fmin(fmin(p0.x, p1.x), fmin(p2.x, p3.x));
	real_t miny = fmin(fmin(p0.y, p1.y), fmin(p2.y, p3.y));
	real_t minz = fmin(fmin(p0.z, p1.z), fmin(p2.z, p3.z));

	real_t maxx = fmax(fmax(p0.x, p1.x), fmax(p2.x, p3.x));
	real_t maxy = fmax(fmax(p0.y, p1.y), fmax(p2.y, p3.y));
	real_t maxz = fmax(fmax(p0.z, p1.z), fmax(p2.z, p3.z));

	t_point3 low = point3_create(minx, miny, minz);
	t_point3 high = point3_create(maxx, maxy, maxz);

	quad->bbox = aabb_from_points(&low, &high);
}

/* Create a quad by value (no heap allocation). Caller may store/copy as needed. */
static inline t_quad quad_create(const t_point3 *Q, const t_vec3 *u, const t_vec3 *v, t_material *mat)
{
	t_quad quad;
	if (!Q || !u || !v)
	{
		/* zero initialize on invalid input */
		quad.q = point3_create(0.0, 0.0, 0.0);
		quad.u = vec3_zero();
		quad.v = vec3_zero();
		quad.mat = NULL;
		quad.bbox = aabb_empty();
		quad.normal = vec3_zero();
		quad.d = (real_t)0.0;
		return quad;
	}

	quad.q = *Q;
	quad.u = *u;
	quad.v = *v;
	quad.mat = mat;

	set_bounding_box(&quad);
	return quad;
}

/* Accessor for bounding box */
static inline t_aabb quad_bounding_box(const t_quad *quad)
{
	if (!quad)
		return aabb_empty();
	return quad->bbox;
}

/* Hit test: plane intersection + inside-parallelogram check */
static inline bool quad_hit(const t_quad *quad, const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	/* basic validation */
	if (!quad || !r || !rec)
		return false;

	/* Ray-plane intersection: denom = dot(n, dir) */
	real_t denom = (real_t)dot(&quad->normal, &r->dir);
	const real_t EPS = (real_t)1e-8;
	if (fabsl((long double)denom) < (long double)EPS)
		return false; /* parallel */

	/* t = (d - dot(n, orig)) / denom */
	real_t numer = (real_t)dot(&quad->normal, &r->orig);
	real_t t = (quad->d - numer) / denom;

	/* check interval */
	if (!contains(rayt.min, rayt.max, t))
		return false;

	/* compute intersection point */
	t_vec3 p = ray_at((t_ray *)r, t);

	/* Check if point lies inside the parallelogram defined by Q, u, v.
	   Solve for uu, vv in p = Q + uu*u + vv*v, then require 0 <= uu,vv <= 1. */
	t_vec3 w = vec3_sub(&p, &quad->q);
	real_t u_len_sq = vec3_length_squared(&quad->u);
	real_t v_len_sq = vec3_length_squared(&quad->v);
	if (u_len_sq <= (real_t)0.0 || v_len_sq <= (real_t)0.0)
		return false; /* degenerate quad */

	real_t uu = (real_t)dot(&w, &quad->u) / u_len_sq;
	real_t vv = (real_t)dot(&w, &quad->v) / v_len_sq;

	/* allow tiny epsilon tolerance */
	const real_t BIAS = (real_t)1e-8;
	if (!(uu + BIAS >= (real_t)0.0 && uu <= (real_t)1.0 + BIAS && vv + BIAS >= (real_t)0.0 && vv <= (real_t)1.0 + BIAS))
		return false;

	/* fill hit record */
	rec->t = t;
	rec->p = p;
	rec->u = uu;
	rec->v = vv;
	rec->mat = quad->mat;
	set_face_normal(rec, r, &quad->normal);

	return true;
}

#endif