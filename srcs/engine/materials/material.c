/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 15:06:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"

real_t	reflectance(real_t cosine, real_t refraction_index)
{
	real_t	r0;
	real_t	omc;

	r0 = (1.0 - refraction_index) / (1.0 + refraction_index);
	r0 = r0 * r0;
	omc = 1.0 - cosine;
	return (r0 + (1.0 - r0) * omc * omc * omc * omc * omc);
}

t_color	default_emitted(t_emit *e)
{
	(void)e;
	return (vec3_create(0.0, 0.0, 0.0));
}

real_t	default_scattering_pdf(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, const t_ray *scattered)
{
	(void)mat;
	(void)r_in;
	(void)rec;
	(void)scattered;
	return ((real_t)0.0);
}

real_t	lambertian_scattering_pdf(const t_material *mat, const t_ray *r_in,
			const t_hit_record *rec, const t_ray *scattered)
{
	real_t	cos_theta;

	(void)mat;
	(void)r_in;
	cos_theta = dot(&rec->normal, &scattered->dir);
	if (cos_theta < (real_t)0.0)
		return ((real_t)0.0);
	return (cos_theta / (real_t)PI);
}

bool	lambertian_scatter(t_scatter *s)
{
	const t_lambertian	*lamb;
	t_vec3				scatter_direction;

	lamb = (const t_lambertian *)s->mat->data;
	if (!lamb || !lamb->tex)
		return (false);
	scatter_direction = random_cosine_direction(&s->rec->normal);
	if (vec3_near_zero(&scatter_direction))
		scatter_direction = s->rec->normal;
	*s->scattered = ray_create(s->rec->p, scatter_direction, s->r_in->tm);
	*s->attenuation = lamb->tex->value(lamb->tex, s->rec->u,
			s->rec->v, &s->rec->p);
	return (true);
}
