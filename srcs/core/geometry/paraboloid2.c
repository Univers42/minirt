/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraboloid2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "paraboloid.h"
#include <stdlib.h>

/* True when the hit point at parameter t sits inside the axial band
   0 <= dot(p - vertex, axis) <= height. */
bool	paraboloid_in_band(const t_paraboloid *pb, const t_ray *r, real_t t)
{
	t_vec3	p;
	t_vec3	w;
	real_t	y;

	p = ray_at((t_ray *)r, t);
	w = vec3_sub(&p, &pb->vertex);
	y = dot(&w, &pb->axis);
	return (y >= (real_t)0.0 && y <= pb->height);
}

/* Normal = unit gradient of F: grad = 2*w - (2*y + k)*axis, w = p - vertex. */
void	paraboloid_set_record(const t_paraboloid *pb, const t_ray *r,
		real_t t, t_hit_record *rec)
{
	t_vec3	w;
	t_vec3	scaled;
	t_vec3	grad;
	real_t	y;

	rec->t = t;
	rec->p = ray_at((t_ray *)r, t);
	w = vec3_sub(&rec->p, &pb->vertex);
	y = dot(&w, &pb->axis);
	scaled = vec3_mul_scalar(&pb->axis, (real_t)2.0 * y + pb->k);
	grad = vec3_mul_scalar(&w, (real_t)2.0);
	grad = vec3_sub(&grad, &scaled);
	grad = unit_vector(&grad);
	rec->u = 0;
	rec->v = 0;
	rec->mat = pb->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	set_face_normal(rec, r, &grad);
}

static __thread const t_paraboloid	*g_current_paraboloid;

void	set_current_paraboloid(const void *obj)
{
	g_current_paraboloid = (const t_paraboloid *)obj;
}

bool	paraboloid_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_paraboloid)
		return (false);
	return (paraboloid_hit(g_current_paraboloid, r, rayt, rec));
}

bool	hittable_list_add_paraboloid(t_hittable_list *list,
		const t_paraboloid *pb)
{
	t_paraboloid		*copy;
	t_hittable_wrapper	wrap;

	if (!list || !pb)
		return (false);
	copy = (t_paraboloid *)malloc(sizeof(t_paraboloid));
	if (!copy)
		return (false);
	*copy = *pb;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_paraboloid;
	wrap.hit_noobj = paraboloid_hit_noobj;
	wrap.bbox = pb->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
