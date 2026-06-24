/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 04:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 04:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "plane.h"
#include "interval.h"
#include <math.h>

/* Build an orthonormal in-plane (u, v) basis for texture coordinates. */
static void	plane_basis(t_plane *p)
{
	t_vec3	ref;

	if (fabs(p->normal.x) < (real_t)0.9)
		ref = vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);
	else
		ref = vec3_create((real_t)0.0, (real_t)1.0, (real_t)0.0);
	p->u_axis = cross(&p->normal, &ref);
	p->u_axis = unit_vector(&p->u_axis);
	p->v_axis = cross(&p->normal, &p->u_axis);
}

t_plane	plane_create(const t_point3 *point, const t_vec3 *normal,
			t_material *mat)
{
	t_plane	p;

	p.q = *point;
	p.normal = unit_vector(normal);
	plane_basis(&p);
	p.mat = mat;
	p.bbox.x = interval(-INFINITY, INFINITY);
	p.bbox.y = interval(-INFINITY, INFINITY);
	p.bbox.z = interval(-INFINITY, INFINITY);
	return (p);
}

bool	plane_hit(const t_plane *plane, const t_ray *r, t_interval rayt,
			t_hit_record *rec)
{
	real_t	denom;
	real_t	t;
	t_vec3	rel;
	t_vec3	p;

	if (!plane || !r || !rec)
		return (false);
	denom = dot(&plane->normal, &r->dir);
	if (fabs(denom) < (real_t)1e-8)
		return (false);
	rel = vec3_sub(&plane->q, &r->orig);
	t = dot(&plane->normal, &rel) / denom;
	if (!contains(rayt.min, rayt.max, t))
		return (false);
	p = ray_at((t_ray *)r, t);
	rec->t = t;
	rec->p = p;
	rec->mat = plane->mat;
	set_face_normal(rec, r, &plane->normal);
	rel = vec3_sub(&p, &plane->q);
	rec->u = dot(&rel, &plane->u_axis);
	rec->v = dot(&rel, &plane->v_axis);
	return (true);
}
