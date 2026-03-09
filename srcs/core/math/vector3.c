/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:17:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vector.h"

real_t	vec3_length_squared(const t_vec3 *v)
{
	return (v->x * v->x + v->y * v->y + v->z * v->z);
}

real_t	vec3_length(const t_vec3 *v)
{
	return ((real_t)sqrt((double)vec3_length_squared(v)));
}

t_vec3	unit_vector(const t_vec3 *v)
{
	real_t	len;

	len = vec3_length(v);
	if (len < (real_t)1e-10)
		return (vec3_zero());
	return (vec3_div_scalar(v, len));
}

t_vec3	vec3_lerp(const t_vec3 *a, const t_vec3 *b, real_t t)
{
	t_vec3	one_minus_t;
	t_vec3	t_term;

	one_minus_t = vec3_mul_scalar(a, (real_t)1.0 - t);
	t_term = vec3_mul_scalar(b, t);
	return (vec3_add(&one_minus_t, &t_term));
}

bool	vec3_near_zero(const t_vec3 *v)
{
	const real_t	s = (real_t)1e-8;

	return ((fabsl((long double)v->x) < (long double)s)
		&& (fabsl((long double)v->y) < (long double)s)
		&& (fabsl((long double)v->z) < (long double)s));
}
