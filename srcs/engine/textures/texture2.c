/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:32:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "texture.h"
#include <math.h>

t_color	checker_texture_value(const t_texture *tex, real_t u, real_t v,
			const t_point3 *p)
{
	const t_checker_texture	*checker;
	int						xi;
	int						yi;
	int						zi;
	t_texture				*sample;

	(void)u;
	(void)v;
	checker = (const t_checker_texture *)tex->data;
	if (!checker || !checker->even || !checker->odd)
		return (vec3_create(0.0, 0.0, 0.0));
	xi = (int)floor((double)(checker->inv_scale * p->x));
	yi = (int)floor((double)(checker->inv_scale * p->y));
	zi = (int)floor((double)(checker->inv_scale * p->z));
	if (((xi + yi + zi) % 2) == 0)
		sample = checker->even;
	else
		sample = checker->odd;
	return (sample->value(sample, u, v, p));
}

void	checker_texture_destroy(t_texture *tex)
{
	t_checker_texture	*checker;

	if (!tex || !tex->data)
		return ;
	checker = (t_checker_texture *)tex->data;
	if (checker->even)
	{
		checker->even->destroy(checker->even);
		free(checker->even);
	}
	if (checker->odd)
	{
		checker->odd->destroy(checker->odd);
		free(checker->odd);
	}
	free(tex->data);
	tex->data = NULL;
}

t_texture	*checker_texture_create(real_t scale, t_texture *even,
				t_texture *odd)
{
	t_texture			*tex;
	t_checker_texture	*checker;

	tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return (NULL);
	checker = (t_checker_texture *)malloc(sizeof(t_checker_texture));
	if (!checker)
	{
		free(tex);
		return (NULL);
	}
	if (scale > 0.0)
		checker->inv_scale = 1.0 / scale;
	else
		checker->inv_scale = 1.0;
	checker->even = even;
	checker->odd = odd;
	tex->data = checker;
	tex->value = checker_texture_value;
	tex->destroy = checker_texture_destroy;
	return (tex);
}
