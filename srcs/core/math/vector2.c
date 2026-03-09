/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:17:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vector.h"

t_vec3	vec3_mul_elem(const t_vec3 *a, const t_vec3 *b)
{
	return (vec3_create(a->x * b->x, a->y * b->y, a->z * b->z));
}

t_vec3	vec3_mul_scalar(const t_vec3 *v, real_t t)
{
	return (vec3_create(v->x * t, v->y * t, v->z * t));
}

t_vec3	vec3_div_scalar(const t_vec3 *v, real_t t)
{
	return (vec3_mul_scalar(v, (real_t)1.0 / t));
}

real_t	dot(const t_vec3 *a, const t_vec3 *b)
{
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

t_vec3	cross(const t_vec3 *a, const t_vec3 *b)
{
	return (vec3_create(
			a->y * b->z - a->z * b->y,
			a->z * b->x - a->x * b->z,
			a->x * b->y - a->y * b->x));
}
