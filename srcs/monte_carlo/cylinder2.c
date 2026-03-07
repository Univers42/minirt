/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:05:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include "hittable_list.h"
#include <stdlib.h>

static bool	cyl_check_side(const t_cylinder *cyl, const t_ray *r,
		real_t t, t_hit_record *rec)
{
	t_vec3	hit_pt;
	t_vec3	to_hit;
	real_t	h;
	t_vec3	ac;
	t_vec3	outward;

	hit_pt = ray_at((t_ray *)r, t);
	to_hit = vec3_sub(&hit_pt, &cyl->base);
	h = dot(&to_hit, &cyl->axis);
	if (h < 0 || h > cyl->height)
		return (false);
	ac = vec3_mul_scalar(&cyl->axis, h);
	ac = vec3_add(&cyl->base, &ac);
	outward = vec3_sub(&hit_pt, &ac);
	outward = unit_vector(&outward);
	rec->t = t;
	rec->p = hit_pt;
	set_face_normal(rec, r, &outward);
	cylinder_get_uv(cyl, &hit_pt, &rec->u, &rec->v);
	return (true);
}

static bool	cyl_check_cap(const t_cylinder *cyl, const t_ray *r,
		real_t t, t_hit_record *cap_rec)
{
	t_vec3	hit_pt;
	t_vec3	center;
	t_vec3	to_hit;
	t_vec3	ap;
	t_vec3	perp;

	hit_pt = ray_at((t_ray *)r, t);
	if (cap_rec->v > 0.5)
	{
		ap = vec3_mul_scalar(&cyl->axis, cyl->height);
		center = vec3_add(&cyl->base, &ap);
		to_hit = vec3_sub(&hit_pt, &center);
	}
	else
		to_hit = vec3_sub(&hit_pt, &cyl->base);
	ap = vec3_mul_scalar(&cyl->axis, dot(&to_hit, &cyl->axis));
	perp = vec3_sub(&to_hit, &ap);
	if (vec3_length_squared(&perp) > cyl->radius * cyl->radius)
		return (false);
	cap_rec->t = t;
	cap_rec->p = hit_pt;
	cap_rec->u = (perp.x / cyl->radius + (real_t)1.0) * (real_t)0.5;
	cap_rec->v = (perp.z / cyl->radius + (real_t)1.0) * (real_t)0.5;
	return (true);
}

static bool	cyl_hit_sides(const t_cylinder *cyl, const t_ray *r,
		real_t *closest_t, t_hit_record *rec)
{
	t_vec3	oc;
	real_t	dots[2];
	t_vec3	d_perp;
	t_vec3	oc_perp;
	real_t	abc[3];
	real_t	disc;
	real_t	roots[2];
	bool	hit;
	int		i;

	oc = vec3_sub(&r->orig, &cyl->base);
	dots[0] = dot(&r->dir, &cyl->axis);
	dots[1] = dot(&oc, &cyl->axis);
	d_perp = vec3_mul_scalar(&cyl->axis, dots[0]);
	d_perp = vec3_sub(&r->dir, &d_perp);
	oc_perp = vec3_mul_scalar(&cyl->axis, dots[1]);
	oc_perp = vec3_sub(&oc, &oc_perp);
	abc[0] = vec3_length_squared(&d_perp);
	abc[1] = dot(&oc_perp, &d_perp);
	abc[2] = vec3_length_squared(&oc_perp) - cyl->radius * cyl->radius;
	if (abc[0] <= (real_t)1e-8)
		return (false);
	disc = abc[1] * abc[1] - abc[0] * abc[2];
	if (disc < 0)
		return (false);
	roots[0] = (-abc[1] - (real_t)sqrt((double)disc)) / abc[0];
	roots[1] = (-abc[1] + (real_t)sqrt((double)disc)) / abc[0];
	hit = false;
	i = -1;
	while (++i < 2)
	{
		if (roots[i] >= (real_t)1e-8 && roots[i] < *closest_t
			&& cyl_check_side(cyl, r, roots[i], rec))
		{
			*closest_t = roots[i];
			hit = true;
		}
	}
	return (hit);
}

bool	cylinder_hit(const t_cylinder *cyl, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	bool		hit_anything;
	real_t		closest_t;
	t_vec3		oc;
	real_t		dots[2];
	real_t		t;
	t_hit_record	cap_rec;

	if (!cyl || !r || !rec)
		return (false);
	closest_t = rayt.max;
	hit_anything = cyl_hit_sides(cyl, r, &closest_t, rec);
	oc = vec3_sub(&r->orig, &cyl->base);
	dots[0] = dot(&r->dir, &cyl->axis);
	dots[1] = dot(&oc, &cyl->axis);
	if (fabsl((long double)dots[0]) > (long double)1e-8)
	{
		t = -dots[1] / dots[0];
		cap_rec.v = 0.0;
		if (t >= rayt.min && t < closest_t && cyl_check_cap(cyl, r, t, &cap_rec))
		{
			closest_t = t;
			{
				t_vec3 neg_ax;
				neg_ax = vec3_neg(&cyl->axis);
				set_face_normal(&cap_rec, r, &neg_ax);
			}
			*rec = cap_rec;
			hit_anything = true;
		}
		t = (cyl->height - dots[1]) / dots[0];
		cap_rec.v = 1.0;
		if (t >= rayt.min && t < closest_t && cyl_check_cap(cyl, r, t, &cap_rec))
		{
			closest_t = t;
			set_face_normal(&cap_rec, r, &cyl->axis);
			*rec = cap_rec;
			hit_anything = true;
		}
	}
	if (!hit_anything)
		return (false);
	rec->mat = cyl->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	return (true);
}

bool	hittable_list_add_cylinder(t_hittable_list *list,
		const t_cylinder *cyl)
{
	t_cylinder			*copy;
	t_hittable_wrapper	wrap;

	if (!list || !cyl)
		return (false);
	copy = (t_cylinder *)malloc(sizeof(t_cylinder));
	if (!copy)
		return (false);
	*copy = *cyl;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_cylinder;
	wrap.hit_noobj = cylinder_hit_noobj;
	wrap.bbox = cyl->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
