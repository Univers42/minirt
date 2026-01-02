/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vector.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:44:21 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 17:48:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
#define VECTOR_H
#include "settings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include "types.h"
#include "stdint.h"

#ifndef FLOAT_TYPE
typedef struct s_vec3
{
	double e[TOT_COORD3];
} t_vec3;
#else

typedef struct s_vec3
{
	float e[TOT_COORD3];
} t_vec3;
#endif

typedef struct s_color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} t_color;

/* Vec3 helpers in C (constructors & operations similar to the C++ class) */

static inline t_vec3 vec3_create(real_t e0, real_t e1, real_t e2)
{
	t_vec3 v;
	v.e[X] = (real_t)e0;
	v.e[Y] = (real_t)e1;
	v.e[Z] = (real_t)e2;
	return v;
}

static inline t_vec3 vec3_zero(void)
{
	return vec3_create((real_t)0, (real_t)0, (real_t)0);
}

static inline real_t vec3_x(const t_vec3 *v) { return v->e[X]; }
static inline real_t vec3_y(const t_vec3 *v) { return v->e[Y]; }
static inline real_t vec3_z(const t_vec3 *v) { return v->e[Z]; }

static inline t_vec3 vec3_neg(const t_vec3 *v)
{
	return vec3_create(-v->e[X], -v->e[Y], -v->e[Z]);
}

static inline t_vec3 vec3_add(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->e[X] + b->e[X],
					   a->e[Y] + b->e[Y],
					   a->e[Z] + b->e[Z]);
}

static inline t_vec3 vec3_sub(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->e[X] - b->e[X],
					   a->e[Y] - b->e[Y],
					   a->e[Z] - b->e[Z]);
}

static inline t_vec3 vec3_mul_elem(const t_vec3 *a, const t_vec3 *b)
{
	return vec3_create(a->e[X] * b->e[X],
					   a->e[Y] * b->e[Y],
					   a->e[Z] * b->e[Z]);
}

static inline t_vec3 vec3_mul_scalar(const t_vec3 *v, real_t t)
{
	return vec3_create(v->e[X] * t, v->e[Y] * t, v->e[Z] * t);
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
	return (real_t)(v->e[X] * v->e[X] + v->e[Y] * v->e[Y] + v->e[Z] * v->e[Z]);
}

static inline real_t vec3_length(const t_vec3 *v)
{
	return (real_t)sqrt((double)vec3_length_squared(v));
}

static inline double dot(const t_vec3 *u, const t_vec3 *v)
{
	return (double)(u->e[X] * v->e[X] + u->e[Y] * v->e[Y] + u->e[Z] * v->e[Z]);
}

static inline t_vec3 cross(const t_vec3 *u, const t_vec3 *v)
{
	return vec3_create(
		(real_t)(u->e[Y] * v->e[Z] - u->e[Z] * v->e[Y]),
		(real_t)(u->e[Z] * v->e[X] - u->e[X] * v->e[Z]),
		(real_t)(u->e[X] * v->e[Y] - u->e[Y] * v->e[X]));
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