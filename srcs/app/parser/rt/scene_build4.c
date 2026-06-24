/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_build4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "material.h"
#include "texture.h"
#include "noise_texture.h"

/*
** Load the bump image named in the spec and attach it to a lambertian
** material (no-op on other kinds).  Called by create_material so any
** textured/plain lambertian can gain surface relief from a height image.
*/
void	attach_bump_map(const t_mat_spec *ms, t_material *m)
{
	t_texture	*bump;

	if (!m || !ms->bump_path[0])
		return ;
	bump = image_texture_create_png(ms->bump_path);
	if (bump)
		lambertian_attach_bump(m, bump, (real_t)ms->bump_strength);
}

/*
** Procedural lambertian from Perlin noise.  MAT_MARBLE -> tinted marble
** veins, MAT_WOOD -> concentric grain rings.  The .rt colour tints the
** pattern so e.g. "sp 0,1,0 2 230,230,230 marble" gives a pale marble.
*/
t_material	*create_mat_noise(const t_mat_spec *ms, t_color clr)
{
	t_texture	*noise;
	double		scale;
	int			mode;

	scale = ms->scale;
	if (scale <= 0.0)
		scale = 4.0;
	if (ms->type == MAT_WOOD)
		mode = 2;
	else
		mode = 1;
	noise = noise_texture_create_tinted(scale, clr, mode);
	if (!noise)
		return (NULL);
	return (lambertian_create_texture(noise));
}
