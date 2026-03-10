/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj_parser.h"
#include "wf_rt.h"
#include <stdio.h>

bool	obj_parse_to_mesh(const char *path, t_mesh *mesh,
		t_material *mat, float target_size)
{
	if (!path || !mesh || !mat)
		return (false);
	if (!wf_obj_to_rt_mesh(path, mesh, mat, target_size))
	{
		fprintf(stderr, "obj_parser: failed to load '%s'\n", path);
		return (false);
	}
	return (true);
}

bool	obj_parse_to_mesh_at(const char *path, t_mesh *mesh,
		t_material *mat, float target_size,
		float px, float py, float pz)
{
	if (!path || !mesh || !mat)
		return (false);
	if (!wf_obj_to_rt_mesh_at(path, mesh, mat, target_size,
			px, py, pz))
	{
		fprintf(stderr, "obj_parser: failed to load '%s'\n", path);
		return (false);
	}
	return (true);
}

bool	obj_parse_to_list(const char *path, t_hittable_list *list,
		t_material *mat, float target_size,
		float px, float py, float pz)
{
	if (!path || !list || !mat)
		return (false);
	if (!wf_obj_to_hittable_list(path, list, mat, target_size,
			px, py, pz))
	{
		fprintf(stderr, "obj_parser: failed to load '%s'\n", path);
		return (false);
	}
	return (true);
}
