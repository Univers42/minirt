/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:00:53 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "color.h"
#include "studio_config.h"

real_t	vec3_max_component(const t_vec3 *v)
{
	real_t	m;

	m = v->x;
	if (v->y > m)
		m = v->y;
	if (v->z > m)
		m = v->z;
	return (m);
}

/*
** aces_tonemap — Narkowicz 2015 ACES filmic tone-mapping curve.
** Maps HDR linear values to [0,1] display range with graceful
** highlight rolloff and preserved shadow detail.
** Formula: (v*(2.51v+0.03)) / (v*(2.43v+0.59)+0.14)
*/
real_t	aces_tonemap(real_t v)
{
	real_t	num;
	real_t	den;

	if (v < (real_t)0.0)
		v = (real_t)0.0;
	num = v * ((real_t)2.51 * v + (real_t)0.03);
	den = v * ((real_t)2.43 * v + (real_t)0.59) + (real_t)0.14;
	v = num / den;
	if (v > (real_t)1.0)
		v = (real_t)1.0;
	return (v);
}

/* Convert a [0,1] component to byte [0,255] with clamping (no gamma here) */
int	component_to_byte(real_t v, const t_interval *intensity)
{
	v = clamp(v, intensity->min, intensity->max);
	return ((int)(256.0 * v));
}

/* S-curve contrast: pivot at 0.5, scale deviation by 'c'. */
real_t	apply_contrast(real_t v, real_t c)
{
	v = ((real_t)0.5) + (v - (real_t)0.5) * c;
	if (v < (real_t)0.0)
		v = (real_t)0.0;
	if (v > (real_t)1.0)
		v = (real_t)1.0;
	return (v);
}
