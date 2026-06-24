/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   noise_texture2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "noise_texture.h"
#include <math.h>

/*
** Concentric wood rings: radius in the x-z plane, modulated by turbulence so
** the rings wobble like real grain.  Returns a 0..1 banding factor.
*/
real_t	noise_wood_base(const t_noise_texture *nt, const t_point3 *p)
{
	int		depth;
	real_t	t;
	double	radius;
	double	rings;

	depth = nt->turb_depth;
	if (depth <= 0)
		depth = 4;
	t = perlin_turb(&nt->perlin, (const t_vec3 *)p, depth);
	radius = sqrt((double)(p->x * p->x + p->z * p->z));
	rings = nt->scale * radius + 6.0 * (double)t;
	return ((real_t)0.5 * ((real_t)1.0 + (real_t)sin(rings)));
}

t_texture	*noise_texture_create_tinted(real_t scale, t_color tint, int mode)
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
		nt->scale = (real_t)4.0;
	nt->use_turb = true;
	nt->turb_depth = 7;
	nt->mode = mode;
	nt->tint = tint;
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
	nt->mode = 0;
	nt->tint = vec3_create(1.0, 1.0, 1.0);
	tex->data = nt;
	tex->value = noise_texture_value;
	tex->destroy = noise_texture_destroy;
	return (tex);
}
