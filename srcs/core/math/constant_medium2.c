/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constant_medium2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:02:40 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:04:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "constant_medium.h"

/* Constructor: constant medium with texture */
t_constant_medium	*constant_medium_create_texture(
		const t_hittable_wrapper *boundary, real_t density, t_texture *tex)
{
	t_constant_medium	*medium;

	if (!boundary || density <= (real_t)0.0 || !tex)
		return (NULL);
	medium = (t_constant_medium *)malloc(sizeof(t_constant_medium));
	if (!medium)
		return (NULL);
	medium->boundary = *boundary;
	medium->neg_inv_density = (real_t)-1.0 / density;
	medium->phase_function = isotropic_create_texture(tex);
	if (!medium->phase_function)
	{
		free(medium);
		return (NULL);
	}
	medium->bbox = boundary->bbox;
	return (medium);
}

/* Constructor: constant medium with color albedo */
t_constant_medium	*constant_medium_create_color(
		const t_hittable_wrapper *boundary, real_t density, t_color albedo)
{
	t_constant_medium	*medium;

	if (!boundary || density <= (real_t)0.0)
		return (NULL);
	medium = (t_constant_medium *)malloc(sizeof(t_constant_medium));
	if (!medium)
		return (NULL);
	medium->boundary = *boundary;
	medium->neg_inv_density = (real_t)-1.0 / density;
	medium->phase_function = isotropic_create(albedo);
	if (!medium->phase_function)
	{
		free(medium);
		return (NULL);
	}
	medium->bbox = boundary->bbox;
	return (medium);
}

/* Get bounding box */
t_aabb	constant_medium_bounding_box(const t_constant_medium *medium)
{
	if (!medium)
		return (aabb_empty());
	return (medium->bbox);
}

/* Destructor: cleanup owned material (boundary is not owned) */
void	constant_medium_destroy(t_constant_medium *medium)
{
	if (!medium)
		return ;
	if (medium->phase_function)
	{
		medium->phase_function->destroy(medium->phase_function);
		free(medium->phase_function);
		medium->phase_function = NULL;
	}
	free(medium);
}
