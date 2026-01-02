/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:52:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 00:53:06 by dlesieur         ###   ########.fr       */
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

/* Convert a component in [0,1] to an integer byte 0..255 (clamped). */
static inline int component_to_byte(real_t v)
{
	if (v <= (real_t)0.0)
		return 0;
	if (v >= (real_t)1.0)
		return 255;
	return (int)(255.999 * v);
}

/* Write pixel color to the given FILE stream as "R G B\n". */
static inline void write_color(FILE *out, const t_vec3 *pixel)
{
	int r = component_to_byte(pixel->x);
	int g = component_to_byte(pixel->y);
	int b = component_to_byte(pixel->z);
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

/* return color seen along ray r, using the hittable list 'world' */
static inline t_vec3 ray_color_world(const t_ray *r, const t_hittable_list *world)
{
	t_hit_record rec;
	if (hittable_list_hit(world, r, interval(0, INFINITY), &rec))
	{
		/* 0.5 * (rec.normal + (1,1,1)) */
		t_vec3 one = vec3_create(1.0, 1.0, 1.0);
		t_vec3 n_plus = vec3_add(&rec.normal, &one);
		return vec3_mul_scalar(&n_plus, (real_t)0.5);
	}

	t_vec3 unit_dir = unit_vector(&r->dir);
	real_t a = (real_t)0.5 * (unit_dir.y + (real_t)1.0);
	t_vec3 white = vec3_create(1.0, 1.0, 1.0);
	t_vec3 blue = vec3_create(0.5, 0.7, 1.0);
	return vec3_lerp(&white, &blue, a);
}

#endif