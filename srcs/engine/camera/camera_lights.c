/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_lights.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 23:33:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera_lights.h"
#include "ray.h"
#include "interval.h"
#include "studio_config.h"
#include <math.h>

/* Light cache — read-only during rendering, safe for concurrent access
   from OpenMP threads.  Populated once before render begins. */
t_light_info	g_lights[RT_MAX_LIGHTS];
int				g_light_count = 0;

/* Distance attenuation: RT_LIGHT_FALLOFF==1 → physical (1/d^2), else
   artistic linear (1/d).  RT_LIGHT_FALLOFF is a compile constant. */
static real_t	light_falloff(real_t nl, real_t dist_center)
{
	if (RT_LIGHT_FALLOFF == 1)
		return (nl / (dist_center * dist_center));
	return (nl / dist_center);
}

/* Pick one sample point on the light surface (jittered when soft). */
static t_point3	light_target(int idx)
{
	t_vec3	jitter;

	if (RT_SHADOW_SAMPLES > 1)
	{
		jitter = random_unit_vector();
		jitter = vec3_mul_scalar(&jitter, g_lights[idx].radius);
		return (vec3_add(&g_lights[idx].pos, &jitter));
	}
	return (g_lights[idx].pos);
}

/* Contribution of one shadow ray toward a sampled light point (zero if it
   faces away or is occluded). */
static t_color	one_sample(const t_ls *ls, t_point3 target)
{
	t_vec3	to_light;
	real_t	dist;
	real_t	nl;
	real_t	shadow_max;
	real_t	f;

	to_light = vec3_sub(&target, &ls->rec->p);
	dist = vec3_length(&to_light);
	if (dist < (real_t)1e-6)
		return (vec3_zero());
	to_light = vec3_div_scalar(&to_light, dist);
	nl = dot(&ls->rec->normal, &to_light);
	if (nl <= (real_t)0.0)
		return (vec3_zero());
	shadow_max = ls->dist_center - g_lights[ls->idx].radius - (real_t)0.01;
	if (shadow_max < (real_t)0.002)
		shadow_max = (real_t)0.002;
	if (shadow_ray_blocked(&ls->rec->p, &to_light, shadow_max, ls->world))
		return (vec3_zero());
	f = light_falloff(nl, ls->dist_center);
	return (vec3_mul_scalar(&g_lights[ls->idx].emission, f));
}

/*
** sample_one_light — multi-sample jittered shadow rays for one light.
** RT_SHADOW_SAMPLES: 1 = hard shadow (ray to centre), N = soft shadow
** (N jittered rays across the light sphere), averaged.
*/
static t_color	sample_one_light(const t_hit_record *rec, int idx,
					const t_hittable_list *world)
{
	t_ls	ls;
	t_color	accum;
	t_vec3	to_center;
	int		s;

	to_center = vec3_sub(&g_lights[idx].pos, &rec->p);
	ls = (t_ls){rec, world, idx, vec3_length(&to_center)};
	accum = vec3_zero();
	if (ls.dist_center < g_lights[idx].radius + (real_t)0.01)
		return (accum);
	s = 0;
	while (s < RT_SHADOW_SAMPLES)
	{
		to_center = one_sample(&ls, light_target(idx));
		accum = vec3_add(&accum, &to_center);
		s++;
	}
	if (RT_SHADOW_SAMPLES > 1)
		accum = vec3_div_scalar(&accum, (real_t)RT_SHADOW_SAMPLES);
	return (accum);
}

/* ------------------------------------------------------------------ */
/*  sample_direct_lights — accumulate contribution from all lights     */
/* ------------------------------------------------------------------ */

t_color	sample_direct_lights(const t_hit_record *rec,
			const t_hittable_list *world)
{
	t_color	total;
	int		i;
	t_color	contrib;

	total = vec3_zero();
	i = 0;
	while (i < g_light_count)
	{
		contrib = sample_one_light(rec, i, world);
		total = vec3_add(&total, &contrib);
		i++;
	}
	return (total);
}
