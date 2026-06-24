/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:40 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quad.h"

static void	set_bounding_box_plane(t_quad *quad)
{
	t_vec3	n;
	t_vec3	n_unit;

	n = cross(&quad->u, &quad->v);
	quad->w = n;
	n_unit = unit_vector(&n);
	quad->normal = n_unit;
	quad->d = (real_t)dot(&quad->normal, &quad->q);
}

void	set_bounding_box(t_quad *quad)
{
	t_point3	p[4];
	t_point3	low;
	t_point3	high;

	if (!quad)
		return ;
	set_bounding_box_plane(quad);
	p[0] = quad->q;
	p[1] = point3_create(quad->q.x + quad->u.x,
			quad->q.y + quad->u.y, quad->q.z + quad->u.z);
	p[2] = point3_create(quad->q.x + quad->v.x,
			quad->q.y + quad->v.y, quad->q.z + quad->v.z);
	p[3] = point3_create(quad->q.x + quad->u.x + quad->v.x,
			quad->q.y + quad->u.y + quad->v.y,
			quad->q.z + quad->u.z + quad->v.z);
	low = point3_create(fmin(fmin(p[0].x, p[1].x), fmin(p[2].x, p[3].x)),
			fmin(fmin(p[0].y, p[1].y), fmin(p[2].y, p[3].y)),
			fmin(fmin(p[0].z, p[1].z), fmin(p[2].z, p[3].z)));
	high = point3_create(fmax(fmax(p[0].x, p[1].x), fmax(p[2].x, p[3].x)),
			fmax(fmax(p[0].y, p[1].y), fmax(p[2].y, p[3].y)),
			fmax(fmax(p[0].z, p[1].z), fmax(p[2].z, p[3].z)));
	quad->bbox = aabb_from_points(&low, &high);
}

t_quad	quad_create(const t_point3 *q_pos, const t_vec3 *u,
		const t_vec3 *v, t_material *mat)
{
	t_quad	quad;

	if (!q_pos || !u || !v)
	{
		quad.q = point3_create(0.0, 0.0, 0.0);
		quad.u = vec3_zero();
		quad.v = vec3_zero();
		quad.w = vec3_zero();
		quad.mat = NULL;
		quad.bbox = aabb_empty();
		quad.normal = vec3_zero();
		quad.d = (real_t)0.0;
		return (quad);
	}
	quad.q = *q_pos;
	quad.u = *u;
	quad.v = *v;
	quad.mat = mat;
	set_bounding_box(&quad);
	return (quad);
}

t_aabb	quad_bounding_box(const t_quad *quad)
{
	if (!quad)
		return (aabb_empty());
	return (quad->bbox);
}
