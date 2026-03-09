/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_model3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:08:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static bool	wf_grow_faces(t_wf_group *g)
{
	size_t		newcap;
	t_wf_face	*newarr;

	newcap = 64;
	if (g->face_cap != 0)
		newcap = g->face_cap * 2;
	newarr = (t_wf_face *)realloc(g->faces, newcap * sizeof(t_wf_face));
	if (!newarr)
		return (false);
	g->faces = newarr;
	g->face_cap = newcap;
	return (true);
}

bool	wf_group_add_face(t_wf_model *m, const t_wf_face *f)
{
	t_wf_group	*g;

	if (!wf_model_ensure_group(m))
		return (false);
	g = &m->groups[m->ngroups - 1];
	if (g->nfaces >= g->face_cap)
	{
		if (!wf_grow_faces(g))
			return (false);
	}
	g->faces[g->nfaces++] = *f;
	return (true);
}

void	wf_model_set_material(t_wf_model *m, const char *name)
{
	t_wf_group	*g;

	if (!wf_model_ensure_group(m))
		return ;
	g = &m->groups[m->ngroups - 1];
	strncpy(g->material, name, 63);
	g->material[63] = '\0';
}

void	wf_model_compute_center(t_wf_model *m)
{
	float	dx;
	float	dy;
	float	dz;

	m->center.x = (m->bounds_min.x + m->bounds_max.x) * 0.5f;
	m->center.y = (m->bounds_min.y + m->bounds_max.y) * 0.5f;
	m->center.z = (m->bounds_min.z + m->bounds_max.z) * 0.5f;
	dx = m->bounds_max.x - m->bounds_min.x;
	dy = m->bounds_max.y - m->bounds_min.y;
	dz = m->bounds_max.z - m->bounds_min.z;
	m->scale = dx;
	if (dy > m->scale)
		m->scale = dy;
	if (dz > m->scale)
		m->scale = dz;
	if (m->scale < 1e-6f)
		m->scale = 1.0f;
}

void	wf_model_print_info(const t_wf_model *m)
{
	size_t	total_faces;
	size_t	i;

	total_faces = 0;
	i = 0;
	while (i < m->ngroups)
		total_faces += m->groups[i++].nfaces;
	fprintf(stderr, "WF Model: %zu verts, %zu norms, %zu texcoords\n",
		m->nverts, m->nnorms, m->ntexcoords);
	fprintf(stderr, "  %zu groups, %zu total faces\n",
		m->ngroups, total_faces);
	fprintf(stderr, "  bounds: [%.3f,%.3f,%.3f] - [%.3f,%.3f,%.3f]\n",
		m->bounds_min.x, m->bounds_min.y, m->bounds_min.z,
		m->bounds_max.x, m->bounds_max.y, m->bounds_max.z);
}
