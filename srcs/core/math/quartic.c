/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quartic.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quartic.h"
#include "settings.h"
#include <math.h>

/* One real root of the depressed cubic t^3 + p*t + q = 0 when the
   discriminant says a single real root exists (Cardano). */
static real_t	cubic_one_real(real_t q, real_t disc)
{
	real_t	sq;
	real_t	u;
	real_t	v;

	sq = sqrt(disc);
	u = cbrt(-q * 0.5 + sq);
	v = cbrt(-q * 0.5 - sq);
	return (u + v);
}

/* Three real roots of the depressed cubic via the trigonometric method
   (used when the discriminant is negative -> casus irreducibilis). */
static int	cubic_three_real(real_t p, real_t q, real_t out[3])
{
	real_t	m;
	real_t	theta;
	int		i;

	m = 2.0 * sqrt(-p / 3.0);
	theta = acos(3.0 * q / (p * m)) / 3.0;
	i = -1;
	while (++i < 3)
		out[i] = m * cos(theta - 2.0 * PI * (real_t)i / 3.0);
	return (3);
}

/* Solve the depressed cubic t^3 + p*t + q = 0 (roots in t). */
static int	depressed_cubic(real_t p, real_t q, real_t out[3])
{
	real_t	disc;

	if (fabs(p) < 1e-14 && fabs(q) < 1e-14)
		return (out[0] = 0.0, 1);
	disc = q * q * 0.25 + p * p * p / 27.0;
	if (disc > 0.0)
	{
		out[0] = cubic_one_real(q, disc);
		return (1);
	}
	return (cubic_three_real(p, q, out));
}

/* a*x^3 + b*x^2 + c*x + d = 0.  Depress with x = t - b/(3a), then shift. */
int	solve_cubic(real_t a, real_t b, real_t c, real_t d, real_t out[3])
{
	real_t	p;
	real_t	q;
	real_t	shift;
	int		n;
	int		i;

	b /= a;
	c /= a;
	d /= a;
	p = c - b * b / 3.0;
	q = 2.0 * b * b * b / 27.0 - b * c / 3.0 + d;
	n = depressed_cubic(p, q, out);
	shift = b / 3.0;
	i = -1;
	while (++i < n)
		out[i] -= shift;
	return (n);
}
