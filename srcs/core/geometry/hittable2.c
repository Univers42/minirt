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
