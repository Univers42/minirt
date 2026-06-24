/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shade_ao.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 03:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 03:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shading.h"
#include "ray.h"
#include "interval.h"
#include "studio_config.h"
#include <math.h>

/* Fixed cosine-weighted hemisphere directions in the local frame where
   the surface normal is +Z.  (x, y) is a disk sample; z = sqrt(1-x^2-y^2)
   recovers the cosine-distributed elevation (Malley's method).  Using a
   precomputed set (not random_*) keeps AO bit-for-bit reproducible. */
static const real_t	g_ao[16][2] = {
{0.0, 0.0}, {0.50, 0.0}, {0.15, 0.47}, {-0.40, 0.29},
{-0.40, -0.29}, {0.15, -0.47}, {0.78, 0.32}, {-0.30, 0.78},
{-0.82, 0.0}, {-0.30, -0.78}, {0.78, -0.32}, {0.0, 0.60},
{-0.58, -0.15}, {0.42, -0.42}, {0.0, -0.72}, {0.58, 0.15}
};

/* Orthonormal tangent basis around the surface normal n. */
static void	ao_basis(const t_vec3 *n, t_vec3 *t, t_vec3 *b)
{
	t_vec3	up;

	up = (t_vec3){0.0, 1.0, 0.0};
	if (fabs(n->y) > (real_t)0.99)
		up = (t_vec3){1.0, 0.0, 0.0};
	*t = cross(n, &up);
	*t = unit_vector(t);
	*b = cross(n, t);
}

/* World-space cosine-weighted hemisphere direction for sample s. */
static t_vec3	ao_dir(int s, const t_vec3 *n, const t_vec3 *t, const t_vec3 *b)
{
	t_vec3	dir;
	t_vec3	tmp;
	real_t	z;

	z = (real_t)1.0 - g_ao[s][0] * g_ao[s][0] - g_ao[s][1] * g_ao[s][1];
	if (z < (real_t)0.0)
		z = (real_t)0.0;
	z = sqrt(z);
	dir = vec3_mul_scalar(t, g_ao[s][0]);
	tmp = vec3_mul_scalar(b, g_ao[s][1]);
	dir = vec3_add(&dir, &tmp);
	tmp = vec3_mul_scalar(n, z);
	return (vec3_add(&dir, &tmp));
}

/* Unoccluded fraction in [0,1] of N fixed hemisphere probe rays of length
   RT_AO_RADIUS around the surface normal.  1.0 = fully open (no darkening),
   lower = more enclosed (crease / contact).  Modulates ambient only. */
real_t	ambient_occlusion(const t_hit_record *rec, const t_hittable_list *world)
{
	t_vec3	t;
	t_vec3	b;
	t_ray	probe;
	int		n[2];
	int		s;

	n[0] = rt_clampi(RT_AO_SAMPLES, 0, 16);
	if (n[0] <= 0)
		return ((real_t)1.0);
	ao_basis(&rec->normal, &t, &b);
	n[1] = 0;
	s = -1;
	while (++s < n[0])
	{
		probe = ray_create(rec->p, ao_dir(s, &rec->normal, &t, &b),
				(real_t)0.0);
		n[1] += !hittable_list_hit(world, &probe,
				interval((real_t)1e-3, RT_AO_RADIUS), NULL);
	}
	return ((real_t)RT_AO_MIN + ((real_t)1.0 - (real_t)RT_AO_MIN)
		* ((real_t)n[1] / (real_t)n[0]));
}
