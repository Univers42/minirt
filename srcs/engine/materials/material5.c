/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material5.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"

t_material	*lambertian_create_texture(t_texture *tex)
{
	t_material		*mat;
	t_lambertian	*lamb;

	if (!tex)
		return (NULL);
	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	lamb = (t_lambertian *)malloc(sizeof(t_lambertian));
	if (!lamb)
		return (free(mat), NULL);
	lamb->tex = tex;
	lamb->bump = NULL;
	lamb->bump_strength = (real_t)0.0;
	mat->data = lamb;
	mat->emitted = default_emitted;
	mat->scatter = lambertian_scatter;
	mat->scattering_pdf = lambertian_scattering_pdf;
	mat->destroy = lambertian_destroy;
	return (mat);
}

t_material	*lambertian_create(t_color albedo)
{
	t_material		*mat;
	t_lambertian	*lamb;

	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	lamb = (t_lambertian *)malloc(sizeof(t_lambertian));
	if (!lamb)
		return (free(mat), NULL);
	lamb->tex = solid_color_create(albedo);
	if (!lamb->tex)
	{
		free(lamb);
		free(mat);
		return (NULL);
	}
	lamb->bump = NULL;
	lamb->bump_strength = (real_t)0.0;
	mat->data = lamb;
	mat->emitted = default_emitted;
	mat->scatter = lambertian_scatter;
	mat->scattering_pdf = lambertian_scattering_pdf;
	mat->destroy = lambertian_destroy;
	return (mat);
}

t_material	*metal_create_fuzz(t_color albedo, real_t fuzz)
{
	t_material	*mat;
	t_metal		*metal;

	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	metal = (t_metal *)malloc(sizeof(t_metal));
	if (!metal)
		return (free(mat), NULL);
	if (fuzz < (real_t)0.0)
		fuzz = (real_t)0.0;
	else if (fuzz > (real_t)1.0)
		fuzz = (real_t)1.0;
	metal->albedo = albedo;
	metal->fuzz = fuzz;
	mat->data = metal;
	mat->emitted = default_emitted;
	mat->scatter = metal_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = metal_destroy;
	return (mat);
}

t_material	*metal_create(t_color albedo)
{
	return (metal_create_fuzz(albedo, (real_t)0.0));
}

t_material	*dielectric_create(real_t refraction_index)
{
	t_material		*mat;
	t_dielectric	*dielec;

	mat = (t_material *)malloc(sizeof(t_material));
	if (!mat)
		return (NULL);
	dielec = (t_dielectric *)malloc(sizeof(t_dielectric));
	if (!dielec)
		return (free(mat), NULL);
	dielec->refraction_index = refraction_index;
	mat->data = dielec;
	mat->emitted = default_emitted;
	mat->scatter = dielectric_scatter;
	mat->scattering_pdf = default_scattering_pdf;
	mat->destroy = dielectric_destroy;
	return (mat);
}
