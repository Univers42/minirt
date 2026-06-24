/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disk.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "disk.h"
#include "interval.h"
#include <math.h>

/* AABB of a disk: center +/- radius on every axis (a conservative box). */
static void	disk_init_bbox(t_disk *d)
{
	t_point3	low;
	t_point3	high;

	low = point3_create(d->center.x - d->radius, d->center.y - d->radius,
			d->center.z - d->radius);
	high = point3_create(d->center.x + d->radius, d->center.y + d->radius,
			d->center.z + d->radius);
	d->bbox = aabb_from_points(&low, &high);
}

t_disk	disk_create(const t_point3 *center, const t_vec3 *normal,
			real_t radius, t_material *mat)
{
	t_disk	d;

	d.center = *center;
	d.normal = unit_vector(normal);
	if (radius > (real_t)0.0)
		d.radius = radius;
	else
		d.radius = (real_t)0.0;
	d.mat = mat;
	disk_init_bbox(&d);
	return (d);
}

bool	disk_hit(const t_disk *disk, const t_ray *r, t_interval rayt,
			t_hit_record *rec)
{
	real_t	denom;
	real_t	t;
	t_vec3	rel;
	t_vec3	p;

	if (!disk || !r || !rec)
		return (false);
	denom = dot(&disk->normal, &r->dir);
	if (fabs(denom) < (real_t)1e-8)
		return (false);
	rel = vec3_sub(&disk->center, &r->orig);
	t = dot(&disk->normal, &rel) / denom;
	if (!contains(rayt.min, rayt.max, t))
		return (false);
	p = ray_at((t_ray *)r, t);
	rel = vec3_sub(&p, &disk->center);
	if (vec3_length_squared(&rel) > disk->radius * disk->radius)
		return (false);
	rec->t = t;
	rec->p = p;
	rec->mat = disk->mat;
	set_face_normal(rec, r, &disk->normal);
	return (true);
}
