/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:17:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vector.h"

t_vec3	vec3_create(real_t x, real_t y, real_t z)
{
	return ((t_vec3){.x = x, .y = y, .z = z});
}

t_vec3	vec3_zero(void)
{
	return (vec3_create((real_t)0.0, (real_t)0.0, (real_t)0.0));
}

t_vec3	vec3_neg(const t_vec3 *v)
{
	return (vec3_create(-v->x, -v->y, -v->z));
}

t_vec3	vec3_add(const t_vec3 *a, const t_vec3 *b)
{
	return (vec3_create(a->x + b->x, a->y + b->y, a->z + b->z));
}

t_vec3	vec3_sub(const t_vec3 *a, const t_vec3 *b)
{
	return (vec3_create(a->x - b->x, a->y - b->y, a->z - b->z));
}
