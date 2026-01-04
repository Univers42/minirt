/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:54:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 00:36:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUAD_H
#define QUAD_H

#include "common.h"

typedef struct s_quad
{
	t_point3 q;
	t_vec3 u;
	t_vec3 v;
	t_vec3 w;
	t_material *mat;
	t_aabb bbox;
	t_vec3 normal;
	real_t d;
} t_quad;

/* Compute bounding box from quad vertices: Q, Q+u, Q+v, Q+u+v
   Also compute plane normal = unit(cross(u, v)), w = cross(u,v), and d = dot(normal, Q) */
static inline void set_bounding_box(t_quad *quad)
{
	if (!quad)
		return;

	/* compute plane normal, w vector, and D */
	{
		t_vec3 n = cross(&quad->u, &quad->v);
		quad->w = n; /* store raw cross product for plane coordinate calculations */
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
		quad.w = vec3_zero();
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

/* Check if plane coordinates (alpha, beta) are interior to the quad and set rec UV */
static inline bool quad_is_interior(real_t alpha, real_t beta, t_hit_record *rec)
{
	/* Check if both coordinates are in [0, 1] */
	if (!contains((real_t)0.0, (real_t)1.0, alpha) || !contains((real_t)0.0, (real_t)1.0, beta))
		return false;

	/* Set UV coordinates in hit record */
	rec->u = alpha;
	rec->v = beta;
	return true;
}

/* Hit test: plane intersection + plane-coordinate check */
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

	/* Compute plane coordinates using cross products and dot products
	   alpha = dot(w, cross(planar_hitpt_vector, v)) / |w|^2
	   beta  = dot(w, cross(u, planar_hitpt_vector)) / |w|^2 */
	t_vec3 planar_hitpt_vector = vec3_sub(&p, &quad->q);
	real_t w_len_sq = vec3_length_squared(&quad->w);
	if (w_len_sq <= (real_t)0.0)
		return false; /* degenerate quad */

	t_vec3 cross_phv_v = cross(&planar_hitpt_vector, &quad->v);
	real_t alpha = (real_t)dot(&quad->w, &cross_phv_v) / w_len_sq;

	t_vec3 cross_u_phv = cross(&quad->u, &planar_hitpt_vector);
	real_t beta = (real_t)dot(&quad->w, &cross_u_phv) / w_len_sq;

	/* Check if point is interior and set UV; reject if exterior */
	if (!quad_is_interior(alpha, beta, rec))
		return false;

	/* Ray hits the 2D shape; set the rest of the hit record and return true */
	rec->t = t;
	rec->p = p;
	rec->mat = quad->mat;
	set_face_normal(rec, r, &quad->normal);

	return true;
}

#endif