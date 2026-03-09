/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_mesh.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:08:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>

void	wf_mesh_init(t_wf_mesh *mesh)
{
	memset(mesh, 0, sizeof(*mesh));
	mesh->bounds_min = (t_wf_vec3){FLT_MAX, FLT_MAX, FLT_MAX};
	mesh->bounds_max = (t_wf_vec3){-FLT_MAX, -FLT_MAX, -FLT_MAX};
}

void	wf_mesh_free(t_wf_mesh *mesh)
{
	if (!mesh)
		return ;
	free(mesh->tris);
	memset(mesh, 0, sizeof(*mesh));
}

static bool	wf_mesh_grow(t_wf_mesh *mesh)
{
	size_t			newcap;
	t_wf_triangle	*newarr;

	newcap = 256;
	if (mesh->tri_cap != 0)
		newcap = mesh->tri_cap * 2;
	newarr = (t_wf_triangle *)realloc(mesh->tris,
			newcap * sizeof(t_wf_triangle));
	if (!newarr)
		return (false);
	mesh->tris = newarr;
	mesh->tri_cap = newcap;
	return (true);
}

bool	wf_mesh_add_tri(t_wf_mesh *mesh, const t_wf_triangle *tri)
{
	if (mesh->ntris >= mesh->tri_cap)
	{
		if (!wf_mesh_grow(mesh))
			return (false);
	}
	mesh->tris[mesh->ntris++] = *tri;
	return (true);
}
