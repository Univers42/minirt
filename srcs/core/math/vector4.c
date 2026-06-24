/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector4.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:17:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vector.h"
#include "random.h"

t_vec3	vec3_reflect(const t_vec3 *v, const t_vec3 *n)
{
	real_t	d;
	t_vec3	scaled;

	d = dot(v, n);
	scaled = vec3_mul_scalar(n, (real_t)2.0 * d);
	return (vec3_sub(v, &scaled));
}

t_vec3	vec3_refract(const t_vec3 *uv, const t_vec3 *n, real_t etai_over_etat)
{
	t_vec3	neg_uv;
	real_t	cos_theta;
	t_vec3	n_scaled;
	t_vec3	r_out_perp_sum;
	t_vec3	r_out_perp;
	real_t	perp_len_sq;
	real_t	parallel_factor;
	t_vec3	r_out_parallel;

	neg_uv = vec3_neg(uv);
	cos_theta = dot(&neg_uv, n);
	if (cos_theta > (real_t)1.0)
		cos_theta = (real_t)1.0;
	n_scaled = vec3_mul_scalar(n, cos_theta);
	r_out_perp_sum = vec3_add(uv, &n_scaled);
	r_out_perp = vec3_mul_scalar(&r_out_perp_sum, etai_over_etat);
	perp_len_sq = vec3_length_squared(&r_out_perp);
	parallel_factor = (real_t)-sqrt(
			(double)fabsl((long double)((real_t)1.0 - perp_len_sq)));
	r_out_parallel = vec3_mul_scalar(n, parallel_factor);
	return (vec3_add(&r_out_perp, &r_out_parallel));
}

t_vec3	vec3_random(void)
{
	return (vec3_create(random_real(), random_real(), random_real()));
}

t_vec3	vec3_random_interval(real_t min, real_t max)
{
	return (vec3_create(
			random_real_interval(min, max),
			random_real_interval(min, max),
			random_real_interval(min, max)));
}

t_vec3	random_in_unit_sphere(void)
{
	t_vec3	p;

	while (1)
	{
		p = vec3_random_interval((real_t)(-1.0), (real_t)1.0);
		if (vec3_length_squared(&p) < (real_t)1.0)
			return (p);
	}
}
