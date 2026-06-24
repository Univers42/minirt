/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_scene.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:44:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_scene.h"
#include "json_helpers.h"
#include <stdio.h>
#include <string.h>
#include "libft.h"

/* Forward declarations for json_scene2.c */
bool	parse_json_objects(const t_json_node *arr, t_scene *sc);

/* Forward declaration for json_scene3.c */
void	parse_mat_spec(const t_json_node *o, t_rt_object *out);

/* Forward declaration for json_scene4.c */
bool	parse_render_block(const t_json_node *obj, t_scene *sc);

/* ------------------------------------------------------------------ */
/*  Parse ambient light                                               */
/*  { "ratio": 0.2, "color": [255,255,255] }                         */
/* ------------------------------------------------------------------ */

static bool	parse_ambient(const t_json_node *obj, t_scene *sc)
{
	if (!obj || obj->type != JSON_OBJECT)
		return (true);
	sc->ambient.ratio = json_num(json_get(obj, "ratio"), 0.2);
	sc->ambient.color = json_to_color255(json_get(obj, "color"));
	sc->has_ambient = true;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Parse camera                                                      */
/*  { "position": [0,0,0], "direction": [0,0,-1], "fov": 70 }       */
/* ------------------------------------------------------------------ */

static bool	parse_camera(const t_json_node *obj, t_scene *sc)
{
	if (!obj || obj->type != JSON_OBJECT)
		return (false);
	sc->camera.pos = json_to_vec3(json_get(obj, "position"));
	sc->camera.dir = json_to_vec3(json_get(obj, "direction"));
	sc->camera.fov = json_num(json_get(obj, "fov"), 70.0);
	if (json_get(obj, "defocus_angle"))
	{
		sc->camera.defocus_angle = json_num(json_get(obj,
					"defocus_angle"), 0.0);
		sc->camera.has_defocus = true;
	}
	if (json_get(obj, "focus_dist"))
	{
		sc->camera.focus_dist = json_num(json_get(obj,
					"focus_dist"), 10.0);
		sc->camera.has_focus_dist = true;
	}
	if (json_get(obj, "vup"))
	{
		sc->camera.vup = json_to_vec3(json_get(obj, "vup"));
		sc->camera.has_vup = true;
	}
	sc->has_camera = true;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Parse a lights array                                              */
/*  [ { "position":[…], "brightness":0.6, "color":[…] }, … ]        */
/* ------------------------------------------------------------------ */

static bool	parse_lights(const t_json_node *arr, t_scene *sc)
{
	size_t			i;
	t_json_node		*l;

	if (!arr || arr->type != JSON_ARRAY)
		return (true);
	i = 0;
	while (i < json_arr_len(arr) && sc->light_count < RT_MAX_LIGHTS)
	{
		l = json_arr_at(arr, i);
		sc->lights[sc->light_count].pos
			= json_to_vec3(json_get(l, "position"));
		sc->lights[sc->light_count].brightness
			= json_num(json_get(l, "brightness"), 0.6);
		sc->lights[sc->light_count].color
			= json_to_color255(json_get(l, "color"));
		sc->light_count++;
		i++;
	}
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Fill one t_rt_object from a JSON dict                             */
/* ------------------------------------------------------------------ */

bool	fill_obj(t_rt_object *out, const t_json_node *o)
{
	const char	*type;

	type = json_str(json_get(o, "type"), "");
	if (ft_strcmp(type, "sphere") == 0)
	{
		out->type = OBJ_SPHERE;
		out->data.sphere.center = json_to_vec3(json_get(o, "center"));
		out->data.sphere.diameter = json_num(json_get(o, "diameter"), 1);
		out->data.sphere.color = json_to_color255(json_get(o, "color"));
	}
	else if (ft_strcmp(type, "plane") == 0)
	{
		out->type = OBJ_PLANE;
		out->data.plane.point = json_to_vec3(json_get(o, "point"));
		out->data.plane.normal = json_to_vec3(json_get(o, "normal"));
		out->data.plane.color = json_to_color255(json_get(o, "color"));
	}
	else
		return (false);
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Public entry point                                                */
/* ------------------------------------------------------------------ */

bool	json_parse_scene(const char *filepath, t_scene *scene)
{
	t_json_node	*root;

	root = json_parse_file(filepath);
	if (!root || root->type != JSON_OBJECT)
	{
		fprintf(stderr, "Error\nFailed to parse JSON scene: %s\n",
			filepath);
		json_free(root);
		return (false);
	}
	scene_init(scene);
	parse_ambient(json_get(root, "ambient"), scene);
	parse_render_block(json_get(root, "render"), scene);
	if (!parse_camera(json_get(root, "camera"), scene))
	{
		json_free(root);
		return (fprintf(stderr, "Error\nJSON scene: missing camera\n"),
			false);
	}
	parse_lights(json_get(root, "lights"), scene);
	if (!parse_json_objects(json_get(root, "objects"), scene))
	{
		json_free(root);
		return (false);
	}
	json_free(root);
	return (true);
}
