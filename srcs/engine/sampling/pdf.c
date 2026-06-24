/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pdf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:32 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 15:06:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pdf.h"
#include <math.h>

void	build_onb_from_w(t_vec3 *uvw, const t_vec3 *n)
{
	t_vec3	a;
	t_vec3	cross_wa;

	uvw[2] = unit_vector(n);
	if (fabsl((long double)uvw[2].x) > (long double)0.9)
		a = vec3_create(0.0, 1.0, 0.0);
	else
		a = vec3_create(1.0, 0.0, 0.0);
	cross_wa = cross(&uvw[2], &a);
	uvw[1] = unit_vector(&cross_wa);
	uvw[0] = cross(&uvw[2], &uvw[1]);
}

t_vec3	onb_local(const t_vec3 *uvw, const t_vec3 *a)
{
	t_vec3	u_scaled;
	t_vec3	v_scaled;
	t_vec3	w_scaled;
	t_vec3	uv;

	u_scaled = vec3_mul_scalar(&uvw[0], a->x);
	v_scaled = vec3_mul_scalar(&uvw[1], a->y);
	w_scaled = vec3_mul_scalar(&uvw[2], a->z);
	uv = vec3_add(&u_scaled, &v_scaled);
	return (vec3_add(&uv, &w_scaled));
}

real_t	cosine_pdf_value(const t_pdf *pdf, const t_vec3 *direction)
{
	const t_cosine_pdf	*cpdf;
	t_vec3				unit_dir;
	real_t				cosine;

	cpdf = (const t_cosine_pdf *)pdf->data;
	if (!cpdf)
		return (0.0);
	unit_dir = unit_vector(direction);
	cosine = dot(&unit_dir, &cpdf->uvw[2]);
	if (cosine <= 0.0)
		return (0.0);
	return (cosine / (real_t)PI);
}

static t_vec3	cosine_local_dir(real_t r1, real_t r2)
{
	real_t	z;
	real_t	phi;

	z = (real_t)sqrt((double)r2);
	phi = 2.0 * PI * r1;
	return (vec3_create(
			(real_t)cos((double)phi) * (real_t)sqrt(1.0 - (double)r2),
			(real_t)sin((double)phi) * (real_t)sqrt(1.0 - (double)r2), z));
}

t_vec3	cosine_pdf_generate(const t_pdf *pdf)
{
	const t_cosine_pdf	*cpdf;
	t_vec3				local_dir;
	real_t				r1;

	cpdf = (const t_cosine_pdf *)pdf->data;
	if (!cpdf)
		return (vec3_create(0.0, 1.0, 0.0));
	r1 = random_real();
	local_dir = cosine_local_dir(r1, random_real());
	return (onb_local(cpdf->uvw, &local_dir));
}
