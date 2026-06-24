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
