/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector5.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:17:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vector.h"
#include "random.h"
#include "settings.h"

t_vec3	random_unit_vector(void)
{
	t_vec3	p;

	p = random_in_unit_sphere();
	return (unit_vector(&p));
}

t_vec3	random_on_hemisphere(const t_vec3 *normal)
{
	t_vec3	on_unit_sphere;

	on_unit_sphere = random_unit_vector();
	if (dot(&on_unit_sphere, normal) > (real_t)0.0)
		return (on_unit_sphere);
	return (vec3_neg(&on_unit_sphere));
}

t_vec3	random_in_unit_disk(void)
{
	t_vec3	p;

	while (1)
	{
		p = vec3_create(
				random_real_interval((real_t)(-1.0), (real_t)1.0),
				random_real_interval((real_t)(-1.0), (real_t)1.0),
				(real_t)0.0);
		if (vec3_length_squared(&p) < (real_t)1.0)
			return (p);
	}
}

t_vec3	random_cosine_direction(const t_vec3 *normal)
{
	real_t	r1;
	real_t	r2;
	real_t	phi;
	real_t	cos_theta;
	real_t	sin_theta;
	t_vec3	w;
	t_vec3	a;
	t_vec3	v_cross;
	t_vec3	v;
	t_vec3	u;
	t_vec3	u_scaled;
	t_vec3	v_scaled;
	t_vec3	w_scaled;
	t_vec3	uv;

	r1 = random_real();
	r2 = random_real();
	phi = (real_t)(2.0 * PI) * r1;
	cos_theta = (real_t)sqrt((double)r2);
	sin_theta = (real_t)sqrt((double)((real_t)1.0 - r2));
	w = unit_vector(normal);
	if (fabsl((long double)w.x) > (long double)0.9)
		a = vec3_create((real_t)0.0, (real_t)1.0, (real_t)0.0);
	else
		a = vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);
	v_cross = cross(&w, &a);
	v = unit_vector(&v_cross);
	u = cross(&w, &v);
	u_scaled = vec3_mul_scalar(&u, (real_t)cos((double)phi) * sin_theta);
	v_scaled = vec3_mul_scalar(&v, (real_t)sin((double)phi) * sin_theta);
	w_scaled = vec3_mul_scalar(&w, cos_theta);
	uv = vec3_add(&u_scaled, &v_scaled);
	return (vec3_add(&uv, &w_scaled));
}
