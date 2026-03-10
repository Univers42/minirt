/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:43:36 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 23:33:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "aabb.h"

t_aabb	aabb_empty(void)
{
	return ((t_aabb){
		.x = interval(INFINITY, -INFINITY),
		.y = interval(INFINITY, -INFINITY),
		.z = interval(INFINITY, -INFINITY)});
}

t_aabb	aabb_from_intervals(const t_interval *x, const t_interval *y,
		const t_interval *z)
{
	return ((t_aabb){.x = *x, .y = *y, .z = *z});
}

t_aabb	aabb_from_points(const t_point3 *a, const t_point3 *b)
{
	t_aabb	box;

	if (a->x <= b->x)
		box.x = interval(a->x, b->x);
	else
		box.x = interval(b->x, a->x);
	if (a->y <= b->y)
		box.y = interval(a->y, b->y);
	else
		box.y = interval(b->y, a->y);
	if (a->z <= b->z)
		box.z = interval(a->z, b->z);
	else
		box.z = interval(b->z, a->z);
	aabb_pad(&box);
	return (box);
}

t_aabb	aabb_merge(const t_aabb *box0, const t_aabb *box1)
{
	return ((t_aabb){
		.x = interval_merge(&box0->x, &box1->x),
		.y = interval_merge(&box0->y, &box1->y),
		.z = interval_merge(&box0->z, &box1->z)});
}

const t_interval	*aabb_axis_interval(const t_aabb *box, int axis)
{
	if (axis == 1)
		return (&box->y);
	if (axis == 2)
		return (&box->z);
	return (&box->x);
}
