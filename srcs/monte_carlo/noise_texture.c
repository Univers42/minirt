/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   noise_texture.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:15:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "noise_texture.h"
#include <math.h>

real_t	rt_clamp(real_t x, real_t lo, real_t hi)
{
	if (x < lo)
		return (lo);
	if (x > hi)
		return (hi);
	return (x);
}

t_color	noise_texture_value(const t_texture *tex, real_t u, real_t v,
			const t_point3 *p)
{
	const t_noise_texture	*nt;
	int						depth;
	real_t					t;
	double					angle;
	real_t					base;

	(void)u;
	(void)v;
	nt = (const t_noise_texture *)tex->data;
	if (!nt || !p)
		return (vec3_create(0.0, 0.0, 0.0));
	depth = nt->turb_depth;
	if (depth <= 0)
		depth = 7;
	t = perlin_turb(&nt->perlin, (const t_vec3 *)p, depth);
	angle = (double)(nt->scale * p->z) + 10.0 * (double)t;
	base = (real_t)0.5 * ((real_t)1.0 + (real_t)sin(angle));
	return (vec3_create(base, base, base));
}

void	noise_texture_destroy(t_texture *tex)
{
	if (tex && tex->data)
	{
		free(tex->data);
		tex->data = NULL;
	}
}

t_texture	*noise_texture_create(real_t scale)
{
	t_texture		*tex;
	t_noise_texture	*nt;

	tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return (NULL);
	nt = (t_noise_texture *)malloc(sizeof(t_noise_texture));
	if (!nt)
		return (free(tex), NULL);
	perlin_init(&nt->perlin);
	if (scale > (real_t)0.0)
		nt->scale = scale;
	else
		nt->scale = (real_t)1.0;
	nt->use_turb = false;
	nt->turb_depth = 0;
	tex->data = nt;
	tex->value = noise_texture_value;
	tex->destroy = noise_texture_destroy;
	return (tex);
}

t_texture	*noise_texture_create_turb(real_t scale, int turb_depth)
{
	t_texture		*tex;
	t_noise_texture	*nt;

	tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return (NULL);
	nt = (t_noise_texture *)malloc(sizeof(t_noise_texture));
	if (!nt)
		return (free(tex), NULL);
	perlin_init(&nt->perlin);
	if (scale > (real_t)0.0)
		nt->scale = scale;
	else
		nt->scale = (real_t)1.0;
	nt->use_turb = true;
	if (turb_depth > 0)
		nt->turb_depth = turb_depth;
	else
		nt->turb_depth = 7;
	tex->data = nt;
	tex->value = noise_texture_value;
	tex->destroy = noise_texture_destroy;
	return (tex);
}
