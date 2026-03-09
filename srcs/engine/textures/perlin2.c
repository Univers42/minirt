/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:07:54 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "perlin.h"
#include <math.h>

static void	perlin_gather(const t_perlin *perlin, t_vec3 c[2][2][2],
				int ijk[3])
{
	int	di;
	int	dj;
	int	dk;
	int	idx;

	di = -1;
	while (++di < 2)
	{
		dj = -1;
		while (++dj < 2)
		{
			dk = -1;
			while (++dk < 2)
			{
				idx = perlin->perm_x[(ijk[0] + di) & 255]
					^ perlin->perm_y[(ijk[1] + dj) & 255]
					^ perlin->perm_z[(ijk[2] + dk) & 255];
				c[di][dj][dk] = perlin->rand_vec[idx];
			}
		}
	}
}

real_t	perlin_noise(const t_perlin *perlin, const t_vec3 *p)
{
	real_t	uvw[3];
	int		ijk[3];
	t_vec3	c[2][2][2];

	if (!perlin || !p)
		return (0.0);
	uvw[0] = p->x - floor((double)p->x);
	uvw[1] = p->y - floor((double)p->y);
	uvw[2] = p->z - floor((double)p->z);
	ijk[0] = (int)floor((double)p->x);
	ijk[1] = (int)floor((double)p->y);
	ijk[2] = (int)floor((double)p->z);
	perlin_gather(perlin, c, ijk);
	return (perlin_interp(c, uvw[0], uvw[1], uvw[2]));
}

real_t	perlin_turb(const t_perlin *perlin, const t_vec3 *p, int depth)
{
	real_t	accum;
	real_t	weight;
	t_vec3	temp;
	int		i;

	if (!perlin || !p || depth <= 0)
		return (0.0);
	accum = 0.0;
	weight = 1.0;
	temp = *p;
	i = 0;
	while (i < depth)
	{
		accum += weight * perlin_noise(perlin, &temp);
		weight *= 0.5;
		temp = vec3_mul_scalar(&temp, (real_t)2.0);
		i++;
	}
	return ((real_t)fabs((double)accum));
}
