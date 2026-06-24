/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constant_medium.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:02:40 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:04:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "constant_medium.h"

/* Set current medium for hit_noobj callback */
static __thread const t_constant_medium	*g_current_medium = NULL;

void	set_current_medium(const void *obj)
{
	g_current_medium = (const t_constant_medium *)obj;
}

/* Find the [entry, exit] t-range the ray spends inside the boundary,
   clamped to rayt.  Returns false if the span is empty or missed. */
static bool	medium_span(const t_constant_medium *medium, const t_ray *r,
		t_interval rayt, real_t span[2])
{
	t_hit_record	rec1;
	t_hit_record	rec2;

	if (!medium->boundary.set_current || !medium->boundary.hit_noobj)
		return (false);
	medium->boundary.set_current(medium->boundary.object);
	if (!medium->boundary.hit_noobj(r, interval(-INFINITY, INFINITY), &rec1))
		return (false);
	medium->boundary.set_current(medium->boundary.object);
	if (!medium->boundary.hit_noobj(r, interval(rec1.t + (real_t)0.0001,
				INFINITY), &rec2))
		return (false);
	if (rec1.t < rayt.min)
		rec1.t = rayt.min;
	if (rec2.t > rayt.max)
		rec2.t = rayt.max;
	if (rec1.t >= rec2.t)
		return (false);
	if (rec1.t < (real_t)0.0)
		rec1.t = (real_t)0.0;
	span[0] = rec1.t;
	span[1] = rec2.t;
	return (true);
}

/* Fill the scattering record for a hit at parameter t inside the medium. */
static void	medium_record(t_hit_record *rec, const t_constant_medium *medium,
		const t_ray *r, real_t t)
{
	rec->t = t;
	rec->p = ray_at((t_ray *)r, rec->t);
	rec->normal = vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);
	rec->front_face = true;
	rec->mat = medium->phase_function;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	rec->u = (real_t)0.0;
	rec->v = (real_t)0.0;
}

/* Hit function for constant medium */
bool	constant_medium_hit(const t_constant_medium *medium, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	real_t	span[2];
	real_t	ray_length;
	real_t	random_val;
	real_t	hit_distance;

	if (!medium || !rec)
		return (false);
	if (!medium_span(medium, r, rayt, span))
		return (false);
	ray_length = vec3_length(&r->dir);
	random_val = random_double();
	if (random_val <= (real_t)0.0)
		random_val = (real_t)1e-10;
	hit_distance = medium->neg_inv_density * (real_t)log((double)random_val);
	if (hit_distance > (span[1] - span[0]) * ray_length)
		return (false);
	medium_record(rec, medium, r, span[0] + hit_distance / ray_length);
	return (true);
}

/* Wrapper for hit_noobj callback */
bool	constant_medium_hit_noobj(const t_ray *r, t_interval rayt,
		t_hit_record *rec)
{
	if (!g_current_medium)
		return (false);
	return (constant_medium_hit(g_current_medium, r, rayt, rec));
}
