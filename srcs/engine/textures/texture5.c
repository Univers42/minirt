/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture5.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Footprint-based checker filtering.
**
** A 3D checker viewed at a grazing angle packs many tiles into one pixel,
** producing moire / scanline aliasing toward the horizon.  We approximate
** the pixel footprint by the distance from the camera and, once a pixel
** spans more than ~one tile, blend the checker toward its average colour.
** Distant checker then fades to flat grey (reads as natural distance haze)
** instead of shimmering.  The camera position is published once per render
** via checker_set_view().
*/

#include "texture.h"
#include "studio_config.h"
#include <math.h>

static t_point3	g_checker_view;

void	checker_set_view(const t_point3 *cam)
{
	g_checker_view = *cam;
}

int	checker_parity(const t_checker_texture *ck, const t_point3 *p)
{
	int	xi;
	int	yi;
	int	zi;

	xi = (int)floor((double)(ck->inv_scale * p->x));
	yi = (int)floor((double)(ck->inv_scale * p->y));
	zi = (int)floor((double)(ck->inv_scale * p->z));
	return (((xi + yi + zi) % 2) == 0);
}

real_t	checker_blend(const t_checker_texture *ck, const t_point3 *p)
{
	t_vec3	d;
	real_t	fp;

	d = vec3_sub(p, &g_checker_view);
	fp = vec3_length(&d) * ck->inv_scale * (real_t)RT_CHECKER_FILTER;
	if (fp <= (real_t)1.0)
		return ((real_t)0.0);
	fp = (fp - (real_t)1.0) / (real_t)3.0;
	if (fp > (real_t)1.0)
		fp = (real_t)1.0;
	return (fp);
}
