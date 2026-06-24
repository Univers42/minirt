/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quartic.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUARTIC_H
# define QUARTIC_H

# include "types.h"

/* Real-root solvers used by equation-based primitives (e.g. the torus).
   Coefficients are passed as the monic-normalisable forms below; each
   routine writes the distinct real roots into out[] and returns how many
   were found (0..n).  These are pure helpers — no allocation, no globals. */

/* Coefficients of a*x^3 + b*x^2 + c*x + d = 0 (a != 0). */
typedef struct s_cubic
{
	real_t	a;
	real_t	b;
	real_t	c;
	real_t	d;
}	t_cubic;

/* a*x^3 + b*x^2 + c*x + d = 0 (a != 0).  Returns 1..3 real roots. */
int		solve_cubic(const t_cubic *cub, real_t out[3]);

/* coef[] = {c4, c3, c2, c1, c0} of c4*x^4+...+c0 = 0, c4 != 0.
   Returns 0..4 real roots in out[] (unordered). */
int		solve_quartic(const real_t coef[5], real_t out[4]);

#endif
