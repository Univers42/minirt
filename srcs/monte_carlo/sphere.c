/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:21:35 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sphere.h"

t_vec3	sphere_center_at(const t_sphere *s, real_t time)
{
	t_vec3	scaled;

	scaled = vec3_mul_scalar(&s->center.center_velocity, time);
	return (vec3_add(&s->center.center1, &scaled));
}

void	sphere_get_uv(const t_vec3 *p, real_t *u, real_t *v)
{
	real_t	theta;
	real_t	phi;

	theta = (real_t)acos((double)(-p->y));
	phi = (real_t)atan2((double)(-p->z), (double)(p->x)) + (real_t)PI;
	*u = phi / ((real_t)2.0 * (real_t)PI);
	*v = theta / (real_t)PI;
}

static void	sphere_init_bbox_stationary(t_sphere *s, const t_point3 *center)
{
	t_point3	center_low;
	t_point3	center_high;

	center_low = point3_create(center->x - s->radius,
			center->y - s->radius, center->z - s->radius);
	center_high = point3_create(center->x + s->radius,
			center->y + s->radius, center->z + s->radius);
	s->bbox = aabb_from_points(&center_low, &center_high);
}

t_sphere	create_sphere(const t_point3 *center, real_t radius,
		t_vec3 albedo, t_material *mat)
{
	t_sphere	s;

	s.center.center1 = vec3_create(center->x, center->y, center->z);
	s.center.center_velocity = vec3_zero();
	if (radius > 0.0)
		s.radius = radius;
	else
		s.radius = 0.0;
	s.albedo = albedo;
	s.mat = mat;
	sphere_init_bbox_stationary(&s, center);
	return (s);
}

t_sphere	create_sphere_moving(const t_point3 *center1,
		const t_point3 *center2, real_t radius, t_vec3 albedo,
		t_material *mat)
{
	t_sphere	s;
	t_vec3		c2;
	t_aabb		box0;
	t_aabb		box1;

	s.center.center1 = vec3_create(center1->x, center1->y, center1->z);
	c2 = vec3_create(center2->x, center2->y, center2->z);
	s.center.center_velocity = vec3_sub(&c2, &s.center.center1);
	if (radius > 0.0)
		s.radius = radius;
	else
		s.radius = 0.0;
	s.albedo = albedo;
	s.mat = mat;
	box0 = aabb_from_points(
			&(t_point3){center1->x - s.radius, center1->y - s.radius,
			center1->z - s.radius},
			&(t_point3){center1->x + s.radius, center1->y + s.radius,
			center1->z + s.radius});
	box1 = aabb_from_points(
			&(t_point3){center2->x - s.radius, center2->y - s.radius,
			center2->z - s.radius},
			&(t_point3){center2->x + s.radius, center2->y + s.radius,
			center2->z + s.radius});
	s.bbox = aabb_merge(&box0, &box1);
	return (s);
}
