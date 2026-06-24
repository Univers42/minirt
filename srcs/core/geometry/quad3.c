/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quad.h"

bool	quad_is_interior(real_t alpha, real_t beta, t_hit_record *rec)
{
	if (!contains((real_t)0.0, (real_t)1.0, alpha)
		|| !contains((real_t)0.0, (real_t)1.0, beta))
		return (false);
	rec->u = alpha;
	rec->v = beta;
	return (true);
}

static bool	quad_barycentric(const t_quad *quad, const t_vec3 *p, real_t ab[2])
{
	t_vec3	phv;
	t_vec3	cross1;
	t_vec3	cross2;
	real_t	wlen;

	wlen = vec3_length_squared(&quad->w);
	if (wlen <= (real_t)0.0)
		return (false);
	phv = vec3_sub(p, &quad->q);
	cross1 = cross(&phv, &quad->v);
	ab[0] = (real_t)dot(&quad->w, &cross1) / wlen;
	cross2 = cross(&quad->u, &phv);
	ab[1] = (real_t)dot(&quad->w, &cross2) / wlen;
	return (true);
}

bool	quad_hit(const t_quad *quad, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	real_t	denom;
	real_t	t;
	t_vec3	p;
	real_t	ab[2];

	if (!quad || !r || !rec)
		return (false);
	denom = (real_t)dot(&quad->normal, &r->dir);
	if (fabsl((long double)denom) < (long double)1e-8)
		return (false);
	t = (quad->d - (real_t)dot(&quad->normal, &r->orig)) / denom;
	if (!contains(rayt.min, rayt.max, t))
		return (false);
	p = ray_at((t_ray *)r, t);
	if (!quad_barycentric(quad, &p, ab))
		return (false);
	if (!quad_is_interior(ab[0], ab[1], rec))
		return (false);
	rec->t = t;
	rec->p = p;
	rec->mat = quad->mat;
	set_face_normal(rec, r, &quad->normal);
	return (true);
}
