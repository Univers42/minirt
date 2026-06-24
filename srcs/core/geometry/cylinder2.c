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

static void	cyl_side_coeffs(const t_cylinder *cyl, const t_ray *r,
		real_t *abc)
{
	t_vec3	oc;
	t_vec3	d_perp;
	t_vec3	oc_perp;

	oc = vec3_sub(&r->orig, &cyl->base);
	d_perp = vec3_mul_scalar(&cyl->axis, dot(&r->dir, &cyl->axis));
	d_perp = vec3_sub(&r->dir, &d_perp);
	oc_perp = vec3_mul_scalar(&cyl->axis, dot(&oc, &cyl->axis));
	oc_perp = vec3_sub(&oc, &oc_perp);
	abc[0] = vec3_length_squared(&d_perp);
	abc[1] = dot(&oc_perp, &d_perp);
	abc[2] = vec3_length_squared(&oc_perp) - cyl->radius * cyl->radius;
}

static bool	cyl_side_roots(const t_cylinder *cyl, const t_ray *r,
		real_t *roots)
{
	real_t	abc[3];
	real_t	disc;

	cyl_side_coeffs(cyl, r, abc);
	if (abc[0] <= (real_t)1e-8)
		return (false);
	disc = abc[1] * abc[1] - abc[0] * abc[2];
	if (disc < 0)
		return (false);
	roots[0] = (-abc[1] - (real_t)sqrt((double)disc)) / abc[0];
	roots[1] = (-abc[1] + (real_t)sqrt((double)disc)) / abc[0];
	return (true);
}

static bool	cyl_hit_sides(const t_cylinder *cyl, const t_ray *r,
		real_t *closest_t, t_hit_record *rec)
{
	real_t	roots[2];
	bool	hit;
	int		i;

	if (!cyl_side_roots(cyl, r, roots))
		return (false);
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
	t_cyl_caps	ctx;

	if (!cyl || !r || !rec)
		return (false);
	closest_t = rayt.max;
	hit_anything = cyl_hit_sides(cyl, r, &closest_t, rec);
	ctx.rayt = rayt;
	ctx.closest_t = &closest_t;
	ctx.rec = rec;
	if (cyl_hit_caps(cyl, r, &ctx))
		hit_anything = true;
	if (!hit_anything)
		return (false);
	rec->mat = cyl->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	return (true);
}
