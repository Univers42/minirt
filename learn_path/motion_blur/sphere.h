/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 14:30:52 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 15:57:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPHERE_H
#define SPHERE_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "point.h"
#include "hittable.h"
#include "interval.h"
#include <math.h>
#include <stdbool.h>

/* Forward declaration to avoid circular dependency */
typedef struct s_material t_material;

/* Moving sphere: center1 + time * (center2 - center1) */
typedef struct s_moving_sphere
{
	t_vec3 center1;
	t_vec3 center_velocity; /* center2 - center1 */
} t_moving_sphere;

/* Sphere type: moving center, radius, albedo, and material pointer */
typedef struct s_sphere
{
	t_moving_sphere center;
	real_t radius;
	t_vec3 albedo;
	t_material *mat; /* pointer to material that determines scattering */
} t_sphere;

/* Compute sphere center at given time: center1 + time * velocity */
static inline t_vec3 sphere_center_at(const t_sphere *s, real_t time)
{
	t_vec3 scaled = vec3_mul_scalar(&s->center.center_velocity, time);
	return vec3_add(&s->center.center1, &scaled);
}

/* create_sphere: stationary sphere (center2 = center1, velocity = 0) */
static inline t_sphere create_sphere(const t_point3 *center, real_t radius, t_vec3 albedo, t_material *mat)
{
	t_sphere s;
	s.center.center1 = vec3_create(center->x, center->y, center->z);
	s.center.center_velocity = vec3_zero();
	s.radius = (radius > 0.0) ? radius : 0.0;
	s.albedo = albedo;
	s.mat = mat;
	return s;
}

/* create_sphere_moving: moving sphere (center1 to center2) */
static inline t_sphere create_sphere_moving(const t_point3 *center1, const t_point3 *center2,
											real_t radius, t_vec3 albedo, t_material *mat)
{
	t_sphere s;
	s.center.center1 = vec3_create(center1->x, center1->y, center1->z);
	t_vec3 c2 = vec3_create(center2->x, center2->y, center2->z);
	s.center.center_velocity = vec3_sub(&c2, &s.center.center1);
	s.radius = (radius > 0.0) ? radius : 0.0;
	s.albedo = albedo;
	s.mat = mat;
	return s;
}

/* create_sphere_default: stationary sphere with white albedo and no material */
static inline t_sphere create_sphere_default(const t_point3 *center, real_t radius)
{
	t_sphere s;
	s.center.center1 = vec3_create(center->x, center->y, center->z);
	s.center.center_velocity = vec3_zero();
	s.radius = (radius > 0.0) ? radius : 0.0;
	s.albedo = vec3_create(1.0, 1.0, 1.0);
	s.mat = NULL;
	return s;
}

/* simple t-only test used by earlier code paths */
static inline real_t hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r)
{
	t_vec3 oc = vec3_sub(center, &r->orig);
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

/* 4-arg sphere hit: now uses time-dependent center and assigns material to rec->mat */
static inline bool sphere_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_sphere *s = g_current_sphere;
	if (!s)
		return false;

	/* Get sphere center at ray time */
	t_vec3 current_center = sphere_center_at(s, r->tm);

	t_vec3 oc = vec3_sub(&r->orig, &current_center);
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
	t_vec3 tmp = vec3_sub(&rec->p, &current_center);
	t_vec3 outward_normal = unit_vector(&tmp);
	set_face_normal(rec, r, &outward_normal);
	/* set per-hit albedo and material from sphere */
	rec->albedo = s->albedo;
	rec->mat = s->mat;
	return true;
}

#endif