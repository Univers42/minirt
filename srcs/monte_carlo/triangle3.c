/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:51:55 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:29:05 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "triangle.h"

void	mesh_add_box(t_mesh *mesh, const t_point3 *min_pt,
		const t_point3 *max_pt, t_material *mat)
{
	t_point3	v[8];

	if (!mesh || !min_pt || !max_pt || !mat)
		return ;
	v[0] = point3_create(min_pt->x, min_pt->y, min_pt->z);
	v[1] = point3_create(max_pt->x, min_pt->y, min_pt->z);
	v[2] = point3_create(max_pt->x, max_pt->y, min_pt->z);
	v[3] = point3_create(min_pt->x, max_pt->y, min_pt->z);
	v[4] = point3_create(min_pt->x, min_pt->y, max_pt->z);
	v[5] = point3_create(max_pt->x, min_pt->y, max_pt->z);
	v[6] = point3_create(max_pt->x, max_pt->y, max_pt->z);
	v[7] = point3_create(min_pt->x, max_pt->y, max_pt->z);
	mesh_add_quad(mesh, &v[0], &v[1], &v[2], &v[3], mat);
	mesh_add_quad(mesh, &v[5], &v[4], &v[7], &v[6], mat);
	mesh_add_quad(mesh, &v[4], &v[0], &v[3], &v[7], mat);
	mesh_add_quad(mesh, &v[1], &v[5], &v[6], &v[2], mat);
	mesh_add_quad(mesh, &v[3], &v[2], &v[6], &v[7], mat);
	mesh_add_quad(mesh, &v[4], &v[5], &v[1], &v[0], mat);
}

void	mesh_add_pyramid(t_mesh *mesh, const t_point3 *base_center,
		real_t base_size, real_t height, t_material *mat)
{
	real_t		half;
	t_point3	b[4];
	t_point3	apex;
	t_triangle	side;

	if (!mesh || !base_center || !mat)
		return ;
	half = base_size / (real_t)2.0;
	b[0] = point3_create(base_center->x - half, base_center->y,
			base_center->z - half);
	b[1] = point3_create(base_center->x + half, base_center->y,
			base_center->z - half);
	b[2] = point3_create(base_center->x + half, base_center->y,
			base_center->z + half);
	b[3] = point3_create(base_center->x - half, base_center->y,
			base_center->z + half);
	apex = point3_create(base_center->x, base_center->y + height,
			base_center->z);
	mesh_add_quad(mesh, &b[0], &b[1], &b[2], &b[3], mat);
	side = triangle_create(&b[0], &b[1], &apex, mat);
	mesh_add_triangle(mesh, &side);
	side = triangle_create(&b[1], &b[2], &apex, mat);
	mesh_add_triangle(mesh, &side);
	side = triangle_create(&b[2], &b[3], &apex, mat);
	mesh_add_triangle(mesh, &side);
	side = triangle_create(&b[3], &b[0], &apex, mat);
	mesh_add_triangle(mesh, &side);
}

void	mesh_add_icosahedron(t_mesh *mesh, const t_point3 *center,
		real_t radius, t_material *mat)
{
	const real_t	phi = (real_t)((1.0 + sqrt(5.0)) / 2.0);
	real_t			len;
	real_t			a;
	real_t			b;
	t_point3		v[12];
	int				faces[20][3];
	t_triangle		tri;
	int				i;

	if (!mesh || !center || !mat || radius <= 0)
		return ;
	len = (real_t)sqrt(1.0 + (double)(phi * phi));
	a = radius / len;
	b = radius * phi / len;
	v[0] = point3_create(center->x - a, center->y + b, center->z);
	v[1] = point3_create(center->x + a, center->y + b, center->z);
	v[2] = point3_create(center->x - a, center->y - b, center->z);
	v[3] = point3_create(center->x + a, center->y - b, center->z);
	v[4] = point3_create(center->x, center->y - a, center->z + b);
	v[5] = point3_create(center->x, center->y + a, center->z + b);
	v[6] = point3_create(center->x, center->y - a, center->z - b);
	v[7] = point3_create(center->x, center->y + a, center->z - b);
	v[8] = point3_create(center->x + b, center->y, center->z - a);
	v[9] = point3_create(center->x + b, center->y, center->z + a);
	v[10] = point3_create(center->x - b, center->y, center->z - a);
	v[11] = point3_create(center->x - b, center->y, center->z + a);
	memcpy(faces, (int[20][3]){{0,11,5},{0,5,1},{0,1,7},{0,7,10},{0,10,11},
		{1,5,9},{5,11,4},{11,10,2},{10,7,6},{7,1,8},{3,9,4},{3,4,2},{3,2,6},
		{3,6,8},{3,8,9},{4,9,5},{2,4,11},{6,2,10},{8,6,7},{9,8,1}},
		sizeof(faces));
	i = -1;
	while (++i < 20)
	{
		tri = triangle_create(&v[faces[i][0]], &v[faces[i][1]],
				&v[faces[i][2]], mat);
		mesh_add_triangle(mesh, &tri);
	}
}
