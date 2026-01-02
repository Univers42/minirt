/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:44:21 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 00:51:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
#define VECTOR_H

#include "settings.h"
#include <stdint.h>
#include <math.h>

/* real_t is expected to be defined by types.h before this header is included */

typedef struct s_vec3
{
	real_t x;
	real_t y;
	real_t z;
} t_vec3;

/* Vec3 helpers in C (constructors & operations similar to the C++ class) */

static inline t_vec3 vec3_create(real_t e0, real_t e1, real_t e2)
{
	t_vec3 v;
	v.x = (real_t)e0;
	v.y = (real_t)e1;
	v.z = (real_t)e2;
	return v;
}

static inline t_vec3 vec3_zero(void) { return vec3_create((real_t)0, (real_t)0, (real_t)0); }

static inline real_t vec3_x(const t_vec3 *v) { return v->x; }
static inline real_t vec3_y(const t_vec3 *v) { return v->y; }
static inline real_t vec3_z(const t_vec3 *v) { return v->z; }

static inline t_vec3 vec3_neg(const t_vec3 *v)
{
	return vec3_create(-v->x, -v->y, -v->z);
}

static inline t_vec3 vec3_add(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->x + b->x,
					   a->y + b->y,
					   a->z + b->z);
}

static inline t_vec3 vec3_sub(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->x - b->x,
					   a->y - b->y,
					   a->z - b->z);
}

static inline t_vec3 vec3_mul_elem(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->x * b->x,
					   a->y * b->y,
					   a->z * b->z);
}

static inline t_vec3 vec3_mul_scalar(const t_vec3 *v, real_t t)
{
	return vec3_create(v->x * t, v->y * t, v->z * t);
}

static inline t_vec3 vec3_mul_scalar_rev(real_t t, const t_vec3 *v)
{
	return vec3_mul_scalar(v, t);
}

static inline t_vec3 vec3_div_scalar(const t_vec3 *v, real_t t)
{
	return vec3_mul_scalar(v, (real_t)(1.0 / t));
}

static inline real_t vec3_length_squared(const t_vec3 *v)
{
	return (real_t)(v->x * v->x + v->y * v->y + v->z * v->z);
}

static inline real_t vec3_length(const t_vec3 *v)
{
	return (real_t)sqrt((double)vec3_length_squared(v));
}

static inline double dot(const t_vec3 *u, const t_vec3 *v)
{
	return (double)(u->x * v->x + u->y * v->y + u->z * v->z);
}

static inline t_vec3 cross(const t_vec3 *u, const t_vec3 *v)
{
	return vec3_create(
		(real_t)(u->y * v->z - u->z * v->y),
		(real_t)(u->z * v->x - u->x * v->z),
		(real_t)(u->x * v->y - u->y * v->x));
}

static inline t_vec3 unit_vector(const t_vec3 *v)
{
	real_t len = vec3_length(v);
	if (len == (real_t)0)
		return vec3_zero();
	return vec3_div_scalar(v, len);
}

/* Linear interpolation between two vectors: (1-t)*a + t*b */
static inline t_vec3 vec3_lerp(const t_vec3 *a, const t_vec3 *b, real_t t)
{
	t_vec3 one_minus_t_a = vec3_mul_scalar(a, (real_t)(1.0 - t));
	t_vec3 t_b = vec3_mul_scalar(b, t);
	return vec3_add(&one_minus_t_a, &t_b);
}

#endif