/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_color_shade.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 03:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 03:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shading.h"
#include "material.h"

/* Matte surface (lambertian / isotropic): ambient + direct diffuse +
   specular shine.  No indirect bounce — the classic ray-tracing model. */
t_vec3	shade_matte(const t_shade_ctx *c)
{
	t_color	albedo;
	t_ray	scattered;
	t_vec3	view;
	t_color	amb;
	t_color	lit;

	albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	c->rec->mat->scatter(c->rec->mat, c->r, c->rec, &albedo, &scattered);
	view = vec3_neg(&c->r->dir);
	view = unit_vector(&view);
	amb = vec3_mul_elem(c->amb, &albedo);
	amb = vec3_mul_scalar(&amb, ambient_occlusion(c->rec, c->world));
	lit = direct_lighting(c->rec, c->world, &view, &albedo);
	amb = vec3_add(&amb, &lit);
	return (vec3_add(&c->emission, &amb));
}

/* Reflective surface (metal / glossy): recurse along the scattered
   (reflected) ray and tint by the material albedo. */
t_vec3	shade_reflective(const t_shade_ctx *c)
{
	t_color	att;
	t_ray	scattered;
	t_color	rcol;
	t_color	out;

	att = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	if (!c->rec->mat->scatter(c->rec->mat, c->r, c->rec, &att, &scattered))
		return (c->emission);
	rcol = ray_color_direct(&scattered, c->world, c->depth - 1, c->amb);
	out = vec3_mul_elem(&att, &rcol);
	return (vec3_add(&c->emission, &out));
}
