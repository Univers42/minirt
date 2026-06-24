/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 13:16:23 by dlesieur         ###   ########.fr       */
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

static t_aabb	sphere_box_at(const t_point3 *c, real_t radius)
{
	t_point3	low;
	t_point3	high;

	low = point3_create(c->x - radius, c->y - radius, c->z - radius);
	high = point3_create(c->x + radius, c->y + radius, c->z + radius);
	return (aabb_from_points(&low, &high));
}

t_sphere	create_sphere_moving(const t_sphere_motion *path, real_t radius,
		t_vec3 albedo, t_material *mat)
{
	t_sphere	s;
	t_vec3		c2;
	t_aabb		box0;
	t_aabb		box1;

	s.center.center1 = vec3_create(path->start.x, path->start.y,
			path->start.z);
	c2 = vec3_create(path->end.x, path->end.y, path->end.z);
	s.center.center_velocity = vec3_sub(&c2, &s.center.center1);
	if (radius > 0.0)
		s.radius = radius;
	else
		s.radius = 0.0;
	s.albedo = albedo;
	s.mat = mat;
	box0 = sphere_box_at(&path->start, s.radius);
	box1 = sphere_box_at(&path->end, s.radius);
	s.bbox = aabb_merge(&box0, &box1);
	return (s);
}
