/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:05:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include <stddef.h>

static void	cylinder_compute_bbox(t_cylinder *cyl, const t_point3 *base)
{
	t_vec3		scaled_axis;
	t_point3	top;
	real_t		e[3];
	t_point3	low;
	t_point3	high;

	scaled_axis = vec3_mul_scalar(&cyl->axis, cyl->height);
	top = vec3_add(base, &scaled_axis);
	e[0] = cyl->radius * (real_t)sqrt(1.0
			- (double)(cyl->axis.x * cyl->axis.x));
	e[1] = cyl->radius * (real_t)sqrt(1.0
			- (double)(cyl->axis.y * cyl->axis.y));
	e[2] = cyl->radius * (real_t)sqrt(1.0
			- (double)(cyl->axis.z * cyl->axis.z));
	low = point3_create(fmin(base->x, top.x) - e[0],
			fmin(base->y, top.y) - e[1], fmin(base->z, top.z) - e[2]);
	high = point3_create(fmax(base->x, top.x) + e[0],
			fmax(base->y, top.y) + e[1], fmax(base->z, top.z) + e[2]);
	cyl->bbox = aabb_from_points(&low, &high);
}

t_cylinder	cylinder_create(const t_point3 *base, const t_vec3 *axis,
		const t_shape_dims *dims, t_material *mat)
{
	t_cylinder	cyl;

	cyl.base = *base;
	cyl.axis = unit_vector(axis);
	cyl.radius = dims->size;
	if (dims->size <= 0)
		cyl.radius = (real_t)0.1;
	cyl.height = dims->height;
	if (dims->height <= 0)
		cyl.height = (real_t)1.0;
	cyl.mat = mat;
	cylinder_compute_bbox(&cyl, base);
	return (cyl);
}

t_cylinder	cylinder_create_y(const t_point3 *base, real_t radius,
		real_t height, t_material *mat)
{
	t_vec3			y_axis;
	t_shape_dims	dims;

	y_axis = vec3_create((real_t)0.0, (real_t)1.0, (real_t)0.0);
	dims.size = radius;
	dims.height = height;
	return (cylinder_create(base, &y_axis, &dims, mat));
}

static __thread const t_cylinder	*g_current_cylinder = NULL;

void	set_current_cylinder(const void *obj)
{
	g_current_cylinder = (const t_cylinder *)obj;
}

bool	cylinder_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_cylinder)
		return (false);
	return (cylinder_hit(g_current_cylinder, r, rayt, rec));
}
