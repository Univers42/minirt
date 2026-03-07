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

bool	glossy_scatter(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_glossy	*glossy;
	t_vec3			reflected;
	t_vec3			direction;

	glossy = (const t_glossy *)mat->data;
	if (!glossy)
		return (false);
	*attenuation = glossy->albedo;
	if (random_double() > glossy->roughness)
	{
		reflected = vec3_reflect(&r_in->dir, &rec->normal);
		direction = unit_vector(&reflected);
		reflected = random_unit_vector();
		reflected = vec3_mul_scalar(&reflected, glossy->roughness * 0.5);
		direction = vec3_add(&direction, &reflected);
		if (dot(&direction, &rec->normal) <= 0)
			direction = rec->normal;
		*scattered = ray_create(rec->p, direction, r_in->tm);
	}
	else
	{
		direction = random_cosine_direction(&rec->normal);
		if (vec3_near_zero(&direction))
			direction = rec->normal;
		*scattered = ray_create(rec->p, direction, r_in->tm);
	}
	return (true);
}

bool	diffuse_light_scatter(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	(void)mat;
	(void)r_in;
	(void)rec;
	(void)attenuation;
	(void)scattered;
	return (false);
}

bool	isotropic_scatter(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_isotropic	*iso;
	t_vec3				scattered_dir;

	iso = (const t_isotropic *)mat->data;
	if (!iso || !iso->tex)
		return (false);
	scattered_dir = random_unit_vector();
	*scattered = ray_create(rec->p, scattered_dir, r_in->tm);
	*attenuation = iso->tex->value(iso->tex, rec->u, rec->v, &rec->p);
	return (true);
}

void	lambertian_destroy(t_material *mat)
{
	if (!mat || !mat->data)
		return ;
	free(mat->data);
	mat->data = NULL;
}

void	metal_destroy(t_material *mat)
{
	if (mat && mat->data)
	{
		free(mat->data);
		mat->data = NULL;
	}
}
