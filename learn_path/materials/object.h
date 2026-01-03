/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 17:49:10 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 02:18:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJECT_H
#define OBJECT_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "point.h"
#include "hittable.h"
#include "interval.h" /* <-- added so t_interval is defined */
#include <math.h>
#include <stdbool.h>

/* Sphere type uses t_vec3 center so vector ops work uniformly. */
typedef struct s_sphere
{
	t_vec3 center;
	real_t radius;
	t_vec3 albedo; /* new: sphere color */
} t_sphere;

/* create_sphere now accepts an albedo */
static inline t_sphere create_sphere(const t_point3 *center, real_t radius, t_vec3 albedo)
{
	t_sphere s;
	s.center = vec3_create(center->x, center->y, center->z);
	s.radius = radius;
	s.albedo = albedo;
	return s;
}

static inline t_sphere create_sphere_default(const t_point3 *center, real_t radius)
{
	t_sphere s;
	s.center = vec3_create(center->x, center->y, center->z);
	s.radius = radius;
	s.albedo = vec3_create(1.0, 1.0, 1.0); // default to white color
	return s;
}

/* simple t-only test used by earlier code paths */
static inline real_t hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r)
{
	t_vec3 oc = vec3_sub(center, &r->orig); /* center - origin */
	real_t a = vec3_length_squared(&r->dir);
	real_t h = (real_t)dot(&r->dir, &oc);
	real_t c = vec3_length_squared(&oc) - radius * radius;
	real_t discriminant = (real_t)h * (real_t)h - (real_t)a * (real_t)c;
	if (discriminant < 0.0)
		return -1.0;
	real_t root = sqrt(discriminant);
	return ((real_t)h - root) / (real_t)a;
}

/* Module-local current sphere for no-obj 4-arg hit calls */
static const t_sphere *g_current_sphere = NULL;
static inline void set_current_sphere(const void *obj) { g_current_sphere = (const t_sphere *)obj; }

/* 4-arg sphere hit using the currently-bound sphere */
static inline bool sphere_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_sphere *s = g_current_sphere;
	if (!s)
		return false;
	t_vec3 oc = vec3_sub(&r->orig, &s->center);
	real_t a = vec3_length_squared(&r->dir);
	real_t half_b = dot(&r->dir, &oc);
	real_t c = (real_t)vec3_length_squared(&oc) - (real_t)s->radius * (real_t)s->radius;
	real_t discriminant = half_b * half_b - (real_t)a * c;
	if (discriminant < 0.0)
		return false;
	real_t sqrtd = sqrt(discriminant);
	/* try the nearer root first; if it is outside the interval try the farther */
	real_t root = (-half_b - sqrtd) / (real_t)a;
	if (!contains(rayt.min, rayt.max, root))
	{
		root = (-half_b + sqrtd) / (real_t)a;
		if (!contains(rayt.min, rayt.max, root))
			return false;
	}
	rec->t = (real_t)root;
	rec->p = ray_at((t_ray *)r, rec->t);
	t_vec3 tmp = vec3_sub(&rec->p, &s->center);
	t_vec3 outward_normal = unit_vector(&tmp);
	set_face_normal(rec, r, &outward_normal);
	/* set per-hit albedo from sphere */
	rec->albedo = s->albedo;
	return true;
}

#endif