/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_run.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include "json_scene.h"
#include "live.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

bool	add_scene_lights(t_hittable_list *world, const t_scene *sc);

int	run_rt_display(const char *filepath, t_out_mode out)
{
	t_scene	scene;
	int		ret;

	if (!parse_scene(filepath, &scene))
		return (1);
	if (!build_scene_objects(&scene))
	{
		fprintf(stderr, "Error\nFailed to build RT scene objects\n");
		scene_cleanup(&scene);
		return (1);
	}
	if (!add_scene_lights(&scene.world, &scene))
	{
		scene_cleanup(&scene);
		return (1);
	}
	ret = display_scene(&scene, out);
	scene_cleanup(&scene);
	return (ret);
}

int	run_json(const char *filepath, t_out_mode out)
{
	t_scene	scene;
	int		ret;

	if (!json_parse_scene(filepath, &scene))
		return (1);
	if (!build_scene_objects(&scene))
	{
		fprintf(stderr, "Error\nFailed to build JSON scene objects\n");
		scene_cleanup(&scene);
		return (1);
	}
	if (!add_scene_lights(&scene.world, &scene))
	{
		scene_cleanup(&scene);
		return (1);
	}
	ret = display_scene(&scene, out);
	scene_cleanup(&scene);
	return (ret);
}

static bool	edit_parse(const char *filepath, t_scene *scene)
{
	const char	*ext;

	ext = get_ext(filepath);
	if (strcasecmp(ext, ".json") == 0)
		return (json_parse_scene(filepath, scene));
	return (parse_scene(filepath, scene));
}

int	run_edit(const char *filepath)
{
	t_scene	scene;
	int		ret;

	if (!edit_parse(filepath, &scene))
		return (1);
	if (!build_scene_objects(&scene))
	{
		fprintf(stderr, "Error\nFailed to build scene objects\n");
		scene_cleanup(&scene);
		return (1);
	}
	if (!add_scene_lights(&scene.world, &scene))
	{
		scene_cleanup(&scene);
		return (1);
	}
	ret = rt_live_run(&scene);
	scene_cleanup(&scene);
	return (ret);
}
