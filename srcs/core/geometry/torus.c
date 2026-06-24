/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   torus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "torus.h"
#include "interval.h"
#include "quartic.h"
#include <math.h>

/* torus2.c */
void	torus_local(const t_torus *to, const t_ray *r, t_vec3 *o, t_vec3 *d);
void	torus_coeffs(const t_torus *to, const t_vec3 *o, const t_vec3 *d,
			real_t c[5]);
void	torus_set_record(const t_torus *to, const t_ray *r, real_t t,
			t_hit_record *rec);
bool	torus_root_valid(const t_torus *to, const t_vec3 *o,
			const t_vec3 *d, real_t t);

/* Pick any unit vector orthogonal to `axis` to seed the local frame. */
static t_vec3	torus_pick_ortho(const t_vec3 *axis)
{
	t_vec3	helper;

	if (fabs(axis->x) > 0.9)
		helper = vec3_create(0.0, 1.0, 0.0);
	else
		helper = vec3_create(1.0, 0.0, 0.0);
	helper = cross(axis, &helper);
	return (unit_vector(&helper));
}

t_torus	torus_create(const t_point3 *center, const t_vec3 *axis,
		const t_torus_radii *rad, t_material *mat)
{
	t_torus		to;
	real_t		reach;
	t_point3	low;
	t_point3	high;

	to.center = *center;
	to.axis = unit_vector(axis);
	to.u = torus_pick_ortho(&to.axis);
	to.v = cross(&to.axis, &to.u);
	to.major = rad->major;
	to.minor = rad->minor;
	to.mat = mat;
	reach = rad->major + rad->minor;
	low = point3_create(center->x - reach, center->y - reach,
			center->z - reach);
	high = point3_create(center->x + reach, center->y + reach,
			center->z + reach);
	to.bbox = aabb_from_points(&low, &high);
	return (to);
}

/* Smallest root inside (rayt) that truly lies on the surface.  Each quartic
   root is re-checked against the implicit equation (torus_root_valid) to
   discard the spurious near-zero-imaginary roots the solver can emit for
   grazing rays.  Writes the hit distance to *out and returns whether ANY
   valid root was found — a found-flag, never a sentinel, so an unbounded
   rayt.max can no longer be mistaken for an at-infinity hit. */
static bool	torus_nearest(const t_torus *to, const t_vec3 *o,
		const t_vec3 *d, t_root_set *rs)
{
	bool	found;
	int		i;

	found = false;
	i = -1;
	while (++i < rs->n)
	{
		if (contains(rs->rayt.min, rs->rayt.max, rs->roots[i])
			&& (!found || rs->roots[i] < rs->best)
			&& torus_root_valid(to, o, d, rs->roots[i]))
		{
			rs->best = rs->roots[i];
			found = true;
		}
	}
	return (found);
}

bool	torus_hit(const t_torus *to, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_vec3		o;
	t_vec3		d;
	real_t		coef[5];
	t_root_set	rs;

	if (!to || !r || !rec)
		return (false);
	torus_local(to, r, &o, &d);
	torus_coeffs(to, &o, &d, coef);
	rs.rayt = rayt;
	rs.n = solve_quartic(coef, rs.roots);
	if (!torus_nearest(to, &o, &d, &rs))
		return (false);
	torus_set_record(to, r, rs.best, rec);
	return (true);
}
