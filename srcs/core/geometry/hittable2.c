/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:08:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 15:06:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable.h"
#include "settings.h"
#include <stdlib.h>
#include <math.h>

/* rotate_y wrapper */
static __thread const t_rotate_y_wrap	*g_current_rotate = NULL;

void	set_current_rotate(const void *obj)
{
	g_current_rotate = (const t_rotate_y_wrap *)obj;
}

bool	rotate_y_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_rotate_y_wrap	*rot;
	t_vec3					o;
	t_vec3					d;
	t_ray					rotated_r;

	rot = g_current_rotate;
	if (!rot)
		return (false);
	o = rotate_y_vec(&r->orig, -rot->sin_theta, rot->cos_theta);
	d = rotate_y_vec(&r->dir, -rot->sin_theta, rot->cos_theta);
	rotated_r = ray_create(o, d, r->tm);
	if (!rot->child.set_current || !rot->child.hit_noobj)
		return (false);
	rot->child.set_current(rot->child.object);
	if (!rot->child.hit_noobj(&rotated_r, rayt, rec))
		return (false);
	rec->p = rotate_y_vec(&rec->p, rot->sin_theta, rot->cos_theta);
	rec->normal = rotate_y_vec(&rec->normal, rot->sin_theta, rot->cos_theta);
	return (true);
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
				corner = vec3_create(
						ijk[0] ? cb->x.max : cb->x.min,
						ijk[1] ? cb->y.max : cb->y.min,
						ijk[2] ? cb->z.max : cb->z.min);
				rotated = rotate_y_vec(&corner,
						rot->sin_theta, rot->cos_theta);
				if (rotated.x < minp.x)
					minp.x = rotated.x;
				if (rotated.y < minp.y)
					minp.y = rotated.y;
				if (rotated.z < minp.z)
					minp.z = rotated.z;
				if (rotated.x > maxp.x)
					maxp.x = rotated.x;
				if (rotated.y > maxp.y)
					maxp.y = rotated.y;
				if (rotated.z > maxp.z)
					maxp.z = rotated.z;
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
