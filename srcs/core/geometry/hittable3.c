/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable.h"
#include "settings.h"
#include <stdlib.h>
#include <math.h>

static void	expand_bounds(t_point3 *minp, t_point3 *maxp, const t_vec3 *p)
{
	if (p->x < minp->x)
		minp->x = p->x;
	if (p->y < minp->y)
		minp->y = p->y;
	if (p->z < minp->z)
		minp->z = p->z;
	if (p->x > maxp->x)
		maxp->x = p->x;
	if (p->y > maxp->y)
		maxp->y = p->y;
	if (p->z > maxp->z)
		maxp->z = p->z;
}

static t_vec3	pick_corner(const t_aabb *cb, const int *ijk)
{
	real_t	cx;
	real_t	cy;
	real_t	cz;

	cx = cb->x.min;
	cy = cb->y.min;
	cz = cb->z.min;
	if (ijk[0])
		cx = cb->x.max;
	if (ijk[1])
		cy = cb->y.max;
	if (ijk[2])
		cz = cb->z.max;
	return (vec3_create(cx, cy, cz));
}

static void	rotate_y_compute_bbox(t_rotate_y_wrap *rot, const t_aabb *cb)
{
	t_point3	minp;
	t_point3	maxp;
	int			ijk[3];
	t_vec3		corner;
	t_vec3		rotated;

	minp = point3_create(INFINITY, INFINITY, INFINITY);
	maxp = point3_create(-INFINITY, -INFINITY, -INFINITY);
	ijk[0] = -1;
	while (++ijk[0] < 2)
	{
		ijk[1] = -1;
		while (++ijk[1] < 2)
		{
			ijk[2] = -1;
			while (++ijk[2] < 2)
			{
				corner = pick_corner(cb, ijk);
				rotated = rotate_y_vec(&corner,
						rot->sin_theta, rot->cos_theta);
				expand_bounds(&minp, &maxp, &rotated);
			}
		}
	}
	rot->bbox = aabb_from_points(&minp, &maxp);
}

t_rotate_y_wrap	*rotate_y_create(const t_hittable_wrapper *child,
		real_t angle_deg)
{
	t_rotate_y_wrap	*rot;
	real_t			radians;
	t_aabb			cb;

	if (!child)
		return (NULL);
	rot = (t_rotate_y_wrap *)malloc(sizeof(t_rotate_y_wrap));
	if (!rot)
		return (NULL);
	radians = degrees_to_radians(angle_deg);
	rot->sin_theta = (real_t)sin((double)radians);
	rot->cos_theta = (real_t)cos((double)radians);
	rot->child = *child;
	cb = child->bbox;
	rotate_y_compute_bbox(rot, &cb);
	return (rot);
}
