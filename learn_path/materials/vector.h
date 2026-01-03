/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:44:21 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 02:18:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
#define VECTOR_H

#include "settings.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "random.h"
#include <stdbool.h>

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

/* Linear interpolation between two vectors: (1-t)*a + t*b
   Moved earlier so other headers (color.h) see it during inclusion. */
static inline t_vec3 vec3_lerp(const t_vec3 *a, const t_vec3 *b, real_t t)
{
	t_vec3 one_minus_t_a = vec3_mul_scalar(a, (real_t)(1.0 - t));
	t_vec3 t_b = vec3_mul_scalar(b, t);
	return vec3_add(&one_minus_t_a, &t_b);
}

/* random helpers (kept here) */
static inline t_vec3 vec3_random(void)
{
	return ((t_vec3){.x = random_real(), .y = random_real(), .z = random_real()});
}

static inline t_vec3 vec3_random_interval(real_t min, real_t max)
{
	return (vec3_create(random_real_interval(min, max), random_real_interval(min, max), random_real_interval(min, max)));
}

/* random_unit_vector and hemisphere sampler */
static inline t_vec3 random_unit_vector(void)
{
	t_vec3 p;
	real_t lensq;

	while (true)
	{
		p = vec3_random_interval((real_t)-1, (real_t)1);
		lensq = vec3_length_squared(&p);
		/* reject extremely small squared-lengths to avoid underflow/denormals,
		   use double-precision cutoff as requested */
		if ((double)1e-160 < (double)lensq && (double)lensq <= 1.0)
		{
			real_t len = (real_t)sqrt((double)lensq);
			return vec3_div_scalar(&p, len);
		}
	}
}

static inline t_vec3 random_on_hemisphere(const t_vec3 *normal)
{
	t_vec3 on_unit_sphere;

	on_unit_sphere = random_unit_vector();
	if (dot(&on_unit_sphere, normal) > 0.0)
		return (on_unit_sphere);
	else
		return (vec3_neg(&on_unit_sphere));
}

/* Cosine-weighted sampling on the hemisphere oriented by `normal`.
   Build cross product into locals (no &temporary). */
static inline t_vec3 random_cosine_direction(const t_vec3 *normal)
{
	/* build local orthonormal basis (u,v,w) with w = normal */
	t_vec3 w = *normal;
	/* choose arbitrary vector not parallel to w */
	t_vec3 a = (fabs(w.x) > (real_t)0.9) ? vec3_create((real_t)0.0, (real_t)1.0, (real_t)0.0) : vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);
	t_vec3 cross_wa = cross(&w, &a);
	t_vec3 v = unit_vector(&cross_wa);
	t_vec3 u = cross(&w, &v);

	/* sample on unit disk and lift to hemisphere (cosine-weighted) */
	real_t r1 = random_real();
	real_t r2 = random_real();
	real_t r = (real_t)sqrt((double)r1);
	real_t theta = (real_t)(2.0 * PI * (double)r2);
	real_t x = r * (real_t)cos((double)theta);
	real_t y = r * (real_t)sin((double)theta);
	real_t z = (real_t)sqrt(1.0 - (double)r1);

	/* transform to world coordinates: dir = x*u + y*v + z*w */
	t_vec3 xu = vec3_mul_scalar(&u, x);
	t_vec3 yv = vec3_mul_scalar(&v, y);
	t_vec3 zw = vec3_mul_scalar(&w, z);
	t_vec3 tmp = vec3_add(&xu, &yv);
	return vec3_add(&tmp, &zw);
}

#endif