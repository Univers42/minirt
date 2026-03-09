/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_mesh4.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:08:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include <math.h>

void	wf_mesh_normalize(t_wf_mesh *mesh, float target_size)
{
	float	dx;
	float	dy;
	float	dz;
	float	s;
	size_t	i;

	dx = mesh->bounds_max.x - mesh->bounds_min.x;
	dy = mesh->bounds_max.y - mesh->bounds_min.y;
	dz = mesh->bounds_max.z - mesh->bounds_min.z;
	s = dx;
	if (dy > s)
		s = dy;
	if (dz > s)
		s = dz;
	if (s < 1e-6f)
		return ;
	s = target_size / s;
	mesh->scale = s;
	i = 0;
	while (i < mesh->ntris)
	{
		mesh->tris[i].v[0].x *= s;
		mesh->tris[i].v[0].y *= s;
		mesh->tris[i].v[0].z *= s;
		mesh->tris[i].v[1].x *= s;
		mesh->tris[i].v[1].y *= s;
		mesh->tris[i].v[1].z *= s;
		mesh->tris[i].v[2].x *= s;
		mesh->tris[i].v[2].y *= s;
		mesh->tris[i].v[2].z *= s;
		i++;
	}
	wf_mesh_compute_bounds(mesh);
}

void	wf_mesh_center(t_wf_mesh *mesh)
{
	float	cx;
	float	cy;
	float	cz;
	size_t	i;
	int		j;

	cx = mesh->center.x;
	cy = mesh->center.y;
	cz = mesh->center.z;
	i = 0;
	while (i < mesh->ntris)
	{
		j = 0;
		while (j < 3)
		{
			mesh->tris[i].v[j].x -= cx;
			mesh->tris[i].v[j].y -= cy;
			mesh->tris[i].v[j].z -= cz;
			j++;
		}
		i++;
	}
	wf_mesh_compute_bounds(mesh);
}

void	wf_mesh_translate(t_wf_mesh *mesh, float dx, float dy, float dz)
{
	size_t	i;
	int		j;

	i = 0;
	while (i < mesh->ntris)
	{
		j = 0;
		while (j < 3)
		{
			mesh->tris[i].v[j].x += dx;
			mesh->tris[i].v[j].y += dy;
			mesh->tris[i].v[j].z += dz;
			j++;
		}
		i++;
	}
	wf_mesh_compute_bounds(mesh);
}

void	wf_mesh_scale(t_wf_mesh *mesh, float sx, float sy, float sz)
{
	size_t	i;
	int		j;

	i = 0;
	while (i < mesh->ntris)
	{
		j = 0;
		while (j < 3)
		{
			mesh->tris[i].v[j].x *= sx;
			mesh->tris[i].v[j].y *= sy;
			mesh->tris[i].v[j].z *= sz;
			j++;
		}
		i++;
	}
	wf_mesh_compute_bounds(mesh);
}

void	wf_mesh_gen_normals(t_wf_mesh *mesh)
{
	size_t		i;
	t_wf_vec3	e1;
	t_wf_vec3	e2;
	t_wf_vec3	n;
	float		len;

	i = 0;
	while (i < mesh->ntris)
	{
		e1.x = mesh->tris[i].v[1].x - mesh->tris[i].v[0].x;
		e1.y = mesh->tris[i].v[1].y - mesh->tris[i].v[0].y;
		e1.z = mesh->tris[i].v[1].z - mesh->tris[i].v[0].z;
		e2.x = mesh->tris[i].v[2].x - mesh->tris[i].v[0].x;
		e2.y = mesh->tris[i].v[2].y - mesh->tris[i].v[0].y;
		e2.z = mesh->tris[i].v[2].z - mesh->tris[i].v[0].z;
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
		mesh->tris[i].n[0] = n;
		mesh->tris[i].n[1] = n;
		mesh->tris[i].n[2] = n;
		mesh->tris[i].has_normals = true;
		i++;
	}
}
