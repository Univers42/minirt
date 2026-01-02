/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 19:37:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 00:09:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
#define HITTABLE_H

#include "types.h"
#include "vector.h"
#include "ray.h"
# include <stdbool.h>
# include "common.h"

/* Hit record: store intersection point, normal and t. */
typedef struct s_hit_record
{
	t_vec3	p;
	t_vec3	normal;
	real_t	t;
	bool	front_face;
}	t_hit_record;

/* set_face_normal: outward_normal is assumed unit length. */
static inline void set_face_normal(t_hit_record *hit, const t_ray *r, const t_vec3 *outward_normal)
{
	bool front = (dot(&r->dir, outward_normal) < 0.0);
	hit->front_face = front;
	if (front)
		hit->normal = *outward_normal;
	else
		hit->normal = vec3_neg(outward_normal);
}

#endif