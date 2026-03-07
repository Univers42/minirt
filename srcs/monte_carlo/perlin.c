/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:07:54 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "perlin.h"

void	perlin_permute(int *p, int n)
{
	int	i;
	int	target;
	int	tmp;

	i = n - 1;
	while (i > 0)
	{
		target = random_int(0, i);
		tmp = p[i];
		p[i] = p[target];
		p[target] = tmp;
		i--;
	}
}

void	perlin_generate_perm(int *p)
{
	int	i;

	i = 0;
	while (i < POINT_COUNT)
	{
		p[i] = i;
		i++;
	}
	perlin_permute(p, POINT_COUNT);
}

real_t	perlin_fade(real_t t)
{
	return (t * t * ((real_t)3.0 - (real_t)2.0 * t));
}

real_t	perlin_interp(const t_vec3 c[2][2][2], real_t u, real_t v, real_t w)
{
	real_t	uu;
	real_t	vv;
	real_t	ww;
	double	accum;
	int		i[3];

	uu = perlin_fade(u);
	vv = perlin_fade(v);
	ww = perlin_fade(w);
	accum = 0.0;
	i[0] = -1;
	while (++i[0] < 2)
	{
		i[1] = -1;
		while (++i[1] < 2)
		{
			i[2] = -1;
			while (++i[2] < 2)
			{
				t_vec3 weight = vec3_create(u - (real_t)i[0],
						v - (real_t)i[1], w - (real_t)i[2]);
				accum += (i[0] ? uu : (1.0 - uu))
					* (i[1] ? vv : (1.0 - vv))
					* (i[2] ? ww : (1.0 - ww))
					* dot(&c[i[0]][i[1]][i[2]], &weight);
			}
		}
	}
	return ((real_t)accum);
}

void	perlin_init(t_perlin *perlin)
{
	int	i;

	if (!perlin)
		return ;
	i = 0;
	while (i < POINT_COUNT)
	{
		perlin->rand_vec[i] = random_unit_vector();
		i++;
	}
	perlin_generate_perm(perlin->perm_x);
	perlin_generate_perm(perlin->perm_y);
	perlin_generate_perm(perlin->perm_z);
}
