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
#include <string.h>

/* Light cache — read-only during rendering, safe for concurrent access
   from OpenMP threads.  Populated once before render begins. */
t_light_info	g_lights[RT_MAX_LIGHTS];
int				g_light_count = 0;

/* ------------------------------------------------------------------ */
/*  set_scene_lights — call once per thread before rendering           */
/* ------------------------------------------------------------------ */

void	set_scene_lights(const t_light_info *infos, int count)
{
	if (count > RT_MAX_LIGHTS)
		count = RT_MAX_LIGHTS;
	g_light_count = count;
	if (count > 0)
		memcpy(g_lights, infos, (size_t)count * sizeof(t_light_info));
}

/* ------------------------------------------------------------------ */
/*  shadow_ray_blocked — test if a single shadow ray is occluded       */
/* ------------------------------------------------------------------ */

static int	shadow_ray_blocked(const t_point3 *origin,
				const t_vec3 *dir, real_t max_dist,
				const t_hittable_list *world)
{
	t_ray	shadow;

	shadow = ray_create(*origin, *dir, (real_t)0.0);
	return (hittable_list_hit(world, &shadow,
			interval((real_t)0.001, max_dist), NULL));
}

/* ------------------------------------------------------------------ */
/*  sample_one_light — multi-sample jittered shadow rays               */
/*                                                                     */
/*  RT_SHADOW_SAMPLES controls how many rays per light:                */
/*    1  = hard shadows (single ray toward light centre)               */
/*    N  = soft shadows (N jittered rays across light sphere surface)  */
/*                                                                     */
/*  RT_LIGHT_FALLOFF selects attenuation:                              */
/*    0 = artistic  (NdotL / dist)                                    */
/*    1 = physical  (NdotL / dist²)                                   */
/* ------------------------------------------------------------------ */

static t_color	sample_one_light(const t_hit_record *rec, int idx,
					const t_hittable_list *world)
{
	t_color		accum;
	int			s;
	int			valid;
	t_vec3		target;
	t_vec3		to_light;
	real_t		dist;
	real_t		nl;
	real_t		shadow_max;
	real_t		falloff;
	t_vec3		to_center;
	real_t		dist_center;

	accum = vec3_zero();
	valid = 0;
	to_center = vec3_sub(&g_lights[idx].pos, &rec->p);
	dist_center = vec3_length(&to_center);
	if (dist_center < g_lights[idx].radius + (real_t)0.01)
		return (accum);
	s = 0;
	while (s < RT_SHADOW_SAMPLES)
	{
		if (RT_SHADOW_SAMPLES > 1)
		{
			t_vec3 jitter = random_unit_vector();
			jitter = vec3_mul_scalar(&jitter, g_lights[idx].radius);
			target = vec3_add(&g_lights[idx].pos, &jitter);
		}
		else
			target = g_lights[idx].pos;
		to_light = vec3_sub(&target, &rec->p);
		dist = vec3_length(&to_light);
		if (dist < (real_t)1e-6)
		{
			s++;
			continue ;
		}
		to_light = vec3_div_scalar(&to_light, dist);
		nl = dot(&rec->normal, &to_light);
		if (nl <= (real_t)0.0)
		{
			s++;
			continue ;
		}
		shadow_max = dist_center - g_lights[idx].radius - (real_t)0.01;
		if (shadow_max < (real_t)0.002)
			shadow_max = (real_t)0.002;
		if (!shadow_ray_blocked(&rec->p, &to_light, shadow_max, world))
		{
#if RT_LIGHT_FALLOFF == 1
			falloff = nl / (dist_center * dist_center);
#else
			falloff = nl / dist_center;
#endif
			accum = vec3_add(&accum,
					&(t_color){g_lights[idx].emission.x * falloff,
					g_lights[idx].emission.y * falloff,
					g_lights[idx].emission.z * falloff});
			valid++;
		}
		s++;
	}
	if (valid > 0 && RT_SHADOW_SAMPLES > 1)
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
