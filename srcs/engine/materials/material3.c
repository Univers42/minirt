/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"
#include "random.h"

static t_vec3	glossy_spec_dir(const t_glossy *glossy, t_scatter *s)
{
	t_vec3	reflected;
	t_vec3	direction;

	reflected = vec3_reflect(&s->r_in->dir, &s->rec->normal);
	direction = unit_vector(&reflected);
	reflected = random_unit_vector();
	reflected = vec3_mul_scalar(&reflected, glossy->roughness * 0.5);
	direction = vec3_add(&direction, &reflected);
	if (dot(&direction, &s->rec->normal) <= 0)
		direction = s->rec->normal;
	return (direction);
}

static t_vec3	glossy_diff_dir(t_scatter *s)
{
	t_vec3	direction;

	direction = random_cosine_direction(&s->rec->normal);
	if (vec3_near_zero(&direction))
		direction = s->rec->normal;
	return (direction);
}

bool	glossy_scatter(t_scatter *s)
{
	const t_glossy	*glossy;
	t_vec3			direction;

	glossy = (const t_glossy *)s->mat->data;
	if (!glossy)
		return (false);
	*s->attenuation = glossy->albedo;
	if (random_double() > glossy->roughness)
		direction = glossy_spec_dir(glossy, s);
	else
		direction = glossy_diff_dir(s);
	*s->scattered = ray_create(s->rec->p, direction, s->r_in->tm);
	return (true);
}

bool	diffuse_light_scatter(t_scatter *s)
{
	(void)s;
	return (false);
}

bool	isotropic_scatter(t_scatter *s)
{
	const t_isotropic	*iso;
	t_vec3				scattered_dir;

	iso = (const t_isotropic *)s->mat->data;
	if (!iso || !iso->tex)
		return (false);
	scattered_dir = random_unit_vector();
	*s->scattered = ray_create(s->rec->p, scattered_dir, s->r_in->tm);
	*s->attenuation = iso->tex->value(iso->tex, s->rec->u,
			s->rec->v, &s->rec->p);
	return (true);
}
