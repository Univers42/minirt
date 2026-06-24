/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle5.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "triangle.h"

static void	interp_normal(const t_triangle *tri, real_t u, real_t v,
		t_vec3 *out)
{
	t_vec3	a;
	t_vec3	b;
	t_vec3	c;

	a = vec3_mul_scalar(&tri->n0, (real_t)1.0 - u - v);
	b = vec3_mul_scalar(&tri->n1, u);
	c = vec3_mul_scalar(&tri->n2, v);
	*out = vec3_add(&a, &b);
	*out = vec3_add(out, &c);
	*out = unit_vector(out);
}

static bool	triangle_solve(const t_triangle *tri, const t_ray *r,
		t_tri_uvt *uvt)
{
	t_vec3	h;
	t_vec3	s;
	t_vec3	q;
	real_t	f;
	real_t	a;

	h = cross(&r->dir, &tri->e2);
	a = dot(&tri->e1, &h);
	if (fabsl((long double)a) < (long double)1e-8)
		return (false);
	f = (real_t)1.0 / a;
	s = vec3_sub(&r->orig, &tri->v0);
	uvt->u = f * dot(&s, &h);
	if (uvt->u < (real_t)0.0 || uvt->u > (real_t)1.0)
		return (false);
	q = cross(&s, &tri->e1);
	uvt->v = f * dot(&r->dir, &q);
	if (uvt->v < (real_t)0.0 || uvt->u + uvt->v > (real_t)1.0)
		return (false);
	uvt->t = f * dot(&tri->e2, &q);
	return (true);
}

static void	triangle_fill(const t_triangle *tri, const t_ray *r,
		const t_tri_uvt *uvt, t_hit_record *rec)
{
	t_vec3	n;

	rec->t = uvt->t;
	rec->p = ray_at((t_ray *)r, uvt->t);
	rec->u = uvt->u;
	rec->v = uvt->v;
	rec->mat = tri->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	if (tri->has_smooth)
	{
		interp_normal(tri, uvt->u, uvt->v, &n);
		set_face_normal(rec, r, &n);
	}
	else
		set_face_normal(rec, r, &tri->normal);
}

bool	triangle_hit(const t_triangle *tri, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_tri_uvt	uvt;

	if (!tri || !r || !rec)
		return (false);
	if (!triangle_solve(tri, r, &uvt))
		return (false);
	if (!contains(rayt.min, rayt.max, uvt.t))
		return (false);
	triangle_fill(tri, r, &uvt, rec);
	return (true);
}
