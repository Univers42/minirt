/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle6.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "triangle.h"

static void	icosa_verts(const t_point3 *c, real_t a, real_t b, t_point3 v[12])
{
	v[0] = point3_create(c->x - a, c->y + b, c->z);
	v[1] = point3_create(c->x + a, c->y + b, c->z);
	v[2] = point3_create(c->x - a, c->y - b, c->z);
	v[3] = point3_create(c->x + a, c->y - b, c->z);
	v[4] = point3_create(c->x, c->y - a, c->z + b);
	v[5] = point3_create(c->x, c->y + a, c->z + b);
	v[6] = point3_create(c->x, c->y - a, c->z - b);
	v[7] = point3_create(c->x, c->y + a, c->z - b);
	v[8] = point3_create(c->x + b, c->y, c->z - a);
	v[9] = point3_create(c->x + b, c->y, c->z + a);
	v[10] = point3_create(c->x - b, c->y, c->z - a);
	v[11] = point3_create(c->x - b, c->y, c->z + a);
}

static void	icosa_faces(int faces[20][3])
{
	const int	src[20][3] = {{0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10},
	{0, 10, 11}, {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
	{3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9}, {4, 9, 5},
	{2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}};

	memcpy(faces, src, sizeof(src));
}

static void	icosa_emit(t_mesh *mesh, const t_point3 v[12], int faces[20][3],
		t_material *mat)
{
	t_triangle	tri;
	int			i;

	i = -1;
	while (++i < 20)
	{
		tri = triangle_create(&v[faces[i][0]], &v[faces[i][1]],
				&v[faces[i][2]], mat);
		mesh_add_triangle(mesh, &tri);
	}
}

void	mesh_add_icosahedron(t_mesh *mesh, const t_point3 *center,
		real_t radius, t_material *mat)
{
	const real_t	phi = (real_t)((1.0 + sqrt(5.0)) / 2.0);
	real_t			len;
	t_point3		v[12];
	int				faces[20][3];

	if (!mesh || !center || !mat || radius <= 0)
		return ;
	len = (real_t)sqrt(1.0 + (double)(phi * phi));
	icosa_verts(center, radius / len, radius * phi / len, v);
	icosa_faces(faces);
	icosa_emit(mesh, v, faces, mat);
}
