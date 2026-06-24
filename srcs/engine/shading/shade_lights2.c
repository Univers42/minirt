/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shade_lights2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 03:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 03:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shading.h"
#include "camera_lights.h"
#include "ray.h"
#include "interval.h"
#include "studio_config.h"
#include <math.h>

/* Fixed deterministic disk pattern (unit radius) over the light surface.
   Index 0 is the centre; the rest are two stratified rings.  Using a
   precomputed set (not random_*) keeps renders bit-for-bit reproducible. */
static const real_t	g_disk[16][2] = {
{0.0, 0.0}, {0.52, 0.0}, {0.16, 0.49}, {-0.42, 0.31},
{-0.42, -0.31}, {0.16, -0.49}, {0.92, 0.38}, {-0.35, 0.92},
{-0.97, 0.0}, {-0.35, -0.92}, {0.92, -0.38}, {0.0, 0.70},
{-0.68, -0.18}, {0.50, -0.50}, {0.0, -0.85}, {0.68, 0.18}
};

/* Orthonormal tangent basis perpendicular to the light direction l. */
static void	light_basis(const t_vec3 *l, t_vec3 *t, t_vec3 *b)
{
	t_vec3	up;

	up = (t_vec3){0.0, 1.0, 0.0};
	if (fabs(l->y) > (real_t)0.99)
		up = (t_vec3){1.0, 0.0, 0.0};
	*t = cross(l, &up);
	*t = unit_vector(t);
	*b = cross(l, t);
}

/* Cast one shadow ray toward a sampled point on the light surface, but
   stop it at maxd (distance to light centre minus its radius) so it never
   strikes the light's own emissive sphere. */
static int	occluded(const t_point3 *p, const t_vec3 *target,
				real_t maxd, const t_hittable_list *world)
{
	t_ray	s;
	t_vec3	dir;
	real_t	len;

	dir = vec3_sub(target, p);
	len = vec3_length(&dir);
	if (len < (real_t)1e-6)
		return (0);
	dir = vec3_div_scalar(&dir, len);
	if (maxd < (real_t)0.002)
		maxd = (real_t)0.002;
	s = ray_create(*p, dir, (real_t)0.0);
	return (hittable_list_hit(world, &s, interval((real_t)0.001, maxd), NULL));
}

/* Point on the light's surface for sample s, offset within the tangent
   plane (t, b) by the fixed disk pattern scaled to the light radius. */
static t_point3	sample_point(int s, int idx, const t_vec3 *t, const t_vec3 *b)
{
	t_vec3	ot;
	t_vec3	ob;
	t_vec3	off;

	ot = vec3_mul_scalar(t, g_disk[s][0] * g_lights[idx].radius);
	ob = vec3_mul_scalar(b, g_disk[s][1] * g_lights[idx].radius);
	off = vec3_add(&ot, &ob);
	return (vec3_add(&g_lights[idx].pos, &off));
}

/* Count how many of the q->count stratified disk samples reach the light. */
static int	count_visible(const t_shadow_q *q)
{
	t_vec3		t;
	t_vec3		b;
	t_point3	target;
	int			vis;
	int			s;

	light_basis(q->l, &t, &b);
	vis = 0;
	s = -1;
	while (++s < q->count)
	{
		target = sample_point(s, q->idx, &t, &b);
		vis += !occluded(q->p, &target, q->maxd, q->world);
	}
	return (vis);
}

real_t	soft_shadow_visibility(const t_point3 *p, const t_vec3 *l,
			int idx, const t_hittable_list *world)
{
	t_shadow_q	q;
	t_point3	target;

	target = vec3_sub(&g_lights[idx].pos, p);
	q.p = p;
	q.l = l;
	q.world = world;
	q.idx = idx;
	q.maxd = vec3_length(&target) - g_lights[idx].radius - (real_t)0.01;
	q.count = rt_clampi(RT_SOFT_SHADOW_SAMPLES, 1, 16);
	if (q.count == 1 || g_lights[idx].radius < (real_t)1e-4)
	{
		if (occluded(p, &g_lights[idx].pos, q.maxd, world))
			return ((real_t)0.0);
		return ((real_t)1.0);
	}
	return ((real_t)count_visible(&q) / (real_t)q.count);
}
