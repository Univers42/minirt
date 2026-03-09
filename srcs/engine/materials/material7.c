/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material7.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"

t_material	*diffuse_light_create_texture_scaled(t_texture *tex, real_t scale)
{
	t_material	*mat;

	mat = diffuse_light_create_texture(tex);
	if (!mat || !mat->data)
		return (mat);
	if (scale > (real_t)0.0)
		((t_diffuse_light *)mat->data)->scale = scale;
	else
		((t_diffuse_light *)mat->data)->scale = (real_t)1.0;
	return (mat);
}

t_material	*isotropic_create(t_color albedo)
{
	t_material		*mat;
	t_isotropic		*iso;

	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	iso = (t_isotropic *)malloc(sizeof(t_isotropic));
	if (!iso)
		return (free(mat), NULL);
	iso->tex = solid_color_create(albedo);
	if (!iso->tex)
	{
		free(iso);
		free(mat);
		return (NULL);
	}
	mat->data = iso;
	mat->emitted = default_emitted;
	mat->scatter = isotropic_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = isotropic_destroy;
	return (mat);
}

t_material	*isotropic_create_texture(t_texture *tex)
{
	t_material	*mat;
	t_isotropic	*iso;

	if (!tex)
		return (NULL);
	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	iso = (t_isotropic *)malloc(sizeof(t_isotropic));
	if (!iso)
		return (free(mat), NULL);
	iso->tex = tex;
	mat->data = iso;
	mat->emitted = default_emitted;
	mat->scatter = isotropic_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = isotropic_destroy;
	return (mat);
}
