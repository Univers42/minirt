/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_color_glass.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 03:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 03:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shading.h"
#include "material.h"
#include <math.h>

/* Beer-less tint: tinted_glass carries a colour, plain dielectric is clear. */
static t_color	glass_tint(const t_material *mat)
{
	if (mat->scatter == tinted_glass_scatter)
		return (((t_tinted_glass *)mat->data)->tint);
	return (vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0));
}

/* Deterministic Whitted glass: trace BOTH the reflected and refracted rays
   and blend them by the Fresnel (Schlick) factor. On total internal
   reflection only the reflection survives. */
static t_color	glass_combine(const t_shade_ctx *c, const t_vec3 *ud,
					real_t ri, real_t cos_t)
{
	real_t	sin_t;
	real_t	fres;
	t_vec3	dir;
	t_ray	ray;
	t_color	refl;
	t_color	refr;
	t_color	a;

	sin_t = sqrt(fmax((real_t)0.0, (real_t)1.0 - cos_t * cos_t));
	dir = vec3_reflect(ud, &c->rec->normal);
	ray = ray_create(c->rec->p, dir, (real_t)0.0);
	refl = ray_color_direct(&ray, c->world, c->depth - 1, c->amb);
	if (ri * sin_t > (real_t)1.0)
		return (refl);
	fres = reflectance(cos_t, ri);
	dir = vec3_refract(ud, &c->rec->normal, ri);
	ray = ray_create(c->rec->p, dir, (real_t)0.0);
	refr = ray_color_direct(&ray, c->world, c->depth - 1, c->amb);
	a = vec3_mul_scalar(&refl, fres);
	refr = vec3_mul_scalar(&refr, (real_t)1.0 - fres);
	return (vec3_add(&a, &refr));
}

t_vec3	shade_glass(const t_shade_ctx *c)
{
	t_vec3	ud;
	t_vec3	neg;
	real_t	ri;
	real_t	cos_t;
	t_color	col;
	t_color	tint;

	ud = unit_vector(&c->r->dir);
	ri = ((t_dielectric *)c->rec->mat->data)->refraction_index;
	if (c->rec->front_face)
		ri = (real_t)1.0 / ri;
	neg = vec3_neg(&ud);
	cos_t = fmin(dot(&neg, &c->rec->normal), (real_t)1.0);
	col = glass_combine(c, &ud, ri, cos_t);
	tint = glass_tint(c->rec->mat);
	col = vec3_mul_elem(&col, &tint);
	return (vec3_add(&c->emission, &col));
}
