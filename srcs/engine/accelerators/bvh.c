/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:59:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:59:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh.h"

int	bvh_box_compare(const void *a, const void *b, int axis_index)
{
	const t_hittable_wrapper	*wa;
	const t_hittable_wrapper	*wb;
	real_t						a_min;
	real_t						b_min;

	wa = (const t_hittable_wrapper *)a;
	wb = (const t_hittable_wrapper *)b;
	a_min = aabb_axis_interval(&wa->bbox, axis_index)->min;
	b_min = aabb_axis_interval(&wb->bbox, axis_index)->min;
	if (a_min < b_min)
		return (-1);
	else if (a_min > b_min)
		return (1);
	return (0);
}

int	bvh_box_x_compare(const void *a, const void *b)
{
	return (bvh_box_compare(a, b, 0));
}

int	bvh_box_y_compare(const void *a, const void *b)
{
	return (bvh_box_compare(a, b, 1));
}

int	bvh_box_z_compare(const void *a, const void *b)
{
	return (bvh_box_compare(a, b, 2));
}

t_comparator_fn	bvh_axis_comparator(int axis)
{
	if (axis == 0)
		return (bvh_box_x_compare);
	if (axis == 1)
		return (bvh_box_y_compare);
	return (bvh_box_z_compare);
}
