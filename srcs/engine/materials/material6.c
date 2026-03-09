/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material6.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"

t_material	*tinted_glass_create(real_t refraction_index, t_color tint)
{
	t_material		*mat;
	t_tinted_glass	*glass;

	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	glass = (t_tinted_glass *)malloc(sizeof(t_tinted_glass));
	if (!glass)
		return (free(mat), NULL);
	glass->refraction_index = refraction_index;
	glass->tint = tint;
	mat->data = glass;
	mat->emitted = default_emitted;
	mat->scatter = tinted_glass_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = tinted_glass_destroy;
	return (mat);
}

t_material	*glossy_create(t_color albedo, real_t roughness, real_t metalness)
{
	t_material	*mat;
	t_glossy	*glossy;

	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	glossy = (t_glossy *)malloc(sizeof(t_glossy));
	if (!glossy)
		return (free(mat), NULL);
	glossy->albedo = albedo;
	glossy->roughness = clamp(roughness, 0.0, 1.0);
	glossy->metalness = clamp(metalness, 0.0, 1.0);
	mat->data = glossy;
	mat->emitted = default_emitted;
	mat->scatter = glossy_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = glossy_destroy;
	return (mat);
}

t_material	*diffuse_light_create(t_color emit)
{
	t_material		*mat;
	t_diffuse_light	*light;

	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	light = (t_diffuse_light *)malloc(sizeof(t_diffuse_light));
	if (!light)
		return (free(mat), NULL);
	light->tex = solid_color_create(emit);
	if (!light->tex)
	{
		free(light);
		free(mat);
		return (NULL);
	}
	light->scale = (real_t)1.0;
	mat->data = light;
	mat->emitted = diffuse_light_emitted;
	mat->scatter = diffuse_light_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = diffuse_light_destroy;
	return (mat);
}

t_material	*diffuse_light_create_scaled(t_color emit, real_t scale)
{
	t_material	*mat;

	mat = diffuse_light_create(emit);
	if (!mat || !mat->data)
		return (mat);
	if (scale > (real_t)0.0)
		((t_diffuse_light *)mat->data)->scale = scale;
	else
		((t_diffuse_light *)mat->data)->scale = (real_t)1.0;
	return (mat);
}

t_material	*diffuse_light_create_texture(t_texture *tex)
{
	t_material		*mat;
	t_diffuse_light	*light;

	if (!tex)
		return (NULL);
	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	light = (t_diffuse_light *)malloc(sizeof(t_diffuse_light));
	if (!light)
		return (free(mat), NULL);
	light->tex = tex;
	light->scale = (real_t)1.0;
	mat->data = light;
	mat->emitted = diffuse_light_emitted;
	mat->scatter = diffuse_light_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = diffuse_light_destroy;
	return (mat);
}
