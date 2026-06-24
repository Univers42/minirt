/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 15:06:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"
#include "random.h"

static t_vec3	metal_fuzzed_dir(const t_metal *metal, const t_ray *r_in,
					const t_hit_record *rec)
{
	t_vec3	reflected;
	t_vec3	random_vec;

	reflected = vec3_reflect(&r_in->dir, &rec->normal);
	reflected = unit_vector(&reflected);
	random_vec = random_unit_vector();
	random_vec = vec3_mul_scalar(&random_vec, metal->fuzz);
	return (vec3_add(&reflected, &random_vec));
}

bool	metal_scatter(t_scatter *s)
{
	const t_metal	*metal;
	t_vec3			fuzzed;

	metal = (const t_metal *)s->mat->data;
	if (!metal)
		return (false);
	fuzzed = metal_fuzzed_dir(metal, s->r_in, s->rec);
	*s->scattered = ray_create(s->rec->p, fuzzed, s->r_in->tm);
	*s->attenuation = metal->albedo;
	return (dot(&fuzzed, &s->rec->normal) > 0.0);
}

static bool	dielectric_get_dir(const t_hit_record *rec,
				const t_vec3 *unit_dir, real_t ri, t_vec3 *direction)
{
	t_vec3	neg_unit_dir;
	real_t	cos_theta;
	real_t	sin_theta;
	bool	should_reflect;

	neg_unit_dir = vec3_neg(unit_dir);
	cos_theta = dot(&neg_unit_dir, &rec->normal);
	if (cos_theta > 1.0)
		cos_theta = 1.0;
	sin_theta = (real_t)sqrt(1.0 - (double)(cos_theta * cos_theta));
	should_reflect = (ri * sin_theta > 1.0)
		|| (reflectance(cos_theta, ri) > random_double());
	if (should_reflect)
		*direction = vec3_reflect(unit_dir, &rec->normal);
	else
		*direction = vec3_refract(unit_dir, &rec->normal, ri);
	return (true);
}

bool	dielectric_scatter(t_scatter *s)
{
	const t_dielectric	*dielec;
	real_t				ri;
	t_vec3				unit_direction;
	t_vec3				direction;

	dielec = (const t_dielectric *)s->mat->data;
	if (!dielec)
		return (false);
	*s->attenuation = vec3_create(1.0, 1.0, 1.0);
	if (s->rec->front_face)
		ri = 1.0 / dielec->refraction_index;
	else
		ri = dielec->refraction_index;
	unit_direction = unit_vector(&s->r_in->dir);
	dielectric_get_dir(s->rec, &unit_direction, ri, &direction);
	*s->scattered = ray_create(s->rec->p, direction, s->r_in->tm);
	return (true);
}

bool	tinted_glass_scatter(t_scatter *s)
{
	const t_tinted_glass	*glass;
	real_t					ri;
	t_vec3					unit_direction;
	t_vec3					direction;

	glass = (const t_tinted_glass *)s->mat->data;
	if (!glass)
		return (false);
	*s->attenuation = glass->tint;
	if (s->rec->front_face)
		ri = 1.0 / glass->refraction_index;
	else
		ri = glass->refraction_index;
	unit_direction = unit_vector(&s->r_in->dir);
	dielectric_get_dir(s->rec, &unit_direction, ri, &direction);
	*s->scattered = ray_create(s->rec->p, direction, s->r_in->tm);
	return (true);
}
