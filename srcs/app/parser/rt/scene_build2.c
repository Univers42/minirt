/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_build2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:50:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "material.h"
#include "texture.h"
#include "quad.h"
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/*  Create material from specification                                */
/*  Falls back to lambertian if spec type is MAT_LAMBERTIAN or        */
/*  if allocation fails for the requested type.                       */
/* ------------------------------------------------------------------ */

static t_material	*create_mat_special(const t_mat_spec *ms, t_color clr)
{
	double	ior;
	double	rough;

	ior = ms->ior;
	if (ior <= 1.0)
		ior = 1.5;
	rough = ms->roughness;
	if (rough <= 0.0)
		rough = 0.3;
	if (ms->type == MAT_METAL)
		return (metal_create_fuzz(clr, ms->fuzz));
	if (ms->type == MAT_DIELECTRIC)
		return (dielectric_create(ior));
	if (ms->type == MAT_GLOSSY)
		return (glossy_create(clr, rough, ms->fuzz));
	if (ms->type == MAT_TINTED_GLASS)
		return (tinted_glass_create(ior, clr));
	if (ms->type == MAT_ISOTROPIC)
		return (isotropic_create(clr));
	return (NULL);
}

static t_material	*create_mat_emissive(const t_mat_spec *ms, t_color clr)
{
	if (ms->scale > 0.0)
		return (diffuse_light_create_scaled(clr, ms->scale));
	return (diffuse_light_create(clr));
}

static t_material	*create_mat_checker(const t_mat_spec *ms, t_color clr)
{
	t_texture	*even;
	t_texture	*odd;
	t_texture	*checker;
	double		scale;

	even = solid_color_create(clr);
	odd = solid_color_create(ms->color2);
	if (!even || !odd)
		return (NULL);
	scale = ms->scale;
	if (scale <= 0.0)
		scale = 10.0;
	checker = checker_texture_create(scale, even, odd);
	if (!checker)
		return (NULL);
	return (lambertian_create_texture(checker));
}

t_material	*create_mat_noise(const t_mat_spec *ms, t_color clr);

t_material	*create_material(const t_mat_spec *ms, t_color clr)
{
	t_texture	*img;
	t_material	*m;

	m = NULL;
	if (ms->texture_path[0])
	{
		img = image_texture_create_png(ms->texture_path);
		if (img)
			m = lambertian_create_texture(img);
	}
	if (!m && ms->type == MAT_EMISSIVE)
		m = create_mat_emissive(ms, clr);
	else if (!m && ms->type == MAT_CHECKER)
		m = create_mat_checker(ms, clr);
	else if (!m && (ms->type == MAT_MARBLE || ms->type == MAT_WOOD))
		m = create_mat_noise(ms, clr);
	else if (!m && ms->type != MAT_LAMBERTIAN)
		m = create_mat_special(ms, clr);
	else if (!m)
		m = lambertian_create(clr);
	mat_registry_add(m);
	return (m);
}

/* ------------------------------------------------------------------ */
/*  Build a quad directly into the world hittable list                */
/* ------------------------------------------------------------------ */

bool	build_quad_obj(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_quad		q;
	t_quad		*qp;

	mat = create_material(&obj->mat, obj->data.quad.color);
	if (!mat)
		return (false);
	q = quad_create(&obj->data.quad.position,
			&obj->data.quad.u, &obj->data.quad.v, mat);
	qp = (t_quad *)malloc(sizeof(t_quad));
	if (!qp)
		return (false);
	*qp = q;
	return (hittable_list_add_nonowned(world, qp,
			set_current_quad, quad_hit_noobj, &q.bbox));
}
