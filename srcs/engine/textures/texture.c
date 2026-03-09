/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "texture.h"
#include <math.h>

t_color	solid_color_value(const t_texture *tex, real_t u, real_t v,
			const t_point3 *p)
{
	const t_solid_color	*sc;

	(void)u;
	(void)v;
	(void)p;
	sc = (const t_solid_color *)tex->data;
	if (sc)
		return (sc->albedo);
	return (vec3_create(0.0, 0.0, 0.0));
}

void	solid_color_destroy(t_texture *tex)
{
	if (tex && tex->data)
	{
		free(tex->data);
		tex->data = NULL;
	}
}

t_texture	*solid_color_create(t_color albedo)
{
	t_texture		*tex;
	t_solid_color	*sc;

	tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return (NULL);
	sc = (t_solid_color *)malloc(sizeof(t_solid_color));
	if (!sc)
	{
		free(tex);
		return (NULL);
	}
	sc->albedo = albedo;
	tex->data = sc;
	tex->value = solid_color_value;
	tex->destroy = solid_color_destroy;
	return (tex);
}

real_t	srgb_to_linear(unsigned char c)
{
	double	v;

	v = (double)c / 255.0;
	if (v <= 0.04045)
		return ((real_t)(v / 12.92));
	return ((real_t)pow((v + 0.055) / 1.055, 2.4));
}

real_t	linear_to_srgb(real_t linear)
{
	if (linear <= 0.0031308)
		return (12.92 * linear);
	return (1.055 * pow(linear, 1.0 / 2.4) - 0.055);
}
