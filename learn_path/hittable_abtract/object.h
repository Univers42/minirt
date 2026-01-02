/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 17:49:10 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 20:49:59 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJECT_H
#define OBJECT_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "point.h"
#include "hittable.h"
#include <math.h>
# include <stdbool.h>

/* Sphere type uses t_vec3 center so vector ops work uniformly. */
typedef struct s_sphere
{
	t_vec3 center;
	real_t radius;
} t_sphere;

static inline t_sphere create_sphere(const t_point3 *center, real_t radius)
{
	t_sphere s;
	s.center = vec3_create(center->x, center->y, center->z);
	s.radius = radius;
	return s;
}

/* simple t-only test used by earlier code paths */
static inline double hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r)
{
	t_vec3 oc = vec3_sub(center, &r->orig); /* center - origin */
	real_t a = vec3_length_squared(&r->dir);
	real_t h = (real_t)dot(&r->dir, &oc);
	real_t c = vec3_length_squared(&oc) - radius * radius;
	double discriminant = (double)h * (double)h - (double)a * (double)c;
	if (discriminant < 0.0)
		return -1.0;
	double root = sqrt(discriminant);
	return ((double)h - root) / (double)a;
}

/* Module-local current sphere for no-obj 4-arg hit calls */
static const t_sphere *g_current_sphere = NULL;
static inline void set_current_sphere(const void *obj) { g_current_sphere = (const t_sphere *)obj; }

/* 4-arg sphere hit using the currently-bound sphere */
static inline bool sphere_hit_noobj(const t_ray *r, real_t ray_tmin, real_t ray_tmax, t_hit_record *rec)
{
	const t_sphere *s = g_current_sphere;
	if (!s)
		return false;
	t_vec3 oc = vec3_sub(&r->orig, &s->center);
	real_t a = vec3_length_squared(&r->dir);
	double half_b = dot(&r->dir, &oc);
	double c = (double)vec3_length_squared(&oc) - (double)s->radius * (double)s->radius;
	double discriminant = half_b * half_b - (double)a * c;
	if (discriminant < 0.0)
		return false;
	double sqrtd = sqrt(discriminant);
	double root = (-half_b - sqrtd) / (double)a;
	if (root <= (double)ray_tmin || (double)ray_tmax <= root)
	{
		root = (-half_b + sqrtd) / (double)a;
		if (root <= (double)ray_tmin || (double)ray_tmax <= root)
			return false;
	}
	rec->t = (real_t)root;
	rec->p = ray_at((t_ray *)r, rec->t);
	t_vec3 tmp = vec3_sub(&rec->p, &s->center);
	t_vec3 outward_normal = unit_vector(&tmp);
	set_face_normal(rec, r, &outward_normal);
	return true;
}

#endif