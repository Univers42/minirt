/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material4.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:03 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"

void	dielectric_destroy(t_material *mat)
{
	if (!mat)
		return ;
	free(mat->data);
	free(mat);
}

void	tinted_glass_destroy(t_material *mat)
{
	if (!mat)
		return ;
	free(mat->data);
	free(mat);
}

void	glossy_destroy(t_material *mat)
{
	if (!mat)
		return ;
	free(mat->data);
	free(mat);
}

void	diffuse_light_destroy(t_material *mat)
{
	t_diffuse_light	*light;

	if (!mat)
		return ;
	light = (t_diffuse_light *)mat->data;
	if (light && light->tex)
	{
		if (light->tex->destroy)
			light->tex->destroy(light->tex);
		free(light->tex);
	}
	free(mat->data);
	free(mat);
}

void	isotropic_destroy(t_material *mat)
{
	t_isotropic	*iso;

	if (!mat)
		return ;
	iso = (t_isotropic *)mat->data;
	if (iso && iso->tex)
	{
		if (iso->tex->destroy)
			iso->tex->destroy(iso->tex);
		free(iso->tex);
	}
	free(mat->data);
	free(mat);
}
