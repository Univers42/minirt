/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder4.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include "hittable_list.h"
#include <stdlib.h>

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

static bool	cyl_try_cap(const t_cylinder *cyl, const t_ray *r,
		t_cyl_cap *cap, t_cyl_caps *ctx)
{
	t_hit_record	cap_rec;
	t_vec3			normal;

	if (!(cap->t >= ctx->rayt.min && cap->t < *ctx->closest_t))
		return (false);
	cap_rec.v = cap->capv;
	if (!cyl_check_cap(cyl, r, cap->t, &cap_rec))
		return (false);
	*ctx->closest_t = cap->t;
	normal = cyl->axis;
	if (cap->capv < 0.5)
		normal = vec3_neg(&cyl->axis);
	set_face_normal(&cap_rec, r, &normal);
	*ctx->rec = cap_rec;
	return (true);
}

bool	cyl_hit_caps(const t_cylinder *cyl, const t_ray *r, t_cyl_caps *ctx)
{
	real_t		dots[2];
	t_vec3		oc;
	t_cyl_cap	cap;
	bool		hit;

	oc = vec3_sub(&r->orig, &cyl->base);
	dots[0] = dot(&r->dir, &cyl->axis);
	dots[1] = dot(&oc, &cyl->axis);
	if (fabsl((long double)dots[0]) <= (long double)1e-8)
		return (false);
	hit = false;
	cap.t = -dots[1] / dots[0];
	cap.capv = 0.0;
	if (cyl_try_cap(cyl, r, &cap, ctx))
		hit = true;
	cap.t = (cyl->height - dots[1]) / dots[0];
	cap.capv = 1.0;
	if (cyl_try_cap(cyl, r, &cap, ctx))
		hit = true;
	return (hit);
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
