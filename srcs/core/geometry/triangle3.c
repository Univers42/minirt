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
	mesh_add_quad(mesh, &(t_quad_pts){v[0], v[1], v[2], v[3]}, mat);
	mesh_add_quad(mesh, &(t_quad_pts){v[5], v[4], v[7], v[6]}, mat);
	mesh_add_quad(mesh, &(t_quad_pts){v[4], v[0], v[3], v[7]}, mat);
	mesh_add_quad(mesh, &(t_quad_pts){v[1], v[5], v[6], v[2]}, mat);
	mesh_add_quad(mesh, &(t_quad_pts){v[3], v[2], v[6], v[7]}, mat);
	mesh_add_quad(mesh, &(t_quad_pts){v[4], v[5], v[1], v[0]}, mat);
}

static void	pyramid_base(const t_point3 *c, real_t half, t_point3 b[4])
{
	b[0] = point3_create(c->x - half, c->y, c->z - half);
	b[1] = point3_create(c->x + half, c->y, c->z - half);
	b[2] = point3_create(c->x + half, c->y, c->z + half);
	b[3] = point3_create(c->x - half, c->y, c->z + half);
}

static void	pyramid_sides(t_mesh *mesh, const t_point3 b[4],
		const t_point3 *apex, t_material *mat)
{
	t_triangle	side;
	int			i;

	i = -1;
	while (++i < 4)
	{
		side = triangle_create(&b[i], &b[(i + 1) % 4], apex, mat);
		mesh_add_triangle(mesh, &side);
	}
}

void	mesh_add_pyramid(t_mesh *mesh, const t_point3 *base_center,
		const t_shape_dims *dims, t_material *mat)
{
	t_point3	b[4];
	t_point3	apex;

	if (!mesh || !base_center || !mat)
		return ;
	pyramid_base(base_center, dims->size / (real_t)2.0, b);
	apex = point3_create(base_center->x, base_center->y + dims->height,
			base_center->z);
	mesh_add_quad(mesh, &(t_quad_pts){b[0], b[1], b[2], b[3]}, mat);
	pyramid_sides(mesh, b, &apex, mat);
}
