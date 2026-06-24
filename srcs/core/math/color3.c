/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:00:53 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "color.h"
#include "studio_config.h"

#if RT_TONE_MAP == 1

void	apply_tonemap(real_t *r, real_t *g, real_t *b)
{
	*r = aces_tonemap(*r);
	*g = aces_tonemap(*g);
	*b = aces_tonemap(*b);
}

#else

void	apply_tonemap(real_t *r, real_t *g, real_t *b)
{
	(void)r;
	(void)g;
	(void)b;
}

#endif

#if !(RT_TONE_MAP == 1 && RT_ACES_GAMMA_BAKED)

void	apply_gamma(real_t *r, real_t *g, real_t *b)
{
	*r = linear_to_gamma(*r);
	*g = linear_to_gamma(*g);
	*b = linear_to_gamma(*b);
}

#else

void	apply_gamma(real_t *r, real_t *g, real_t *b)
{
	(void)r;
	(void)g;
	(void)b;
}

#endif

/* Contrast S-curve then BT.709 saturation, with a final clamp to >= 0. */
void	apply_grade(real_t *r, real_t *g, real_t *b)
{
	real_t	luma;

	*r = apply_contrast(*r, (real_t)RT_CONTRAST);
	*g = apply_contrast(*g, (real_t)RT_CONTRAST);
	*b = apply_contrast(*b, (real_t)RT_CONTRAST);
	luma = (real_t)0.2126 * (*r) + (real_t)0.7152 * (*g)
		+ (real_t)0.0722 * (*b);
	*r = luma + (real_t)RT_SATURATION * (*r - luma);
	*g = luma + (real_t)RT_SATURATION * (*g - luma);
	*b = luma + (real_t)RT_SATURATION * (*b - luma);
	if (*r < (real_t)0.0)
		*r = (real_t)0.0;
	if (*g < (real_t)0.0)
		*g = (real_t)0.0;
	if (*b < (real_t)0.0)
		*b = (real_t)0.0;
}
