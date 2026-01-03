/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:52:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 02:46:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include "types.h"
#include "vector.h"
#include "ray.h"
#include "object.h"
#include "hittable_list.h"
#include "interval.h"

/* Return a color (t_vec3) by value, not a pointer */
static inline t_vec3 color_create(real_t x, real_t y, real_t z)
{
	return vec3_create(x, y, z);
}

/* helper: return max component of a color/vector */
static inline real_t vec3_max_component(const t_vec3 *v)
{
	real_t m = v->x;
	if (v->y > m)
		m = v->y;
	if (v->z > m)
		m = v->z;
	return m;
}

/* Convert a component in [0,1] to an integer byte 0..255 (clamped) with gamma correction */
static inline int component_to_byte(real_t v, const t_interval *interval)
{
	/* clamp */
	v = clamp(v, interval->min, interval->max);
	/* gamma-2.0 correction */
	v = (real_t)sqrt((double)v);
	return (int)(256.0 * v);
}

/* Write pixel color to the given FILE stream as "R G B\n". */
static inline void write_color(FILE *out, const t_vec3 *pixel)
{
	t_interval intensity = interval(0.000, 0.999);
	int r = component_to_byte(pixel->x, &intensity);
	int g = component_to_byte(pixel->y, &intensity);
	int b = component_to_byte(pixel->z, &intensity);
	fprintf(out, "%d %d %d\n", r, g, b);
}

/* simple ray_color that uses hit_sphere for legacy tests (kept) */
static inline t_vec3 ray_color_legacy(const t_ray *r)
{
	t_vec3 center = vec3_create((real_t)0, (real_t)0, (real_t)-1);
	real_t t = hit_sphere(&center, (real_t)0.5, r);
	if (t > 0.0)
	{
		t_vec3 p = ray_at((t_ray *)r, (real_t)t);
		t_vec3 diff = vec3_sub(&p, &center);
		t_vec3 n = unit_vector(&diff);
		t_vec3 one = vec3_create((real_t)1, (real_t)1, (real_t)1);
		t_vec3 n_plus = vec3_add(&n, &one);
		return vec3_mul_scalar(&n_plus, (real_t)0.5);
	}
	t_vec3 unit_dir = unit_vector(&r->dir);
	real_t tt = 0.5 * (unit_dir.y + (real_t)1.0);
	t_vec3 white = vec3_create(1.0, 1.0, 1.0);
	t_vec3 blue = vec3_create(0.5, 0.7, 1.0);
	return vec3_lerp(&white, &blue, tt);
}

/* depth-limited ray-color implemented recursively for true Lambertian scattering.
   On hit: scatter direction = normal + random_unit_vector(); recurse with depth-1.
   Uses per-hit albedo stored in rec.albedo. */
static inline t_vec3 ray_color_depth(const t_ray *r, const t_hittable_list *world, int depth)
{
	/* termination */
	if (depth <= 0)
		return vec3_zero();

	t_hit_record rec;
	/* use small t_min to avoid self intersections */
	if (hittable_list_hit(world, r, interval((real_t)1e-4, INFINITY), &rec))
	{
		/* Lambertian scatter: target = rec.p + rec.normal + random_unit_vector() */
		t_vec3 rnd = random_unit_vector();
		t_vec3 n_plus_rnd = vec3_add(&rec.normal, &rnd);
		t_vec3 target = vec3_add(&rec.p, &n_plus_rnd);
		t_vec3 direction = vec3_sub(&target, &rec.p);
		t_ray scattered = ray_create(rec.p, direction);

		/* attenuation by per-hit albedo */
		t_vec3 scattered_col = ray_color_depth(&scattered, world, depth - 1);
		return vec3_mul_elem(&rec.albedo, &scattered_col);
	}

	/* Miss (background) */
	t_vec3 unit_dir = unit_vector(&r->dir);
	real_t a = (real_t)0.5 * (unit_dir.y + (real_t)1.0);
	t_vec3 white = vec3_create(1.0, 1.0, 1.0);
	t_vec3 blue = vec3_create(0.5, 0.7, 1.0);
	return vec3_lerp(&white, &blue, a);
}

/* Public API: keep previous signature; use moderate recursion depth (25). */
static inline t_vec3 ray_color_world(const t_ray *r, const t_hittable_list *world)
{
	return ray_color_depth(r, world, 25);
}

#endif