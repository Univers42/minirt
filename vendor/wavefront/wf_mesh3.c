/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_mesh3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:08:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include <string.h>
#include <math.h>
#include <float.h>
#include <stdio.h>

bool	wf_mesh_add_tri(t_wf_mesh *mesh, const t_wf_triangle *tri);
void	wf_fill_tri(t_wf_triangle *tri, const t_wf_model *m,
			const t_wf_face *f, const int idx[3]);

static bool	wf_triangulate_face(t_wf_mesh *mesh, const t_wf_model *m,
		const t_wf_face *f, int group_idx)
{
	t_wf_triangle	tri;
	int				idx[3];

	memset(&tri, 0, sizeof(tri));
	tri.group_idx = group_idx;
	idx[0] = 0;
	idx[1] = 1;
	idx[2] = 2;
	wf_fill_tri(&tri, m, f, idx);
	if (!wf_mesh_add_tri(mesh, &tri))
		return (false);
	if (f->nverts == 4)
	{
		idx[0] = 0;
		idx[1] = 2;
		idx[2] = 3;
		wf_fill_tri(&tri, m, f, idx);
		if (!wf_mesh_add_tri(mesh, &tri))
			return (false);
	}
	return (true);
}

bool	wf_triangulate(t_wf_mesh *mesh, const t_wf_model *m)
{
	size_t	gi;
	size_t	fi;

	gi = 0;
	while (gi < m->ngroups)
	{
		fi = 0;
		while (fi < m->groups[gi].nfaces)
		{
			if (!wf_triangulate_face(mesh, m,
					&m->groups[gi].faces[fi], (int)gi))
				return (false);
			fi++;
		}
		gi++;
	}
	wf_mesh_compute_bounds(mesh);
	return (true);
}

void	wf_mesh_compute_bounds(t_wf_mesh *mesh)
{
	size_t	i;
	int		j;

	mesh->bounds_min = (t_wf_vec3){FLT_MAX, FLT_MAX, FLT_MAX};
	mesh->bounds_max = (t_wf_vec3){-FLT_MAX, -FLT_MAX, -FLT_MAX};
	i = 0;
	while (i < mesh->ntris)
	{
		j = 0;
		while (j < 3)
		{
			if (mesh->tris[i].v[j].x < mesh->bounds_min.x)
				mesh->bounds_min.x = mesh->tris[i].v[j].x;
			if (mesh->tris[i].v[j].y < mesh->bounds_min.y)
				mesh->bounds_min.y = mesh->tris[i].v[j].y;
			if (mesh->tris[i].v[j].z < mesh->bounds_min.z)
				mesh->bounds_min.z = mesh->tris[i].v[j].z;
			if (mesh->tris[i].v[j].x > mesh->bounds_max.x)
				mesh->bounds_max.x = mesh->tris[i].v[j].x;
			if (mesh->tris[i].v[j].y > mesh->bounds_max.y)
				mesh->bounds_max.y = mesh->tris[i].v[j].y;
			if (mesh->tris[i].v[j].z > mesh->bounds_max.z)
				mesh->bounds_max.z = mesh->tris[i].v[j].z;
			j++;
		}
		i++;
	}
	mesh->center.x = (mesh->bounds_min.x + mesh->bounds_max.x) * 0.5f;
	mesh->center.y = (mesh->bounds_min.y + mesh->bounds_max.y) * 0.5f;
	mesh->center.z = (mesh->bounds_min.z + mesh->bounds_max.z) * 0.5f;
}

void	wf_mesh_print_info(const t_wf_mesh *mesh)
{
	fprintf(stderr, "WF Mesh: %zu triangles\n", mesh->ntris);
	fprintf(stderr, "  bounds: [%.3f,%.3f,%.3f] - [%.3f,%.3f,%.3f]\n",
		mesh->bounds_min.x, mesh->bounds_min.y, mesh->bounds_min.z,
		mesh->bounds_max.x, mesh->bounds_max.y, mesh->bounds_max.z);
	fprintf(stderr, "  center: [%.3f,%.3f,%.3f]\n",
		mesh->center.x, mesh->center.y, mesh->center.z);
}
