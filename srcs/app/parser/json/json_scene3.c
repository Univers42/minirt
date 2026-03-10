/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_scene3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:55:47 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_scene.h"
#include "json_helpers.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Map a material name string to t_mat_type                          */
/*  "lambertian" (default), "metal", "dielectric", "emissive",       */
/*  "checker", "glossy"                                               */
/* ------------------------------------------------------------------ */

static t_mat_type	mat_type_from_str(const char *s)
{
	if (strcmp(s, "metal") == 0)
		return (MAT_METAL);
	if (strcmp(s, "dielectric") == 0 || strcmp(s, "glass") == 0)
		return (MAT_DIELECTRIC);
	if (strcmp(s, "tinted_glass") == 0)
		return (MAT_TINTED_GLASS);
	if (strcmp(s, "emissive") == 0 || strcmp(s, "light") == 0)
		return (MAT_EMISSIVE);
	if (strcmp(s, "checker") == 0)
		return (MAT_CHECKER);
	if (strcmp(s, "glossy") == 0)
		return (MAT_GLOSSY);
	if (strcmp(s, "isotropic") == 0)
		return (MAT_ISOTROPIC);
	return (MAT_LAMBERTIAN);
}

/* ------------------------------------------------------------------ */
/*  Parse optional "material" field on a JSON object                  */
/*                                                                    */
/*  Accepted forms:                                                   */
/*    "material": "metal"              – shorthand                    */
/*    "material": {                                                    */
/*      "type": "metal",                                              */
/*      "fuzz": 0.3,                                                  */
/*      "ior": 1.5,                                                   */
/*      "scale": 5.0,                                                 */
/*      "roughness": 0.1,                                             */
/*      "color2": [0,0,0]                                             */
/*    }                                                                */
/* ------------------------------------------------------------------ */

static void	parse_mat_obj(const t_json_node *m, t_mat_spec *ms)
{
	ms->type = mat_type_from_str(json_str(json_get(m, "type"), ""));
	ms->fuzz = json_num(json_get(m, "fuzz"), 0.0);
	ms->ior = json_num(json_get(m, "ior"), 1.5);
	ms->scale = json_num(json_get(m, "scale"), 0.0);
	ms->roughness = json_num(json_get(m, "roughness"), 0.0);
	ms->color2 = json_to_color255(json_get(m, "color2"));
}

void	parse_mat_spec(const t_json_node *o, t_rt_object *out)
{
	const t_json_node	*m;

	m = json_get(o, "material");
	if (!m)
		return ;
	if (m->type == JSON_STRING)
	{
		out->mat.type = mat_type_from_str(m->val.str);
		out->mat.ior = 1.5;
		return ;
	}
	if (m->type == JSON_OBJECT)
		parse_mat_obj(m, &out->mat);
}
