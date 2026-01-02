/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 17:49:10 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 18:36:28 by dlesieur         ###   ########.fr       */
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
/**
 * intersection code simplification
 */
static inline double hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r)
{
	t_vec3 oc;
	real_t a;
	real_t h;
	real_t c;
	double discriminant;

	oc = vec3_sub(center, &r->orig);				/* oc = center - origin */
	a = vec3_length_squared(&r->dir);				/* a = |dir|^2 */
	h = (real_t)dot(&r->dir, &oc);					/* h = dot(dir, oc) */
	c = vec3_length_squared(&oc) - radius * radius; /* c = |oc|^2 - r^2 */
	discriminant = (double)h * (double)h - (double)a * (double)c;

	if (discriminant < 0.0)
		return -1.0;
	{
		double root = sqrt(discriminant);
		return ((double)h - root) / (double)a;
	}
}

#endif