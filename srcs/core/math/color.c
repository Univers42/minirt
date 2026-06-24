/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:00:53 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "color.h"
#include "studio_config.h"

t_vec3	color_create(real_t x, real_t y, real_t z)
{
	return (vec3_create(x, y, z));
}

/*
** gamma_transfer — profile-specific transfer curve (v > 0 assumed).
** Selected at compile time by RT_COLOR_PROFILE (see settings.h).
**   CIE     : pow(v, 1/2.2)  — deepest blacks, best perceptual contrast
**   sRGB    : IEC 61966-2-1 piecewise (linear toe + pow 1/2.4)
**   GAMMA20 : sqrt(v)        — book default
*/
#if RT_COLOR_PROFILE == RT_COLOR_SRGB

static real_t	gamma_transfer(real_t v)
{
	if (v <= (real_t)0.0031308)
		return ((real_t)(12.92 * v));
	return ((real_t)(1.055 * pow((double)v, 1.0 / 2.4) - 0.055));
}

#elif RT_COLOR_PROFILE == RT_COLOR_GAMMA20

static real_t	gamma_transfer(real_t v)
{
	return ((real_t)sqrt((double)v));
}

#else

static real_t	gamma_transfer(real_t v)
{
	return ((real_t)pow((double)v, 1.0 / 2.2));
}

#endif

real_t	linear_to_gamma(real_t v)
{
	if (v <= (real_t)0.0)
		return ((real_t)0.0);
	return (gamma_transfer(v));
}
