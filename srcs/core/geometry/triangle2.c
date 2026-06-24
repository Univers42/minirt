/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:51:55 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 20:07:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "triangle.h"
#include "hittable_list.h"
#include <stdlib.h>

void	mesh_init(t_mesh *mesh)
{
	if (!mesh)
		return ;
	mesh->triangles = NULL;
	mesh->count = 0;
	mesh->capacity = 0;
	mesh->bbox = aabb_empty();
}

bool	mesh_add_triangle(t_mesh *mesh, const t_triangle *tri)
{
	size_t		newcap;
	t_triangle	*newarr;

	if (!mesh || !tri)
		return (false);
	if (mesh->count >= mesh->capacity)
	{
		newcap = 16;
		if (mesh->capacity != 0)
			newcap = mesh->capacity * 2;
		newarr = (t_triangle *)realloc(mesh->triangles,
				newcap * sizeof(t_triangle));
		if (!newarr)
			return (false);
		mesh->triangles = newarr;
		mesh->capacity = newcap;
	}
	mesh->triangles[mesh->count++] = *tri;
	mesh->bbox = aabb_merge(&mesh->bbox, &tri->bbox);
	return (true);
}

void	mesh_clear(t_mesh *mesh)
{
	if (!mesh)
		return ;
	free(mesh->triangles);
	mesh->triangles = NULL;
	mesh->count = 0;
	mesh->capacity = 0;
	mesh->bbox = aabb_empty();
}

bool	mesh_add_to_list(const t_mesh *mesh, t_hittable_list *list)
{
	size_t	i;

	if (!mesh || !list)
		return (false);
	i = 0;
	while (i < mesh->count)
	{
		if (!hittable_list_add_triangle(list, &mesh->triangles[i]))
			return (false);
		i++;
	}
	return (true);
}

void	mesh_add_quad(t_mesh *mesh, const t_quad_pts *pts, t_material *mat)
{
	t_triangle	t1;
	t_triangle	t2;

	t1 = triangle_create(&pts->a, &pts->b, &pts->c, mat);
	t2 = triangle_create(&pts->a, &pts->c, &pts->d, mat);
	mesh_add_triangle(mesh, &t1);
	mesh_add_triangle(mesh, &t2);
}
