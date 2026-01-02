/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 17:49:10 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 18:03:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <math.h>
#include "Vector.h"
#include "ray.h"
#include "types.h"
#include "settings.h"

/* Return the smallest positive t where the ray hits the sphere, or -1.0 if no hit. */
static inline double hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r)
{
	/* oc = center - origin */
	t_vec3 oc = vec3_sub(center, &r->orig);
	double a = dot(&r->dir, &r->dir);
	double b = -2.0 * dot(&r->dir, &oc);
	double c = dot(&oc, &oc) - ((double)radius * (double)radius);
	double discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0)
		return -1.0;
	/* return nearest root */
	return (-b - sqrt(discriminant)) / (2.0 * a);
}

#endif