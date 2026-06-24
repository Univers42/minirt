/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   torus2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "torus.h"
#include <math.h>

/* Express the ray in the torus local frame (u, axis, v): the X axis is u,
   the hole runs along Y = axis, and Z is v.  Rotations preserve length so
   |d|^2 == |dir|^2 and o.d == oc.dir afterwards. */
void	torus_local(const t_torus *to, const t_ray *r, t_vec3 *o, t_vec3 *d)
{
	t_vec3	oc;

	oc = vec3_sub(&r->orig, &to->center);
	*o = vec3_create(dot(&oc, &to->u), dot(&oc, &to->axis),
			dot(&oc, &to->v));
	*d = vec3_create(dot(&r->dir, &to->u), dot(&r->dir, &to->axis),
			dot(&r->dir, &to->v));
}

/* Quartic c4*t^4+..+c0 for (|P|^2+R^2-r^2)^2 = 4R^2(X^2+Z^2), P = o + t*d.
   g=d.d, f=o.d, oo=o.o, e=oo+R^2-r^2; the X^2+Z^2 term drops the Y axis. */
void	torus_coeffs(const t_torus *to, const t_vec3 *o, const t_vec3 *d,
		real_t c[5])
{
	real_t	g;
	real_t	f;
	real_t	e;
	real_t	rr4;
	real_t	k[3];

	g = dot(d, d);
	f = dot(o, d);
	e = dot(o, o) + to->major * to->major - to->minor * to->minor;
	rr4 = 4.0 * to->major * to->major;
	k[0] = g - d->y * d->y;
	k[1] = f - o->y * d->y;
	k[2] = dot(o, o) - o->y * o->y;
	c[0] = g * g;
	c[1] = 4.0 * f * g;
	c[2] = 2.0 * g * e + 4.0 * f * f - rr4 * k[0];
	c[3] = 4.0 * f * e - 2.0 * rr4 * k[1];
	c[4] = e * e - rr4 * k[2];
}

/* Outward normal at a local point: project onto the tube circle.  The
   nearest point of the major circle (radius R, in the XZ plane) is
   (R*X/s, 0, R*Z/s); the surface normal is P minus that, mapped back to
   world via the (u, axis, v) basis. */
static t_vec3	torus_normal(const t_torus *to, const t_vec3 *lp)
{
	real_t	s;
	t_vec3	n;
	t_vec3	wn;
	t_vec3	tmp;

	s = sqrt(lp->x * lp->x + lp->z * lp->z);
	if (s < 1e-12)
		s = 1e-12;
	n = vec3_create(lp->x - to->major * lp->x / s, lp->y,
			lp->z - to->major * lp->z / s);
	wn = vec3_mul_scalar(&to->u, n.x);
	tmp = vec3_mul_scalar(&to->axis, n.y);
	wn = vec3_add(&wn, &tmp);
	tmp = vec3_mul_scalar(&to->v, n.z);
	wn = vec3_add(&wn, &tmp);
	return (unit_vector(&wn));
}

void	torus_set_record(const t_torus *to, const t_ray *r, real_t t,
		t_hit_record *rec)
{
	t_vec3	oc;
	t_vec3	lp;
	t_vec3	n;

	rec->t = t;
	rec->p = ray_at((t_ray *)r, t);
	oc = vec3_sub(&rec->p, &to->center);
	lp = vec3_create(dot(&oc, &to->u), dot(&oc, &to->axis),
			dot(&oc, &to->v));
	n = torus_normal(to, &lp);
	rec->u = 0;
	rec->v = 0;
	rec->mat = to->mat;
	rec->albedo = vec3_create(1.0, 1.0, 1.0);
	set_face_normal(rec, r, &n);
}

/* True when P = o + t*d actually satisfies the torus implicit to a small
   relative tolerance.  The quartic solver can return spurious "real" roots
   (near-zero imaginary part) for grazing rays; substituting back into
   (|P|^2+R^2-r^2)^2 - 4R^2(X^2+Z^2) rejects them so the bbox is not painted. */
bool	torus_root_valid(const t_torus *to, const t_vec3 *o,
		const t_vec3 *d, real_t t)
{
	t_vec3	p;
	t_vec3	td;
	real_t	s;
	real_t	lhs;
	real_t	scale;

	td = vec3_mul_scalar(d, t);
	p = vec3_add(o, &td);
	s = dot(&p, &p) + to->major * to->major - to->minor * to->minor;
	lhs = s * s - 4.0 * to->major * to->major * (p.x * p.x + p.z * p.z);
	scale = to->major * to->major * to->major * to->major + 1.0;
	return (fabs(lhs) < 1e-6 * scale);
}
