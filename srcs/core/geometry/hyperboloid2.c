/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hyperboloid2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hyperboloid.h"
#include <stdlib.h>

/* True when the hit point at parameter t sits inside the axial band
   -half_h <= dot(p - center, axis) <= half_h. */
bool	hyperboloid_in_band(const t_hyperboloid *hy, const t_ray *r, real_t t)
{
	t_vec3	p;
	t_vec3	w;
	real_t	y;

	p = ray_at((t_ray *)r, t);
	w = vec3_sub(&p, &hy->center);
	y = dot(&w, &hy->axis);
	return (y >= -hy->half_h && y <= hy->half_h);
}

/* Normal = unit gradient of F: grad = 2*inv_a2*w - 2*(inv_a2+inv_c2)*y*axis,
   w = p - center, y = dot(w, axis). */
void	hyperboloid_set_record(const t_hyperboloid *hy, const t_ray *r,
		real_t t, t_hit_record *rec)
{
	t_vec3	w;
	t_vec3	scaled;
	t_vec3	grad;
	real_t	y;

	rec->t = t;
	rec->p = ray_at((t_ray *)r, t);
	w = vec3_sub(&rec->p, &hy->center);
	y = dot(&w, &hy->axis);
	scaled = vec3_mul_scalar(&hy->axis,
			(real_t)2.0 * (hy->inv_a2 + hy->inv_c2) * y);
	grad = vec3_mul_scalar(&w, (real_t)2.0 * hy->inv_a2);
	grad = vec3_sub(&grad, &scaled);
	grad = unit_vector(&grad);
	rec->u = 0;
	rec->v = 0;
	rec->mat = hy->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	set_face_normal(rec, r, &grad);
}

static __thread const t_hyperboloid	*g_current_hyperboloid;

void	set_current_hyperboloid(const void *obj)
{
	g_current_hyperboloid = (const t_hyperboloid *)obj;
}

bool	hyperboloid_hit_noobj(const t_ray *r, t_interval rayt,
		t_hit_record *rec)
{
	if (!g_current_hyperboloid)
		return (false);
	return (hyperboloid_hit(g_current_hyperboloid, r, rayt, rec));
}

bool	hittable_list_add_hyperboloid(t_hittable_list *list,
		const t_hyperboloid *hy)
{
	t_hyperboloid		*copy;
	t_hittable_wrapper	wrap;

	if (!list || !hy)
		return (false);
	copy = (t_hyperboloid *)malloc(sizeof(t_hyperboloid));
	if (!copy)
		return (false);
	*copy = *hy;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_hyperboloid;
	wrap.hit_noobj = hyperboloid_hit_noobj;
	wrap.bbox = hy->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
