/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 13:16:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sphere.h"
#include <stddef.h>

static __thread const t_sphere	*g_current_sphere = NULL;

void	set_current_sphere(const void *obj)
{
	g_current_sphere = (const t_sphere *)obj;
}

static void	sphere_fill_record(const t_sphere *s, const t_ray *r,
		const t_vec3 *center, t_hit_record *rec)
{
	t_vec3	oc;

	rec->p = ray_at((t_ray *)r, rec->t);
	oc = vec3_sub(&rec->p, center);
	oc = unit_vector(&oc);
	set_face_normal(rec, r, &oc);
	sphere_get_uv(&oc, &rec->u, &rec->v);
	rec->albedo = s->albedo;
	rec->mat = s->mat;
}

static bool	sphere_solve(const t_sphere *s, const t_ray *r,
		t_interval rayt, real_t *root)
{
	t_vec3	oc;
	t_vec3	center;
	real_t	vars[4];

	center = sphere_center_at(s, r->tm);
	oc = vec3_sub(&r->orig, &center);
	vars[0] = vec3_length_squared(&r->dir);
	vars[1] = dot(&r->dir, &oc);
	vars[2] = vec3_length_squared(&oc) - s->radius * s->radius;
	vars[3] = vars[1] * vars[1] - vars[0] * vars[2];
	if (vars[3] < 0.0)
		return (false);
	*root = (-vars[1] - sqrt(vars[3])) / vars[0];
	if (!contains(rayt.min, rayt.max, *root))
	{
		*root = (-vars[1] + sqrt(vars[3])) / vars[0];
		if (!contains(rayt.min, rayt.max, *root))
			return (false);
	}
	return (true);
}

bool	sphere_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_sphere	*s;
	t_vec3			center;
	real_t			root;

	s = g_current_sphere;
	if (!s)
		return (false);
	if (!sphere_solve(s, r, rayt, &root))
		return (false);
	center = sphere_center_at(s, r->tm);
	rec->t = root;
	sphere_fill_record(s, r, &center, rec);
	return (true);
}
