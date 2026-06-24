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
** Grazing-angle-aware checker filtering.
**
** A 3D checker viewed at a grazing angle packs many tiles into one pixel,
** producing moire / scanline aliasing toward the horizon.  The pixel
** footprint on a planar floor grows with distance AND with 1/cos^2(theta),
** where theta is the angle between the view ray and the surface normal —
** so distance alone under-filters near the horizon, leaving dark bands.
** We scale the footprint by 1/cos^2 and, once it spans most of a tile,
** smoothly blend the checker toward its average colour over a short range.
** Distant/grazing checker then fades to flat grey instead of shimmering,
** while near checker stays crisp.  The camera position is published once
** per render via checker_set_view(); the hit normal is published per-hit
** (OpenMP-thread-local) via checker_set_hit().
*/

#include "texture.h"
#include "studio_config.h"
#include <math.h>

static t_point3			g_checker_view;
static __thread t_vec3	g_checker_normal = {0.0, 1.0, 0.0};

void	checker_set_view(const t_point3 *cam)
{
	g_checker_view = *cam;
}

void	checker_set_hit(const t_vec3 *normal)
{
	g_checker_normal = *normal;
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
	t_vec3	v;
	real_t	cosg;
	real_t	fp;

	d = vec3_sub(p, &g_checker_view);
	v = unit_vector(&d);
	cosg = fabs((double)dot(&v, &g_checker_normal));
	if (cosg < (real_t)0.01)
		cosg = (real_t)0.01;
	fp = vec3_length(&d) * ck->inv_scale * (real_t)RT_CHECKER_FILTER;
	fp = fp / (cosg * cosg);
	if (fp <= (real_t)0.7)
		return ((real_t)0.0);
	if (fp >= (real_t)1.0)
		return ((real_t)1.0);
	fp = (fp - (real_t)0.7) / (real_t)0.3;
	return (fp * fp * ((real_t)3.0 - (real_t)2.0 * fp));
}
