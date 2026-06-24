/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_scene2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:50:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_scene.h"
#include "json_helpers.h"
#include <stdio.h>
#include <string.h>
#include "libft.h"

/* scene_build2.c (json_scene3.c) */
void	parse_mat_spec(const t_json_node *o, t_rt_object *out);

/* json_scene4.c */
bool	fill_mesh(t_rt_object *out, const t_json_node *o);

/* ------------------------------------------------------------------ */
/*  Fill cylinder from JSON                                           */
/* ------------------------------------------------------------------ */

static bool	fill_cylinder(t_rt_object *out, const t_json_node *o)
{
	out->type = OBJ_CYLINDER;
	out->data.cylinder.center = json_to_vec3(json_get(o, "center"));
	out->data.cylinder.axis = json_to_vec3(json_get(o, "axis"));
	out->data.cylinder.diameter = json_num(json_get(o, "diameter"), 1);
	out->data.cylinder.height = json_num(json_get(o, "height"), 1);
	out->data.cylinder.color = json_to_color255(json_get(o, "color"));
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Fill cone from JSON                                               */
/* ------------------------------------------------------------------ */

static bool	fill_cone(t_rt_object *out, const t_json_node *o)
{
	out->type = OBJ_CONE;
	out->data.cone.apex = json_to_vec3(json_get(o, "apex"));
	out->data.cone.axis = json_to_vec3(json_get(o, "axis"));
	out->data.cone.diameter = json_num(json_get(o, "diameter"), 1);
	out->data.cone.height = json_num(json_get(o, "height"), 1);
	out->data.cone.color = json_to_color255(json_get(o, "color"));
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Fill quad from JSON                                               */
/* ------------------------------------------------------------------ */

static bool	fill_quad(t_rt_object *out, const t_json_node *o)
{
	out->type = OBJ_QUAD;
	out->data.quad.position = json_to_vec3(json_get(o, "position"));
	out->data.quad.u = json_to_vec3(json_get(o, "u"));
	out->data.quad.v = json_to_vec3(json_get(o, "v"));
	out->data.quad.color = json_to_color255(json_get(o, "color"));
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Dispatch object type by string                                    */
/* ------------------------------------------------------------------ */

static bool	fill_extra(t_rt_object *out, const t_json_node *o,
		const char *type)
{
	if (ft_strcmp(type, "cylinder") == 0)
		return (fill_cylinder(out, o));
	if (ft_strcmp(type, "cone") == 0)
		return (fill_cone(out, o));
	if (ft_strcmp(type, "quad") == 0)
		return (fill_quad(out, o));
	if (ft_strcmp(type, "mesh") == 0 || ft_strcmp(type, "obj") == 0)
		return (fill_mesh(out, o));
	fprintf(stderr, "Warning: unknown object type \"%s\"\n", type);
	return (false);
}

/* ------------------------------------------------------------------ */
/*  Parse the "objects" array                                         */
/*  Called from json_scene.c — declared extern                        */
/* ------------------------------------------------------------------ */

/* Forward declaration used in json_scene.c */
bool	fill_obj(t_rt_object *out, const t_json_node *o);

bool	parse_json_objects(const t_json_node *arr, t_scene *sc)
{
	size_t		i;
	t_json_node	*o;
	const char	*type;

	if (!arr || arr->type != JSON_ARRAY)
		return (true);
	i = 0;
	while (i < json_arr_len(arr) && sc->object_count < RT_MAX_OBJECTS)
	{
		o = json_arr_at(arr, i);
		type = json_str(json_get(o, "type"), "");
		if (!type[0])
		{
			i++;
			continue ;
		}
		if (!fill_obj(&sc->objects[sc->object_count], o))
			if (!fill_extra(&sc->objects[sc->object_count], o, type))
			{
				i++;
				continue ;
			}
		parse_mat_spec(o, &sc->objects[sc->object_count]);
		sc->object_count++;
		i++;
	}
	return (true);
}
