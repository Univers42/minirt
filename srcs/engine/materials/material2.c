/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"
#include "random.h"

bool	metal_scatter(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_metal	*metal;
	t_vec3			reflected;
	t_vec3			unit_reflected;
	t_vec3			random_vec;
	t_vec3			fuzz_offset;
	t_vec3			fuzzed;

	metal = (const t_metal *)mat->data;
	if (!metal)
		return (false);
	reflected = vec3_reflect(&r_in->dir, &rec->normal);
	unit_reflected = unit_vector(&reflected);
	random_vec = random_unit_vector();
	fuzz_offset = vec3_mul_scalar(&random_vec, metal->fuzz);
	fuzzed = vec3_add(&unit_reflected, &fuzz_offset);
	*scattered = ray_create(rec->p, fuzzed, r_in->tm);
	*attenuation = metal->albedo;
	return (dot(&fuzzed, &rec->normal) > 0.0);
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

bool	dielectric_scatter(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_dielectric	*dielec;
	real_t				ri;
	t_vec3				unit_direction;
	t_vec3				direction;

	dielec = (const t_dielectric *)mat->data;
	if (!dielec)
		return (false);
	*attenuation = vec3_create(1.0, 1.0, 1.0);
	if (rec->front_face)
		ri = 1.0 / dielec->refraction_index;
	else
		ri = dielec->refraction_index;
	unit_direction = unit_vector(&r_in->dir);
	dielectric_get_dir(rec, &unit_direction, ri, &direction);
	*scattered = ray_create(rec->p, direction, r_in->tm);
	return (true);
}

bool	tinted_glass_scatter(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, t_color *attenuation, t_ray *scattered)
{
	const t_tinted_glass	*glass;
	real_t					ri;
	t_vec3					unit_direction;
	t_vec3					direction;

	glass = (const t_tinted_glass *)mat->data;
	if (!glass)
		return (false);
	*attenuation = glass->tint;
	if (rec->front_face)
		ri = 1.0 / glass->refraction_index;
	else
		ri = glass->refraction_index;
	unit_direction = unit_vector(&r_in->dir);
	dielectric_get_dir(rec, &unit_direction, ri, &direction);
	*scattered = ray_create(rec->p, direction, r_in->tm);
	return (true);
}

t_color	diffuse_light_emitted(const t_material *mat, real_t u, real_t v,
			const t_point3 *p)
{
	const t_diffuse_light	*light;
	t_color					c;

	light = (const t_diffuse_light *)mat->data;
	if (!light || !light->tex)
		return (vec3_create(0.0, 0.0, 0.0));
	c = light->tex->value(light->tex, u, v, p);
	if (light->scale > (real_t)0.0)
		return (vec3_mul_scalar(&c, light->scale));
	return (vec3_mul_scalar(&c, (real_t)1.0));
}
