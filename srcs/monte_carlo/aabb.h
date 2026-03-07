/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:43:36 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:58:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.h"
#include "point.h"
#include <stdbool.h>

/* Axis-Aligned Bounding Box */
typedef struct s_aabb
{
	t_interval x;
	t_interval y;
	t_interval z;
} t_aabb;

t_aabb aabb_add_vec3(const t_aabb *box, const t_vec3 *offset);
bool aabb_hit(const t_aabb *box, const t_ray *r, t_interval *ray_t);
int aabb_longest_axis(const t_aabb *box);
const t_interval *aabb_axis_interval(const t_aabb *box, int axis);
t_aabb aabb_merge(const t_aabb *box0, const t_aabb *box1);
t_aabb aabb_from_points(const t_point3 *a, const t_point3 *b);
t_aabb aabb_from_intervals(const t_interval *x, const t_interval *y, const t_interval *z);
t_aabb aabb_empty(void);


#endif