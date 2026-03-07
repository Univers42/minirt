/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_mesh2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:08:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include <math.h>

bool	wf_mesh_add_tri(t_wf_mesh *mesh, const t_wf_triangle *tri);

static t_wf_vec3	wf_get_vert(const t_wf_model *m, int32_t idx)
{
	if (idx >= 1 && (size_t)idx <= m->nverts)
		return (m->verts[idx - 1]);
	return ((t_wf_vec3){0, 0, 0});
}

static t_wf_vec3	wf_get_norm(const t_wf_model *m, int32_t idx)
{
	if (idx >= 1 && (size_t)idx <= m->nnorms)
		return (m->norms[idx - 1]);
	return ((t_wf_vec3){0, 0, 0});
}

static t_wf_vec2	wf_get_texcoord(const t_wf_model *m, int32_t idx)
{
	if (idx >= 1 && (size_t)idx <= m->ntexcoords)
		return (m->texcoords[idx - 1]);
	return ((t_wf_vec2){0, 0});
}

static t_wf_vec3	wf_compute_face_normal(t_wf_vec3 a, t_wf_vec3 b,
		t_wf_vec3 c)
{
	t_wf_vec3	e1;
	t_wf_vec3	e2;
	t_wf_vec3	n;
	float		len;

	e1.x = b.x - a.x;
	e1.y = b.y - a.y;
	e1.z = b.z - a.z;
	e2.x = c.x - a.x;
	e2.y = c.y - a.y;
	e2.z = c.z - a.z;
	n.x = e1.y * e2.z - e1.z * e2.y;
	n.y = e1.z * e2.x - e1.x * e2.z;
	n.z = e1.x * e2.y - e1.y * e2.x;
	len = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
	if (len > 1e-8f)
	{
		n.x /= len;
		n.y /= len;
		n.z /= len;
	}
	return (n);
}

void	wf_fill_tri(t_wf_triangle *tri, const t_wf_model *m,
		const t_wf_face *f, const int idx[3])
{
	int	i;

	i = 0;
	while (i < 3)
	{
		tri->v[i] = wf_get_vert(m, f->verts[idx[i]].vi);
		tri->has_normals = (f->verts[idx[0]].ni > 0);
		tri->has_texcoords = (f->verts[idx[0]].ti > 0);
		if (tri->has_normals)
			tri->n[i] = wf_get_norm(m, f->verts[idx[i]].ni);
		if (tri->has_texcoords)
			tri->t[i] = wf_get_texcoord(m, f->verts[idx[i]].ti);
		i++;
	}
	if (!tri->has_normals)
	{
		tri->n[0] = wf_compute_face_normal(tri->v[0], tri->v[1], tri->v[2]);
		tri->n[1] = tri->n[0];
		tri->n[2] = tri->n[0];
	}
}
