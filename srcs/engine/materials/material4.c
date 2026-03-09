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
	if (mat && mat->data)
	{
		free(mat->data);
		mat->data = NULL;
	}
}

void	tinted_glass_destroy(t_material *mat)
{
	if (mat && mat->data)
	{
		free(mat->data);
		mat->data = NULL;
	}
}

void	glossy_destroy(t_material *mat)
{
	if (mat && mat->data)
	{
		free(mat->data);
		mat->data = NULL;
	}
}

void	diffuse_light_destroy(t_material *mat)
{
	t_diffuse_light	*light;

	if (!mat || !mat->data)
		return ;
	light = (t_diffuse_light *)mat->data;
	if (light->tex && light->tex->destroy)
	{
		light->tex->destroy(light->tex);
		free(light->tex);
		light->tex = NULL;
	}
	free(mat->data);
	mat->data = NULL;
}

void	isotropic_destroy(t_material *mat)
{
	if (!mat || !mat->data)
		return ;
	free(mat->data);
	mat->data = NULL;
}
