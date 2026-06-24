/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:51:55 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:44:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "triangle.h"
#include "hittable_list.h"
#include <stdlib.h>

static void	triangle_pad_bbox(real_t *mn, real_t *mx)
{
	const real_t	delta = (real_t)0.0001;

	if (*mx - *mn < delta)
	{
		*mn -= delta;
		*mx += delta;
	}
}

static void	triangle_init_bbox(t_triangle *tri)
{
	real_t	mn[3];
	real_t	mx[3];

	mn[0] = fmin(fmin(tri->v0.x, tri->v1.x), tri->v2.x);
	mn[1] = fmin(fmin(tri->v0.y, tri->v1.y), tri->v2.y);
	mn[2] = fmin(fmin(tri->v0.z, tri->v1.z), tri->v2.z);
	mx[0] = fmax(fmax(tri->v0.x, tri->v1.x), tri->v2.x);
	mx[1] = fmax(fmax(tri->v0.y, tri->v1.y), tri->v2.y);
	mx[2] = fmax(fmax(tri->v0.z, tri->v1.z), tri->v2.z);
	triangle_pad_bbox(&mn[0], &mx[0]);
	triangle_pad_bbox(&mn[1], &mx[1]);
	triangle_pad_bbox(&mn[2], &mx[2]);
	tri->bbox = aabb_from_points(
			&(t_point3){mn[0], mn[1], mn[2]},
			&(t_point3){mx[0], mx[1], mx[2]});
}

t_triangle	triangle_create(const t_point3 *v0, const t_point3 *v1,
		const t_point3 *v2, t_material *mat)
{
	t_triangle	tri;
	t_vec3		n;

	tri.v0 = *v0;
	tri.v1 = *v1;
	tri.v2 = *v2;
	tri.mat = mat;
	tri.has_smooth = false;
	tri.e1 = vec3_sub(v1, v0);
	tri.e2 = vec3_sub(v2, v0);
	n = cross(&tri.e1, &tri.e2);
	tri.normal = unit_vector(&n);
	triangle_init_bbox(&tri);
	return (tri);
}

/* ------------------------------------------------------------------ */
/*  Smooth-shading variant — stores per-vertex normals for            */
/*  barycentric interpolation in triangle_hit().                      */
/* ------------------------------------------------------------------ */

t_triangle	triangle_create_smooth(const t_tri_smooth *s, t_material *mat)
{
	t_triangle	tri;

	tri = triangle_create(&s->v0, &s->v1, &s->v2, mat);
	tri.n0 = s->n0;
	tri.n1 = s->n1;
	tri.n2 = s->n2;
	tri.has_smooth = true;
	return (tri);
}
