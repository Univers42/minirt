/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera9.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "camera_lights.h"
#include "studio_config.h"
#include "random.h"

/* Direct (next-event) + indirect (recursive bounce) lighting for one hit. */
t_color	compute_lighting(const t_li *li)
{
	t_color	direct;
	t_color	indirect;

	direct = vec3_zero();
	if (RT_DIRECT_LIGHT_ENABLED)
	{
		direct = sample_direct_lights(li->rec, li->world);
		direct = vec3_mul_elem(li->att, &direct);
	}
	indirect = ray_color_with_background(li->scattered, li->world,
			li->depth - 1, li->bg);
	indirect = vec3_mul_elem(li->att, &indirect);
	return (vec3_add(&direct, &indirect));
}

/* Largest of the three attenuation components, clamped to [0.05, 0.95]
   (the path's survival probability). */
static real_t	att_peak(const t_color *att)
{
	real_t	p_max;

	p_max = att->x;
	if (att->y > p_max)
		p_max = att->y;
	if (att->z > p_max)
		p_max = att->z;
	if (p_max < (real_t)0.05)
		p_max = (real_t)0.05;
	if (p_max > (real_t)0.95)
		p_max = (real_t)0.95;
	return (p_max);
}

/*
** russian_roulette — probabilistic path termination for unbiased speedup.
** After RT_RR_START_DEPTH bounces, paths with low attenuation have a
** proportional chance of being terminated.  Survivors are boosted by
** 1/p_continue to keep the estimator unbiased.  RT_RR_START_DEPTH is a
** compile constant, so the disabled (<= 0) branch folds away.
**
** Returns: 1 if the path survives (att is scaled), 0 if terminated.
*/
int	russian_roulette(int depth, int max_depth, t_color *att)
{
	real_t	p_continue;

	if (RT_RR_START_DEPTH <= 0 || max_depth - depth < RT_RR_START_DEPTH)
		return (1);
	p_continue = att_peak(att);
	if (random_real() > p_continue)
		return (0);
	*att = vec3_div_scalar(att, p_continue);
	return (1);
}
