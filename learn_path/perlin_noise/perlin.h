/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 20:16:49 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 23:01:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PERLIN_H
#define PERLIN_H

#include "common.h"
#include "random.h"
#define POINT_COUNT 256

typedef struct s_perlin
{
	t_vec3 randvec[POINT_COUNT]; /* gradient vectors */
	int perm_x[POINT_COUNT];
	int perm_y[POINT_COUNT];
	int perm_z[POINT_COUNT];
} t_perlin;

/* Forward prototypes */
static inline void permute(int *p, int n);
static inline void perlin_generate_perm(int *p);
static inline real_t perlin_interp(const t_vec3 c[2][2][2], real_t u, real_t v, real_t w);

/* Hermitian fade: 3t^2 - 2t^3 */
static inline real_t perlin_fade(real_t t)
{
	double td = (double)t;
	double res = td * td * (3.0 - 2.0 * td);
	return (real_t)res;
}

/* Create perlin with random gradient vectors */
static inline t_perlin perlin_create(void)
{
	t_perlin v;

	for (int i = 0; i < POINT_COUNT; i++)
	{
		v.randvec[i] = random_unit_vector();
	}
	perlin_generate_perm(v.perm_x);
	perlin_generate_perm(v.perm_y);
	perlin_generate_perm(v.perm_z);
	return (v);
}

/* Perlin noise using gradient vectors and dot products */
static inline real_t perlin_noise(const t_perlin *p, const t_point3 *point)
{
	/* Integer lattice corner */
	int i = (int)floor((double)point->x);
	int j = (int)floor((double)point->y);
	int k = (int)floor((double)point->z);

	/* Fractional part */
	real_t u = (real_t)(point->x - floor((double)point->x));
	real_t v = (real_t)(point->y - floor((double)point->y));
	real_t w = (real_t)(point->z - floor((double)point->z));

	/* Fetch 8 gradient vectors at lattice corners */
	t_vec3 c[2][2][2];
	for (int di = 0; di < 2; ++di)
		for (int dj = 0; dj < 2; ++dj)
			for (int dk = 0; dk < 2; ++dk)
			{
				int xi = (i + di) & 255;
				int yj = (j + dj) & 255;
				int zk = (k + dk) & 255;
				int idx = p->perm_x[xi] ^ p->perm_y[yj] ^ p->perm_z[zk];
				c[di][dj][dk] = p->randvec[idx];
			}

	return perlin_interp(c, u, v, w);
}

/* Trilinear interpolation with gradient dot products */
static inline real_t perlin_interp(const t_vec3 c[2][2][2], real_t u, real_t v, real_t w)
{
	/* Apply Hermitian fade to fractional coordinates */
	real_t uu = perlin_fade(u);
	real_t vv = perlin_fade(v);
	real_t ww = perlin_fade(w);

	real_t accum = 0.0;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				/* Weight vector from lattice point to sample point */
				t_vec3 weight_v = vec3_create(u - (real_t)i, v - (real_t)j, w - (real_t)k);

				/* Dot product of gradient with weight vector */
				real_t dot_prod = (real_t)dot(&c[i][j][k], &weight_v);

				/* Trilinear blend weights */
				real_t blend_i = (real_t)i * uu + (real_t)(1 - i) * (1.0 - uu);
				real_t blend_j = (real_t)j * vv + (real_t)(1 - j) * (1.0 - vv);
				real_t blend_k = (real_t)k * ww + (real_t)(1 - k) * (1.0 - ww);

				accum += blend_i * blend_j * blend_k * dot_prod;
			}
		}
	}

	return accum;
}

/* Turbulence (FBM): sum of octaves of noise at increasing frequencies */
static inline real_t perlin_turb(const t_perlin *p, const t_point3 *pt, int depth)
{
	real_t accum = 0.0;
	real_t weight = 1.0;
	t_point3 temp_p = *pt;

	for (int i = 0; i < depth; i++)
	{
		accum += weight * perlin_noise(p, &temp_p);
		weight *= 0.5;
		/* Scale point for next octave */
		temp_p = point3_create(temp_p.x * 2.0, temp_p.y * 2.0, temp_p.z * 2.0);
	}

	/* Return absolute value for cloud-like patterns */
	if (accum < 0.0)
		accum = -accum;
	return accum;
}

/* Fisher–Yates shuffle */
static inline void permute(int *p, int n)
{
	for (int i = n - 1; i > 0; i--)
	{
		int target = random_int(0, i);
		int tmp = p[i];
		p[i] = p[target];
		p[target] = tmp;
	}
}

/* Fill permutation array and shuffle */
static inline void perlin_generate_perm(int *p)
{
	for (int i = 0; i < POINT_COUNT; i++)
		p[i] = i;
	permute(p, POINT_COUNT);
}

#endif