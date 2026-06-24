/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 21:00:45 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:14:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERVAL_H
# define INTERVAL_H

# include "types.h"
# include <stdbool.h>
# include <math.h>

/* Interval type */
typedef struct s_interval
{
	real_t	min;
	real_t	max;
	bool	sense;
}	t_interval;

/* interval.c */
t_interval	interval(real_t min, real_t max);
t_interval	interval_merge(const t_interval *a, const t_interval *b);
bool		contains(real_t min, real_t max, real_t x);
real_t		clamp(real_t x, real_t min, real_t max);
bool		surrounds(real_t min, real_t max, real_t x);

/* interval2.c */
real_t		interval_clamp(const t_interval *it, real_t x);
real_t		interval_size(const t_interval *it);
bool		interval_contains(const t_interval *it, real_t x);
bool		interval_surrounds(const t_interval *it, real_t x);
t_interval	interval_expand(real_t min, real_t max, real_t delta);

/* interval3.c */
t_interval	interval_add(const t_interval *ival, real_t displacement);

#endif