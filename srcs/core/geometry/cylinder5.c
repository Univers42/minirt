/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder5.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"

static void	cyl_uv_axes(const t_cylinder *cyl, t_vec3 *u_axis, t_vec3 *v_axis)
{
	t_vec3	ref;

	if (fabsl((long double)cyl->axis.y) > (long double)0.9)
		ref = vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);
	else
		ref = vec3_create((real_t)0.0, (real_t)1.0, (real_t)0.0);
	*u_axis = cross(&cyl->axis, &ref);
	*u_axis = unit_vector(u_axis);
	*v_axis = cross(&cyl->axis, u_axis);
}

void	cylinder_get_uv(const t_cylinder *cyl, const t_vec3 *p,
		real_t *u, real_t *v)
{
	t_vec3	to_p;
	t_vec3	radial;
	t_vec3	u_axis;
	t_vec3	v_axis;
	real_t	theta;

	to_p = vec3_sub(p, &cyl->base);
	*v = dot(&to_p, &cyl->axis) / cyl->height;
	radial = vec3_mul_scalar(&cyl->axis, dot(&to_p, &cyl->axis));
	radial = vec3_sub(&to_p, &radial);
	cyl_uv_axes(cyl, &u_axis, &v_axis);
	theta = (real_t)atan2((double)dot(&radial, &v_axis),
			(double)dot(&radial, &u_axis));
	*u = (theta + (real_t)PI) / ((real_t)2.0 * (real_t)PI);
}
