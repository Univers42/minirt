/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 21:00:45 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 01:04:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERVAL_H
#define INTERVAL_H

#include "types.h"
#include <stdbool.h>
#include <math.h>

/* Interval type */
typedef struct s_interval
{
	real_t min;
	real_t max;
	bool sense;
} t_interval;

/* Constructor: build an interval and return it */
static inline t_interval interval(real_t min, real_t max)
{
	t_interval between;
	between.sense = (min < max) ? true : false;
	between.min = min;
	between.max = max;
	return between;
}

/* Legacy helpers that operate on min/max pairs (kept to avoid changing callers) */
static inline bool contains(real_t min, real_t max, real_t x)
{
	return (min <= x && x <= max);
}

static inline real_t	clamp(real_t x, real_t min, real_t max)
{
	if (x < min)
		return (min);
	else if (x > max)
		return (max);
	else
		return (x);
}

static inline bool surrounds(real_t min, real_t max, real_t x)
{
	return (min < x && x < max);
}

/* Optional interval-based helpers */
static inline real_t interval_size(const t_interval *it)
{
	return (it->max - it->min);
}

static inline bool interval_contains(const t_interval *it, real_t x)
{
	return contains(it->min, it->max, x);
}

static inline bool interval_surrounds(const t_interval *it, real_t x)
{
	return surrounds(it->min, it->max, x);
}

#endif