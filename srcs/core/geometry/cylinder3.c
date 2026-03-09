/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:05:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include "hittable_list.h"
#include <stdlib.h>
#include <stddef.h>

static void	cone_compute_bbox(t_cone *cone, const t_point3 *apex)
{
	real_t		br;
	t_vec3		to_base;
	t_point3	bc;
	real_t		e[3];
	t_point3	low;
	t_point3	high;

	br = cone->height * (real_t)tan((double)cone->angle);
	to_base = vec3_mul_scalar(&cone->axis, cone->height);
	bc = vec3_add(apex, &to_base);
	e[0] = br * (real_t)sqrt(1.0 - (double)(cone->axis.x * cone->axis.x));
	e[1] = br * (real_t)sqrt(1.0 - (double)(cone->axis.y * cone->axis.y));
	e[2] = br * (real_t)sqrt(1.0 - (double)(cone->axis.z * cone->axis.z));
	low = point3_create(fmin(apex->x, bc.x - e[0]),
			fmin(apex->y, bc.y - e[1]), fmin(apex->z, bc.z - e[2]));
	high = point3_create(fmax(apex->x, bc.x + e[0]),
			fmax(apex->y, bc.y + e[1]), fmax(apex->z, bc.z + e[2]));
	cone->bbox = aabb_from_points(&low, &high);
}

t_cone	cone_create(const t_point3 *apex, const t_vec3 *axis,
		real_t angle_deg, real_t height, t_material *mat)
{
	t_cone	cone;

	cone.apex = *apex;
	cone.axis = unit_vector(axis);
	cone.angle = degrees_to_radians(angle_deg);
	cone.height = height;
	if (height <= 0)
		cone.height = (real_t)1.0;
	cone.mat = mat;
	cone_compute_bbox(&cone, apex);
	return (cone);
}

bool	cone_hit(const t_cone *cone, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_vec3	co;
	real_t	trig[2];
	real_t	dv[2];
	real_t	tan2;
	real_t	abc[3];
	bool	hit_anything;
	real_t	closest_t;
	t_vec3	closest_normal;
	real_t	disc;

	if (!cone || !r || !rec)
		return (false);
	co = vec3_sub(&r->orig, &cone->apex);
	trig[0] = (real_t)cos((double)cone->angle);
	trig[1] = (real_t)sin((double)cone->angle);
	dv[0] = dot(&r->dir, &cone->axis);
	dv[1] = dot(&co, &cone->axis);
	tan2 = (trig[1] * trig[1]) / (trig[0] * trig[0]);
	abc[0] = dot(&r->dir, &r->dir) - (1 + tan2) * dv[0] * dv[0];
	abc[1] = dot(&r->dir, &co) - (1 + tan2) * dv[0] * dv[1];
	abc[2] = dot(&co, &co) - (1 + tan2) * dv[1] * dv[1];
	hit_anything = false;
	closest_t = rayt.max;
	if (fabsl((long double)abc[0]) > (long double)1e-8)
	{
		disc = abc[1] * abc[1] - abc[0] * abc[2];
		if (disc >= 0)
		{
			real_t roots[2] = {(-abc[1] - (real_t)sqrt((double)disc)) / abc[0],
				(-abc[1] + (real_t)sqrt((double)disc)) / abc[0]};
			for (int i = 0; i < 2; ++i)
			{
				if (roots[i] < rayt.min || roots[i] >= closest_t)
					continue;
				t_vec3 hp = ray_at((t_ray *)r, roots[i]);
				t_vec3 th = vec3_sub(&hp, &cone->apex);
				real_t h = dot(&th, &cone->axis);
				if (h >= 0 && h <= cone->height)
				{
					hit_anything = true;
					closest_t = roots[i];
					t_vec3 ap = vec3_mul_scalar(&cone->axis, h);
					t_vec3 rad = vec3_sub(&th, &ap);
					t_vec3 ru = unit_vector(&rad);
					t_vec3 n1 = vec3_mul_scalar(&ru, trig[0]);
					t_vec3 n2 = vec3_mul_scalar(&cone->axis, -trig[1]);
					closest_normal = vec3_add(&n1, &n2);
					closest_normal = unit_vector(&closest_normal);
				}
			}
		}
	}
	if (fabsl((long double)dv[0]) > (long double)1e-8)
	{
		real_t t = (cone->height - dv[1]) / dv[0];
		if (t >= rayt.min && t < closest_t)
		{
			t_vec3 hp = ray_at((t_ray *)r, t);
			t_vec3 bac = vec3_mul_scalar(&cone->axis, cone->height);
			t_vec3 bc = vec3_add(&cone->apex, &bac);
			t_vec3 th = vec3_sub(&hp, &bc);
			real_t br = cone->height * (real_t)tan((double)cone->angle);
			if (vec3_length_squared(&th) <= br * br)
			{
				hit_anything = true;
				closest_t = t;
				closest_normal = cone->axis;
			}
		}
	}
	if (!hit_anything)
		return (false);
	rec->t = closest_t;
	rec->p = ray_at((t_ray *)r, closest_t);
	rec->u = 0;
	rec->v = 0;
	rec->mat = cone->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	set_face_normal(rec, r, &closest_normal);
	return (true);
}

static __thread const t_cone	*g_current_cone = NULL;

void	set_current_cone(const void *obj)
{
	g_current_cone = (const t_cone *)obj;
}

bool	cone_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_cone)
		return (false);
	return (cone_hit(g_current_cone, r, rayt, rec));
}

bool	hittable_list_add_cone(t_hittable_list *list, const t_cone *cone)
{
	t_cone				*copy;
	t_hittable_wrapper	wrap;

	if (!list || !cone)
		return (false);
	copy = (t_cone *)malloc(sizeof(t_cone));
	if (!copy)
		return (false);
	*copy = *cone;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_cone;
	wrap.hit_noobj = cone_hit_noobj;
	wrap.bbox = cone->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
