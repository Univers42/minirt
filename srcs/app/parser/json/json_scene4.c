/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_scene4.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:40:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:55:47 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_scene.h"
#include "json_helpers.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Fill a mesh (OBJ) object from JSON                                */
/*  { "type":"mesh", "path":"model.obj", "position":[0,0,0],        */
/*    "scale":4.0, "color":[200,200,200] }                           */
/* ------------------------------------------------------------------ */

bool	fill_mesh(t_rt_object *out, const t_json_node *o)
{
	const char	*path;

	out->type = OBJ_MESH;
	path = json_str(json_get(o, "path"), "");
	strncpy(out->data.mesh.path, path,
		sizeof(out->data.mesh.path) - 1);
	out->data.mesh.path[sizeof(out->data.mesh.path) - 1] = '\0';
	out->data.mesh.position = json_to_vec3(json_get(o, "position"));
	out->data.mesh.scale = json_num(json_get(o, "scale"), 4.0);
	out->data.mesh.color = json_to_color255(json_get(o, "color"));
	return (path[0] != '\0');
}

/* ------------------------------------------------------------------ */
/*  Parse optional "render" block from JSON                           */
/*                                                                    */
/*  Accepted fields:                                                  */
/*    "width"       : int   — output image width in pixels            */
/*    "aspect_ratio": float — width/height ratio                      */
/*    "spp"         : int   — samples per pixel                       */
/*    "max_depth"   : int   — max ray bounce depth                    */
/*    "color_space" : str   — "srgb" or "spectrum"                    */
/*                                                                    */
/*  All fields are optional.  Values override studio_config.h defs.   */
/* ------------------------------------------------------------------ */

static void	parse_render_cspace(const t_json_node *obj, t_scene *sc)
{
	const char	*cs;

	if (!json_get(obj, "color_space"))
		return ;
	cs = json_str(json_get(obj, "color_space"), "srgb");
	if (strcmp(cs, "spectrum") == 0)
		sc->render.color_space = 1;
	else
		sc->render.color_space = 0;
	sc->render.has_color_space = true;
}

bool	parse_render_block(const t_json_node *obj, t_scene *sc)
{
	if (!obj || obj->type != JSON_OBJECT)
		return (true);
	if (json_get(obj, "width"))
	{
		sc->render.width = (int)json_num(json_get(obj, "width"), 800);
		sc->render.has_width = true;
	}
	if (json_get(obj, "aspect_ratio"))
	{
		sc->render.aspect_ratio = json_num(json_get(obj,
					"aspect_ratio"), 1.7778);
		sc->render.has_aspect = true;
	}
	if (json_get(obj, "spp"))
	{
		sc->render.spp = (int)json_num(json_get(obj, "spp"), 400);
		sc->render.has_spp = true;
	}
	if (json_get(obj, "max_depth"))
	{
		sc->render.max_depth = (int)json_num(json_get(obj,
					"max_depth"), 50);
		sc->render.has_depth = true;
	}
	parse_render_cspace(obj, sc);
	return (true);
}
