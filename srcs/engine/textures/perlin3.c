/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:07:54 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 23:07:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "perlin.h"

static double	perlin_axis(int idx, real_t fade_val)
{
	if (idx)
		return (fade_val);
	return (1.0 - fade_val);
}

static double	perlin_term(const t_vec3 c[2][2][2], const int i[3],
	const t_vec3 *fade, const t_vec3 *pos)
{
	t_vec3	weight;

	weight = vec3_create(pos->x - (real_t)i[0],
			pos->y - (real_t)i[1], pos->z - (real_t)i[2]);
	return (perlin_axis(i[0], fade->x) * perlin_axis(i[1], fade->y)
		* perlin_axis(i[2], fade->z) * dot(&c[i[0]][i[1]][i[2]], &weight));
}

real_t	perlin_interp(const t_vec3 c[2][2][2], real_t u, real_t v, real_t w)
{
	t_vec3	fade;
	t_vec3	pos;
	double	accum;
	int		i[3];

	fade = vec3_create(perlin_fade(u), perlin_fade(v), perlin_fade(w));
	pos = vec3_create(u, v, w);
	accum = 0.0;
	i[0] = -1;
	while (++i[0] < 2)
	{
		i[1] = -1;
		while (++i[1] < 2)
		{
			i[2] = -1;
			while (++i[2] < 2)
				accum += perlin_term(c, i, &fade, &pos);
		}
	}
	return ((real_t)accum);
}
