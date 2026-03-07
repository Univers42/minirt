/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 17:09:23 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:13:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ray.h"

t_ray	ray_create(t_vec3 origin, t_vec3 direction, real_t time)
{
	t_ray	r;

	r.orig = origin;
	r.dir = direction;
	r.tm = time;
	return (r);
}

t_ray	ray_create_default(t_vec3 origin, t_vec3 direction)
{
	return (ray_create(origin, direction, (real_t)0.0));
}

const t_vec3	*ray_origin(t_ray *ray)
{
	return (&ray->orig);
}

const t_vec3	*ray_direction(t_ray *ray)
{
	return (&ray->dir);
}

real_t	ray_time(const t_ray *ray)
{
	return (ray->tm);
}
