/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 17:49:10 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 17:57:05 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include "Vector.h"
#include "ray.h"
#include "types.h"
#include "settings.h"

static inline bool hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r)
{
	t_vec3 oc = vec3_sub(center, &r->orig);
	double a = dot(&r->dir, &r->dir);
	double b = -2.0 * dot(&r->dir, &oc);
	double c = dot(&oc, &oc) - ((double)radius * (double)radius);
	double discriminant = b * b - 4.0 * a * c;
	return (discriminant >= 0.0);
}

#endif