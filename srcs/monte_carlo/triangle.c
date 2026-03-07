/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle.c                                         :+:      :+:    :+:   */
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

static void	triangle_pad_bbox(real_t *mn, real_t *mx)
{
	const real_t	delta = (real_t)0.0001;

	if (*mx - *mn < delta)
	{
		*mn -= delta;
		*mx += delta;
	}
}

t_triangle	triangle_create(const t_point3 *v0, const t_point3 *v1,
		const t_point3 *v2, t_material *mat)
{
	t_triangle	tri;
	t_vec3		n;
	real_t		mn[3];
	real_t		mx[3];

	tri.v0 = *v0;
	tri.v1 = *v1;
	tri.v2 = *v2;
	tri.mat = mat;
	tri.e1 = vec3_sub(v1, v0);
	tri.e2 = vec3_sub(v2, v0);
	n = cross(&tri.e1, &tri.e2);
	tri.normal = unit_vector(&n);
	mn[0] = fmin(fmin(v0->x, v1->x), v2->x);
	mn[1] = fmin(fmin(v0->y, v1->y), v2->y);
	mn[2] = fmin(fmin(v0->z, v1->z), v2->z);
	mx[0] = fmax(fmax(v0->x, v1->x), v2->x);
	mx[1] = fmax(fmax(v0->y, v1->y), v2->y);
	mx[2] = fmax(fmax(v0->z, v1->z), v2->z);
	triangle_pad_bbox(&mn[0], &mx[0]);
	triangle_pad_bbox(&mn[1], &mx[1]);
	triangle_pad_bbox(&mn[2], &mx[2]);
	tri.bbox = aabb_from_points(
			&(t_point3){mn[0], mn[1], mn[2]},
			&(t_point3){mx[0], mx[1], mx[2]});
	return (tri);
}

bool	triangle_hit(const t_triangle *tri, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_vec3	h;
	real_t	a;
	real_t	f;
	t_vec3	s;
	real_t	u;
	t_vec3	q;
	real_t	v;
	real_t	t;

	if (!tri || !r || !rec)
		return (false);
	h = cross(&r->dir, &tri->e2);
	a = dot(&tri->e1, &h);
	if (fabsl((long double)a) < (long double)1e-8)
		return (false);
	f = (real_t)1.0 / a;
	s = vec3_sub(&r->orig, &tri->v0);
	u = f * dot(&s, &h);
	if (u < (real_t)0.0 || u > (real_t)1.0)
		return (false);
	q = cross(&s, &tri->e1);
	v = f * dot(&r->dir, &q);
	if (v < (real_t)0.0 || u + v > (real_t)1.0)
		return (false);
	t = f * dot(&tri->e2, &q);
	if (!contains(rayt.min, rayt.max, t))
		return (false);
	rec->t = t;
	rec->p = ray_at((t_ray *)r, t);
	rec->u = u;
	rec->v = v;
	rec->mat = tri->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	set_face_normal(rec, r, &tri->normal);
	return (true);
}

static __thread const t_triangle	*g_current_triangle = NULL;

void	set_current_triangle(const void *obj)
{
	g_current_triangle = (const t_triangle *)obj;
}

bool	triangle_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_triangle)
		return (false);
	return (triangle_hit(g_current_triangle, r, rayt, rec));
}

bool	hittable_list_add_triangle(t_hittable_list *list,
		const t_triangle *tri)
{
	t_triangle			*copy;
	t_hittable_wrapper	wrap;

	if (!list || !tri)
		return (false);
	copy = (t_triangle *)malloc(sizeof(t_triangle));
	if (!copy)
		return (false);
	*copy = *tri;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_triangle;
	wrap.hit_noobj = triangle_hit_noobj;
	wrap.bbox = tri->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
