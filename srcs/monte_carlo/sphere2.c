/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:40 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sphere.h"
#include <stddef.h>

t_sphere	create_sphere_default(const t_point3 *center, real_t radius)
{
	t_sphere	s;
	t_point3	center_low;
	t_point3	center_high;

	s.center.center1 = vec3_create(center->x, center->y, center->z);
	s.center.center_velocity = vec3_zero();
	if (radius > 0.0)
		s.radius = radius;
	else
		s.radius = 0.0;
	s.albedo = vec3_create(1.0, 1.0, 1.0);
	s.mat = NULL;
	center_low = point3_create(center->x - s.radius,
			center->y - s.radius, center->z - s.radius);
	center_high = point3_create(center->x + s.radius,
			center->y + s.radius, center->z + s.radius);
	s.bbox = aabb_from_points(&center_low, &center_high);
	return (s);
}

real_t	hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r)
{
	t_vec3	oc;
	real_t	a;
	real_t	h;
	real_t	c;
	real_t	discriminant;

	oc = vec3_sub(center, &r->orig);
	a = vec3_length_squared(&r->dir);
	h = (real_t)dot(&r->dir, &oc);
	c = vec3_length_squared(&oc) - radius * radius;
	discriminant = h * h - a * c;
	if (discriminant < 0.0)
		return (-1.0);
	return ((h - sqrt(discriminant)) / a);
}

static __thread const t_sphere	*g_current_sphere = NULL;

void	set_current_sphere(const void *obj)
{
	g_current_sphere = (const t_sphere *)obj;
}

static bool	sphere_try_root(real_t root, t_interval rayt,
		const t_sphere *s, t_hit_record *rec)
{
	t_vec3	tmp;
	t_vec3	outward_normal;

	if (!contains(rayt.min, rayt.max, root))
		return (false);
	rec->t = root;
	return (true);
	(void)s;
}

bool	sphere_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_sphere	*s;
	t_vec3			current_center;
	t_vec3			oc;
	real_t			vars[4];
	real_t			root;

	s = g_current_sphere;
	if (!s)
		return (false);
	current_center = sphere_center_at(s, r->tm);
	oc = vec3_sub(&r->orig, &current_center);
	vars[0] = vec3_length_squared(&r->dir);
	vars[1] = dot(&r->dir, &oc);
	vars[2] = vec3_length_squared(&oc) - s->radius * s->radius;
	vars[3] = vars[1] * vars[1] - vars[0] * vars[2];
	if (vars[3] < 0.0)
		return (false);
	root = (-vars[1] - sqrt(vars[3])) / vars[0];
	if (!contains(rayt.min, rayt.max, root))
	{
		root = (-vars[1] + sqrt(vars[3])) / vars[0];
		if (!contains(rayt.min, rayt.max, root))
			return (false);
	}
	rec->t = root;
	rec->p = ray_at((t_ray *)r, rec->t);
	oc = vec3_sub(&rec->p, &current_center);
	oc = unit_vector(&oc);
	set_face_normal(rec, r, &oc);
	sphere_get_uv(&oc, &rec->u, &rec->v);
	rec->albedo = s->albedo;
	rec->mat = s->mat;
	return (true);
}
