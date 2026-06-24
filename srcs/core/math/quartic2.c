/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quartic2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quartic.h"
#include <math.h>

/* Add the real roots of y^2 + b*y + c = 0 to out[] starting at *n. */
static void	add_quadratic(real_t b, real_t c, real_t out[4], int *n)
{
	real_t	disc;
	real_t	sq;

	disc = b * b - 4.0 * c;
	if (disc < 0.0)
	{
		if (disc < -1e-9)
			return ;
		disc = 0.0;
	}
	sq = sqrt(disc);
	out[(*n)++] = (-b + sq) * 0.5;
	out[(*n)++] = (-b - sq) * 0.5;
}

/* Largest real root of the resolvent w^3 + 2p*w^2 + (p^2-4r)*w - q^2 = 0.
   Its constant term is -q^2 <= 0, so a non-negative real root always
   exists; the largest one gives the most stable quadratic split. */
static real_t	resolvent_w(real_t p, real_t q, real_t r)
{
	t_cubic	cub;
	real_t	roots[3];
	real_t	w;
	int		n;
	int		i;

	cub.a = 1.0;
	cub.b = 2.0 * p;
	cub.c = p * p - 4.0 * r;
	cub.d = -q * q;
	n = solve_cubic(&cub, roots);
	w = roots[0];
	i = 0;
	while (++i < n)
		if (roots[i] > w)
			w = roots[i];
	return (w);
}

/* Depressed quartic y^4 + p*y^2 + q*y + r = 0 via Ferrari.  Factor as
   (y^2 + R*y + s)(y^2 - R*y + t) with R = sqrt(w), w the resolvent root,
   s,t = ((p+w) -/+ q/R)/2.  Collect the real roots of both quadratics. */
static int	solve_depressed(real_t p, real_t q, real_t r, real_t out[4])
{
	real_t	w;
	real_t	rr;
	real_t	st[2];
	int		n;

	w = resolvent_w(p, q, r);
	if (w <= 1e-12)
		return (0);
	rr = sqrt(w);
	st[0] = ((p + w) - q / rr) * 0.5;
	st[1] = ((p + w) + q / rr) * 0.5;
	n = 0;
	add_quadratic(rr, st[0], out, &n);
	add_quadratic(-rr, st[1], out, &n);
	return (n);
}

/* coef[] = {c4,c3,c2,c1,c0}.  Normalise to monic, depress x = y - a/4,
   solve, then shift the roots back by a/4. */
int	solve_quartic(const real_t coef[5], real_t out[4])
{
	real_t	a[4];
	real_t	pqr[3];
	int		n;
	int		i;

	a[0] = coef[1] / coef[0];
	a[1] = coef[2] / coef[0];
	a[2] = coef[3] / coef[0];
	a[3] = coef[4] / coef[0];
	pqr[0] = a[1] - 3.0 * a[0] * a[0] / 8.0;
	pqr[1] = a[2] - a[0] * a[1] / 2.0 + a[0] * a[0] * a[0] / 8.0;
	pqr[2] = a[3] - a[0] * a[2] / 4.0 + a[0] * a[0] * a[1] / 16.0
		- 3.0 * a[0] * a[0] * a[0] * a[0] / 256.0;
	n = solve_depressed(pqr[0], pqr[1], pqr[2], out);
	i = -1;
	while (++i < n)
		out[i] -= a[0] / 4.0;
	return (n);
}
